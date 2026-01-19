import os
import subprocess
import signal
import time
import sys
import random
from tqdm import tqdm  # Import the progress bar library

# --- Configuration ---
BIN_DIR = os.path.expanduser("~/Programming/OpenSource/gamecube-examples/bin")
GAMES_DIR = os.path.expanduser("~/dolphingames")
WIIMIX_EXE = "./build/Binaries/wiimix-nogui"
TEST_TIMEOUT = 10  # seconds
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
    Runs a single test, captures its output, and returns a result.
    This version uses wait() THEN communicate() to prevent I/O deadlocks.
    """
    cmd = [WIIMIX_EXE, "--diff-test", "--frames", str(steps), "-e", file_path]

    proc = None
    stdout = ""
    stderr = ""

    try:
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="ignore",
        )

        # 1. Wait for the process to terminate OR timeout.
        try:
            proc.wait(timeout=TEST_TIMEOUT)
        except subprocess.TimeoutExpired:
            if proc:
                proc.kill()
                stdout, stderr = proc.communicate()

            if stdout:
                tqdm.write("--- STDOUT (on timeout) ---")
                tqdm.write(stdout)
            if stderr:
                tqdm.write("--- STDERR (on timeout) ---")
                tqdm.write(stderr)
            return "FAIL", f"TIMEOUT (Process ran > {TEST_TIMEOUT}s)"

        # 2. The process is dead. Safely read all output.
        stdout, stderr = proc.communicate()

        # 3. Print the output we captured
        if stdout:
            tqdm.write("--- STDOUT ---")
            tqdm.write(stdout)
        if stderr:
            tqdm.write("--- STDERR ---")
            tqdm.write(stderr)

        full_output = stdout + stderr

        # --- NEW PASS/FAIL LOGIC (Priority Changed) ---

        # CHECK 1: Success String (Priority 1)
        # If we see this, we declare success, even if the
        # emulator crashed on shutdown (like acube.dol).
        if SUCCESS_STRING in full_output:
            return "PASS", ""

        # CHECK 2: Crash / Segfault (Priority 2)
        # If no success string was found, then a non-zero
        # exit code is a real failure (like mp3player.dol).
        if proc.returncode != 0:
            reason = "NON-ZERO EXIT CODE"
            if proc.returncode < 0:
                try:
                    sig_name = signal.Signals(-proc.returncode).name
                    reason = f"SEGFAULT or SIGNAL ({sig_name})"
                except ValueError:
                    reason = "Process terminated by UNKNOWN SIGNAL"
            return "FAIL", f"{reason} (Code: {proc.returncode})"

        # CHECK 3: Failure String (Priority 3)
        if FAILURE_STRING in full_output:
            return "FAIL", "FAILURE_STRING found"

        # CHECK 4: Silent Pass (Priority 4)
        # Exited 0, but didn't print PASS or FAIL.
        return "FAIL", "SILENT PASS (Exited 0, but no success string)"

    except Exception as e:
        if proc:
            proc.kill()
        return "FAIL", f"Exception: {e}"


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

        # First, test single step from startup (with sub-progress bar)
        for file_path in tqdm(
            files, unit="test", bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt}"
        ):

            # Use tqdm.write() for all printing inside the loop
            tqdm.write(f"\n--- RUNNING: {file_path} ---")
            out_file.write(f"\n--- RUNNING: {file_path} ---\n")

            status, reason = run_single_test(file_path)

            if status == "PASS":
                tqdm.write("--- RESULT: PASS ---")
                out_file.write("--- RESULT: PASS ---\n")
            else:
                tqdm.write(f"--- RESULT: FAIL ---")
                tqdm.write(f"    Reason: {reason}")
                out_file.write(f"--- RESULT: FAIL ---\n")
                out_file.write(f"    Reason: {reason}\n")
                failures.append((file_path, reason))

        # Have a local summary for that test group
        summary = "\n" + "=" * 70 + "\nSINGLE-STEP TEST SUMMARY\n"
        group_total = len(files)
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

        # Then, test multi-step from startup (with sub-progress bar)
        for file_path in tqdm(
            files, unit="test", bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt}"
        ):

            tqdm.write(f"\n--- RUNNING MULTI-STEP: {file_path} ---")
            out_file.write(f"\n--- RUNNING MULTI-STEP: {file_path} ---\n")

            steps = random.randint(2, 120)  # Random steps between 2 and 120
            status, reason = run_single_test(file_path, steps=steps)

            if status == "PASS":
                tqdm.write(f"--- RESULT: PASS (Steps: {steps}) ---")
                out_file.write(f"--- RESULT: PASS (Steps: {steps}) ---\n")
            else:
                tqdm.write(f"--- RESULT: FAIL (Steps: {steps}) ---")
                tqdm.write(f"    Reason: {reason}")
                out_file.write(f"--- RESULT: FAIL (Steps: {steps}) ---\n")
                out_file.write(f"    Reason: {reason}\n")
                failures.append((file_path, f"Multi-step failed: {reason}"))

        # Have a local summary for that test group
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
