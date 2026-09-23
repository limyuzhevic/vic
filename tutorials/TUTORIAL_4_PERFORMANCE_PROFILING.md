# Tutorial 4: Performance Profiling

## Overview

This tutorial introduces you to NLM (Neural Learning Machine) performance profiling and optimization. It covers:

- Performance profiling with PerformanceProfiler
- Memory profiling with MemoryProfiler
- Performance analysis and optimization
- Profiling best practices
- Performance optimization strategies

This tutorial is designed for users who want to achieve maximum performance from their NLM simulations.

## Prerequisites

Before starting this tutorial, you should:

1. **Be familiar with Tutorial 3**: Batch processing and performance concepts
2. **Understand performance optimization**: Memory usage, CPU performance
3. **Have NLM installed**: As per the main installation instructions
4. **Consider system resources**: Ensure adequate memory and CPU for profiling

## What You'll Learn

By the end of this tutorial, you will:

- Understand NLM performance profiling architecture and components
- Create performance profiles for simulations
- Analyze performance data and identify bottlenecks
- Implement memory profiling for optimization
- Apply performance optimization strategies
- Use profiling tools for continuous improvement

## Setting Up Your Environment

The same as previous tutorials. Ensure NLM is installed:

```bash
pip install pynlm
```

## NLM Performance Profiling System

### Overview

NLM's performance profiling system provides comprehensive performance analysis tools for neural simulations, enabling you to:

1. **Profile Performance**: Profile execution time, memory usage, and I/O operations
2. **Analyze Performance**: Analyze performance data and identify bottlenecks
3. **Optimize Performance**: Apply optimization strategies based on profiling results
4. **Monitor Performance**: Monitor performance in real-time
5. **Document Performance**: Document performance characteristics for future reference

### Performance Profiling Architecture

```
NLM Performance Profiling Architecture

┌─────────────────────────────────────────────────────────┐
│                    Performance Profiling              │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐            │
│  │ PerformanceProfiler   │  │  MemoryProfiler   │            │
│  │ (CPU, I/O, Time) │  │ (Memory, Leaks) │            │
│  └─────────────────┘  └─────────────────┘            │
│         │                      │                   │
│   ┌─────▼─────┐        ┌─────────▼─────────┐           │
│   │ DataCollection │        │   Analysis     │           │
│   │ (Metrics)    │        │ (Statistics)   │           │
│   └─────────────┘        └─────────────────┘           │
│                                │                   │
│   ┌─────────────────┐  ┌─────────▼────────┐          │
│   │ Reporting     │  │  Optimization     │          │
│   │ (Reports)    │  │ (Recommendations) │          │
│   └─────────────────┘  └─────────────────┘          │
└─────────────────────────────────────────────────────────┘
```

## Performance Profiling Components

### 1. PerformanceProfiler

Performance profiling and analysis:

```cpp
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& name);
    ~PerformanceProfiler();
    
    void setup();
    void teardown();
    
    void recordMetric(const std::string& name, double value);
    PerformanceReport generateReport();
    
    void analyzeReport(const PerformanceReport& report);
    void printReport(const PerformanceReport& report);
    
    double calculatePerformanceRating(const PerformanceReport& report) const;
    
private:
    std::string name_;
    std::map<std::string, std::vector<double>> metrics_;
    double startTime_;
    double endTime_;
};
```

**Key Features**:
- **Multi-Stage Profiling**: Setup, execution, and teardown phases
- **Component-Level Profiling**: Profile individual components separately
- **Metric Collection**: Collect and store performance metrics
- **Report Generation**: Generate performance reports
- **Performance Analysis**: Analyze performance data and calculate ratings

### 2. MemoryProfiler

Memory profiling and analysis:

```cpp
class MemoryProfiler {
public:
    MemoryProfiler(const std::string& name);
    ~MemoryProfiler();
    
    void setup();
    void teardown();
    
    void recordMemoryUsage();
    MemoryReport generateReport();
    
    void analyzeReport(const MemoryReport& report);
    void printReport(const MemoryReport& report);
    
    double calculateMemoryRating(const MemoryReport& report) const;
    
private:
    std::string name_;
    std::vector<MemorySample> memorySamples_;
    double startTime_;
    double endTime_;
};
```

**Key Features**:
- **Allocation Tracking**: Track all memory allocations and deallocations
- **Leak Detection**: Detect memory leaks
- **Memory Usage Analysis**: Analyze memory usage patterns
- **Fragmentation Analysis**: Analyze memory fragmentation
- **Memory Efficiency Calculation**: Calculate memory efficiency metrics

## Performance Profiling Techniques

### 1. CPU Profiling

```python
import pynlm
from performance import PerformanceProfiler

print("=== NLM Tutorial 4: Performance Profiling ===")

# 1. Create performance profiler
print("\n1. Creating performance profiler...")
profiler = PerformanceProfiler("cpu_performance")
profiler.setup()

# 2. Create brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation with CPU profiling
print("\n2. Running simulation with CPU profiling...")
print("   Processing 10000 simulation steps...")

start_time = time.time()
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        print(f"   Step {step}: CPU profiling active...")
end_time = time.time()

profiler.teardown()

# 4. Generate performance report
print("\n3. Generating performance report...")
report = profiler.generateReport()

# 5. Analyze performance report
print("\n4. Analyzing performance report...")
profiler.analyzeReport(report)

# 6. Print performance report
print("\n5. Performance Report:")
profiler.printReport(report)

# 7. Calculate performance rating
print("\n6. Performance Analysis:")
performance_rating = profiler.calculatePerformanceRating(report)
print(f"   Performance rating: {performance_rating:.1f}/100")
print(f"   Duration: {report['duration']:.2f}s")
print(f"   Throughput: {report['throughput']:.0f} steps/s")
print(f"   CPU efficiency: {report['cpu_efficiency'] * 100:.1f}%")

print("\n=== Performance Profiling Complete! ===")
```

### 2. Memory Profiling

```python
import pynlm
from performance import MemoryProfiler

print("=== NLM Tutorial 4: Memory Profiling ===")

# 1. Create memory profiler
print("\n1. Creating memory profiler...")
profiler = MemoryProfiler("memory_performance")
profiler.setup()

# 2. Create brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation with memory profiling
print("\n2. Running simulation with memory profiling...")
print("   Processing 10000 simulation steps...")

for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        print(f"   Step {step}: Memory profiling active...")

profiler.teardown()

# 4. Generate memory report
print("\n3. Generating memory report...")
report = profiler.generateReport()

# 5. Analyze memory report
print("\n4. Analyzing memory report...")
profiler.analyzeReport(report)

# 6. Print memory report
print("\n5. Memory Report:")
profiler.printReport(report)

# 7. Calculate memory rating
print("\n6. Memory Analysis:")
memory_rating = profiler.calculateMemoryRating(report)
print(f"   Memory rating: {memory_rating:.1f}/100")
print(f"   Peak memory: {report['peak_memory']:.2f} MB")
print(f"   Average memory: {report['average_memory']:.2f} MB")
print(f"   Memory efficiency: {report['memory_efficiency']:.2f} neurons/MB")

if report['memory_leaks_detected']:
    print("\n   Memory leaks detected:")
    for leak in report['memory_leaks']:
        print(f"     - {leak}")

print("\n=== Memory Profiling Complete! ===")
```

### 3. Combined Profiling

```python
import pynlm
from performance import PerformanceProfiler, MemoryProfiler

print("=== NLM Tutorial 4: Combined Performance and Memory Profiling ===")

# 1. Create performance and memory profilers
print("\n1. Creating performance and memory profilers...")
performance_profiler = PerformanceProfiler("combined_performance")
memory_profiler = MemoryProfiler("combined_memory")

# 2. Start profiling
print("\n2. Starting combined profiling...")
performance_profiler.setup()
memory_profiler.setup()

# 3. Create brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# 4. Run simulation with combined profiling
print("\n3. Running simulation with combined profiling...")
print("   Processing 10000 simulation steps...")

start_time = time.time()
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        print(f"   Step {step}: Combined profiling active...")
end_time = time.time()

performance_profiler.teardown()
memory_profiler.teardown()

# 5. Generate reports
print("\n4. Generating reports...")
performance_report = performance_profiler.generateReport()
memory_report = memory_profiler.generateReport()

# 6. Analyze reports
print("\n5. Analyzing reports...")
performance_profiler.analyzeReport(performance_report)
memory_profiler.analyzeReport(memory_report)

# 7. Print combined analysis
print("\n6. Combined Performance and Memory Analysis:")
print("=== Performance Analysis ===")
performance_profiler.printReport(performance_report)
print("\n=== Memory Analysis ===")
memory_profiler.printReport(memory_report)

# 8. Calculate combined ratings
performance_rating = performance_profiler.calculatePerformanceRating(performance_report)
memory_rating = memory_profiler.calculateMemoryRating(memory_report)
combined_rating = (performance_rating + memory_rating) / 2

print(f"\n=== Combined Analysis ===")
print(f"   Performance rating: {performance_rating:.1f}/100")
print(f"   Memory rating: {memory_rating:.1f}/100")
print(f"   Combined rating: {combined_rating:.1f}/100")

print("\n=== Combined Performance and Memory Profiling Complete! ===")
```

## Performance Analysis

### CPU Performance Analysis

```python
import pynlm
from performance import PerformanceProfiler

print("=== NLM Tutorial 4: CPU Performance Analysis ===")

# 1. Create performance profiler
print("\n1. Creating performance profiler...")
profiler = PerformanceProfiler("cpu_analysis")
profiler.setup()

# 2. Create brain with performance optimization
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation with CPU profiling
print("\n2. Running simulation with CPU profiling...")
print("   Processing 10000 simulation steps...")

start_time = time.time()
for step in range(10000):
    brain.step(step)
end_time = time.time()

profiler.teardown()

# 4. Generate performance report
print("\n3. Generating performance report...")
report = profiler.generateReport()

# 5. Analyze CPU performance
print("\n4. CPU Performance Analysis:")
print(f"   Duration: {report['duration']:.2f}s")
print(f"   Steps per second: {report['steps_per_second']:.0f}")
print(f"   CPU time: {report['cpu_time']:.2f}s")
print(f"   CPU efficiency: {report['cpu_efficiency'] * 100:.1f}%")
print(f"   Real-time factor: {report['real_time_factor']:.2f}")

# 6. Identify performance bottlenecks
print("\n5. Performance Bottleneck Analysis:")
if report['cpu_efficiency'] < 0.5:
    print("   CPU efficiency is low (< 50%)")
    print("   Possible bottlenecks:")
    print("     - Memory bandwidth limitations")
    print("     - Cache misses")
    print("     - I/O bottlenecks")
    print("     - Synchronization overhead")
elif report['real_time_factor'] < 1.0:
    print("   Simulation is not real-time (real-time factor < 1.0)")
    print("   Possible solutions:")
    print("     - Increase batch size")
    print("     - Enable SIMD optimization")
    print("     - Optimize memory access patterns")
elif report['steps_per_second'] < 1000:
    print("   Low performance (< 1000 steps/second)")
    print("   Possible optimizations:")
    print("     - Use batch processing")
    print("     - Enable multithreading")
    print("     - Optimize data structures")
else:
    print("   CPU performance is good!")

print("\n=== CPU Performance Analysis Complete! ===")
```

### Memory Performance Analysis

```python
import pynlm
from performance import MemoryProfiler

print("=== NLM Tutorial 4: Memory Performance Analysis ===")

# 1. Create memory profiler
print("\n Ernest creating memory profiler...")
profiler = MemoryProfiler("memory_analysis")
profiler.setup()

# 2. Create brain with memory optimization
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)

brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation with memory profiling
print("\n2. Running simulation with memory profiling...")
print("   Processing 10000 simulation steps...")

for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        print(f"   Step {step}: Memory analysis active...")

profiler.teardown()

# 4. Generate memory report
print("\n3. Generating memory report...")
report = profiler.generateReport()

# 5. Analyze memory performance
print("\n4. Memory Performance Analysis:")
print(f"   Peak memory: {report['peak_memory']:.2f} MB")
print(f"   Average memory: {report['average_memory']:.2f} MB")
print(f"   Memory efficiency: {report['memory_efficiency']:.2f} neurons/MB")
print(f"   Memory usage per step: {report['memory_per_step']:.2f} bytes")

# 6. Identify memory issues
print("\n5. Memory Issue Analysis:")
if report['memory_efficiency'] < 100:
    print("   Memory efficiency is low (< 100 neurons/MB)")
    print("   Possible issues:")
    print("     - Memory pooling not enabled")
    print("     - Inefficient data structures")
    print("     - Memory fragmentation")
    print("     - Large neuron states")
elif report['peak_memory'] > 500:
    print("   Peak memory is high (> 500 MB)")
    print("   Possible optimizations:")
    print("     - Use smaller brain size")
    print("     - Optimize memory layout")
    print("     - Enable memory compression")
elif report['memory_leaks_detected']:
    print("   Memory leaks detected")
    print("   Possible issues:")
    print("     - Memory allocation not freed")
    print("     - Circular references")
    print("     - Resource management issues")
else:
    print("   Memory usage is good!")

print("\n=== Memory Performance Analysis Complete! ===")
```

## Performance Optimization Strategies

### 1. CPU Optimization

#### Use Batch Processing
```python
import pynlm
from performance import BatchBrain

# Create batch brain with performance optimizations
config = pynlm.createDefaultConfig()
batch_brain = BatchBrain(config)

# Enable performance optimizations
batch_brain.enableSIMD(True)
batch_brain.enableMultithreading(True)
batch_brain.enableMemoryPool(True)

# Process batch
batch_brain.processBatch(0, 9999)
```

#### Optimize Memory Access
```python
import pynlm
import numpy as np

class CacheOptimizedBrain:
    """Cache-optimized brain implementation."""
    
    def __init__(self, config):
        """Initialize cache-optimized brain."""
        self.config = config
        self.setupCacheOptimization()
        
        # Create cache-optimized data structures
        self.setupCacheOptimizedDataStructures()
    
    def setupCacheOptimization(self):
        """Set up cache optimization strategies."""
        # Enable cache optimization
        self.cache_optimized = True
        self.cache_line_size = 64  # bytes
        
        # Configure access patterns
        self.spatial_locality = True
        self.temporal_locality = True
    
    def setupCacheOptimizedDataStructures(self):
        """Set up cache-optimized data structures."""
        # Use Structure-of-Arrays (SoA) for better cache performance
        self.setupSoALayout()
        
        # Use aligned memory for better cache performance
        self.setupAlignedMemory()
    
    def setupSoALayout(self):
        """Set up Structure-of-Arrays layout."""
        # Use separate arrays for each neuron state component
        # to improve cache locality
        
        self.neuron_ids = np.arange(self.neuron_count, dtype=np.int32)
        self.membrane_potentials = np.full(self.neuron_count, -70.0, dtype=np.float32)
        self.refractory_counters = np.zeros(self.neuron_count, dtype=np.float32)
        self.spike_counts = np.zeros(self.neuron_count, dtype=np.int32)
    
    def setupAlignedMemory(self):
        """Set up aligned memory for cache efficiency."""
        # Align arrays to cache line boundary
        cache_line_size = 64
        
        # Note: NumPy arrays are already allocated on heap,
        # but we can ensure they're accessed in cache-friendly patterns
    
    def processNeuron(self, neuron_id):
        """Process single neuron with cache-friendly access."""
        # Access neuron data in cache-friendly pattern
        membrane_potential = self.membrane_potentials[neuron_id]
        refractory_counter = self.refractory_counters[neuron_id]
        
        # Update neuron state
        self.updateNeuronState(neuron_id, membrane_potential, refractory_counter)
```

#### Use SIMD Vectorization
```python
import pynlm
from performance import BatchBrain

# Create SIMD-optimized batch brain
config = pynlm.createDefaultConfig()
batch_brain = BatchBrain(config)

# Enable SIMD optimization
batch_brain.enableSIMD(True)

# Process batch with SIMD optimization
batch_brain.processBatch(0, 9999)
```

### 2. Memory Optimization

#### Use Memory Pooling
```python
import pynlm

class MemoryOptimizedBrain:
    """Memory-optimized brain implementation."""
    
    def __init__(self, config):
        """Initialize memory-optimized brain."""
        self.config = config
        self.setupMemoryOptimization()
        
        # Create memory-optimized data structures
        self.setupMemoryOptimizedDataStructures()
    
    def setupMemoryOptimization(self):
        """Set up memory optimization strategies."""
        # Enable memory pooling
        self.memory_pooling_enabled = True
        
        # Configure memory optimization
        self.slab_allocation_enabled = True
        self.cache_friendly_layout = True
    
    def setupMemoryOptimizedDataStructures(self):
        """Set up memory-optimized data structures."""
        # Use Structure-of-Arrays (SoA) for memory efficiency
        self.setupSoALayout()
        
        # Use small data types for memory efficiency
        self.setupSmallDataTypes()
    
    def setupSoALayout(self):
        """Set up Structure-of-Arrays layout."""
        # Use separate arrays for each neuron state component
        self.neuron_potentials = np.zeros(self.neuron_count, dtype=np.float32)
        self.refractory_counters = np.zeros(self.neuron_count, dtype=np.float32)
        self.spike_counts = np.zeros(self.neuron_count, dtype=np.int32)
    
    def setupSmallDataTypes(self):
        """Set up small data types for memory efficiency."""
        # Use smaller data types to reduce memory usage
        self.synapse_weights = np.zeros(self.synapse_count, dtype=np.float16)
        self.synapse_delays = np.zeros(self.synapse_count, dtype=np.float16)
```

#### Use Event-Driven Processing
```python
import pynlm

class EventDrivenOptimizedBrain:
    """Event-driven, optimized brain implementation."""
    
    def __init__(self, config):
        """Initialize event-driven optimized brain."""
        self.config = config
        self.setupEventDrivenOptimization()
        
        # Create event-driven components
        self.setupEventDrivenComponents()
    
    def setupEventDrivenOptimization(self):
        """Set up event-driven optimization strategies."""
        # Enable event-driven processing
        self.event_driven_enabled = True
        
        # Configure event processing
        self.sparse_connection_enabled = True
        self.delayed_spike_enabled = True
    
    def setupEventDrivenComponents(self):
        """Set up event-driven components."""
        # Create event queue
        self.event_queue = EventQueue(16384)
        
        # Create time-bucketed delayed spike queue
        self.delayed_spike_queue = TimeBucketedQueue(1024, 100)
    
    def step(self, step_number):
        """Process one simulation step with event-driven optimization."""
        current_time = step_number * 0.1
        
        # Process events from queue
        self.processEvents(current_time)
        
        # Schedule new events
        self.scheduleNewEvents(current_time)
```

## Performance Profiling Tools

### Performance Profiler

```python
import pynlm
from performance import PerformanceProfiler

# Create performance profiler
profiler = PerformanceProfiler("simulation_profile")

# Start profiling
profiler.setup()

# Run simulation
for step in range(1000):
    brain.step(step)

# Stop profiling
profiler.teardown()

# Generate performance report
report = profiler.generateReport()

# Save performance report
profiler.saveReport("performance_report.json")

# Analyze performance report
profiler.analyzeReport(report)
```

### Memory Profiler

```python
import pynlm
from performance import MemoryProfiler

# Create memory profiler
profiler = MemoryProfiler("memory_profile")

# Start memory profiling
profiler.setup()

# Run simulation
for step in range(1000):
    brain.step(step)

# Stop memory profiling
profiler.teardown()

# Generate memory report
report = profiler.generateReport()

# Save memory report
profiler.saveReport("memory_report.json")

# Analyze memory report
profiler.analyzeReport(report)
```

## Performance Monitoring Scripts

### Performance Monitoring Script

```bash
#!/bin/bash
# monitor_performance.sh - Monitor NLM performance

# Check if NLM is installed
if ! command -v nlm &> /dev/null; then
    echo "NLM is not installed. Installing..."
    pip install pynlm
fi

# Create performance monitoring configuration
mkdir -p config

cat > config/performance_monitor.json << EOF
{
  "performance": {
    "profiling": true,
    "memory_profiling": true,
    "batch_processing": true
  },
  "brain": {
    "neuron_count": 5000,
    "synapse_density": 0.1
  }
}
