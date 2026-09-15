#!/usr/bin/env python3
"""
Example: Performance Benchmark and Optimization Analysis

This example demonstrates performance benchmarking and optimization
techniques for NLM brain simulations.

Features demonstrated:
- Performance benchmarking
- Scalability analysis
- Memory usage monitoring
- Optimization recommendations
"""

import pynlm
import time
import statistics
import psutil
import os
from datetime import datetime
from typing import Dict, List, Tuple
import json

class PerformanceBenchmark:
    """
    Performance benchmarking and optimization analysis for NLM brains.
    """
    
    def __init__(self):
        self.process = psutil.Process(os.getpid())
        self.benchmark_results = {}
        self.memory_samples = []
        self.time_samples = []
    
    def get_memory_usage(self):
        """Get current memory usage in MB."""
        return self.process.memory_info().rss / 1024 / 1024
    
    def benchmark_brain_creation(self, brain_configs):
        """
        Benchmark brain creation across different configurations.
        
        Args:
            brain_configs: List of dicts with 'name' and 'neurons' keys
        
        Returns:
            Dictionary with benchmark results
        """
        print("\nBenchmarking brain creation...")
        
        results = {}
        
        for config in brain_configs:
            name = config['name']
            neuron_count = config['neurons']
            
            print(f"  Testing {name} ({neuron_count} neurons)...")
            
            # Measure memory before creation
            mem_before = self.get_memory_usage()
            
            # Create brain
            start_time = time.time()
            
            nlm_config = pynlm.createDefaultConfig()
            nlm_config.set("neuron_count", neuron_count)
            
            brain = pynlm.createBrain(nlm_config)
            brain.initialize()
            
            end_time = time.time()
            
            # Measure memory after creation
            mem_after = self.get_memory_usage()
            
            # Calculate metrics
            creation_time = end_time - start_time
            mem_increase = mem_after - mem_before
            neurons_per_second = neuron_count / creation_time if creation_time > 0 else 0
            mem_per_neuron = mem_increase / neuron_count if neuron_count > 0 else 0
            
            results[name] = {
                'neuron_count': neuron_count,
                'creation_time': creation_time,
                'memory_increase_mb': mem_increase,
                'neurons_per_second': neurons_per_second,
                'memory_per_neuron_kb': mem_per_neuron * 1024,
                'efficiency_score': neurons_per_second / (mem_per_neuron + 0.001)
            }
            
            print(f"    ✓ Creation time: {creation_time:.3f}s")
            print(f"    ✓ Memory increase: {mem_increase:.1f}MB")
            print(f"    ✓ Neurons/second: {neurons_per_second:.0f}")
            
            # Clean up
            del brain
        
        self.benchmark_results['brain_creation'] = results
        return results
    
    def benchmark_simulation(self, brain_configs, simulation_steps=100):
        """
        Benchmark simulation performance across configurations.
        
        Args:
            brain_configs: List of dicts with 'name', 'neurons', and 'step_delay' keys
            simulation_steps: Number of simulation steps for each benchmark
        
        Returns:
            Dictionary with benchmark results
        """
        print("\nBenchmarking simulation performance...")
        
        results = {}
        
        for config in brain_configs:
            name = config['name']
            neuron_count = config['neurons']
            step_delay = config.get('step_delay', 0.0)
            
            print(f"  Testing {name} ({neuron_count} neurons, {simulation_steps} steps)...")
            
            # Setup brain
            nlm_config = pynlm.createDefaultConfig()
            nlm_config.set("neuron_count", neuron_count)
            nlm_config.set("region_count", 2)
            
            brain = pynlm.createBrain(nlm_config)
            brain.initialize()
            
            # Measure performance
            start_time = time.time()
            mem_start = self.get_memory_usage()
            
            step_times = []
            for step in range(simulation_steps):
                step_start = time.time()
                brain.step(step)
                
                if step_delay > 0:
                    time.sleep(step_delay)
                
                step_end = time.time()
                step_times.append(step_end - step_start)
                
                # Sample memory every 50 steps
                if step % 50 == 0:
                    self.memory_samples.append(self.get_memory_usage())
            
            end_time = time.time()
            mem_end = self.get_memory_usage()
            
            # Calculate metrics
            total_time = end_time - start_time
            avg_step_time = statistics.mean(step_times) if step_times else 0
            min_step_time = min(step_times) if step_times else 0
            max_step_time = max(step_times) if step_times else 0
            steps_per_second = simulation_steps / total_time if total_time > 0 else 0
            
            # Memory analysis
            mem_increase = mem_end - mem_start
            avg_memory = statistics.mean(self.memory_samples) if self.memory_samples else 0
            
            # Performance characteristics
            step_time_std = statistics.stdev(step_times) if len(step_times) > 1 else 0
            step_time_cv = step_time_std / avg_step_time if avg_step_time > 0 else 0  # Coefficient of variation
            
            results[name] = {
                'neuron_count': neuron_count,
                'simulation_steps': simulation_steps,
                'total_time': total_time,
                'avg_step_time': avg_step_time,
                'min_step_time': min_step_time,
                'max_step_time': max_step_time,
                'steps_per_second': steps_per_second,
                'memory_increase_mb': mem_increase,
                'avg_memory_mb': avg_memory,
                'step_time_std': step_time_std,
                'step_time_cv': step_time_cv,
                'performance_score': steps_per_second / (avg_step_time + 0.001)
            }
            
            print(f"    ✓ Total time: {total_time:.2f}s")
            print(f"    ✓ Steps/second: {steps_per_second:.1f}")
            print(f"    ✓ Avg step time: {avg_step_time*1000:.2f}ms")
            print(f"    ✓ Memory increase: {mem_increase:.1f}MB")
            print(f"    ✓ Performance score: {results[name]['performance_score']:.1f}")
            
            # Clean up
            del brain
            self.memory_samples.clear()
        
        self.benchmark_results['simulation'] = results
        return results
    
    def analyze_scalability(self, brain_creation_results, simulation_results):
        """
        Analyze scalability characteristics.
        
        Args:
            brain_creation_results: Results from brain creation benchmark
            simulation_results: Results from simulation benchmark
        
        Returns:
            Dictionary with scalability analysis
        """
        print("\nAnalyzing scalability...")
        
        analysis = {}
        
        # Get data points sorted by neuron count
        creation_configs = sorted(brain_creation_results.values(), 
                                 key=lambda x: x['neuron_count'])
        simulation_configs = sorted(simulation_results.values(),
                                   key=lambda x: x['neuron_count'])
        
        if len(creation_configs) >= 2:
            # Creation scalability
            neuron_ratios = []
            time_ratios = []
            mem_ratios = []
            
            for i in range(1, len(creation_configs)):
                prev = creation_configs[i-1]
                curr = creation_configs[i]
                
                neuron_ratio = curr['neuron_count'] / prev['neuron_count']
                time_ratio = curr['creation_time'] / prev['creation_time']
                mem_ratio = curr['memory_increase_mb'] / prev['memory_increase_mb']
                
                neuron_ratios.append(neuron_ratio)
                time_ratios.append(time_ratio)
                mem_ratios.append(mem_ratio)
            
            # Calculate scaling exponents
            avg_time_scaling = statistics.mean(time_ratios) if time_ratios else 0
            avg_mem_scaling = statistics.mean(mem_ratios) if mem_ratios else 0
            
            analysis['creation_scaling'] = {
                'time_scaling_exponent': avg_time_scaling,
                'memory_scaling_exponent': avg_mem_scaling,
                'linear_scaling_time': avg_time_scaling < 1.5,
                'linear_scaling_memory': avg_mem_scaling < 1.5,
                'cpu_efficiency': 'GOOD' if avg_time_scaling < 2.0 else 'POOR'
            }
        
        if len(simulation_configs) >= 2:
            # Simulation scalability
            sim_ratios = []
            steps_per_sec_ratios = []
            
            for i in range(1, len(simulation_configs)):
                prev = simulation_configs[i-1]
                curr = simulation_configs[i]
                
                neuron_ratio = curr['neuron_count'] / prev['neuron_count']
                steps_per_sec_ratio = curr['steps_per_second'] / prev['steps_per_second'] if prev['steps_per_second'] > 0 else 0
                
                sim_ratios.append(neuron_ratio)
                steps_per_sec_ratios.append(steps_per_sec_ratio)
            
            # Calculate simulation scaling efficiency
            avg_efficiency = statistics.mean(steps_per_sec_ratios) if steps_per_sec_ratios else 0
            
            analysis['simulation_scaling'] = {
                'throughput_efficiency': avg_efficiency,
                'strong_scaling': avg_efficiency > 0.8,
                'weak_scaling': avg_efficiency > 0.6,
                'scalability_grade': self._grade_scalability(avg_efficiency)
            }
        
        # Memory efficiency analysis
        creation_efficiencies = [c['efficiency_score'] for c in creation_configs]
        if creation_efficiencies:
            analysis['memory_efficiency'] = {
                'avg_efficiency': statistics.mean(creation_efficiencies),
                'max_efficiency': max(creation_efficiencies),
                'min_efficiency': min(creation_efficiencies),
                'efficiency_consistency': statistics.stdev(creation_efficiencies) if len(creation_efficiencies) > 1 else 0
            }
        
        self.benchmark_results['scalability_analysis'] = analysis
        return analysis
    
    def _grade_scalability(self, efficiency):
        """Grade scalability based on efficiency."""
        if efficiency > 1.5:
            return "EXCELLENT"
        elif efficiency > 1.2:
            return "GOOD"
        elif efficiency > 0.8:
            return "FAIR"
        elif efficiency > 0.5:
            return "POOR"
        else:
            return "VERY_POOR"
    
    def generate_optimization_recommendations(self, analysis):
        """Generate optimization recommendations based on benchmark results."""
        recommendations = []
        
        if 'creation_scaling' in analysis:
            creation = analysis['creation_scaling']
            if not creation['linear_scaling_time']:
                recommendations.append(
                    "Consider optimizing brain initialization for better linear scaling"
                )
            if not creation['linear_scaling_memory']:
                recommendations.append(
                    "Optimize memory allocation for better memory scaling"
                )
        
        if 'simulation_scaling' in analysis:
            sim = analysis['simulation_scaling']
            if sim['throughput_efficiency'] < 0.8:
                recommendations.append(
                    "Investigate parallelization opportunities for better simulation scaling"
                )
            if sim['scalability_grade'] in ['POOR', 'VERY_POOR']:
                recommendations.append(
                    "Consider architectural changes for better scalability"
                )
        
        if 'memory_efficiency' in analysis:
            mem = analysis['memory_efficiency']
            if mem['efficiency_consistency'] > 2.0:
                recommendations.append(
                    "Inconsistent memory efficiency across sizes - investigate memory leaks"
                )
        
        if not recommendations:
            recommendations.append(
                "Performance is good - consider micro-optimizations for further gains"
            )
        
        return recommendations
    
    def save_results(self, filename='performance_benchmark.json'):
        """Save benchmark results to file."""
        results = {
            'benchmark_timestamp': datetime.now().isoformat(),
            'system_info': {
                'python_version': '3.8+',
                'available_memory': psutil.virtual_memory().total / 1024 / 1024 / 1024,
                'cpu_count': psutil.cpu_count()
            },
            'benchmarks': self.benchmark_results
        }
        
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"\nBenchmark results saved to: {filename}")
        return results

def run_performance_benchmark():
    """Run comprehensive performance benchmark."""
    print("=== NLM Performance Benchmark and Optimization Example ===")
    print("Setting up performance benchmarking...")
    
    # Initialize benchmark
    benchmark = PerformanceBenchmark()
    
    # Step 1: Brain creation benchmark
    print("\n1. Brain Creation Benchmark")
    print("-" * 40)
    brain_configs = [
        {'name': 'Small', 'neurons': 100},
        {'name': 'Medium', 'neurons': 500},
        {'name': 'Large', 'neurons': 1000},
        {'name': 'ExtraLarge', 'neurons': 2000}
    ]
    
    creation_results = benchmark.benchmark_brain_creation(brain_configs)
    
    # Step 2: Simulation benchmark
    print("\n2. Simulation Performance Benchmark")
    print("-" * 40)
    simulation_configs = [
        {'name': 'Small', 'neurons': 100, 'step_delay': 0.0},
        {'name': 'Medium', 'neurons': 500, 'step_delay': 0.0},
        {'name': 'Large', 'neurons': 1000, 'step_delay': 0.001},
        {'name': 'Heavy', 'neurons': 2000, 'step_delay': 0.001}
    ]
    
    simulation_results = benchmark.benchmark_simulation(simulation_configs, simulation_steps=200)
    
    # Step 3: Scalability analysis
    print("\n3. Scalability Analysis")
    print("-" * 40)
    scalability_analysis = benchmark.analyze_scalability(
        creation_results, simulation_results
    )
    
    # Print scalability summary
    if 'creation_scaling' in scalability_analysis:
        cs = scalability_analysis['creation_scaling']
        print(f"   Creation scaling:")
        print(f"     Time scaling exponent: {cs['time_scaling_exponent']:.3f}")
        print(f"     Memory scaling exponent: {cs['memory_scaling_exponent']:.3f}")
        print(f"     CPU efficiency: {cs['cpu_efficiency']}")
    
    if 'simulation_scaling' in scalability_analysis:
        ss = scalability_analysis['simulation_scaling']
        print(f"   Simulation scaling:")
        print(f"     Throughput efficiency: {ss['throughput_efficiency']:.3f}")
        print(f"     Scalability grade: {ss['scalability_grade']}")
        print(f"     Strong scaling: {'Yes' if ss['strong_scaling'] else 'No'}")
        print(f"     Weak scaling: {'Yes' if ss['weak_scaling'] else 'No'}")
    
    # Step 4: Generate recommendations
    print("\n4. Optimization Recommendations")
    print("-" * 40)
    recommendations = benchmark.generate_optimization_recommendations(scalability_analysis)
    for i, rec in enumerate(recommendations, 1):
        print(f"   {i}. {rec}")
    
    # Step 5: Save results
    print("\n5. Saving Results")
    results = benchmark.save_results()
    
    # Print final summary
    print("\n" + "="*50)
    print("✓ PERFORMANCE BENCHMARK COMPLETED!")
    print("="*50)
    
    print("\nBenchmark Summary:")
    print(f"  • Brain sizes tested: {len(brain_configs)}")
    print(f"  • Simulation configurations: {len(simulation_configs)}")
    print(f"  • Total benchmarks run: 2")
    print(f"  • Scalability analyzed: Yes")
    print(f"  • Recommendations generated: {len(recommendations)}")
    
    return benchmark, results

def main():
    benchmark, results = run_performance_benchmark()
    
    print("\nPerformance Analysis:")
    print("  ✓ Memory usage patterns documented")
    print("  ✓ Simulation throughput measured")
    print("  ✓ Scalability characteristics identified")
    print("  ✓ Optimization opportunities found")
    
    return 0

if __name__ == "__main__":
    exit(main())