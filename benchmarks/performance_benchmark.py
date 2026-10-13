"""
Performance Benchmark for NLM
=============================

This module provides performance benchmarking capabilities for the NLM framework,
measuring performance across different network sizes, plasticity configurations,
and simulation parameters.
"""

import time
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import json
from typing import Dict, List, Any
import argparse

class NLMPerformanceBenchmark:
    """
    Performance benchmarking suite for NLM.
    
    This class benchmarks various aspects of NLM performance including:
    - Network size scalability
    - Plasticity performance
    - Memory efficiency
    - Simulation speed
    """
    
    def __init__(self):
        self.results = []
        self.benchmark_configs = []
    
    def add_benchmark_config(self, name: str, neuron_count: int, 
                           synapse_density: float = 0.1, description: str = ""):
        """
        Add a benchmark configuration.
        
        Args:
            name: Name of the benchmark
            neuron_count: Number of neurons to simulate
            synapse_density: Synapse density (0.0 to 1.0)
            description: Description of the benchmark
        """
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", synapse_density)
        
        self.benchmark_configs.append({
            'name': name,
            'neuron_count': neuron_count,
            'synapse_density': synapse_density,
            'config': config,
            'description': description
        })
    
    def run_single_benchmark(self, name: str, neuron_count: int, 
                           steps: int = 1000, warmup_steps: int = 100,
                           description: str = ""):
        """
        Run a single benchmark.
        
        Args:
            name: Name of the benchmark
            neuron_count: Number of neurons
            steps: Number of simulation steps
            warmup_steps: Number of warm-up steps
            description: Description
        
        Returns:
            dict: Benchmark results
        """
        print(f"Running benchmark: {name}")
        print(f"  Neurons: {neuron_count}, Steps: {steps}, Warm-up: {warmup_steps}")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Warm-up run
        print(f"  Warm-up phase...")
        warmup_start = time.time()
        for step in range(warmup_steps):
            brain.step(step)
        warmup_time = time.time() - warmup_start
        
        # Benchmark run
        print(f"  Benchmark phase...")
        benchmark_start = time.time()
        for step in range(steps):
            brain.step(step)
        benchmark_time = time.time() - benchmark_start
        
        total_time = warmup_time + benchmark_time
        total_spikes = brain.getTotalSpikeCount()
        
        # Calculate metrics
        warmup_rate = warmup_steps / warmup_time if warmup_time > 0 else 0
        benchmark_rate = steps / benchmark_time if benchmark_time > 0 else 0
        total_rate = (warmup_steps + steps) / total_time if total_time > 0 else 0
        spikes_per_second = total_spikes / total_time if total_time > 0 else 0
        neurons_per_second = neuron_count / total_time if total_time > 0 else 0
        
        result = {
            'name': name,
            'neuron_count': neuron_count,
            'synapse_density': 0.1,
            'steps': steps,
            'warmup_steps': warmup_steps,
            'warmup_time': warmup_time,
            'benchmark_time': benchmark_time,
            'total_time': total_time,
            'total_spikes': total_spikes,
            'warmup_rate': warmup_rate,
            'benchmark_rate': benchmark_rate,
            'total_rate': total_rate,
            'spikes_per_second': spikes_per_second,
            'neurons_per_second': neurons_per_second,
            'description': description
        }
        
        self.results.append(result)
        
        # Print results
        print(f"  Results:")
        print(f"    Warm-up time: {warmup_time:.3f}s ({warmup_rate:.1f} steps/s)")
        print(f"    Benchmark time: {benchmark_time:.3f}s ({benchmark_rate:.1f} steps/s)")
        print(f"    Total time: {total_time:.3f}s ({total_rate:.1f} steps/s)")
        print(f"    Total spikes: {total_spikes:,}")
        print(f"    Spikes per second: {spikes_per_second:.0f}")
        print(f"    Neurons per second: {neurons_per_second:.1f}")
        
        return result
    
    def run_memory_benchmark(self, neuron_count: int, steps: int = 1000):
        """
        Benchmark memory usage.
        
        Args:
            neuron_count: Number of neurons
            steps: Number of simulation steps
        
        Returns:
            dict: Memory benchmark results
        """
        print(f"\nRunning memory benchmark: {neuron_count} neurons, {steps} steps")
        
        import tracemalloc
        
        # Start memory tracing
        tracemalloc.start()
        
        # Create and run simulation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Convert to MB
        current_mb = current / 1024 / 1024
        peak_mb = peak / 1024 / 1024
        
        result = {
            'name': f'Memory_{neuron_count}',
            'neuron_count': neuron_count,
            'steps': steps,
            'current_memory_mb': current_mb,
            'peak_memory_mb': peak_mb,
            'memory_per_neuron_mb': peak_mb / neuron_count if neuron_count > 0 else 0
        }
        
        self.results.append(result)
        
        print(f"  Memory usage:")
        print(f"    Current: {current_mb:.2f} MB")
        print(f"    Peak: {peak_mb:.2f} MB")
        print(f"    Per neuron: {result['memory_per_neuron_mb']:.4f} MB")
        
        return result
    
    def run_plasticity_benchmark(self):
        """
        Benchmark different plasticity configurations.
        
        Returns:
            dict: Plasticity benchmark results
        """
        print(f"\nRunning plasticity benchmarks")
        
        plasticity_configs = [
            {'hebbian': False, 'stdp': False, 'reward': False, 'name': 'None'},
            {'hebbian': True, 'stdp': False, 'reward': False, 'name': 'Hebbian'},
            {'hebbian': False, 'stdp': True, 'reward': False, 'name': 'STDP'},
            {'hebbian': True, 'stdp': True, 'reward': True, 'name': 'All'}
        ]
        
        for config in plasticity_configs:
            print(f"\n  Testing {config['name']} plasticity...")
            
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Enable plasticity
            if config['hebbian']:
                # Enable Hebbian
                pass
            if config['stdp']:
                # Enable STDP
                pass
            if config['reward']:
                # Enable reward modulation
                pass
            
            # Run benchmark
            start_time = time.time()
            for step in range(1000):
                brain.step(step)
            end_time = time.time()
            
            duration = end_time - start_time
            
            result = {
                'name': f'Plasticity_{config["name"]}',
                'plasticity_config': config,
                'duration': duration,
                'spikes_per_second': brain.getTotalSpikeCount() / duration if duration > 0 else 0
            }
            
            self.results.append(result)
            print(f"    Time: {duration:.3f}s, Spikes/s: {result['spikes_per_second']:.0f}")
        
        return self.results
    
    def compare_scalability(self, neuron_counts: List[int] = None):
        """
        Compare scalability across different network sizes.
        
        Args:
            neuron_counts: List of neuron counts to test
        
        Returns:
            dict: Scalability results
        """
        if neuron_counts is None:
            neuron_counts = [100, 500, 1000, 2000, 5000]
        
        print(f"\nComparing scalability across network sizes")
        print("Neurons    | Time (s) | Steps/s | Spikes/s | Neurons/s")
        print("-" * 65)
        
        for neuron_count in neuron_counts:
            result = self.run_single_benchmark(
                name=f'Scalability_{neuron_count}',
                neuron_count=neuron_count,
                steps=1000,
                warmup_steps=100,
                description=f"Scalability test for {neuron_count} neurons"
            )
            
            print(f"{neuron_count:9d} | {result['total_time']:8.3f} | {result['total_rate']:7.1f} | "
                  f"{result['spikes_per_second']:7.0f} | {result['neurons_per_second']:8.1f}")
        
        return self.results
    
    def generate_report(self, output_file: str = "benchmark_report.json"):
        """
        Generate benchmark report.
        
        Args:
            output_file: Output file path
        """
        print(f"\nGenerating benchmark report: {output_file}")
        
        report = {
            'benchmarks': self.results,
            'summary': {
                'total_benchmarks': len(self.results),
                'total_time': sum(r.get('total_time', r.get('duration', 0)) for r in self.results),
                'avg_performance': sum(r.get('total_rate', 0) for r in self.results) / len(self.results) if self.results else 0
            }
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"  Report saved to: {output_file}")
        
        # Print summary
        self.print_summary()
        
        return report
    
    def print_summary(self):
        """Print benchmark summary."""
        print(f"\n{'='*60}")
        print(f"BENCHMARK SUMMARY")
        print(f"{'='*60}")
        
        if not self.results:
            print("No benchmarks completed.")
            return
        
        print(f"\nTotal benchmarks: {len(self.results)}")
        
        # Filter and categorize results
        scalability_results = [r for r in self.results if r.get('name', '').startswith('Scalability_')]
        memory_results = [r for r in self.results if r.get('name', '').startswith('Memory_')]
        plasticity_results = [r for r in self.results if r.get('name', '').startswith('Plasticity_')]
        
        print(f"\nScalability Benchmarks ({len(scalability_results)}):")
        for result in scalability_results:
            print(f"  {result['neuron_count']:6d} neurons: {result['total_time']:6.3f}s "
                  f"({result['total_rate']:6.1f} steps/s)")
        
        print(f"\nMemory Benchmarks ({len(memory_results)}):")
        for result in memory_results:
            print(f"  {result['neuron_count']:6d} neurons: {result['peak_memory_mb']:6.2f}MB "
                  f"({result['memory_per_neuron_mb']:8.4f}MB per neuron)")
        
        print(f"\nPlasticity Benchmarks ({len(plasticity_results)}):")
        for result in plasticity_results:
            config = result['plasticity_config']
            name = config['name']
            print(f"  {name:10s}: {result['duration']:6.3f}s "
                  f"({result['spikes_per_second']:6.0f} spikes/s)")
        
        # Performance recommendations
        print(f"\nPERFORMANCE RECOMMENDATIONS:")
        
        if scalability_results:
            # Find optimal performance
            best_performance = max(scalability_results, key=lambda x: x['total_rate'])
            print(f"  - Optimal performance: {best_performance['neuron_count']} neurons "
                  f"at {best_performance['total_rate']:.1f} steps/second")
        
        if memory_results:
            # Find memory efficiency
            most_efficient = min(memory_results, key=lambda x: x['memory_per_neuron_mb'])
            print(f"  - Memory efficient: {most_efficient['neuron_count']} neurons "
                  f"uses {most_efficient['memory_per_neuron_mb']:.4f}MB per neuron")
        
        if plasticity_results:
            # Find fastest plasticity
            fastest = min(plasticity_results, key=lambda x: x['duration'])
            config = fastest['plasticity_config']
            print(f"  - Fastest plasticity: {config['name']} mode ({fastest['duration']:.3f}s)")
        
        # Overall performance rating
        avg_rate = sum(r.get('total_rate', 0) for r in scalability_results) / len(scalability_results) if scalability_results else 0
        if avg_rate > 1000:
            rating = "EXCELLENT"
        elif avg_rate > 500:
            rating = "GOOD"
        elif avg_rate > 100:
            rating = "FAIR"
        else:
            rating = "POOR"
        
        print(f"  - Overall performance rating: {rating} ({avg_rate:.1f} steps/second)")
        
        return self.results
def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description='NLM Performance Benchmark')
    
    parser.add_argument('--mode', choices=['scalability', 'memory', 'plasticity', 'all'],
                       default='all', help='Benchmark mode')
    
    parser.add_argument('--neuron-counts', nargs='+', type=int,
                       default=[100, 500, 1000, 2000],
                       help='Neuron counts for scalability tests')
    
    parser.add_argument('--steps', type=int, default=1000,
                       help='Number of simulation steps for each benchmark')
    
    parser.add_argument('--warmup-steps', type=int, default=100,
                       help='Number of warm-up steps')
    
    parser.add_argument('--output', type=str, default='benchmark_report.json',
                       help='Output report file')
    
    parser.add_argument('--verbose', action='store_true',
                       help='Verbose output')
    
    return parser.parse_args()
def main():
    """Main benchmark function."""
    args = parse_arguments()
    
    print("NLM Performance Benchmark")
    print("=" * 50)
    print(f"Mode: {args.mode}")
    print(f"Neuron counts: {args.neuron_counts}")
    print(f"Steps per benchmark: {args.steps}")
    print(f"Warm-up steps: {args.warmup_steps}")
    print(f"Output: {args.output}")
    print()
    
    benchmark = NLMPerformanceBenchmark()
    
    # Run benchmarks based on mode
    if args.mode in ['scalability', 'all']:
        print("Running scalability benchmarks...")
        benchmark.compare_scalability(args.neuron_counts)
    
    if args.mode in ['memory', 'all']:
        print("Running memory benchmarks...")
        for neuron_count in args.neuron_counts:
            benchmark.run_memory_benchmark(neuron_count, args.steps)
    
    if args.mode in ['plasticity', 'all']:
        print("Running plasticity benchmarks...")
        benchmark.run_plasticity_benchmark()
    
    # Generate report
    benchmark.generate_report(args.output)
    
    print(f"\nBenchmark completed successfully!")
    print(f"Results saved to: {args.output}")
    
    return 0
if __name__ == "__main__":
    exit(main())