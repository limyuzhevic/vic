#!/usr/bin/env python3
"""
NLM Benchmark Tool
Comprehensive benchmarking system for Neural Learning Machine

This script provides utilities for:
- Performance benchmarking across different configurations
- Memory usage profiling
- Comparison of network sizes
- Visualization of benchmark results
- Automated benchmarking scripts
"""

import os
import sys
import json
import yaml
import argparse
import time
import statistics
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime
import matplotlib.pyplot as plt
import numpy as np

class NLMBenchmark:
    """Comprehensive benchmarking system for NLM."""
    
    def __init__(self):
        self.results = {}
        self.configurations = []
    
    def run_configuration_benchmark(self, config_name: str, config_params: Dict, 
                                   steps: int = 1000, description: str = None) -> Dict:
        """Run benchmark for a specific configuration."""
        print(f"\n🔬 Benchmarking: {config_name}")
        print(f"   Description: {description or config_name}")
        print(f"   Parameters: {config_params}")
        
        try:
            import pynlm
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Apply parameters
            for key, value in config_params.items():
                config.set(key, value)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Warm-up phase
            print("   Warming up...")
            for i in range(min(100, steps // 10)):
                brain.step(i)
            
            # Actual benchmark
            print(f"   Running {steps} simulation steps...")
            start_time = time.perf_counter()
            
            for step in range(steps):
                brain.step(step)
                
                # Log progress every 10% completion
                if (step + 1) % (steps // 10) == 0:
                    progress = (step + 1) / steps * 100
                    print(f"     {progress:.0f}% complete...")
            
            end_time = time.perf_counter()
            duration = end_time - start_time
            
            # Collect metrics
            result = {
                'config_name': config_name,
                'description': description or config_name,
                'parameters': config_params,
                'steps': steps,
                'duration': duration,
                'steps_per_second': steps / duration,
                'neurons': brain.getTotalNeuronCount(),
                'synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'timestamp': datetime.now().isoformat()
            }
            
            print(f"   ✅ Completed!")
            print(f"     Time: {duration:.2f}s ({steps / duration:.0f} steps/sec)")
            print(f"     Neurons: {result['neurons']}")
            print(f"     Synapses: {result['synapses']}")
            print(f"     Firing neurons: {result['firing_neurons']}")
            print(f"     Total spikes: {result['total_spikes']}")
            
            return result
            
        except Exception as e:
            print(f"   ❌ Error: {e}")
            return {
                'config_name': config_name,
                'error': str(e),
                'timestamp': datetime.now().isoformat()
            }
    
    def run_benchmark_suite(self, benchmark_configs: List[Dict]) -> List[Dict]:
        """Run benchmark suite with multiple configurations."""
        print("\n🧪 NLM Benchmark Suite")
        print("=" * 60)
        print(f"Running {len(benchmark_configs)} benchmark configurations...")
        
        results = []
        
        for i, config_spec in enumerate(benchmark_configs):
            config_name = config_spec.get('name', f"config_{i+1}")
            config_params = config_spec.get('parameters', {})
            steps = config_spec.get('steps', 1000)
            description = config_spec.get('description', config_name)
            
            result = self.run_configuration_benchmark(
                config_name, config_params, steps, description
            )
            
            results.append(result)
            
            # Small delay between benchmarks
            time.sleep(1)
        
        self.results = {r['config_name']: r for r in results if 'error' not in r}
        return results
    
    def compare_network_sizes(self, base_neurons: int = 1000):
        """Compare performance across different network sizes."""
        print(f"\n🖥️  Network Size Comparison (Base: {base_neurons} neurons)")
        print("=" * 60)
        
        size_multipliers = [0.5, 1.0, 2.0, 5.0, 10.0]
        benchmark_configs = []
        
        for multiplier in size_multipliers:
            neurons = int(base_neurons * multiplier)
            config_params = {
                'neuron_count': neurons,
                'region_count': max(2, int(neurons / 1000)),
                'connection_probability': 0.1,
            }
            
            description = f"{neurons} neurons ({multiplier}x base)"
            
            benchmark_configs.append({
                'name': f"network_{int(multiplier*10)}",
                'parameters': config_params,
                'steps': max(500, 10000 // neurons),
                'description': description
            })
        
        results = self.run_benchmark_suite(benchmark_configs)
        
        # Analyze results
        print(f"\n📊 Network Size Analysis")
        print("=" * 40)
        
        for result in results:
            if 'error' not in result:
                neurons = result['neurons']
                steps_per_sec = result['steps_per_second']
                spikes_per_step = result['total_spikes'] / result['steps']
                
                print(f"  {neurons:6d} neurons: {steps_per_sec:8.0f} steps/sec, "
                      f"{spikes_per_step:6.2f} spikes/step")
        
        return results
    
    def profile_memory_usage(self, config_params: Dict, iterations: int = 100):
        """Profile memory usage for a configuration."""
        print(f"\n💾 Memory Usage Profile ({iterations} iterations)")
        print("=" * 50)
        
        try:
            import pynlm
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            for key, value in config_params.items():
                config.set(key, value)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Warm-up
            for i in range(10):
                brain.step(i)
            
            # Memory benchmark
            start_time = time.perf_counter()
            start_memory = self._get_memory_usage()
            
            for i in range(iterations):
                brain.step(i % 1000)  # Reuse steps to save memory
            
            end_time = time.perf_counter()
            end_memory = self._get_memory_usage()
            
            duration = end_time - start_time
            memory_growth = end_memory - start_memory
            
            print(f"   Duration: {duration:.2f}s")
            print(f"   Memory growth: {memory_growth / 1024 / 1024:.2f} MB")
            print(f"   Average steps/sec: {iterations / duration:.0f}")
            print(f"   Memory per step: {memory_growth / iterations / 1024:.2f} KB")
            
            return {
                'duration': duration,
                'memory_growth': memory_growth,
                'steps_per_second': iterations / duration,
                'memory_per_step': memory_growth / iterations
            }
            
        except Exception as e:
            print(f"   ❌ Error: {e}")
            return {'error': str(e)}
    
    def _get_memory_usage(self) -> int:
        """Get current memory usage in bytes."""
        try:
            import psutil
            process = psutil.Process(os.getpid())
            return process.memory_info().rss
        except ImportError:
            return 0
    
    def save_results(self, filename: str, results: List[Dict]):
        """Save benchmark results to file."""
        data = {
            'timestamp': datetime.now().isoformat(),
            'results': results,
            'summary': self._generate_summary(results)
        }
        
        if filename.endswith('.json'):
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
        elif filename.endswith(('.yaml', '.yml')):
            with open(filename, 'w') as f:
                yaml.dump(data, f, default_flow_style=False)
        
        print(f"✅ Results saved to: {filename}")
    
    def _generate_summary(self, results: List[Dict]) -> Dict:
        """Generate summary of benchmark results."""
        successful_results = [r for r in results if 'error' not in r]
        
        if not successful_results:
            return {'total_tests': len(results), 'successful_tests': 0}
        
        steps_per_sec_values = [r['steps_per_second'] for r in successful_results]
        memory_values = [r.get('memory_per_step', 0) for r in successful_results]
        
        summary = {
            'total_tests': len(results),
            'successful_tests': len(successful_results),
            'failed_tests': len(results) - len(successful_results),
            'best_steps_per_sec': max(steps_per_sec_values) if steps_per_sec_values else 0,
            'average_steps_per_sec': statistics.mean(steps_per_sec_values) if steps_per_sec_values else 0,
            'median_steps_per_sec': statistics.median(steps_per_sec_values) if steps_per_sec_values else 0,
            'average_memory_per_step_kb': statistics.mean(memory_values) / 1024 if memory_values else 0,
        }
        
        return summary
    
    def generate_report(self, results: List[Dict]) -> str:
        """Generate human-readable report from results."""
        report_lines = []
        
        report_lines.append("NLM Benchmark Report")
        report_lines.append("=" * 50)
        report_lines.append(f"Generated: {datetime.now().isoformat()}")
        report_lines.append("")
        
        successful_results = [r for r in results if 'error' not in r]
        
        report_lines.append(f"Tests: {len(successful_results)}/{len(results)} successful")
        report_lines.append("")
        
        if successful_results:
            report_lines.append("Performance Summary:")
            report_lines.append("-" * 30)
            
            for result in successful_results:
                config_name = result['config_name']
                description = result['description']
                steps_per_sec = result['steps_per_second']
                neurons = result['neurons']
                synapses = result['synapses']
                
                report_lines.append(
                    f"{config_name:20s} | {steps_per_sec:10.0f} steps/sec | "
                    f"{neurons:6d} neurons | {synapses:6d} synapses | "
                    f"{description}"
                )
        
        report_lines.append("")
        report_lines.append("Recommendations:")
        report_lines.append("-" * 20)
        
        if successful_results:
            best_config = max(successful_results, key=lambda x: x['steps_per_second'])
            report_lines.append(f"Best performance: {best_config['config_name']} "
                              f"({best_config['steps_per_second']:.0f} steps/sec)")
        
        # Check for performance issues
        for result in successful_results:
            if result['steps_per_second'] < 1000:
                report_lines.append(f"⚠️  Low performance in {result['config_name']}: "
                                  f"{result['steps_per_second']:.0f} steps/sec")
            
            if result['neurons'] > 5000 and result['steps_per_second'] < 100:
                report_lines.append(f"⚠️  Large network with poor performance: "
                                  f"{result['neurons']} neurons, {result['steps_per_second']:.0f} steps/sec")
        
        return "\n".join(report_lines)

class BenchmarkManager:
    """Manager for NLM benchmarking system."""
    
    def __init__(self):
        self.benchmarker = NLMBenchmark()
    
    def run_standard_benchmarks(self):
        """Run standard benchmark suite."""
        print("🚀 Running Standard NLM Benchmarks")
        print("=" * 60)
        
        # Standard configurations
        benchmark_configs = [
            {
                'name': 'minimal',
                'parameters': {
                    'neuron_count': 100,
                    'region_count': 1,
                    'connection_probability': 0.05,
                },
                'steps': 500,
                'description': 'Minimal configuration for testing'
            },
            {
                'name': 'small',
                'parameters': {
                    'neuron_count': 500,
                    'region_count': 2,
                    'connection_probability': 0.1,
                },
                'steps': 1000,
                'description': 'Small network configuration'
            },
            {
                'name': 'medium',
                'parameters': {
                    'neuron_count': 1000,
                    'region_count': 4,
                    'connection_probability': 0.15,
                },
                'steps': 1000,
                'description': 'Medium network configuration'
            },
            {
                'name': 'large',
                'parameters': {
                    'neuron_count': 2000,
                    'region_count': 6,
                    'connection_probability': 0.2,
                },
                'steps': 500,
                'description': 'Large network configuration'
            },
            {
                'name': 'xlarge',
                'parameters': {
                    'neuron_count': 5000,
                    'region_count': 8,
                    'connection_probability': 0.1,
                },
                'steps': 200,
                'description': 'Extra-large network configuration'
            }
        ]
        
        results = self.benchmarker.run_benchmark_suite(benchmark_configs)
        
        # Save results
        save_default = input(f"\n💾 Save benchmark results? (y/N): ").strip().lower() == 'y'
        if save_default:
            filename = f"benchmark_results_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            self.benchmarker.save_results(filename, results)
        
        # Generate report
        report = self.benchmarker.generate_report(results)
        
        print(f"\n📊 Benchmark Report:")
        print(report)
        
        # Save report
        save_report = input(f"\n📄 Save benchmark report? (y/N): ").strip().lower() == 'y'
        if save_report:
            report_file = f"benchmark_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
            with open(report_file, 'w') as f:
                f.write(report)
            print(f"✅ Report saved to: {report_file}")
        
        return results
    
    def run_network_size_comparison(self):
        """Run network size comparison benchmarks."""
        print("\n🖥️  Network Size Comparison")
        print("=" * 50)
        
        # Get base configuration from user
        try:
            base_neurons = int(input("Enter base number of neurons: ").strip() or "1000")
        except ValueError:
            base_neurons = 1000
            print(f"Using default: {base_neurons} neurons")
        
        results = self.benchmarker.compare_network_sizes(base_neurons)
        
        # Save results
        save = input(f"\n💾 Save network comparison results? (y/N): ").strip().lower() == 'y'
        if save:
            filename = f"network_comparison_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            self.benchmarker.save_results(filename, results)
        
        return results
    
    def run_memory_benchmark(self):
        """Run memory usage benchmarks."""
        print("\n💾 Memory Usage Benchmarks")
        print("=" * 50)
        
        # Get configuration from user
        try:
            neurons = int(input("Number of neurons: ").strip() or "1000")
            regions = int(input("Number of regions: ").strip() or "4")
            iterations = int(input("Number of iterations: ").strip() or "100")
        except ValueError:
            neurons, regions, iterations = 1000, 4, 100
            print(f"Using defaults: {neurons} neurons, {regions} regions, {iterations} iterations")
        
        config_params = {
            'neuron_count': neurons,
            'region_count': regions,
            'connection_probability': 0.1,
        }
        
        result = self.benchmarker.profile_memory_usage(config_params, iterations)
        
        print(f"\n📊 Memory Benchmark Results:")
        if 'error' not in result:
            print(f"  Duration: {result['duration']:.2f}s")
            print(f"  Memory growth: {result['memory_growth'] / 1024 / 1024:.2f} MB")
            print(f"  Steps/sec: {result['steps_per_second']:.0f}")
            print(f"  Memory per step: {result['memory_per_step'] / 1024:.2f} KB")
        
        return result

def main():
    benchmark_manager = BenchmarkManager()

    parser = argparse.ArgumentParser(
        description="NLM Benchmark Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Run standard benchmarks
    subparsers.add_parser("standard", help="Run standard benchmark suite")

    # Run network size comparison
    subparsers.add_parser("network", help="Compare performance across network sizes")

    # Run memory benchmark
    subparsers.add_parser("memory", help="Profile memory usage")

    args = parser.parse_args()

    if args.command == "standard":
        benchmark_manager.run_standard_benchmarks()
    elif args.command == "network":
        benchmark_manager.run_network_size_comparison()
    elif args.command == "memory":
        benchmark_manager.run_memory_benchmark()
    else:
        print("\nAvailable benchmark commands:")
        print("  nlm-benchmark standard    - Run standard benchmark suite")
        print("  nlm-benchmark network     - Compare network sizes")
        print("  nlm-benchmark memory      - Profile memory usage")
        print("\n💡 Run with --help for more options")

if __name__ == "__main__":
    main()
