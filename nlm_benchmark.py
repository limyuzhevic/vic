#!/usr/bin/env python3
"""
NLM Benchmark Tool

This script provides benchmark testing capabilities for the NLM (Neural Learning Machine)
framework, allowing users to measure performance, validate configurations, and compare
experiment results.

Features:
- Benchmark core NLM operations (neuron simulation, plasticity, memory)
- Performance profiling of different brain systems
- Memory usage monitoring
- Experiment comparison
- Results analysis and reporting
"""

import argparse
import json
import time
import sys
import os
import statistics
from pathlib import Path
from typing import Dict, List, Any, Optional
import subprocess

class NLMBenchmark:
    def __init__(self, project_root: str = "/workspace/app"):
        self.project_root = Path(project_root)
        self.build_dir = self.project_root / "build"
        self.results_dir = self.project_root / "benchmark_results"
        self.results_dir.mkdir(exist_ok=True)
    
    def run_build(self) -> bool:
        """Build the NLM project"""
        print("Building NLM project...")
        try:
            # Run CMake
            os.makedirs(self.build_dir, exist_ok=True)
            subprocess.run([
                "cmake", "..", "-DCMAKE_BUILD_TYPE=Release"
            ], cwd=self.build_dir, check=True)
            
            # Run make
            subprocess.run([
                "make", "-j4"
            ], cwd=self.build_dir, check=True)
            
            print("✓ Build completed successfully")
            return True
        except subprocess.CalledProcessError as e:
            print(f"✗ Build failed: {e}")
            return False
        except Exception as e:
            print(f"✗ Error during build: {e}")
            return False
    
    def run_phase2_demo(self, iterations: int = 10) -> Dict[str, Any]:
        """Run Phase 2 demo multiple times for benchmarking"""
        print(f"Running Phase 2 demo ({iterations} iterations)...")
        
        results = {
            'iterations': iterations,
            'success_count': 0,
            'failure_count': 0,
            'total_time': 0.0,
            'avg_time': 0.0,
            'times': [],
            'results': []
        }
        
        for i in range(iterations):
            try:
                print(f"  Iteration {i + 1}/{iterations}...")
                
                # Run the nlm executable
                start_time = time.time()
                process = subprocess.run([
                    str(self.build_dir / "nlm"),
                    "--experiment", "learning",
                    "--log-level", "error",  # Reduce output for benchmarking
                    "--quiet"
                ], cwd=self.project_root, 
                capture_output=True, 
                text=True,
                timeout=60)  # 60 second timeout
                
                end_time = time.time()
                elapsed = end_time - start_time
                
                if process.returncode == 0:
                    results['success_count'] += 1
                    results['times'].append(elapsed)
                    results['results'].append({
                        'iteration': i + 1,
                        'success': True,
                        'time': elapsed,
                        'stdout': process.stdout,
                        'stderr': process.stderr
                    })
                else:
                    results['failure_count'] += 1
                    results['results'].append({
                        'iteration': i + 1,
                        'success': False,
                        'error': f"Process failed with return code {process.returncode}",
                        'stdout': process.stdout,
                        'stderr': process.stderr
                    })
                
                results['total_time'] += elapsed
                
            except subprocess.TimeoutExpired:
                print(f"  ✗ Iteration {i + 1} timed out")
                results['failure_count'] += 1
                results['results'].append({
                    'iteration': i + 1,
                    'success': False,
                    'error': 'Timeout expired'
                })
            
            except Exception as e:
                print(f"  ✗ Iteration {i + 1} failed: {e}")
                results['failure_count'] += 1
                results['results'].append({
                    'iteration': i + 1,
                    'success': False,
                    'error': str(e)
                })
        
        # Calculate statistics
        if results['times']:
            results['avg_time'] = statistics.mean(results['times'])
            results['median_time'] = statistics.median(results['times'])
            results['min_time'] = min(results['times'])
            results['max_time'] = max(results['times'])
            
            if len(results['times']) > 1:
                results['std_dev'] = statistics.stdev(results['times'])
                results['variance'] = statistics.variance(results['times'])
        
        return results
    
    def run_phase6_demo(self, iterations: int = 5, neurons: int = 500, steps: int = 2000) -> Dict[str, Any]:
        """Run Phase 6 demo for benchmarking"""
        print(f"Running Phase 6 demo ({iterations} iterations, {neurons} neurons, {steps} steps)...")
        
        results = {
            'iterations': iterations,
            'success_count': 0,
            'failure_count': 0,
            'total_time': 0.0,
            'avg_time': 0.0,
            'times': [],
            'results': []
        }
        
        for i in range(iterations):
            try:
                print(f"  Iteration {i + 1}/{iterations}...")
                
                # Run the nlm_phase6_demo executable
                start_time = time.time()
                process = subprocess.run([
                    str(self.build_dir / "nlm_phase6_demo"),
                    "--mode", "full",
                    "--neurons", str(neurons),
                    "--steps", str(steps),
                    "--quiet"
                ], cwd=self.project_root,
                capture_output=True,
                text=True,
                timeout=120)  # 120 second timeout
                
                end_time = time.time()
                elapsed = end_time - start_time
                
                if process.returncode == 0:
                    results['success_count'] += 1
                    results['times'].append(elapsed)
                    results['results'].append({
                        'iteration': i + 1,
                        'success': True,
                        'time': elapsed,
                        'stdout': process.stdout,
                        'stderr': process.stderr
                    })
                else:
                    results['failure_count'] += 1
                    results['results'].append({
                        'iteration': i + 1,
                        'success': False,
                        'error': f"Process failed with return code {process.returncode}",
                        'stdout': process.stdout,
                        'stderr': process.stderr
                    })
                
                results['total_time'] += elapsed
                
            except subprocess.TimeoutExpired:
                print(f"  ✗ Iteration {i + 1} timed out")
                results['failure_count'] += 1
                results['results'].append({
                    'iteration': i + 1,
                    'success': False,
                    'error': 'Timeout expired'
                })
            
            except Exception as e:
                print(f"  ✗ Iteration {i + 1} failed: {e}")
                results['failure_count'] += 1
                results['results'].append({
                    'iteration': i + 1,
                    'success': False,
                    'error': str(e)
                })
        
        # Calculate statistics
        if results['times']:
            results['avg_time'] = statistics.mean(results['times'])
            results['median_time'] = statistics.median(results['times'])
            results['min_time'] = min(results['times'])
            results['max_time'] = max(results['times'])
            
            if len(results['times']) > 1:
                results['std_dev'] = statistics.stdev(results['times'])
                results['variance'] = statistics.variance(results['times'])
        
        return results
    
    def run_config_validation(self, config_files: List[str], iterations: int = 1000) -> Dict[str, Any]:
        """Benchmark configuration validation performance"""
        print(f"Running configuration validation benchmark ({iterations} iterations)...")
        
        results = {
            'config_files': config_files,
            'iterations': iterations,
            'total_time': 0.0,
            'avg_time': 0.0,
            'files': {}
        }
        
        for config_file in config_files:
            file_path = Path(config_file)
            if not file_path.exists():
                print(f"✗ Configuration file not found: {config_file}")
                continue
            
            try:
                with open(file_path, 'r') as f:
                    content = f.read()
                
                # Measure validation time
                start_time = time.time()
                
                for _ in range(iterations):
                    try:
                        json.loads(content)
                    except json.JSONDecodeError:
                        pass  # Expected for non-JSON files
                
                end_time = time.time()
                total_time = end_time - start_time
                avg_time = (total_time / iterations) * 1000  # Convert to milliseconds
                
                results['files'][str(file_path)] = {
                    'total_time': total_time,
                    'avg_time_ms': avg_time,
                    'validations_per_second': iterations / total_time
                }
                
                print(f"  {file_path.name}: {avg_time:.4f} ms avg, {iterations / total_time:.0f} validations/sec")
                
            except Exception as e:
                print(f"  ✗ Error benchmarking {config_file}: {e}")
        
        # Calculate overall statistics
        if results['files']:
            all_times = [stats['avg_time_ms'] for stats in results['files'].values()]
            results['avg_time'] = statistics.mean(all_times)
            results['min_time'] = min(all_times)
            results['max_time'] = max(all_times)
        
        return results
    
    def save_results(self, benchmark_type: str, results: Dict[str, Any]) -> str:
        """Save benchmark results to file"""
        timestamp = int(time.time())
        filename = f"{benchmark_type}_benchmark_{timestamp}.json"
        filepath = self.results_dir / filename
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"✓ Results saved to: {filepath}")
        return str(filepath)
    
    def print_results_summary(self, benchmark_type: str, results: Dict[str, Any]):
        """Print benchmark results summary"""
        print(f"\n{'='*60}")
        print(f"NLM {benchmark_type.capitalize()} Benchmark Results")
        print(f"{'='*60}")
        
        if benchmark_type == "phase2_demo":
            print(f"Iterations: {results['iterations']}")
            print(f"Successful: {results['success_count']}")
            print(f"Failed: {results['failure_count']}")
            print(f"Success Rate: {results['success_count']/results['iterations']*100:.1f}%")
            
            if results['times']:
                print(f"\nTiming Statistics:")
                print(f"  Average: {results['avg_time']:.3f}s")
                print(f"  Median: {results['median_time']:.3f}s")
                print(f"  Minimum: {results['min_time']:.3f}s")
                print(f"  Maximum: {results['max_time']:.3f}s")
                
                if 'std_dev' in results:
                    print(f"  Standard Deviation: {results['std_dev']:.3f}s")
        
        elif benchmark_type == "phase6_demo":
            print(f"Iterations: {results['iterations']}")
            print(f"Successful: {results['success_count']}")
            print(f"Failed: {results['failure_count']}")
            print(f"Success Rate: {results['success_count']/results['iterations']*100:.1f}%")
            
            if results['times']:
                print(f"\nTiming Statistics:")
                print(f"  Average: {results['avg_time']:.3f}s")
                print(f"  Median: {results['median_time']:.3f}s")
                print(f"  Minimum: {results['min_time']:.3f}s")
                print(f"  Maximum: {results['max_time']:.3f}s")
                
                if 'std_dev' in results:
                    print(f"  Standard Deviation: {results['std_dev']:.3f}s")
        
        elif benchmark_type == "config_validation":
            print(f"Configuration Files: {len(results['files'])}")
            print(f"Iterations per file: {results['iterations']}")
            
            if 'avg_time' in results:
                print(f"\nPerformance Statistics:")
                print(f"  Average validation time: {results['avg_time']:.4f} ms")
                print(f"  Minimum: {results['min_time']:.4f} ms")
                print(f"  Maximum: {results['max_time']:.4f} ms")
        
        print(f"{'='*60}")

def main():
    parser = argparse.ArgumentParser(
        description="NLM Benchmark Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_benchmark --build
  nlm_benchmark --phase2-demo --iterations 10
  nlm_benchmark --phase6-demo --iterations 5 --neurons 1000 --steps 5000
  nlm_benchmark --config-validation configs/default.cfg
  nlm_benchmark --all
        ""
    )
    
    # Main action group
    action_group = parser.add_mutually_exclusive_group(required=True)
    action_group.add_argument('--build', action='store_true',
                             help='Build the NLM project')
    action_group.add_argument('--phase2-demo', action='store_true',
                             help='Benchmark Phase 2 demo')
    action_group.add_argument('--phase6-demo', action='store_true',
                             help='Benchmark Phase 6 demo')
    action_group.add_argument('--config-validation', action='store_true',
                             help='Benchmark configuration validation')
    action_group.add_argument('--all', action='store_true',
                             help='Run all benchmarks')
    
    # Common arguments
    parser.add_argument('--iterations', type=int, default=10,
                       help='Number of iterations for benchmarks (default: 10)')
    parser.add_argument('--neurons', type=int, default=500,
                       help='Number of neurons for Phase 6 benchmark (default: 500)')
    parser.add_argument('--steps', type=int, default=2000,
                       help='Maximum steps for Phase 6 benchmark (default: 2000)')
    parser.add_argument('--config-files', nargs='+',
                       help='Configuration files for validation benchmark')
    parser.add_argument('--output-dir', default="/workspace/app/benchmark_results",
                       help='Output directory for benchmark results')
    
    args = parser.parse_args()
    
    # Initialize benchmark tool
    benchmark = NLMBenchmark()
    
    # Route to appropriate benchmark
    try:
        if args.build:
            success = benchmark.run_build()
            if success:
                print("✓ Build completed successfully")
                return True
            else:
                print("✗ Build failed")
                return False
        
        elif args.phase2_demo:
            results = benchmark.run_phase2_demo(args.iterations)
            benchmark.save_results("phase2_demo", results)
            benchmark.print_results_summary("phase2_demo", results)
            return True
        
        elif args.phase6_demo:
            results = benchmark.run_phase6_demo(args.iterations, args.neurons, args.steps)
            benchmark.save_results("phase6_demo", results)
            benchmark.print_results_summary("phase6_demo", results)
            return True
        
        elif args.config_validation:
            if not args.config_files:
                print("✗ --config-validation requires --config-files argument")
                return False
            
            results = benchmark.run_config_validation(args.config_files, args.iterations)
            benchmark.save_results("config_validation", results)
            benchmark.print_results_summary("config_validation", results)
            return True
        
        elif args.all:
            print("Running all benchmarks...")
            
            # Build
            if not benchmark.run_build():
                return False
            
            # Phase 2 demo
            print("\n" + "="*60)
            print("PHASE 2 DEMO BENCHMARK")
            print("="*60)
            phase2_results = benchmark.run_phase2_demo(5)
            phase2_file = benchmark.save_results("phase2_demo", phase2_results)
            
            # Phase 6 demo
            print("\n" + "="*60)
            print("PHASE 6 DEMO BENCHMARK")
            print("="*60)
            phase6_results = benchmark.run_phase6_demo(3, 500, 2000)
            phase6_file = benchmark.save_results("phase6_demo", phase6_results)
            
            # Config validation (if config files exist)
            default_config = "/workspace/app/configs/default.cfg"
            if os.path.exists(default_config):
                print("\n" + "="*60)
                print("CONFIGURATION VALIDATION BENCHMARK")
                print("="*60)
                config_results = benchmark.run_config_validation([default_config], 1000)
                config_file = benchmark.save_results("config_validation", config_results)
            else:
                print(f"\n⚠ Configuration file not found: {default_config}")
                print("  Skipping configuration validation benchmark")
                config_file = None
            
            # Summary
            print("\n" + "="*60)
            print("BENCHMARK SUMMARY")
            print("="*60)
            print(f"Phase 2 Demo Results: {phase2_file}")
            print(f"Phase 6 Demo Results: {phase6_file}")
            if config_file:
                print(f"Config Validation Results: {config_file}")
            
            return True
        
        else:
            parser.print_help()
            return False
    
    except Exception as e:
        print(f"✗ Benchmark failed: {e}")
        return False

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
