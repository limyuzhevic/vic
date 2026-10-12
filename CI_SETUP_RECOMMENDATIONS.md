#!/usr/bin/env python3
"""
CI/CD Setup Recommendations for NLM Python Bindings

This document provides comprehensive CI/CD setup recommendations for
automating testing of the NLM Python bindings.
"""

import os
from pathlib import Path


def create_github_actions_workflow():
    """
    Create GitHub Actions workflow configuration.
    
    Returns:
        Dict: GitHub Actions workflow configuration
    """
    workflows = {
        "tests.yml": {
            "name": "Run Tests",
            "on": {
                "push": {
                    "branches": ["main", "develop"],
                    "tags": ["v*"],
                },
                "pull_request": {
                    "branches": ["main", "develop"],
                },
                "schedule": [
                    {
                        "cron": "0 2 * * *",  # Daily at 2 AM UTC
                    },
                ],
            },
            "jobs": {
                "test": {
                    "name": "Run Test Suite",
                    "runs-on": "ubuntu-latest",
                    "strategy": {
                        "matrix": {
                            "python-version": ["3.8", "3.9", "3.10", "3.11", "3.12"],
                        },
                    },
                    "steps": [
                        {
                            "name": "Checkout code",
                            "uses": "actions/checkout@v3",
                        },
                        {
                            "name": "Set up Python ${{ matrix.python-version }}",
                            "uses": "actions/setup-python@v3",
                            "with": {
                                "python-version": "${{ matrix.python-version }}",
                            },
                        },
                        {
                            "name": "Install system dependencies",
                            "run": "|
                                sudo apt-get update
                                sudo apt-get install -y \
                                    build-essential \
                                    cmake \
                                    pkg-config \
                                    libpython3-dev \
                                    python3-dev
                            ",
                        },
                        {
                            "name": "Install Python dependencies",
                            "run": "|
                                python -m pip install --upgrade pip
                                pip install pytest>=7.0
                                pip install numpy>=1.20
                                pip install psutil>=5.9.0
                            ",
                        },
                        {
                            "name": "Build C++ bindings",
                            "run": "|
                                mkdir -p build
                                cd build
                                cmake .. -DCMAKE_BUILD_TYPE=Release
                                make -j4
                            ",
                        },
                        {
                            "name": "Install pynlm package",
                            "run": "|
                                cd python
                                pip install -e .
                            ",
                        },
                        {
                            "name": "Run unit tests",
                            "run": "|
                                pytest tests/test_pynlm_basic.py tests/test_pynlm_config.py tests/test_pynlm_error_handling.py -v
                            ",
                        },
                        {
                            "name": "Run integration tests",
                            "run": "|
                                pytest tests/test_pynlm_brain_simulation.py tests/test_pynlm_agent_world.py -v
                            ",
                        },
                        {
                            "name": "Run plasticity tests",
                            "run": "|
                                pytest tests/test_pynlm_plasticity.py -v
                            ",
                        },
                        {
                            "name": "Run performance tests",
                            "run": "|
                                pytest tests/test_pynlm_performance.py -v --benchmark-only
                            ",
                            "if": "matrix.python-version == '3.8'",  # Run only on one version
                        },
                        {
                            "name": "Run memory tests",
                            "run": "|
                                pytest tests/test_pynlm_memory.py -v --memory-leak
                            ",
                            "if": "matrix.python-version == '3.8'",  # Run only on one version
                        },
                        {
                            "name": "Generate test reports",
                            "run": "|
                                pytest tests/ --html=reports/test_results.html --json=test_results.json
                            ",
                        },
                        {
                            "name": "Upload test results",
                            "uses": "actions/upload-artifact@v3",
                            "with": {
                                "name": "test-results-${{ matrix.python-version }}",
                                "path": "reports/",
                                "retention-days": 30,
                            },
                        },
                        {
                            "name": "Check for performance regressions",
                            "run": "|
                                python scripts/check_performance_regression.py
                            ",
                            "if": "matrix.python-version == '3.8'",
                        },
                    ],
                },
                "performance-benchmark": {
                    "name": "Run Performance Benchmarks",
                    "runs-on": "ubuntu-latest",
                    "needs": ["test"],
                    "if": "always() and github.event.pull_request == null",  # Only on main branch
                    "steps": [
                        {
                            "name": "Checkout code",
                            "uses": "actions/checkout@v3",
                        },
                        {
                            "name": "Set up Python 3.8",
                            "uses": "actions/setup-python@v3",
                            "with": {
                                "python-version": "3.8",
                            },
                        },
                        {
                            "name": "Install dependencies",
                            "run": "|
                                python -m pip install --upgrade pip
                                pip install pytest pytest-benchmark
                                pip install numpy psutil
                            ",
                        },
                        {
                            "name": "Build C++ bindings",
                            "run": "|
                                mkdir -p build
                                cd build
                                cmake .. -DCMAKE_BUILD_TYPE=Release
                                make -j4
                            ",
                        },
                        {
                            "name": "Install pynlm package",
                            "run": "cd python && pip install -e .",
                        },
                        {
                            "name": "Run performance benchmarks",
                            "run": "|
                                pytest tests/test_pynlm_performance.py -v --benchmark-only --benchmark-save=benchmark_results.json --benchmark-min-time=1
                            ",
                        },
                        {
                            "name": "Compare with baseline",
                            "run": "|
                                python scripts/compare_performance.py benchmark_results.json baseline.json
                            ",
                        },
                        {
                            "name": "Upload benchmark results",
                            "uses": "actions/upload-artifact@v3",
                            "with": {
                                "name": "performance-benchmark",
                                "path": "benchmark_results.json",
                                "retention-days": 30,
                            },
                        },
                    ],
                },
                "memory-leak-detection": {
                    "name": "Run Memory Leak Detection",
                    "runs-on": "ubuntu-latest",
                    "needs": ["test"],
                    "if": "always() and github.event.pull_request == null",
                    "steps": [
                        {
                            "name": "Checkout code",
                            "uses": "actions/checkout@v3",
                        },
                        {
                            "name": "Set up Python 3.8",
                            "uses": "actions/setup-python@v3",
                            "with": {
                                "python-version": "3.8",
                            },
                        },
                        {
                            "name": "Install dependencies",
                            "run": "|
                                python -m pip install --upgrade pip
                                pip install pytest pytest-memory-profiler
                                pip install numpy psutil
                            ",
                        },
                        {
                            "name": "Build C++ bindings",
                            "run": "|
                                mkdir -p build
                                cd build
                                cmake .. -DCMAKE_BUILD_TYPE=Release
                                make -j4
                            ",
                        },
                        {
                            "name": "Install pynlm package",
                            "run": "cd python && pip install -e .",
                        },
                        {
                            "name": "Run memory leak detection",
                            "run": "|
                                pytest tests/test_pynlm_memory.py -v --memory-leak
                            ",
                        },
                        {
                            "name": "Generate memory report",
                            "run": "|
                                python scripts/generate_memory_report.py
                            ",
                        },
                        {
                            "name": "Upload memory report",
                            "uses": "actions/upload-artifact@v3",
                            "with": {
                                "name": "memory-report",
                                "path": "reports/memory/",
                                "retention-days": 30,
                            },
                        },
                    ],
                },
            },
        },
        "performance.yml": {
            "name": "Performance Testing",
            "on": {
                "push": {
                    "branches": ["main"],
                    "tags": ["v*"],
                },
                "schedule": [
                    {
                        "cron": "0 6 * * 1",  # Weekly on Monday at 6 AM UTC
                    },
                ],
            },
            "jobs": {
                "benchmark": {
                    "name": "Performance Benchmark",
                    "runs-on": "ubuntu-latest",
                    "steps": [
                        {
                            "name": "Checkout code",
                            "uses": "actions/checkout@v3",
                        },
                        {
                            "name": "Set up Python 3.8",
                            "uses": "actions/setup-python@v3",
                            "with": {
                                "python-version": "3.8",
                            },
                        },
                        {
                            "name": "Install dependencies",
                            "run": "|
                                python -m pip install --upgrade pip
                                pip install pytest pytest-benchmark memory_profiler
                                pip install numpy psutil
                            ",
                        },
                        {
                            "name": "Build C++ bindings",
                            "run": "|
                                mkdir -p build
                                cd build
                                cmake .. -DCMAKE_BUILD_TYPE=Release
                                make -j4
                            ",
                        },
                        {
                            "name": "Install pynlm package",
                            "run": "cd python && pip install -e .",
                        },
                        {
                            "name": "Run comprehensive benchmarks",
                            "run": "|
                                pytest tests/ -k "benchmark" -v --benchmark-only --benchmark-save=weekly_benchmarks.json
                            ",
                        },
                        {
                            "name": "Compare with historical data",
                            "run": "|
                                python scripts/compare_weekly_benchmarks.py weekly_benchmarks.json historical_benchmarks.json
                            ",
                        },
                    ],
                },
            },
        },
    }

    return workflows


def create_local_ci_scripts():
    """
    Create local CI scripts for development.
    
    Returns:
        Dict: Local CI scripts
    """
    scripts = {
        "run_all_tests.py": '''#!/usr/bin/env python3
"""
Run all tests for NLM Python bindings.
"""
import subprocess
import sys
import os
from pathlib import Path

def run_tests():
    """Run all tests and provide comprehensive reporting."""
    print("=" * 60)
    print("NLM Python Bindings - Complete Test Suite")
    print("=" * 60)
    
    # Test categories
    test_categories = [
        ("Unit Tests", [
            "tests/test_pynlm_basic.py",
            "tests/test_pynlm_config.py",
            "tests/test_pynlm_error_handling.py",
        ]),
        ("Integration Tests", [
            "tests/test_pynlm_brain_simulation.py",
            "tests/test_pynlm_agent_world.py",
        ]),
        ("Plasticity Tests", [
            "tests/test_pynlm_plasticity.py",
        ]),
        ("Performance Tests", [
            "tests/test_pynlm_performance.py",
        ]),
        ("Memory Tests", [
            "tests/test_pynlm_memory.py",
        ]),
    ]
    
    total_tests = 0
    passed_tests = 0
    failed_tests = 0
    
    for category, test_files in test_categories:
        print(f"\n{category}:")
        print("-" * 40)
        
        category_tests = 0
        category_passed = 0
        category_failed = 0
        
        for test_file in test_files:
            if not os.path.exists(test_file):
                print(f"  Skipping {test_file} (file not found)")
                continue
                
            print(f"  Running {test_file}...")
            
            try:
                result = subprocess.run([
                    sys.executable, "-m", "pytest",
                    test_file,
                    "-v",
                    "--tb=short",
                ], capture_output=True, text=True, cwd=os.getcwd())
                
                category_tests += 1
                total_tests += 1
                
                if result.returncode == 0:
                    category_passed += 1
                    passed_tests += 1
                    print(f"    ✓ PASSED ({result.stdout.count('PASSED')} tests)")
                else:
                    category_failed += 1
                    failed_tests += 1
                    print(f"    ✗ FAILED")
                    print(f"      Output: {result.stdout[:200]}...")
                    
            except Exception as e:
                category_failed += 1
                failed_tests += 1
                print(f"    ✗ ERROR: {str(e)}")
        
        print(f"\n  {category} Summary:")
        print(f"    Tests: {category_tests}")
        print(f"    Passed: {category_passed}")
        print(f"    Failed: {category_failed}")
    
    print("\n" + "=" * 60)
    print("Overall Summary")
    print("=" * 60)
    print(f"Total Tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {failed_tests}")
    print(f"Success Rate: {passed_tests/total_tests*100:.1f}%")
    
    if failed_tests > 0:
        print(f"\n{failed_tests} tests failed.")
        return 1
    else:
        print(f"\nAll {total_tests} tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(run_tests())
''',

        "check_performance_regression.py": '''#!/usr/bin/env python3
"""
Check for performance regressions in NLM Python bindings.
"""
import json
import os
import sys
from pathlib import Path
import argparse

def load_benchmark_results(filepath):
    """Load benchmark results from JSON file."""
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Warning: Benchmark file not found: {filepath}")
        return {}
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in benchmark file: {e}")
        return {}

def calculate_regression(current, baseline, threshold=0.1):
    """
    Calculate performance regression between current and baseline.
    
    Args:
        current: Current benchmark results
        baseline: Baseline benchmark results
        threshold: Regression threshold (10% = 0.1)
    
    Returns:
        Dict: Regression analysis
    """
    regressions = {}
    
    for test_name in current:
        if test_name in baseline:
            current_time = current[test_name].get('avg_time', 0)
            baseline_time = baseline[test_name].get('avg_time', 0)
            
            if baseline_time > 0 and current_time > 0:
                regression = (current_time - baseline_time) / baseline_time
                regressions[test_name] = {
                    'current_time': current_time,
                    'baseline_time': baseline_time,
                    'regression': regression,
                    'significant': regression > threshold,
                }
    
    return regressions

def generate_report(regressions, output_file):
    """
    Generate performance regression report.
    
    Args:
        regressions: Regression analysis results
        output_file: Output file path
    """
    with open(output_file, 'w') as f:
        f.write("Performance Regression Report\n")
        f.write("=" * 50 + "\n\n")
        
        if not regressions:
            f.write("No performance regressions detected.\n")
            return
        
        significant = {k: v for k, v in regressions.items() if v['significant']}
        
        if significant:
            f.write(f"Significant performance regressions detected:\n")
            f.write(f"{'Test Name':<30} {'Current (s)':<15} {'Baseline (s)':<15} {'Regression':<12} {'Status':<10}\n")
            f.write("-" * 80 + "\n")
            
            for test_name, data in significant.items():
                f.write(f"{test_name:<30} {data['current_time']:<15.4f} {data['baseline_time']:<15.4f} "
                       f"{data['regression']:<12.2%} {'REGRESSION':<10}\n")
        
        # Summary
        f.write(f"\nSummary:\n")
        f.write(f"Total tests analyzed: {len(regressions)}\n")
        f.write(f"Tests with regressions: {len(significant)}\n")
        f.write(f"Regression rate: {len(significant)/len(regressions)*100:.1f}%\n")
        
        if significant:
            f.write(f"\nWARNING: Performance regressions detected. Please review.\n")
            return 1
        else:
            f.write(f"\nOK: No significant performance regressions detected.\n")
            return 0

def main():
    parser = argparse.ArgumentParser(description="Check for performance regressions")
    parser.add_argument("current", help="Current benchmark results JSON file")
    parser.add_argument("baseline", help="Baseline benchmark results JSON file")
    parser.add_argument("-o", "--output", help="Output report file", default="regression_report.txt")
    parser.add_argument("-t", "--threshold", help="Regression threshold (0.0-1.0)", type=float, default=0.1)
    
    args = parser.parse_args()
    
    current_results = load_benchmark_results(args.current)
    baseline_results = load_benchmark_results(args.baseline)
    
    if not current_results or not baseline_results:
        print("Error: Could not load benchmark results")
        return 1
    
    regressions = calculate_regression(current_results, baseline_results, args.threshold)
    
    return generate_report(regressions, args.output)

if __name__ == "__main__":
    sys.exit(main())
''',

        "compare_performance.py": '''#!/usr/bin/env python3
"""
Compare performance benchmarks between runs.
"""
import json
import os
import sys
import argparse
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

def load_benchmark_file(filepath):
    """Load benchmark results from JSON file."""
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: Benchmark file not found: {filepath}")
        return {}
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in benchmark file: {e}")
        return {}

def create_comparison_chart(current, baseline, output_file):
    """Create comparison chart between current and baseline benchmarks."""
    try:
        # Prepare data
        test_names = []
        current_times = []
        baseline_times = []
        
        for test_name in current:
            if test_name in baseline:
                test_names.append(test_name)
                current_times.append(current[test_name].get('avg_time', 0))
                baseline_times.append(baseline[test_name].get('avg_time', 0))
        
        if not test_names:
            print("No common tests found for comparison")
            return
        
        # Create bar chart
        x = np.arange(len(test_names))
        width = 0.35
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
        
        # Time comparison
        bars1 = ax1.bar(x - width/2, baseline_times, width, label='Baseline', color='skyblue')
        bars2 = ax1.bar(x + width/2, current_times, width, label='Current', color='lightcoral')
        
        ax1.set_xlabel('Test Name')
        ax1.set_ylabel('Average Time (seconds)')
        ax1.set_title('Performance Comparison')
        ax1.set_xticks(x)
        ax1.set_xticklabels(test_names, rotation=45, ha='right')
        ax1.legend()
        
        # Regression percentage
        regression_percentages = []
        for current_time, baseline_time in zip(current_times, baseline_times):
            if baseline_time > 0 and current_time > 0:
                regression = (current_time - baseline_time) / baseline_time * 100
                regression_percentages.append(regression)
            else:
                regression_percentages.append(0)
        
        colors = ['green' if r <= 10 else 'yellow' if r <= 25 else 'red' for r in regression_percentages]
        bars3 = ax2.bar(range(len(regression_percentages)), regression_percentages, color=colors)
        
        ax2.set_xlabel('Test Index')
        ax2.set_ylabel('Regression Percentage (%)')
        ax2.set_title('Performance Regression (%)')
        ax2.axhline(y=10, color='green', linestyle='--', alpha=0.5, label='10% threshold')
        ax2.axhline(y=25, color='yellow', linestyle='--', alpha=0.5, label='25% threshold')
        ax2.axhline(y=50, color='red', linestyle='--', alpha=0.5, label='50% threshold')
        ax2.legend()
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"Comparison chart saved to: {output_file}")
        
    except Exception as e:
        print(f"Error creating comparison chart: {e}")

def generate_comparison_report(current, baseline, output_file):
    """Generate detailed comparison report."""
    with open(output_file, 'w') as f:
        f.write("Performance Benchmark Comparison Report\n")
        f.write("=" * 60 + "\n\n")
        
        # Summary
        common_tests = [t for t in current if t in baseline]
        f.write(f"Summary:\n")
        f.write(f"  Total tests analyzed: {len(common_tests)}\n")
        f.write(f"  Current run tests: {len(current)}\n")
        f.write(f"  Baseline tests: {len(baseline)}\n\n")
        
        if not common_tests:
            f.write("No common tests found for comparison.\n")
            return
        
        # Detailed comparison
        f.write(f"Detailed Comparison:\n")
        f.write(f"{'Test Name':<30} {'Baseline (s)':<15} {'Current (s)':<15} {'Regression':<12} {'Status':<10}\n")
        f.write("-" * 85 + "\n")
        
        for test_name in common_tests:
            baseline_time = baseline[test_name].get('avg_time', 0)
            current_time = current[test_name].get('avg_time', 0)
            
            if baseline_time > 0 and current_time > 0:
                regression = (current_time - baseline_time) / baseline_time
                status = "GOOD" if regression <= 0.1 else "WARNING" if regression <= 0.25 else "BAD"
                
                f.write(f"{test_name:<30} {baseline_time:<15.4f} {current_time:<15.4f} "
                       f"{regression:<12.2%} {status:<10}\n")
        
        # Statistics
        regressions = []
        for test_name in common_tests:
            baseline_time = baseline[test_name].get('avg_time', 0)
            current_time = current[test_name].get('avg_time', 0)
            
            if baseline_time > 0 and current_time > 0:
                regression = (current_time - baseline_time) / baseline_time
                regressions.append(regression)
        
        if regressions:
            avg_regression = np.mean(regressions)
            max_regression = np.max(regressions)
            
            f.write(f"\nRegression Statistics:\n")
            f.write(f"  Average regression: {avg_regression*100:.1f}%\n")
            f.write(f"  Maximum regression: {max_regression*100:.1f}%\n")
            f.write(f"  Tests improved: {sum(1 for r in regressions if r < 0)}\n")
            f.write(f"  Tests degraded: {sum(1 for r in regressions if r > 0)}\n\n")
        
        # Recommendations
        f.write(f"Recommendations:\n")
        significant_regressions = [r for r in regressions if r > 0.1]
        if significant_regressions:
            f.write(f"  ⚠️  {len(significant_regressions)} tests show significant performance degradation (>")
            f.write(f"10%). Please investigate:\n")
            for test_name in common_tests:
                baseline_time = baseline[test_name].get('avg_time', 0)
                current_time = current[test_name].get('avg_time', 0)
                if baseline_time > 0 and current_time > 0:
                    regression = (current_time - baseline_time) / baseline_time
                    if regression > 0.1:
                        f.write(f"    - {test_name}: {(regression*100):.1f}% regression\n")
        else:
            f.write(f"  ✓ All tests within acceptable performance thresholds.\n")

def main():
    parser = argparse.ArgumentParser(description="Compare performance benchmarks")
    parser.add_argument("current", help="Current benchmark results JSON file")
    parser.add_argument("baseline", help="Baseline benchmark results JSON file")
    parser.add_argument("-o", "--output", help="Output directory", default="reports/comparison")
    
    args = parser.parse_args()
    
    current_results = load_benchmark_file(args.current)
    baseline_results = load_benchmark_file(args.baseline)
    
    if not current_results or not baseline_results:
        print("Error: Could not load benchmark results")
        return 1
    
    # Create output directory
    os.makedirs(args.output, exist_ok=True)
    
    # Generate reports
    chart_file = os.path.join(args.output, "comparison_chart.png")
    report_file = os.path.join(args.output, "comparison_report.txt")
    
    create_comparison_chart(current_results, baseline_results, chart_file)
    generate_comparison_report(current_results, baseline_results, report_file)
    
    print(f"Comparison completed successfully!")
    print(f"Chart saved to: {chart_file}")
    print(f"Report saved to: {report_file}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
''',

        "generate_memory_report.py": '''#!/usr/bin/env python3
"""
Generate memory usage report for NLM Python bindings.
"""
import json
import os
import sys
import argparse
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
from datetime import datetime

def load_memory_results(filepath):
    """Load memory test results from JSON file."""
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: Memory results file not found: {filepath}")
        return {}
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in memory results file: {e}")
        return {}

def create_memory_usage_chart(memory_results, output_file):
    """Create memory usage chart."""
    try:
        # Extract memory data
        test_names = []
        peak_memories = []
        
        for test_result in memory_results.get('test_results', []):
            test_names.append(test_result.get('test_name', 'Unknown'))
            peak_memories.append(test_result.get('peak_memory', 0))
        
        if not test_names:
            print("No memory data found for chart")
            return
        
        # Create bar chart
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
        
        # Memory usage bar chart
        colors = ['skyblue', 'lightgreen', 'salmon', 'gold', 'plum']
        bars = ax1.bar(range(len(peak_memories)), peak_memories, color=colors)
        
        ax1.set_xlabel('Test Number')
        ax1.set_ylabel('Peak Memory (bytes)')
        ax1.set_title('Memory Usage by Test')
        ax1.set_xticks(range(len(peak_memories)))
        ax1.set_xticklabels([f"Test {i+1}" for i in range(len(peak_memories))])
        
        # Add value labels on bars
        for bar, memory in zip(bars, peak_memories):
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height,
                    f'{memory/1024/1024:.1f} MB',
                    ha='center', va='bottom')
        
        # Memory growth line chart
        if 'growth_trend' in memory_results:
            growth_trend = memory_results['growth_trend']
            iterations = list(range(len(growth_trend)))
            
            ax2.plot(iterations, growth_trend, 'b-', linewidth=2, marker='o', markersize=4)
            ax2.set_xlabel('Iteration')
            ax2.set_ylabel('Memory Growth (bytes)')
            ax2.set_title('Memory Growth Trend')
            ax2.grid(True, alpha=0.3)
            
            # Add trend line
            if len(growth_trend) > 1:
                z = np.polyfit(iterations, growth_trend, 1)
                p = np.poly1d(z)
                ax2.plot(iterations, p(iterations), "r--", alpha=0.8, label=f'Trend: {z[0]:.2f}x + {z[1]:.0f}')
                ax2.legend()
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"Memory usage chart saved to: {output_file}")
        
    except Exception as e:
        print(f"Error creating memory usage chart: {e}")

def generate_memory_report(memory_results, output_file):
    """Generate detailed memory usage report."""
    with open(output_file, 'w') as f:
        f.write("Memory Usage Report\n")
        f.write("=" * 50 + "\n")
        f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        if not memory_results:
            f.write("No memory results available.\n")
            return
        
        # Executive summary
        f.write("Executive Summary:\n")
        f.write("-" * 20 + "\n")
        
        test_count = len(memory_results.get('test_results', []))
        peak_memory = memory_results.get('peak_memory', 0)
        avg_memory = memory_results.get('avg_memory', 0)
        memory_growth = memory_results.get('memory_growth', 0)
        
        f.write(f"  Total tests executed: {test_count}\n")
        f.write(f"  Peak memory usage: {peak_memory/1024/1024:.1f} MB\n")
        f.write(f"  Average memory usage: {avg_memory/1024/1024:.1f} MB\n")
        f.write(f"  Memory growth: {memory_growth/1024/1024:.1f} MB\n\n")
        
        # Test results details
        f.write("Test Results Details:\n")
        f.write("-" * 20 + "\n")
        
        test_results = memory_results.get('test_results', [])
        if test_results:
            f.write(f"{'Test #':<8} {'Test Name':<40} {'Peak Memory':<15} {'Status':<10}\n")
            f.write("-" * 80 + "\n")
            
            for i, test in enumerate(test_results, 1):
                test_name = test.get('test_name', f'Test {i}')
                peak_mem = test.get('peak_memory', 0)
                status = test.get('status', 'Unknown')
                
                f.write(f"{i:<8} {test_name:<40} {peak_mem/1024/1024:<15.1f} MB {status:<10}\n")
        
        f.write("\n")
        
        # Memory analysis
        f.write("Memory Analysis:\n")
        f.write("-" * 20 + "\n")
        
        if 'growth_trend' in memory_results:
            growth_trend = memory_results['growth_trend']
            
            if growth_trend:
                initial_memory = growth_trend[0] if growth_trend else 0
                final_memory = growth_trend[-1] if growth_trend else 0
                total_growth = final_memory - initial_memory
                
                f.write(f"  Initial memory: {initial_memory/1024/1024:.1f} MB\n")
                f.write(f"  Final memory: {final_memory/1024/1024:.1f} MB\n")
                f.write(f"  Total growth: {total_growth/1024/1024:.1f} MB\n")
                
                if initial_memory > 0:
                    growth_rate = (total_growth / initial_memory) * 100
                    f.write(f"  Growth rate: {growth_rate:.1f}%\n")
                
                if total_growth > 100 * 1024 * 1024:  # > 100 MB
                    f.write(f"  ⚠️  WARNING: Excessive memory growth detected!\n")
                else:
                    f.write(f"  ✓ Memory growth within acceptable limits.\n")
        
        # Recommendations
        f.write("\nRecommendations:\n")
        f.write("-" * 20 + "\n")
        
        if memory_growth > 200 * 1024 * 1024:  # > 200 MB
            f.write("  1. Consider implementing memory optimization techniques\n")
            f.write("  2. Add memory profiling to identify leaks\n")
            f.write("  3. Increase garbage collection frequency\n")
        elif memory_growth > 50 * 1024 * 1024:  # > 50 MB
            f.write("  1. Monitor memory usage over longer test runs\n")
            f.write("  2. Consider memory-efficient data structures\n")
        else:
            f.write("  1. Memory usage is within acceptable limits\n")
            f.write("  2. Continue monitoring for memory leaks\n")
        
        f.write("\n")
        
        # Technical information
        f.write("Technical Information:\n")
        f.write("-" * 20 + "\n")
        
        f.write(f"  Test framework: {memory_results.get('framework', 'pytest')}\n")
        f.write(f"  Python version: {memory_results.get('python_version', 'Unknown')}\n")
        f.write(f"  System memory: {memory_results.get('system_memory', 0)/1024/1024:.1f} MB\n")
        f.write(f"  Process ID: {memory_results.get('process_id', os.getpid())}\n")

def main():
    parser = argparse.ArgumentParser(description="Generate memory usage report")
    parser.add_argument("input", help="Input memory results JSON file")
    parser.add_argument("-o", "--output", help="Output directory", default="reports/memory")
    
    args = parser.parse_args()
    
    memory_results = load_memory_results(args.input)
    
    if not memory_results:
        print("Error: Could not load memory results")
        return 1
    
    # Create output directory
    os.makedirs(args.output, exist_ok=True)
    
    # Generate reports
    chart_file = os.path.join(args.output, "memory_usage.png")
    report_file = os.path.join(args.output, "memory_report.txt")
    
    create_memory_usage_chart(memory_results, chart_file)
    generate_memory_report(memory_results, report_file)
    
    print(f"Memory report generated successfully!")
    print(f"Chart saved to: {chart_file}")
    print(f"Report saved to: {report_file}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
''',

        "scripts/install_dependencies.py": '''#!/usr/bin/env python3
"""
Install all required dependencies for NLM Python bindings testing.
"""
import subprocess
import sys
import os

def run_command(command, description):
    """Run a command and check for errors."""
    print(f"\n{description}...")
    try:
        result = subprocess.run(command, shell=True, check=True, capture_output=True, text=True)
        print(f"✓ {description}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ {description}")
        print(f"  Error: {e}")
        if e.stdout:
            print(f"  stdout: {e.stdout}")
        if e.stderr:
            print(f"  stderr: {e.stderr}")
        return False

def main():
    """Install all required dependencies."""
    print("=" * 60)
    print("NLM Python Bindings - Dependency Installation")
    print("=" * 60)
    
    # Python version check
    python_version = sys.version_info
    if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
        print("Error: Python 3.8 or higher is required")
        return 1
    
    print(f"Python {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    # System dependencies
    system_deps = [
        ("build-essential", "C++ compiler and build tools"),
        ("cmake", "CMake build system"),
        ("pkg-config", "Package configuration tool"),
        ("libpython3-dev", "Python development headers"),
        ("python3-dev", "Python development files"),
    ]
    
    # Install system dependencies (requires sudo)
    print("\nSystem Dependencies:")
    print("-" * 30)
    
    for package, description in system_deps:
        # Note: This requires sudo, so we'll just check if they're available
        # In a real CI environment, these would be installed separately
        print(f"  {package}: {description}")
    
    # Python dependencies
    python_deps = [
        ("pytest>=7.0", "Testing framework"),
        ("numpy>=1.20", "Numerical computing"),
        ("psutil>=5.9.0", "System and process utilities"),
        ("memory_profiler", "Memory profiling tools"),
        ("pytest-benchmark", "Benchmark testing plugin"),
        ("pytest-html", "HTML reporting plugin"),
        ("pytest-json-report", "JSON reporting plugin"),
    ]
    
    print("\nPython Dependencies:")
    print("-" * 30)
    
    for package, description in python_deps:
        success = run_command(f"pip install {package}", description)
        if not success:
            print(f"Warning: Failed to install {package}")
    
    # Install pynlm itself
    print("\nInstalling pynlm package...")
    try:
        # Check if pynlm directory exists
        if os.path.exists("python"):
            os.chdir("python")
            result = subprocess.run([sys.executable, "-m", "pip", "install", "-e", "."], 
                                  capture_output=True, text=True)
            if result.returncode == 0:
                print("✓ pynlm package installed successfully")
            else:
                print(f"✗ Failed to install pynlm: {result.stderr}")
        else:
            print("✗ python directory not found")
    except Exception as e:
        print(f"✗ Error installing pynlm: {e}")
    
    print("\n" + "=" * 60)
    print("Dependency installation completed")
    print("=" * 60)
    print("\nNext steps:")
    print("1. Build the C++ bindings:")
    print("   mkdir -p build")
    print("   cd build")
    print("   cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("   make -j4")
    print("\n2. Run the test suite:")
    print("   python run_all_tests.py")
    print("\n3. Generate test reports:")
    print("   pytest tests/ --html=reports/test_results.html")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
''',

        "setup_guide.md": """# NLM Python Bindings Test Setup Guide

## Overview
This guide provides step-by-step instructions for setting up the testing
environment for NLM Python bindings.

## System Requirements

### Minimum Requirements
- Python 3.8 or higher
- 4 GB RAM
- 2 GB disk space
- C++ compiler (gcc 7.0 or higher)
- CMake 3.16 or higher

### Recommended Requirements
- Python 3.10 or higher
- 8 GB RAM
- 10 GB disk space
- Modern CPU with multiple cores

## Installation Steps

### 1. Clone the Repository
```bash
git clone https://github.com/your-org/nlm.git
cd nlm
```

### 2. Install System Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake pkg-config libpython3-dev python3-dev
```

#### CentOS/RHEL
```bash
sudo yum groupinstall -y "Development Tools"
sudo yum install -y cmake python3-devel
```

#### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake
```

### 3. Install Python Dependencies
```bash
# Upgrade pip
python -m pip install --upgrade pip

# Install testing dependencies
python -m pip install pytest>=7.0 numpy>=1.20 psutil>=5.9.0

# Install additional plugins (optional)
python -m pip install pytest-benchmark pytest-html pytest-json-report memory-profiler
```

### 4. Build C++ Bindings
```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### 5. Install pynlm Package
```bash
# Install the Python bindings
cd python
python -m pip install -e .
```

## Running Tests

### Basic Test Execution
```bash
# Run all tests
pytest tests/ -v --tb=short --timeout=300

# Run only unit tests
pytest tests/ -k "not integration and not performance and not memory" -v

# Run integration tests
pytest tests/ -m "integration" -v

# Run performance tests
pytest tests/ -m "performance" -v

# Run memory tests
pytest tests/ -m "memory" -v
```

### Comprehensive Test Suite
```bash
# Run the comprehensive test suite
python run_all_tests.py
```

### Generate Test Reports
```bash
# Generate HTML reports
pytest tests/ --html=reports/test_results.html --capture=no

# Generate JSON reports
pytest tests/ --json=test_results.json

# Generate combined reports
pytest tests/ --html=reports/test_results.html --json=test_results.json
```

## Advanced Configuration

### Pytest Configuration
Edit `pyproject.toml` to configure pytest:

```ini
[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
addopts = [
    "-v",
    "--tb=short",
    "--timeout=300",
    "--strict-markers",
    "--disable-warnings",
    "-x",
]
markers = [
    "slow(reason): marks tests as slow",
    "integration(reason): marks tests as integration tests",
    "performance(reason): marks tests as performance tests",
    "memory(reason): marks tests as memory tests",
]
```

### Environment Variables
```bash
# Set timeout for individual tests
export PYTEST_TIMEOUT=600

# Set cache directory
export PYTEST_CACHE_DIR=.pytest_cache

# Enable debug mode
export PYTEST_DEBUG=1
```

## Test Categories

### Unit Tests
```bash
# Run unit tests
pytest tests/test_pynlm_basic.py tests/test_pynlm_config.py tests/test_pynlm_error_handling.py -v
```

### Integration Tests
```bash
# Run integration tests
pytest tests/test_pynlm_brain_simulation.py tests/test_pynlm_agent_world.py -v
```

### Performance Tests
```bash
# Run performance tests
pytest tests/test_pynlm_performance.py -v --benchmark-only
```

### Memory Tests
```bash
# Run memory tests
pytest tests/test_pynlm_memory.py -v --memory-leak
```

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'pynlm'"
Solution: Ensure pynlm is properly installed:
```bash
python -c "import pynlm; print(pynlm.__version__)"
```

#### "ImportError: libstdc++.so.6: cannot open shared object file"
Solution: Install required system libraries:
```bash
# Ubuntu
sudo apt-get install -y libstdc++6

# CentOS/RHEL
sudo yum install -y libstdc++
```

#### "CMakeError: CMAKE_CXX_COMPILER not found"
Solution: Install a C++ compiler:
```bash
# Ubuntu
sudo apt-get install -y g++

# CentOS/RHEL
sudo yum groupinstall -y "Development Tools"
```

### Test Failures

#### Memory Issues
```bash
# Clear pytest cache
rm -rf .pytest_cache

# Clear temporary files
rm -f /tmp/test_*.json /tmp/test_*.bin

# Check system memory
free -h
```

#### Performance Issues
```bash
# Check system resources
htop

# Monitor memory usage
watch -n 1 free -h
```

## Maintenance

### Regular Maintenance Tasks
1. **Clean up test artifacts**:
```bash
rm -rf reports/*
rm -rf .pytest_cache
rm -f test_results.json
```

2. **Update dependencies**:
```bash
python -m pip list --outdated | grep -E "pytest|numpy|psutil"
```

3. **Check for memory leaks**:
```bash
pytest tests/test_pynlm_memory.py -v --memory-leak
```

4. **Performance regression testing**:
```bash
python scripts/check_performance_regression.py baseline.json current.json
```

### Backup and Restore
```bash
# Backup test results
cp -r reports/ reports_backup_$(date +%Y%m%d_%H%M%S)

# Restore test results
mkdir -p reports
cp -r reports_backup_*/reports/* reports/
```

## Conclusion

This setup guide provides comprehensive instructions for installing and running
tests for the NLM Python bindings. By following these steps, you'll have a
fully functional testing environment that supports:

1. **Comprehensive test coverage** across all functionality areas
2. **Performance and memory testing** for reliability assurance
3. **Integration testing** for component interactions
4. **Error handling testing** for robustness validation
5. **Scalability testing** for production readiness

For additional support, refer to the individual test file documentation
or contact the development team.
""",

        "running_tests.md": """# How to Run NLM Python Bindings Tests

## Overview
This document provides detailed instructions for running different types
of tests for the NLM Python bindings.

## Test Execution Commands

### 1. Run All Tests
```bash
# Basic test execution
pytest tests/ -v --tb=short --timeout=300

# With HTML reporting
pytest tests/ --html=reports/test_results.html --capture=no

# With JSON reporting
pytest tests/ --json=test_results.json

# Combined with both HTML and JSON
pytest tests/ --html=reports/test_results.html --json=test_results.json
```

### 2. Run Unit Tests
```bash
# Run only unit tests (fast, isolated tests)
pytest tests/test_pynlm_basic.py tests/test_pynlm_config.py tests/test_pynlm_error_handling.py -v

# Run unit tests with markers
pytest tests/ -k "not integration and not performance and not memory" -v

# Run unit tests with specific categories
pytest tests/ -m "slow" -v  # Run slow unit tests
```

### 3. Run Integration Tests
```bash
# Run integration tests
pytest tests/test_pynlm_brain_simulation.py tests/test_pynlm_agent_world.py -v

# Run integration tests with markers
pytest tests/ -m "integration" -v

# Run integration tests with specific scenarios
pytest tests/test_pynlm_agent_world.py::TestAgentWorldSimulation -v
```

### 4. Run Performance Tests
```bash
# Run performance tests
pytest tests/test_pynlm_performance.py -v --benchmark-only

# Run performance tests with markers
pytest tests/ -m "performance" -v

# Run specific performance benchmarks
pytest tests/test_pynlm_performance.py::TestPerformanceBenchmarks -v
```

### 5. Run Memory Tests
```bash
# Run memory tests
pytest tests/test_pynlm_memory.py -v --memory-leak

# Run memory tests with markers
pytest tests/ -m "memory" -v

# Run specific memory tests
pytest tests/test_pynlm_memory.py::TestMemoryUsage -v
```

### 6. Run Plasticity Tests
```bash
# Run plasticity tests
pytest tests/test_pynlm_plasticity.py -v

# Run specific plasticity tests
pytest tests/test_pynlm_plasticity.py::TestPlasticityMechanisms -v
```

### 7. Run Comprehensive Test Suite
```bash
# Run the comprehensive test suite (Python script)
python run_all_tests.py

# Or use pytest directly
pytest tests/ -v --tb=short --timeout=300
```

## Test Categories

### Unit Tests
- **Location**: `tests/test_pynlm_basic.py`, `tests/test_pynlm_config.py`, `tests/test_pynlm_error_handling.py`
- **Description**: Fast, isolated tests for individual components
- **Characteristics**: 
  - Test specific functions and classes
  - Mock external dependencies
  - Fast execution time
  - High test coverage per component

### Integration Tests
- **Location**: `tests/test_pynlm_brain_simulation.py`, `tests/test_pynlm_agent_world.py`
- **Description**: Tests for component interactions
- **Characteristics**:
  - Test multiple components working together
  - Simulate real-world scenarios
  - Longer execution time
  - Lower test isolation

### Performance Tests
- **Location**: `tests/test_pynlm_performance.py`
- **Description**: Performance benchmarks and scalability tests
- **Characteristics**:
  - Measure execution time
  - Test memory usage
  - Validate performance under load
  - Establish baseline metrics

### Memory Tests
- **Location**: `tests/test_pynlm_memory.py`
- **Description**: Memory usage and leak detection tests
- **Characteristics**:
  - Monitor memory allocation
  - Detect memory leaks
  - Test memory efficiency
  - Validate cleanup

### Plasticity Tests
- **Location**: `tests/test_pynlm_plasticity.py`
- **Description**: Learning and plasticity mechanism tests
- **Characteristics**:
  - Test plasticity algorithms
  - Validate learning behavior
  - Test synaptic changes
  - Simulate learning scenarios

## Advanced Test Commands

### Run Tests with Specific Markers
```bash
# Run slow tests (requires marker)
pytest tests/ -m "slow" -v

# Run regression tests
pytest tests/ -m "regression" -v

# Run benchmark tests
pytest tests/ -m "benchmark" -v
```

### Run Tests with Timeouts
```bash
# Set custom timeout
pytest tests/ --timeout=600

# Skip timeout tests
pytest tests/ --timeout=0
```

### Run Tests with Caching
```bash
# Enable pytest cache
pytest tests/ --cache-show

# Clear pytest cache
pytest --cache-clear
```

### Run Tests with Parallel Execution
```bash
# Run tests in parallel (use with caution)
pytest tests/ -n auto -v

# Run tests with specific number of workers
pytest tests/ -n 4 -v
```

## Test Results Analysis

### HTML Report
```bash
# Open HTML report in browser
pytest tests/ --html=reports/test_results.html

# View specific test results
pytest tests/ --html=reports/test_results.html --json=test_results.json
```

### JSON Report
```bash
# Generate JSON report
pytest tests/ --json=test_results.json

# Analyze JSON report
python -c "import json; data = json.load(open('test_results.json')); print(f'Tests: {len(data[\"report\"][\"tests\"])}')"
```

### Performance Comparison
```bash
# Compare with baseline
python scripts/compare_performance.py baseline.json current.json

# Generate performance report
python scripts/compare_performance.py baseline.json current.json -o reports/comparison
```

## Test Failure Analysis

### Debug Test Failures
```bash
# Run specific test with debug output
pytest tests/test_pynlm_basic.py::TestBasicFunctionality::test_basic_object_creation -v -s

# Run test with full traceback
pytest tests/test_pynlm_basic.py::TestBasicFunctionality::test_basic_object_creation -v --tb=long

# Run test with capture disabled
pytest tests/test_pynlm_basic.py::TestBasicFunctionality::test_basic_object_creation -v -s
```

### Memory Leak Detection
```bash
# Run memory leak detection
pytest tests/test_pynlm_memory.py -v --memory-leak

# Generate memory report
python scripts/generate_memory_report.py memory_results.json
```

### Performance Regression Detection
```bash
# Check for performance regressions
python scripts/check_performance_regression.py baseline.json current.json
```

## Continuous Integration

### GitHub Actions
```yaml
# .github/workflows/tests.yml
name: Run Tests

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, 3.10, 3.11, 3.12]
    
    steps:
    - uses: actions/checkout@v3
    - uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install pytest>=7.0 numpy>=1.20 psutil>=5.9.0
    
    - name: Build C++ bindings
      run: |
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    
    - name: Install pynlm
      run: cd python && pip install -e .
    
    - name: Run tests
      run: pytest tests/ -v --tb=short --timeout=300
```

### Local Development
```bash
# Install dependencies locally
python -m pip install -r requirements.txt

# Run tests in development mode
pytest tests/ -v --tb=short --timeout=300 --cache-clear

# Watch for test changes (development)
pytest --version
watchmedit -e MODIFY test_*.py python -m pytest
```

## Best Practices

### Writing New Tests
1. **Isolation**: Each test should be independent
2. **Documentation**: Include clear docstrings
3. **Error Handling**: Test both success and failure cases
4. **Performance**: Avoid performance-heavy tests in standard suites
5. **Memory**: Monitor memory usage and cleanup

### Running Tests Regularly
1. **Daily**: Run basic unit tests
2. **Weekly**: Run integration and performance tests
3. **Monthly**: Run comprehensive test suite with memory leak detection

### Test Maintenance
1. **Update tests** when functionality changes
2. **Monitor test performance** regularly
3. **Clean up temporary files** and caches
4. **Update documentation** when test behavior changes

## Conclusion

This document provides comprehensive instructions for running tests for the
NLM Python bindings. By following these guidelines, you'll be able to:

1. **Run all tests** with comprehensive reporting
2. **Run specific test categories** based on needs
3. **Analyze test results** and identify issues
4. **Set up continuous integration** for automated testing
5. **Maintain test quality** through regular execution

For additional support, refer to individual test files or contact the
development team.
""",
    }

    return scripts


def create_ci_configuration():
    """
    Create CI configuration files.
    
    Returns:
        Dict: CI configuration files
    """
    ci_config = {
        "pre-commit-config.yaml": """# Pre-commit configuration for NLM Python bindings

repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-ast
      - id: check-builtin-deps
      - id: check-case-conflict
      - id: check-docstring-first
      - id: check-executables
      - id: check-json
      - id: check-merge-conflict
      - id: check-symlinks
      - id: check-toml
      - id: check-vcs-permalinks
      - id: check-xml
      - id: check-yaml
      - id: debug-statements
      - id: detect-private-key
      - id: end-of-file-fixer
      - id: fix-encoding-charset
      - id: mixed-line-ending
      - id: trailing-whitespace

  - repo: https://github.com/psf/black
    rev: 23.3.0
    hooks:
      - id: black
        args: [--check, --diff]

  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort
        args: [--check-only, --diff]

  - repo: https://github.com/PyCQA/flake8
    rev: 6.0.0
    hooks:
      - id: flake8

  - repo: https://github.com/asottile/pyupgrade
    rev: v3.3.1
    hooks:
      - id: pyupgrade
        args: [--py38-plus]

  - repo: https://github.com/executablebooks/MyPy
    rev: v1.3.0
    hooks:
      - id: mypy
        args: [--strict, pynlm]

  - repo: https://github.com/python-poetry/poetry
    rev: 1.5.1
    hooks:
      - id: poetry-check
      - id: poetry-lock
      - id: poetry-export
        args: [--without-hashes, --format, requirements.txt, --output, requirements.txt]

  - repo: local
    hooks:
      - id: run-tests
        name: Run tests
        entry: python run_all_tests.py
        language: system
        pass_filenames: false
        always_run: true
""",

        ".flake8": """# Flake8 configuration for NLM Python bindings

[flake8]
max-line-length = 88
extend-ignore = E203, W503
exclude = 
    .git,
    __pycache__,
    build/,
    dist/,
    *.egg-info/,
    .pytest_cache/,
    reports/,
    docs/source/build/,
    */__pycache__/*,
    *test*.py,
    scripts/*.py,
    */test_*.py,
    */tests/*.py,

per-file-ignores = 
    */test_*.py: S101
    scripts/*.py: S101
""",

        ".editorconfig": """# EditorConfig configuration for NLM Python bindings

root = true

[*]
charset = utf-8
end_of_line = lf
indent_size = 4
indent_style = space
insert_final_newline = true
max_line_length = 88
tab_width = 4
trim_trailing_whitespace = true

[*.md]
max_line_length = 0

[*.py]
max_line_length = 88

[*.yml]
max_line_length = 100

[*.json]
indent = 2
max_line_length = 100
""",

        "requirements.txt": """# Python dependencies for NLM Python bindings testing
pytest>=7.0
numpy>=1.20
psutil>=5.9.0
memory-profiler>=2.2.0
pytest-benchmark>=4.0.0
pytest-html>=3.2.0
pytest-json-report>=1.5.0
pytest-timeout>=2.1.0
pytest-cov>=4.0.0
pytest-mock>=3.10.0
-r python/pyproject.toml[dev]
""",

        "dev-requirements.txt": """# Development dependencies for NLM Python bindings
# Core testing dependencies
pytest>=7.0
numpy>=1.20
psutil>=5.9.0

# Additional testing plugins
pytest-benchmark>=4.0.0
pytest-html>=3.2.0
pytest-json-report>=1.5.0
pytest-timeout>=2.1.0
pytest-cov>=4.0.0
pytest-mock>=3.10.0
pytest-xdist>=3.0.0
pytest-randomly>=3.15.0

# Memory profiling
memory-profiler>=2.2.0
objgraph>=3.4.0
pympler>=0.9.0

# Performance testing
pytest-asyncio>=0.21.0
asyncio>=3.4.2

# Documentation
sphinx>=4.4.0
sphinx-rtd-theme>=1.0.0
nbsphinx>=0.9.2

# Code quality
black>=23.3.0
isort>=5.12.0
flake8>=6.0.0
mypy>=1.3.0
pre-commit>=3.3.0

# Environment
python-dotenv>=1.0.0
python-Levenshtein>=0.20.0
""",

        "tox.ini": """# Tox configuration for NLM Python bindings testing

[tox]
envlist = py38,py39,py310,py311,py312
requires = tox-pyenv
isolated_build = true
envdir = {homedir}/.tox
skip_missing_interpreters = true

[testenv]
# Set the path to the virtual environment
testsdir = tests
deps = 
    pytest>=7.0
    numpy>=1.20
    psutil>=5.9.0
    pytest-benchmark>=4.0.0
    pytest-html>=3.2.0
    pytest-json-report>=1.5.0
    pytest-timeout>=2.1.0
    pytest-cov>=4.0.0
    pytest-mock>=3.10.0
    memory-profiler>=2.2.0
commands = 
    python -m pytest tests/ -v --tb=short --timeout=300
    python -m pytest tests/ -m "integration" -v
    python -m pytest tests/ -m "performance" -v --benchmark-only
    python -m pytest tests/ -m "memory" -v --memory-leak

[testenv:py38]
deps =
    {[testenv]deps}
    {[testenv]deps}

[testenv:py39]
deps =
    {[testenv]deps}
    {[testenv]deps}

[testenv:py310]
deps =
    {[testenv]deps}
    {[testenv]deps}

[testenv:py311]
deps =
    {[testenv]deps}
    {[testenv]deps}

[testenv:py312]
deps =
    {[testenv]deps}
    {[testenv]deps}

[testenv:benchmark]
basepython = python3.8
deps =
    {[testenv]deps}
    pytest-benchmark>=4.0.0
commands = 
    python -m pytest tests/ -m "performance" -v --benchmark-only --benchmark-save=benchmark_results.json

[testenv:memory]
basepython = python3.8
deps =
    {[testenv]deps}
    memory-profiler>=2.2.0
commands = 
    python -m pytest tests/ -m "memory" -v --memory-leak
    python scripts/generate_memory_report.py memory_results.json

[testenv:docs]
basepython = python3.8
deps =
    {[testenv]deps}
    sphinx>=4.4.0
    sphinx-rtd-theme>=1.0.0
    nbsphinx>=0.9.2
commands = 
    sphinx-build -b html docs/ docs/build/html
    sphinx-build -b json docs/ docs/build/json
""",
    }

    return ci_config


def create_performance_testing_strategy():
    """
    Create comprehensive performance testing strategy.
    
    Returns:
        Dict: Performance testing strategy
    """
    strategy = {
        "overview": {
            "title": "Performance Testing Strategy for NLM Python Bindings",
            "description": "Comprehensive performance testing approach for the NLM Python bindings",
            "objectives": [
                "Establish baseline performance metrics",
                "Detect performance regressions",
                "Validate performance under load",
                "Ensure memory efficiency",
                "Optimize performance for large networks",
            ],
        },
        "testing_types": {
            "execution_time_benchmarks": {
                "description": "Measure execution time of various operations",
                "tests": [
                    "test_brain_creation_benchmark",
                    "test_brain_step_benchmark",
                    "test_agent_world_benchmark",
                    "test_plasticity_performance",
                ],
                "metrics": [
                    "avg_time_per_operation",
                    "max_time_per_operation",
                    "throughput_operations_per_second",
                    "time_scaling_with_size",
                ],
            },
            "memory_usage_benchmarks": {
                "description": "Measure memory usage patterns",
                "tests": [
                    "test_memory_usage_with_plasticity",
                    "test_stress_test_memory",
                    "test_memory_efficiency_large_scale",
                    "test_long_running_simulation_memory",
                ],
                "metrics": [
                    "peak_memory_usage",
                    "memory_growth_rate",
                    "memory_per_neuron",
                    "memory_efficiency_ratio",
                ],
            },
            "scalability_tests": {
                "description": "Test scalability with increasing system sizes",
                "tests": [
                    "test_scalability_neuron_count",
                    "test_scalability_synapse_density",
                    "test_scalability_world_size",
                    "test_scalability_agents",
                ],
                "metrics": [
                    "time_scaling_with_neurons",
                    "time_scaling_with_synapses",
                    "memory_scaling_with_size",
                    "performance_degradation",
                ],
            },
            "load_tests": {
                "description": "Test performance under load conditions",
                "tests": [
                    "test_multiple_agent_performance",
                    "test_stress_test_memory",
                    "test_large_network_performance",
                    "test_plasticity_computation_performance",
                ],
                "metrics": [
                    "concurrent_user_performance",
                    "resource_utilization",
                    "bottleneck_identification",
                    "scalability_limits",
                ],
            },
        },
        "benchmark_categories": {
            "unit_operations": {
                "description": "Test individual unit operations",
                "benchmarks": [
                    "brain_creation",
                    "brain_initialization",
                    "brain_step",
                    "agent_initialization",
                    "world_update",
                    "action_decoding",
                    "sensory_processing",
                ],
            },
            "simulation_scenarios": {
                "description": "Test complete simulation scenarios",
                "benchmarks": [
                    "basic_brain_simulation",
                    "agent_world_simulation",
                    "learning_simulation",
                    "plasticity_simulation",
                    "development_simulation",
                ],
            },
            "real_world_workloads": {
                "description": "Test realistic workloads",
                "benchmarks": [
                    "small_networks_100_neuron",
                    "medium_networks_1000_neuron",
                    "large_networks_5000_neuron",
                    "many_agents_10_agents",
                    "mixed_scenarios",
                ],
            },
        },
        "testing_approach": {
            "baseline_establishment": {
                "steps": [
                    "Install testing environment",
                    "Build C++ bindings",
                    "Install Python dependencies",
                    "Run standard test suite",
                    "Record performance metrics",
                ],
                "metrics": [
                    "brain_creation_time",
                    "simulation_step_time",
                    "memory_usage",
                    "cpu_usage",
                    "throughput",
                ],
            },
            "regression_testing": {
                "steps": [
                    "Run performance tests on current code",
                    "Compare with baseline metrics",
                    "Identify performance changes",
                    "Investigate root causes",
                    "Implement optimizations if needed",
                ],
                "thresholds": {
                    "acceptable_regression": 0.10,  # 10%
                    "warning_regression": 0.25,   # 25%
                    "critical_regression": 0.50,  # 50%
                },
            },
            "continuous_testing": {
                "steps": [
                    "Run performance tests on every commit",
                    "Compare with baseline",
                    "Alert on performance regressions",
                    "Track performance trends",
                    "Update baselines as needed",
                ],
                "integration": [
                    "GitHub Actions",
                    "Jenkins",
                    "GitLab CI",
                    "Azure DevOps",
                ],
            },
        },
        "tools_and_techniques": {
            "benchmark_framework": [
                "pytest-benchmark",
                "memory_profiler",
                "timeit",
                "cProfile",
                "objgraph",
            ],
            "performance_monitoring": [
                "psutil",
                "memory_profiler",
                "system monitoring",
                "resource usage tracking",
            ],
            "analysis_tools": [
                "matplotlib",
                "seaborn",
                "pandas",
                "plotly",
                "JSON reporting",
            ],
            "automation": [
                "GitHub Actions",
                "CI/CD pipelines",
                "scheduled benchmarks",
                "alert systems",
            ],
        },
        "reporting_and_analysis": {
            "reports": [
                "HTML performance reports",
                "JSON benchmark data",
                "Performance trend analysis",
                "Memory usage reports",
                "Comparison charts",
                "Regression analysis",
                "Scalability analysis",
            ],
            "metrics_dashboard": {
                "real-time_monitoring": True,
                "historical_tracking": True,
                "trend_analysis": True,
                "alerting": True,
                "visualization": True,
            },
        },
        "best_practices": {
            "benchmarking": [
                "Use consistent test data",
                "Run multiple iterations",
                "Calculate averages and standard deviations",
                "Use statistical significance testing",
                "Warm up before measurements",
                "Clear caches before runs",
                "Run in isolated environments",
            ],
            "memory_testing": [
                "Track allocation patterns",
                "Monitor peak memory usage",
                "Check for memory leaks",
                "Analyze memory efficiency",
                "Test with different data sizes",
                "Clear memory between tests",
                "Use memory profiling tools",
            ],
            "scalability_testing": [
                "Test with increasing system sizes",
                "Measure performance scaling",
                "Identify bottlenecks",
                "Validate resource requirements",
                "Test with concurrent users",
                "Simulate real-world workloads",
                "Test edge cases",
            ],
            "regression_testing": [
                "Establish clear baselines",
                "Monitor changes over time",
                "Set acceptable thresholds",
                "Alert on significant regressions",
                "Investigate root causes",
                "Implement optimizations",
                "Update baselines as needed",
            ],
        },
        "implementation_roadmap": {
            "phase1": {
                "title": "Foundation",
                "tasks": [
                    "Establish baseline benchmarks",
                    "Create basic performance tests",
                    "Set up performance monitoring",
                    "Create initial test suite",
                ],
            },
            "phase2": {
                "title": "Expansion",
                "tasks": [
                    "Add comprehensive performance tests",
                    "Implement memory leak detection",
                    "Create scalability tests",
                    "Set up automated testing",
                ],
            },
            "phase3": {
                "title": "Optimization",
                "tasks": [
                    "Optimize performance tests",
                    "Enhance reporting capabilities",
                    "Implement advanced analytics",
                    "Set up continuous monitoring",
                ],
            },
        },
    }

    return strategy


def main():
    """
    Generate comprehensive CI setup recommendations.
    
    Returns:
        str: Complete CI setup documentation
    """
    ci_setup = """# CI/CD Setup Recommendations for NLM Python Bindings

## Overview
This document provides comprehensive CI/CD setup recommendations for
automating testing of the NLM Python bindings.

## 1. GitHub Actions Configuration

### Tests Workflow (`.github/workflows/tests.yml`)

This workflow runs the complete test suite on every push and pull request:

```yaml
name: Run Tests

on:
  push:
    branches: [main, develop]
    tags: [v*]
  pull_request:
    branches: [main, develop]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM UTC

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, 3.10, 3.11, 3.12]
    
    steps:
    - uses: actions/checkout@v3
    - uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install system dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          pkg-config \
          libpython3-dev \
          python3-dev
    
    - name: Install Python dependencies
      run: |
        python -m pip install --upgrade pip
        pip install pytest>=7.0
        pip install numpy>=1.20
        pip install psutil>=5.9.0
    
    - name: Build C++ bindings
      run: |
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    
    - name: Install pynlm package
      run: |
        cd python
        pip install -e .
    
    - name: Run unit tests
      run: |
        pytest tests/test_pynlm_basic.py tests/test_pynlm_config.py tests/test_pynlm_error_handling.py -v
    
    - name: Run integration tests
      run: |
        pytest tests/test_pynlm_brain_simulation.py tests/test_pynlm_agent_world.py -v
    
    - name: Run plasticity tests
      run: |
        pytest tests/test_pynlm_plasticity.py -v
    
    - name: Run performance tests (once)
      if: matrix.python-version == '3.8'
      run: |
        pytest tests/test_pynlm_performance.py -v --benchmark-only
    
    - name: Run memory tests (once)
      if: matrix.python-version == '3.8'
      run: |
        pytest tests/test_pynlm_memory.py -v --memory-leak
    
    - name: Generate test reports
      run: |
        pytest tests/ --html=reports/test_results.html --json=test_results.json
    
    - name: Upload test results
      uses: actions/upload-artifact@v3
      with:
        name: test-results-${{ matrix.python-version }}
        path: reports/
        retention-days: 30
```

### Performance Workflow (`.github/workflows/performance.yml`)

This workflow runs performance benchmarks on a schedule:

```yaml
name: Performance Testing

on:
  push:
    branches: [main]
    tags: [v*]
  schedule:
    - cron: '0 6 * * 1'  # Weekly on Monday at 6 AM UTC

jobs:
  benchmark:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    - uses: actions/setup-python@v3
      with:
        python-version: '3.8'
    
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install pytest pytest-benchmark
        pip install numpy psutil
    
    - name: Build C++ bindings
      run: |
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    
    - name: Install pynlm package
      run: cd python && pip install -e .
    
    - name: Run comprehensive benchmarks
      run: |
        pytest tests/ -k "benchmark" -v --benchmark-only --benchmark-save=weekly_benchmarks.json
    
    - name: Compare with historical data
      run: |
        python scripts/compare_weekly_benchmarks.py weekly_benchmarks.json historical_benchmarks.json
```

## 2. Local CI Scripts

### Test Runner (`run_all_tests.py`)

```python
#!/usr/bin/env python3
"""
Run all tests for NLM Python bindings.
"""
import subprocess
import sys
import os
from pathlib import Path

def run_tests():
    """Run all tests and provide comprehensive reporting."""
    print("=" * 60)
    print("NLM Python Bindings - Complete Test Suite")
    print("=" * 60)
    
    # Test categories
    test_categories = [
        ("Unit Tests", [
            "tests/test_pynlm_basic.py",
            "tests/test_pynlm_config.py",
            "tests/test_pynlm_error_handling.py",
        ]),
        ("Integration Tests", [
            "tests/test_pynlm_brain_simulation.py",
            "tests/test_pynlm_agent_world.py",
        ]),
        ("Plasticity Tests", [
            "tests/test_pynlm_plasticity.py",
        ]),
        ("Performance Tests", [
            "tests/test_pynlm_performance.py",
        ]),
        ("Memory Tests", [
            "tests/test_pynlm_memory.py",
        ]),
    ]
    
    total_tests = 0
    passed_tests = 0
    failed_tests = 0
    
    for category, test_files in test_categories:
        print(f"\n{category}:")
        print("-" * 40)
        
        category_tests = 0
        category_passed = 0
        category_failed = 0
        
        for test_file in test_files:
            if not os.path.exists(test_file):
                print(f"  Skipping {test_file} (file not found)")
                continue
                
            print(f"  Running {test_file}...")
            
            try:
                result = subprocess.run([
                    sys.executable, "-m", "pytest",
                    test_file,
                    "-v",
                    "--tb=short",
                ], capture_output=True, text=True, cwd=os.getcwd())
                
                category_tests += 1
                total_tests += 1
                
                if result.returncode == 0:
                    category_passed += 1
                    passed_tests += 1
                    print(f"    ✓ PASSED ({result.stdout.count('PASSED')} tests)")
                else:
                    category_failed += 1
                    failed_tests += 1
                    print(f"    ✗ FAILED")
                    print(f"      Output: {result.stdout[:200]}...")
                    
            except Exception as e:
                category_failed += 1
                failed_tests += 1
                print(f"    ✗ ERROR: {str(e)}")
        
        print(f"\n  {category} Summary:")
        print(f"    Tests: {category_tests}")
        print(f"    Passed: {category_passed}")
        print(f"    Failed: {category_failed}")
    
    print("\n" + "=" * 60)
    print("Overall Summary")
    print("=" * 60)
    print(f"Total Tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {failed_tests}")
    print(f"Success Rate: {passed_tests/total_tests*100:.1f}%")
    
    if failed_tests > 0:
        print(f"\n{failed_tests} tests failed.")
        return 1
    else:
        print(f"\nAll {total_tests} tests passed!")
        return 0

if __name__ == "__main__":
    sys.exit(run_tests())
```

### Performance Regression Checker (`check_performance_regression.py`)

```python
#!/usr/bin/env python3
"""
Check for performance regressions in NLM Python bindings.
"""
import json
import os
import sys
from pathlib import Path
import argparse

def load_benchmark_results(filepath):
    """Load benchmark results from JSON file."""
    try:
        with open(filepath, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Warning: Benchmark file not found: {filepath}")
        return {}
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in benchmark file: {e}")
        return {}

def calculate_regression(current, baseline, threshold=0.1):
    """
    Calculate performance regression between current and baseline.
    
    Args:
        current: Current benchmark results
        baseline: Baseline benchmark results
        threshold: Regression threshold (10% = 0.1)
    
    Returns:
        Dict: Regression analysis
    """
    regressions = {}
    
    for test_name in current:
        if test_name in baseline:
            current_time = current[test_name].get('avg_time', 0)
            baseline_time = baseline[test_name].get('avg_time', 0)
            
            if baseline_time > 0 and current_time > 0:
                regression = (current_time - baseline_time) / baseline_time
                regressions[test_name] = {
                    'current_time': current_time,
                    'baseline_time': baseline_time,
                    'regression': regression,
                    'significant': regression > threshold,
                }
    
    return regressions

def generate_report(regressions, output_file):
    """
    Generate performance regression report.
    
    Args:
        regressions: Regression analysis results
        output_file: Output file path
    """
    with open(output_file, 'w') as f:
        f.write("Performance Regression Report\n")
        f.write("=" * 50 + "\n\n")
        
        if not regressions:
            f.write("No performance regressions detected.\n")
            return
        
        significant = {k: v for k, v in regressions.items() if v['significant']}
        
        if significant:
            f.write(f"Significant performance regressions detected:\n")
            f.write(f"{'Test Name':<30} {'Current (s)':<15} {'Baseline (s)':<15} {'Regression':<12} {'Status':<10}\n")
            f.write("-" * 80 + "\n")
            
            for test_name, data in significant.items():
                f.write(f"{test_name:<30} {data['current_time']:<15.4f} {data['baseline_time']:<15.4f} "
                       f"{data['regression']:<12.2%} {'REGRESSION':<10}\n")
        
        # Summary
        f.write(f"\nSummary:\n")
        f.write(f"Total tests analyzed: {len(regressions)}\n")
        f.write(f"Tests with regressions: {len(significant)}\n")
        f.write(f"Regression rate: {len(significant)/len(regressions)*100:.1f}%\n")
        
        if significant:
            f.write(f"\nWARNING: Performance regressions detected. Please review.\n")
            return 1
        else:
            f.write(f"\nOK: No significant performance regressions detected.\n")
            return 0

def main():
    parser = ArgumentParser(description="Check for performance regressions")
    parser.add_argument("current", help="Current benchmark results JSON file")
    parser.add_argument("baseline", help="Baseline benchmark results JSON file")
    parser.add_argument("-o", "--output", help="Output report file", default="regression_report.txt")
    parser.add_argument("-t", "--threshold", help="Regression threshold (0.0-1.0)", type=float, default=0.1)
    
    args = parser.parse_args()
    
    current_results = load_benchmark_results(args.current)
    baseline_results = load_benchmark_results(args.baseline)
    
    if not current_results or not baseline_results:
        print("Error: Could not load benchmark results")
        return 1
    
    regressions = calculate_regression(current_results, baseline_results, args.threshold)
    
    return generate_report(regressions, args.output)

if __name__ == "__main__":
    sys.exit(main())
```

## 3. Local Development Setup

### Development Dependencies (`dev-requirements.txt`)
```txt
# Core testing dependencies
pytest>=7.0
numpy>=1.20
psutil>=5.9.0

# Additional testing plugins
pytest-benchmark>=4.0.0
pytest-html>=3.2.0
pytest-json-report>=1.5.0
pytest-timeout>=2.1.0
pytest-cov>=4.0.0
pytest-mock>=3.10.0
pytest-xdist>=3.0.0
pytest-randomly>=3.15.0

# Memory profiling
memory-profiler>=2.2.0
objgraph>=3.4.0
pympler>=0.9.0

# Performance testing
pytest-asyncio>=0.21.0
asyncio>=3.4.2

# Documentation
sphinx>=4.4.0
sphinx-rtd-theme>=1.0.0
nbsphinx>=0.9.2

# Code quality
black>=23.3.0
isort>=5.12.0
flake8>=6.0.0
mypy>=1.3.0
pre-commit>=3.3.0

# Environment
python-dotenv>=1.0.0
python-Levenshtein>=0.20.0
```

### Pre-commit Configuration (`.pre-commit-config.yaml`)
```yaml
# Pre-commit configuration for NLM Python bindings

repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-ast
      - id: check-builtin-deps
      - id: check-case-conflict
      - id: check-docstring-first
      - id: check-executables
      - id: check-json
      - id: check-merge-conflict
      - id: check-symlinks
      - id: check-toml
      - id: check-vcs-permalinks
      - id: check-xml
      - id: check-yaml
      - id: debug-statements
      - id: detect-private-key
      - id: end-of-file-fixer
      - id: fix-encoding-charset
      - id: mixed-line-ending
      - id: trailing-whitespace

  - repo: https://github.com/psf/black
    rev: 23.3.0
    hooks:
      - id: black
        args: [--check, --diff]

  - repo: https://github.com/pycqa/isort
    rev: 5.12.0
    hooks:
      - id: isort
        args: [--check-only, --diff]

  - repo: https://github.com/PyCQA/flake8
    rev: 6.0.0
    hooks:
      - id: flake8

  - repo: https://github.com/asottile/pyupgrade
    rev: v3.3.1
    hooks:
      - id: pyupgrade
        args: [--py38-plus]

  - repo: https://github.com/executablebooks/MyPy
    rev: v1.3.0
    hooks:
      - id: mypy
        args: [--strict, pynlm]

  - repo: https://github.com/python-poetry/poetry
    rev: 1.5.1
    hooks:
      - id: poetry-check
      - id: poetry-lock
      - id: poetry-export
        args: [--without-hashes, --format, requirements.txt, --output, requirements.txt]

  - repo: local
    hooks:
      - id: run-tests
        name: Run tests
        entry: python run_all_tests.py
        language: system
        pass_filenames: false
        always_run: true
```

### Configuration Files

#### `.flake8`
```ini
# Flake8 configuration for NLM Python bindings

[flake8]
max-line-length = 88
extend-ignore = E203, W503
exclude = 
    .git,
    __pycache__,
    build/,
    dist/,
    *.egg-info/,
    .pytest_cache/,
    reports/,
    docs/source/build/,
    */__pycache__/*,
    *test*.py,
    scripts/*.py,
    */test_*.py,
    */tests/*.py,

per-file-ignores = 
    */test_*.py: S101
    scripts/*.py: S101
```

#### `.editorconfig`
```ini
# EditorConfig configuration for NLM Python bindings

root = true

[*]
charset = utf-8
end_of_line = lf
indent_size = 4
indent_style = space
insert_final_newline = true
max_line_length = 88
tab_width = 4
trim_trailing_whitespace = true

[*.md]
max_line_length = 0

[*.py]
max_line_length = 88

[*.yml]
max_line_length = 100

[*.json]
indent = 2
max_line_length = 100
```

#### `requirements.txt`
```txt
# Python dependencies for NLM Python bindings testing
pytest>=7.0
numpy>=1.20
psutil>=5.9.0
memory-profiler>=2.2.0
pytest-benchmark>=4.0.0
pytest-html>=3.2.0
pytest-json-report>=1.5.0
pytest-timeout>=2.1.0
pytest-cov>=4.0.0
pytest-mock>=3.10.0
-r python/pyproject.toml[dev]
```

## 4. Advanced Configuration

### Pytest Configuration (`pyproject.toml`)
```toml
[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
addopts = [
    "-v",
    "--tb=short",
    "--timeout=300",
    "--strict-markers",
    "--disable-warnings",
    "-x",
]
markers = [
    "slow(reason): marks tests as slow (skip with '-m "not slow"')",
    "integration(reason): marks tests as integration tests",
    "performance(reason): marks tests as performance tests",
    "memory(reason): marks tests as memory tests",
    "regression(reason): marks tests as regression tests",
    "benchmark(reason): marks tests as benchmark tests",
]
filterwarnings = [
    "ignore::UserWarning",
    "ignore::DeprecationWarning",
    "ignore::FutureWarning",
]
```

### Test Environment Variables
```bash
# Set timeout for individual tests
export PYTEST_TIMEOUT=600

# Set cache directory
export PYTEST_CACHE_DIR=.pytest_cache

# Enable debug mode
export PYTEST_DEBUG=1
```

## 5. Test Categories and Execution

### Running Specific Test Categories
```bash
# Run unit tests
pytest tests/test_pynlm_basic.py tests/test_pynlm_config.py tests/test_pynlm_error_handling.py -v

# Run integration tests
pytest tests/test_pynlm_brain_simulation.py tests/test_pynlm_agent_world.py -v

# Run plasticity tests
pytest tests/test_pynlm_plasticity.py -v

# Run performance tests
pytest tests/test_pynlm_performance.py -v --benchmark-only

# Run memory tests
pytest tests/test_pynlm_memory.py -v --memory-leak
```

### Running Tests with Specific Markers
```bash
# Run slow tests
pytest tests/ -m "slow" -v

# Run integration tests
pytest tests/ -m "integration" -v

# Run performance tests
pytest tests/ -m "performance" -v

# Run memory tests
pytest tests/ -m "memory" -v

# Run regression tests
pytest tests/ -m "regression" -v
```

### Running Tests with Parallel Execution
```bash
# Run tests in parallel
pytest tests/ -n auto -v

# Run tests with specific number of workers
pytest tests/ -n 4 -v
```

## 6. Test Reports and Analysis

### HTML Reports
```bash
# Generate HTML reports
pytest tests/ --html=reports/test_results.html --capture=no

# View specific test results
pytest tests/ --html=reports/test_results.html --json=test_results.json
```

### Performance Comparison
```bash
# Compare with baseline
python scripts/compare_performance.py baseline.json current.json

# Generate performance report
python scripts/compare_performance.py baseline.json current.json -o reports/comparison
```

### Memory Reports
```bash
# Generate memory report
python scripts/generate_memory_report.py memory_results.json
```

## 7. Troubleshooting

### Common Issues

#### ModuleNotFoundError: No module named 'pynlm'
```bash
# Install the package
pip install -e python/

# Or install from source
pip install -e .  # From project root
```

#### CMake Errors
```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake pkg-config libpython3-dev python3-dev
```

#### Memory Issues
```bash
# Clear pytest cache
rm -rf .pytest_cache

# Clear temporary files
rm -f /tmp/test_*.json /tmp/test_*.bin

# Check system memory
free -h
```

## 8. Maintenance

### Regular Maintenance Tasks

#### 1. Clean up Test Artifacts
```bash
# Clean up reports and caches
rm -rf reports/*
rm -rf .pytest_cache
rm -f test_results.json
rm -f benchmark_results.json
rm -f regression_report.txt
```

#### 2. Update Dependencies
```bash
# Check for outdated packages
pip list --outdated | grep -E "pytest|numpy|psutil"

# Update packages
pip install --upgrade pytest numpy psutil
```

#### 3. Check for Performance Regressions
```bash
# Run performance regression check
python scripts/check_performance_regression.py baseline.json current.json -o regression_report.txt
```

#### 4. Monitor Memory Usage
```bash
# Run memory tests
pytest tests/test_pynlm_memory.py -v --memory-leak

# Generate memory report
python scripts/generate_memory_report.py memory_results.json
```

### Backup and Restore
```bash
# Backup test results
cp -r reports/ reports_backup_$(date +%Y%m%d_%H%M%S)

# Restore test results
mkdir -p reports
cp -r reports_backup_*/reports/* reports/
```

## 9. Conclusion

This CI/CD setup provides a comprehensive framework for automating testing
of the NLM Python bindings. It includes:

1. **Automated Testing**: GitHub Actions for continuous integration
2. **Performance Monitoring**: Scheduled benchmarks and regression detection
3. **Memory Leak Detection**: Automated memory usage monitoring
4. **Comprehensive Reporting**: HTML, JSON, and custom reports
5. **Local Development**: Scripts and configuration for local testing
6. **Quality Assurance**: Pre-commit hooks and code quality checks
7. **Scalability**: Support for parallel testing and large-scale execution

By implementing this CI/CD setup, you'll ensure that the NLM Python bindings
remain reliable, performant, and maintainable throughout their development lifecycle.
"""

    return ci_setup


def create_performance_testing_strategy_document():
    """
    Create performance testing strategy document.
    
    Returns:
        str: Performance testing strategy
    """
    return """# Performance Testing Strategy for NLM Python Bindings

## Overview
This document outlines the comprehensive performance testing strategy
for the NLM Python bindings, ensuring robust performance validation
and regression detection.

## 1. Performance Testing Objectives

### Primary Goals
1. Establish baseline performance metrics for the NLM Python bindings
2. Detect and prevent performance regressions over time
3. Validate performance under various load conditions
4. Ensure memory efficiency and leak detection
5. Optimize performance for large-scale neural networks

### Secondary Goals
1. Benchmark different configuration options
2. Identify performance bottlenecks
3. Validate scalability characteristics
4. Compare performance across different Python versions
5. Establish performance thresholds for CI/CD pipelines

## 2. Performance Testing Categories

### 2.1 Execution Time Benchmarks
**Purpose**: Measure execution time of various operations

#### Test Categories
- **Brain Creation**: Time to create and initialize a brain
- **Brain Simulation**: Time per simulation step
- **Agent-World Interaction**: Time for complete agent-world cycles
- **Plasticity Computation**: Time with learning mechanisms enabled

#### Metrics
- Average execution time
- Maximum execution time
- Throughput (operations per second)
- Time scaling with system size

#### Test Files
- `test_pynlm_performance.py` - Performance benchmarks
- `test_pynlm_basic.py` - Basic operation timing
- `test_pynlm_brain_simulation.py` - Simulation performance

### 2.2 Memory Usage Benchmarks
**Purpose**: Measure memory usage patterns and detect leaks

#### Test Categories
- **Memory Growth**: Memory increase over time
- **Peak Memory**: Maximum memory usage during tests
- **Memory Efficiency**: Memory per neuron/synapse
- **Large Network Memory**: Memory usage with extensive networks

#### Metrics
- Peak memory usage
- Memory growth rate
- Memory allocation patterns
- Memory cleanup effectiveness

#### Test Files
- `test_pynlm_memory.py` - Memory usage and leak detection
- `test_pynlm_performance.py` - Memory benchmarking

### 2.3 Scalability Tests
**Purpose**: Test performance with increasing system sizes

#### Test Categories
- **Neuron Count Scaling**: Performance with different neuron counts
- **Synapse Density Scaling**: Performance with different connection densities
- **World Size Scaling**: Performance with different world dimensions
- **Agent Count Scaling**: Performance with multiple agents

#### Metrics
- Time scaling with system size
- Memory scaling with system size
- Performance degradation rate
- Resource utilization efficiency

#### Test Files
- `test_pynlm_performance.py` - Scalability tests
- `test_pynlm_agent_world.py` - Multi-agent performance

### 2.4 Load Tests
**Purpose**: Test performance under realistic load conditions

#### Test Categories
- **Concurrent Agents**: Multiple agents in same world
- **Large Networks**: Extensive neural networks
- **Complex Plasticity**: Multiple learning mechanisms
- **Real-world Scenarios**: Simulation of real-world conditions

#### Metrics
- Concurrent user performance
- Resource contention handling
- Bottleneck identification
- Scalability limits

#### Test Files
- `test_pynlm_performance.py` - Load testing
- `test_pynlm_agent_world.py` - Multi-agent scenarios

## 3. Testing Approach

### 3.1 Baseline Establishment
**Steps**:
1. Set up clean testing environment
2. Build C++ bindings with optimization
3. Install Python dependencies
4. Run standard test suite to establish baseline
5. Record performance metrics

**Metrics Collected**:
- Brain creation time
- Simulation step time
- Memory usage
- CPU usage
- Throughput

### 3.2 Regression Testing
**Steps**:
1. Run performance tests on current code
2. Compare with baseline metrics
3. Identify performance changes
4. Investigate root causes
5. Implement optimizations if needed

**Regression Thresholds**:
```yaml
acceptable_regression: 0.10  # 10%
warning_regression: 0.25     # 25%
critical_regression: 0.50     # 50%
```

### 3.3 Continuous Testing
**Steps**:
1. Run performance tests on every commit
2. Compare with baseline
3. Alert on performance regressions
4. Track performance trends
5. Update baselines as needed

**Integration Tools**:
- GitHub Actions
- Jenkins
- GitLab CI
- Azure DevOps

## 4. Tools and Techniques

### 4.1 Benchmark Framework
```python
import pytest_benchmark
import time
import tracemalloc
import psutil

# Example benchmark
@pytest.mark.benchmark
def test_brain_creation(benchmark):
    """Benchmark brain creation."""
    benchmark(lambda: pynlm.createBrain(pynlm.createDefaultConfig()))
```

### 4.2 Performance Monitoring
```python
import psutil
import tracemalloc

# Track memory usage
def track_memory():
    process = psutil.Process()
    return process.memory_info().rss

# Track CPU usage
def track_cpu():
    return psutil.cpu_percent()
```

### 4.3 Analysis Tools
```python
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import json

# Performance comparison
def compare_performance(current, baseline):
    # Load and compare benchmark results
    pass

# Memory analysis
def analyze_memory_usage(results):
    # Analyze memory patterns
    pass
```

### 4.4 Automation
```yaml
# GitHub Actions workflow
jobs:
  test:
    steps:
    - name: Run performance tests
      run: |
        pytest tests/ -m "performance" -v --benchmark-only
        pytest tests/ -m "memory" -v --memory-leak
```

## 5. Reporting and Analysis

### 5.1 Reports
- HTML performance reports
- JSON benchmark data
- Performance trend analysis
- Memory usage reports
- Comparison charts
- Regression analysis
- Scalability analysis

### 5.2 Metrics Dashboard
```yaml
# Real-time monitoring configuration
metrics:
  - execution_time
  - memory_usage
  - throughput
  - scalability_score
  

alerts:
  - regression_threshold: 0.10
  - memory_leak_threshold: 100MB
  - performance_degradation_threshold: 0.25
```

## 6. Best Practices

### 6.1 Benchmarking
1. Use consistent test data
2. Run multiple iterations
3. Calculate averages and standard deviations
4. Use statistical significance testing
5. Warm up before measurements
6. Clear caches before runs
7. Run in isolated environments

### 6.2 Memory Testing
1. Track allocation patterns
2. Monitor peak memory usage
3. Check for memory leaks
4. Analyze memory efficiency
5. Test with different data sizes
6. Clear memory between tests
7. Use memory profiling tools

### 6.3 Scalability Testing
1. Test with increasing system sizes
2. Measure performance scaling
3. Identify bottlenecks
4. Validate resource requirements
5. Test with concurrent users
6. Simulate real-world workloads
7. Test edge cases

### 6.4 Regression Testing
1. Establish clear baselines
2. Monitor changes over time
3. Set acceptable thresholds
4. Alert on significant regressions
5. Investigate root causes
6. Implement optimizations
7. Update baselines as needed

## 7. Implementation Roadmap

### Phase 1: Foundation
**Duration**: 2-3 weeks
**Tasks**:
- [ ] Establish baseline benchmarks
- [ ] Create basic performance tests
- [ ] Set up performance monitoring
- [ ] Create initial test suite

### Phase 2: Expansion
**Duration**: 4-6 weeks
**Tasks**:
- [ ] Add comprehensive performance tests
- [ ] Implement memory leak detection
- [ ] Create scalability tests
- [ ] Set up automated testing

### Phase 3: Optimization
**Duration**: 3-4 weeks
**Tasks**:
- [ ] Optimize performance tests
- [ ] Enhance reporting capabilities
- [ ] Implement advanced analytics
- [ ] Set up continuous monitoring

## 8. Performance Testing Integration

### 8.1 GitHub Actions Integration
```yaml
# .github/workflows/tests.yml
test:
  jobs:
    test:
      steps:
      - name: Run performance tests
        run: |
          pytest tests/ -m "performance" -v --benchmark-only
        # Save benchmark results
      - uses: actions/upload-artifact@v3
        with:
          name: benchmarks
          path: benchmark_results.json
```

### 8.2 Local Development
```bash
# Install development dependencies
pip install -r dev-requirements.txt

# Run performance tests locally
pytest tests/test_pynlm_performance.py -v --benchmark-only

# Run memory tests
pytest tests/test_pynlm_memory.py -v --memory-leak
```

### 8.3 CI/CD Pipeline
```yaml
# Jenkinsfile or GitLab CI configuration
stages:
  - unit-tests
  - integration-tests
  - performance-tests
  - memory-tests
  - deploy
```

## 9. Performance Testing Categories

### 9.1 Unit Operation Performance
**Focus**: Individual operations and their performance characteristics

**Test Cases**:
- Brain creation and initialization
- Synapse operations
- Neuron operations
- Population operations
- Region operations

**Metrics**:
- Operation time
- Memory allocation
- CPU utilization

### 9.2 Simulation Scenario Performance
**Focus**: Complete simulation workflows

**Test Cases**:
- Basic brain simulation
- Agent-world interaction
- Learning and plasticity
- Development simulation

**Metrics**:
- Simulation throughput
- Resource utilization
- Time to completion
- Memory efficiency

### 9.3 Real-World Workload Performance
**Focus**: Realistic usage scenarios

**Test Cases**:
- Small-scale networks (100 neurons)
- Medium-scale networks (1000 neurons)
- Large-scale networks (5000+ neurons)
- Multiple agents (2-10 agents)
- Mixed scenarios (various configurations)

**Metrics**:
- Performance scaling
- Resource efficiency
- Bottleneck identification
- Real-world relevance

## 10. Performance Validation

### 10.1 Acceptance Criteria
```yaml
performance_thresholds:
  unit_operations:
    max_creation_time: 1.0  # seconds
    max_step_time: 0.01      # seconds
    max_memory_per_neuron: 1024  # bytes
  
  simulation:
    max_simulation_time: 10.0  # seconds for 100 steps
    max_memory_growth: 100MB    # per simulation
    
  scalability:
    acceptable_time_scaling: 0.1  # 10% increase per 10x size
    acceptable_memory_scaling: 0.5  # 50% increase per 10x size
```

### 10.2 Quality Gates
- **Unit Tests**: Pass rate > 95%
- **Integration Tests**: Pass rate > 90%
- **Performance Tests**: No regressions > 10%
- **Memory Tests**: No leaks > 50MB
- **Scalability Tests**: Acceptable scaling < 25%

### 10.3 Continuous Monitoring
```python
# Performance monitoring script
import time
import psutil
import json

def monitor_performance():
    # Collect performance data
    # Compare with thresholds
    # Alert on anomalies
    # Log results
    pass
```

## 11. Conclusion

This performance testing strategy provides a comprehensive framework for:

1. **Baseline Establishment**: Create initial performance benchmarks
2. **Regression Detection**: Monitor and prevent performance regressions
3. **Scalability Validation**: Test performance with increasing system sizes
4. **Memory Efficiency**: Ensure efficient memory usage and leak detection
5. **Continuous Improvement**: Set up automated testing and monitoring

By implementing this strategy, the NLM Python bindings will maintain
robust performance, provide valuable performance insights, and support
sustainable development practices.

**Next Steps**:
1. Implement Phase 1: Foundation
2. Set up CI/CD integration
3. Create initial performance baselines
4. Establish performance monitoring
5. Begin comprehensive testing
"""

    return strategy


if __name__ == "__main__":
    # Create CI setup recommendations
    ci_setup = create_github_actions_workflow()
    scripts = create_local_ci_scripts()
    strategy = create_performance_testing_strategy()
    
    print("=" * 60)
    print("NLM Python Bindings - CI/CD Setup Recommendations")
    print("=" * 60)
    print(f"GitHub Actions Workflows: {len(ci_setup)} files")
    print(f"Local CI Scripts: {len(scripts)} files")
    print(f"Performance Testing Strategy: {strategy['overview']['title']}")
    print("=" * 60)
