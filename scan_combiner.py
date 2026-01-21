import os
import sys

CHUNK_SIZE = 15000  # Maximum lines per output file


def combine_files(directory):
    summary_path = os.path.join(directory, "SUMMARY.txt")
    output_base = os.path.join(directory, "scan_results_combined_part")

    # Read SUMMARY.txt
    with open(summary_path, "r", encoding="utf-8") as summary_file:
        combined_lines = summary_file.read().splitlines(keepends=True)
        combined_lines.append("\n")

    # Get all files except SUMMARY.txt and output files
    files = [
        f
        for f in os.listdir(directory)
        if os.path.isfile(os.path.join(directory, f))
        and f != "SUMMARY.txt"
        and not f.startswith("scan_results_combined_part")
    ]

    files.sort()

    # Append contents of each file
    for filename in files:
        file_path = os.path.join(directory, filename)
        combined_lines.append(f"--- {filename} ---\n")
        with open(file_path, "r", encoding="utf-8") as f:
            combined_lines.extend(f.read().splitlines(keepends=True))
            combined_lines.append("\n")

    # Write in chunks of CHUNK_SIZE lines
    total_lines = len(combined_lines)
    part = 1
    for i in range(0, total_lines, CHUNK_SIZE):
        chunk = combined_lines[i : i + CHUNK_SIZE]
        output_path = f"{output_base}{part}.txt"
        with open(output_path, "w", encoding="utf-8") as out_file:
            out_file.writelines(chunk)
        part += 1


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python scan_combiner.py <directory>")
    else:
        combine_files(sys.argv[1])
