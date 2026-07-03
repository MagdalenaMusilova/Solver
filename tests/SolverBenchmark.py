#!/usr/bin/env python3

import subprocess
import time
import os
import json
from pathlib import Path
from datetime import datetime
import statistics
import sys

class SolverBenchmark:
    def __init__(self, solver_binary="./cmake-build-debug/Solver"):
        self.solver_binary = solver_binary
        self.results = []
        self.data_dir = "data"
        self.timing_tests_dir = "data/timing_tests"

    def run_solver(self, board_file, solver_type="sequential", threads=1, runs=1):
        """Run solver and return execution time(s)."""
        times = []

        for run in range(runs):
            cmd = [
                self.solver_binary,
                "--board", board_file,
                "--solver", solver_type,
            ]

            if threads > 1:
                cmd.extend(["--threads", str(threads)])

            try:
                start = time.perf_counter()
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
                end = time.perf_counter()

                elapsed = end - start
                times.append(elapsed)

                if result.returncode != 0:
                    print(f"Warning: Solver returned non-zero code for {board_file}")
                    return None

            except subprocess.TimeoutExpired:
                print(f"Error: Solver timed out for {board_file}")
                return None
            except Exception as e:
                print(f"Error running solver: {e}")
                return None

        return times

    def benchmark_all_solvers(self):
        """Benchmark all solver types on test data."""
        print("=" * 70)
        print("          SOLVER PERFORMANCE BENCHMARKS")
        print("=" * 70)
        print()

        test_files = self._get_test_files()
        solver_types = ["sequential", "task-parallel", "data-parallel"]
        thread_counts = {"sequential": [1], "task-parallel": [2, 4, 8], "data-parallel": [2, 4, 8]}

        all_results = {}

        for file_path in test_files:
            file_name = os.path.basename(file_path)
            print(f"\nTesting: {file_name}")
            print("-" * 70)

            file_results = {}

            for solver_type in solver_types:
                for threads in thread_counts[solver_type]:
                    print(f"  {solver_type} (threads={threads})...", end=" ", flush=True)

                    times = self.run_solver(file_path, solver_type=solver_type, threads=threads, runs=3)

                    if times is None:
                        print("FAILED")
                        continue

                    avg_time = statistics.mean(times)
                    std_dev = statistics.stdev(times) if len(times) > 1 else 0
                    min_time = min(times)
                    max_time = max(times)

                    print(f"✓ {avg_time:.3f}s (±{std_dev:.3f}s, min={min_time:.3f}s, max={max_time:.3f}s)")

                    key = f"{solver_type}_t{threads}"
                    file_results[key] = {
                        "average": avg_time,
                        "stddev": std_dev,
                        "min": min_time,
                        "max": max_time,
                        "runs": len(times)
                    }

            all_results[file_name] = file_results

        self._print_summary(all_results)
        self._save_results(all_results)

    def benchmark_timing_tests(self):
        """Benchmark on timing test files (no reference solution)."""
        if not os.path.isdir(self.timing_tests_dir):
            print(f"Timing tests directory not found: {self.timing_tests_dir}")
            return

        print("\n" + "=" * 70)
        print("         TIMING TESTS (No Reference Solution)")
        print("=" * 70)
        print()

        timing_files = self._get_timing_test_files()

        if not timing_files:
            print("No timing test files found.")
            return

        for file_path in timing_files:
            file_name = os.path.basename(file_path)
            print(f"\n{file_name}")
            print("-" * 70)

            times_seq = self.run_solver(file_path, solver_type="sequential", threads=1, runs=1)
            if times_seq:
                print(f"  Sequential: {times_seq[0]:.3f}s")

            for solver_type in ["task-parallel", "data-parallel"]:
                for threads in [4, 8]:
                    times = self.run_solver(file_path, solver_type=solver_type, threads=threads, runs=1)
                    if times:
                        print(f"  {solver_type} (t={threads}): {times[0]:.3f}s")

    def _get_test_files(self):
        """Get list of test files with reference solutions."""
        test_files = [
            "data/mapa3_5.txt",
            "data/mapa5_5.txt",
            "data/mapa3_11.txt",
            "data/mapa7_7.txt",
            "data/mapa4_15.txt",
            "data/mapa5_11.txt",
            "data/mapa7_10.txt",
            "data/mapa5_15.txt",
            "data/mapa7_11.txt",
            "data/mapa15_5.txt",
            "data/mapa9_9.txt"
        ]

        existing_files = [f for f in test_files if os.path.isfile(f)]
        return existing_files

    def _get_timing_test_files(self):
        """Get list of timing test files."""
        if not os.path.isdir(self.timing_tests_dir):
            return []

        timing_files = []
        for file in os.listdir(self.timing_tests_dir):
            if file.endswith(".txt"):
                timing_files.append(os.path.join(self.timing_tests_dir, file))

        return sorted(timing_files)

    def _print_summary(self, results):
        """Print a summary of results."""
        print("\n" + "=" * 70)
        print("                        SUMMARY")
        print("=" * 70)

        for file_name, file_results in results.items():
            print(f"\n{file_name}:")

            if "sequential_t1" in file_results:
                seq_time = file_results["sequential_t1"]["average"]
                print(f"  Sequential baseline: {seq_time:.3f}s")

                for solver_type in ["task-parallel", "data-parallel"]:
                    for threads in [2, 4, 8]:
                        key = f"{solver_type}_t{threads}"
                        if key in file_results:
                            result = file_results[key]
                            speedup = seq_time / result["average"]
                            print(f"  {key}: {result['average']:.3f}s (speedup: {speedup:.2f}x)")

    def _save_results(self, results):
        """Save benchmark results to JSON file."""
        timestamp = datetime.now().isoformat()
        output_file = f"benchmark_results_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"

        with open(output_file, 'w') as f:
            json.dump({
                "timestamp": timestamp,
                "results": results
            }, f, indent=2)

        print(f"\nResults saved to: {output_file}")

def main():
    if not os.path.isfile("./cmake-build-debug/Solver"):
        print("Error: Solver binary not found at ./cmake-build-debug/Solver")
        print("Please build the project first:")
        print("  mkdir -p cmake-build-debug")
        print("  cd cmake-build-debug")
        print("  cmake ..")
        print("  make")
        sys.exit(1)

    benchmark = SolverBenchmark()
    benchmark.benchmark_all_solvers()
    benchmark.benchmark_timing_tests()

if __name__ == "__main__":
    main()