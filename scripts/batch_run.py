#!/usr/bin/env python3
"""
NLM Batch Runner Script
Runs multiple simulations with different configurations.

This script provides batch processing capabilities for NLM simulations,
enabling automated execution of multiple simulations with different
parameters and configurations.
"""

import argparse
import subprocess
import os
import json
import time
from pathlib import Path
from typing import List, Dict, Any

# Add the src directory to the Python path
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'src'))

class NLMBatchRunner:
    """Batch runner for NLM simulations."""
    
    def __init__(self, output_dir: str = "batch_results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
    
    def run_simulation(self, preset: str, neurons: int, steps: int, 
                      output_dir: str, verbose: bool = False) -> bool:
        """Run a single simulation with specified parameters."""
        output_file = Path(output_dir) / f"{preset}_{neurons}_{steps}.json"
        
        cmd = [
            "nlm", "run",
            "--type", preset,
            "--steps", str(steps),
            "--neurons", str(neurons),
            "--output", str(output_file),
            "--verbose", "info" if verbose else "quiet"
        ]
        
        print(f"Running: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd, 
                capture_output=True, 
                text=True,
                timeout=300  # 5 minute timeout
            )
            
            if result.returncode == 0:
                print(f"✓ Completed: {output_file}")
                return True
            else:
                print(f"✗ Failed: {output_file}")
                if verbose:
                    print(f"Error: {result.stderr}")
                return False
                
        except subprocess.TimeoutExpired:
            print(f"✗ Timeout: {output_file}")
            return False
        except Exception as e:
            print(f"✗ Error: {e}")
            return False
    
    def run_batch(self, presets: List[str], neurons: List[int], 
                 steps: List[int], verbose: bool = False) -> Dict[str, Any]:
        """Run batch of simulations and return results."""
        results = {
            'total': 0,
            'completed': 0,
            'failed': 0,
            'execution_times': [],
            'failed_jobs': []
        }
        
        total_jobs = len(presets) * len(neurons) * len(steps)
        results['total'] = total_jobs
        
        print(f"Starting batch run: {total_jobs} simulations")
        print(f"Presets: {presets}")
        print(f"Neurons: {neurons}")
        print(f"Steps: {steps}")
        print("-" * 50)
        
        job_count = 0
        start_time = time.time()
        
        for preset in presets:
            for neuron_count in neurons:
                for step_count in steps:
                    job_count += 1
                    job_start = time.time()
                    
                    success = self.run_simulation(
                        preset, neuron_count, step_count, 
                        str(self.output_dir), verbose
                    )
                    
                    execution_time = time.time() - job_start
                    results['execution_times'].append(execution_time)
                    
                    if success:
                        results['completed'] += 1
                    else:
                        results['failed'] += 1
                        results['failed_jobs'].append({
                            'preset': preset,
                            'neurons': neuron_count,
                            'steps': step_count,
                            'execution_time': execution_time
                        })
                    
                    # Progress update
                    progress = (job_count / total_jobs) * 100
                    print(f"Progress: {progress:.1f}% ({job_count}/{total_jobs})")
        
        total_time = time.time() - start_time
        
        results['total_time'] = total_time
        results['average_time'] = total_time / total_jobs if total_jobs > 0 else 0
        
        print("-" * 50)
        print(f"Batch completed: {results['completed']}/{results['total']} successful")
        print(f"Total time: {total_time:.2f} seconds")
        print(f"Average time per simulation: {results['average_time']:.2f} seconds")
        
        if results['failed'] > 0:
            print(f"\nFailed jobs ({results['failed']}):")
            for job in results['failed_jobs']:
                print(f"  - {job['preset']}: {job['neurons']} neurons, {job['steps']} steps")
        
        return results

    def save_results(self, results: Dict[str, Any], filename: str = "batch_summary.json"):
        """Save batch results to JSON file."""
        filepath = self.output_dir / filename
        
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to: {filepath}")


def main():
    parser = argparse.ArgumentParser(
        description="Run batch of NLM simulations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python batch_run.py --presets basic plasticity --neurons 500 1000 --steps 100 500
  python batch_run.py --presets basic --neurons 500 --steps 100 --output-dir my_results
  python batch_run.py --presets plasticity development --neurons 1000 --steps 500 1000 --verbose
        """
    )
    
    parser.add_argument(
        "--presets", nargs="+", default=["basic", "plasticity"],
        help="Presets to run (default: basic plasticity)"
    )
    
    parser.add_argument(
        "--neurons", nargs="+", type=int, default=[500, 1000],
        help="Neuron counts to test (default: 500 1000)"
    )
    
    parser.add_argument(
        "--steps", nargs="+", type=int, default=[100, 500],
        help="Step counts to test (default: 100 500)"
    )
    
    parser.add_argument(
        "--output-dir", default="batch_results",
        help="Output directory for results (default: batch_results)"
    )
    
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--save-results", action="store_true",
        help="Save batch results to JSON file"
    )
    
    parser.add_argument(
        "--results-file", default="batch_summary.json",
        help="Filename for results JSON file (default: batch_summary.json)"
    )
    
    args = parser.parse_args()
    
    # Create batch runner
    runner = NLMBatchRunner(args.output_dir)
    
    # Run batch
    results = runner.run_batch(
        args.presets, 
        args.neurons, 
        args.steps, 
        args.verbose
    )
    
    # Save results if requested
    if args.save_results:
        runner.save_results(results, args.results_file)
    
    return 0

if __name__ == "__main__":
    exit(main())
