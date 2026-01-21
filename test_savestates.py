import os
import subprocess
import signal
import sys
import random
from tqdm import tqdm  # Import the progress bar library

# --- Configuration ---
BIN_DIR = os.path.expanduser("~/Programming/OpenSource/gamecube-examples/bin")
GAMES_DIR = os.path.expanduser("~/dolphingames")
WIIMIX_EXE = "./build/Binaries/wiimix-nogui"
TEST_TIMEOUT = 20  # seconds
SUCCESS_STRING = "Diff Test PASSED!"
FAILURE_STRING = "Diff Test FAILED"
# --- End Configuration ---


def find_test_files():
    """Finds all .dol files in the target directory."""
    try:
        files = [
            os.path.join(dir, f)
            for dir in [BIN_DIR, GAMES_DIR]
            for f in os.listdir(dir)
            if os.path.isfile(os.path.join(dir, f))
            and (f.endswith(".dol") or f.endswith(".ciso"))
        ]
        files.sort()
        return files
    except FileNotFoundError:
        print(f"Error: Directory not found: {BIN_DIR}", file=sys.stderr)
        return []


def run_single_test(file_path, steps=1):
    """
    Runs a single test, captures its output safely to prevent deadlocks,
    and returns (status, reason, full_log).
    """
    cmd = [WIIMIX_EXE, "--diff-test", "--frames", str(steps), "-e", file_path]

    proc = None
    full_output = ""

    try:
        # Start the process
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="ignore",
        )

        stdout = ""
        stderr = ""

        # Use communicate() to read from pipes while waiting.
        # This prevents the C++ process from hanging if it fills the OS pipe buffer.
        try:
            stdout, stderr = proc.communicate(timeout=TEST_TIMEOUT)
        except subprocess.TimeoutExpired:
            # If it times out, kill it and grab whatever output is available
            proc.kill()
            outs, errs = proc.communicate()
            stdout = (stdout or "") + (outs or "")
            stderr = (stderr or "") + (errs or "")
            full_output = stdout + stderr
            return "FAIL", f"TIMEOUT (Process ran > {TEST_TIMEOUT}s)", full_output

        full_output = stdout + stderr

        # --- Check Test Results ---

        # Priority 1: Did it print the success string?
        if SUCCESS_STRING in full_output:
            return "PASS", "", full_output

        # Priority 2: Did it exit with an error code?
        if proc.returncode != 0:
            reason = "NON-ZERO EXIT CODE"
            if proc.returncode < 0:
                try:
                    # Negative return codes are signals (e.g. -11 = SIGSEGV)
                    sig_name = signal.Signals(-proc.returncode).name
                    reason = f"SEGFAULT or SIGNAL ({sig_name})"
                except ValueError:
                    reason = "Process terminated by UNKNOWN SIGNAL"
            return "FAIL", f"{reason} (Code: {proc.returncode})", full_output

        # Priority 3: Did it print the explicit failure string?
        if FAILURE_STRING in full_output:
            return "FAIL", "FAILURE_STRING found", full_output

        # Priority 4: Silent Pass (Exited 0 but no explicit success msg)
        return "FAIL", "SILENT PASS (Exited 0, but no success string)", full_output

    except Exception as e:
        if proc:
            try:
                proc.kill()
            except:
                pass
        return "FAIL", f"Exception: {e}", full_output


def main():
    """Main test runner function."""
    files = find_test_files()
    if not files:
        print(
            f"No .dol or .ciso files found in {BIN_DIR} or {GAMES_DIR}", file=sys.stderr
        )
        return 1

    print(f"Found {len(files)} tests to run...")
    print(f"Emulator: {WIIMIX_EXE}\n")

    failures = []

    # Open the output file for writing all test output
    with open("test_output", "w", encoding="utf-8") as out_file:

        # --- PHASE 1: Single-Step Tests ---
        for file_path in tqdm(
            files, unit="test", bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt}"
        ):
            tqdm.write(f"\n--- RUNNING: {file_path} ---")
            out_file.write(f"\n--- RUNNING: {file_path} ---\n")

            status, reason, full_log = run_single_test(file_path)

            if status == "PASS":
                tqdm.write("--- RESULT: PASS ---")
                out_file.write("--- RESULT: PASS ---\n")
            else:
                tqdm.write(f"--- RESULT: FAIL ---")
                tqdm.write(f"    Reason: {reason}")
                out_file.write(f"--- RESULT: FAIL ---\n")
                out_file.write(f"    Reason: {reason}\n")

                # Write the captured log to the file for debugging
                out_file.write("-" * 20 + " CAPTURED LOG " + "-" * 20 + "\n")
                out_file.write(full_log)
                out_file.write("\n" + "-" * 54 + "\n")

                failures.append((file_path, reason))

        # Phase 1 Summary
        summary = "\n" + "=" * 70 + "\nSINGLE-STEP TEST SUMMARY\n"
        group_total = len(files)
        # Filter out multi-step failures (none yet)
        group_failures = [
            f for f in failures if not f[1].startswith("Multi-step failed:")
        ]
        group_fail_count = len(group_failures)
        group_pass_count = group_total - group_fail_count

        summary += (
            f"Tests run: {group_total}\n"
            f"Failures: {group_fail_count}\n"
            f"Successes: {group_pass_count}\n"
        )
        if group_failures:
            summary += "Failed tests:\n"
            for filename, reason in group_failures:
                summary += f"- {filename}: {reason}\n"
        else:
            summary += "No failures in this group.\n"
        summary += "=" * 70 + "\n"
        print(summary)
        out_file.write(summary)

        # --- PHASE 2: Multi-Step Tests ---
        for file_path in tqdm(
            files, unit="test", bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt}"
        ):
            tqdm.write(f"\n--- RUNNING MULTI-STEP: {file_path} ---")
            out_file.write(f"\n--- RUNNING MULTI-STEP: {file_path} ---\n")

            steps = random.randint(2, 120)  # Random steps between 2 and 120
            status, reason, full_log = run_single_test(file_path, steps=steps)

            if status == "PASS":
                tqdm.write(f"--- RESULT: PASS (Steps: {steps}) ---")
                out_file.write(f"--- RESULT: PASS (Steps: {steps}) ---\n")
            else:
                tqdm.write(f"--- RESULT: FAIL (Steps: {steps}) ---")
                tqdm.write(f"    Reason: {reason}")
                out_file.write(f"--- RESULT: FAIL (Steps: {steps}) ---\n")
                out_file.write(f"    Reason: {reason}\n")

                # Write the captured log to the file
                out_file.write("-" * 20 + " CAPTURED LOG " + "-" * 20 + "\n")
                out_file.write(full_log)
                out_file.write("\n" + "-" * 54 + "\n")

                failures.append((file_path, f"Multi-step failed: {reason}"))

        # Phase 2 Summary
        summary = "\n" + "=" * 70 + "\nMULTI-STEP TEST SUMMARY\n"
        group_total = len(files)
        group_failures = [f for f in failures if f[1].startswith("Multi-step failed:")]
        group_fail_count = len(group_failures)
        group_pass_count = group_total - group_fail_count

        summary += (
            f"Tests run: {group_total}\n"
            f"Failures: {group_fail_count}\n"
            f"Successes: {group_pass_count}\n"
        )
        if group_failures:
            summary += "Failed tests:\n"
            for filename, reason in group_failures:
                summary += f"- {filename}: {reason}\n"
        else:
            summary += "No failures in this group.\n"
        summary += "=" * 70 + "\n"
        print(summary)
        out_file.write(summary)

        # --- Final Summary ---
        summary = "\n" + "=" * 70 + "\nFINAL TEST SUMMARY\n" + "=" * 70 + "\n"
        total_tests = len(files) * 2
        total_failures = len(failures)
        total_passes = total_tests - total_failures

        final_summary = (
            f"Total tests run: {total_tests}\n"
            f"Total failures: {total_failures}\n"
            f"Total passes: {total_passes}\n"
        )
        print(final_summary)
        out_file.write(final_summary)

        if not failures:
            msg = f"All {total_tests} tests passed! ✨\n"
            print(msg)
            out_file.write(msg)
            return 0
        else:
            msg = f"{total_failures} / {total_tests} tests failed: ❌\n"
            print(msg)
            out_file.write(msg)
            for filename, reason in failures:
                fail_line = f"- {filename}: {reason}\n"
                print(fail_line, end="")
                out_file.write(fail_line)
            return 1


if __name__ == "__main__":
    # Exit with a non-zero code if any tests failed (good for CI)
    sys.exit(main())
