#!/usr/bin/env python3
import subprocess
import json
import sys
import argparse
import platform
from pathlib import Path
from collections import defaultdict
from typing import Any

HAS_MATPLOTLIB = False
plt = None
matplotlib = None

try:
    import matplotlib as _matplotlib
    import matplotlib.pyplot as _plt
    _matplotlib.use('Agg')
    plt = _plt
    matplotlib = _matplotlib
    HAS_MATPLOTLIB = True
except ImportError:
    pass

BUILD_DIR = Path(__file__).parent / "build"
RESULTS_DIR = BUILD_DIR / "results"
EXECUTABLE = BUILD_DIR / "allocbench.exe" if sys.platform == "win32" else BUILD_DIR / "allocbench"
OUTPUT_FILE = Path(__file__).parent / "bench_results.md"
PLOTS_BASE_DIR = Path(__file__).parent / "plots"

def get_platform_name() -> str:
    system = platform.system().lower()
    if system == "windows":
        return "windows"
    elif system == "linux":
        return "linux"
    elif system == "darwin":
        return "macos"
    return system

PLOTS_DIR = PLOTS_BASE_DIR / get_platform_name()

def run_benchmarks(graph_mode: bool = False) -> str:
    cmd = [str(EXECUTABLE)]
    if graph_mode:
        cmd.append("--graph")
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=BUILD_DIR, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running benchmarks: {result.stderr}")
        sys.exit(1)
    return result.stdout

def load_latest_benchmark_json() -> dict:
    json_files = sorted(RESULTS_DIR.glob("benchmark_*.json"), key=lambda x: x.stat().st_mtime, reverse=True)
    if not json_files:
        return {"results": []}
    with open(json_files[0]) as f:
        return json.load(f)

def load_graph_jsons() -> list[dict]:
    graph_files = sorted(RESULTS_DIR.glob("graph_*.json"))
    results = []
    for gf in graph_files:
        with open(gf) as f:
            results.append(json.load(f))
    return results

def format_number(val: Any, unit: str = "") -> str:
    if val is None:
        return "N/A"
    if isinstance(val, float):
        if val >= 1e6:
            return f"{val/1e6:.2f}M{unit}"
        elif val >= 1e3:
            return f"{val/1e3:.2f}K{unit}"
        else:
            return f"{val:.2f}{unit}"
    return str(val) + unit

def format_bytes(val: Any) -> str:
    if val is None or val == 0:
        return "0"
    if val >= 1 << 30:
        return f"{val / (1 << 30):.2f} GB"
    elif val >= 1 << 20:
        return f"{val / (1 << 20):.2f} MB"
    elif val >= 1 << 10:
        return f"{val / (1 << 10):.2f} KB"
    return f"{val} B"

def generate_benchmark_tables(data: dict) -> str:
    md = "## Results\n\n"
    benchmarks = defaultdict(list)
    for entry in data.get("results", []):
        benchmarks[entry["benchmark"]].append(entry)

    overall_wins = defaultdict(int)

    for bench_name in sorted(benchmarks.keys()):
        entries = benchmarks[bench_name]
        entries.sort(key=lambda x: x["metrics"]["total_ops_per_sec"], reverse=True)

        md += f"### {bench_name}\n\n"
        md += "| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |\n"
        md += "|-----------|-------------|-------------|------------|---------------|----------|----------|----------|\n"

        winner = entries[0]["allocator"] if entries else None
        for entry in entries:
            alloc = entry["allocator"]
            m = entry["metrics"]
            marker = " **(Winner)**" if alloc == winner else ""
            p50 = format_number(m.get("p50_alloc_time_ns"), "ns") if m.get("p50_alloc_time_ns") else "N/A"
            p99 = format_number(m.get("p99_alloc_time_ns"), "ns") if m.get("p99_alloc_time_ns") else "N/A"
            peak_rss = format_bytes(m.get("peak_rss_kb", 0) * 1024)

            md += f"| {alloc}{marker} | {format_number(m['total_ops_per_sec'])} | {format_number(m['alloc_ops_per_sec'])} | {format_number(m['free_ops_per_sec'])} | {format_number(m['avg_alloc_time_ns'], 'ns')} | {p50} | {p99} | {peak_rss} |\n"

        if winner:
            overall_wins[winner] += 1
        md += "\n"

    if overall_wins:
        overall_winner = max(overall_wins.keys(), key=lambda x: overall_wins[x])
        md += "### Overall Summary\n\n"
        md += f"**Overall Winner: {overall_winner}** (won {overall_wins[overall_winner]} benchmarks)\n\n"
        wins_list = sorted(overall_wins.items(), key=lambda x: x[1], reverse=True)
        md += "| Allocator | Wins |\n|-----------|------|\n"
        for alloc, wins in wins_list:
            md += f"| {alloc} | {wins} |\n"
        md += "\n"

    return md

def generate_plots(graph_data: list[dict], plots_dir: Path) -> str:
    if not HAS_MATPLOTLIB:
        return "\n*Matplotlib not available, skipping plots*\n"

    plots_dir.mkdir(parents=True, exist_ok=True)
    md = "## Performance plots\n\n"

    for g in graph_data:
        bench_name = g["benchmark"]
        iterations = g["iterations"]
        allocators = g["allocators"]
        metrics = g["metrics"]

        md += f"### {bench_name}\n\n"

        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle(f"{bench_name} - Performance vs Iterations", fontsize=14)

        plot_configs = [
            ("total_ops_per_sec", "Total Ops/sec", "Ops/sec"),
            ("alloc_ops_per_sec", "Alloc Ops/sec", "Ops/sec"),
            ("avg_alloc_time_ns", "Avg Alloc Time", "Nanoseconds"),
            ("peak_rss_kb", "Peak RSS", "KB"),
        ]

        for idx, (metric_key, title, ylabel) in enumerate(plot_configs):
            ax = axes[idx // 2, idx % 2]
            if metric_key not in metrics:
                ax.text(0.5, 0.5, "N/A", ha='center', va='center')
                ax.set_title(title)
                continue

            data = metrics[metric_key]
            for alloc in allocators:
                if alloc in data:
                    vals = data[alloc]
                    ax.plot(iterations, vals, marker='o', label=alloc, linewidth=2, markersize=4)

            ax.set_xlabel("Iterations")
            ax.set_ylabel(ylabel)
            ax.set_title(title)
            ax.legend()
            ax.grid(True, alpha=0.3)
            ax.set_xscale('log')

        plt.tight_layout()
        plot_path = plots_dir / f"{bench_name}.png"
        plt.savefig(plot_path, dpi=150, bbox_inches='tight')
        plt.close()

        rel_path = plot_path.relative_to(Path(__file__).parent).as_posix()
        md += f"![{bench_name}]({rel_path})\n\n"

        fastest = {}
        for metric_key in ["total_ops_per_sec", "alloc_ops_per_sec"]:
            if metric_key in metrics:
                data = metrics[metric_key]
                best_alloc = max(allocators, key=lambda a: sum(data.get(a, [0])))
                fastest[metric_key] = best_alloc

        if fastest:
            md += "**Best performer:** "
            md += ", ".join([f"{k}: {v}" for k, v in fastest.items()])
            md += "\n\n"

    return md

def generate_threaded_analysis(data: dict, plots_dir: Path) -> str:
    md = "## Thread scaling\n\n"

    threaded = defaultdict(dict)
    for entry in data.get("results", []):
        bench = entry["benchmark"]
        if bench.startswith("threaded_alloc_"):
            thread_count = entry["metrics"].get("thread_count", 1)
            alloc = entry["allocator"]
            ops = entry["metrics"]["total_ops_per_sec"]
            threaded[thread_count][alloc] = ops

    if not threaded:
        return md + "*No threaded benchmark data available*\n\n"

    thread_counts = sorted(threaded.keys())
    allocators = set()
    for tc in thread_counts:
        allocators.update(threaded[tc].keys())
    allocators = sorted(allocators)

    md += "| Threads | " + " | ".join(allocators) + " | Best |\n"
    md += "|---------|" + "|".join(["--------"] * (len(allocators) + 1)) + "|\n"

    for tc in thread_counts:
        row = [str(tc)]
        best_alloc = max(allocators, key=lambda a: threaded[tc].get(a, 0))
        for alloc in allocators:
            ops = threaded[tc].get(alloc, 0)
            marker = " **" if alloc == best_alloc else ""
            row.append(f"{format_number(ops)}{marker}")
        row.append(best_alloc)
        md += "| " + " | ".join(row) + " |\n"

    md += "\n"

    if HAS_MATPLOTLIB and len(thread_counts) > 1:
        plots_dir.mkdir(parents=True, exist_ok=True)
        fig, ax = plt.subplots(figsize=(10, 6))
        for alloc in allocators:
            ops_vals = [threaded[tc].get(alloc, 0) for tc in thread_counts]
            ax.plot(thread_counts, ops_vals, marker='o', label=alloc, linewidth=2)

        ax.set_xlabel("Thread Count")
        ax.set_ylabel("Total Ops/sec")
        ax.set_title("Thread Scaling Performance")
        ax.legend()
        ax.grid(True, alpha=0.3)

        plot_path = plots_dir / "thread_scaling.png"
        plt.savefig(plot_path, dpi=150, bbox_inches='tight')
        plt.close()

        rel_path = plot_path.relative_to(Path(__file__).parent).as_posix()
        md += f"![Thread Scaling]({rel_path})\n\n"

    return md

def main():
    parser = argparse.ArgumentParser(description="Run allocator benchmarks and generate report")
    parser.add_argument("--skip-run", action="store_true", help="Skip running benchmarks")
    args = parser.parse_args()

    platform_name = get_platform_name()
    plots_dir = PLOTS_BASE_DIR / platform_name

    print("=" * 60)
    print(f"Platform: {platform_name}")
    print("=" * 60)

    if not args.skip_run:
        print("\n[1/4] Running standard benchmarks...")
        run_benchmarks(graph_mode=False)

        print("\n[2/4] Running graph mode benchmarks...")
        run_benchmarks(graph_mode=True)
    else:
        print("\n[1/4] Skipping benchmark run (--skip-run)")
        print("[2/4] Skipping graph mode run (--skip-run)")

    print("\n[3/4] Loading results...")
    benchmark_data = load_latest_benchmark_json()
    graph_data = load_graph_jsons()

    print("\n[4/4] Generating report...")

    md = f"## Benchmark results ({platform_name})\n\n"

    md += "## Tested allocators\n\n"
    allocators = set()
    for entry in benchmark_data.get("results", []):
        allocators.add(entry["allocator"])
    md += ", ".join(sorted(allocators)) + "\n\n"

    md += generate_benchmark_tables(benchmark_data)
    md += generate_threaded_analysis(benchmark_data, plots_dir)
    md += generate_plots(graph_data, plots_dir)

    with open(OUTPUT_FILE, "w") as f:
        f.write(md)

    print("\n" + "=" * 60)
    print(md)
    print("=" * 60)
    print(f"\nReport saved to: {OUTPUT_FILE}")
    if HAS_MATPLOTLIB:
        print(f"Plots saved to: {plots_dir}")

if __name__ == "__main__":
    main()
