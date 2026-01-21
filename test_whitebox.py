import os
import subprocess
import sys

# --- CONFIG ---
WIIMIX_EXE = "./build/Binaries/wiimix-nogui"
TEST_FILE = os.path.expanduser(
    "~/Programming/OpenSource/gamecube-examples/bin/acube.dol"
)  # The consistently failing test
MAX_FRAMES = 120
# --------------


def run_test(frames):
    print(f"Testing {frames} frames...")
    cmd = [WIIMIX_EXE, "--diff-test", "--frames", str(frames), "-e", TEST_FILE]
    try:
        # Use communicate to avoid deadlock
        proc = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        stdout, stderr = proc.communicate(timeout=15)

        if "Diff Test PASSED!" in stdout:
            return True
        return False
    except subprocess.TimeoutExpired:
        if proc:
            proc.kill()
        return False
    except Exception as e:
        print(f"Error: {e}")
        return False


def main():
    print(f"--- BISECTING FAILURE FRAME FOR {os.path.basename(TEST_FILE)} ---")

    # Linear scan is safer than binary search for determinism issues
    # because sometimes a desync might randomly 're-sync' by luck.
    for i in range(1, MAX_FRAMES + 1):
        success = run_test(i)
        if not success:
            print(f"\n[!] FIRST FAILURE FOUND AT FRAME: {i}")
            print(
                f"Run the emulator manually with --frames {i} to debug this specific moment."
            )
            return
        else:
            print(f"Frame {i}: OK")

    print("\nAll frames passed? The issue might be intermittent or deeper.")


if __name__ == "__main__":
    main()
