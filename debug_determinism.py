#!/usr/bin/env python3
import os
import sys
import argparse
import subprocess
import re
import glob
import shutil
import struct
import time

# --- Configuration ---
WIIMIX_EXE = "./build/Binaries/wiimix-nogui"
SOURCE_ROOT = "./Source/Core"
RESULTS_DIR = "scan_results"
SUMMARY_FILE = os.path.join(RESULTS_DIR, "SUMMARY.txt")

# Colors
C_RED = "\033[91m"
C_GREEN = "\033[92m"
C_YELLOW = "\033[93m"
C_CYAN = "\033[96m"
C_MAGENTA = "\033[95m"
C_RESET = "\033[0m"
C_BOLD = "\033[1m"

# Subsystems
POISON_SUITE = [1024, 32768, 16384, 4]
POISON_NAMES = {0: "None", 1024: "DVD", 32768: "DSP", 16384: "AI", 4: "CoreTiming"}

DEPENDENCIES = {
    "after main dsp state": [
        "AudioInterface.cpp",
        "DSPEmulator.cpp",
        "DSPHLE.cpp",
        "DSPLLE.cpp",
    ],
    "DSP": ["AudioInterface.cpp", "DSPEmulator.cpp"],
    "AudioInterface": ["DSP.cpp", "AudioCommon.cpp"],
    "DVDInterface": ["DVDThread.cpp", "IOS.cpp", "Volume.cpp"],
    "CoreTiming": ["SystemTimers.cpp", "PowerPC.cpp"],
}

# Logger Handle (Per-File)
CURRENT_LOG_HANDLE = None


class TestResult:
    def __init__(
        self,
        category,
        name,
        poison,
        jitter,
        frames,
        status,
        code,
        reason,
        out="",
        err="",
        analysis=None,
    ):
        self.category = category
        self.name = name
        self.poison = poison
        self.jitter = jitter
        self.frames = frames
        self.status = status
        self.code = code
        self.reason = reason
        self.out = out
        self.err = err
        self.analysis = analysis


def ensure_dirs():
    if not os.path.exists(RESULTS_DIR):
        os.makedirs(RESULTS_DIR)


def open_log(filename):
    global CURRENT_LOG_HANDLE
    log_path = os.path.join(RESULTS_DIR, f"{os.path.basename(filename)}_report.txt")
    CURRENT_LOG_HANDLE = open(log_path, "w", encoding="utf-8")
    return log_path


def close_log():
    global CURRENT_LOG_HANDLE
    if CURRENT_LOG_HANDLE:
        CURRENT_LOG_HANDLE.close()
        CURRENT_LOG_HANDLE = None


def log_summary(msg):
    with open(SUMMARY_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")


def log(msg, color=""):
    # Print to terminal
    print(f"{color}{msg}{C_RESET}")
    # Write to individual report file
    if CURRENT_LOG_HANDLE:
        clean = re.sub(r"\033\[[0-9;]*m", "", str(msg))
        CURRENT_LOG_HANDLE.write(clean + "\n")
        CURRENT_LOG_HANDLE.flush()


def log_raw(msg):
    print(msg)
    if CURRENT_LOG_HANDLE:
        clean = re.sub(r"\033\[[0-9;]*m", "", str(msg))
        CURRENT_LOG_HANDLE.write(clean + "\n")


def print_header(msg, color=C_CYAN):
    border = "=" * 80
    log(f"\n{border}", color)
    log(msg, color)
    log(border, color)


def clean_artifacts():
    for f in glob.glob("*.bin") + glob.glob("*.log") + glob.glob("gdb_cmds.txt"):
        try:
            os.remove(f)
        except:
            pass


def run_cmd(cmd, timeout=45):
    try:
        if shutil.which("stdbuf") and sys.platform != "win32":
            cmd = ["stdbuf", "-o0", "-e0"] + cmd
        result = subprocess.run(
            cmd, capture_output=True, text=True, errors="replace", timeout=timeout
        )
        return result.stdout, result.stderr, result.returncode
    except subprocess.TimeoutExpired:
        return "[TIMEOUT]", "", -999


def run_gdb(dol, poison, frames, jitter):
    log(f"      >> Re-running with GDB...", C_YELLOW)
    jitter_flag = ["--jitter", "true"] if jitter else []
    gdb_cmds = [
        "set pagination off",
        f"run --diff-test --frames {frames} -e {dol} --poison {poison} {' '.join(jitter_flag)}",
        "bt",
        "quit",
    ]
    with open("gdb_cmds.txt", "w") as f:
        f.write("\n".join(gdb_cmds))
    out, _, _ = run_cmd(["gdb", "-batch", "-x", "gdb_cmds.txt", WIIMIX_EXE], timeout=60)
    if "Thread 1" in out:
        return out.split("Thread 1")[-1].split("Thread")[0].strip()
    return out[-1000:]


def find_source_file_smart(target):
    if not target:
        return None
    target_base = target.replace(".cpp", "").replace(".h", "")
    for root, _, files in os.walk(SOURCE_ROOT):
        for f in files:
            if f.lower() == f"{target_base.lower()}.cpp":
                return os.path.join(root, f)
    for root, _, files in os.walk(SOURCE_ROOT):
        for f in files:
            if f.lower() == f"{target_base.lower()}.h":
                return os.path.join(root, f)
    return None


def resolve_section_source(section_name):
    map_lookup = {
        "after main dsp state": "DSPEmulator.cpp",
        "DSP": "DSP.cpp",
        "DVDInterface": "DVDInterface.cpp",
        "DVDThread": "DVDThread.cpp",
        "CoreTiming": "CoreTiming.cpp",
        "PowerPC": "PowerPC.cpp",
        "Memory": "Memmap.cpp",
        "MemoryInterface": "MemoryInterface.cpp",
        "VideoInterface": "VideoInterface.cpp",
        "SerialInterface": "SI.cpp",
        "HW": "HW.cpp",
        "AudioInterface": "AudioInterface.cpp",
    }
    if section_name in map_lookup:
        return find_source_file_smart(map_lookup[section_name])
    clean = section_name.replace("after ", "").replace(" state", "").replace(" ", "")
    return find_source_file_smart(clean)


def parse_section_sizes(log_text):
    sections = {}
    offsets = []
    for line in log_text.splitlines():
        if "WII-MIX-MAP: Offset after" in line:
            parts = line.split(":")
            name = parts[1].replace("Offset after", "").strip()
            try:
                off = int(parts[2].strip(), 16)
                offsets.append((off, name))
            except:
                pass
    offsets.sort(key=lambda x: x[0])
    prev_off = 0
    for off, name in offsets:
        sections[name] = off - prev_off
        prev_off = off
    return sections


def analyze_size_mismatch(log_out):
    parts_b = log_out.split("WII-MIX-TEST-LOG: Save B START")
    parts_c = log_out.split("WII-MIX-TEST-LOG: Save C START")
    if len(parts_b) < 2 or len(parts_c) < 2:
        return None
    log_b = parts_b[1].split("WII-MIX-TEST-LOG")[0]
    log_c = parts_c[1].split("WII-MIX-TEST-LOG")[0]
    sizes_b = parse_section_sizes(log_b)
    sizes_c = parse_section_sizes(log_c)
    diffs = []
    for name, size_b in sizes_b.items():
        size_c = sizes_c.get(name, 0)
        if size_b != size_c:
            diffs.append(
                {
                    "name": name,
                    "size_b": size_b,
                    "size_c": size_c,
                    "delta": size_c - size_b,
                }
            )
    return diffs


def analyze_binary_mismatch(path1, path2, log_out):
    if not (os.path.exists(path1) and os.path.exists(path2)):
        return None
    with open(path1, "rb") as f1, open(path2, "rb") as f2:
        d1, d2 = f1.read(), f2.read()
    diff_idx = next((i for i in range(min(len(d1), len(d2))) if d1[i] != d2[i]), -1)
    if diff_idx == -1:
        if len(d1) == len(d2):
            return None
        diff_idx = min(len(d1), len(d2))

    markers = {}
    for line in log_out.splitlines():
        if "WII-MIX-MAP: Offset after" in line:
            parts = line.split(":")
            try:
                markers[int(parts[2].strip(), 16)] = (
                    parts[1].replace("Offset after", "").strip()
                )
            except:
                pass

    section = "Start/Header"
    sec_start = 0
    for off in sorted(markers.keys()):
        if off <= diff_idx:
            section, sec_start = markers[off], off

    return {
        "offset": diff_idx,
        "section": section,
        "val1": d1[diff_idx : diff_idx + 4],
        "val2": d2[diff_idx : diff_idx + 4],
        "delta": len(d2) - len(d1),
        "src_file": resolve_section_source(section),
        "rel_offset": diff_idx - sec_start,
        "size_diffs": analyze_size_mismatch(log_out),
    }


def run_test_case(dol, category, name, poison, frames, jitter):
    clean_artifacts()
    jitter_flag = ["--jitter", "true"] if jitter else []
    t_flag = ["--identity-test"] if name.startswith("Identity") else ["--diff-test"]
    cmd = (
        [WIIMIX_EXE]
        + t_flag
        + ["--frames", str(frames), "-e", dol, "--poison", str(poison)]
        + jitter_flag
    )

    line = f"{category}: {name:<25} | Poison: {poison:<5} | Jitter: {str(jitter):<5}"
    sys.stdout.write(f"{C_CYAN}{line}{C_RESET}")
    sys.stdout.flush()
    if CURRENT_LOG_HANDLE:
        CURRENT_LOG_HANDLE.write(line)

    out, err, ret = run_cmd(cmd)

    status = "PASS" if ret == 0 else "FAIL"
    reason = "OK"
    analysis = None

    if ret != 0:
        reason = (
            "CRASH"
            if ret == -11
            else "ASSERT" if ret == -6 else "SIZE" if "mismatch" in err else "FAIL"
        )
        msg = f" -> {C_RED}{status} ({reason}){C_RESET}"
        print(msg)
        if CURRENT_LOG_HANDLE:
            CURRENT_LOG_HANDLE.write(re.sub(r"\033\[[0-9;]*m", "", msg) + "\n")

        if reason == "SIZE" or reason == "FAIL":
            p1, p2 = (
                ("Identity_A.bin", "Identity_B.bin")
                if name.startswith("Identity")
                else ("B.bin", "C.bin")
            )
            analysis = analyze_binary_mismatch(p1, p2, out)
    else:
        msg = f" -> {C_GREEN}PASS{C_RESET}"
        print(msg)
        if CURRENT_LOG_HANDLE:
            CURRENT_LOG_HANDLE.write(re.sub(r"\033\[[0-9;]*m", "", msg) + "\n")

    return TestResult(
        category, name, poison, jitter, frames, status, ret, reason, out, err, analysis
    )


def run_phase(dol, title, tests):
    print_header(title)
    results = []
    failed = False
    for t in tests:
        res = run_test_case(
            dol, title, t["name"], t["poison"], t["frames"], t["jitter"]
        )
        results.append(res)
        if res.status == "FAIL":
            failed = True
            break
    return results, failed


def dump_file(path):
    if not path or not os.path.exists(path):
        return
    print_header(f"FULL SOURCE CODE: {os.path.basename(path)}", C_CYAN)
    try:
        with open(path, "r", errors="ignore") as f:
            lines = f.readlines()
            for i, line in enumerate(lines):
                log_raw(f"{i+1:4} | {line.rstrip()}")
    except:
        log(f"[!] Could not read {path}", C_RED)


def generate_failure_report(target, dol_path, no_src=False):
    print_header("FORENSIC ANALYSIS REPORT", C_MAGENTA)
    log(f"Primary Failure: {target.category} - {target.name}")
    log(
        f"Configuration:   Poison={target.poison}, Jitter={target.jitter}, Frames={target.frames}"
    )
    log(f"Exit Code:       {target.code} ({target.reason})")

    if target.reason in ["CRASH", "HANG", "ASSERT"]:
        trace = run_gdb(dol_path, target.poison, target.frames, target.jitter)
        print_header("GDB STACK TRACE", C_YELLOW)
        log(trace)

    if target.analysis:
        a = target.analysis
        if a.get("size_diffs"):
            print_header("SECTION SIZE DIFFERENTIAL", C_YELLOW)
            log(f"{'SECTION':<30} | {'SIZE B':<10} | {'SIZE C':<10} | {'DELTA'}")
            log("-" * 65)
            for d in a["size_diffs"]:
                log(
                    f"{d['name']:<30} | {d['size_b']:<10} | {d['size_c']:<10} | {C_RED}{d['delta']:+d}{C_RESET}"
                )

        print_header("BINARY CONTENT ANALYSIS", C_YELLOW)
        log(f"Mismatch Offset: 0x{a['offset']:X}")
        log(f"Section:         {a['section']}")
        log(f"Relative Offset: +0x{a['rel_offset']:X}")

        v1_i = int.from_bytes(a["val1"], "little")
        v2_i = int.from_bytes(a["val2"], "little")
        v1_f = struct.unpack("<f", a["val1"])[0]
        v2_f = struct.unpack("<f", a["val2"])[0]

        log(f"\nValue B (Clean): 0x{v1_i:08X} (Int: {v1_i}, Float: {v1_f:.6f})")
        log(f"Value C (Dirty): 0x{v2_i:08X} (Int: {v2_i}, Float: {v2_f:.6f})")

        if not no_src:
            dump_file(find_source_file_smart("State.cpp"))
            dump_file(a["src_file"])
            if a["section"] in DEPENDENCIES:
                for dep in DEPENDENCIES[a["section"]]:
                    path = find_source_file_smart(dep)
                    if path and path != a["src_file"]:
                        dump_file(path)
        else:
            print_header("SOURCE FILES (CONTENT SUPPRESSED)", C_CYAN)
            state_path = find_source_file_smart("State.cpp")
            log(
                f"State Logic:   {os.path.basename(state_path) if state_path else 'State.cpp (Not found)'}"
            )

            src_path = a["src_file"]
            log(
                f"Target Source: {os.path.basename(src_path) if src_path else 'Unknown'}"
            )

            if a["section"] in DEPENDENCIES:
                deps = DEPENDENCIES[a["section"]]
                log(f"Dependencies:  {', '.join(deps)}")

    print_header("FULL DEBUG LOG", C_MAGENTA)
    if target.out:
        log("--- STDOUT ---")
        log_raw(target.out)
    if target.err:
        log("\n--- STDERR ---")
        log_raw(target.err)


def scan_file(file_path, no_src=False):
    filename = os.path.basename(file_path)
    print_header(f"SCANNING: {filename}", C_BOLD)

    log_path = open_log(file_path)
    log(f"Processing {file_path}...")

    # PHASE 1: BASELINE
    tests_baseline = [
        {"name": "Identity Check", "poison": 0, "frames": 0, "jitter": False},
        {"name": "1 Step Check", "poison": 0, "frames": 1, "jitter": False},
        {"name": "10 Step Check", "poison": 0, "frames": 10, "jitter": False},
    ]
    res, failed = run_phase(file_path, "BASELINE", tests_baseline)
    if failed:
        generate_failure_report(res[-1], file_path, no_src)
        log_summary(
            f"{filename:<40} : {C_RED}FAIL{C_RESET} (Baseline - {res[-1].reason})"
        )
        close_log()
        return False

    # PHASE 2: JITTER
    tests_jitter = [
        {"name": "Identity Check", "poison": 0, "frames": 0, "jitter": True},
        {"name": "1 Step Check", "poison": 0, "frames": 1, "jitter": True},
        {"name": "10 Step Check", "poison": 0, "frames": 10, "jitter": True},
    ]
    res, failed = run_phase(file_path, "JITTER", tests_jitter)
    if failed:
        generate_failure_report(res[-1], file_path, no_src)
        log_summary(
            f"{filename:<40} : {C_RED}FAIL{C_RESET} (Jitter - {res[-1].reason})"
        )
        close_log()
        return False

    # PHASE 3: POISON
    tests_poison = []
    for p in POISON_SUITE:
        tests_poison.append(
            {
                "name": f"Identity ({POISON_NAMES.get(p)})",
                "poison": p,
                "frames": 0,
                "jitter": False,
            }
        )
        tests_poison.append(
            {
                "name": f"1 Step ({POISON_NAMES.get(p)})",
                "poison": p,
                "frames": 1,
                "jitter": False,
            }
        )
        tests_poison.append(
            {
                "name": f"10 Step ({POISON_NAMES.get(p)})",
                "poison": p,
                "frames": 10,
                "jitter": False,
            }
        )

    res, failed = run_phase(file_path, "POISON", tests_poison)
    if failed:
        generate_failure_report(res[-1], file_path, no_src)
        log_summary(
            f"{filename:<40} : {C_RED}FAIL{C_RESET} (Poison - {res[-1].reason})"
        )
        close_log()
        return False

    # PHASE 4: CHAOS
    tests_chaos = []
    for p in POISON_SUITE:
        tests_chaos.append(
            {
                "name": f"1 Step ({POISON_NAMES.get(p)})",
                "poison": p,
                "frames": 1,
                "jitter": True,
            }
        )
        tests_chaos.append(
            {
                "name": f"10 Step ({POISON_NAMES.get(p)})",
                "poison": p,
                "frames": 10,
                "jitter": True,
            }
        )

    res, failed = run_phase(file_path, "CHAOS", tests_chaos)

    if failed:
        generate_failure_report(res[-1], file_path, no_src)
        log_summary(f"{filename:<40} : {C_RED}FAIL{C_RESET} (Chaos - {res[-1].reason})")
    else:
        print_header(f"{filename} - ALL TESTS PASSED", C_GREEN)
        log_summary(f"{filename:<40} : {C_GREEN}PASS{C_RESET}")

    close_log()
    return not failed


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("path", help="Path to file OR directory to scan")
    parser.add_argument(
        "--no-src",
        action="store_true",
        help="Do not dump source code in failure reports",
    )
    args = parser.parse_args()

    ensure_dirs()
    with open(SUMMARY_FILE, "w") as f:
        f.write("--- WIIMIX DETERMINISM BATCH SCAN SUMMARY ---\n")

    files_to_scan = []
    if os.path.isfile(args.path):
        files_to_scan.append(args.path)
    else:
        extensions = [".dol", ".elf", ".iso", ".gcm", ".ciso"]
        for root, dirs, files in os.walk(args.path):
            for file in files:
                if any(file.lower().endswith(ext) for ext in extensions):
                    files_to_scan.append(os.path.join(root, file))
        files_to_scan.sort()

    print(f"{C_BOLD}Found {len(files_to_scan)} files to scan.{C_RESET}")
    print(f"Results will be saved to: {os.path.abspath(RESULTS_DIR)}\n")

    success_count = 0
    for f in files_to_scan:
        if scan_file(f, args.no_src):
            success_count += 1

    print_header("BATCH SCAN COMPLETE", C_MAGENTA)
    print(f"Total Files: {len(files_to_scan)}")
    print(f"Passed:      {C_GREEN}{success_count}{C_RESET}")
    print(f"Failed:      {C_RED}{len(files_to_scan) - success_count}{C_RESET}")
    print(f"Summary:     {os.path.abspath(SUMMARY_FILE)}")


if __name__ == "__main__":
    main()
