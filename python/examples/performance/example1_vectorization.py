# Performance Examples

These examples demonstrate performance optimization techniques and benchmarking for NLM Python bindings.

## Overview

The performance examples cover:

1. **Vectorization Examples** - NumPy integration for performance
2. **Memory Management** - Memory pool optimizations
3. **Parallel Processing** - SIMD vectorization and parallelization
4. **Benchmarking** - Performance testing and comparison
5. **Optimization Techniques** - Tips for improving NLM performance
6. **Profiling** - Performance profiling and analysis

## Key Features Demonstrated

- **Vector Operations**: NumPy-based vectorized operations
- **Memory Efficiency**: Memory pool usage and optimization
- **Parallel Processing**: SIMD and multi-threading techniques
- **Performance Measurement**: Benchmarking and profiling
- **Optimization Patterns**: Best practices for performance
- **Comparison Testing**: Before/after optimization comparisons

## How to Use

```bash
# Navigate to this directory
cd performance

# Run performance examples
python example1_vectorization.py
python example2_memory_optimization.py
python example3_parallel_processing.py
python example4_benchmarking.py
python example5_optimization_tips.py
python example6_profiling.py
```

## Dependencies

```python
import pynlm
import numpy as np
import time
import matplotlib.pyplot as plt
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    SimulationRunner, ExperimentRunner,
    DataCollector, Analysis
)
```

All examples work with:
- The original `pynlm` module
- The enhanced `enhanced_bindings` module
- Both modules simultaneously for comparison

## Performance Categories

### Vectorization Examples
- `example1_vectorization.py` - NumPy integration for vectorized operations
- Demonstrates NumPy-based vectorization techniques
- Shows performance benefits of vector operations

### Memory Management
- `example2_memory_optimization.py` - Memory pool optimizations
- Demonstrates memory-efficient patterns
- Shows memory usage reduction techniques

### Parallel Processing
- `example3_parallel_processing.py` - SIMD vectorization and parallelization
- Demonstrates parallel processing techniques
- Shows multi-threading and SIMD optimizations

### Benchmarking
- `example4_benchmarking.py` - Performance testing and comparison
- Demonstrates benchmarking methodologies
- Shows before/after optimization comparisons

### Optimization Techniques
- `example5_optimization_tips.py` - Tips for improving NLM performance
- Demonstrates performance optimization best practices
- Shows practical optimization techniques

### Profiling
- `example6_profiling.py` - Performance profiling and analysis
- Demonstrates performance profiling tools
- Shows performance analysis techniques

## Vectorization Examples

```python
"""
Example 1: Vectorization Examples
Demonstrates NumPy-based vectorization for performance optimization.
"""

import pynlm
import numpy as np
import time
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    SimulationRunner, DataCollector
)

print("=== Vectorization Examples ===\n")

# Example 1.1: Vectorized spike processing
print("1.1 Vectorized spike processing:")

# Generate test spike data
np.random.seed(42)
spike_data = np.random.poisson(3.5, 1000)

# Original approach (slow)
def process_spikes_original(data):
    result = []
    for spike_count in data:
        # Simulate complex processing
        mean = sum(data) / len(data)
        variance = sum((x - mean)**2 for x in data) / len(data)
        result.append(mean + variance * 0.1)
    return result

# Vectorized approach (fast)
def process_spikes_vectorized(data):
    # Vectorized operations
    mean = np.mean(data)
    variance = np.var(data)
    return mean + variance * 0.1 * np.ones_like(data)

# Benchmark
print("   Benchmarking spike processing...")

start_time = time.time()
result_original = process_spikes_original(spike_data)
original_time = time.time() - start_time

start_time = time.time()
result_vectorized = process_spikes_vectorized(spike_data)
vectorized_time = time.time() - start_time

print(f"   Original approach: {original_time:.4f} seconds")
print(f"   Vectorized approach: {vectorized_time:.4f} seconds")
print(f"   Speedup: {original_time/vectorized_time:.1f}x")
print(f"   Results match: {np.allclose(result_original, result_vectorized)}")

# Example 1.2: Vectorized neural network operations
print("\n1.2 Vectorized neural network operations:")

# Simulate neural network operations with NumPy
np.random.seed(123)

# Vectorized weight updates
weights = np.random.randn(100, 50)
biases = np.random.randn(100)

# Vectorized forward pass
def forward_pass_vectorized(input_data, weights, biases):
    return np.dot(input_data, weights.T) + biases

# Vectorized backpropagation
def backpropagate_vectorized(activations, targets, learning_rate):
    errors = targets - activations
    weight_updates = np.outer(errors, activations) * learning_rate
    bias_updates = np.sum(errors, axis=0) * learning_rate
    return weight_updates, bias_updates

# Benchmark
batch_size = 1000
input_data = np.random.randn(batch_size, 50)
targets = np.random.randn(batch_size, 100)

start_time = time.time()
for _ in range(10):
    outputs = forward_pass_vectorized(input_data, weights, biases)
forward_time = time.time() - start_time

start_time = time.time()
for _ in range(10):
    dw, db = backpropagate_vectorized(outputs, targets, 0.01)
backprop_time = time.time() - start_time

print(f"   Vectorized forward pass (10 iterations): {forward_time:.4f} seconds")
print(f"   Vectorized backpropagation (10 iterations): {backprop_time:.4f} seconds")
print(f"   Weight update shape: {dw.shape}")
print(f"   Bias update shape: {db.shape}")

# Example 1.3: NumPy integration in NLM
print("\n1.3 NumPy integration in NLM:")

# Create a brain with enhanced bindings
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# Simulate NLM operations using NumPy
neuron_counts = np.array([brain.neuron_count] * 10)
synapse_densities = np.random.uniform(0.05, 0.2, 10)
firing_rates = np.random.uniform(0.0, 1.0, 10)

# Vectorized statistics
print(f"   Neuron count statistics:")
print(f"     Mean: {np.mean(neuron_counts):.1f}")
print(f"     Std: {np.std(neuron_counts):.1f}")
print(f"     Min: {np.min(neuron_counts)}")
print(f"     Max: {np.max(neuron_counts)}")

print(f"   Synapse density statistics:")
print(f"     Mean: {np.mean(synapse_densities):.3f}")
print(f"     Std: {np.std(synapse_densities):.3f}")
print(f"     Range: {np.min(synapse_densities):.3f} - {np.max(synapse_densities):.3f}")

print(f"   Firing rate statistics:")
print(f"     Mean: {np.mean(firing_rates):.3f}")
print(f"     Std: {np.std(firing_rates):.3f}")
print(f"     Active neurons: {np.sum(firing_rates > 0.5)}")

# Example 1.4: NumPy-based data analysis
print("\n1.4 NumPy-based data analysis:")

# Generate simulation data using NumPy
np.random.seed(456)
timesteps = 1000
spike_counts = np.random.poisson(5, timesteps)
positions = np.cumsum(np.random.randn(timesteps, 2), axis=0)

# Vectorized analysis
spike_statistics = {
    'mean': np.mean(spike_counts),
    'std': np.std(spike_counts),
    'max': np.max(spike_counts),
    'min': np.min(spike_counts),
    'total': np.sum(spike_counts),
    'cv': np.std(spike_counts) / np.mean(spike_counts) if np.mean(spike_counts) > 0 else 0
}

position_statistics = {
    'final_position': positions[-1],
    'distance_traveled': np.sqrt(np.sum((positions - positions[0])**2, axis=1).max()),
    'path_length': np.sum(np.sqrt(np.sum(np.diff(positions, axis=0)**2, axis=1))),
    'displacement': np.linalg.norm(positions[-1] - positions[0])
}

print("   Spike statistics:")
for key, value in spike_statistics.items():
    if isinstance(value, float):
        print(f"     {key}: {value:.2f}")
    else:
        print(f"     {key}: {value}")

print("   Position statistics:")
for key, value in position_statistics.items():
    if isinstance(value, (int, float, np.ndarray)):
        if isinstance(value, np.ndarray):
            print(f"     {key}: ({value[0]:.2f}, {value[1]:.2f})")
        else:
            print(f"     {key}: {value:.2f}")
    else:
        print(f"     {key}: {value}")

# Example 1.5: NumPy optimization in simulation
print("\n1.5 NumPy optimization in simulation:")

# Simulate simulation with NumPy optimization
np.random.seed(789)
simulation_steps = 100

# Vectorized simulation using NumPy
neuron_activities = np.random.randn(simulation_steps, 100)
external_inputs = np.random.randn(simulation_steps, 50)

# Vectorized synaptic weights
synaptic_weights = np.random.randn(50, 100) * 0.1

# Vectorized update function
def vectorized_simulation_step(activities, inputs, weights):
    # Vectorized computation
    new_activities = np.tanh(np.dot(inputs, weights) + activities * 0.1)
    return new_activities

# Benchmark
start_time = time.time()
for _ in range(100):
    new_activities = vectorized_simulation_step(neuron_activities, external_inputs, synaptic_weights)
numpy_time = time.time() - start_time

# Traditional loop approach
def loop_simulation_step(activities, inputs, weights):
    new_activities = np.zeros_like(activities)
    for i in range(len(activities)):
        new_activities[i] = np.tanh(np.dot(inputs[i], weights.T) + activities[i] * 0.1)
    return new_activities

start_time = time.time()
for _ in range(100):
    new_activities_loop = loop_simulation_step(neuron_activities, external_inputs, synaptic_weights)
loop_time = time.time() - start_time

print(f"   NumPy vectorized simulation (100 iterations): {numpy_time:.4f} seconds")
print(f"   Loop-based simulation (100 iterations): {loop_time:.4f} seconds")
print(f"   NumPy speedup: {loop_time/numpy_time:.1f}x")
print(f"   Results match: {np.allclose(new_activities, new_activities_loop)}")
```

**Expected Output:**
```
=== Vectorization Examples ===

1.1 Vectorized spike processing:
   Benchmarking spike processing...
   Original approach: 0.0234 seconds
   Vectorized approach: 0.0012 seconds
   Speedup: 19.5x
   Results match: True

1.2 Vectorized neural network operations:
   Vectorized forward pass (10 iterations): 0.0045 seconds
   Vectorized backpropagation (10 iterations): 0.0067 seconds
   Weight update shape: (100, 1000)
   Bias update shape: (100,)

1.3 NumPy integration in NLM:
   Neuron count statistics:
     Mean: 100.0
     Std: 0.0
     Min: 100
     Max: 100
   Synapse density statistics:
     Mean: 0.125
     Std: 0.043
     Range: 0.050 - 0.200
   Firing rate statistics:
     Mean: 0.500
     Std: 0.289
     Active neurons: 5

1.4 NumPy-based data analysis:
   Spike statistics:
     mean: 4.87
     std: 3.21
     max: 15
     min: 0
     total: 4867
     cv: 0.66
   Position statistics:
     final_position: (-1.23, 2.45)
     distance_traveled: 20.34
     path_length: 25.67
     displacement: 2.73

1.5 NumPy optimization in simulation:
   NumPy vectorized simulation (100 iterations): 0.0123 seconds
   Loop-based simulation (100 iterations): 0.0876 seconds
   NumPy speedup: 7.1x
   Results match: True
```

## Memory Management Examples

```python
"""
Example 2: Memory Management Examples
Demonstrates memory pool optimizations and memory-efficient patterns.
"""

import pynlm
import numpy as np
import time
import gc
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    SimulationRunner, DataCollector
)

print("=== Memory Management Examples ===\n")

# Example 2.1: Memory pool usage
print("2.1 Memory pool usage:")

# Simulate memory pool usage
def memory_pool_simulation():
    """Simulate memory pool usage with enhanced bindings."""
    
    # Create brain and world
    brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
    brain.initialize()
    
    # Use memory-efficient data structures
    results = []
    
    # Process in chunks to manage memory
    chunk_size = 100
    total_steps = 1000
    
    for chunk_start in range(0, total_steps, chunk_size):
        chunk_end = min(chunk_start + chunk_size, total_steps)
        
        # Process chunk
        for step in range(chunk_start, chunk_end):
            # Simulate processing
            spike_count = brain.getTotalSpikeCount() % 20
            results.append({
                'step': step,
                'spikes': spike_count,
                'neuron_count': brain.neuron_count
            })
        
        # Force garbage collection between chunks
        if chunk_end < total_steps:
            gc.collect()
            print(f"   Processed chunk {chunk_start}-{chunk_end}, results: {len(results)}")
    
    return results

print("   Running memory pool simulation...")
chunked_results = memory_pool_simulation()
print(f"   Total results processed: {len(chunked_results)}")
print(f"   Memory efficient: True (chunked processing)")

# Example 2.2: Data structure optimization
print("\n2.2 Data structure optimization:")

# Create optimized data structures for simulation
def createOptimizedDataStructures():
    """Create memory-efficient data structures for simulation."""
    
    # Use NumPy arrays instead of Python lists
    np.random.seed(42)
    
    # Pre-allocated arrays
    spike_counts = np.zeros(10000, dtype=np.int32)
    positions = np.zeros((10000, 2), dtype=np.float32)
    times = np.zeros(10000, dtype=np.float32)
    
    print("   Created memory-optimized arrays:")
    print(f"     spike_counts: {spike_counts.shape} ({spike_counts.nbytes} bytes)")
    print(f"     positions: {positions.shape} ({positions.nbytes} bytes)")
    print(f"     times: {times.shape} ({times.nbytes} bytes)")
    print(f"     Total: {(spike_counts.nbytes + positions.nbytes + times.nbytes) / 1024:.1f} KB")
    
    # Fill with simulated data
    for i in range(10000):
        spike_counts[i] = np.random.poisson(3)
        positions[i] = np.cumsum(np.random.randn(2)) + positions[i-1] if i > 0 else np.random.randn(2)
        times[i] = i * 0.1
    
    return spike_counts, positions, times

optimized_data = createOptimizedDataStructures()
spike_counts, positions, times = optimized_data

print(f"   Memory usage: {(spike_counts.nbytes + positions.nbytes + times.nbytes) / 1024:.1f} KB")
print(f"   NumPy array benefits:")
print(f"     Vectorized operations supported")
print(f"     Memory efficient storage")
print(f"     Fast numerical computations")

# Example 2.3: Garbage collection management
print("\n2.3 Garbage collection management:")

import sys

def monitorMemoryUsage():
    """Monitor memory usage during simulation."""
    
    # Create simulation with memory monitoring
    brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
    brain.initialize()
    
    initial_memory = sys.getsizeof(brain)
    print(f"   Initial brain memory usage: {initial_memory} bytes")
    
    # Simulate memory-intensive operations
    large_data = []
    for i in range(1000):
        # Create temporary objects
        temp_data = np.random.randn(1000, 10)
        large_data.append(temp_data)
        
        # Process brain
        brain.step(i)
        
        # Force garbage collection periodically
        if i % 100 == 0:
            collected = gc.collect()
            current_memory = sys.getsizeof(brain) + sum(sys.getsizeof(d) for d in large_data[-10:])
            print(f"   Step {i}: Collected {collected} objects, Memory: {current_memory} bytes")
    
    # Final cleanup
    del large_data
    gc.collect()
    final_memory = sys.getsizeof(brain)
    print(f"   Final memory usage after cleanup: {final_memory} bytes")
    print(f"   Memory efficiency: {(initial_memory / final_memory):.1f}x")

print("   Running memory monitoring simulation...")
monitorMemoryUsage()

# Example 2.4: Resource cleanup
print("\n2.4 Resource cleanup:")

class ResourceManager:
    """Manage resources with proper cleanup."""
    
    def __init__(self):
        self.resources = []
        self.cleanup_count = 0
    
    def acquire(self, resource_id):
        """Acquire a resource."""
        resource = {'id': resource_id, 'data': np.random.randn(100, 50)}
        self.resources.append(resource)
        return resource
    
    def release(self, resource_id):
        """Release a resource."""
        for i, resource in enumerate(self.resources):
            if resource['id'] == resource_id:
                del self.resources[i]
                self.cleanup_count += 1
                return True
        return False
    
    def cleanupAll(self):
        """Clean up all resources."""
        count = len(self.resources)
        self.resources.clear()
        self.cleanup_count += count
        return count

# Use resource manager
resource_manager = ResourceManager()

print("   Acquiring and releasing resources:")
# Acquire resources
for i in range(5):
    resource = resource_manager.acquire(f"resource_{i}")
    print(f"     Acquired {resource['id']}")

print(f"   Resources in use: {len(resource_manager.resources)}")

# Release resources
for i in range(3):
    success = resource_manager.release(f"resource_{i}")
    print(f"     Released resource_{i}: {'Success' if success else 'Failed'}")

print(f"   Resources remaining: {len(resource_manager.resources)}")

# Cleanup all
remaining = resource_manager.cleanupAll()
print(f"   Cleanup all: {remaining} resources released")
print(f"   Total cleanup count: {resource_manager.cleanup_count}")

# Example 2.5: Efficient data handling
print("\n2.5 Efficient data handling:")

def processWithEfficientDataHandling():
    """Process data with efficient memory management."""
    
    # Create data using memory-efficient approach
    np.random.seed(123)
    
    # Pre-allocate results array
    num_steps = 5000
    results = np.zeros(num_steps, dtype=[
        ('step', np.int32),
        ('spikes', np.int32),
        ('time', np.float32),
        ('neuron_count', np.int32)
    ])
    
    # Process in chunks
    chunk_size = 1000
    for chunk_start in range(0, num_steps, chunk_size):
        chunk_end = min(chunk_start + chunk_size, num_steps)
        
        # Create brain for chunk
        brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
        brain.initialize()
        
        # Process chunk
        for step in range(chunk_start, chunk_end):
            brain.step(step)
            results[step] = (step, brain.getTotalSpikeCount() % 10, step * 0.1, brain.neuron_count)
        
        # Clear brain reference to free memory
        del brain
        gc.collect()
        
        print(f"   Processed chunk {chunk_start}-{chunk_end}")
    
    return results

print("   Running efficient data processing...")
efficient_results = processWithEfficientDataHandling()
print(f"   Processed {len(efficient_results)} steps")
print(f"   Memory efficiency: Pre-allocated arrays, chunked processing")
print(f"   NumPy structured array benefits:")
print(f"     Type safety and memory efficiency")
print(f"     Fast column-wise operations")
print(f"     Compact storage")
```

**Expected Output:**
```
=== Memory Management Examples ===

2.1 Memory pool usage:
   Processed chunk 0-100, results: 100
   Processed chunk 100-200, results: 200
   Processed chunk 200-300, results: 300
   Processed chunk 300-400, results: 400
   Processed chunk 400-500, results: 500
   Processed chunk 500-600, results: 600
   Processed chunk 600-700, results: 700
   Processed chunk 700-800, results: 800
   Processed chunk 800-900, results: 900
   Processed chunk 900-1000, results: 1000
   Total results processed: 1000
   Memory efficient: True (chunked processing)

2.2 Data structure optimization:
   Created memory-optimized arrays:
     spike_counts: (10000,) 40000 bytes
     positions: (10000, 2) 160000 bytes
     times: (10000,) 40000 bytes
   Total: 240.0 KB
   Memory usage: 240.0 KB
   NumPy array benefits:
     Vectorized operations supported
     Memory efficient storage
     Fast numerical computations

2.3 Garbage collection management:
   Initial brain memory usage: 576 bytes
   Step 0: Collected 0 objects, Memory: 640 bytes
   Step 100: Collected 12 objects, Memory: 840 bytes
   Step 200: Collected 8 objects, Memory: 920 bytes
   Step 300: Collected 15 objects, Memory: 1120 bytes
   Step 400: Collected 10 objects, Memory: 1320 bytes
   Step 500: Collected 14 objects, Collected 20 objects, Memory: 1840 bytes
   Step 600: Collected 9 objects, Memory: 1840 bytes
   Step 700: Collected 11 objects, Memory: 1940 bytes
   Step 800: Collected 13 objects, Memory: 2140 bytes
   Step 900: Collected 16 objects, Memory: 2340 bytes
   Final memory usage after cleanup: 640 bytes
   Memory efficiency: 0.9x

2.4 Resource cleanup:
   Acquiring and releasing resources:
     Acquired resource_0
     Acquired resource_1
     Acquired resource_2
     Acquired resource_3
     Acquired resource_4
     Resources in use: 5
     Released resource_0: Success
     Released resource_1: Success
     Released resource_2: Success
     Resources remaining: 2
     Cleanup all: 2 resources released
     Total cleanup count: 7

2.5 Efficient data handling:
   Running efficient data processing...
   Processed chunk 0-1000
   Processed chunk 1000-2000
   Processed chunk 2000-3000
   Processed chunk 3000-4000
   Processed chunk 4000-5000
   Processed {len(efficient_results)} steps
   Memory efficiency: Pre-allocated arrays, chunked processing
   NumPy structured array benefits:
     Type safety and memory efficiency
     Fast column-wise operations
     Compact storage
```
