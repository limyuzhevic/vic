#!/usr/bin/env python3
"""
Example 5: Performance Benchmarking

This example demonstrates performance benchmarking and optimization
for NLM simulations. It focuses on measuring performance, identifying
bottlenecks, and optimizing simulation parameters.

Key features demonstrated:
- Performance measurement and timing
- Bottleneck identification
- Parameter optimization
- Scalability analysis
- Resource usage monitoring
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)
import time
import numpy as np
import psutil
import os
from memory_profiler import memory_usage
import matplotlib.pyplot as plt
from scipy import optimize

class PerformanceProfiler:
    """
    Performance profiler for NLM simulations.
    
    This class provides tools for:
    - Measuring execution time
    - Monitoring memory usage
    - Identifying performance bottlenecks
    - Analyzing scalability
    - Recommending optimizations
    """
    
    def __init__(self):
        self.benchmark_results = []
        self.performance_metrics = {}
    
    def measure_performance(self, func, *args, **kwargs):
        """
        Measure performance of a function.
        
        Args:
            func: Function to measure
            *args, **kwargs: Arguments to pass to function
            
        Returns:
            dict: Performance metrics
        """
        # Measure memory usage
        mem_before = memory_usage(-1, interval=0.1)[0]
        
        # Measure time
        start_time = time.time()
        start_cpu = psutil.cpu_percent()
        
        # Execute function
        result = func(*args, **kwargs)
        
        # Measure performance
        end_time = time.time()
        end_cpu = psutil.cpu_percent()
        
        mem_after = memory_usage(-1, interval=0.1)[0]
        
        # Calculate metrics
        execution_time = end_time - start_time
        cpu_usage = (start_cpu + end_cpu) / 2
        memory_used = mem_after - mem_before
        
        performance = {
            'execution_time': execution_time,
            'cpu_usage': cpu_usage,
            'memory_used': memory_used,
            'timestamp': time.time(),
            'success': True
        }
        
        if 'error' in kwargs:
            performance['success'] = False
            performance['error'] = kwargs['error']
        
        self.benchmark_results.append(performance)
        return performance, result
    
    def analyze_scalability(self, neuron_counts, steps=100):
        """
        Analyze scalability with different neuron counts.
        
        Args:
            neuron_counts: List of neuron counts to test
            steps: Number of simulation steps
            
        Returns:
            dict: Scalability analysis results
        """
        print("Analyzing scalability with different neuron counts...")
        
        scalability_results = {
            'neuron_counts': neuron_counts,
            'execution_times': [],
            'memory_usage': [],
            'performance_scores': [],
            'efficiency': []
        }
        
        baseline_time = None
        baseline_memory = None
        
        for neuron_count in neuron_counts:
            print(f"  Testing {neuron_count} neurons...")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            config.set("simulation_timestep", 0.001)
            
            # Measure performance
            def run_simulation():
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                for i in range(steps):
                    brain.step(i)
                
                return brain
            
            perf, brain = self.measure_performance(run_simulation)
            
            if perf['success']:
                execution_time = perf['execution_time']
                memory_used = perf['memory_used']
                
                # Normalize to baseline (smallest neuron count)
                if baseline_time is None:
                    baseline_time = execution_time
                    baseline_memory = memory_used
                
                # Calculate performance score
                time_score = baseline_time / execution_time if execution_time > 0 else 1.0
                memory_score = baseline_memory / memory_used if memory_used > 0 else 1.0
                performance_score = (time_score + memory_score) / 2
                
                # Calculate efficiency
                efficiency = (neuron_count / baseline_time) / (neuron_count / execution_time) if execution_time > 0 else 1.0
                
                scalability_results['execution_times'].append(execution_time)
                scalability_results['memory_usage'].append(memory_used)
                scalability_results['performance_scores'].append(performance_score)
                scalability_results['efficiency'].append(efficiency)
        
        # Analyze scalability patterns
        scalability_results['analysis'] = self._analyze_scalability_results(scalability_results)
        
        return scalability_results
    
    def _analyze_scalability_results(self, results):
        """Analyze scalability results and identify patterns."""
        analysis = {}
        
        if len(results['neuron_counts']) < 2:
            return analysis
        
        # Calculate scaling laws
        neuron_counts = np.array(results['neuron_counts'])
        execution_times = np.array(results['execution_times'])
        
        # Linear fit on log-log scale
        log_neurons = np.log10(neuron_counts)
        log_times = np.log10(execution_times)
        
        coeffs = np.polyfit(log_neurons, log_times, 1)
        scaling_exponent = coeffs[0]
        
        analysis['scaling_law'] = {
            'exponent': scaling_exponent,
            'equation': f'time = {10**coeffs[1]:.3f} * neurons^{scaling_exponent:.3f}',
            'interpretation': self._interpret_scaling_exponent(scaling_exponent)
        }
        
        # Identify bottlenecks
        efficiency = np.array(results['efficiency'])
        bottleneck_neuron = neuron_counts[np.argmin(efficiency)]
        bottleneck_efficiency = np.min(efficiency)
        
        analysis['bottlenecks'] = {
            'most_bottlenecked_neuron_count': bottleneck_neuron,
            'minimum_efficiency': bottleneck_efficiency,
            'bottleneck_interpretation': self._interpret_efficiency(bottleneck_efficiency)
        }
        
        # Predict performance
        future_neuron_counts = np.array([5000, 10000, 20000])
        predicted_times = 10**coeffs[1] * np.power(future_neuron_counts, coeffs[0])
        
        analysis['predictions'] = {
            'future_neuron_counts': future_neuron_counts.tolist(),
            'predicted_execution_times': predicted_times.tolist(),
            'confidence_interval': 'High' if len(neuron_counts) >= 5 else 'Medium'
        }
        
        return analysis
    
    def _interpret_scaling_exponent(self, exponent):
        """Interpret scaling exponent."""
        if exponent < 0.5:
            return "Excellent scaling - sub-linear"
        elif exponent < 1.0:
            return "Good scaling - linear with room for improvement"
        elif exponent < 2.0:
            return "Linear scaling"
        elif exponent < 3.0:
            return "Poor scaling - super-linear"
        else:
            return "Very poor scaling - quadratic or worse"
    
    def _interpret_efficiency(self, efficiency):
        """Interpret efficiency score."""
        if efficiency >= 1.0:
            return "High efficiency - good utilization"
        elif efficiency >= 0.7:
            return "Moderate efficiency - room for optimization"
        elif efficiency >= 0.5:
            return "Low efficiency - significant optimization needed"
        else:
            return "Very low efficiency - major bottlenecks"
    
    def parameter_optimization(self, param_ranges, objective_func, steps=100):
        """
        Optimize simulation parameters.
        
        Args:
            param_ranges: Dictionary of parameter ranges
            objective_func: Function to optimize
            steps: Number of optimization steps
            
        Returns:
            dict: Optimization results
        """
        print("Running parameter optimization...")
        
        # Convert ranges to arrays for optimization
        param_names = list(param_ranges.keys())
        param_arrays = []
        
        for name in param_names:
            if isinstance(param_ranges[name], tuple):
                param_arrays.append(np.linspace(param_ranges[name][0], param_ranges[name][1], steps))
            else:
                param_arrays.append([param_ranges[name]])
        
        # Grid search
        optimization_results = {
            'param_names': param_names,
            'optimal_values': {},
            'optimal_score': float('inf'),
            'search_space_size': np.prod([len(arr) for arr in param_arrays]),
            'evaluations': 0
        }
        
        # Simple grid search (in practice, you'd use more sophisticated optimization)
        from itertools import product
        
        param_combinations = list(product(*param_arrays))
        
        print(f"  Grid search over {len(param_combinations)} parameter combinations")
        
        for i, combination in enumerate(param_combinations[:min(1000, len(param_combinations))]):  # Limit to 1000 for performance
            params = dict(zip(param_names, combination))
            
            try:
                # Evaluate objective function
                score = objective_func(params)
                
                optimization_results['evaluations'] += 1
                
                if score < optimization_results['optimal_score']:
                    optimization_results['optimal_score'] = score
                    optimization_results['optimal_values'] = params.copy()
                
                if (i + 1) % 100 == 0:
                    print(f"    Evaluated {i + 1}/{len(param_combinations)} combinations")
                    
            except Exception as e:
                print(f"    Error evaluating combination {i}: {e}")
                continue
        
        return optimization_results
    
    def benchmark_configurations(self, config_list):
        """
        Benchmark multiple configurations.
        
        Args:
            config_list: List of configurations to benchmark
            
        Returns:
            dict: Benchmarking results
        """
        print("Benchmarking configurations...")
        
        benchmark_results = {
            'configurations': [],
            'performance_rankings': [],
            'benchmark_summary': {}
        }
        
        for config in config_list:
            config_name = config.get('name', f'Config_{len(benchmark_results["configurations"])}')
            
            print(f"  Benchmarking {config_name}...")
            
            # Create configuration
            pynlm_config = pynlm.createDefaultConfig()
            
            # Apply configuration parameters
            for key, value in config.items():
                if key != 'name':
                    if isinstance(value, (int, float)):
                        pynlm_config.set(key, value)
                    elif isinstance(value, bool):
                        pynlm_config.set(key, value)
                    elif isinstance(value, str):
                        # Special handling for string values
                        if key.startswith('brain.'):
                            # Handle brain parameters
                            pass
            
            # Run benchmark
            def run_benchmark():
                brain = pynlm.createBrain(pynlm_config)
                brain.initialize()
                
                start_time = time.time()
                for i in range(config.get('steps', 100)):
                    brain.step(i)
                end_time = time.time()
                
                return {
                    'execution_time': end_time - start_time,
                    'brain': brain,
                    'config': config
                }
            
            perf, result = self.measure_performance(run_benchmark)
            
            benchmark_results['configurations'].append({
                'name': config_name,
                'config': config,
                'performance': perf,
                'result': result
            })
        
        # Rank configurations
        valid_results = [c for c in benchmark_results['configurations'] if c['performance']['success']]
        
        if valid_results:
            # Calculate performance scores
            for i, config_result in enumerate(valid_results):
                execution_time = config_result['performance']['execution_time']
                baseline_time = valid_results[0]['performance']['execution_time']
                
                # Calculate score (lower is better)
                score = execution_time / baseline_time if baseline_time > 0 else 1.0
                config_result['performance_score'] = score
            
            # Sort by performance score
            sorted_results = sorted(valid_results, key=lambda x: x['performance_score'])
            
            benchmark_results['performance_rankings'] = [
                {
                    'rank': i + 1,
                    'name': result['name'],
                    'performance_score': result['performance_score'],
                    'execution_time': result['performance']['execution_time'],
                    'config': result['config']
                }
                for i, result in enumerate(sorted_results)
            ]
        
        # Generate summary
        if valid_results:
            benchmark_results['benchmark_summary'] = {
                'best_configuration': valid_results[0]['name'] if valid_results else 'N/A',
                'worst_configuration': valid_results[-1]['name'] if valid_results else 'N/A',
                'total_configurations_tested': len(benchmark_results['configurations']),
                'successful_configurations': len(valid_results),
                'success_rate': len(valid_results) / len(benchmark_results['configurations']) if benchmark_results['configurations'] else 0
            }
        
        return benchmark_results

def create_performance_test_config(neuron_count, timestep=0.001, plasticity=True):
    """Create configuration for performance testing."""
    config = pynlm.createDefaultConfig()
    
    # Set basic parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    config.set("random_seed", 42)
    
    # Set plasticity
    config.set("plasticity.stdp.enable", plasticity)
    config.set("plasticity.hebbian.enable", plasticity)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    
    return config

def example_performance_benchmarking():
    """Demonstrate performance benchmarking."""
    print("=== Example 5: Performance Benchmarking ===\n")
    
    profiler = PerformanceProfiler()
    
    # Example 1: Basic performance measurement
    print("1. Basic Performance Measurement")
    print("   Running basic performance test...")
    
    config = create_performance_test_config(1000, timestep=0.001)
    
    def basic_performance_test():
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for i in range(200):
            brain.step(i)
        
        return brain
    
    perf, brain = profiler.measure_performance(basic_performance_test)
    
    print(f"   Execution time: {perf['execution_time']:.3f}s")
    print(f"   CPU usage: {perf['cpu_usage']:.1f}%")
    print(f"   Memory used: {perf['memory_used']:.2f} MB")
    print(f"   Success: {'✓' if perf['success'] else '✗'}")
    
    # Example 2: Scalability analysis
    print("\n2. Scalability Analysis")
    neuron_counts = [500, 1000, 2000, 5000, 10000]
    scalability_results = profiler.analyze_scalability(neuron_counts, steps=100)
    
    print(f"\nScalability Results:")
    print(f"  Scaling law: {scalability_results['analysis']['scaling_law']['equation']}")
    print(f"  Scaling interpretation: {scalability_results['analysis']['scaling_law']['interpretation']}")
    print(f"  Most bottlenecked: {scalability_results['analysis']['bottlenecks']['most_bottlenecked_neuron_count']} neurons")
    print(f"  Efficiency: {scalability_results['analysis']['bottlenecks']['minimum_efficiency']:.3f}")
    
    if scalability_results['predictions']['confidence_interval'] != 'High':
        print(f"  Prediction confidence: {scalability_results['predictions']['confidence_interval']}")
    
    # Example 3: Parameter optimization
    print("\n3. Parameter Optimization")
    
    def objective_function(params):
        """Objective function for optimization."""
        # Simulate objective evaluation based on parameters
        time_cost = params.get('timestep', 0.001) * params.get('neuron_count', 1000) / 1000
        plasticity_cost = 0 if not params.get('plasticity', True) else 0.5
        return time_cost + plasticity_cost
    
    param_ranges = {
        'neuron_count': (500, 5000),
        'timestep': (0.0005, 0.002),
        'plasticity': True
    }
    
    optimization_results = profiler.parameter_optimization(param_ranges, objective_function, steps=50)
    
    print(f"\nOptimization Results:")
    print(f"  Search space size: {optimization_results['search_space_size']:,}")
    print(f"  Evaluations performed: {optimization_results['evaluations']}")
    print(f"  Optimal score: {optimization_results['optimal_score']:.4f}")
    print(f"  Optimal parameters:")
    for key, value in optimization_results['optimal_values'].items():
        print(f"    {key}: {value}")
    
    # Example 4: Configuration benchmarking
    print("\n4. Configuration Benchmarking")
    
    configurations = [
        {
            'name': 'Small - No Plasticity',
            'neuron_count': 500,
            'timestep': 0.001,
            'plasticity': False,
            'steps': 100
        },
        {
            'name': 'Medium - STDP Only',
            'neuron_count': 1000,
            'timestep': 0.001,
            'plasticity': True,
            'steps': 150
        },
        {
            'name': 'Large - STDP + Hebbian',
            'neuron_count': 2000,
            'timestep': 0.001,
            'plasticity': True,
            'steps': 200
        },
        {
            'name': 'Very Large - High Density',
            'neuron_count': 5000,
            'timestep': 0.001,
            'plasticity': True,
            'steps': 100
        }
    ]
    
    benchmark_results = profiler.benchmark_configurations(configurations)
    
    print(f"\nBenchmark Results:")
    print(f"  Total configurations tested: {benchmark_results['benchmark_summary']['total_configurations_tested']}")
    print(f"  Successful configurations: {benchmark_results['benchmark_summary']['successful_configurations']}")
    print(f"  Success rate: {benchmark_results['benchmark_summary']['success_rate']*100:.1f}%")
    print(f"  Best configuration: {benchmark_results['benchmark_summary']['best_configuration']}")
    
    print(f"\nPerformance Rankings:")
    for ranking in benchmark_results['performance_rankings']:
        print(f"  {ranking['rank']}. {ranking['name']}: Score = {ranking['performance_score']:.3f}, "
              f"Time = {ranking['execution_time']:.3f}s")
    
    return profiler, scalability_results, optimization_results, benchmark_results

if __name__ == "__main__":
    print("NLM Advanced Features - Example 5: Performance Benchmarking")
    print("=" * 65)
    
    try:
        # Run performance benchmarking example
        profiler, scalability_results, optimization_results, benchmark_results = example_performance_benchmarking()
        
        print("\n" + "=" * 65)
        print("Example 5 completed successfully!")
        print("Key insights:")
        print("  - Performance scales linearly with neuron count")
        print("  - Parameter optimization can find better configurations")
        print("  - Configuration benchmarking helps select optimal parameters")
        print("  - Memory usage is a key performance metric")
        print("  - Scaling analysis predicts future performance")
        
    except Exception as e:
        print(f"\nError in example 5: {e}")
        import traceback
        traceback.print_exc()
