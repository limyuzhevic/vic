# NLM Performance Guide
# ==========================

This document provides comprehensive guidance on optimizing NLM (Neural Learning Machine) performance. It covers performance analysis techniques, optimization strategies, benchmarking methods, and best practices for achieving optimal performance in NLM simulations.

## 1. Performance Measurement and Metrics

### Key Performance Metrics

| Metric | Description | Units | Target Range |
|--------|-------------|-------|--------------|
| **Steps/Second** | Simulation speed | steps/second | 100-10000 |
| **Memory Usage** | RAM consumption during simulation | MB | < 2000 |
| **CPU Usage** | Processor utilization | % | < 80% |
| **Firing Rate** | Average neuron firing rate | Hz | 10-1000 |
| **Neuron Utilization** | Percentage of active neurons | % | 10-50% |
| **Simulation Time** | Real-world time per simulation | seconds | < 1000 |
| **Memory Efficiency** | Memory per neuron | KB/neuron | < 50 |

### Performance Measurement Techniques

#### Basic Performance Measurement

```python
import pynlm
import time
import psutil
import numpy as np

class PerformanceMonitor:
    """Monitor NLM simulation performance."""
    
    def __init__(self):
        self.metrics = {
            'start_time': None,
            'end_time': None,
            'steps_completed': 0,
            'peak_memory': 0,
            'cpu_samples': [],
            'simulation_time': 0,
            'steps_per_second': 0,
            'memory_efficiency': 0
        }
    
    def start_monitoring(self):
        """Start performance monitoring."""
        self.metrics['start_time'] = time.time()
        self.metrics['cpu_samples'] = []
        print("Performance monitoring started")
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.metrics['end_time'] = time.time()
        self.calculate_metrics()
        self.print_report()
    
    def step_completed(self, brain, step):
        """Record completion of a simulation step."""
        self.metrics['steps_completed'] = step + 1
        
        # Sample CPU usage
        cpu_percent = psutil.cpu_percent(interval=None)
        self.metrics['cpu_samples'].append(cpu_percent)
        
        # Monitor memory usage
        memory_info = psutil.virtual_memory()
        self.metrics['peak_memory'] = max(
            self.metrics['peak_memory'],
            memory_info.used / 1024 / 1024  # MB
        )
    
    def calculate_metrics(self):
        """Calculate performance metrics."""
        if self.metrics['start_time'] is None or self.metrics['end_time'] is None:
            return
        
        # Calculate simulation time
        self.metrics['simulation_time'] = (
            self.metrics['end_time'] - self.metrics['start_time']
        )
        
        # Calculate steps per second
        if self.metrics['simulation_time'] > 0:
            self.metrics['steps_per_second'] = (
                self.metrics['steps_completed'] / self.metrics['simulation_time']
            )
        
        # Calculate CPU usage statistics
        if self.metrics['cpu_samples']:
            self.metrics['avg_cpu_usage'] = np.mean(self.metrics['cpu_samples'])
            self.metrics['max_cpu_usage'] = np.max(self.metrics['cpu_samples'])
        
        # Calculate memory efficiency
        if hasattr(self, '_brain_neuron_count'):
            neurons = self._brain_neuron_count
            if neurons > 0:
                self.metrics['memory_efficiency'] = (
                    self.metrics['peak_memory'] * 1024 * 1024 / neurons  # Bytes per neuron
                )
    
    def print_report(self):
        """Print performance report."""
        print("\n=== NLM Performance Report ===")
        print(f"Simulation Steps: {self.metrics['steps_completed']}")
        print(f"Simulation Time: {self.metrics['simulation_time']:.2f} seconds")
        print(f"Steps/Second: {self.metrics['steps_per_second']:.1f}")
        print(f"Peak Memory Usage: {self.metrics['peak_memory']:.1f} MB")
        
        if hasattr(self, 'avg_cpu_usage'):
            print(f"Average CPU Usage: {self.metrics['avg_cpu_usage']:.1f}%")
            print(f"Maximum CPU Usage: {self.metrics['max_cpu_usage']:.1f}%")
        
        if hasattr(self, 'memory_efficiency'):
            print(f"Memory Efficiency: {self.metrics['memory_efficiency']:.1f} bytes/neuron")
        
        # Performance assessment
        self.assess_performance()
    
    def assess_performance(self):
        """Assess performance based on metrics."""
        issues = []
        
        # Check steps per second
        if self.metrics['steps_per_second'] < 100:
            issues.append("Slow simulation speed (< 100 steps/second)")
        
        # Check memory usage
        if self.metrics['peak_memory'] > 2000:
            issues.append("High memory usage (> 2000 MB)")
        
        # Check CPU usage
        if hasattr(self, 'max_cpu_usage') and self.metrics['max_cpu_usage'] > 80:
            issues.append("High CPU usage (> 80%)")
        
        # Check memory efficiency
        if hasattr(self, 'memory_efficiency') and self.metrics['memory_efficiency'] > 1000:
            issues.append("Poor memory efficiency (> 1000 bytes/neuron)")
        
        if issues:
            print("\nPerformance Issues:")
            for issue in issues:
                print(f"  - {issue}")
        else:
            print("\nPerformance: Good")
```

#### Advanced Performance Profiling

```python
import pynlm
import cProfile
import pstats
import io
from memory_profiler import profile
import line_profiler
import time

class AdvancedPerformanceProfiler:
    """Advanced performance profiling for NLM simulations."""
    
    def __init__(self, brain):
        self.brain = brain
        self.profile_data = {}
    
    def profile_cpu_usage(self, config, steps=100):
        """Profile CPU usage with detailed statistics."""
        print(f"Profiling CPU usage for {steps} steps...")
        
        # Detailed profiling
        pr = cProfile.Profile()
        pr.enable()
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            self.brain.step(step)
        end_time = time.time()
        
        pr.disable()
        
        # Generate profile statistics
        s = io.StringIO()
        ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
        ps.print_stats(20)  # Top 20 functions
        
        self.profile_data['cpu_profile'] = {
            'total_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'profile_output': s.getvalue(),
            'top_functions': self.extract_top_functions(s.getvalue())
        }
        
        return self.profile_data['cpu_profile']
    
    def profile_memory_usage(self, steps=100):
        """Profile memory usage over time."""
        print(f"Profiling memory usage for {steps} steps...")
        
        # Memory profiling setup
        memory_profiler.enable()
        
        start_time = time.time()
        for step in range(steps):
            self.brain.step(step)
        
        # Get memory usage profile
        mem_usage = memory_profiler.memory_usage()
        end_time = time.time()
        
        memory_profiler.disable()
        
        self.profile_data['memory_profile'] = {
            'time_points': len(mem_usage),
            'max_memory': max(mem_usage),
            'min_memory': min(mem_usage),
            'avg_memory': sum(mem_usage) / len(mem_usage),
            'memory_growth': mem_usage[-1] - mem_usage[0],
            'peak_memory_increase': self.calculate_peak_memory_increase(mem_usage)
        }
        
        return self.profile_data['memory_profile']
    
    def profile_specific_functions(self, functions_to_profile):
        """Profile specific functions in detail."""
        print("Profiling specific functions...")
        
        profiler = line_profiler.LineProfiler()
        
        # Add functions to profile
        for func in functions_to_profile:
            profiler.add_function(func)
        
        # Run simulation with profiling
        profiler.enable_by_count()
        
        # Run simulation
        for step in range(50):
            self.brain.step(step)
        
        profiler.disable_by_count()
        
        # Print results
        profiler.print_stats()
        
        self.profile_data['function_profile'] = profiler.get_stats()
        return self.profile_data['function_profile']
    
    def extract_top_functions(self, profile_output):
        """Extract top functions from profile output."""
        lines = profile_output.split('\n')
        top_functions = []
        
        for line in lines:
            if line.strip() and not line.startswith('   ncalls'):
                # Parse function information
                parts = line.split()
                if len(parts) >= 6:
                    try:
                        ncalls = parts[0]
                        time_taken = parts[1]
                        per_call = parts[2]
                        cumulative_time = parts[3]
                        per_cumulative = parts[4]
                        function_name = ' '.join(parts[5:])
                        
                        top_functions.append({
                            'ncalls': ncalls,
                            'time_taken': time_taken,
                            'per_call': per_call,
                            'cumulative_time': cumulative_time,
                            'per_cumulative': per_cumulative,
                            'function_name': function_name
                        })
                    except (IndexError, ValueError):
                        continue
        
        return top_functions[:10]  # Top 10 functions
    
    def calculate_peak_memory_increase(self, mem_usage):
        """Calculate peak memory increase."""
        if len(mem_usage) < 2:
            return 0.0
        
        baseline = mem_usage[0]
        peak_increase = max(mem_usage) - baseline
        return peak_increase
    
    def generate_performance_report(self):
        """Generate comprehensive performance report."""
        report = "# NLM Performance Analysis Report\n\n"
        
        if 'cpu_profile' in self.profile_data:
            cpu = self.profile_data['cpu_profile']
            report += f"## CPU Performance\n"
            report += f"- Total Time: {cpu['total_time']:.2f} seconds\n"
            report += f"- Steps/Second: {cpu['steps_per_second']:.1f}\n"
            report += f"- Top Functions:\n"
            
            for func in cpu['top_functions']:
                report += f"  - {func['function_name']}: {func['cumulative_time']} cumulative, {func['ncalls']} calls\n"
            
            report += f"\n"
        
        if 'memory_profile' in self.profile_data:
            mem = self.profile_data['memory_profile']
            report += f"## Memory Performance\n"
            report += f"- Time Points: {mem['time_points']}\n"
            report += f"- Maximum Memory: {mem['max_memory']:.1f} MB\n"
            report += f"- Minimum Memory: {mem['min_memory']:.1f} MB\n"
            report += f"- Average Memory: {mem['avg_memory']:.1f} MB\n"
            report += f"- Memory Growth: {mem['memory_growth']:.1f} MB\n"
            report += f"- Peak Memory Increase: {mem['peak_memory_increase']:.1f} MB\n"
            report += f"\n"
        
        return report
```

## 2. Benchmarking Techniques

### Basic Benchmarking

```python
import pynlm
import time
import statistics
import json
from typing import Dict, List

class PerformanceBenchmark:
    """Benchmark NLM performance across different configurations."""
    
    def __init__(self):
        self.benchmark_results = {}
    
    def benchmark_configuration(self, config, name, steps=100, iterations=5):
        """Benchmark a single configuration."""
        print(f"Benchmarking {name} with {steps} steps, {iterations} iterations...")
        
        results = {
            'config_name': name,
            'steps': steps,
            'iterations': iterations,
            'results': [],
            'statistics': {}
        }
        
        for iteration in range(iterations):
            # Create brain with configuration
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Time the simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            # Calculate metrics
            elapsed_time = end_time - start_time
            steps_per_second = steps / elapsed_time
            firing_rate = brain.getAverageFiringRate()
            neuron_count = brain.getTotalNeuronCount()
            synapse_count = brain.getTotalSynapseCount()
            
            iteration_result = {
                'iteration': iteration + 1,
                'elapsed_time': elapsed_time,
                'steps_per_second': steps_per_second,
                'firing_rate': firing_rate,
                'neuron_count': neuron_count,
                'synapse_count': synapse_count,
                'memory_efficiency': neuron_count / (elapsed_time * 1024 * 1024)  # neurons/MB
            }
            
            results['results'].append(iteration_result)
        
        # Calculate statistics
        results['statistics'] = self.calculate_statistics(results['results'])
        self.benchmark_results[name] = results
        
        return results
    
    def calculate_statistics(self, results: List[Dict]) -> Dict:
        """Calculate statistics from benchmark results."""
        stats = {}
        
        # Steps per second statistics
        steps_per_second = [r['steps_per_second'] for r in results]
        stats['steps_per_second'] = {
            'mean': statistics.mean(steps_per_second),
            'stdev': statistics.stdev(steps_per_second) if len(steps_per_second) > 1 else 0,
            'min': min(steps_per_second),
            'max': max(steps_per_second),
            'cv': statistics.stdev(steps_per_second) / statistics.mean(steps_per_second) if statistics.mean(steps_per_second) > 0 else 0
        }
        
        # Firing rate statistics
        firing_rates = [r['firing_rate'] for r in results]
        stats['firing_rate'] = {
            'mean': statistics.mean(firing_rates),
            'stdev': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
            'min': min(firing_rates),
            'max': max(firing_rates)
        }
        
        # Memory efficiency statistics
        memory_efficiencies = [r['memory_efficiency'] for r in results]
        stats['memory_efficiency'] = {
            'mean': statistics.mean(memory_efficiencies),
            'stdev': statistics.stdev(memory_efficiencies) if len(memory_efficiencies) > 1 else 0,
            'min': min(memory_efficiencies),
            'max': max(memory_efficiencies)
        }
        
        # Time statistics
        times = [r['elapsed_time'] for r in results]
        stats['elapsed_time'] = {
            'mean': statistics.mean(times),
            'stdev': statistics.stdev(times) if len(times) > 1 else 0,
            'min': min(times),
            'max': max(times)
        }
        
        return stats
    
    def compare_configurations(self, config_dict: Dict[str, any], steps=100, iterations=5):
        """Compare multiple configurations."""
        print(f"Comparing {len(config_dict)} configurations...")
        
        comparison_results = {}
        
        for name, config in config_dict.items():
            print(f"  Benchmarking {name}...")
            results = self.benchmark_configuration(config, name, steps, iterations)
            comparison_results[name] = results
        
        return comparison_results
    
    def generate_benchmark_report(self, results: Dict) -> str:
        """Generate comprehensive benchmark report."""
        report = "# NLM Performance Benchmark Report\n\n"
        
        for config_name, config_results in results.items():
            report += f"## {config_name}\n"
            report += f"- Steps: {config_results['steps']}\n"
            report += f"- Iterations: {config_results['iterations']}\n"
            
            stats = config_results['statistics']
            
            # Steps per second
            sps_stats = stats['steps_per_second']
            report += f"- Steps/Second: {sps_stats['mean']:.1f} ± {sps_stats['stdev']:.1f} (min: {sps_stats['min']:.1f}, max: {sps_stats['max']:.1f})\n"
            report += f"  - Coefficient of Variation: {sps_stats['cv']:.3f}\n"
            
            # Firing rate
            fr_stats = stats['firing_rate']
            report += f"- Firing Rate: {fr_stats['mean']:.1f} ± {fr_stats['stdev']:.1f} Hz (min: {fr_stats['min']:.1f}, max: {fr_stats['max']:.1f})\n"
            
            # Memory efficiency
            me_stats = stats['memory_efficiency']
            report += f"- Memory Efficiency: {me_stats['mean']:.1f} ± {me_stats['stdev']:.1f} neurons/MB\n"
            
            # Time
            time_stats = stats['elapsed_time']
            report += f"- Average Time per Iteration: {time_stats['mean']:.2f} ± {time_stats['stdev']:.2f} seconds\n"
            
            report += f"\n"
        
        # Statistical comparisons
        report += f"## Statistical Analysis\n"
        report += f"\n"
        
        return report
```

### Advanced Benchmarking with System Metrics

```python
import pynlm
import time
import psutil
import numpy as np
import os
from typing import Dict, List

class SystemAwareBenchmark:
    """Benchmark that considers system state and external factors."""
    
    def __init__(self):
        self.system_state = {}
    
    def benchmark_with_system_monitoring(self, config, name, steps=100, iterations=5):
        """Benchmark configuration with system monitoring."""
        print(f"System-aware benchmarking {name}...")
        
        results = {
            'config_name': name,
            'steps': steps,
            'iterations': iterations,
            'results': [],
            'system_metrics': {}
        }
        
        for iteration in range(iterations):
            # Record system state before benchmark
            system_state_before = self.capture_system_state()
            
            # Create and run brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Monitor system during simulation
            simulation_metrics = self.monitor_simulation(brain, steps)
            
            # Record system state after benchmark
            system_state_after = self.capture_system_state()
            
            # Calculate results
            iteration_result = self.calculate_iteration_result(
                brain, steps, simulation_metrics,
                system_state_before, system_state_after
            )
            
            results['results'].append(iteration_result)
        
        # Calculate system metrics
        results['system_metrics'] = self.calculate_system_metrics(results['results'])
        
        return results
    
    def capture_system_state(self):
        """Capture current system state."""
        return {
            'cpu_percent': psutil.cpu_percent(interval=0),
            'memory': psutil.virtual_memory(),
            'disk': psutil.disk_usage('/'),
            'timestamp': time.time()
        }
    
    def monitor_simulation(self, brain, steps):
        """Monitor system during simulation."""
        metrics = {
            'cpu_samples': [],
            'memory_samples': [],
            'timestamps': [],
            'start_time': time.time()
        }
        
        for step in range(steps):
            # Sample system metrics
            metrics['cpu_samples'].append(psutil.cpu_percent(interval=None))
            metrics['memory_samples'].append(
                psutil.virtual_memory().used / 1024 / 1024  # MB
            )
            metrics['timestamps'].append(time.time())
            
            # Run simulation step
            brain.step(step)
        
        return metrics
    
    def calculate_iteration_result(self, brain, steps, simulation_metrics,
                                 system_state_before, system_state_after):
        """Calculate benchmark result for one iteration."""
        # Calculate performance metrics
        elapsed_time = time.time() - simulation_metrics['start_time']
        steps_per_second = steps / elapsed_time
        
        # Calculate system impact
        cpu_impact = self.calculate_cpu_impact(
            simulation_metrics['cpu_samples'], system_state_before, system_state_after
        )
        
        memory_impact = self.calculate_memory_impact(
            simulation_metrics['memory_samples'], system_state_before, system_state_after
        )
        
        return {
            'elapsed_time': elapsed_time,
            'steps_per_second': steps_per_second,
            'firing_rate': brain.getAverageFiringRate(),
            'neuron_count': brain.getTotalNeuronCount(),
            'cpu_impact': cpu_impact,
            'memory_impact': memory_impact,
            'simulation_metrics': simulation_metrics
        }
    
    def calculate_cpu_impact(self, cpu_samples, before, after):
        """Calculate CPU impact of simulation."""
        avg_cpu_during = np.mean(cpu_samples) if cpu_samples else 0
        baseline_cpu = before['cpu_percent']
        
        return {
            'absolute_increase': avg_cpu_during - baseline_cpu,
            'relative_increase': (avg_cpu_during - baseline_cpu) / baseline_cpu if baseline_cpu > 0 else 0,
            'average_during': avg_cpu_during,
            'baseline': baseline_cpu
        }
    
    def calculate_memory_impact(self, memory_samples, before, after):
        """Calculate memory impact of simulation."""
        avg_memory_during = np.mean(memory_samples) if memory_samples else 0
        baseline_memory = before['memory'].used
        
        return {
            'absolute_increase': avg_memory_during - (baseline_memory / 1024 / 1024),  # MB
            'relative_increase': (avg_memory_during - (baseline_memory / 1024 / 1024)) / (baseline_memory / 1024 / 1024) if baseline_memory > 0 else 0,
            'average_during': avg_memory_during,
            'baseline': baseline_memory / 1024 / 1024
        }
    
    def calculate_system_metrics(self, results):
        """Calculate system-wide metrics."""
        cpu_impacts = [r['cpu_impact'] for r in results]
        memory_impacts = [r['memory_impact'] for r in results]
        
        return {
            'cpu_impact_mean': np.mean([imp['average_during'] for imp in cpu_impacts]),
            'cpu_impact_cv': np.std([imp['average_during'] for imp in cpu_impacts]) / np.mean([imp['average_during'] for imp in cpu_impacts]) if np.mean([imp['average_during'] for imp in cpu_impacts]) > 0 else 0,
            'memory_impact_mean': np.mean([imp['average_during'] for imp in memory_impacts]),
            'memory_impact_cv': np.std([imp['average_during'] for imp in memory_impacts]) / np.mean([imp['average_during'] for imp in memory_impacts]) if np.mean([imp['average_during'] for imp in memory_impacts]) > 0 else 0
        }
```

## 3. Optimization Strategies

### Configuration-Based Optimization

```python
import pynlm
import numpy as np
import itertools

class ConfigurationOptimizer:
    """Optimize NLM configurations for performance."""
    
    def __init__(self, base_config):
        self.base_config = base_config
        self.optimization_history = []
    
    def optimize_neuron_count(self, neuron_range, steps=100, iterations=3):
        """Optimize neuron count for performance."""
        print(f"Optimizing neuron count in range {neuron_range}...")
        
        best_config = None
        best_performance = 0
        
        for neuron_count in neuron_range:
            # Create configuration with current neuron count
            config = self.create_config_with_neuron_count(neuron_count)
            
            # Benchmark configuration
            performance = self.benchmark_configuration(config, steps, iterations)
            
            # Track optimization history
            self.optimization_history.append({
                'neuron_count': neuron_count,
                'performance': performance,
                'timestamp': time.time()
            })
            
            # Update best if improved
            if performance > best_performance:
                best_performance = performance
                best_config = config
        
        print(f"Best neuron count: {neuron_count} (performance: {best_performance:.2f})")
        return best_config, best_performance
    
    def optimize_plasticity_parameters(self, param_space, steps=100, iterations=3):
        """Optimize plasticity parameters using random search."""
        print("Optimizing plasticity parameters...")
        
        best_params = None
        best_performance = 0
        
        # Generate random parameter combinations
        for _ in range(param_space['n_combinations']):
            # Sample parameters
            params = self.sample_parameters(param_space)
            
            # Create configuration
            config = self.create_config_with_plasticity_params(params)
            
            # Benchmark
            performance = self.benchmark_configuration(config, steps, iterations)
            
            # Track optimization history
            self.optimization_history.append({
                'parameters': params,
                'performance': performance,
                'timestamp': time.time()
            })
            
            # Update best if improved
            if performance > best_performance:
                best_performance = performance
                best_params = params
        
        print(f"Best plasticity parameters: {best_params} (performance: {best_performance:.2f})")
        return best_params, best_performance
    
    def optimize_simulation_parameters(self, param_space, steps=100, iterations=3):
        """Optimize simulation parameters."""
        print("Optimizing simulation parameters...")
        
        # Try different simulation parameters
        parameter_combinations = list(itertools.product(*param_space.values()))
        
        best_config = None
        best_performance = 0
        
        for i, (timestep, max_steps) in enumerate(parameter_combinations):
            # Create configuration
            config = self.base_config.copy()
            config.set("simulation.timestep", timestep)
            config.set("simulation.max_steps", max_steps)
            
            # Benchmark
            performance = self.benchmark_configuration(config, max_steps // 10, iterations)
            
            # Track optimization history
            self.optimization_history.append({
                'simulation_parameters': {'timestep': timestep, 'max_steps': max_steps},
                'performance': performance,
                'timestamp': time.time()
            })
            
            # Update best if improved
            if performance > best_performance:
                best_performance = performance
                best_config = config
        
        print(f"Best simulation parameters: timestep={timestep}, max_steps={max_steps} (performance: {best_performance:.2f})")
        return best_config, best_performance
    
    def create_config_with_neuron_count(self, neuron_count):
        """Create configuration with specific neuron count."""
        config = self.base_config.copy()
        config.set("brain.neuron_count", neuron_count)
        return config
    
    def create_config_with_plasticity_params(self, params):
        """Create configuration with plasticity parameters."""
        config = self.base_config.copy()
        config.set("plasticity.stdp.learning_rate", params['stdp_learning_rate'])
        config.set("plasticity.stdp.ltp_weight", params['stdp_ltp_weight'])
        config.set("plasticity.stdp.ltd_weight", params['stdp_ltd_weight'])
        return config
    
    def sample_parameters(self, param_space):
        """Sample parameters from parameter space."""
        params = {}
        
        for param_name, bounds in param_space['bounds'].items():
            # Sample from log-normal distribution for positive parameters
            if bounds['min'] > 0 and bounds['max'] > bounds['min']:
                log_min = np.log(bounds['min'])
                log_max = np.log(bounds['max'])
                sampled = np.exp(np.random.uniform(log_min, log_max))
            else:
                # Uniform sampling
                sampled = np.random.uniform(bounds['min'], bounds['max'])
            
            params[param_name] = sampled
        
        return params
    
    def benchmark_configuration(self, config, steps, iterations):
        """Benchmark configuration and return performance score."""
        performances = []
        
        for iteration in range(iterations):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            end_time = time.time()
            
            # Calculate performance score
            elapsed = end_time - start_time
            steps_per_second = steps / elapsed
            firing_rate = brain.getAverageFiringRate()
            
            # Composite performance score
            # Higher steps/second is better
            # Moderate firing rate is better
            # Balance both metrics
            performance = (steps_per_second / 1000) + (firing_rate / 100)
            
            performances.append(performance)
        
        return np.mean(performances)
    
    def generate_optimization_report(self):
        """Generate optimization report."""
        report = "# NLM Configuration Optimization Report\n\n"
        
        report += f"## Optimization History\n"
        report += f"Total optimizations performed: {len(self.optimization_history)}\n"
        
        if self.optimization_history:
            report += f"\n## Recent Optimizations\n"
            for i, opt in enumerate(self.optimization_history[-10:]):  # Last 10 optimizations
                timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(opt['timestamp']))
                report += f"{i+1}. {timestamp} - {opt.get('neuron_count', opt.get('parameters', opt.get('simulation_parameters', {})))}: {opt['performance']:.2f}\n"
        
        report += f"\n## Key Findings\n"
        
        # Analyze optimization results
        if self.optimization_history:
            # Find best performing optimization
            best_opt = max(self.optimization_history, key=lambda x: x['performance'])
            report += f"- Best performance achieved: {best_opt['performance']:.2f}\n"
            
            # Analyze parameter impact
            if 'neuron_count' in best_opt:
                report += f"- Optimal neuron count for this test: {best_opt['neuron_count']}\n"
        
        return report
```

### Memory Optimization

```python
import pynlm
import numpy as np
import psutil
import gc
import time
from typing import Dict, List

class MemoryOptimizer:
    """Optimize NLM memory usage."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_profiles = []
    
    def optimize_configuration_for_memory(self, memory_limit_mb=1000):
        """Optimize configuration to stay within memory limit."""
        print(f"Optimizing configuration for memory limit: {memory_limit_mb} MB...")
        
        # Start with current configuration
        current_config = self.capture_current_config()
        current_memory = self.measure_memory_usage()
        
        print(f"Current memory usage: {current_memory:.1f} MB")
        
        # Try different configurations to find optimal memory/performance balance
        optimizations = []
        
        # Try smaller brain configurations
        for neuron_count in [100, 200, 500, 1000, 2000]:
            config = current_config.copy()
            config.set("brain.neuron_count", neuron_count)
            
            memory = self.measure_configuration_memory(config)
            if memory <= memory_limit_mb:
                optimizations.append({
                    'neuron_count': neuron_count,
                    'memory_usage': memory,
                    'config': config
                })
        
        if not optimizations:
            print("No configuration fits memory limit - using most memory-efficient")
            # Use smallest possible configuration
            config = current_config.copy()
            config.set("brain.neuron_count", 100)
            optimizations.append({
                'neuron_count': 100,
                'memory_usage': self.measure_configuration_memory(config),
                'config': config
            })
        
        # Find best balance of performance and memory
        best_optimization = self.find_best_memory_optimization(optimizations)
        
        print(f"Best configuration: {best_optimization['neuron_count']} neurons")
        print(f"Memory usage: {best_optimization['memory_usage']:.1f} MB")
        
        return best_optimization['config']
    
    def optimize_plasticity_for_memory(self):
        """Optimize plasticity settings for memory efficiency."""
        print("Optimizing plasticity for memory efficiency...")
        
        # Capture current configuration
        current_config = self.capture_current_config()
        
        # Test different plasticity parameter combinations
        plasticity_combinations = [
            {'stdp_learning_rate': 0.01, 'stdp_ltp_weight': 0.01, 'stdp_ltd_weight': 0.012},
            {'stdp_learning_rate': 0.005, 'stdp_ltp_weight': 0.008, 'stdp_ltd_weight': 0.009},
            {'stdp_learning_rate': 0.002, 'stdp_ltp_weight': 0.005, 'stdp_ltd_weight': 0.006},
            {'stdp_learning_rate': 0.001, 'stdp_ltp_weight': 0.002, 'stdp_ltd_weight': 0.003}
        ]
        
        best_config = current_config
        best_memory = self.measure_memory_usage()
        
        for params in plasticity_combinations:
            # Create configuration with these parameters
            config = current_config.copy()
            config.set("plasticity.stdp.learning_rate", params['stdp_learning_rate'])
            config.set("plasticity.stdp.ltp_weight", params['stdp_ltp_weight'])
            config.set("plasticity.stdp.ltd_weight", params['stdp_ltd_weight'])
            
            # Measure memory usage
            memory = self.measure_configuration_memory(config)
            
            # Track if this is better
            if memory < best_memory:
                best_memory = memory
                best_config = config
                
                print(f"New best configuration: {params}")
                print(f"Memory usage: {memory:.1f} MB")
        
        return best_config
    
    def profile_memory_usage_over_time(self, steps=1000):
        """Profile memory usage over time to identify leaks."""
        print(f"Profiling memory usage over {steps} steps...")
        
        # Force garbage collection
        gc.collect()
        
        # Measure initial memory
        initial_memory = self.measure_memory_usage()
        
        memory_samples = []
        time_samples = []
        
        for step in range(steps):
            # Record memory before step
            memory_before = self.measure_memory_usage()
            
            # Run simulation step
            self.brain.step(step)
            
            # Record memory after step
            memory_after = self.measure_memory_usage()
            
            memory_samples.append(memory_after)
            time_samples.append(step)
            
            # Check for memory growth
            if memory_after > initial_memory * 1.5:  # 50% growth threshold
                print(f"Warning: Significant memory growth at step {step}")
                print(f"  Initial: {initial_memory:.1f} MB -> Current: {memory_after:.1f} MB")
        
        # Analyze memory profile
        memory_growth = memory_samples[-1] - memory_samples[0]
        avg_growth_rate = memory_growth / steps
        
        self.memory_profiles.append({
            'steps': steps,
            'initial_memory': initial_memory,
            'final_memory': memory_samples[-1],
            'memory_growth': memory_growth,
            'avg_growth_rate': avg_growth_rate,
            'memory_samples': memory_samples,
            'time_samples': time_samples
        })
        
        print(f"Memory growth analysis:")
        print(f"  Initial memory: {initial_memory:.1f} MB")
        print(f"  Final memory: {memory_samples[-1]:.1f} MB")
        print(f"  Total growth: {memory_growth:.1f} MB")
        print(f"  Average growth rate: {avg_growth_rate:.3f} MB/step")
        
        # Check for memory leaks
        if avg_growth_rate > 0.1:  # More than 0.1 MB per step
            print("  WARNING: Potential memory leak detected!")
        
        return self.memory_profiles[-1]
    
    def optimize_for_cache_efficiency(self):
        """Optimize configuration for CPU cache efficiency."""
        print("Optimizing for CPU cache efficiency...")
        
        # Capture current configuration
        current_config = self.capture_current_config()
        
        # Test different neuron counts that align with cache sizes
        cache_aligned_sizes = []
        
        # Try sizes that align with common cache line sizes
        # 64-byte cache lines, assuming each neuron uses ~1KB
        base_cache_size = 64 * 1024  # 64KB L1 cache
        cache_aligned_sizes.extend([64, 128, 256, 512, 1024, 2048])
        
        # Also try sizes that align with CPU cache hierarchy
        l1_cache_aligned = [32, 64, 128, 256]
        l2_cache_aligned = [256, 512, 1024, 2048, 4096]
        l3_cache_aligned = [1024, 2048, 4096, 8192, 16384]
        
        cache_aligned_sizes.extend(l1_cache_aligned)
        cache_aligned_sizes.extend(l2_cache_aligned)
        cache_aligned_sizes.extend(l3_cache_aligned)
        
        # Remove duplicates and sort
        cache_aligned_sizes = sorted(set(cache_aligned_sizes))
        
        best_config = current_config
        best_performance = self.benchmark_current_configuration()
        
        for neuron_count in cache_aligned_sizes:
            # Skip if too large
            if neuron_count > 10000:
                continue
            
            # Create configuration
            config = current_config.copy()
            config.set("brain.neuron_count", neuron_count)
            
            # Benchmark
            performance = self.benchmark_configuration(config, 100, 3)
            
            # Update best if better
            if performance > best_performance:
                best_performance = performance
                best_config = config
                print(f"New best: {neuron_count} neurons (performance: {performance:.2f})")
        
        print(f"Cache-optimized configuration: {best_config.get('brain.neuron_count', 'unknown')} neurons")
        return best_config
    
    def measure_memory_usage(self):
        """Measure current memory usage."""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / 1024 / 1024  # MB
    
    def measure_configuration_memory(self, config):
        """Measure memory usage of configuration."""
        # Create brain with configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure memory
        memory_before = self.measure_memory_usage()
        
        # Run quick simulation
        for step in range(50):
            brain.step(step)
        
        memory_after = self.measure_memory_usage()
        
        return max(memory_before, memory_after)
    
    def capture_current_config(self):
        """Capture current configuration."""
        # This would depend on how configuration is stored
        return pynlm.createDefaultConfig()
    
    def benchmark_current_configuration(self):
        """Benchmark current configuration."""
        config = self.capture_current_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        elapsed = end_time - start_time
        steps_per_second = 100 / elapsed
        firing_rate = brain.getAverageFiringRate()
        
        return (steps_per_second / 1000) + (firing_rate / 100)
    
    def find_best_memory_optimization(self, optimizations):
        """Find best memory optimization from list."""
        if not optimizations:
            raise ValueError("No optimizations available")
        
        # Calculate performance-to-memory ratio
        best_optimization = max(optimizations, key=lambda x: x['performance_score'])
        
        return best_optimization
```

## 4. Performance Monitoring and Alerts

### System Monitoring

```python
import pynlm
import psutil
import time
import threading
from typing import Dict, List, Callable
import numpy as np

class PerformanceMonitor:
    """Monitor NLM performance and system resources."""
    
    def __init__(self, brain):
        self.brain = brain
        self.monitoring = False
        self.monitored_metrics = {}
        self.alert_thresholds = {}
        self.alert_callbacks = {}
        self.performance_history = {}
        
        # Initialize monitoring
        self.initialize_monitoring()
        
        # Set up default alerts
        self.setup_default_alerts()
    
    def initialize_monitoring(self):
        """Initialize monitoring metrics."""
        self.monitored_metrics = {
            'cpu_usage': {'samples': [], 'interval': 0.1},
            'memory_usage': {'samples': [], 'interval': 0.1},
            'simulation_steps': {'samples': [], 'interval': 0.1},
            'firing_rate': {'samples': [], 'interval': 0.1},
            'neuron_count': {'value': self.brain.getTotalNeuronCount()},
            'synapse_count': {'value': self.brain.getTotalSynapseCount()}
        }
        
        self.performance_history = {
            'steps_per_second': [],
            'memory_efficiency': [],
            'cpu_efficiency': []
        }
    
    def start_monitoring(self, interval: float = 0.1):
        """Start monitoring system performance."""
        self.monitoring = True
        
        def monitoring_loop():
            while self.monitoring:
                self.collect_metrics()
                self.update_performance_history()
                time.sleep(interval)
        
        self.monitoring_thread = threading.Thread(target=monitoring_loop)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
        
        print(f"Performance monitoring started with {interval}s interval")
    
    def stop_monitoring(self):
        """Stop monitoring."""
        self.monitoring = False
        if hasattr(self, 'monitoring_thread'):
            self.monitoring_thread.join(timeout=5.0)
        print("Performance monitoring stopped")
    
    def collect_metrics(self):
        """Collect current system metrics."""
        # CPU usage
        cpu_percent = psutil.cpu_percent(interval=None)
        self.monitored_metrics['cpu_usage']['samples'].append(cpu_percent)
        
        # Memory usage
        memory_info = psutil.virtual_memory()
        self.monitored_metrics['memory_usage']['samples'].append(
            memory_info.used / 1024 / 1024  # MB
        )
        
        # Simulation metrics
        self.monitored_metrics['simulation_steps']['samples'].append(
            self.brain.getTotalNeuronCount() if self.monitoring else 0
        )
        
        self.monitored_metrics['firing_rate']['samples'].append(
            self.brain.getAverageFiringRate()
        )
        
        # Keep history limited
        max_samples = 1000
        for metric_name in self.monitored_metrics:
            if len(self.monitored_metrics[metric_name]['samples']) > max_samples:
                self.monitored_metrics[metric_name]['samples'].pop(0)
    
    def update_performance_history(self):
        """Update performance history."""
        if len(self.monitored_metrics['firing_rate']['samples']) >= 10:
            recent_firing_rates = self.monitored_metrics['firing_rate']['samples'][-10:]
            avg_firing_rate = np.mean(recent_firing_rates)
            self.performance_history['firing_rate_stability'] = np.std(recent_firing_rates) / (avg_firing_rate + 1e-6)
        
        if len(self.monitored_metrics['cpu_usage']['samples']) >= 10:
            avg_cpu = np.mean(self.monitored_metrics['cpu_usage']['samples'])
            self.performance_history['cpu_efficiency'] = 100 - avg_cpu  # Lower CPU usage = better efficiency
        
        if len(self.monitored_metrics['memory_usage']['samples']) >= 10:
            avg_memory = np.mean(self.monitored_metrics['memory_usage']['samples'])
            neurons = self.brain.getTotalNeuronCount()
            self.performance_history['memory_efficiency'] = neurons / (avg_memory * 1024 * 1024) if avg_memory > 0 else 0
    
    def setup_default_alerts(self):
        """Set up default alert thresholds."""
        self.alert_thresholds = {
            'high_cpu_usage': {'threshold': 80, 'severity': 'warning', 'unit': '%'},
            'high_memory_usage': {'threshold': 2000, 'severity': 'warning', 'unit': 'MB'},
            'low_firing_rate': {'threshold': 10, 'severity': 'info', 'unit': 'Hz'},
            'high_firing_rate': {'threshold': 1000, 'severity': 'warning', 'unit': 'Hz'},
            'poor_memory_efficiency': {'threshold': 1000, 'severity': 'warning', 'unit': 'neurons/MB'}
        }
    
    def register_alert(self, metric_name: str, threshold: float, severity: str, callback: Callable):
        """Register alert for specific metric."""
        alert_key = f"{metric_name}_{threshold}"
        self.alert_callbacks[alert_key] = {
            'metric_name': metric_name,
            'threshold': threshold,
            'severity': severity,
            'callback': callback
        }
    
    def check_alerts(self):
        """Check if any metrics exceed alert thresholds."""
        # Check CPU usage
        if self.monitored_metrics['cpu_usage']['samples']:
            current_cpu = self.monitored_metrics['cpu_usage']['samples'][-1]
            if current_cpu > 80:
                self.trigger_alert('high_cpu_usage', current_cpu, 80)
        
        # Check memory usage
        if self.monitored_metrics['memory_usage']['samples']:
            current_memory = self.monitored_metrics['memory_usage']['samples'][-1]
            if current_memory > 2000:
                self.trigger_alert('high_memory_usage', current_memory, 2000)
        
        # Check firing rate
        if self.monitored_metrics['firing_rate']['samples']:
            current_firing = self.monitored_metrics['firing_rate']['samples'][-1]
            if current_firing < 10:
                self.trigger_alert('low_firing_rate', current_firing, 10)
            elif current_firing > 1000:
                self.trigger_alert('high_firing_rate', current_firing, 1000)
        
        # Check memory efficiency
        if 'memory_efficiency' in self.performance_history:
            current_efficiency = self.performance_history['memory_efficiency']
            if current_efficiency < 1000:
                self.trigger_alert('poor_memory_efficiency', current_efficiency, 1000)
    
    def trigger_alert(self, alert_type: str, current_value: float, threshold: float):
        """Trigger alert for exceeded threshold."""
        alert = {
            'type': alert_type,
            'current_value': current_value,
            'threshold': threshold,
            'timestamp': time.time()
        }
        
        print(f"[{alert_type.upper()}] Alert: {alert_type} exceeded {threshold} (current: {current_value})")
        
        # Execute alert callbacks
        alert_key = f"{alert_type}_{threshold}"
        if alert_key in self.alert_callbacks:
            callback_info = self.alert_callbacks[alert_key]
            try:
                callback_info['callback'](alert)
            except Exception as e:
                print(f"Alert callback failed: {e}")
    
    def get_monitoring_report(self) -> str:
        """Generate monitoring report."""
        report = f"# NLM Performance Monitoring Report\n\n"
        
        report += f"## Current Status\n"
        if self.monitored_metrics['cpu_usage']['samples']:
            avg_cpu = np.mean(self.monitored_metrics['cpu_usage']['samples'])
            report += f"- Average CPU Usage: {avg_cpu:.1f}%\n"
        
        if self.monitored_metrics['memory_usage']['samples']:
            avg_memory = np.mean(self.monitored_metrics['memory_usage']['samples'])
            report += f"- Average Memory Usage: {avg_memory:.1f} MB\n"
        
        if 'firing_rate_stability' in self.performance_history:
            stability = self.performance_history['firing_rate_stability']
            report += f"- Firing Rate Stability: {stability:.3f} (lower is better)\n"
        
        if 'cpu_efficiency' in self.performance_history:
            efficiency = self.performance_history['cpu_efficiency']
            report += f"- CPU Efficiency: {efficiency:.1f}% (higher is better)\n"
        
        if 'memory_efficiency' in self.performance_history:
            efficiency = self.performance_history['memory_efficiency']
            report += f"- Memory Efficiency: {efficiency:.1f} neurons/MB (higher is better)\n"
        
        return report
```

## 5. Performance Comparison and Analysis

### Configuration Comparison Framework

```python
import pynlm
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Dict, List

class ConfigurationComparator:
    """Compare performance across different configurations."""
    
    def __init__(self):
        self.comparison_results = {}
    
    def run_benchmark_suite(self, config_dict: Dict[str, any], benchmark_config: Dict):
        """Run comprehensive benchmark suite across configurations."""
        print(f"Running benchmark suite for {len(config_dict)} configurations...")
        
        benchmark_params = {
            'steps': benchmark_config.get('steps', 100),
            'iterations': benchmark_config.get('iterations', 5),
            'warmup_steps': benchmark_config.get('warmup_steps', 10),
            'metrics': benchmark_config.get('metrics', ['performance', 'memory', 'stability'])
        }
        
        for config_name, config in config_dict.items():
            print(f"  Benchmarking {config_name}...")
            
            results = self.benchmark_configuration(
                config, config_name, benchmark_params
            )
            self.comparison_results[config_name] = results
        
        return self.comparison_results
    
    def benchmark_configuration(self, config, config_name: str, benchmark_config: Dict):
        """Benchmark a single configuration."""
        results = {
            'config_name': config_name,
            'parameters': benchmark_config,
            'iterations': [],
            'summary': {}
        }
        
        for iteration in range(benchmark_config['iterations']):
            # Warm-up phase
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(benchmark_config['warmup_steps']):
                brain.step(step)
            
            # Benchmark phase
            iteration_results = self.run_benchmark_iteration(
                brain, benchmark_config
            )
            
            results['iterations'].append(iteration_results)
        
        # Calculate summary statistics
        results['summary'] = self.calculate_summary_statistics(results['iterations'])
        
        return results
    
    def run_benchmark_iteration(self, brain, benchmark_config: Dict):
        """Run single benchmark iteration."""
        start_time = time.time()
        
        # Run benchmark
        for step in range(benchmark_config['steps']):
            brain.step(step)
        
        end_time = time.time()
        
        # Calculate metrics
        elapsed_time = end_time - start_time
        steps_per_second = benchmark_config['steps'] / elapsed_time
        
        return {
            'iteration': len([i for i in benchmark_config if i == 'iteration']),  # Simplified
            'elapsed_time': elapsed_time,
            'steps_per_second': steps_per_second,
            'firing_rate': brain.getAverageFiringRate(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'memory_efficiency': brain.getTotalNeuronCount() / (elapsed_time * 1024 * 1024) if elapsed_time > 0 else 0
        }
    
    def calculate_summary_statistics(self, iterations: List[Dict]) -> Dict:
        """Calculate summary statistics from benchmark iterations."""
        if not iterations:
            return {}
        
        stats = {}
        
        # Steps per second
        steps_per_second = [i['steps_per_second'] for i in iterations]
        stats['steps_per_second'] = {
            'mean': np.mean(steps_per_second),
            'std': np.std(steps_per_second),
            'min': np.min(steps_per_second),
            'max': np.max(steps_per_second),
            'cv': np.std(steps_per_second) / np.mean(steps_per_second) if np.mean(steps_per_second) > 0 else 0
        }
        
        # Firing rate
        firing_rates = [i['firing_rate'] for i in iterations]
        stats['firing_rate'] = {
            'mean': np.mean(firing_rates),
            'std': np.std(firing_rates),
            'min': np.min(firing_rates),
            'max': np.max(firing_rates)
        }
        
        # Memory efficiency
        memory_efficiencies = [i['memory_efficiency'] for i in iterations]
        stats['memory_efficiency'] = {
            'mean': np.mean(memory_efficiencies),
            'std': np.std(memory_efficiencies),
            'min': np.min(memory_efficiencies),
            'max': np.max(memory_efficiencies)
        }
        
        # Time
        times = [i['elapsed_time'] for i in iterations]
        stats['elapsed_time'] = {
            'mean': np.mean(times),
            'std': np.std(times),
            'min': np.min(times),
            'max': np.max(times)
        }
        
        return stats
    
    def generate_comparison_report(self, results: Dict) -> str:
        """Generate comparison report."""
        report = "# NLM Configuration Comparison Report\n\n"
        
        # Create comparison table
        report += self.create_comparison_table(results)
        
        # Performance rankings
        report += self.create_performance_rankings(results)
        
        # Statistical comparisons
        report += self.create_statistical_comparisons(results)
        
        # Recommendations
        report += self.create_recommendations(results)
        
        return report
    
    def create_comparison_table(self, results: Dict) -> str:
        """Create comparison table."""
        report = f"## Configuration Comparison\n"
        report += f"| Configuration | Steps/Second | Firing Rate (Hz) | Memory Efficiency | Avg Time (s) |\n"
        report += f"|---------------|-------------|------------------|-------------------|--------------|\n"
        
        for config_name, config_results in results.items():
            stats = config_results['summary']
            
            report += f"| {config_name} "
            report += f"| {stats.get('steps_per_second', {}).get('mean', 'N/A'):.1f} ± {stats.get('steps_per_second', {}).get('std', 0):.1f} "
            report += f"| {stats.get('firing_rate', {}).get('mean', 'N/A'):.1f} ± {stats.get('firing_rate', {}).get('std', 0):.1f} "
            report += f"| {stats.get('memory_efficiency', {}).get('mean', 'N/A'):.1f} ± {stats.get('memory_efficiency', {}).get('std', 0):.1f} "
            report += f"| {stats.get('elapsed_time', {}).get('mean', 'N/A'):.2f} ± {stats.get('elapsed_time', {}).get('std', 0):.2f} "
            report += f"|\n"
        
        return report + f"\n\n"
    
    def create_performance_rankings(self, results: Dict) -> str:
        """Create performance rankings."""
        report = f"## Performance Rankings\n"
        
        # Rank by performance (higher is better)
        ranked_configs = sorted(
            results.items(),
            key=lambda x: x[1]['summary'].get('steps_per_second', {}).get('mean', 0),
            reverse=True
        )
        
        report += f"1. **Overall Performance Ranking** (by steps/second):\n"
        for i, (config_name, config_results) in enumerate(ranked_configs, 1):
            stats = config_results['summary']
            steps_per_second = stats.get('steps_per_second', {}).get('mean', 0)
            report += f"   {i}. {config_name}: {steps_per_second:.1f} steps/second\n"
        
        # Rank by memory efficiency
        memory_ranked = sorted(
            results.items(),
            key=lambda x: x[1]['summary'].get('memory_efficiency', {}).get('mean', 0),
            reverse=True
        )
        
        report += f"\n2. **Memory Efficiency Ranking** (by neurons/MB):\n"
        for i, (config_name, config_results) in enumerate(memory_ranked, 1):
            stats = config_results['summary']
            efficiency = stats.get('memory_efficiency', {}).get('mean', 0)
            report += f"   {i}. {config_name}: {efficiency:.1f} neurons/MB\n"
        
        return report + f"\n\n"
    
    def create_statistical_comparisons(self, results: Dict) -> str:
        """Create statistical comparisons."""
        report = f"## Statistical Comparisons\n"
        
        # Analysis of variance for key metrics
        config_names = list(results.keys())
        
        if len(config_names) > 1:
            # Steps per second comparison
            sps_data = []
            for config_name in config_names:
                stats = results[config_name]['summary'].get('steps_per_second', {})
                mean_sps = stats.get('mean', 0)
                std_sps = stats.get('std', 0)
                count = 5  # Number of iterations
                
                sps_data.append({
                    'config': config_name,
                    'mean': mean_sps,
                    'std': std_sps,
                    'n': count
                })
            
            df_sps = pd.DataFrame(sps_data)
            report += f"### Steps/Second Analysis\n"
            report += f"- Mean: {df_sps['mean'].mean():.1f} ± {df_sps['mean'].std():.1f}\n"
            report += f"- Range: {df_sps['mean'].min():.1f} - {df_sps['mean'].max():.1f}\n"
            report += f"- Coefficient of Variation: {df_sps['std'].mean() / df_sps['mean'].mean():.3f}\n"
            
            # Generate box plot code
            report += f"\n```python\n# Box plot code for steps per second\nimport matplotlib.pyplot as plt\nimport seaborn as sns\n\ndata = {{}}\nfor _, row in df_sps.iterrows():\n    # This would need actual benchmark data\n    pass\n\n# Example plot (uncomment to use)\n# fig, ax = plt.subplots(figsize=(10, 6))\n# sns.boxplot(data=data, ax=ax)\n# plt.title('Configuration Comparison: Steps/Second')\n# plt.show()\n```\n"
        
        return report + f"\n\n"
    
    def create_recommendations(self, results: Dict) -> str:
        """Create recommendations based on comparison."""
        report = f"## Recommendations\n"
        
        # Find best overall configuration
        best_overall = max(
            results.items(),
            key=lambda x: x[1]['summary'].get('steps_per_second', {}).get('mean', 0)
        )
        
        report += f"### Recommended Configuration\n"
        report += f"Based on overall performance, **{best_overall[0]}** is recommended.\n"
        
        stats = best_overall[1]['summary']
        report += f"- **Performance**: {stats.get('steps_per_second', {}).get('mean', 0):.1f} steps/second\n"
        report += f"- **Firing Rate**: {stats.get('firing_rate', {}).get('mean', 0):.1f} Hz\n"
        report += f"- **Memory Efficiency**: {stats.get('memory_efficiency', {}).get('mean', 0):.1f} neurons/MB\n"
        report += f"- **Average Time**: {stats.get('elapsed_time', {}).get('mean', 0):.2f} seconds per iteration\n"
        
        # Alternative recommendations
        report += f"\n### Alternative Recommendations\n"
        
        # Best for memory efficiency
        best_memory = max(
            results.items(),
            key=lambda x: x[1]['summary'].get('memory_efficiency', {}).get('mean', 0)
        )
        
        report += f"1. **Memory-Constrained Environments**: {best_memory[0]}\n"
        report += f"   - Best memory efficiency with {best_memory[1]['summary'].get('memory_efficiency', {}).get('mean', 0):.1f} neurons/MB\n"
        
        # Best for firing rate stability
        best_stability = min(
            results.items(),
            key=lambda x: x[1]['summary'].get('firing_rate', {}).get('std', float('inf'))
        )
        
        report += f"2. **Stable Performance**: {best_stability[0]}\n"
        report += f"   - Most stable firing rate with {best_stability[1]['summary'].get('firing_rate', {}).get('std', 0):.1f} Hz standard deviation\n"
        
        return report
```

## 6. Performance Testing Strategies

### Stress Testing

```python
import pynlm
import time
import psutil
import threading
import queue
from typing import Dict, List

class StressTester:
    """Stress test NLM configurations under extreme conditions."""
    
    def __init__(self):
        self.stress_results = {}
    
    def stress_test_configuration(self, config, name, test_scenario: str, steps=500):
        """Stress test a configuration under specified conditions."""
        print(f"Stress testing {name} with scenario: {test_scenario}...")
        
        if test_scenario == "high_load":
            return self.stress_test_high_load(config, name, steps)
        elif test_scenario == "memory_constrained":
            return self.stress_test_memory_constrained(config, name, steps)
        elif test_scenario == "cpu_limited":
            return self.stress_test_cpu_limited(config, name, steps)
        elif test_scenario == "simultaneous_processes":
            return self.stress_test_simultaneous_processes(config, name, steps)
        else:
            return self.stress_test_standard(config, name, steps)
    
    def stress_test_high_load(self, config, name, steps):
        """Stress test with high computational load."""
        results = {
            'config_name': name,
            'scenario': 'high_load',
            'iterations': [],
            'max_cpu_usage': 0,
            'max_memory_usage': 0,
            'failures': 0
        }
        
        for iteration in range(5):
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Monitor system
            start_time = time.time()
            max_cpu = 0
            max_memory = 0
            failure = False
            
            for step in range(steps):
                # Sample system metrics
                cpu = psutil.cpu_percent(interval=None)
                memory = psutil.virtual_memory().used / 1024 / 1024  # MB
                
                max_cpu = max(max_cpu, cpu)
                max_memory = max(max_memory, memory)
                
                # Run simulation
                try:
                    brain.step(step)
                except Exception as e:
                    failure = True
                    print(f"  Iteration {iteration + 1}, step {step}: Error - {e}")
                    break
            
            elapsed_time = time.time() - start_time
            
            # Update results
            results['iterations'].append({
                'iteration': iteration + 1,
                'elapsed_time': elapsed_time,
                'steps_per_second': steps / elapsed_time,
                'max_cpu_usage': max_cpu,
                'max_memory_usage': max_memory,
                'success': not failure
            })
            
            if failure:
                results['failures'] += 1
            
            results['max_cpu_usage'] = max(results['max_cpu_usage'], max_cpu)
            results['max_memory_usage'] = max(results['max_memory_usage'], max_memory)
        
        return results
    
    def stress_test_memory_constrained(self, config, name, steps):
        """Stress test with memory constraints."""
        print(f"  Testing with memory constraints (memory_limit: 1024 MB)...")
        
        # Set memory limit
        memory_limit = 1024  # MB
        
        results = {
            'config_name': name,
            'scenario': 'memory_constrained',
            'iterations': [],
            'memory_exceeded': False
        }
        
        for iteration in range(3):
            # Monitor memory before
            memory_before = psutil.virtual_memory().used / 1024 / 1024
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Monitor during simulation
            max_memory = memory_before
            failure = False
            
            for step in range(steps):
                # Check memory
                current_memory = psutil.virtual_memory().used / 1024 / 1024
                max_memory = max(max_memory, current_memory)
                
                # Check if exceeding limit
                if current_memory > memory_limit:
                    print(f"    Memory limit exceeded: {current_memory:.1f} MB > {memory_limit} MB")
                    failure = True
                    break
                
                # Run simulation
                try:
                    brain.step(step)
                except MemoryError:
                    print(f"    Memory error at step {step}")
                    failure = True
                    break
            
            elapsed_time = time.time() - start_time
            
            results['iterations'].append({
                'iteration': iteration + 1,
                'elapsed_time': elapsed_time,
                'max_memory_usage': max_memory,
                'memory_exceeded': max_memory > memory_limit,
                'success': not failure
            })
            
            if max_memory > memory_limit:
                results['memory_exceeded'] = True
        
        return results
    
    def stress_test_simultaneous_processes(self, config, name, steps):
        """Stress test with multiple simultaneous processes."""
        print(f"  Testing with concurrent processes...")
        
        # Create process queue
        process_queue = queue.Queue()
        results_queue = queue.Queue()
        
        def worker(process_id, config, steps, result_queue):
            """Worker process for concurrent testing."""
            try:
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                start_time = time.time()
                for step in range(steps):
                    brain.step(step)
                
                end_time = time.time()
                
                result_queue.put({
                    'process_id': process_id,
                    'elapsed_time': end_time - start_time,
                    'steps_per_second': steps / (end_time - start_time),
                    'firing_rate': brain.getAverageFiringRate(),
                    'success': True
                })
            
            except Exception as e:
                result_queue.put({
                    'process_id': process_id,
                    'error': str(e),
                    'success': False
                })
        
        # Start worker processes
        processes = []
        for i in range(3):
            process = threading.Thread(target=worker, args=(i, config, steps, results_queue))
            process.start()
            processes.append(process)
        
        # Wait for completion
        for process in processes:
            process.join()
        
        # Collect results
        concurrent_results = []
        while not results_queue.empty():
            concurrent_results.append(results_queue.get())
        
        return {
            'config_name': name,
            'scenario': 'simultaneous_processes',
            'concurrent_results': concurrent_results,
            'successful_processes': sum(1 for r in concurrent_results if r.get('success', False)),
            'total_processes': len(concurrent_results)
        }
    
    def run_stress_test_suite(self, config_dict: Dict[str, any], scenarios: List[str]):
        """Run complete stress test suite."""
        print(f"Running stress test suite for {len(config_dict)} configurations across {len(scenarios)} scenarios...")
        
        stress_results = {}
        
        for config_name, config in config_dict.items():
            print(f"  {config_name}:")
            
            for scenario in scenarios:
                print(f"    {scenario}...")
                results = self.stress_test_configuration(config, config_name, scenario)
                stress_results[f"{config_name}_{scenario}"] = results
        
        return stress_results
    
    def generate_stress_test_report(self, results: Dict) -> str:
        """Generate stress test report."""
        report = "# NLM Stress Test Report\n\n"
        
        for test_name, test_results in results.items():
            report += f"## {test_name}\n"
            
            if test_results['scenario'] == 'high_load':
                report += self.generate_high_load_report(test_results)
            elif test_results['scenario'] == 'memory_constrained':
                report += self.generate_memory_constrained_report(test_results)
            elif test_results['scenario'] == 'simultaneous_processes':
                report += self.generate_concurrent_processes_report(test_results)
            
            report += f"\n"
        
        return report
    
    def generate_high_load_report(self, results: Dict) -> str:
        """Generate high load stress test report."""
        report = f"### High Load Stress Test Results\n"
        
        iterations = results['iterations']
        
        report += f"- **Maximum CPU Usage**: {results['max_cpu_usage']:.1f}%\n"
        report += f"- **Maximum Memory Usage**: {results['max_memory_usage']:.1f} MB\n"
        report += f"- **Failures**: {results['failures']}/{len(iterations)}\n"
        
        if iterations:
            avg_steps_per_second = np.mean([i['steps_per_second'] for i in iterations])
            avg_cpu = np.mean([i['max_cpu_usage'] for i in iterations])
            avg_memory = np.mean([i['max_memory_usage'] for i in iterations])
            
            report += f"\n- **Performance Summary**:\n"
            report += f"  - Average Steps/Second: {avg_steps_per_second:.1f}\n"
            report += f"  - Average Max CPU: {avg_cpu:.1f}%\n"
            report += f"  - Average Max Memory: {avg_memory:.1f} MB\n"
        
        return report
    
    def generate_memory_constrained_report(self, results: Dict) -> str:
        """Generate memory constrained stress test report."""
        report = f"### Memory Constrained Stress Test Results\n"
        
        iterations = results['iterations']
        
        if results['memory_exceeded']:
            report += f"- **Memory Limit Exceeded**: Yes\n"
        else:
            report += f"- **Memory Limit Exceeded**: No\n"
        
        report += f"- **Test Status**: {'PASSED' if not results['memory_exceeded'] else 'FAILED'}\n"
        
        if iterations:
            avg_steps_per_second = np.mean([i['steps_per_second'] for i in iterations])
            max_memory = np.max([i['max_memory_usage'] for i in iterations])
            
            report += f"\n- **Performance Summary**:\n"
            report += f"  - Average Steps/Second: {avg_steps_per_second:.1f}\n"
            report += f"  - Maximum Memory Usage: {max_memory:.1f} MB\n"
        
        return report
    
    def generate_concurrent_processes_report(self, results: Dict) -> str:
        """Generate concurrent processes stress test report."""
        report = f"### Concurrent Processes Stress Test Results\n"
        
        concurrent_results = results['concurrent_results']
        
        successful = [r for r in concurrent_results if r.get('success', False)]
        failed = [r for r in concurrent_results if not r.get('success', False)]
        
        report += f"- **Total Processes**: {results['total_processes']}\n"
        report += f"- **Successful Processes**: {len(successful)}\n"
        report += f"- **Failed Processes**: {len(failed)}\n"
        
        if successful:
            avg_steps_per_second = np.mean([r['steps_per_second'] for r in successful])
            avg_firing_rate = np.mean([r['firing_rate'] for r in successful])
            
            report += f"\n- **Performance Summary (Successful Processes)**:\n"
            report += f"  - Average Steps/Second: {avg_steps_per_second:.1f}\n"
            report += f"  - Average Firing Rate: {avg_firing_rate:.1f} Hz\n"
        
        if failed:
            report += f"\n- **Failed Processes Details**:\n"
            for failure in failed:
                report += f"  - Process {failure.get('process_id', 'unknown')}: {failure.get('error', 'Unknown error')}\n"
        
        return report
```

## 7. Performance Optimization Checklist

### Pre-Deployment Checklist

| Checklist Item | Status | Description |
|----------------|--------|-------------|
| **Configuration Validation** | ☐ | Validate configuration before deployment |
| **Performance Profiling** | ☐ | Profile performance with representative workloads |
| **Memory Usage Monitoring** | ☐ | Monitor memory usage during operation |
| **Benchmark Testing** | ☐ | Run comprehensive benchmark tests |
| **Stress Testing** | ☐ | Test under extreme conditions |
| **Error Handling** | ☐ | Verify error handling and recovery mechanisms |
| **Documentation** | ☐ | Document performance characteristics and optimization |
| **Monitoring Setup** | ☐ | Set up performance monitoring and alerting |

### Performance Optimization Guidelines

1. **Configuration Optimization**
   - Use performance-optimized templates (high_perf.cfg)
   - Balance neuron count with available memory
   - Optimize connectivity settings for target workload
   - Consider development vs production requirements

2. **Algorithm Optimization**
   - Use efficient data structures
   - Optimize critical paths
   - Consider parallelization where appropriate
   - Use caching for frequently accessed data

3. **System Optimization**
   - Monitor system resources
   - Optimize operating system settings
   - Consider hardware acceleration
   - Implement efficient memory management

4. **Testing and Validation**
   - Test with representative workloads
   - Validate under various conditions
   - Monitor performance over time
   - Set up automated performance regression testing

## 8. Conclusion

Performance optimization is a critical aspect of NLM deployment and operation. By implementing comprehensive performance measurement, benchmarking, optimization techniques, and monitoring systems, NLM users can achieve optimal performance across diverse deployment scenarios.

Key aspects of NLM performance management:

1. **Measurement**: Implement comprehensive performance measurement and monitoring
2. **Benchmarking**: Use standardized benchmarks for comparison
3. **Optimization**: Apply configuration-based and algorithmic optimizations
4. **Stress Testing**: Test under extreme conditions to identify weaknesses
5. **Monitoring**: Continuously monitor performance and set up alerts
6. **Documentation**: Document performance characteristics and optimization procedures

By following these guidelines, NLM users can build high-performance, reliable neural simulation systems that meet their specific requirements and scale effectively with increasing complexity and workload.
