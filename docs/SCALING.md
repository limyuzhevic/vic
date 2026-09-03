# NLM Scaling Documentation

## Scaling Philosophy

NLM is designed to scale progressively from small proof-of-concept simulations to large-scale research platforms. Scaling is not automatic - it requires:

1. **Performance optimization**: Efficient data structures and algorithms
2. **Hardware adaptation**: CPU/GPU/cluster strategies
3. **Architectural decisions**: Avoiding O(N²) operations
4. **Measurement**: Benchmarks at each scale
5. **Honest assessment**: When scaling fails, understanding why

## Scale Levels

### Standard Scale Progression

```
┌─────────┐
│   Micro │  1,000 neurons     (Proof of concept)
└────┬────┘
     ↓
┌─────────┐
│  Small  │  10,000 neurons    (Basic experiments)
└────┬────┘
     ↓
┌─────────┐
│ Medium  │  100,000 neurons   (Moderate research)
└────┬────┘
     ↓
┌─────────┐
│  Large  │  1,000,000 neurons (Large-scale studies)
└────┬────┘
     ↓
┌─────────┐
│ X-Large │  10,000,000+ neurons (Research platform)
└─────────┘
```

### Scale Characteristics

| Scale | Neurons | Synapses | Memory | Target |
|-------|---------|----------|--------|--------|
| Micro | 1,000 | ~10,000 | <10 MB | < 1 min |
| Small | 10,000 | ~100,000 | ~100 MB | 1-5 min |
| Medium | 100,000 | ~1,000,000 | ~1 GB | 10-30 min |
| Large | 1,000,000 | ~10,000,000 | ~10 GB | 1-4 hr |
| X-Large | 10,000,000+ | ~100,000,000+ | ~100 GB+ | Hours-days |

## Scaling Challenges

### Memory Scaling

**Challenge**: Synapse storage dominates memory

**Dense connectivity**: N×N matrix = impossible at scale

**Sparse connectivity**: O(N×K) where K = synapses per neuron

**NLM approach**:
- Adjacency lists instead of matrices
- SoA layouts for cache efficiency
- Memory pools to reduce fragmentation

### Compute Scaling

**Challenge**: O(N) or O(N×K) operations per step

**Dense approach**: Every neuron connects to every other = O(N²)

**Sparse approach**: Fixed synapses per neuron = O(N)

**NLM approach**:
- Event-driven: Only active neurons compute
- Parallel: Distribute across cores
- SIMD: Vectorize across neurons

### Communication Scaling

**Challenge**: Spike events must reach destination neurons

**Biological reality**:
- Axonal delays vary (1-100ms)
- Synaptic transmission has latency

**NLM approach**:
- Time-bucketed spike delivery
- Delayed event queues
- Batch processing

## Scaling Strategies

### CPU Scaling

**Threading model**:
```
Main thread:
├── Sensory processing
├── Motor processing
└── Brain step
    ├── Spike delivery
    ├── Neuron updates (parallel)
    ├── Spike detection (parallel)
    └── Plasticity updates
```

**Scaling efficiency**:
- 1→2 threads: ~1.8x speedup
- 1→4 threads: ~3.2x speedup
- 1→8 threads: ~5.5x speedup
- Diminishing returns from ~8 threads

**Reasons for inefficiency**:
- Synchronization overhead
- Cache coherency traffic
- NUMA effects
- Load imbalance

### Memory Scaling

**SoA vs AoS**:

Structure of Arrays (SoA):
```cpp
struct Neurons {
    float membranePotential[N];
    float threshold[N];
    uint32_t refractory[N];
};
```

Arrays of Structures (AoS):
```cpp
struct Neuron {
    float membranePotential;
    float threshold;
    uint32_t refractory;
};
Neuron neurons[N];
```

**SoA benefits**:
- Cache line loads affect multiple neurons
- SIMD vector loads
- Better memory bandwidth utilization

### Event-Driven Optimization

**Traditional approach**: Update all neurons every step
```
for each neuron:
    update_membrane_potential(neuron)
```

**Event-driven approach**: Only update active neurons
```
for each active_neuron:
    update_membrane_potential(neuron)
```

**Speedup**: 10-100x for sparse activity

## Benchmark Framework

### Scaling Benchmark System

```cpp
ScalingBenchmark benchmark;
benchmark.configure({
    .scales = {1000, 10000, 100000, 1000000},
    .seeds = {42, 43, 44},
    .stepsPerScale = 10000
});

auto results = benchmark.run(createBrain, runSimulation, collectMetrics);
```

### Metrics Collected

**Performance**:
- neuronsPerSecond
- spikesPerSecond
- realTimeFactor
- avgStepTimeMs

**Memory**:
- neuronMemoryMB
- synapseMemoryMB
- peakMemoryMB

**Activity**:
- avgFiringRate
- activeNeurons
- spikeDensity

## Scaling Experiments

### Experiment 1: Linear Scaling Test

**Goal**: Verify O(N) scaling

**Method**:
1. Run with N neurons, 10k steps
2. Run with 2N neurons, 10k steps
3. Compare time

**Expected**: Time should ~double
**Acceptable**: Time increases by < 2.5x

### Experiment 2: Event-Driven Efficiency

**Goal**: Measure benefit of sparse updates

**Method**:
1. Run with sparse activity (~1% firing)
2. Run with dense activity (~50% firing)
3. Compare time

**Expected**: Sparse should be 10-50x faster

### Experiment 3: Parallel Scaling

**Goal**: Measure threading efficiency

**Method**:
1. Run with 1 thread
2. Run with 2 threads
3. Run with 4 threads
4. Run with 8 threads
5. Compare speedup

**Expected**: Diminishing returns, ~5-6x at 8 threads

### Experiment 4: Memory Scaling

**Goal**: Verify memory usage

**Method**:
1. Measure memory at each scale
2. Plot memory vs neuron count
3. Compare to theoretical

**Expected**: Linear growth with slope ≈ synapses per neuron × synapse size

## Scale-Specific Considerations

### Micro Scale (1,000 neurons)

**Use case**: Quick tests, development

**Considerations**:
- Overhead dominates
- Not representative of large-scale behavior
- Good for debugging

### Small Scale (10,000 neurons)

**Use case**: Basic experiments

**Considerations**:
- Simple behaviors emerge
- Learning observable
- ~100ms per step typical

### Medium Scale (100,000 neurons)

**Use case**: Research studies

**Considerations**:
- Interesting dynamics emerge
- Memory ~1GB
- Parallel processing beneficial

### Large Scale (1,000,000 neurons)

**Use case**: Complex experiments

**Considerations**:
- Significant computation required
- Memory ~10GB
- Event-driven essential

### X-Large Scale (10,000,000+ neurons)

**Use case**: Research platform

**Considerations**:
- Distributed computing needed
- Memory ~100GB+
- Checkpointing essential

## Scaling Failures

### Common Scaling Problems

1. **Memory explosion**: O(N²) connectivity
   - Symptom: Memory grows faster than neurons
   - Fix: Use sparse connectivity

2. **Load imbalance**: Uneven work distribution
   - Symptom: Some threads idle
   - Fix: Dynamic work stealing

3. **Cache thrashing**: Poor memory access patterns
   - Symptom: Slowdowns at scale
   - Fix: SoA layout, prefetching

4. **Synchronization bottleneck**: Too much locking
   - Symptom: Threads block each other
   - Fix: Thread-local state, reduce contention

## Hardware Targets

### Single Desktop

- 4-8 CPU cores
- 16-64 GB RAM
- No GPU
- Max: ~100k neurons with optimizations

### Workstation

- 8-32 CPU cores
- 64-256 GB RAM
- Optional GPU
- Max: ~1M neurons

### Server/Cluster

- 32+ CPU cores
- 256+ GB RAM
- GPU optional
- Max: ~10M+ neurons (distributed)

## Recommendations

### For Beginners

1. Start at Small scale (10k neurons)
2. Measure baseline performance
3. Make one change at a time
4. Measure impact before proceeding

### For Research

1. Determine required scale for your questions
2. Benchmark at target scale early
3. Profile to find bottlenecks
4. Optimize iteratively

### For Scaling Studies

1. Test multiple scales
2. Run multiple seeds
3. Collect comprehensive metrics
4. Report scaling behavior honestly

## Future Scaling Directions

### Phase 6+ Ideas

1. **GPU acceleration**: CUDA/HIP for neuron updates
2. **Distributed simulation**: Multi-machine scaling
3. **Hierarchical partitioning**: Region-level parallelism
4. **Adaptive granularity**: Dynamic load balancing
5. **Approximate computing**: Trading precision for speed