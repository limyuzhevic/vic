# Performance Monitoring Module

This module provides comprehensive performance monitoring and optimization utilities for the enhanced NLM Python API.

## Overview

The performance module extends the basic NLM API with advanced performance monitoring, benchmarking, and optimization capabilities. It provides:

- **Benchmarking tools** for measuring simulation performance
- **Memory monitoring** utilities for tracking memory usage
- **Performance profiling** for detailed analysis of execution bottlenecks
- **Adaptive optimization** based on performance metrics
- **Resource monitoring** for CPU, memory, and I/O usage
- **Bottleneck detection** and analysis
- **Performance analysis** with automated recommendations

## Key Features

### Benchmarking

```python
from nlm_utils.performance import Benchmark, benchmark_simulation

# Simple benchmarking
benchmark = Benchmark("simulation_benchmark", warmup_steps=10)

@benchmark
def run_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)

# Run benchmark
results = benchmark.run(run_simulation, 1000)
print(f"Benchmark results: {results}")
print(f"Average step time: {results['avg_time_ms']:.2f} ms")
```

### Memory Monitoring

```python
from nlm_utils.performance import MemoryMonitor, monitor_memory_usage

# Memory monitoring
monitor = MemoryMonitor("simulation_memory", interval=0.1)

@monitor
def run_memory_monitored_simulation(brain, steps):
    for step in range(steps):
        brain.step(step)
        
        # Check memory usage
        if step % 100 == 0:
            mem_info = monitor.get_current_usage()
            print(f"Step {step}: Memory usage: {mem_info['rss_mb']:.2f} MB")
            
        # Check for memory alerts
        alerts = monitor.check_alerts()
        if alerts:
            print(f"Memory alerts: {alerts}")
```

### Performance Profiling

```python
from nlm_utils.performance import Profiler, PerformanceProfiler

# Performance profiling
profiler = Profiler("brain_step_profiler")

@profiler.profile("step_execution")
def run_profiled_step(brain, step):
    brain.step(step)

# Run profiled step
run_profiled_step(brain, step)

# Get profiling results
results = profiler.get_results()
profiler.print_report()
```

## Advanced Performance Features

### Performance Metrics

The benchmarking system tracks comprehensive performance metrics:

- **Execution Time**: Wall clock time with precision timing
- **Step Performance**: Microsecond-precision step timing
- **Memory Usage**: Detailed RSS/VMS tracking with alerts
- **CPU Usage**: Real-time CPU utilization monitoring
- **Speed Metrics**: Steps per second with accuracy
- **Memory Efficiency**: Memory usage per computational unit
- **Component Performance**: Individual component profiling
- **Scalability Metrics**: Performance scaling with problem size
- **Resource Utilization**: CPU, memory, and I/O usage
- **Thermal Metrics**: CPU temperature and cooling efficiency

### Adaptive Performance Optimization

```python
from nlm_utils.performance import (
    AdaptivePerformanceOptimizer,
    ResourceMonitor,
    PerformanceTuner,
    bottleneck_detector
)

# Create adaptive performance optimizer
optimizer = AdaptivePerformanceOptimizer("simulation_optimizer")

@optimizer.optimize("memory_efficient")
@optimizer.optimize("adaptive_step")
@optimizer.optimize("batch_processing")
def optimized_simulation(brain, world, agent, steps, optimization_level=2):
    """Run simulation with adaptive performance optimization."""
    
    optimizations = {
        1: ["memory_efficient", "basic_adaptive"],
        2: ["memory_efficient", "adaptive_step", "batch_processing"],
        3: ["memory_efficient", "adaptive_step", "batch_processing", "threading", "caching"]
    }
    
    active_optimizations = optimizations.get(optimization_level, [])
    
    # Configure resource monitor
    resource_monitor = ResourceMonitor("optimized_simulation")
    resource_monitor.set_limits(
        memory_mb=1000,
        cpu_percent=80,
        step_time_ms=50
    )
    
    # Run simulation with optimizations
    for step in range(steps):
        # Adaptive performance based on current state
        if "adaptive_step" in active_optimizations:
            # Adapt step timing based on performance
            performance = optimizer.get_current_performance()
            if performance['step_time_ms'] > 50:
                # Reduce workload for slow steps
                continue
        
        # Memory-efficient processing
        if "memory_efficient" in active_optimizations:
            # Use memory-efficient data structures
            pass
        
        # Batch processing optimization
        if "batch_processing" in active_optimizations and step % 10 == 0:
            # Process in batches for efficiency
            batch_steps = min(10, steps - step)
            for batch_step in range(batch_steps):
                current_step = step + batch_step
                brain.step(current_step)
            continue
        
        # Run single step with optimizations
        brain.step(step)
        
        # Resource monitoring and throttling
        if resource_monitor.should_throttle():
            import time
            time.sleep(0.001)  # Throttle if resource limits exceeded
        
        # Log optimization metrics
        optimization_metrics = optimizer.get_optimization_metrics()
        if step % 100 == 0:
            print(f"Optimization metrics: {optimization_metrics}")
    
    # Get final optimization results
    final_results = optimizer.get_final_results()
    return final_results
```

### Memory Leak Detection

```python
from nlm_utils.performance import MemoryMonitor

# Memory leak detection
memory_monitor = MemoryMonitor("leak_detection", interval=0.1)

@memory_monitor
def run_leak_detection_simulation(brain, steps):
    """Run simulation with memory leak detection."""
    for step in range(steps):
        brain.step(step)
        
        # Check for memory leaks
        if memory_monitor.detect_memory_leak():
            print("WARNING: Potential memory leak detected!")
            
            # Get detailed leak analysis
            leak_analysis = memory_monitor.analyze_memory_leak()
            print(f"Leak analysis: {leak_analysis}")
            
            # Get memory trend
            trend = memory_monitor.get_memory_trend()
            print(f"Memory trend: {trend}")
```

### Bottleneck Detection

```python
from nlm_utils.performance import bottleneck_detector

# Bottleneck detection
bottleneck_detector = bottleneck_detector("performance_analyzer")

@bottleneck_detector.track("simulation_bottleneck")
@bottleneck_detector.track("memory_bottleneck")
@bottleneck_detector.track("cpu_bottleneck")
def detect_simulation_bottlenecks(brain, steps):
    """Detect and analyze performance bottlenecks."""
    
    # Run simulation for bottleneck detection
    for step in range(steps):
        brain.step(step)
    
    # Analyze for bottlenecks
    bottlenecks = bottleneck_detector.analyze_bottlenecks()
    
    if bottlenecks:
        print("Performance bottlenecks detected:")
        for bottleneck in bottlenecks:
            print(f"- {bottleneck['type']}: {bottleneck['severity']}")
            print(f"  Location: {bottleneck['location']}")
            print(f"  Impact: {bottleneck['impact']}")
            print(f"  Suggested fix: {bottleneck['suggested_fix']}")
    
    return bottlenecks
```

### Resource Monitoring

```python
from nlm_utils.performance import ResourceMonitor

# Resource monitoring
resource_monitor = ResourceMonitor("simulation_resources")
resource_monitor.set_limits(
    memory_mb=2000,
    cpu_percent=90,
    step_time_ms=100,
    disk_io_mb_per_sec=100
)

@resource_monitor
@Profiler("resource_profiler")
def run_resource_monitored_simulation(brain, steps):
    """Run simulation with comprehensive resource monitoring."""
    for step in range(steps):
        brain.step(step)
        
        # Check resource limits
        if resource_monitor.should_throttle():
            print("WARNING: Resource limits exceeded, throttling...")
            resource_monitor.throttle(step)
        
        # Get resource usage
        resource_usage = resource_monitor.get_current_usage()
        print(f"Step {step}: {resource_usage}")
        
        # Check for resource alerts
        alerts = resource_monitor.check_alerts()
        if alerts:
            print(f"Resource alerts: {alerts}")
```

### Performance Analysis

```python
from nlm_utils.performance import PerformanceAnalysis, PerformanceMetrics

# Performance analysis
performance_data = PerformanceMetrics()

# Collect performance data
for step in range(1000):
    step_data = collect_performance_data(step)
    performance_data.add_step_data(step, step_data)

# Analyze performance
analysis = PerformanceAnalysis(performance_data)
analysis.print_detailed_report()
analysis.export_results("performance_report.json")

# Get performance insights
insights = analysis.get_performance_insights()
print(f"Performance insights: {insights}")
```

## Advanced Performance Optimization

### 1. Adaptive Step Timing

```python
from nlm_utils.performance import AdaptivePerformanceOptimizer

# Create adaptive performance optimizer
optimizer = AdaptivePerformanceOptimizer("step_optimization")

@optimizer.optimize("adaptive_step_timing")
def adaptive_step_timing(brain, steps):
    """Run simulation with adaptive step timing."""
    for step in range(steps):
        # Get current performance
        performance = optimizer.get_current_performance()
        
        # Adapt step timing based on performance
        if performance['step_time_ms'] > 100:
            # Step is taking too long, reduce work
            continue
        elif performance['step_time_ms'] < 10:
            # Step is fast, increase work
            brain.step(step)
        else:
            # Normal step timing
            brain.step(step)
```

### 2. Memory-Efficient Processing

```python
from nlm_utils.performance import (
    MemoryMonitor,
    AdaptivePerformanceOptimizer
)

# Create memory-efficient optimizer
optimizer = AdaptivePerformanceOptimizer("memory_efficiency")

@optimizer.optimize("memory_efficient")
@MemoryMonitor("memory_efficient_monitor")
def memory_efficient_simulation(brain, steps):
    """Run simulation with memory-efficient processing."""
    # Use memory pools for neurons
    neuron_pool = create_neuron_pool()
    
    for step in range(steps):
        # Get neurons from pool (reusing memory)
        neurons = neuron_pool.get_neurons(brain.getNeuronCount())
        
        # Process with memory-efficient data structures
        for neuron in neurons:
            # Process neuron
            process_neuron(neuron)
        
        # Release neurons back to pool
        neuron_pool.release_neurons(neurons)
        
        # Monitor memory usage
        memory_info = get_current_memory_usage()
        print(f"Step {step}: Memory usage: {memory_info['rss_mb']:.2f} MB")
```

### 3. Batch Processing Optimization

```python
from nlm_utils.performance import (
    AdaptivePerformanceOptimizer,
    Benchmark
)

# Create batch processing optimizer
optimizer = AdaptivePerformanceOptimizer("batch_optimization")

@optimizer.optimize("batch_processing")
@Benchmark("batch_processing_benchmark")
def batch_processing_simulation(brain, steps, batch_size=10):
    """Run simulation with batch processing optimization."""
    for batch_start in range(0, steps, batch_size):
        batch_end = min(batch_start + batch_size, steps)
        
        # Process batch
        for step in range(batch_start, batch_end):
            brain.step(step)
        
        # Log batch performance
        batch_metrics = optimizer.get_batch_metrics(batch_start, batch_end)
        print(f"Batch {batch_start}-{batch_end}: {batch_metrics}")
```

### 4. Component-Level Profiling

```python
from nlm_utils.performance import PerformanceProfiler

# Create component-level profiler
profiler = PerformanceProfiler("component_profiling")

@profiler.profile("neural_dynamics")
@profiler.profile("plasticity_updates")
@profiler.profile("memory_management")
@profiler.profile("development_updates")
@profiler.profile("neuromodulation")
def component_profiled_simulation(brain, steps):
    """Run simulation with component-level profiling."""
    for step in range(steps):
        # Profile neural dynamics
        neural_start = time.time()
        brain.update_neural_dynamics()
        neural_time = time.time() - neural_start
        
        # Profile plasticity updates
        plasticity_start = time.time()
        brain.update_plasticity()
        plasticity_time = time.time() - plasticity_start
        
        # Profile memory management
        memory_start = time.time()
        brain.manage_memory()
        memory_time = time.time() - memory_start
        
        # Get component metrics
        component_metrics = {
            'neural_time': neural_time,
            'plasticity_time': plasticity_time,
            'memory_time': memory_time,
            'step_time': time.time() - start_time
        }
        
        # Log component metrics
        print(f"Step {step}: {component_metrics}")
```

## Integration Examples

### Enhanced Simulation with Performance Monitoring

```python
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, Benchmark, MemoryMonitor,
    AdaptivePerformanceOptimizer
)

def run_enhanced_simulation_with_performance(brain_config, steps=1000):
    """Run enhanced simulation with comprehensive performance monitoring."""
    
    # Create enhanced configuration
    config = SessionConfigAdvanced(
        name="performance_monitored_simulation",
        brain_config=brain_config,
        checkpoint_interval=100,
        performance_monitoring=True,
        error_handling.recovery=True
    )
    
    session = AdvancedBrainSession(config)
    
    # Set up comprehensive performance monitoring
    benchmark = Benchmark("performance_monitored_simulation", warmup_steps=20)
    memory_monitor = MemoryMonitor("simulation_memory")
    optimizer = AdaptivePerformanceOptimizer("simulation_optimization")
    
    @benchmark
    @memory_monitor
    @optimizer.optimize("adaptive_step")
    @optimizer.optimize("memory_efficient")
    def run_monitored_simulation(session, steps):
        """Run simulation with comprehensive performance monitoring."""
        for step in range(steps):
            # Run step with performance optimization
            session.brain.step(step)
            
            # Check performance metrics
            performance = optimizer.get_current_performance()
            if performance['step_time_ms'] > 50:
                # Optimize based on performance
                optimizer.optimize_for_performance()
            
            # Log performance metrics
            if step % 100 == 0:
                print(f"Step {step}: Performance {performance}")
        
        return {
            'benchmark_results': benchmark.get_results(),
            'memory_stats': memory_monitor.get_summary(),
            'optimization_results': optimizer.get_final_results()
        }
    
    # Run enhanced simulation
    results = run_monitored_simulation(session, steps)
    
    # Save session
    session.save("performance_monitored_simulation")
    
    return results
```

### Performance-Optimized Experiment

```python
from nlm_utils import (
    AdvancedExperiment, MultiObjectiveExperiment,
    AdaptivePerformanceOptimizer, ResourceMonitor
)

def run_performance_optimized_experiment(experiment_config, steps=1000):
    """Run experiment with performance optimization."""
    
    # Configure performance optimizer
    optimizer = AdaptivePerformanceOptimizer("experiment_optimization")
    resource_monitor = ResourceMonitor("experiment_resources")
    
    @optimizer.optimize("adaptive_step")
    @optimizer.optimize("memory_efficient")
    @resource_monitor
    def run_optimized_experiment(experiment_func, parameters):
        """Run experiment with performance optimization."""
        for step in range(steps):
            # Run experiment step with optimization
            experiment_func(parameters, step)
            
            # Optimize based on resource usage
            if resource_monitor.should_throttle():
                optimizer.optimize_for_resource_usage()
        
        return experiment_func.get_results()
    
    # Run performance-optimized experiment
    return run_optimized_experiment(experiment_config.run, experiment_config.parameters)
```

## Best Practices

### 1. Configure Performance Monitoring Early

Configure performance monitoring at the start of your application:

```python
from nlm_utils import Benchmark, MemoryMonitor

# Configure monitoring
benchmark = Benchmark("my_application", warmup_steps=10)
memory_monitor = MemoryMonitor("my_application_memory")
```

### 2. Use Adaptive Optimization

Use adaptive optimization based on performance metrics:

```python
from nlm_utils import AdaptivePerformanceOptimizer

optimizer = AdaptivePerformanceOptimizer("my_app")
@optimizer.optimize("adaptive_step")
def my_simulation():
    # Run simulation with adaptive optimization
    pass
```

### 3. Monitor Resource Usage

Monitor resource usage to prevent performance degradation:

```python
from nlm_utils import ResourceMonitor

resource_monitor = ResourceMonitor("my_app")
resource_monitor.set_limits(memory_mb=2000, cpu_percent=90)
```

### 4. Analyze Performance Regularly

Analyze performance data regularly to identify optimization opportunities:

```python
from nlm_utils import PerformanceAnalysis

# Run simulation and collect performance data
performance_data = PerformanceMetrics()

# Analyze performance
analysis = PerformanceAnalysis(performance_data)
analysis.print_detailed_report()
```

### 5. Use Memory-Efficient Patterns

Use memory-efficient patterns for long-running simulations:

```python
from nlm_utils import MemoryMonitor

memory_monitor = MemoryMonitor("memory_efficient_app")

@memory_monitor
def memory_efficient_simulation():
    # Use memory pools and efficient data structures
    pass
```

## Performance Considerations

### Profiling Overhead

- Profiling has minimal overhead when disabled
- Configure profiling levels to reduce overhead in production
- Use sampling for long-running simulations

### Memory Monitoring

- Memory monitoring uses efficient algorithms with minimal overhead
- Configure monitoring intervals to balance granularity and performance
- Use memory pools for better memory management

### Adaptive Optimization

- Adaptive optimization uses lightweight performance metrics
- Configure optimization frequency based on performance requirements
- Use thresholds to balance performance and resource usage

### Resource Management

- Resource monitoring uses efficient resource tracking
- Configure resource limits based on system capabilities
- Implement graceful degradation when resources are constrained

## Migration Guide

### From Basic Simulation to Enhanced Performance Monitoring

```python
# Basic simulation
for step in range(1000):
    brain.step(step)

# Enhanced simulation with performance monitoring
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    Benchmark, MemoryMonitor
)

config = SessionConfigAdvanced(
    name="performance_monitored",
    brain_config={"performance.monitoring": True},
    checkpoint_interval=100
)

session = AdvancedBrainSession(config)
benchmark = Benchmark("enhanced_simulation")
memory_monitor = MemoryMonitor("simulation_memory")

@benchmark
@memory_monitor
def run_enhanced_simulation(session, steps):
    for step in range(steps):
        session.brain.step(step)
    
    return {
        'benchmark_results': benchmark.get_results(),
        'memory_stats': memory_monitor.get_summary()
    }

results = run_enhanced_simulation(session, 1000)
```

### Adding Performance Optimization

```python
# Basic simulation
for step in range(1000):
    brain.step(step)

# Enhanced simulation with performance optimization
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdaptivePerformanceOptimizer, ResourceMonitor
)

config = SessionConfigAdvanced(
    name="performance_optimized",
    brain_config={
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    checkpoint_interval=100
)

session = AdvancedBrainSession(config)
optimizer = AdaptivePerformanceOptimizer("optimized_simulation")
resource_monitor = ResourceMonitor("optimized_resources")

@optimizer.optimize("adaptive_step")
@optimizer.optimize("memory_efficient")
def run_optimized_simulation(session, steps):
    for step in range(steps):
        session.brain.step(step)
    
    return optimizer.get_final_results()

results = run_optimized_simulation(session, 1000)
```

## Conclusion

The performance module provides comprehensive tools for performance monitoring and optimization in NLM applications. It enables:

- **Performance benchmarking** with detailed metrics and analysis
- **Memory monitoring** with leak detection and alerts
- **Performance profiling** with component-level analysis
- **Adaptive optimization** based on real-time performance metrics
- **Resource management** with efficient resource tracking
- **Bottleneck detection** with automated analysis
- **Performance analysis** with actionable insights

These enhancements make NLM applications more efficient and scalable while maintaining backward compatibility.

For more advanced performance optimization features, see the other utility modules in the nlm_utils package.
