#!/usr/bin/env python3
"""
Benchmark script for NLM Phase 6 brain simulation performance testing.
Provides standardized benchmarks and performance validation.
"""

import subprocess
import time
import json
import argparse
import os
from pathlib import Path
from typing import Dict, List, Optional
import sys
from datetime import datetime

class NLMBenchmark:
    """Benchmark suite for NLM Phase 6 brain simulation."""
    
    def __init__(self, project_root: Path):
        self.project_root = project_root
        self.benchmark_results = {}
        
    def run_connectivity_test(self, steps: int = 1000) -> Dict:
        """Run connectivity test and measure performance."""
        print(f"Running connectivity test with {steps} steps...")
        
        start_time = time.time()
        
        # Build and run the binary
        binary_path = self.project_root / "build" / "bin" / "nlm_phase6_demo"
        if not binary_path.exists():
            print("Warning: NLM binary not found, using Python fallback")
            return self._run_connectivity_test_fallback(steps)
            
        # Run with connectivity test
        cmd = [
            str(binary_path),
            "--max-steps", str(steps),
            "--no-plasticity-test",
            "--no-stdp-test",
            "--verbose"
        ]
        
        try:
            process = subprocess.Popen(cmd, 
                                     stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE,
                                     text=True)
            
            stdout, stderr = process.communicate(timeout=steps * 0.001 + 30)  # timeout
            
            end_time = time.time()
            wall_time = end_time - start_time
            
            result = {
                "test_type": "connectivity",
                "steps": steps,
                "wall_time": wall_time,
                "steps_per_second": steps / wall_time,
                "exit_code": process.returncode,
                "stdout": stdout,
                "stderr": stderr,
                "timestamp": datetime.now().isoformat()
            }
            
            print(f"Connectivity test completed in {wall_time:.2f}s ({steps/wall_time:.1f} steps/sec)")
            return result
            
        except subprocess.TimeoutExpired:
            process.kill()
            print("Warning: Connectivity test timed out")
            return {"error": "timeout", "steps": steps}
        except Exception as e:
            print(f"Error running connectivity test: {e}")
            return {"error": str(e), "steps": steps}
            
    def run_plasticity_test(self, steps: int = 1000) -> Dict:
        """Run plasticity test and measure performance."""
        print(f"Running plasticity test with {steps} steps...")
        
        start_time = time.time()
        
        binary_path = self.project_root / "build" / "bin" / "nlm_phase6_demo"
        if not binary_path.exists():
            return self._run_plasticity_test_fallback(steps)
            
        cmd = [
            str(binary_path),
            "--max-steps", str(steps),
            "--no-connectivity-test",
            "--no-stdp-test",
            "--verbose"
        ]
        
        try:
            process = subprocess.Popen(cmd,
                                     stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE,
                                     text=True)
            
            stdout, stderr = process.communicate(timeout=steps * 0.001 + 60)
            
            end_time = time.time()
            wall_time = end_time - start_time
            
            result = {
                "test_type": "plasticity",
                "steps": steps,
                "wall_time": wall_time,
                "steps_per_second": steps / wall_time,
                "exit_code": process.returncode,
                "stdout": stdout,
                "stderr": stderr,
                "timestamp": datetime.now().isoformat()
            }
            
            print(f"Plasticity test completed in {wall_time:.2f}s ({steps/wall_time:.1f} steps/sec)")
            return result
            
        except subprocess.TimeoutExpired:
            process.kill()
            print("Warning: Plasticity test timed out")
            return {"error": "timeout", "steps": steps}
        except Exception as e:
            print(f"Error running plasticity test: {e}")
            return {"error": str(e), "steps": steps}
            
    def run_stdp_test(self, steps: int = 1000) -> Dict:
        """Run STDP test and measure performance."""
        print(f"Running STDP test with {steps} steps...")
        
        start_time = time.time()
        
        binary_path = self.project_root / "build" / "bin" / "nlm_phase6_demo"
        if not binary_path.exists():
            return self._run_stdp_test_fallback(steps)
            
        cmd = [
            str(binary_path),
            "--max-steps", str(steps),
            "--no-connectivity-test",
            "--no-plasticity-test",
            "--verbose"
        ]
        
        try:
            process = subprocess.Popen(cmd,
                                     stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE,
                                     text=True)
            
            stdout, stderr = process.communicate(timeout=steps * 0.001 + 60)
            
            end_time = time.time()
            wall_time = end_time - start_time
            
            result = {
                "test_type": "stdp",
                "steps": steps,
                "wall_time": wall_time,
                "steps_per_second": steps / wall_time,
                "exit_code": process.returncode,
                "stdout": stdout,
                "stderr": stderr,
                "timestamp": datetime.now().isoformat()
            }
            
            print(f"STDP test completed in {wall_time:.2f}s ({steps/wall_time:.1f} steps/sec)")
            return result
            
        except subprocess.TimeoutExpired:
            process.kill()
            print("Warning: STDP test timed out")
            return {"error": "timeout", "steps": steps}
        except Exception as e:
            print(f"Error running STDP test: {e}")
            return {"error": str(e), "steps": steps}
            
    def _run_connectivity_test_fallback(self, steps: int) -> Dict:
        """Fallback implementation using Python for connectivity test."""
        print("Using Python fallback for connectivity test")
        start_time = time.time()
        
        # Simulate work for benchmarking
        time.sleep(min(steps * 0.001, 10))  # Cap at 10 seconds
        
        end_time = time.time()
        wall_time = end_time - start_time
        
        return {
            "test_type": "connectivity_fallback",
            "steps": steps,
            "wall_time": wall_time,
            "steps_per_second": steps / wall_time,
            "error": "fallback_mode",
            "timestamp": datetime.now().isoformat()
        }
        
    def _run_plasticity_test_fallback(self, steps: int) -> Dict:
        """Fallback implementation using Python for plasticity test."""
        print("Using Python fallback for plasticity test")
        start_time = time.time()
        
        time.sleep(min(steps * 0.0015, 15))  # Simulate more work for plasticity
        
        end_time = time.time()
        wall_time = end_time - start_time
        
        return {
            "test_type": "plasticity_fallback",
            "steps": steps,
            "wall_time": wall_time,
            "steps_per_second": steps / wall_time,
            "error": "fallback_mode",
            "timestamp": datetime.now().isoformat()
        }
        
    def _run_stdp_test_fallback(self, steps: int) -> Dict:
        """Fallback implementation using Python for STDP test."""
        print("Using Python fallback for STDP test")
        start_time = time.time()
        
        time.sleep(min(steps * 0.002, 20))  # Simulate most work for STDP
        
        end_time = time.time()
        wall_time = end_time - start_time
        
        return {
            "test_type": "stdp_fallback",
            "steps": steps,
            "wall_time": wall_time,
            "steps_per_second": steps / wall_time,
            "error": "fallback_mode",
            "timestamp": datetime.now().isoformat()
        }
        
    def run_memory_test(self) -> Dict:
        """Run memory usage test."""
        print("Running memory usage test...")
        
        import psutil
        import gc
        
        # Measure baseline memory
        gc.collect()
        process = psutil.Process()
        baseline_memory = process.memory_info().rss / (1024 * 1024)  # MB
        
        start_time = time.time()
        
        # Run a series of operations to measure memory growth
        data = []
        for i in range(1000):
            # Simulate brain data structures
            data.append({
                'neurons': [i % 1000 for _ in range(100)],
                'synapses': [(j, k) for j in range(50) for k in range(50)],
                'weights': [0.5] * 2500
            })
            
            if i % 100 == 0:
                gc.collect()
                
        end_time = time.time()
        
        # Measure final memory
        gc.collect()
        final_memory = process.memory_info().rss / (1024 * 1024)  # MB
        peak_memory = process.memory_info().vms / (1024 * 1024)  # MB
        
        wall_time = end_time - start_time
        
        result = {
            "test_type": "memory",
            "baseline_memory": baseline_memory,
            "final_memory": final_memory,
            "peak_memory": peak_memory,
            "memory_growth": final_memory - baseline_memory,
            "wall_time": wall_time,
            "memory_efficiency": (len(data) * 1024) / (final_memory * 1024 * 1024),  # data size / memory
            "timestamp": datetime.now().isoformat()
        }
        
        print(f"Memory test completed: {final_memory:.1f} MB peak, "
              f"{result['memory_growth']:.1f} MB growth")
        return result
        
    def run_scaling_test(self, step_counts: List[int]) -> Dict:
        """Run scaling tests with different step counts."""
        print(f"Running scaling tests with step counts: {step_counts}")
        
        results = {}
        for steps in step_counts:
            print(f"\nTesting with {steps} steps...")
            result = self.run_connectivity_test(steps)
            results[f"{steps}_steps"] = result
            
        # Analyze scaling efficiency
        scaling_analysis = self._analyze_scaling_results(results)
        
        return {
            "test_type": "scaling",
            "individual_results": results,
            "scaling_analysis": scaling_analysis,
            "timestamp": datetime.now().isoformat()
        }
        
    def _analyze_scaling_results(self, results: Dict) -> Dict:
        """Analyze scaling performance from benchmark results."""
        if len(results) < 2:
            return {"error": "need at least 2 data points for scaling analysis"}
            
        steps = []
        times = []
        
        for key, result in results.items():
            if 'error' not in result:
                try:
                    step_count = int(key.replace('_steps', ''))
                    steps.append(step_count)
                    times.append(result.get('wall_time', 0))
                except:
                    continue
                    
        if len(steps) < 2:
            return {"error": "no valid scaling data available"}
            
        # Calculate scaling efficiency
        efficiency_scores = []
        for i in range(1, len(steps)):
            step_ratio = steps[i] / steps[0]
            time_ratio = times[i] / times[0]
            efficiency = step_ratio / time_ratio if time_ratio > 0 else 0
            efficiency_scores.append(efficiency)
            
        avg_efficiency = sum(efficiency_scores) / len(efficiency_scores) if efficiency_scores else 0
        
        return {
            "total_steps": steps,
            "total_times": times,
            "average_efficiency": avg_efficiency,
            "efficiency_scores": efficiency_scores,
            "scaling_classified": self._classify_scaling(efficiency_scores)
        }
        
    def _classify_scaling(self, efficiencies: List[float]) -> str:
        """Classify scaling efficiency."""
        if not efficiencies:
            return "unknown"
            
        avg_efficiency = sum(efficiencies) / len(efficiencies)
        
        if avg_efficiency > 0.9:
            return "linear"  # Near perfect linear scaling
        elif avg_efficiency > 0.7:
            return "sublinear"  # Good scaling but not linear
        elif avg_efficiency > 0.5:
            return "moderate"  # Reasonable scaling
        else:
            return "poor"  # Poor scaling
            
    def run_full_benchmark_suite(self, step_counts: List[int] = None) -> Dict:
        """Run complete benchmark suite."""
        if step_counts is None:
            step_counts = [1000, 5000, 10000]
            
        print("Starting NLM Phase 6 Full Benchmark Suite")
        print("=" * 50)
        
        results = {}
        
        # Run individual tests
        results['connectivity'] = self.run_connectivity_test(step_counts[0])
        results['plasticity'] = self.run_plasticity_test(step_counts[0])
        results['stdp'] = self.run_stdp_test(step_counts[0])
        results['memory'] = self.run_memory_test()
        results['scaling'] = self.run_scaling_test(step_counts)
        
        # Calculate overall performance metrics
        results['overall_performance'] = self._calculate_overall_performance(results)
        
        print("\n" + "=" * 50)
        print("Benchmark Suite Complete")
        print(f"Results saved with timestamp: {datetime.now().isoformat()}")
        
        return results
        
    def _calculate_overall_performance(self, results: Dict) -> Dict:
        """Calculate overall performance metrics."""
        performance_metrics = []
        
        for test_type, result in results.items():
            if test_type not in ['overall_performance', 'scaling'] and 'error' not in result:
                if 'steps_per_second' in result:
                    performance_metrics.append({
                        'test_type': test_type,
                        'performance': result['steps_per_second'],
                        'steps': result.get('steps', 0)
                    })
        
        if not performance_metrics:
            return {"error": "no valid performance metrics"}
            
        # Calculate average performance
        avg_performance = sum(m['performance'] for m in performance_metrics) / len(performance_metrics)
        
        # Determine performance rating
        if avg_performance > 1000:
            rating = "excellent"
        elif avg_performance > 500:
            rating = "good"
        elif avg_performance > 100:
            rating = "fair"
        else:
            rating = "poor"
            
        return {
            "average_steps_per_second": avg_performance,
            "performance_rating": rating,
            "individual_metrics": performance_metrics,
            "total_tests": len(performance_metrics)
        }
        
    def save_results(self, results: Dict, output_file: str = None) -> str:
        """Save benchmark results to file."""
        if output_file is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            output_file = f"nlm_benchmark_{timestamp}.json"
            
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2, default=str)
            
        print(f"Benchmark results saved to: {output_file}")
        return output_file
        
    def generate_report(self, results: Dict, output_file: str = None) -> str:
        """Generate human-readable benchmark report."""
        if output_file is None:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            output_file = f"nlm_benchmark_report_{timestamp}.md"
            
        report_lines = [
            "# NLM Phase 6 Benchmark Report",
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            "",
            "## Executive Summary",
            "",
        ]
        
        overall = results.get('overall_performance', {})
        if 'average_steps_per_second' in overall:
            report_lines.extend([
                f"- **Average Performance**: {overall['average_steps_per_second']:.1f} steps/second",
                f"- **Performance Rating**: {overall['performance_rating'].upper()}",
                f"- **Total Tests**: {overall['total_tests']}",
                ""
            ])
            
        # Individual test results
        report_lines.extend([
            "## Individual Test Results",
            "",
            "| Test Type | Steps | Time (s) | Steps/sec | Rating |",
            "|-----------|-------|----------|-----------|--------|",
        ])
        
        for test_type, result in results.items():
            if test_type not in ['overall_performance', 'scaling', 'individual_results'] and 'error' not in result:
                steps = result.get('steps', 0)
                wall_time = result.get('wall_time', 0)
                steps_per_sec = result.get('steps_per_second', 0)
                
                # Simple rating based on steps/sec
                if steps_per_sec > 1000:
                    rating = "Excellent"
                elif steps_per_sec > 500:
                    rating = "Good"
                elif steps_per_sec > 100:
                    rating = "Fair"
                else:
                    rating = "Poor"
                    
                report_lines.append(
                    f"| {test_type.title()} | {steps} | {wall_time:.2f} | "
                    f"{steps_per_sec:.1f} | {rating} |"
                )
                
        # Scaling analysis
        scaling_result = results.get('scaling', {})
        if 'scaling_analysis' in scaling_result:
            analysis = scaling_result['scaling_analysis']
            report_lines.extend([
                "",
                "## Scaling Analysis",
                "",
                f"- **Average Scaling Efficiency**: {analysis.get('average_efficiency', 0):.2f}",
                f"- **Scaling Classification**: {analysis.get('scaling_classified', 'unknown').upper()}",
                ""
            ])
            
        # Write report
        with open(output_file, 'w') as f:
            f.write("\n".join(report_lines))
            
        print(f"Benchmark report generated: {output_file}")
        return output_file

def main():
    parser = argparse.ArgumentParser(description="NLM Phase 6 Benchmark Suite")
    parser.add_argument("--steps", type=int, nargs='+', default=[1000, 5000, 10000],
                       help="Step counts for testing (default: 1000 5000 10000)")
    parser.add_argument("--output", default="nlm_benchmark.json",
                       help="Output file for benchmark results")
    parser.add_argument("--report", default="nlm_benchmark_report.md",
                       help="Output file for human-readable report")
    parser.add_argument("--quick", action="store_true",
                       help="Run quick benchmark with minimal tests")
    
    args = parser.parse_args()
    
    project_root = Path(__file__).parent.parent
    benchmark = NLMBenchmark(project_root)
    
    if args.quick:
        # Quick benchmark with minimal tests
        print("Running quick benchmark suite...")
        results = {
            'connectivity': benchmark.run_connectivity_test(1000),
            'plasticity': benchmark.run_plasticity_test(1000),
            'stdp': benchmark.run_stdp_test(1000),
            'memory': benchmark.run_memory_test()
        }
    else:
        # Full benchmark suite
        print("Running full benchmark suite...")
        results = benchmark.run_full_benchmark_suite(args.steps)
        
    # Save results
    benchmark.save_results(results, args.output)
    
    # Generate report
    benchmark.generate_report(results, args.report)
    
    print("\n" + "=" * 50)
    print("Benchmark Suite Complete!")
    print(f"Results: {args.output}")
    print(f"Report: {args.report}")

if __name__ == "__main__":
    main()
