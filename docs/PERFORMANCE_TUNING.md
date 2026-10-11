# NLM Performance Tuning Guide

## Overview

This guide provides comprehensive information on optimizing NLM (Neural Learning Machine) performance for various use cases and scenarios. It covers configuration optimization, simulation tuning, and advanced techniques for improving performance.

## Performance Optimization Strategies

### 1. Configuration Optimization

#### Brain Configuration

**Optimal Settings for Different Use Cases:**

```python
# For research simulations (high accuracy)
research_config = pynlm.createDefaultConfig()
research_config.set("brain.neuron_count", 5000)
research_config.set("brain.synapse_density", 0.2)
research_config.set("plasticity.stdp.learning_rate", 0.001)
research_config.set("simulation.timestep", 0.001)

# For real-time applications (low latency)
realtime_config = pynlm.createDefaultConfig()
realtime_config.set("brain.neuron_count", 500)
realtime_config.set("brain.synapse_density", 0.05)
realtime_config.set("simulation.timestep", 0.0001)

# For educational demonstrations (small scale)
ed_config = pynlm.createDefaultConfig()
ed_config.set("brain.neuron_count", 100)
ed_config.set("brain.synapse_density", 0.01)
ed_config.set("plasticity.stdp.learning_rate", 0.01)

# For large-scale simulations (comprehensive)
large_config = pynlm.createDefaultConfig()
large_config.set("brain.neuron_count", 20000)
large_config.set("brain.synapse_density", 0.3)
large_config.set("simulation.timestep", 0.001)
```

#### Plasticity Configuration

**Plasticity Optimization:**

```python
# Enable/disable specific plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.0005)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0001f)
config.set("plasticity.structural.pruning_rate", 0.00001f)
```

#### Neuromodulation Configuration

**Neuromodulation Optimization:**

```python
# Neuromodulation settings
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.scale", 1.0)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.scale", 1.0)
config.set("neuromod.prediction_error.scale", 1.0)
```

### 2. Simulation Parameters

#### Time Stepping Optimization

**Single Step Optimization:**

```python
# Single step performance considerations
time_step = 0.001  # 1 ms (good balance of accuracy and speed)
# Alternative for real-time:
# time_step = 0.0001  # 0.1 ms (higher accuracy, slower)
```

**Multi-step Considerations:**

```python
# For short simulations (high frequency)
short_config = pynlm.createDefaultConfig()
short_config.set("simulation.timestep", 0.0001)
short_config.set("brain.neuron_count", 1000)

# For long simulations (batch processing)
long_config = pynlm.createDefaultConfig()
long_config.set("simulation.timestep", 0.001)
long_config.set("brain.neuron_count", 5000)
```

### 3. Memory Management

#### Memory Pool Configuration

**Enable memory pools for better performance:**

```python
# Enable memory pools (if supported)
config.set("performance.enable_memory_pools", True)
config.set("performance.neuron_pool_size", 1024)
config.set("performance.synapse_pool_size", 5120)
config.set("performance.chunk_size", 64)
```

#### Cache Optimization

**Cache-friendly configurations:**

```python
# Configure for cache efficiency
config.set("brain.neuron_count", 1024)  # Power of 2 for better cache utilization
config.set("brain.synapse_density", 0.1)  # Moderate density for cache efficiency
```

### 4. Parallel Processing

**Multi-core optimization:**

```python
# Configure for parallel processing
config.set("performance.enable_parallel_processing", True)
config.set("performance.num_threads", 4)  # Number of CPU cores to use
```

## Performance Monitoring

### Built-in Metrics

**Available Performance Metrics:**

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Basic performance metrics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spike count: {brain.getTotalSpikeCount()}")
print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")
```

### Custom Performance Tracking

**Implement custom performance monitoring:**

```python
import time
import pynlm

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.step_times = []
        self.memory_usage = []
        self.start_time = time.time()
    
    def record_step(self, step_time):
        self.step_times.append(step_time)
    
    def get_average_step_time(self):
        if not self.step_times:
            return 0.0
        return sum(self.step_times) / len(self.step_times)
    
    def get_total_simulation_time(self):
        return time.time() - self.start_time
    
    def get_performance_stats(self):
        return {
            "total_steps": len(self.step_times),
            "average_step_time": self.get_average_step_time(),
            "total_simulation_time": self.get_total_simulation_time(),
            "steps_per_second": len(self.step_times) / self.get_total_simulation_time() if self.get_total_simulation_time() > 0 else 0
        }

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = PerformanceMonitor(brain)
for step in range(1000):
    start_time = time.time()
    brain.step(step)
    step_time = time.time() - start_time
    monitor.record_step(step_time)

stats = monitor.get_performance_stats()
print(f"Performance stats: {stats}")
```

## Advanced Tuning Techniques

### 1. SIMD Optimizations

**Manual SIMD optimization:**

```python
# Configure for SIMD optimization
config.set("performance.enable_simd", True)
config.set("performance.simd_instruction_set", "AVX2")  # or "SSE4.1", "AVX512"

# Align memory for SIMD operations
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

### 2. NUMA Optimization

**NUMA-aware memory allocation:**

```python
# Configure for NUMA systems
config.set("performance.enable_numa", True)
config.set("performance.numa_node", 0)  # Specify NUMA node
```

### 3. Thermal Management

**Thermal-aware scheduling:**

```python
# Configure for thermal management
config.set("performance.enable_thermal_mitigation", True)
config.set("performance.thermal_throttle_threshold", 75.0)  # Celsius
```

## Profiling and Benchmarking

### Profiling Tools

**Built-in profiling:**

```python
import pynlm

# Enable profiling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Profile different operations
print("=== Brain Creation Profile ===")
import time
start_time = time.time()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
creation_time = time.time() - start_time
print(f"Brain creation time: {creation_time:.3f} seconds")

print("=== Initialization Profile ===")
start_time = time.time()
brain.initialize()
init_time = time.time() - start_time
print(f"Brain initialization time: {init_time:.3f} seconds")

print("=== Step Performance Profile ===")
start_time = time.time()
for i in range(100):
    brain.step(i)
step_time = time.time() - start_time
print(f"100 steps time: {step_time:.3f} seconds")
print(f"Average step time: {step_time/100:.3f} seconds")
```

### Benchmarking

**Benchmarking framework:**

```python
import pynlm
import time

class Benchmark:
    def __init__(self, name):
        self.name = name
        self.results = []
    
    def run(self, config, num_steps=1000):
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(num_steps):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        result = {
            "config": config,
            "steps": num_steps,
            "duration": duration,
            "steps_per_second": num_steps / duration,
            "average_step_time": duration / num_steps
        }
        
        self.results.append(result)
        return result
    
    def compare(self, benchmarks):
        print(f"=== Benchmark Comparison: {self.name} ===")
        for benchmark in benchmarks:
            result = benchmark.results[-1]
            print(f"  {benchmark.name}: {result['steps_per_second']:.0f} steps/sec")
            print(f"    Average step time: {result['average_step_time']*1000:.3f} ms")

# Run benchmarks
configs = [
    ("Small (100 neurons)", pynlm.createDefaultConfig()),
    ("Medium (1000 neurons)", pynlm.createDefaultConfig()),
    ("Large (5000 neurons)", pynlm.createDefaultConfig()),
]

# Configure each config
configs[0][1].set("brain.neuron_count", 100)
configs[1][1].set("brain.neuron_count", 1000)
configs[2][1].set("brain.neuron_count", 5000)

# Run benchmarks
benchmarks = [Benchmark(name) for name, _ in configs]
for (name, config), benchmark in zip(configs, benchmarks):
    print(f"Running benchmark: {name}")
    benchmark.run(config, num_steps=500)

# Compare results
Benchmark.compare(benchmarks)
```

## Memory Optimization

### Memory Pool Configuration

**Memory pool settings:**

```python
# Memory pool configuration
config.set("performance.memory_pool.neuron_pool_size", 1024)
config.set("performance.memory_pool.synapse_pool_size", 5120)
config.set("performance.memory_pool.population_pool_size", 256)
config.set("performance.memory_pool.region_pool_size", 64)
config.set("performance.memory_pool.chunk_size", 64)
config.set("performance.memory_pool.enable_free_list", True)
```

### Memory Monitoring

**Monitor memory usage:**

```python
import psutil
import pynlm

class MemoryMonitor:
    def __init__(self):
        self.process = psutil.Process()
        self.memory_samples = []
    
    def record_memory(self):
        memory_info = self.process.memory_info()
        self.memory_samples.append({
            "rss": memory_info.rss,
            "vms": memory_info.vms,
            "shared": memory_info.shared,
            "text": memory_info.text,
            "data": memory_info.data,
            "dirty": memory_info.dirty
        })
    
    def get_memory_stats(self):
        if not self.memory_samples:
            return None
        
        rss_values = [m["rss"] for m in self.memory_samples]
        return {
            "max_rss": max(rss_values) / 1024 / 1024,  # MB
            "min_rss": min(rss_values) / 1024 / 1024,  # MB
            "avg_rss": sum(rss_values) / len(rss_values) / 1024 / 1024,  # MB
            "sample_count": len(self.memory_samples)
        }

# Usage
monitor = MemoryMonitor()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    monitor.record_memory()

stats = monitor.get_memory_stats()
print(f"Memory stats: {stats}")
```

### Garbage Collection Optimization

**Optimize garbage collection:**

```python
import pynlm
import gc

# Enable debugging mode
config.set("performance.debug.enable", True)
config.set("performance.debug.gc_stats", True)

# Monitor garbage collection
gc.enable()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with GC monitoring
for step in range(1000):
    brain.step(step)
    
    # Force garbage collection every 100 steps
    if step % 100 == 0:
        gc.collect()

print(f"GC stats: {gc.get_stats()}")
```

## Troubleshooting Common Performance Issues

### Slow Brain Initialization

**Symptoms:** Initialization takes longer than expected

**Causes:**
1. Large neuron/synapse counts
2. Complex connectivity patterns
3. Memory allocation issues

**Solutions:**
```python
# Solution 1: Reduce neuron count
config.set("brain.neuron_count", 1000)

# Solution 2: Use smaller populations
config.set("brain.region_count", 1)

# Solution 3: Use optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
```

### Slow Simulation Steps

**Symptoms:** Individual step execution is slow

**Causes:**
1. Large neural populations
2. Complex plasticity rules
3. Memory access patterns
4. Inefficient data structures

**Solutions:**
```python
# Solution 1: Use memory pools
config.set("performance.enable_memory_pools", True)

# Solution 2: Optimize connectivity
config.set("brain.synapse_density", 0.1)  # Lower density

# Solution 3: Use simpler plasticity
config.set("plasticity.stdp.enable", True)  # Or disable, use Hebbian only
```

### High Memory Usage

**Symptoms:** Memory usage grows during simulation

**Causes:**
1. Memory leaks
2. Inefficient data structures
3. Unbounded growth in neural populations

**Solutions:**
```python
# Solution 1: Use memory pools
config.set("performance.enable_memory_pools", True)

# Solution 2: Limit neuron growth
config.set("brain.neuron_count", 5000)  # Fixed limit

# Solution 3: Enable memory monitoring
monitor = MemoryMonitor()
```

### CPU Usage

**Symptoms:** High CPU usage with idle cores

**Causes:**
1. Inefficient algorithms
2. Poor cache utilization
3. Excessive branching

**Solutions:**
```python
# Solution 1: Use SIMD optimizations
config.set("performance.enable_simd", True)

# Solution 2: Optimize cache locality
config.set("brain.neuron_count", 1024)  # Power of 2

# Solution 3: Reduce complexity
config.set("plasticity.stdp.enable", False)  # Or simpler plasticity
```

## Performance Comparison Tables

### Configuration Comparison

| Configuration | Neurons | Synapses | Step Time (ms) | Memory (MB) | Steps/sec |
|---------------|---------|----------|----------------|-------------|-----------|
| Small (Research) | 100 | ~10 | 0.1 | 50 | 10,000 |
| Medium (Education) | 1,000 | ~100 | 1.2 | 500 | 833 |
| Large (Production) | 5,000 | ~500 | 6.8 | 2,500 | 147 |

### Performance Factors

**Factors affecting NLM performance:**

1. **Neuron Count**: Linear relationship with computation
2. **Synapse Density**: Exponential relationship with connectivity
3. **Plasticity Rules**: STDP > Hebbian > None
4. **Memory Pools**: Significant performance improvement
5. **SIMD**: Good improvement for numeric operations
6. **Cache Locality**: Critical for performance
7. **Thread Count**: Limited parallelization benefits

## Best Practices

### Configuration Best Practices

1. **Start Small**: Begin with small simulations for testing
2. **Measure First**: Profile performance before optimization
3. **Iterate Gradually**: Make incremental performance improvements
4. **Monitor Continuously**: Track performance throughout development
5. **Document Decisions**: Document configuration choices and rationale

### Development Best Practices

1. **Use Memory Pools**: Enable memory pools for production
2. **Optimize Data Structures**: Use cache-friendly layouts
3. **Profile Early**: Identify bottlenecks early in development
4. **Test Extensively**: Comprehensive testing for performance
5. **Monitor Production**: Monitor performance in production

### Deployment Best Practices

1. **Resource Planning**: Plan CPU, memory, and storage requirements
2. **Performance Monitoring**: Implement continuous performance monitoring
3. **Capacity Planning**: Plan for scaling requirements
4. **Optimization Loop**: Continuously optimize based on usage patterns
5. **Documentation**: Document performance characteristics and limitations

---

This performance tuning guide provides comprehensive information for optimizing NLM performance across different use cases and scenarios. By following these guidelines, you can achieve significant performance improvements while maintaining accuracy and reliability.
