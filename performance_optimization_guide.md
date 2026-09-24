# NLM Performance Optimization Guidelines

## Overview

This document provides comprehensive guidance for optimizing NLM performance for neural simulation workloads. It covers C++ configuration, Python API usage, and system-level optimizations.

## Quick Performance Tips

### 1. Configuration Optimization

```python
import pynlm

# Optimal configuration for performance
config = pynlm.createDefaultConfig()

# Balance between accuracy and speed
config.set("brain.neuron_count", 500)      # Not too high
config.set("brain.synapse_density", 0.05)   # Moderate connectivity
config.set("brain.initial_weight_mean", 0.3)  # Optimized initial weights

# Enable performance features
config.set("performance.simd.enabled", True)
config.set("performance.event_queue.enabled", True)
config.set("performance.memory_pool.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Simulation Settings

```python
# Optimal simulation parameters
for step in range(1000):
    # Use larger time steps when possible
    timestep = 0.1  # 10ms steps are often optimal
    brain.step(step, timestep)
    
    # Monitor performance metrics
    if step % 100 == 0:
        firing_rate = brain.getAverageFiringRate()
        print(f"Step {step}: {firing_rate:.1f} Hz")
```

### 3. Memory Management

```python
# Implement checkpointing for long simulations
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save checkpoints periodically
for step in range(5000):
    brain.step(step)
    
    if step % 1000 == 0:
        checkpoint_file = f"checkpoint_step_{step}.bin"
        if brain.save(checkpoint_file):
            print(f"✓ Checkpoint saved: {checkpoint_file}")
        else:
            print(f"✗ Failed to save checkpoint")
```

## C++ Configuration Optimization

### Build Configuration

```bash
# Optimized release build
cmake .. -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp" \
          -DNLM_ENABLE_SIMD=ON \
          -DNLM_ENABLE_EVENT_QUEUE=ON \
          -DNLM_ENABLE_MEMORY_POOL=ON

make -j$(nproc)
```

### Key CMake Options

| Option | Description | Recommended Value |
|--------|-------------|-------------------|
| `CMAKE_BUILD_TYPE` | Build type | `Release` |
| `NLM_ENABLE_SIMD` | Enable SIMD optimizations | `ON` |
| `NLM_ENABLE_EVENT_QUEUE` | Enable event-driven simulation | `ON` |
| `NLM_ENABLE_MEMORY_POOL` | Enable memory pools | `ON` |
| `NLM_OPTIMIZATION_LEVEL` | Optimization level | `3` |

## Python API Optimization

### 1. Batch Operations

```python
import pynlm
import time

# Batch sensory processing for better performance
sensory_inputs = []
for _ in range(100):
    world.update(0.1)
    sensory_inputs.append(world.getSensoryPercept())

# Process all at once
agent = pynlm.createAgentBrain(brain)
for percept in sensory_inputs:
    agent.processSensoryInput(percept)
```

### 2. Reduce Method Calls

```python
# Cache frequently accessed properties
agent = pynlm.createAgentBrain(brain)
brain_ref = agent.getBrain()

# Use cached references instead of repeated method calls
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Use cached brain reference
    brain_ref.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. Vectorized Operations

```python
import numpy as np

# Vectorized sensory processing
perception_batch = []
for _ in range(50):
    world.update(0.1)
    perception_batch.append(world.getSensoryPercept())

# Process in batch when possible
if perception_batch:
    for percept in perception_batch:
        agent.processSensoryInput(percept)
```

## Memory Optimization

### 1. Memory Pooling

```python
# Enable memory pooling in config
config.set("performance.memory_pool.enabled", True)
config.set("performance.memory_pool.block_size", 4096)
config.set("performance.memory_pool.max_blocks", 1024)
```

### 2. Event Queue Optimization

```python
# Enable event queue for large networks
config.set("performance.event_queue.enabled", True)
config.set("performance.event_queue.max_events", 100000)
config.set("performance.event_queue.batch_size", 100)
```

## Neuromodulation Optimization

### 1. Selective Neuromodulation

```python
# Only apply neuromodulation when needed
if should_apply_learning(step):
    reward = calculate_reward(action, result)
    agent.applyRewardModulation(reward, 0.0)
else:
    # Skip neuromodulation computation for faster simulation
    pass
```

### 2. Neuromodulator Pruning

```python
# Disable unused neuromodulators for performance
agent.enable_reward_modulation(True)
agent.enable_curiosity(False)  # Disabled if not used
agent.enable_development(False)  # Disabled if not used
```

## Development Stage Optimization

### 1. Critical Period Handling

```python
# Optimize during critical periods
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
    # Use faster but less accurate plasticity
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.learning_rate", 0.005)
else:
    # Use normal learning rates
    pass
```

### 2. Plasticity Modulation

```python
# Reduce plasticity in stable phases
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Adult:
    config.set("plasticity.stdp.learning_rate", 0.0001)
    config.set("plasticity.structural.enabled", False)
```

## Profiling and Benchmarking

### 1. Performance Monitoring

```python
import pynlm
import time
import cProfile
import pstats

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile simulation
profiler = cProfile.Profile()
profiler.enable()

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

profiler.disable()

print(f"Simulation time: {end_time - start_time:.2f}s")
print(f"Average step time: {(end_time - start_time)/1000*1000:.2f}ms")

# Generate profiling report
stats = pstats.Stats(profiler)
stats.sort_stats('cumulative')
stats.print_stats(20)  # Top 20 time-consuming functions
```

### 2. Memory Profiling

```python
import pynlm
from memory_profiler import profile

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

@profile
def run_simulation():
    for step in range(500):
        brain.step(step)

if __name__ == "__main__":
    run_simulation()
```

## System-Level Optimizations

### 1. CPU Optimization

```bash
# CPU-specific optimizations
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp -mtune=native"

# Use all available cores
make -j$(nproc)

# Set CPU affinity for simulation
export OMP_NUM_THREADS=$(nproc)
export MKL_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1
```

### 2. Memory Optimization

```bash
# Memory limit for simulation
ulimit -v 4294967296  # 4GB virtual memory

# Swap optimization
@echo "vm.swappiness=10" | sudo tee -a /etc/sysctl.conf
@echo "vm.vfs_cache_pressure=50" | sudo tee -a /etc/sysctl.conf
sudo sysctl -p
```

### 3. I/O Optimization

```python
# Minimize I/O operations
# Use checkpointing instead of frequent saves
# Batch file operations when possible
import os
os.makedirs("checkpoints", exist_ok=True)

# Use efficient file formats
# Consider binary formats over text
```

## Scaling Guidelines

### 1. Scaling with Network Size

| Neurons | Recommended Steps | Time Step | Memory Usage |
|---------|-------------------|-----------|--------------|
| 100     | 1000-5000        | 0.1       | Low          |
| 500     | 500-2000         | 0.1       | Medium       |
| 1000    | 200-1000         | 0.05      | High         |
| 5000    | 50-200           | 0.01      | Very High    |

### 2. Scaling with Simulation Length

```python
# Adjust for long simulations
config = pynlm.createDefaultConfig()

# For very long simulations (10k+ steps)
if target_steps > 10000:
    config.set("brain.synapse_density", 0.01)  # Lower connectivity
    config.set("plasticity.structural.enabled", False)  # Disable structural plasticity
    config.set("development.enable", False)  # Skip development
    print("Note: Some features disabled for long simulation performance")
```

## Troubleshooting Performance Issues

### Common Problems and Solutions

#### 1. High CPU Usage
**Problem**: Simulation CPU usage consistently > 80%
**Solution**:
- Use SIMD optimizations
- Enable event-driven processing
- Reduce simulation complexity (fewer neurons/synapses)
- Use smaller time steps

#### 2. Memory Issues
**Problem**: Memory usage exceeds available RAM
**Solution**:
- Enable memory pooling
- Use checkpointing
- Reduce network size
- Disable unused features

#### 3. Slow Simulation
**Problem**: Step time > 1ms
**Solution**:
- Enable all performance optimizations
- Use faster file I/O
- Optimize data structures
- Use event-driven processing

### Performance Monitoring Commands

```bash
# Monitor system resources during simulation
watch -n 1 'top -p $(pgrep nlm_brain) | head -20'

# Monitor memory usage
watch -n 1 'free -h; ps aux | grep nlm_brain | grep -v grep'

# Monitor CPU usage
watch -n 1 'mpstat 1 1 | tail -n +4'
```

## Best Practices Summary

1. **Start with small networks** and scale up gradually
2. **Use all performance optimizations** in production
3. **Implement checkpointing** for long simulations
4. **Profile regularly** to identify bottlenecks
5. **Monitor system resources** during simulation
6. **Use vectorized operations** when possible
7. **Disable unused features** to reduce overhead
8. **Optimize for your target hardware** (CPU, memory)
9. **Document performance characteristics** for your use case
10. **Plan for scaling** when designing simulations

## References

- NLM Performance White Paper
- PyBind11 Performance Guidelines
- C++ Optimization Best Practices
- Neural Network Simulation Optimization

This document should be updated regularly as performance optimizations are discovered and implemented.
