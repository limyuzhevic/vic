# NLM Performance Documentation

## Phase 5 Performance Infrastructure

### Overview

Phase 5 introduces comprehensive performance optimizations enabling NLM to scale from thousands to millions of neurons. The performance infrastructure is designed with the following principles:

1. **Event-driven processing**: Only active neurons consume computation
2. **Memory efficiency**: Pre-allocated pools and SoA layouts reduce overhead
3. **Parallelism**: Multithreading for multi-core CPUs
4. **SIMD vectorization**: Accelerate neural computations
5. **Sparse connectivity**: Avoid O(N²) memory for large networks

### Performance Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Performance Infrastructure                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │  Event Queues  │  │  Memory Pools  │  │ SIMD Ops   │ │
│  │  Ring buffers  │  │  Pre-allocated │  │ Vectorized │ │
│  │  Time-bucketed │  │  Cache-aligned │  │ LIF update │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│                                                              │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │     Sparse      │  │   Parallel      │  │  Benchmark  │ │
│  │  Connectivity   │  │   Processing   │  │  Framework  │ │
│  │  Adjacency list │  │  Thread pools  │  │  Profiling  │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

## Components

### 1. Event Queues (EventQueue.hpp)

Lock-free SPSC ring buffer for immediate spikes:
- Capacity: 16,384 spikes (configurable, power of 2)
- Cache-aligned event structures (16 bytes)
- O(1) push/pop operations

Time-bucketed delayed spike queue:
- 1,024 time buckets (configurable)
- Maximum delay: 100 steps
- Efficient batch delivery

### 2. Memory Pools (MemoryPool.hpp)

**NeuronPool**: Pre-allocated neuron storage with SoA layout
- Membrane potentials: Contiguous float array
- Refractory counters: Contiguous uint32 array
- Firing states: Contiguous uint8 array
- Cache line aligned (64 bytes)

**SynapsePool**: Compact synapse storage
- Synapse records: Contiguous struct arrays
- Adjacency lists for sparse connectivity
- Free list for O(1) allocation/deallocation

### 3. Sparse Connectivity (SparseConnectivity.hpp)

Adjacency list representation:
- Outgoing: source neuron → synapse indices
- Incoming: destination neuron → synapse indices
- Synapse records: Contiguous array

Benefits:
- Avoids dense N×N matrix
- Efficient O(k) lookup (k = synapses per neuron)
- Scales to millions of neurons

### 4. Parallel Processing (ParallelProcessing.hpp)

ThreadPool: Lock-free task queue
- Configurable thread count
- Work-stealing support
- Low overhead task submission

ParallelFor: Parallel loop execution
- Static chunk partitioning
- Grain size control
- Reduction operations

ParallelNeuralProcessor: Neural-specific parallelization
- Thread-local state accumulation
- Synchronized aggregation
- Cache-friendly data access

### 5. SIMD Vectorization (SIMDVectorization.hpp)

VectorizedLIF: Batch LIF neuron updates
- AVX-512: 16 neurons per iteration
- AVX2: 8 neurons per iteration
- SSE: 4 neurons per iteration
- Scalar fallback

VectorizedSTDP: Batch plasticity updates
- Vectorized weight calculations
- Reduced memory access

## Benchmarking

### Standard Benchmarks

| Scale | Neurons | Synapses | Target Steps/sec |
|-------|---------|----------|-----------------|
| Micro | 1,000 | ~10,000 | 10,000+ |
| Small | 10,000 | ~100,000 | 5,000+ |
| Medium | 100,000 | ~1,000,000 | 1,000+ |
| Large | 1,000,000 | ~10,000,000 | 100+ |

### Performance Metrics

**Simulation Performance**:
- Neurons per second: Total neurons / wall-clock time
- Spikes per second: Total spikes / wall-clock time
- Real-time factor: simulated_time / wall_clock_time

**Memory Usage**:
- Neuron memory: sizeof(NeuronState) × neuron_count
- Synapse memory: sizeof(SynapseRecord) × synapse_count
- Event queue memory: Queue capacity × event_size

### Measurement Commands

```bash
# Build with performance flags
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run benchmark
./build/nlm --benchmark --neurons=10000 --steps=10000

# Run scaling experiment
./build/nlm --scaling-benchmark
```

## Scaling Results

### Phase 5 Target Scales

```
Scale    Neurons    Synapses    Memory    Steps/sec
──────────────────────────────────────────────────
10k      10,000     ~100,000    ~10 MB    5,000+
100k     100,000    ~1,000,000  ~100 MB   1,000+
1M       1,000,000  ~10,000,000 ~1 GB     100+
```

### Scaling Behavior

**Ideal scaling**: Double neurons → Half performance per neuron
**Actual scaling**: Depends on:
- Memory bandwidth saturation
- Cache miss rates
- Synchronization overhead
- Event queue contention

## Performance Optimization Guidelines

### DO

1. **Profile first**: Use perf/valgrind before optimizing
2. **Measure impact**: Compare before/after benchmarks
3. **Maintain correctness**: Verify numerical results
4. **Document tradeoffs**: Note speed vs. precision costs
5. **Keep it modular**: Easy to disable optimizations

### DON'T

1. **Optimize blindly**: Measure first, optimize second
2. **Sacrifice correctness**: Numerical stability matters
3. **Over-engineer**: Simple optimizations first
4. **Ignore limits**: Some code is inherently serial
5. **Assume scaling**: Test at target scale

## Hardware Considerations

### CPU Scaling

- Single core: Baseline performance
- Multi-core: Near-linear up to ~8 cores
- NUMA: Local memory affinity matters
- AVX-512: Significant boost for vectorized ops

### Memory Scaling

- L3 cache: Critical for neural state access
- Memory bandwidth: Limits update throughput
- NUMA topology: Affects multi-socket scaling

### GPU (Future)

Potential GPU targets:
- Large-scale neuron updates
- Synaptic plasticity
- Event processing
- Population statistics

Not GPU candidates:
- Small networks (< 10k neurons)
- Sparse irregular updates
- Complex branching logic

## Future Optimizations

Phase 6 and beyond may include:

1. **GPU acceleration**: CUDA/HIP for select operations
2. **Distributed simulation**: Multi-machine scaling
3. **FPGA acceleration**: Custom neural accelerators
4. **Approximate computing**: Trading precision for speed
5. **Adaptive load balancing**: Dynamic work distribution