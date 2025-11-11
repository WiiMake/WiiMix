import os
import subprocess
import signal
import time
import sys
from tqdm import tqdm  # Import the progress bar library

# --- Configuration ---
BIN_DIR = os.path.expanduser("~/Programming/OpenSource/gamecube-examples/bin")
WIIMIX_EXE = "./build/Binaries/wiimix-nogui"
TEST_TIMEOUT = 3.5
SUCCESS_STRING = "Diff Test PASSED!"
FAILURE_STRING = "Diff Test FAILED"
# --- End Configuration ---


def find_dol_files():
    """Finds all .dol files in the target directory."""
    try:
        files = [
            f
            for f in os.listdir(BIN_DIR)
            if os.path.isfile(os.path.join(BIN_DIR, f)) and f.endswith(".dol")
        ]
        files.sort()
        return files
    except FileNotFoundError:
        print(f"Error: Directory not found: {BIN_DIR}", file=sys.stderr)
        return []


def run_single_test(filename):
    """
    Runs a single test, captures its output, and returns a result.
    This version uses wait() THEN communicate() to prevent I/O deadlocks.
    """
    file_path = os.path.join(BIN_DIR, filename)
    cmd = [WIIMIX_EXE, "--diff-test", "-e", file_path]

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
    files = find_dol_files()
    if not files:
        print(f"No .dol files found in {BIN_DIR}", file=sys.stderr)
        return 1

    print(f"Found {len(files)} tests to run...")
    print(f"Emulator: {WIIMIX_EXE}\n")

    failures = []

    # Wrap the list of files in tqdm() to create the progress bar
    for filename in tqdm(
        files, unit="test", bar_format="{l_bar}{bar}| {n_fmt}/{total_fmt}"
    ):

        # Use tqdm.write() for all printing inside the loop
        tqdm.write(f"\n--- RUNNING: {filename} ---")

        status, reason = run_single_test(filename)

        if status == "PASS":
            tqdm.write("--- RESULT: PASS ---")
        else:
            tqdm.write(f"--- RESULT: FAIL ---")
            tqdm.write(f"    Reason: {reason}")
            failures.append((filename, reason))

    # --- Final Summary ---
    print("\n" + "=" * 70)
    print("TEST RUN SUMMARY")
    print("=" * 70)

    if not failures:
        print(f"All {len(files)} tests passed! ✨")
        return 0
    else:
        print(f"{len(failures)} / {len(files)} tests failed: ❌\n")
        for filename, reason in failures:
            print(f"- {filename}: {reason}")
        return 1


if __name__ == "__main__":
    # Exit with a non-zero code if any tests failed (good for CI)
    sys.exit(main())
