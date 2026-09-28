# NLM Performance Documentation

## Overview

This document provides performance guidance and benchmarking information for the NLM (Neural Learning Machine) codebase. It includes recommendations for optimizing performance, understanding performance characteristics, and measuring simulation speed.

## Performance Optimization Guide

### 1. Build Configuration

#### Recommended Build Flags
```bash
# Optimized build for maximum performance
mkdir build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_PERF_OPT=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"
make -j$(nproc)
```

#### Performance Build Options
- `ENABLE_PERF_OPT=ON`: Enable compiler optimizations and loop unrolling
- `CMAKE_CXX_FLAGS`: Additional compiler-specific optimizations
- `-j$(nproc)`: Parallel build for faster compilation

### 2. Configuration Optimization

#### High-Performance Configuration
Use the `performance.cfg` template for maximum simulation speed:

```bash
./nlm --config=configs/performance.cfg
```

#### Key Performance Settings
```
# Reduce simulation parameters for speed
simulation_timestep = 0.002      # Larger timestep = faster simulation
neuron_count = 5000              # Controlled larger network
region_count = 4                  # Parallel processing regions
connection_probability = 0.05    # Sparse connectivity

# Disable expensive features
enable_structural_plasticity = false
enable_reward_modulation = false
enable_curiosity = false
enable_prediction = false
enable_development = false

# Lightweight logging
log_level = WARNING
log_to_file = false
```

### 3. Runtime Performance Optimization

#### Memory Management
- **Memory Pools**: Enable memory pools for object allocation (default: ON)
- **Event Queues**: Adjust event queue sizes based on simulation requirements
- **Cache Locality**: The NLM architecture is designed for good cache locality

#### Simulation Parameters
- **Timestep Size**: Larger timesteps increase speed but reduce accuracy
- **Network Size**: More neurons = more computation, but better statistical results
- **Connectivity**: Sparse connectivity (0.05-0.1) balances performance and behavior

### 4. Performance Benchmarking

#### Built-in Benchmarking
```bash
# Run performance benchmarks
./benchmark

# Run specific performance tests
./nlm_phase6_demo --benchmark
```

#### Custom Benchmarking
```cpp
#include "nlm/experiments/Metrics.hpp"

// Create metrics collector
MetricsCollector collector;

// Run benchmark
simulation_time = collector.benchmark("NLM Simulation", [&]() {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (SimulationStep step = 0; step < num_steps; ++step) {
        brain->step(step, timestep * step);
    }
});

// Report results
collector.report(std::cout);
```

### 5. Performance Monitoring

#### Runtime Metrics
Track these key performance indicators:

1. **Simulation Speed**: Steps per second
2. **Memory Usage**: Peak memory consumption
3. **CPU Utilization**: Processor time vs. wall time
4. **Network Activity**: Neuron firing rates and spike counts
5. **Plasticity Overhead**: Time spent on synaptic updates

#### Performance Profiling
```bash
# Use Linux profiling tools
gprof ./nlm_phase6_demo profile.out
cppcheck --enable=performance ./src
```

### 6. Scalability Analysis

#### Linear Scaling
Measure performance as network size increases:

```
Configuration Analysis:
- neuron_count: 500  ->  100 steps/sec
- neuron_count: 1000 ->   80 steps/sec  (1.25x slower per neuron)
- neuron_count: 2000 ->   50 steps/sec  (2.0x slower per neuron)
```

#### Weak Scaling
Measure performance as work increases with problem size:

```
Performance Comparison:
- 500 neurons, 100 steps:  100 steps/sec
- 1000 neurons, 200 steps:  95 steps/sec
- 2000 neurons, 400 steps:  90 steps/sec
```

### 7. Performance Recommendations

#### For Maximum Speed
```
# Use these settings for maximum performance
- neuron_count: 5000-10000
- region_count: 4-8
- connection_probability: 0.02-0.05
- enable_structural_plasticity: false
- enable_reward_modulation: false
- enable_curiosity: false
- enable_prediction: false
- enable_development: false
- log_level: WARNING
- log_to_file: false
```

#### For Research Accuracy
```
# Use these settings for scientific accuracy
- neuron_count: 1000-5000
- region_count: 2-4
- connection_probability: 0.1-0.2
- enable_structural_plasticity: true
- enable_reward_modulation: true
- enable_curiosity: true
- enable_prediction: true
- enable_development: true
- log_level: INFO
- log_to_file: true
```

### 8. Performance Troubleshooting

#### Common Performance Issues and Solutions

1. **Memory Exhaustion**
```
Problem: Running out of memory with large networks
Solution:
- Reduce neuron_count
- Enable memory pools (default: ON)
- Monitor memory usage with tools like valgrind
```

2. **Slow Plasticity Updates**
```
Problem: Plasticity calculations taking too long
Solution:
- Disable structural plasticity for speed
- Reduce plasticity_learning_rate
- Use larger timesteps
```

3. **High CPU Usage**
```
Problem: CPU usage consistently high
Solution:
- Check for infinite loops in plasticity
- Monitor spike generation patterns
- Ensure proper termination conditions
```

4. **Slow Simulation Steps**
```
Problem: Steps taking longer than expected
Solution:
- Profile with gprof or similar tools
- Check for inefficient algorithms
- Consider disabling expensive features
```

### 9. Performance Testing Framework

#### Performance Test Suite
```bash
# Run performance tests
./scripts/run_performance_tests.sh

# Generate performance report
./scripts/generate_performance_report.sh
```

#### Performance Test Script
```bash
#!/bin/bash
# run_performance_tests.sh

# Set up test environment
mkdir -p performance_results
test_start_time=$(date +%s)

# Run different configuration sizes
./benchmark --neurons 500 --regions 1 --steps 1000 > performance_results/500_neurons.txt
./benchmark --neurons 1000 --regions 2 --steps 1000 > performance_results/1000_neurons.txt
./benchmark --neurons 2000 --regions 4 --steps 1000 > performance_results/2000_neurons.txt

# Generate report
./scripts/generate_performance_report.sh

test_end_time=$(date +%s)
echo "Performance tests completed in $((test_end_time - test_start_time)) seconds"
```

### 10. Performance Optimization Checklist

#### Build System Optimization
- [ ] Enable performance optimizations (ENABLE_PERF_OPT=ON)
- [ ] Use -O3 compiler flags
- [ ] Enable link-time optimization (flto)
- [ ] Use -march=native for architecture-specific optimizations

#### Configuration Optimization
- [ ] Use performance.cfg for speed-critical applications
- [ ] Disable expensive plasticity features
- [ ] Reduce logging overhead
- [ ] Adjust simulation parameters for target performance

#### Runtime Optimization
- [ ] Monitor memory usage
- [ ] Profile CPU usage with performance tools
- [ ] Optimize network size for target hardware
- [ ] Tune simulation parameters for desired accuracy/speed trade-off

#### Code Optimization
- [ ] Profile with gprof or similar tools
- [ ] Check for inefficient algorithms
- [ ] Optimize data structures for cache locality
- [ ] Consider parallelization for large networks

## Conclusion

Performance optimization in NLM involves balancing accuracy, realism, and computational efficiency. The configuration system and build options provide multiple levels of optimization, allowing users to tailor performance to their specific needs.

Key principles for NLM performance optimization:

1. **Know Your Target**: Balance accuracy and speed based on application requirements
2. **Profile Regularly**: Use profiling tools to identify bottlenecks
3. **Tune Configuration**: Use appropriate configuration templates for different use cases
4. **Monitor Continuously**: Track performance metrics over time
5. **Plan for Scale**: Design for performance at your target network sizes

The NLM codebase provides a solid foundation for high-performance neural simulation with multiple optimization options and comprehensive performance monitoring capabilities.
