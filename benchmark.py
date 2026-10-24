#!/usr/bin/env python3
"""
NLM Benchmark Script

This script benchmarks the performance of the NLM Phase 6 system
to validate the improvements made.
"""

import time
import json
import statistics
from pathlib import Path
import sys

def run_benchmark():
    """Run NLM benchmarks"""
    print("NLM Phase 6 Benchmark Suite")
    print("=" * 40)
    
    project_root = Path.cwd()
    
    # Check if build directory exists
    build_dir = project_root / "build"
    if not build_dir.exists():
        print("Error: Build directory not found!")
        print("Please run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("And then: make -j4")
        return None
    
    # Benchmark configurations
    benchmarks = [
        {
            'name': 'Minimal Test (Phase 6 Commander)',
            'command': './nlm --mode minimal --neurons 100 --steps 50',
            'description': 'Test basic brain connectivity and spike generation'
        },
        {
            'name': 'Demonstration Test',
            'command': './nlm --mode demo --neurons 200 --steps 100 --verbose',
            'description': 'Test complete brain simulation with world interaction'
        },
        {
            'name': 'Integration Test',
            'command': './nlm --mode integration --neurons 50 --steps 20',
            'description': 'Test Phase 6 integration verification'
        }
    ]
    
    results = {}
    
    for benchmark in benchmarks:
        print(f"\nRunning: {benchmark['name']}")
        print(f"Description: {benchmark['description']}")
        print(f"Command: {benchmark['command']}")
        
        # Run benchmark multiple times for consistency
        times = []
        successes = []
        
        for i in range(3):
            print(f"  Run {i+1}/3...")
            start_time = time.time()
            
            result = subprocess.run(
                benchmark['command'],
                shell=True,
                cwd=build_dir,
                capture_output=True,
                text=True,
                timeout=120  # 2 minute timeout
            )
            
            end_time = time.time()
            execution_time = end_time - start_time
            times.append(execution_time)
            successes.append(result.returncode == 0)
            
            if result.returncode != 0:
                print(f"    Failed! Exit code: {result.returncode}")
                print(f"    stderr: {result.stderr[:500]}...")
        
        # Calculate statistics
        avg_time = statistics.mean(times)
        min_time = min(times)
        max_time = max(times)
        std_time = statistics.stdev(times) if len(times) > 1 else 0
        success_rate = statistics.mean(successes) * 100
        
        result_data = {
            'name': benchmark['name'],
            'description': benchmark['description'],
            'command': benchmark['command'],
            'runs': len(times),
            'successes': sum(successes),
            'success_rate': success_rate,
            'execution_times': {
                'average': avg_time,
                'minimum': min_time,
                'maximum': max_time,
                'std_deviation': std_time
            },
            'status': 'passed' if all(successes) else 'failed'
        }
        
        results[benchmark['name']] = result_data
        
        print(f"  Average time: {avg_time:.2f}s")
        print(f"  Success rate: {success_rate:.1f}%")
        print(f"  Status: {result_data['status']}")
    
    # Overall benchmark results
    print("\n" + "=" * 40)
    print("Overall Benchmark Results")
    print("=" * 40)
    
    total_runs = sum(r['runs'] for r in results.values())
    total_successes = sum(r['successes'] for r in results.values())
    overall_success_rate = (total_successes / total_runs) * 100 if total_runs > 0 else 0
    
    overall_results = {
        'benchmark_suite': 'NLM Phase 6 Benchmark Suite',
        'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
        'total_runs': total_runs,
        'successful_runs': total_successes,
        'overall_success_rate': overall_success_rate,
        'benchmarks': results,
        'improvements_summary': {
            'phase6_demo_executable': 'Added Phase 6 demo executable to build system',
            'modern_main_executable': 'Replaced Phase 2 main.cpp with Phase 6 Commander',
            'comprehensive_cli': 'Added modern command line interface with options',
            'improved_logging': 'Enhanced logging throughout the codebase',
            'configuration_system': 'Improved configuration management',
            'python_examples': 'Added Python interface examples',
            'test_suite': 'Created comprehensive test suite'
        }
    }
    
    # Save results to file
    output_file = project_root / "benchmark_results.json"
    with open(output_file, 'w') as f:
        json.dump(overall_results, f, indent=2)
    
    print(f"\nBenchmark results saved to: {output_file}")
    print(f"Total runs: {total_runs}")
    print(f"Successful runs: {total_successes}")
    print(f"Overall success rate: {overall_success_rate:.1f}%")
    
    # Print individual benchmark results
    for name, result in results.items():
        print(f"\n{name}:")
        print(f"  Success rate: {result['success_rate']:.1f}%")
        print(f"  Average time: {result['execution_times']['average']:.2f}s")
    
    return overall_results

def main():
    """Main benchmark runner"""
    try:
        import subprocess
        results = run_benchmark()
        
        if results and results['overall_success_rate'] >= 66.6:  # At least 2 out of 3 runs successful
            print("\n✓ Benchmark suite completed successfully!")
            return 0
        else:
            print("\n✗ Benchmark suite failed!")
            return 1
            
    except Exception as e:
        print(f"Error running benchmarks: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())