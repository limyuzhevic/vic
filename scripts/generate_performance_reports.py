#!/usr/bin/env python3

"""
NLM Performance Reports Generator
Generates comprehensive performance and regression reports from benchmark data
"""

import json
import os
import sys
from datetime import datetime
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from typing import Dict, List, Any

class PerformanceReportsGenerator:
    def __init__(self, results_dir: str, baselines_dir: str):
        self.results_dir = Path(results_dir)
        self.baselines_dir = Path(baselines_dir)
        
    def load_benchmark_results(self) -> Dict[str, List[Dict]]:
        """Load all benchmark results from results directory"""
        benchmarks = {}
        
        for result_file in self.results_dir.glob("*.json"):
            try:
                with open(result_file, 'r') as f:
                    data = json.load(f)
                    benchmark_name = data.get('benchmark_name', 'unknown')
                    if benchmark_name not in benchmarks:
                        benchmarks[benchmark_name] = []
                    benchmarks[benchmark_name].append(data)
            except Exception as e:
                print(f"Warning: Could not load {result_file}: {e}")
        
        return benchmarks
    
    def generate_performance_report(self, benchmarks: Dict[str, List[Dict]]) -> str:
        """Generate a comprehensive performance report"""
        report = []
        report.append("NLM Performance Analysis Report")
        report.append("=" * 50)
        report.append(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report.append("")
        
        for benchmark_name, results in benchmarks.items():
            report.append(f"Benchmark: {benchmark_name}")
            report.append("-" * 30)
            
            if not results:
                report.append("  No results available")
                report.append("")
                continue
            
            # Calculate statistics
            total_steps = sum(r.get('steps_executed', 0) for r in results)
            total_duration = sum(r.get('total_duration', 0.0) for r in results)
            avg_time_per_step = sum(r.get('avg_time_per_step', 0.0) for r in results) / len(results)
            avg_throughput = sum(r.get('throughput', 0.0) for r in results) / len(results)
            avg_performance_score = sum(r.get('performance_score', 0.0) for r in results) / len(results)
            
            report.append(f"  Total Benchmarks Run: {len(results)}")
            report.append(f"  Total Steps Executed: {total_steps}")
            report.append(f"  Total Duration: {total_duration:.2f} seconds")
            report.append(f"  Average Time per Step: {avg_time_per_step*1000:.2f} ms")
            report.append(f"  Average Throughput: {avg_throughput:.2f} steps/sec")
            report.append(f"  Average Performance Score: {avg_performance_score:.2f}/100")
            
            # Find best and worst results
            if results:
                best_throughput = max(results, key=lambda r: r.get('throughput', 0.0))
                worst_throughput = min(results, key=lambda r: r.get('throughput', 0.0))
                
                report.append(f"  Best Throughput: {best_throughput.get('throughput', 0.0):.2f} steps/sec")
                report.append(f"  Worst Throughput: {worst_throughput.get('throughput', 0.0):.2f} steps/sec")
                
                if best_throughput.get('throughput', 0.0) > 0:
                    improvement = ((best_throughput.get('throughput', 0.0) - worst_throughput.get('throughput', 0.0)) / 
                                  worst_throughput.get('throughput', 0.0)) * 100.0
                    report.append(f"  Throughput Improvement: {improvement:.1f}%")
            
            report.append("")
        
        return "\n".join(report)
    
    def generate_regression_report(self, benchmarks: Dict[str, List[Dict]]) -> str:
        """Generate a regression analysis report"""
        report = []
        report.append("NLM Performance Regression Analysis")
        report.append("=" * 50)
        report.append(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report.append("")
        
        for benchmark_name, results in benchmarks.items():
            report.append(f"Benchmark: {benchmark_name}")
            report.append("-" * 30)
            
            if len(results) < 2:
                report.append("  Not enough data for regression analysis (need at least 2 runs)")
                report.append("")
                continue
            
            # Sort by performance score
            sorted_results = sorted(results, key=lambda r: r.get('performance_score', 0.0), reverse=True)
            
            baseline = sorted_results[-1]  # Worst performer as baseline
            current = sorted_results[0]    # Best performer as current
            
            baseline_score = baseline.get('performance_score', 0.0)
            current_score = current.get('performance_score', 0.0)
            
            if baseline_score > 0:
                score_regression = ((current_score - baseline_score) / baseline_score) * 100.0
                
                if score_regression > 0:
                    report.append(f"  Performance Improvement: {score_regression:.1f}%")
                else:
                    report.append(f"  Performance Regression: {abs(score_regression):.1f}%")
                
                # Determine severity
                if abs(score_regression) > 20.0:
                    severity = "CRITICAL"
                elif abs(score_regression) > 10.0:
                    severity = "WARNING"
                elif abs(score_regression) > 5.0:
                    severity = "MINOR"
                else:
                    severity = "STABLE"
                
                report.append(f"  Severity: {severity}")
            
            # Time per step regression
            baseline_time = baseline.get('avg_time_per_step', 0.0)
            current_time = current.get('avg_time_per_step', 0.0)
            
            if baseline_time > 0:
                time_regression = ((current_time - baseline_time) / baseline_time) * 100.0
                report.append(f"  Time per Step: {time_regression:+.1f}%")
            
            report.append("")
        
        return "\n".join(report)
    
    def save_report(self, content: str, filename: str):
        """Save report to file"""
        output_file = self.results_dir / filename
        with open(output_file, 'w') as f:
            f.write(content)
        print(f"Report saved to: {output_file}")
    
    def generate_visualizations(self, benchmarks: Dict[str, List[Dict]]):
        """Generate performance visualizations"""
        try:
            for benchmark_name, results in benchmarks.items():
                if not results:
                    continue
                
                # Extract data for plotting
                iterations = list(range(len(results)))
                throughputs = [r.get('throughput', 0.0) for r in results]
                performance_scores = [r.get('performance_score', 0.0) for r in results]
                durations = [r.get('total_duration', 0.0) for r in results]
                
                # Create subplots
                fig, axes = plt.subplots(2, 2, figsize=(12, 10))
                fig.suptitle(f"Performance Analysis: {benchmark_name}", fontsize=16)
                
                # Throughput plot
                axes[0, 0].plot(iterations, throughputs, 'b-o', linewidth=2, markersize=6)
                axes[0, 0].set_title('Throughput (steps/sec)')
                axes[0, 0].set_xlabel('Iteration')
                axes[0, 0].set_ylabel('Throughput (steps/sec)')
                axes[0, 0].grid(True, alpha=0.3)
                
                # Performance score plot
                axes[0, 1].plot(iterations, performance_scores, 'g-o', linewidth=2, markersize=6)
                axes[0, 1].set_title('Performance Score (0-100)')
                axes[0, 1].set_xlabel('Iteration')
                axes[0, 1].set_ylabel('Performance Score')
                axes[0, 1].grid(True, alpha=0.3)
                axes[0, 1].set_ylim(0, 100)
                
                # Duration plot
                axes[1, 0].plot(iterations, durations, 'r-o', linewidth=2, markersize=6)
                axes[1, 0].set_title('Duration (seconds)')
                axes[1, 0].set_xlabel('Iteration')
                axes[1, 0].set_ylabel('Duration (seconds)')
                axes[1, 0].grid(True, alpha=0.3)
                
                # Time per step plot
                time_per_steps = [r.get('avg_time_per_step', 0.0) for r in results]
                axes[1, 1].plot(iterations, [t * 1000 for t in time_per_steps], 'c-o', linewidth=2, markersize=6)
                axes[1, 1].set_title('Average Time per Step (ms)')
                axes[1, 1].set_xlabel('Iteration')
                axes[1, 1].set_ylabel('Time per Step (ms)')
                axes[1, 1].grid(True, alpha=0.3)
                
                # Add trend line for throughput
                if len(throughputs) > 1:
                    z = np.polyfit(iterations, throughputs, 1)
                    p = np.poly1d(z)
                    axes[0, 0].plot(iterations, p(iterations), "--", alpha=0.5, color='black')
                
                plt.tight_layout()
                
                # Save visualization
                viz_filename = self.results_dir / f"{benchmark_name}_performance_analysis.png"
                plt.savefig(viz_filename, dpi=150, bbox_inches='tight')
                plt.close()
                
                print(f"Generated visualization: {viz_filename}")
                
        except Exception as e:
            print(f"Warning: Could not generate visualizations: {e}")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Generate NLM performance reports')
    parser.add_argument('results_dir', help='Directory containing benchmark results')
    parser.add_argument('baselines_dir', help='Directory containing baseline performance data')
    parser.add_argument('--output_dir', default='.', help='Output directory for reports')
    parser.add_argument('--generate_visualizations', action='store_true', 
                       help='Generate performance visualizations')
    
    args = parser.parse_args()
    
    # Create output directory if it doesn't exist
    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    
    generator = PerformanceReportsGenerator(args.results_dir, args.baselines_dir)
    
    # Load benchmark results
    benchmarks = generator.load_benchmark_results()
    
    if not benchmarks:
        print("No benchmark results found. Check your results directory.")
        return 1
    
    # Generate performance report
    performance_report = generator.generate_performance_report(benchmarks)
    generator.save_report(performance_report, "performance_report.txt")
    
    # Generate regression report
    regression_report = generator.generate_regression_report(benchmarks)
    generator.save_report(regression_report, "regression_report.txt")
    
    # Generate combined report
    combined_report = performance_report + "\n\n" + regression_report
    generator.save_report(combined_report, "combined_report.txt")
    
    # Generate visualizations if requested
    if args.generate_visualizations:
        generator.generate_visualizations(benchmarks)
    
    print("\nPerformance reports generated successfully!")
    print("Reports saved to:", output_dir.absolute())
    print("  - performance_report.txt")
    print("  - regression_report.txt")
    print("  - combined_report.txt")
    
    if args.generate_visualizations:
        print("  - Performance visualizations (*.png)")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
