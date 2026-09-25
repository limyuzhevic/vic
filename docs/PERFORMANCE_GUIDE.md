# NLM Performance Guide

## Overview

This comprehensive performance guide provides benchmarks, optimization strategies, and scaling recommendations for the NLM brain simulation. It covers everything from basic optimization techniques to large-scale distributed computing strategies.

## Performance Optimization Quick Start

### Basic Optimization Checklist

1. **Use Release Builds**: Always build with `-DCMAKE_BUILD_TYPE=Release`
2. **Enable Performance Flags**: Add `-DNLM_PERFORMANCE_ENABLED=ON`
3. **Use Thread Pool**: Configure for your CPU core count
4. **Enable SIMD**: Requires x86-64 with AVX-512/AVX2 support
5. **Use Sparse Connectivity**: Most important for scaling

### Target Performance Levels

| Scale | Neurons | Synapses | Memory | Target Steps/sec |
|-------|---------|----------|--------|-----------------|
| Micro | 1,000 | ~10,000 | <10 MB | 10,000+ |
| Small | 10,000 | ~100,000 | ~100 MB | 5,000+ |
| Medium | 100,000 | ~1,000,000 | ~1 GB | 1,000+ |
| Large | 1,000,000 | ~10,000,000 | ~10 GB | 100+ |
| X-Large | 10,000,000+ | ~100,000,000+ | ~100 GB+ | Hours-days |

## Benchmarking Framework

### Standard Benchmark Commands

```bash
# Build with performance optimizations
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run standard benchmarks
./nlm --benchmark --neurons=10000 --steps=10000

# Run scaling experiments
./nlm --scaling-benchmark --scales=1000,10000,100000

# Detailed profiling with flame graphs
./nlm --profile --benchmark --neurons=100000 --steps=1000
```

### Performance Metrics

**Core Metrics**:
- **neuronsPerSecond**: Neurons processed per second
- **spikesPerSecond**: Total spikes per second
- **realTimeFactor**: Simulated_time / wall_clock_time
- **avgStepTimeMs**: Average time per simulation step

**Memory Metrics**:
- **neuronMemoryMB**: Memory for neuron state
- **synapseMemoryMB**: Memory for synaptic connections
- **peakMemoryMB**: Maximum memory usage
- **memoryEfficiency**: Memory usage per neuron

**Activity Metrics**:
- **avgFiringRate**: Average firing rate across neurons
- **activeNeurons**: Number of firing neurons
- **spikeDensity**: Spikes per neuron per second
- **synchronization**: Neural synchrony measurements

## Optimization Techniques

### 1. Event-Driven Processing

**Why it matters**: Traditional approaches update all neurons every step, even inactive ones.

**Implementation**:
```cpp
// Traditional approach (inefficient)
for each neuron in network:
    update_membrane_potential(neuron)

// Event-driven approach (efficient)
for each active_neuron in event_queue:
    update_membrane_potential(neuron)
```

**Expected speedup**: 10-100x for sparse activity

**Best for**: Realistic neural activity where most neurons are inactive

### 2. Sparse Connectivity

**Why it matters**: Dense N×N matrices are impossible beyond thousands of neurons.

**Implementation**:
```cpp
// Dense (not recommended beyond 1k neurons)
float weightMatrix[N][N];

// Sparse (NLM approach)
adjacency_list[out_neuron] = list_of_synapse_indices;
synapse_records[all_synapses];
```

**Memory savings**: From O(N²) to O(N×K) where K = synapses per neuron

**Typical K values**: 100-1000 synapses per neuron

### 3. SIMD Vectorization

**Implementation**:
```cpp
// Scalar (baseline)
for (int i = 0; i < N; i++) {
    update_neuron(neuron[i]);
}

// AVX-512 (16 neurons at once)
for (int i = 0; i < N; i += 16) {
    __m512 v_pot = _mm512_loadu_ps(potentials + i);
    // ... vector operations
}
```

**Support levels**:
- **AVX-512**: 16 neurons per iteration
- **AVX2**: 8 neurons per iteration
- **SSE**: 4 neurons per iteration
- **Scalar**: 1 neuron per iteration (fallback)

### 4. Memory Layout Optimization

**Structure of Arrays (SoA) - Recommended**:
```cpp
struct Neurons {
    float membranePotential[N];
    float threshold[N];
    uint32_t refractory[N];
    float synapticWeight[N];
};
```

**Array of Structures (AoS) - Avoid**:
```cpp
struct Neuron {
    float membranePotential;
    float threshold;
    uint32_t refractory;
    float synapticWeight;
} neurons[N];
```

**Benefits of SoA**:
- Better cache utilization (same cache line affects multiple neurons)
- SIMD vector loading efficiency
- Reduced memory bandwidth usage
- Easier parallelism

### 5. Parallel Processing

**Threading Strategy**:
```
Main Thread:
├── Sensory processing
├── Motor processing
└── Brain step
    ├── Spike delivery
    ├── Neuron updates (parallel)
    ├── Spike detection (parallel)
    └── Plasticity updates
```

**Parallel Regions**:
1. **Spike delivery**: Sequential (simple dependencies)
2. **Neuron updates**: Parallel (independent)
3. **Spike detection**: Parallel (independent)
4. **Plasticity updates**: Parallel (independent)

**Expected speedups**:
- 1→2 threads: ~1.8x
- 1→4 threads: ~3.2x
- 1→8 threads: ~5.5x
- Diminishing returns beyond 8 threads

## Hardware-Specific Optimizations

### CPU Considerations

**Cache hierarchy matters**:
- L1 cache (32KB/core): Critical for active neuron state
- L2 cache (256KB/core): Neuron population storage
- L3 cache (MBs): All neurons (if small enough)

**NUMA effects**:
- Bind threads to specific CPU cores
- Place neuron data on local NUMA node
- Avoid cross-node memory access

**SIMD capabilities**:
- Check for AVX-512, AVX2, SSE support
- Fall back to scalar if needed
- Use feature detection at runtime

### Memory Scaling

**SoA memory layout calculation**:
```cpp
// For 1M neurons
struct NeuronState {
    float membranePotential;    // 4 bytes
    float threshold;           // 4 bytes
    uint32_t refractory;       // 4 bytes
    float synapticWeight;      // 4 bytes
    uint8_t firingState;       // 1 byte
    // padding: 3 bytes for alignment
};

// Total per neuron: 20 bytes
// For 1M neurons: 20 MB
```

**Synapse memory** (sparse):
```cpp
struct SynapseRecord {
    uint32_t targetNeuron;     // 4 bytes
    float weight;             // 4 bytes
    float delay;             // 4 bytes
    uint32_t eligibility;    // 4 bytes
};

// 16 bytes per synapse
// For 10M synapses: 160 MB
```

## Scaling Strategies

### Event-Driven Scaling

**Challenge**: Spike delivery latency

**Solutions**:
1. **Time-bucketed queues**: 1,024 buckets for 100-step delays
2. **Batch processing**: Deliver spikes in groups
3. **Predictive scheduling**: Pre-compute spike deliveries

**Implementation**:
```cpp
// Time-bucketed delayed spike queue
EventQueue<1024> delayedQueue; // 1,024 time buckets
max_delay = 100 steps;
```

### Memory Scaling

**Problem**: Memory bandwidth limits performance

**Solutions**:
1. **Compression**: Quantize weights, use half-precision where possible
2. **Caching**: Cache frequently accessed neurons
3. **Paging**: Move inactive neurons to disk
4. **NUMA affinity**: Keep data close to processing threads

### Compute Scaling

**Challenge**: Synchronization overhead

**Solutions**:
1. **Thread-local state**: Reduce shared memory access
2. **Lock-free queues**: For spike delivery
3. **Work stealing**: Balance load across threads
4. **Adaptive parallelism**: Reduce threads when overhead > benefit

## Performance Profiling

### Common Bottlenecks

1. **Memory bandwidth**: Check cache miss rates
2. **Synchronization**: Look for thread contention
3. **Event queue saturation**: Monitor queue fullness
4. **Lock contention**: Profile lock acquisition time

### Profiling Tools

**Linux perf**:
```bash
perf record ./nlm --benchmark --neurons=100000
perf report
perf annot
```

**Valgrind Cachegrind**:
```bash
valgrind --tool=cachegrind ./nlm --benchmark --neurons=100000
cg_annotate cachegrind.out
```

**Custom NLM profilers**:
```bash
./nlm --profile --benchmark --neurons=100000 --output=perf_data
```

## Real-World Performance Examples

### Example 1: Micro-Scale (1k neurons)

**Configuration**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
./nlm --neurons=1000 --steps=10000
```

**Expected results**:
- Time per step: <1ms
- Memory usage: ~2MB
- Real-time factor: 10-100x

### Example 2: Small-Scale (10k neurons)

**Configuration**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
./nlm --neurons=10000 --steps=1000
```

**Expected results**:
- Time per step: ~10ms
- Memory usage: ~20MB
- Real-time factor: 1-10x

### Example 3: Medium-Scale (100k neurons)

**Configuration**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
./nlm --neurons=100000 --steps=100
```

**Expected results**:
- Time per step: ~100ms
- Memory usage: ~200MB
- Real-time factor: 0.1-1x

### Example 4: Large-Scale (1M neurons)

**Configuration**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
./nlm --neurons=1000000 --steps=10
```

**Expected results**:
- Time per step: ~1s
- Memory usage: ~2GB
- Real-time factor: 0.01-0.1x

## Optimization Best Practices

### DO

1. **Profile before optimizing**: Use `perf` or `valgrind`
2. **Measure impact**: Compare benchmarks before/after changes
3. **Maintain correctness**: Verify numerical results remain valid
4. **Document tradeoffs**: Note speed vs. precision costs
5. **Keep it modular**: Make optimizations optional/disablable
6. **Test at target scale**: Optimize for your actual use case
7. **Consider hardware**: Write hardware-aware code
8. **Batch operations**: Group small operations

### DON'T

1. **Optimize blindly**: Always measure first
2. **Sacrifice correctness**: Numerical stability matters more than speed
3. **Over-engineer**: Simple optimizations first
4. **Assume scaling**: Test at your target scale
5. **Ignore limits**: Some operations are inherently serial
6. **Skip testing**: Optimize incrementally and verify each change
7. **Hide complexity**: Make performance implications clear
8. **Assume users know**: Provide performance guidelines

## Future Performance Directions

### Phase 6+ Performance Enhancements

1. **GPU Acceleration**:
   - CUDA kernels for neuron updates
   - Parallel spike delivery
   - Memory transfers optimization

2. **Distributed Computing**:
   - Multi-machine scaling
   - Hierarchical partitioning
   - Consistency protocols

3. **Approximate Computing**:
   - Probabilistic plasticity
   - Quantized weights
   - Approximate neuron models

4. **Adaptive Systems**:
   - Dynamic thread count
   - Adaptive timestep
   - Load-aware scheduling

## Performance Recommendations by Use Case

### For Beginners

1. **Start simple**: 1k neurons for development
2. **Measure baseline**: Establish performance metrics
3. **One change at a time**: Measure impact of each optimization
4. **Use provided configs**: Follow established optimization patterns

### For Research

1. **Target your scale**: Benchmark at your planned scale
2. **Profile bottlenecks**: Find what limits your research
3. **Optimize iteratively**: Focus on what matters for your work
4. **Document your setup**: Record hardware and configuration

### For Production

1. **Automate optimization**: Use performance regression testing
2. **Monitor continuously**: Track performance in production
3. **Plan capacity**: Account for performance degradation
4. **Benchmark regularly**: Compare against baselines

## Performance Troubleshooting

### Common Issues

**Symptom: Slow performance at small scale**
- **Cause**: Overhead dominates computation
- **Fix**: Use micro-scale for development, optimize later

**Symptom: Memory usage grows too fast**
- **Cause**: Dense connectivity or inefficient data structures
- **Fix**: Implement sparse connectivity, use SoA layout

**Symptom: Poor scaling**
- **Cause**: Synchronization bottlenecks or cache misses
- **Fix**: Parallelize independent operations, optimize memory layout

**Symptom: Real-time factor too low**
- **Cause**: Wall-clock time dominates simulation time
- **Fix**: Reduce computation per step, use event-driven approach

### Diagnosis Commands

```bash
# Check memory usage
./nlm --benchmark --neurons=100000 --memory-profile

# Check cache performance
perf record ./nlm --benchmark --neurons=100000
perf report | grep cache-misses

# Check threading efficiency
./nlm --benchmark --neurons=100000 --thread-profile
```

## Performance Summary

Performance optimization in NLM involves:

1. **Memory efficiency**: SoA layout, sparse connectivity
2. **Compute efficiency**: SIMD, parallel processing
3. **I/O efficiency**: Event-driven spike processing
4. **Hardware awareness**: NUMA, cache, SIMD optimization
5. **Measurement**: Continuous benchmarking and profiling

By following these guidelines, you can achieve efficient scaling from micro-scale to X-large scale deployments, enabling research across multiple orders of magnitude of neural system complexity.