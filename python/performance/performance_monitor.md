# NLM Performance Monitoring System

This module provides comprehensive performance monitoring and optimization tools for the NLM neural simulation framework.

## Overview

The Performance Monitor provides real-time monitoring of NLM simulation performance, including:

- CPU and memory usage tracking
- Simulation speed metrics
- Neural activity analysis
- Resource allocation optimization
- Bottleneck identification
- Performance profiling

## Core Classes

### PerformanceMonitor
Primary performance monitoring class that tracks and analyzes NLM simulation performance.

```python
class PerformanceMonitor:
    def __init__(self, brain=None):
        """Initialize performance monitor with optional brain reference"""
    
    def record_step(self, brain):
        """Record a simulation step for performance analysis"""
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get comprehensive performance statistics"""
    
    def get_efficiency_metrics(self) -> Dict[str, Any]:
        """Get efficiency metrics and optimization recommendations"""
    
    def export_metrics(self, filepath: str):
        """Export performance metrics to file"""
```

### PerformanceProfiler
Detailed profiling tools for identifying performance bottlenecks and optimization opportunities.

```python
class PerformanceProfiler:
    def __init__(self, brain=None):
        """Initialize performance profiler"""
    
    def start_profiling(self, duration: float):
        """Start performance profiling"""
    
    def stop_profiling(self):
        """Stop performance profiling"""
    
    def get_profiling_results(self) -> Dict[str, Any]:
        """Get profiling results and analysis"""
    
    def identify_bottlenecks(self) -> List[str]:
        """Identify performance bottlenecks"""
```

### ResourceMonitor
System resource monitoring for memory, CPU, and I/O tracking.

```python
class ResourceMonitor:
    def __init__(self):
        """Initialize resource monitor"""
    
    def get_memory_usage(self) -> Dict[str, Any]:
        """Get current memory usage statistics"""
    
    def get_cpu_usage(self) -> Dict[str, Any]:
        """Get current CPU usage statistics"""
    
    def get_iow_usage(self) -> Dict[str, Any]:
        """Get I/O usage statistics"""
    
    def track_resource_usage(self, brain):
        """Track resource usage during simulation"""
```

## Usage Examples

### Basic Performance Monitoring

```python
import pynlm
from python.performance import PerformanceMonitor

# Create brain and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize performance monitor
monitor = PerformanceMonitor(brain)

# Run simulation with performance tracking
for step in range(1000):
    brain.step(step)
    monitor.record_step(brain)
    
    # Print performance stats every 100 steps
    if step % 100 == 0:
        stats = monitor.get_performance_stats()
        print(f"Step {step}: {stats}")

# Export performance metrics
monitor.export_metrics("performance_metrics.json")
```

### Advanced Profiling

```python
from python.performance import PerformanceProfiler

# Create profiler
profiler = PerformanceProfiler(brain)

# Start profiling
profiler.start_profiling(60.0)  # Profile for 60 seconds

# Run simulation with detailed profiling
for step in range(10000):
    brain.step(step)

# Stop profiling and analyze
profiler.stop_profiling()

# Get detailed analysis
results = profiler.get_profiling_results()
bottlenecks = profiler.identify_bottlenecks()

print("Performance Analysis:")
for key, value in results.items():
    print(f"  {key}: {value}")

print("\nBottlenecks:")
for bottleneck in bottlenecks:
    print(f"  - {bottleneck}")
```

### Resource Monitoring

```python
from python.performance import ResourceMonitor

# Initialize resource monitor
resource_monitor = ResourceMonitor()

# Run simulation with resource tracking
for step in range(1000):
    brain.step(step)
    resource_monitor.track_resource_usage(brain)

# Get resource usage statistics
memory_stats = resource_monitor.get_memory_usage()
cpu_stats = resource_monitor.get_cpu_usage()

print(f"Memory Usage: {memory_stats}")
print(f"CPU Usage: {cpu_stats}")
```

## Performance Metrics

The performance monitoring system tracks and analyzes the following metrics:

### Core Performance Metrics
- **Steps per second (SPS)**: Simulation speed
- **Spikes per second (SpS)**: Neural activity rate
- **Average firing rate**: Neural population activity
- **Excitatory/inhibitory ratio**: Network balance
- **Memory efficiency**: Resource utilization

### Resource Metrics
- **Memory usage**: Resident set size and heap allocation
- **CPU utilization**: Processor usage over time
- **I/O operations**: Read/write operations
- **Network usage**: If applicable

### Efficiency Metrics
- **Simulation efficiency**: Steps per unit time
- **Neural efficiency**: Spikes per neuron
- **Connectivity efficiency**: Synapse utilization
- **Memory efficiency**: Memory per neuron

## Optimization Recommendations

The performance monitoring system provides automated optimization recommendations based on collected data:

### Common Optimization Areas
1. **Memory Management**
   - Identify memory leaks
   - Suggest memory pool usage
   - Recommend object pooling strategies

2. **CPU Optimization**
   - Identify compute bottlenecks
   - Suggest algorithmic improvements
   - Recommend parallelization

3. **I/O Optimization**
   - Identify excessive disk usage
   - Suggest caching strategies
   - Recommend streaming for large datasets

### Performance Thresholds

The system tracks performance against established thresholds:

- **Optimal Performance**: SPS > 1000, SpS > 10000
- **Good Performance**: SPS > 500, SpS > 5000
- **Acceptable Performance**: SPS > 100, SpS > 1000
- **Needs Optimization**: SPS < 100, SpS < 1000

## Integration with Enhanced API

The performance monitoring system integrates seamlessly with the enhanced NLM API:

### EnhancedNLMWorld Integration

```python
from python.enhanced_api import EnhancedNLMWorld
from python.performance import PerformanceMonitor

# Create enhanced world with performance monitoring
world = EnhancedNLMWorld()
monitor = PerformanceMonitor(world.brain)

# Run experiment with automatic performance tracking
def callback(simulator, step):
    monitor.record_step(simulator.brain)

world.run_episode(1000, callback=callback)

# Get performance analysis
performance_stats = monitor.get_performance_stats()
efficiency_metrics = monitor.get_efficiency_metrics()

print(f"Experiment Performance: {performance_stats}")
print(f"Optimization Recommendations: {efficiency_metrics}")
```

### Performance-Optimized Simulation

```python
from python.performance import PerformanceProfiler

# Create performance-optimized configuration
from python.enhanced_api import NLMConfig

config = (NLMConfig()
    .set_brain_config(2000, 2)
    .set_performance_config(
        enable_memory_pools=True,
        enable_simd=True,
        thread_count=2
    )
    .set_plasticity_config(learning_rate=0.005))

# Create brain with performance optimizations
brain = pynlm.createBrain(config.config)
brain.initialize()

# Create profiler for performance analysis
profiler = PerformanceProfiler(brain)

# Run performance-optimized simulation
profiler.start_profiling(120.0)  # Profile for 2 minutes

for step in range(10000):
    brain.step(step)

profiler.stop_profiling()

# Get performance analysis
analysis = profiler.get_profiling_results()
recommendations = profiler.get_optimization_recommendations()

print("Performance Optimization Analysis Complete")
print(f"Performance Score: {analysis.get('performance_score', 'N/A')}")
print(f"Optimization Recommendations:")
for rec in recommendations:
    print(f"  - {rec}")
```

## Implementation Details

### Data Structures

The performance monitoring system uses the following data structures:

```python
# Performance metrics data structure
performance_metrics = {
    'steps_completed': 0,
    'elapsed_time_seconds': 0.0,
    'steps_per_second': 0.0,
    'spikes_per_second': 0.0,
    'neurons': 0,
    'synapses': 0,
    'total_spikes': 0,
    'max_firing_rate': 0.0,
    'memory_usage_mb': 0.0,
    'cpu_usage_percent': 0.0,
}

# Profiling data structure
profiling_data = {
    'start_time': 0.0,
    'end_time': 0.0,
    'step_times': [],
    'neuron_times': [],
    'synapse_times': [],
    'memory_snapshots': [],
    'cpu_snapshots': [],
}
```

### Configuration

Performance monitoring can be configured via the NLM configuration system:

```yaml
performance_monitoring:
  enable: true
  record_interval: 100
  profiling_duration: 300.0
  resource_monitoring: true
  auto_optimization: true

metrics:
  track_steps_per_second: true
  track_spikes_per_second: true
  track_memory_usage: true
  track_cpu_usage: true
  track_neural_efficiency: true
```

## Files and Directories

The performance monitoring system is organized in the following files:

```
python/
├── performance/
│   ├── __init__.py
│   ├── monitor.py          # Core performance monitoring
│   ├── profiler.py         # Detailed profiling tools
│   ├── resource_monitor.py  # System resource monitoring
│   ├── efficiency.py       # Efficiency analysis
│   └── exporter.py         # Metrics export utilities
│
├── enhanced_api.py          # Enhanced Python API
├── __init__.py              # Python module initialization
└── bindings.cpp            # Original C++ bindings
```

## Installation and Setup

### Prerequisites

- Python 3.8+
- NumPy (for performance metrics)
- psutil (for system resource monitoring)

### Installation

```bash
# Install with NLM
pip install -e .

# Install additional dependencies
pip install numpy psutil
```

### Configuration

The performance monitoring system integrates with the NLM configuration system. Add the following to your configuration file:

```yaml
performance:
  enable_monitoring: true
  monitoring_interval: 100
  enable_profiling: false
  enable_resource_monitoring: true
  export_metrics: true
```

## License

MIT License - see LICENSE file for details.

## Authors

NLM Performance Monitoring Team
