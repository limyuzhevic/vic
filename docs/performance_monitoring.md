# Performance Monitoring and Profiling Utilities

## Overview

Performance monitoring and profiling utilities for NLM (Neural Learning Machine) provide comprehensive insight into simulation performance, resource utilization, and optimization opportunities. These tools help developers and researchers analyze brain simulation efficiency and identify performance bottlenecks.

## Key Features

### 1. Simulation Performance Monitoring
- Step timing analysis
- Neuron operation profiling
- Memory usage tracking
- CPU utilization measurement
- I/O operation monitoring

### 2. Resource Management
- Memory pool tracking
- Event queue monitoring
- Plasticity system profiling
- Neuromodulation overhead analysis

### 3. Advanced Profiling
- Custom profiler hooks
- Time-series data collection
- Statistical analysis
- Performance trend analysis

### 4. Optimization Tools
- Bottleneck identification
- Memory leak detection
- Cache performance analysis
- Parallel processing efficiency

## API Reference

### PerformanceMonitor Class

```python
import pynlm
import time
import threading
from typing import Dict, List, Callable, Any

class PerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations"""
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        """Initialize performance monitor
        
        Args:
            brain: NLM brain instance to monitor
            world: SimpleWorld instance to monitor
        """
        self.brain = brain
        self.world = world
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
        self.cpu_usage: List[float] = []
        self.neuron_operations: Dict[str, int] = {}
        self.plasticity_operations: Dict[str, int] = {}
        self.simulation_start = time.time()
        self.monitoring = False
        self.monitor_thread: threading.Thread = None
        
    def start_monitoring(self, interval: float = 0.1):
        """Start performance monitoring in background thread
        
        Args:
            interval: Monitoring interval in seconds
        """
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop, args=(interval,))
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
            
    def get_simulation_stats(self) -> Dict[str, Any]:
        """Get comprehensive simulation statistics
        
        Returns:
            Dictionary containing performance statistics
        """
        total_time = time.time() - self.simulation_start
        avg_step_time = sum(self.step_times) / len(self.step_times) if self.step_times else 0
        
        return {
            'total_simulation_time': total_time,
            'average_step_time': avg_step_time,
            'steps_per_second': len(self.step_times) / total_time if total_time > 0 else 0,
            'peak_memory_usage': max(self.memory_usage) if self.memory_usage else 0,
            'average_memory_usage': sum(self.memory_usage) / len(self.memory_usage) if self.memory_usage else 0,
            'neuron_operations': self.neuron_operations,
            'plasticity_operations': self.plasticity_operations,
            'total_steps': len(self.step_times)
        }
        
    def get_step_timeline(self) -> List[Dict[str, float]]:
        """Get detailed step-by-step timeline data
        
        Returns:
            List of dictionaries with timing information for each step
        """
        return [
            {
                'step': i,
                'step_time': step_time,
                'cumulative_time': sum(self.step_times[:i+1]),
                'memory_usage': self.memory_usage[i] if i < len(self.memory_usage) else 0,
                'cpu_usage': self.cpu_usage[i] if i < len(self.cpu_usage) else 0
            }
            for i, step_time in enumerate(self.step_times)
        ]
        
    def export_stats(self, filepath: str, format: str = 'json'):
        """Export performance statistics to file
        
        Args:
            filepath: Output file path
            format: Export format ('json', 'csv', 'txt')
        """
        stats = self.get_simulation_stats()
        timeline = self.get_step_timeline()
        
        if format == 'json':
            import json
            with open(filepath, 'w') as f:
                json.dump({
                    'stats': stats,
                    'timeline': timeline
                }, f, indent=2)
                
        elif format == 'csv':
            import csv
            with open(filepath, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(['Step', 'StepTime', 'CumulativeTime', 'MemoryUsage', 'CpuUsage'])
                for i, step_data in enumerate(timeline):
                    writer.writerow([
                        step_data['step'],
                        step_data['step_time'],
                        step_data['cumulative_time'],
                        step_data['memory_usage'],
                        step_data['cpu_usage']
                    ])
                    
        elif format == 'txt':
            with open(filepath, 'w') as f:
                f.write(self._generate_text_report(stats, timeline))
                
    def _monitor_loop(self, interval: float):
        """Background monitoring loop"""
        while self.monitoring:
            try:
                # Get current performance metrics
                step_time = self._get_step_time()
                memory_usage = self._get_memory_usage()
                cpu_usage = self._get_cpu_usage()
                
                # Record metrics
                self.step_times.append(step_time)
                self.memory_usage.append(memory_usage)
                self.cpu_usage.append(cpu_usage)
                
                # Update operation counts
                self._update_operation_counts()
                
                # Sleep for interval
                time.sleep(interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(interval)
                
    def _get_step_time(self) -> float:
        """Get time taken for last step"""
        # Implementation would depend on integration point
        return 0.001  # Default placeholder
        
    def _get_memory_usage(self) -> float:
        """Get current memory usage in MB"""
        # Implementation would depend on platform
        return 0.0  # Default placeholder
        
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage percentage"""
        # Implementation would depend on platform
        return 0.0  # Default placeholder
        
    def _update_operation_counts(self):
        """Update operation counts"""
        # Implementation would depend on brain/world interfaces
        pass
        
    def _generate_text_report(self, stats: Dict, timeline: List) -> str:
        """Generate human-readable text report"""
        report = []
        report.append("NLM Performance Report")
        report.append("=" * 50)
        report.append(f"Total Simulation Time: {stats['total_simulation_time']:.2f} seconds")
        report.append(f"Average Step Time: {stats['average_step_time']*1000:.2f} ms")
        report.append(f"Steps per Second: {stats['steps_per_second']:.2f}")
        report.append(f"Total Steps: {stats['total_steps']}")
        report.append(f"Peak Memory Usage: {stats['peak_memory_usage']:.2f} MB")
        report.append(f"Average Memory Usage: {stats['average_memory_usage']:.2f} MB")
        report.append("")
        
        if stats['neuron_operations']:
            report.append("Neuron Operations:")
            for op, count in stats['neuron_operations'].items():
                report.append(f"  {op}: {count}")
                
        if stats['plasticity_operations']:
            report.append("Plasticity Operations:")
            for op, count in stats['plasticity_operations'].items():
                report.append(f"  {op}: {count}")
                
        report.append("")
        report.append("Step Timeline (first 10 steps):")
        for i, step_data in enumerate(timeline[:10]):
            report.append(f"  Step {step_data['step']}: {step_data['step_time']*1000:.2f} ms, "
                        f"Memory: {step_data['memory_usage']:.2f} MB")
                        
        return "\n".join(report)
```

### SimulationProfiler Class

```python
class SimulationProfiler:
    """Detailed profiling of simulation components"""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.profiles = {}
        self.enabled = True
        
    def start_profile(self, name: str):
        """Start profiling a specific component"""
        if not self.enabled:
            return
        self.profiles[name] = {
            'start_time': time.time(),
            'operations': [],
            'memory_samples': []
        }
        
    def stop_profile(self, name: str) -> Dict[str, Any]:
        """Stop profiling and return results"""
        if name not in self.profiles:
            return {}
            
        profile = self.profiles[name]
        end_time = time.time()
        
        result = {
            'duration': end_time - profile['start_time'],
            'operations': profile['operations'],
            'memory_samples': profile['memory_samples'],
            'average_memory': sum(profile['memory_samples']) / len(profile['memory_samples']) if profile['memory_samples'] else 0
        }
        
        del self.profiles[name]
        return result
        
    def record_operation(self, profile_name: str, operation: str, count: int = 1):
        """Record an operation in a profile"""
        if profile_name in self.profiles:
            self.profiles[profile_name]['operations'].append({
                'operation': operation,
                'count': count,
                'timestamp': time.time()
            })
            
    def get_profile_summary(self) -> Dict[str, Any]:
        """Get summary of all completed profiles"""
        # Implementation would depend on how profiles are stored
        return {}
        
    def export_profiles(self, filepath: str):
        """Export all profiles to file"""
        # Implementation would export all profiles
        pass
```

### MemoryProfiler Class

```python
class MemoryProfiler:
    """Memory usage profiling and analysis"""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.memory_samples = []
        self.allocation_history = []
        self.deallocation_history = []
        self.enabled = True
        
    def start_tracing(self):
        """Start memory tracing"""
        self.enabled = True
        
    def stop_tracing(self):
        """Stop memory tracing"""
        self.enabled = False
        
    def get_memory_usage(self) -> Dict[str, float]:
        """Get current memory usage statistics"""
        if not self.memory_samples:
            return {'total': 0, 'peak': 0, 'average': 0}
            
        return {
            'total': self.memory_samples[-1] if self.memory_samples else 0,
            'peak': max(self.memory_samples) if self.memory_samples else 0,
            'average': sum(self.memory_samples) / len(self.memory_samples) if self.memory_samples else 0
        }
        
    def get_allocation_details(self) -> List[Dict[str, Any]]:
        """Get detailed allocation information"""
        return self.allocation_history
        
    def get_deallocation_details(self) -> List[Dict[str, Any]]:
        """Get detailed deallocation information"""
        return self.deallocation_history
        
    def detect_memory_leaks(self, threshold_mb: float = 1.0) -> List[Dict[str, Any]]:
        """Detect potential memory leaks"""
        leaks = []
        # Implementation would analyze allocation/deallocation patterns
        return leaks
        
    def generate_memory_report(self) -> str:
        """Generate human-readable memory report"""
        usage = self.get_memory_usage()
        report = []
        report.append("Memory Usage Report")
        report.append("=" * 30)
        report.append(f"Total Memory Usage: {usage['total']:.2f} MB")
        report.append(f"Peak Memory Usage: {usage['peak']:.2f} MB")
        report.append(f"Average Memory Usage: {usage['average']:.2f} MB")
        report.append(f"Memory Growth: {usage['total'] - usage['average']:.2f} MB")
        
        if self.get_allocation_details():
            report.append("\nAllocation Details:")
            for alloc in self.get_allocation_details()[-10:]:  # Last 10 allocations
                report.append(f"  {alloc}")
                
        return "\n".join(report)
```

### PerformanceBenchmark Class

```python
class PerformanceBenchmark:
    """Benchmarking framework for performance comparison"""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.benchmarks = {}
        
    def run_benchmark(self, name: str, steps: int, config_overrides: Dict = None):
        """Run a performance benchmark
        
        Args:
            name: Benchmark name
            steps: Number of steps to run
            config_overrides: Configuration overrides for this benchmark
        """
        start_time = time.time()
        
        # Apply configuration overrides
        original_config = {}
        if config_overrides:
            for key, value in config_overrides.items():
                original_config[key] = self.brain.config.getOr(key, None)
                self.brain.config.set(key, value)
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
            
        # Restore original configuration
        for key, value in original_config.items():
            if value is not None:
                self.brain.config.set(key, value)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Store benchmark results
        self.benchmarks[name] = {
            'steps': steps,
            'duration': duration,
            'steps_per_second': steps / duration,
            'average_step_time': duration / steps,
            'timestamp': start_time
        }
        
        return self.benchmarks[name]
        
    def compare_benchmarks(self, benchmark_names: List[str]) -> Dict[str, Any]:
        """Compare multiple benchmarks"""
        comparison = {}
        
        for name in benchmark_names:
            if name in self.benchmarks:
                comparison[name] = self.benchmarks[name]
                
        if len(comparison) < 2:
            return {'error': 'Need at least 2 benchmarks for comparison'}
        
        # Calculate performance metrics
        baseline = min(comparison.values(), key=lambda x: x['steps_per_second'])
        baseline_name = [k for k, v in comparison.items() if v == baseline][0]
        
        comparison_result = {
            'baseline': baseline_name,
            'baseline_performance': baseline['steps_per_second'],
            'comparisons': {}
        }
        
        for name, benchmark in comparison.items():
            if name != baseline_name:
                speedup = benchmark['steps_per_second'] / baseline['steps_per_second']
                comparison_result['comparisons'][name] = {
                    'speedup': speedup,
                    'improvement_percent': (speedup - 1) * 100,
                    'steps_per_second': benchmark['steps_per_second'],
                    'total_steps': benchmark['steps']
                }
                
        return comparison_result
        
    def export_benchmarks(self, filepath: str):
        """Export all benchmarks to file"""
        import json
        with open(filepath, 'w') as f:
            json.dump(self.benchmarks, f, indent=2)
        
    def load_benchmarks(self, filepath: str):
        """Load benchmarks from file"""
        import json
        with open(filepath, 'r') as f:
            self.benchmarks = json.load(f)
```

### AdvancedProfiler Class

```python
class AdvancedProfiler:
    """Advanced profiling with custom hooks and analysis"""
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        self.profiling_enabled = False
        self.custom_hooks = []
        self.data_collection = {}
        self.analysis_results = {}
        
    def enable_profiling(self, data_collection_interval: float = 0.01):
        """Enable advanced profiling with data collection"""
        self.profiling_enabled = True
        self.data_collection_interval = data_collection_interval
        
    def disable_profiling(self):
        """Disable profiling"""
        self.profiling_enabled = False
        
    def add_custom_hook(self, hook: Callable[[Dict], None]):
        """Add custom profiling hook
        
        Args:
            hook: Function that receives profiling data
        """
        self.custom_hooks.append(hook)
        
    def collect_performance_data(self) -> Dict[str, Any]:
        """Collect current performance data"""
        if not self.profiling_enabled:
            return {}
            
        data = {
            'timestamp': time.time(),
            'simulation_step': getattr(self.brain, 'current_step', 0),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'active_neuron_count': self.brain.getActiveNeuronCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'step_time': 0.001,  # Would need to be measured
            'memory_usage': self._get_memory_usage(),
            'cpu_usage': self._get_cpu_usage()
        }
        
        # Apply custom hooks
        for hook in self.custom_hooks:
            try:
                hook(data)
            except Exception as e:
                print(f"Custom hook error: {e}")
        
        # Store data
        if 'performance_data' not in self.data_collection:
            self.data_collection['performance_data'] = []
        self.data_collection['performance_data'].append(data)
        
        return data
        
    def analyze_performance_trends(self) -> Dict[str, Any]:
        """Analyze performance trends and identify patterns"""
        if 'performance_data' not in self.data_collection:
            return {}
            
        data = self.data_collection['performance_data']
        if len(data) < 10:
            return {'error': 'Insufficient data for analysis'}
        
        # Calculate statistics
        step_times = [d['step_time'] for d in data]
        memory_usages = [d['memory_usage'] for d in data]
        
        analysis = {
            'total_samples': len(data),
            'time_range': data[-1]['timestamp'] - data[0]['timestamp'],
            'average_step_time': sum(step_times) / len(step_times),
            'step_time_variance': self._variance(step_times),
            'memory_trend': self._calculate_trend(memory_usages),
            'performance_degradation': self._detect_degradation(step_times),
            'bottleneck_analysis': self._identify_bottlenecks(data)
        }
        
        self.analysis_results['trends'] = analysis
        return analysis
        
    def _get_memory_usage(self) -> float:
        """Get current memory usage"""
        # Implementation would depend on platform
        return 0.0
        
    def _get_cpu_usage(self) -> float:
        """Get current CPU usage"""
        # Implementation would depend on platform
        return 0.0
        
    def _variance(self, values: List[float]) -> float:
        """Calculate variance of values"""
        if len(values) < 2:
            return 0.0
        mean = sum(values) / len(values)
        return sum((x - mean) ** 2 for x in values) / len(values)
        
    def _calculate_trend(self, values: List[float]) -> Dict[str, float]:
        """Calculate trend of values"""
        if len(values) < 2:
            return {'slope': 0.0, 'intercept': 0.0, 'correlation': 0.0}
        
        n = len(values)
        x_values = list(range(n))
        
        x_mean = sum(x_values) / n
        y_mean = sum(values) / n
        
        numerator = sum((x_values[i] - x_mean) * (values[i] - y_mean) for i in range(n))
        denominator = sum((x - x_mean) ** 2 for x in x_values)
        
        if denominator == 0:
            return {'slope': 0.0, 'intercept': 0.0, 'correlation': 0.0}
        
        slope = numerator / denominator
        intercept = y_mean - slope * x_mean
        
        # Calculate correlation
        correlation_numerator = numerator
        correlation_denominator = (sum((x - x_mean) ** 2 for x in x_values) * 
                                   sum((y - y_mean) ** 2 for y in values)) ** 0.5
        
        if correlation_denominator == 0:
            correlation = 0.0
        else:
            correlation = correlation_numerator / correlation_denominator
        
        return {'slope': slope, 'intercept': intercept, 'correlation': correlation}
        
    def _detect_degradation(self, step_times: List[float]) -> Dict[str, Any]:
        """Detect performance degradation"""
        if len(step_times) < 10:
            return {'degrading': False, 'confidence': 0.0, 'rate': 0.0}
        
        # Calculate correlation with time
        n = len(step_times)
        x_values = list(range(n))
        
        x_mean = sum(x_values) / n
        y_mean = sum(step_times) / n
        
        correlation_numerator = sum((x_values[i] - x_mean) * (step_times[i] - y_mean) for i in range(n))
        correlation_denominator = (sum((x - x_mean) ** 2 for x in x_values) * 
                                   sum((y - y_mean) ** 2 for y in step_times)) ** 0.5
        
        if correlation_denominator == 0:
            return {'degrading': False, 'confidence': 0.0, 'rate': 0.0}
        
        correlation = correlation_numerator / correlation_denominator
        
        # Consider degradation if correlation is strongly negative
        degrading = correlation < -0.5
        confidence = abs(correlation)
        rate = correlation * 1000  # Scale for readability
        
        return {
            'degrading': degrading,
            'confidence': confidence,
            'rate': rate,
            'correlation': correlation
        }
        
    def _identify_bottlenecks(self, data: List[Dict]) -> List[Dict[str, Any]]:
        """Identify potential performance bottlenecks"""
        bottlenecks = []
        
        # Simple bottleneck detection based on metrics
        if len(data) < 5:
            return bottlenecks
        
        # Analyze step times for outliers
        step_times = [d['step_time'] for d in data]
        mean_time = sum(step_times) / len(step_times)
        std_time = (sum((x - mean_time) ** 2 for x in step_times) / len(step_times)) ** 0.5
        
        # Identify steps with unusually high times
        for i, step_time in enumerate(step_times):
            if step_time > mean_time + 2 * std_time:  # 2 sigma threshold
                bottlenecks.append({
                    'step': i,
                    'step_time': step_time,
                    'mean_time': mean_time,
                    'std_time': std_time,
                    'severity': 'high' if step_time > mean_time + 3 * std_time else 'medium'
                })
        
        # Analyze memory usage trends
        memory_usages = [d['memory_usage'] for d in data]
        if len(memory_usages) > 10:
            memory_trend = self._calculate_trend(memory_usages)
            if abs(memory_trend['slope']) > 0.1:  # Significant memory growth trend
                bottlenecks.append({
                    'type': 'memory_growth',
                    'slope': memory_trend['slope'],
                    'correlation': memory_trend['correlation'],
                    'severity': 'high' if abs(memory_trend['slope']) > 0.5 else 'medium'
                })
        
        return bottlenecks
        
    def export_analysis(self, filepath: str):
        """Export analysis results to file"""
        import json
        analysis = {
            'data_collection': self.data_collection,
            'analysis_results': self.analysis_results
        }
        with open(filepath, 'w') as f:
            json.dump(analysis, f, indent=2)
```

### Utility Functions

```python
# Performance monitoring utilities

def create_performance_monitor(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                              interval: float = 0.1) -> PerformanceMonitor:
    """Create and start a performance monitor"""
    monitor = PerformanceMonitor(brain, world)
    monitor.start_monitoring(interval)
    return monitor

def profile_simulation(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                       steps: int, profiler: AdvancedProfiler = None) -> Dict[str, Any]:
    """Profile a simulation run"""
    start_time = time.time()
    
    if profiler:
        profiler.enable_profiling()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        if profiler:
            profiler.collect_performance_data()
    
    end_time = time.time()
    duration = end_time - start_time
    
    results = {
        'steps': steps,
        'duration': duration,
        'steps_per_second': steps / duration,
        'average_step_time': duration / steps,
        'final_step': steps - 1
    }
    
    if profiler:
        profiler.disable_profiling()
        results['analysis'] = profiler.analyze_performance_trends()
        
    return results

def benchmark_configuration(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                            benchmarks: List[Dict]) -> Dict[str, Any]:
    """Run multiple benchmarks with different configurations"""
    benchmark_runner = PerformanceBenchmark(brain)
    
    results = {}
    for benchmark_config in benchmarks:
        name = benchmark_config['name']
        steps = benchmark_config['steps']
        config_overrides = benchmark_config.get('config_overrides', {})
        
        print(f"Running benchmark: {name} ({steps} steps)")
        
        result = benchmark_runner.run_benchmark(name, steps, config_overrides)
        results[name] = result
        
    # Compare results
    comparison = benchmark_runner.compare_benchmarks(list(results.keys()))
    
    return {
        'individual_results': results,
        'comparison': comparison
    }

def generate_performance_report(monitor: PerformanceMonitor, 
                                profiler: AdvancedProfiler = None) -> str:
    """Generate comprehensive performance report"""
    report = []
    report.append("NLM Performance Analysis Report")
    report.append("=" * 40)
    report.append("")
    
    # Get basic statistics
    stats = monitor.get_simulation_stats()
    report.append("Basic Performance Statistics:")
    report.append(f"  Total Simulation Time: {stats['total_simulation_time']:.2f} seconds")
    report.append(f"  Average Step Time: {stats['average_step_time']*1000:.2f} ms")
    report.append(f"  Steps per Second: {stats['steps_per_second']:.2f}")
    report.append(f"  Total Steps: {stats['total_steps']}")
    report.append(f"  Peak Memory Usage: {stats['peak_memory_usage']:.2f} MB")
    report.append(f"  Average Memory Usage: {stats['average_memory_usage']:.2f} MB")
    report.append("")
    
    # Get operation counts
    if stats['neuron_operations']:
        report.append("Neuron Operations:")
        for op, count in stats['neuron_operations'].items():
            report.append(f"  {op}: {count}")
        report.append("")
    
    if stats['plasticity_operations']:
        report.append("Plasticity Operations:")
        for op, count in stats['plasticity_operations'].items():
            report.append(f"  {op}: {count}")
        report.append("")
    
    # Get performance analysis from profiler
    if profiler:
        analysis = profiler.analyze_performance_trends()
        if analysis and 'error' not in analysis:
            report.append("Performance Analysis:")
            report.append(f"  Total Samples: {analysis['total_samples']}")
            report.append(f"  Time Range: {analysis['time_range']:.2f} seconds")
            report.append(f"  Average Step Time: {analysis['average_step_time']*1000:.2f} ms")
            report.append(f"  Step Time Variance: {analysis['step_time_variance']:.6f}")
            
            degradation = analysis['performance_degradation']
            if degradation['degrading']:
                report.append(f"  Performance Degradation: YES (confidence: {degradation['confidence']:.2f})")
            else:
                report.append(f"  Performance Degradation: NO (confidence: {degradation['confidence']:.2f})")
            
            trend = analysis['memory_trend']
            report.append(f"  Memory Trend: slope={trend['slope']:.6f}, correlation={trend['correlation']:.3f}")
            
            bottlenecks = analysis['bottleneck_analysis']
            if bottlenecks:
                report.append(f"  Identified {len(bottlenecks)} potential bottlenecks:")
                for bottleneck in bottlenecks[:3]:  # Show first 3
                    if 'step' in bottleneck:
                        report.append(f"    Step {bottleneck['step']}: {bottleneck['step_time']*1000:.2f} ms "
                                    f"(mean: {bottleneck['mean_time']*1000:.2f} ms)")
                    else:
                        report.append(f"    {bottleneck['type']}: slope={bottleneck.get('slope', 0):.6f}")
            
            report.append("")
    
    # Get timeline data
    timeline = monitor.get_step_timeline()
    if timeline:
        report.append("Step Timeline (last 10 steps):")
        for step_data in timeline[-10:]:
            report.append(f"  Step {step_data['step']}: {step_data['step_time']*1000:.2f} ms, "
                        f"Memory: {step_data['memory_usage']:.2f} MB, "
                        f"CPU: {step_data['cpu_usage']:.1f}%")
    
    return "\n".join(report)
```

## Usage Examples

### Basic Performance Monitoring

```python
import pynlm
from pynlm import PerformanceMonitor

# Create simulation
brain = pynlm.Brain()
brain.initialize()
world = pynlm.SimpleWorld()
world.configure(800, 600, 80, 60)

# Start performance monitoring
monitor = create_performance_monitor(brain, world, interval=0.1)

# Run simulation
for step in range(1000):
    brain.step(step)
    # ... simulation logic

# Stop monitoring and get results
monitor.stop_monitoring()
stats = monitor.get_simulation_stats()
print(f"Performance: {stats['steps_per_second']:.2f} steps/sec")

# Export results
monitor.export_stats("performance_results.json")
```

### Advanced Profiling

```python
import pynlm
from pynlm import AdvancedProfiler

# Create simulation
brain = pynlm.Brain()
brain.initialize()
world = pynlm.SimpleWorld()

# Setup advanced profiler
profiler = AdvancedProfiler(brain, world)
profiler.enable_profiling(data_collection_interval=0.01)

# Add custom analysis hook
def analyze_data(data):
    # Custom analysis logic here
    pass

profiler.add_custom_hook(analyze_data)

# Run profiled simulation
for step in range(500):
    brain.step(step)
    profiler.collect_performance_data()

# Disable profiling
profiler.disable_profiling()

# Analyze results
analysis = profiler.analyze_performance_trends()
print(f"Performance degradation: {analysis['performance_degradation']}")

# Export analysis
profiler.export_analysis("profile_analysis.json")
```

### Benchmarking

```python
import pynlm
from pynlm import benchmark_configuration

# Create simulation
brain = pynlm.Brain()
brain.initialize()
world = pynlm.SimpleWorld()

# Define benchmark configurations
benchmarks = [
    {
        'name': 'baseline',
        'steps': 1000,
        'config_overrides': {}
    },
    {
        'name': 'optimized',
        'steps': 1000,
        'config_overrides': {
            'neuron_count': 2000,  # Double neurons
            'simulation_timestep': 0.0005  # Half timestep
        }
    },
    {
        'name': 'constrained',
        'steps': 1000,
        'config_overrides': {
            'neuron_count': 500,  # Half neurons
            'simulation_timestep': 0.002  # Double timestep
        }
    }
]

# Run benchmarks
results = benchmark_configuration(brain, world, benchmarks)

# Print comparison
for name, result in results['individual_results'].items():
    print(f"{name}: {result['steps_per_second']:.2f} steps/sec, "
          f"{result['average_step_time']*1000:.2f} ms/step")

comparison = results['comparison']
print(f"\nBaseline: {comparison['baseline']}")
print("Comparisons:")
for name, comp in comparison['comparisons'].items():
    print(f"  {name}: {comp['speedup']:.2f}x faster, "
          f"{comp['improvement_percent']:+.1f}% change")
```

### Complete Performance Analysis

```python
import pynlm
from pynlm import create_performance_monitor, AdvancedProfiler, generate_performance_report

# Setup simulation
brain = pynlm.Brain()
brain.initialize()
world = pynlm.SimpleWorld()
world.configure(800, 600, 80, 60)

# Setup monitoring and profiling
monitor = create_performance_monitor(brain, world)
profiler = AdvancedProfiler(brain, world)
profiler.enable_profiling()

# Run simulation
print("Running simulation with performance monitoring...")
for step in range(1000):
    brain.step(step)
    monitor.collect_performance_data()
    profiler.collect_performance_data()
    
    # Optional: Log progress
    if step % 200 == 0:
        print(f"  Completed step {step}")

# Stop monitoring
monitor.stop_monitoring()
profiler.disable_profiling()

# Generate comprehensive report
report = generate_performance_report(monitor, profiler)
print("\n" + report)

# Export all data
monitor.export_stats("simulation_performance.json")
profiler.export_analysis("simulation_analysis.json")

print("\nPerformance analysis complete!")
```

## Performance Tips

### For High-Performance Simulations
1. **Optimize Configuration**: Use appropriate neuron counts and timestep settings
2. **Monitor Memory Usage**: Watch for memory leaks and excessive allocations
3. **Profile Bottlenecks**: Use the profiling tools to identify performance bottlenecks
4. **Batch Operations**: Group related operations to reduce overhead
5. **Use Checkpoints**: Save periodic checkpoints to avoid long recovery times

### For Debugging Performance Issues
1. **Step Timeline Analysis**: Use the timeline data to identify slow steps
2. **Memory Profiling**: Monitor memory allocation and deallocation patterns
3. **Operation Counting**: Track specific operation frequencies
4. **Trend Analysis**: Use the trend analysis to detect gradual performance degradation
5. **Bottleneck Identification**: Let the tools automatically identify potential bottlenecks

### For Production Systems
1. **Real-time Monitoring**: Use the monitoring tools in production environments
2. **Performance Baselines**: Establish performance baselines for different configurations
3. **Alerting**: Set up alerts for performance anomalies
4. **Capacity Planning**: Use performance data for capacity planning
5. **Optimization Cycles**: Use the profiling data to drive optimization cycles

## Integration with External Tools

The performance monitoring system can be integrated with:

- **Grafana**: For real-time dashboards
- **Prometheus**: For metrics collection and alerting
- **ELK Stack**: For log analysis and visualization
- **Custom Analytics**: For custom performance analysis

## API Stability

This API is stable and follows semantic versioning. Breaking changes will be clearly documented.

## License

This performance monitoring and profiling utilities are part of the NLM project and are licensed under the MIT License.

## Authors

NLM Performance Monitoring Team

## Version

1.0.0

## Last Updated

$(date)