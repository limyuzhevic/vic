# Performance Monitoring and Profiling Utilities

This document provides comprehensive information about NLM's performance monitoring and profiling capabilities, integrating the existing C++ performance infrastructure with new Python utilities for enhanced user experience.

## Overview

NLM provides a multi-layered performance monitoring system:

1. **C++ Core Infrastructure** (Phase 5): Event-driven processing, memory pools, sparse connectivity, parallel processing, SIMD vectorization
2. **Python Bindings**: Performance monitoring utilities with real-time statistics
3. **Benchmarking Framework**: Standardized performance measurement
4. **Profiling Tools**: Detailed profiling of simulation performance

## Python Performance Monitoring

### PerformanceMonitor Class

The `PerformanceMonitor` class provides comprehensive performance tracking for NLM simulations:

```python
import pynlm

# Create performance monitor
monitor = pynlm.PerformanceMonitor()

# Run simulation with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    # Simulate work
    monitor.recordStep(0.001)  # Record time step

# Get performance report
report = monitor.get_report()
print(report)
```

**Key Features:**

- **Real-time tracking**: Monitor step-by-step performance
- **Statistics calculation**: Average time per step, total steps, total time
- **Memory usage monitoring**: Track peak memory consumption
- **Progress reporting**: Get detailed performance reports

### Enhanced Batch Processing

The `BatchProcessor` class provides efficient batch processing for large-scale simulations:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Process multiple steps efficiently
steps_completed = brain.run_steps(10000)
print(f"Completed {steps_completed} steps")

# Run with progress callback
def progress_callback(step, progress):
    if progress % 0.1 == 0:
        print(f"Progress: {progress*100:.1f}%")

brain.run_steps_with_callback(50000, progress_callback)
```

**Key Features:**

- **Efficient batch processing**: Minimize overhead for large step counts
- **Progress monitoring**: Real-time progress tracking with callbacks
- **Non-blocking operations**: Keep Python responsive during large simulations
- **Error handling**: Robust error handling with detailed error messages

## Benchmarking and Profiling

### Benchmarking Utilities

NLM provides comprehensive benchmarking capabilities:

```python
import pynlm
import time

def benchmark_simulation(neuron_count=1000, steps=1000):
    """Benchmark a simulation with specified parameters"""
    print(f"Benchmarking: {neuron_count} neurons, {steps} steps")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    
    start_time = time.time()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    monitor = pynlm.PerformanceMonitor()
    
    for step in range(steps):
        brain.step(step)
        monitor.record_step(0.001)
    
    end_time = time.time()
    duration = end_time - start_time
    
    report = monitor.get_report()
    print(f"Benchmark completed in {duration:.2f} seconds")
    print(report)
    
    return {
        'duration': duration,
        'steps_per_second': steps / duration,
        'avg_time_per_step': duration / steps,
        'report': report
    }

# Run benchmarks
results = benchmark_simulation(neuron_count=1000, steps=1000)
```

### Profiling Utilities

Advanced profiling for detailed performance analysis:

```python
import pynlm

class ProfileAnalyzer:
    def __init__(self):
        self.profiling_data = []
    
    def profile_step(self, brain, step, timestep):
        """Profile a single step with detailed metrics"""
        step_data = {
            'step': step,
            'time': timestep,
            'neuron_count': brain.get_total_neuron_count(),
            'synapse_count': brain.get_total_synapse_count(),
            'spike_count': brain.get_total_spike_count(),
            'firing_neurons': brain.get_firing_neuron_count(),
            'e_i_ratio': brain.get_excitation_inhibition_ratio(),
            'avg_firing_rate': brain.get_average_firing_rate(),
            'pending_events': brain.get_pending_spike_event_count(),
            'memory_usage': self._get_memory_usage(),
            'cpu_time': self._get_cpu_time()
        }
        self.profiling_data.append(step_data)
    
    def analyze_performance(self):
        """Analyze profiling data and return insights"""
        if not self.profiling_data:
            return None
        
        analysis = {
            'total_steps': len(self.profiling_data),
            'avg_neurons': sum(d['neuron_count'] for d in self.profiling_data) / len(self.profiling_data),
            'total_spikes': sum(d['spike_count'] for d in self.profiling_data),
            'avg_spikes_per_step': sum(d['spike_count'] for d in self.profiling_data) / len(self.profiling_data),
            'max_firing_rate': max(d['avg_firing_rate'] for d in self.profiling_data),
            'min_firing_rate': min(d['avg_firing_rate'] for d in self.profiling_data),
            'stability': self._calculate_stability(),
            'bottlenecks': self._identify_bottlenecks()
        }
        
        return analysis
    
    def export_profile(self, filename):
        """Export profiling data to file"""
        import json
        with open(filename, 'w') as f:
            json.dump(self.profiling_data, f, indent=2)
    
    def import_profile(self, filename):
        """Import profiling data from file"""
        import json
        with open(filename, 'r') as f:
            self.profiling_data = json.load(f)

# Usage example
analyzer = ProfileAnalyzer()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    analyzer.profile_step(brain, step, step * 0.001)
    brain.step(step)

analysis = analyzer.analyze_performance()
print("Performance Analysis:", analysis)

# Export for later analysis
analyzer.export_profile("profile.json")
```

## Advanced Performance Features

### Context Manager for Resource Management

```python
import pynlm

# Using context manager for automatic resource cleanup
with pynlm.create_brain(pynlm.create_default_config()) as brain:
    brain.initialize()
    # Automatic cleanup when exiting the context
    for step in range(100):
        brain.step(step)
```

### Configuration-based Performance Tuning

```python
import pynlm

class PerformanceConfig:
    def __init__(self, base_config):
        self.config = base_config
        self.setup_performance_optimizations()
    
    def setup_performance_optimizations(self):
        """Setup optimizations based on available hardware"""
        # Check CPU features
        import os
        if hasattr(os, 'sysconf'):
            # Linux
            cpu_count = os.sysconf("SC_NPROCESSORS_ONLN")
        else:
            # macOS/Windows fallback
            cpu_count = os.cpu_count() or 1
        
        # Optimize for available cores
        if cpu_count >= 8:
            self.config.set("parallel_threads", cpu_count)
            self.config.set("vectorization_level", 3)  # AVX-512
        elif cpu_count >= 4:
            self.config.set("parallel_threads", cpu_count)
            self.config.set("vectorization_level", 2)  # AVX2
        else:
            self.config.set("parallel_threads", 1)
            self.config.set("vectorization_level", 1)  # SSE
    
    def apply(self, brain):
        """Apply configuration to brain"""
        brain.get_config().loadFromArgs()

# Usage
config = PerformanceConfig(pynlm.create_default_config())
brain = config.apply(pynlm.create_brain(config.config))
```

## Performance Monitoring Integration

### Real-time Monitoring

Integrate performance monitoring with simulation progress:

```python
import pynlm
import threading
import time

class RealTimeMonitor:
    def __init__(self, brain, update_interval=1.0):
        self.brain = brain
        self.update_interval = update_interval
        self.running = False
        self.monitor_thread = None
        self.start_time = None
        self.last_step_time = None
    
    def start(self):
        """Start real-time monitoring"""
        self.running = True
        self.start_time = time.time()
        self.last_step_time = time.time()
        
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop(self):
        """Stop real-time monitoring"""
        self.running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitor_loop(self):
        """Main monitoring loop"""
        while self.running:
            try:
                current_time = time.time()
                if self.start_time:
                    elapsed = current_time - self.start_time
                    if elapsed > 0:
                        steps_per_second = self.brain.get_total_spike_count() / elapsed
                        print(f"\r[{elapsed:.1f}s] Spikes/sec: {steps_per_second:.0f} "
                              f"| Neurons firing: {self.brain.get_firing_neuron_count()} "
                              f"| E/I ratio: {self.brain.get_excitation_inhibition_ratio():.2f}", end="")
                
                time.sleep(self.update_interval)
            except Exception as e:
                print(f"\nMonitoring error: {e}")
                break

# Usage
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

monitor = RealTimeMonitor(brain, update_interval=0.5)
monitor.start()

# Run simulation
for step in range(1000):
    brain.step(step)

monitor.stop()
print("\nMonitoring stopped")
```

## Performance Comparison and Optimization

### Before/After Comparison

```python
import pynlm

def compare_performance(config1, config2, steps=1000):
    """Compare performance between two configurations"""
    results = {}
    
    # Test first configuration
    print("Testing Configuration 1...")
    brain1 = pynlm.create_brain(config1)
    brain1.initialize()
    
    import time
    start_time = time.time()
    for step in range(steps):
        brain1.step(step)
    duration1 = time.time() - start_time
    
    results['config1'] = {
        'duration': duration1,
        'steps_per_second': steps / duration1,
        'avg_time_per_step': duration1 / steps
    }
    
    # Test second configuration
    print("Testing Configuration 2...")
    brain2 = pynlm.create_brain(config2)
    brain2.initialize()
    
    start_time = time.time()
    for step in range(steps):
        brain2.step(step)
    duration2 = time.time() - start_time
    
    results['config2'] = {
        'duration': duration2,
        'steps_per_second': steps / duration2,
        'avg_time_per_step': duration2 / steps
    }
    
    # Calculate improvement
    improvement = (duration1 - duration2) / duration1 * 100
    results['improvement_percent'] = improvement
    results['speedup_factor'] = duration1 / duration2
    
    print(f"\nPerformance Comparison:")
    print(f"Config 1: {results['config1']['steps_per_second']:.0f} steps/sec")
    print(f"Config 2: {results['config2']['steps_per_second']:.0f} steps/sec")
    print(f"Improvement: {improvement:.1f}% faster")
    print(f"Speedup factor: {results['speedup_factor']:.2f}x")
    
    return results

# Usage
config1 = pynlm.create_default_config()
config1.set("parallel_threads", 1)

config2 = pynlm.create_default_config()
config2.set("parallel_threads", 4)

results = compare_performance(config1, config2, steps=1000)
```

## Integration with Existing Performance Infrastructure

### Accessing C++ Performance Data

NLM's C++ performance infrastructure provides detailed metrics that can be accessed through Python:

```python
import pynlm

# Create brain with performance monitoring
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

# Access performance metrics
print("Performance Metrics:")
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Pending spike events: {brain.get_pending_spike_event_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.3f}")
print(f"Excitation/Inhibition ratio: {brain.get_excitation_inhibition_ratio():.3f}")

# Performance monitor integration
monitor = pynlm.PerformanceMonitor()
for step in range(100):
    brain.step(step)
    monitor.record_step(0.001)

print("\nPerformance Report:")
print(monitor.get_report())
```

## Best Practices for Performance Optimization

### 1. Profile Before Optimizing

Always profile your application before making performance optimizations:

```python
def profile_and_optimize(brain, num_steps):
    """Profile simulation and suggest optimizations"""
    import time
    start_time = time.time()
    
    # Profile
    for step in range(num_steps):
        brain.step(step)
    
    duration = time.time() - start_time
    
    # Analyze performance
    analysis = {
        'steps_per_second': num_steps / duration,
        'avg_time_per_step': duration / num_steps,
        'cpu_usage': 100.0 * duration / (time.time() - start_time),
        'memory_efficiency': brain.get_total_synapse_count() / brain.get_total_neuron_count()
    }
    
    # Suggest optimizations
    suggestions = []
    if analysis['avg_time_per_step'] > 0.001:
        suggestions.append("Consider reducing neuron count or increasing timesteps")
    if analysis['cpu_usage'] > 80:
        suggestions.append("CPU usage is high - consider optimizing algorithms")
    if analysis['memory_efficiency'] > 0.1:
        suggestions.append("Memory usage may be inefficient - consider sparse connectivity")
    
    return analysis, suggestions
```

### 2. Use Batch Processing for Large Simulations

For large-scale simulations, use batch processing:

```python
def large_simulation(neuron_count=10000, total_steps=100000):
    """Run large simulation with batch processing"""
    config = pynlm.create_default_config()
    config.set("neuron_count", neuron_count)
    config.set("parallel_threads", 4)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run in batches to monitor progress
    batch_size = 10000
    total_batches = (total_steps + batch_size - 1) // batch_size
    
    for batch in range(total_batches):
        start_step = batch * batch_size
        end_step = min(start_step + batch_size, total_steps)
        
        print(f"Running batch {batch + 1}/{total_batches} "
              f"({start_step}-{end_step} steps)")
        
        for step in range(start_step, end_step):
            brain.step(step)
        
        # Monitor progress
        progress = (end_step / total_steps) * 100
        print(f"Progress: {progress:.1f}%")
    
    return brain
```

### 3. Monitor Memory Usage

Monitor memory usage during long simulations:

```python
import pynlm
import psutil
import os

class MemoryMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.process = psutil.Process(os.getpid())
        self.memory_samples = []
    
    def start_monitoring(self, interval=1.0):
        """Start memory monitoring in background thread"""
        import threading
        self.monitoring = True
        
        def monitor_loop():
            while self.monitoring:
                memory_info = self.process.memory_info()
                self.memory_samples.append({
                    'timestamp': time.time(),
                    'rss': memory_info.rss / 1024 / 1024,  # MB
                    'vms': memory_info.vms / 1024 / 1024,  # MB
                    'neurons': self.brain.get_total_neuron_count(),
                    'synapses': self.brain.get_total_synapse_count()
                })
                time.sleep(interval)
        
        self.monitor_thread = threading.Thread(target=monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop memory monitoring"""
        self.monitoring = False
        if hasattr(self, 'monitor_thread'):
            self.monitor_thread.join(timeout=1.0)
    
    def get_memory_report(self):
        """Generate memory usage report"""
        if not self.memory_samples:
            return "No memory samples collected"
        
        report = "Memory Usage Report\n"
        report += "===================\n"
        
        for sample in self.memory_samples[-10:]:  # Last 10 samples
            report += f"Time: {sample['timestamp'] - self.memory_samples[0]['timestamp']:.1f}s "
                     f"| RSS: {sample['rss']:.1f} MB | VMS: {sample['vms']:.1f} MB "
                     f"| Neurons: {sample['neurons']} | Synapses: {sample['synapses']}\n"
        
        return report

# Usage example
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

memory_monitor = MemoryMonitor(brain)
memory_monitor.start_monitoring(interval=0.5)

for step in range(100):
    brain.step(step)

memory_monitor.stop_monitoring()
print(memory_monitor.get_memory_report())
```

## Summary

NLM's performance monitoring and profiling utilities provide comprehensive tools for:

1. **Real-time performance monitoring** with detailed statistics
2. **Efficient batch processing** for large-scale simulations
3. **Comprehensive benchmarking** with standardized metrics
4. **Advanced profiling** with detailed analysis capabilities
5. **Memory monitoring** for long-running simulations
6. **Integration with C++ performance infrastructure** for maximum insight

These utilities enable users to optimize their simulations, identify performance bottlenecks, and achieve the best possible performance from their NLM installations.