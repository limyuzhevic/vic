# Performance Monitoring Tools for NLM

## Overview

Performance monitoring tools for the NLM (Neural Learning Machine) simulation framework. This package provides comprehensive profiling capabilities to analyze and optimize neural network simulation performance.

## Key Features

### 1. Performance Monitor Class
- Real-time performance measurement
- Per-step timing and statistics
- Memory usage tracking
- Hardware performance counters (when available)

### 2. Performance Profiler Class
- Detailed profiling of simulation components
- Call stack analysis
- Resource consumption tracking
- Comparative benchmarking

### 3. Simulation Performance Analyzer
- Bottleneck identification
- Scalability analysis
- Resource efficiency metrics
- Optimization recommendations

### 4. Hardware-Specific Optimizations
- CPU performance monitoring
- Memory bandwidth analysis
- Cache hit/miss statistics
- SIMD instruction efficiency

### 5. Integration with Existing NLM Components
- Seamless integration with Brain class
- Performance monitoring of plasticity rules
- Spike system profiling
- Memory system monitoring

## Usage Examples

### Basic Performance Monitoring
```python
import pynlm

# Create performance monitor
monitor = pynlm.PerformanceMonitor()

# Create and initialize brain
brain = pynlm.createBrainWithConfig(neuron_count=1000)
brain.initialize()

# Start monitoring
monitor.start_monitoring()

# Run simulation
for step in range(1000):
    brain.step(step)

# Stop monitoring and get metrics
monitor.stop_monitoring()
metrics = monitor.get_metrics()

print(f"Average step time: {metrics['avg_step_time']:.6f} s")
print(f"Steps per second: {metrics['steps_per_second']:.0f}")
print(f"Memory usage: {metrics['memory_usage_mb']:.2f} MB")
```

### Detailed Performance Profiling
```python
import pynlm

# Create profiler
profiler = pynlm.PerformanceProfiler()

# Profile plasticity operations
profiler.start_profile("plasticity")
for step in range(500):
    brain.step(step)
profiler.stop_profile("plasticity")

# Profile spike processing
profiler.start_profile("spike_system")
for step in range(500, 1000):
    brain.step(step)
profiler.stop_profile("spike_system")

# Get detailed profiling results
detailed_results = profiler.get_detailed_results()
print("Plasticity performance:", detailed_results['plasticity'])
print("Spike system performance:", detailed_results['spike_system'])
```

### Hardware Performance Analysis
```python
import pynlm

# Create analyzer for hardware-specific insights
analyzer = pynlm.SimulationPerformanceAnalyzer()

# Run simulation with hardware analysis
with pynlm.run_simulation_with_analysis(brain, 1000) as analysis:
    results = analysis.get_hardware_metrics()
    print("CPU efficiency: {:.1f}%".format(results['cpu_efficiency']))
    print("Memory bandwidth: {:.1f} GB/s".format(results['memory_bandwidth']))
    print("Cache hit rate: {:.1f}%".format(results['cache_hit_rate']))
```

### Performance Comparison
```python
import pynlm

# Compare different configurations
configs = [
    pynlm.createDefaultConfig(),
    pynlm.createOptimizedConfig(),
    pynlm.createMemoryEfficientConfig()
]

benchmark_results = {}
for config in configs:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    monitor = pynlm.PerformanceMonitor()
    monitor.start_monitoring()
    
    for step in range(1000):
        brain.step(step)
    
    monitor.stop_monitoring()
    benchmark_results[config.get('name')] = monitor.get_metrics()

# Display comparison
print("Performance Comparison:")
print(f"{'Config':<20} {'Steps/s':<10} {'Avg Step (ms)':<15} {'Memory (MB)':<12}")
print("-" * 60)
for config_name, metrics in benchmark_results.items():
    print(f"{config_name:<20} {metrics['steps_per_second']:<10.0f} "
          f"{metrics['avg_step_time']*1000:<15.3f} {metrics['memory_usage_mb']:<12.2f}")
```

## API Reference

### PerformanceMonitor

```python
class PerformanceMonitor:
    def start_monitoring(self)
    def stop_monitoring(self)
    def get_metrics(self) -> dict
    def reset(self)
    def is_monitoring(self) -> bool
    def get_hardware_info(self) -> dict
```

### PerformanceProfiler

```python
class PerformanceProfiler:
    def start_profile(self, profile_name: str)
    def stop_profile(self, profile_name: str)
    def get_profile_results(self, profile_name: str) -> dict
    def get_all_profile_results(self) -> dict
    def reset(self)
    def export_profile_data(self, filename: str)
    def import_profile_data(self, filename: str)
```

### SimulationPerformanceAnalyzer

```python
class SimulationPerformanceAnalyzer:
    def analyze_simulation(self, brain, steps: int) -> dict
    def identify_bottlenecks(self, brain, steps: int) -> list
    def get_optimization_recommendations(self, brain, steps: int) -> list
    def compare_performance(self, brain1, brain2, steps: int) -> dict
    def predict_scalability(self, brain, max_neurons: int) -> dict
```

## Advanced Features

### 1. Memory Profiling
```python
# Track memory usage over time
memory_profiler = pynlm.MemoryProfiler()
memory_profiler.start_tracking()

for step in range(1000):
    brain.step(step)
    mem_usage = memory_profiler.get_current_usage()
    print(f"Step {step}: {mem_usage} MB")

memory_profiler.stop_tracking()
```

### 2. Spike System Performance
```python
# Monitor spike processing efficiency
spike_monitor = pynlm.SpikeSystemMonitor()
spike_monitor.start_monitoring(brain)

for step in range(1000):
    brain.step(step)
    spike_stats = spike_monitor.get_spike_statistics()
    print(f"Spikes: {spike_stats['total_spikes']}, "
          f"Processing time: {spike_stats['avg_processing_time']:.6f}s")
```

### 3. Plasticity Performance
```python
# Profile plasticity computation
plasticity_profiler = pynlm.PlasticityPerformanceProfiler()
plasticity_profiler.start_profiling(brain)

for step in range(1000):
    brain.step(step)
    plasticity_stats = plasticity_profiler.get_performance_stats()
    print(f"Plasticity operations: {plasticity_stats['operations']}, "
          f"Time per operation: {plasticity_stats['time_per_op']:.6f}s")
```

## Configuration Options

### PerformanceMonitor Configuration
```python
config = {
    'sampling_interval': 0.001,  # seconds between samples
    'track_memory': True,        # Track memory usage
    'track_cpu': True,           # Track CPU usage
    'hardware_counters': True,   # Use hardware performance counters
    'detailed_timing': True      # Enable detailed timing measurements
}
```

### PerformanceProfiler Configuration
```python
config = {
    'profile_types': ['cpu', 'memory', 'io'],  # Types of profiling
    'sampling_rate': 1000,                   # Samples per second
    'include_children': True,                # Profile child processes
    'compress_data': True,                   # Compress profiling data
    'export_format': 'json'                   # Export format
}
```

## Integration with Existing Code

### Existing Performance Monitoring
The performance monitoring tools integrate seamlessly with existing NLM functionality:

```python
import pynlm

# Existing performance monitoring (from current documentation)
def existing_performance_monitoring():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    start_time = time.time()
    for step in range(1000):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    print(f"Total time: {end_time - start_time:.3f} seconds")
    print(f"Average step time: {(end_time - start_time)/1000:.6f} seconds")
```

### New Enhanced Performance Monitoring
```python
import pynlm

def enhanced_performance_monitoring():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create performance monitor
    monitor = pynlm.PerformanceMonitor()
    monitor.start_monitoring()
    
    # Run simulation with detailed monitoring
    for step in range(1000):
        monitor.start_step_timing()
        brain.step(step)
        monitor.end_step_timing()
        
        # Log performance every 100 steps
        if step % 100 == 0:
            metrics = monitor.get_step_metrics()
            print(f"Step {step}: {metrics['step_time_ms']:.3f}ms, "
                  f"{metrics['spikes_per_step']:.1f} spikes/step")
    
    # Get final performance report
    report = monitor.get_performance_report()
    print(report)
    
    monitor.stop_monitoring()
```

## File Structure

```
performance_tools/
├── __init__.py                    # Main package
├── performance_monitor.py         # Performance monitoring
├── performance_profiler.py        # Detailed profiling
├── simulation_analyzer.py         # Performance analysis
├── memory_profiler.py             # Memory usage tracking
├── spike_system_monitor.py        # Spike system monitoring
├── plasticity_profiler.py         # Plasticity performance
├── benchmark_runner.py            # Benchmarking utilities
├── report_generator.py            # Performance report generation
├── config_optimizer.py            # Performance configuration
└── utils.py                       # Utility functions
```

## Installation

```bash
pip install pynlm[performance]
```

Or for development:
```bash
pip install -e .
pip install numpy matplotlib psutil scipy
```

## Dependencies

- Python 3.8+
- NumPy
- Matplotlib (for visualization)
- Psutil (for system monitoring)
- SciPy (for statistical analysis)

## Supported Platforms

- Linux (x86_64, ARM)
- macOS
- Windows

## Performance Requirements

- Minimal overhead: < 1% performance impact when not profiling
- Accurate measurements: < 1ms timing resolution
- Scalable: Works with thousands to millions of neurons
- Platform independent: Works across different hardware architectures

## Limitations

- Hardware performance counters may not be available on all platforms
- Memory tracking accuracy varies by operating system
- Some advanced features require privileged access
- Profiling data storage may be large for long simulations

## Future Enhancements

- Real-time visualization of performance metrics
- Automated performance optimization
- Distributed performance monitoring
- AI-powered bottleneck detection
- Integration with profiling tools like `perf` and `valgrind`

## License

MIT License. See LICENSE file for details.