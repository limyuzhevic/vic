# NLM Performance Optimization Guide

## Overview

NLM features a comprehensive performance infrastructure designed to support neural simulations at scales ranging from thousands to millions of neurons. The performance system is built around event-driven processing, memory efficiency, and parallel computation to maximize throughput while maintaining accuracy.

## Performance Architecture

### Core Performance Principles

1. **Event-Driven Processing**: Only active neurons consume computation
2. **Memory Efficiency**: Pre-allocated pools and SoA layouts reduce overhead
3. **Parallelism**: Multithreading for multi-core CPUs
4. **SIMD Vectorization**: Accelerate neural computations
5. **Sparse Connectivity**: Avoid O(N²) memory for large networks

### Performance Infrastructure

```mermaid
graph LR
    A[Performance Infrastructure] --> B[Event Management]
    A --> C[Memory Management]
    A --> D[Parallel Processing]
    A --> E[Vectorization]
    A --> F[Connectivity]
    A --> G[Benchmarking]
    
    B --> B1[Event Queues]
    B --> B2[Time Buckets]
    
    C --> C1[Memory Pools]
    C --> C2[SoA Layout]
    C --> C3[Cache Optimization]
    
    D --> D1[Thread Pool]
    D --> D2[Parallel Neural Processing]
    
    E --> E1[SIMD Vectorization]
    E --> E2[Batch Operations]
    
    F --> F1[Sparse Connectivity]
    F --> F2[Adjacency Lists]
    
    G --> G1[Benchmark Framework]
    G --> G2[Performance Metrics]
end
```

## Performance Components

### 1. Event Queues (EventQueue.hpp)

Lock-free SPSC ring buffer for immediate spikes:

```cpp
// Event queue for fast spike delivery
class EventQueue {
public:
    EventQueue(size_t capacity = 16384);
    bool push(SpikeEvent event);
    bool pop(SpikeEvent& event);
    bool isEmpty() const;
    size_t size() const;
};
```

**Features:**
- Capacity: 16,384 spikes (configurable, power of 2)
- Cache-aligned event structures (16 bytes)
- O(1) push/pop operations
- Thread-safe for single producer/single consumer

**Time-bucketed delayed spike queue:**

```cpp
// Time-bucketed queue for delayed spikes
class TimeBatchedEventQueue {
public:
    TimeBatchedEventQueue(size_t buckets = 1024, size_t maxDelay = 100);
    void scheduleDelayedSpike(const SpikeEvent& event, Delay delay);
    void processEvents(SimulationStep currentStep);
};
```

- 1,024 time buckets (configurable)
- Maximum delay: 100 steps
- Efficient batch delivery

### 2. Memory Pools (MemoryPool.hpp)

**NeuronPool**: Pre-allocated neuron storage with SoA layout:

```cpp
class NeuronPool {
public:
    NeuronPool(size_t capacity);
    NeuronId allocateNeuron(const NeuronState& initialState);
    void deallocateNeuron(NeuronId id);
    NeuronState& getState(NeuronId id);
    const NeuronState& getState(NeuronId id) const;
    
    // SoA access for performance
    const std::vector<MembranePotential>& getAllMembranes() const { return membranePotentials_; }
    const std::vector<RefractoryCounter>& getAllRefractories() const { return refractoryCounters_; }
    // ... other SoA arrays
};
```

**Features:**
- Contiguous memory allocation
- SoA (Structure of Arrays) layout
- Cache line aligned (64 bytes)
- O(1) allocation/deallocation

**SynapsePool**: Compact synapse storage:

```cpp
class SynapsePool {
public:
    SynapsePool(size_t capacity);
    SynapseId allocateSynapse(const SynapseRecord& initialRecord);
    void deallocateSynapse(SynapseId id);
    SynapseRecord& getRecord(SynapseId id);
    
    // Adjacency lists for sparse connectivity
    const std::vector<SynapseId>& getOutgoingConnections(NeuronId source) const;
    const std::vector<SynapseId>& getIncomingConnections(NeuronId target) const;
};
```

### 3. Sparse Connectivity (SparseConnectivity.hpp)

Adjacency list representation for efficient large-scale networks:

```cpp
class SparseConnectivity {
public:
    SparseConnectivity(size_t neuronCount);
    
    // Outgoing connections
    void addConnection(NeuronId source, NeuronId target, 
                      float weight = 1.0f, Delay delay = 1);
    void removeConnection(NeuronId source, NeuronId target);
    const std::vector<SynapseId>& getOutgoingConnections(NeuronId source) const;
    
    // Incoming connections
    const std::vector<SynapseId>& getIncomingConnections(NeuronId target) const;
    
    // Synapse record access
    SynapseRecord& getSynapse(SynapseId id);
    const SynapseRecord& getSynapse(SynapseId id) const;
    
    // Performance optimizations
    void compact();  // Remove unused synapses
    void rebalance(); // Balance connection distribution
};
```

**Benefits:**
- Avoids dense N×N matrix
- Efficient O(k) lookup (k = synapses per neuron)
- Scales to millions of neurons
- Memory efficient for sparse networks

### 4. Parallel Processing (ParallelProcessing.hpp)

ThreadPool for parallel neural computation:

```cpp
class ThreadPool {
public:
    ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
    template<typename F>
    void submit(F&& task);
    void waitForCompletion();
    size_t getThreadCount() const;
};
```

**Features:**
- Configurable thread count
- Work-stealing support
- Low overhead task submission
- Thread-safe operations

Parallel neural processor with thread-local state:

```cpp
class ParallelNeuralProcessor {
public:
    ParallelNeuralProcessor(size_t neuronCount, size_t threadCount);
    void processNeurons(const std::vector<NeuronState>& states, 
                       std::vector<NeuronState>& results);
    void processSpikes(const std::vector<SpikeEvent>& spikes,
                      std::vector<SpikeEvent>& processedSpikes);
    
    // Parallel plasticity updates
    void applyPlasticity(const std::vector<SynapseRecord>& synapses,
                        const std::vector<PlasticityUpdate>& updates);
};
```

**Features:**
- Thread-local state accumulation
- Synchronized aggregation
- Cache-friendly data access
- Efficient memory usage

### 5. SIMD Vectorization (SIMDVectorization.hpp)

Vectorized LIF neuron updates for high performance:

```cpp
class VectorizedLIF {
public:
    VectorizedLIF();
    
    // Process neurons using SIMD
    void processNeurons(const float* membranePotentials,
                       const float* inputCurrents,
                       float* newMembranePotentials,
                       const float* timeConstants,
                       const float* thresholds,
                       size_t neuronCount);
    
    // Get SIMD capabilities
    static SIMDLevel getSIMDLevel();
    static size_t getSIMDWidth();
};
```

**Features:**
- AVX-512: 16 neurons per iteration
- AVX2: 8 neurons per iteration
- SSE: 4 neurons per iteration
- Scalar fallback

Vectorized STDP for batch plasticity updates:

```cpp
class VectorizedSTDP {
public:
    VectorizedSTDP();
    
    // Vectorized STDP computation
    void computeSTDP(const float* preSpikes,
                    const float* postSpikes,
                    float* weightUpdates,
                    const STDPParameters& params,
                    size_t neuronCount);
};
```

### 6. Benchmark Framework (Benchmark.hpp)

Comprehensive benchmarking infrastructure:

```cpp
class Benchmark {
public:
    struct BenchmarkResult {
        double neuronsPerSecond;
        double spikesPerSecond;
        double realTimeFactor;
        size_t totalNeurons;
        size_t totalSpikes;
        double wallClockTime;
    };
    
    static BenchmarkResult benchmarkNeuralComputation(
        const NeuralSimulation& simulation,
        size_t neuronCount,
        size_t steps);
    
    static void runScalingBenchmarks();
    static void profilePerformance(std::vector<BenchmarkResult>& results);
};
```

## Performance Targets

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

## Performance Measurement

### Running Benchmarks

```bash
# Build with performance flags
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run basic benchmark
./nlm --benchmark --neurons=10000 --steps=10000

# Run scaling benchmark
./nlm --scaling-benchmark

# Profile with custom configuration
./nlm --profile --neurons=50000 --steps=5000 --output=profile.json
```

### Performance Scripts

```bash
#!/bin/bash
# benchmark.sh - Run NLM performance benchmarks

set -e

BUILD_DIR="${1:-build}"
NEURONS="${2:-10000}"
STEPS="${3:-10000}"

# Build with performance optimizations
cmake -S . -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON
make -C $BUILD_DIR -j$(nproc)

# Run benchmark
./nlm/$nlm --benchmark --neurons=$NEURONS --steps=$STEPS

# Run scaling tests
./nlm/$nlm --scaling-benchmark --output=scaling_results.json
```

## Scaling Results

### Phase 5 Target Scales

```
Scale    Neurons    Synapses    Memory    Steps/sec
─────────────────────────────────────────────────
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
6. **Test at scale**: Profile with realistic workload
7. **Consider hardware**: Optimize for target CPU architecture

### DON'T

1. **Optimize blindly**: Measure first, optimize second
2. **Sacrifice correctness**: Numerical stability matters
3. **Over-engineer**: Simple optimizations first
4. **Ignore limits**: Some code is inherently serial
5. **Assume scaling**: Test at target scale
6. **Focus on micro-optimizations**: Macro-optimizations have bigger impact

## Hardware Considerations

### CPU Scaling

- **Single core**: Baseline performance
- **Multi-core**: Near-linear up to ~8 cores
- **NUMA**: Local memory affinity matters
- **AVX-512**: Significant boost for vectorized ops
- **Cache hierarchy**: Critical for performance

### Memory Scaling

- **L1 cache**: Small but fast for hot data
- **L2 cache**: Medium performance cache
- **L3 cache**: Critical for neural state access
- **Memory bandwidth**: Limits update throughput
- **NUMA topology**: Affects multi-socket scaling

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

## Optimization Strategies

### 1. Memory Layout Optimization

**Structure of Arrays (SoA)** vs **Arrays of Structures (AoS)**:

```cpp
// ❌ AoS - Poor cache locality
struct Neuron {
    float membranePotential;
    float refractoryCounter;
    float inputCurrent;
    // ... other fields
};
Neuron neurons[1000];

// ✅ SoA - Excellent cache locality
class NeuronPool {
    std::vector<float> membranePotentials_;
    std::vector<float> refractoryCounters_;
    std::vector<float> inputCurrents_;
    // ... separate arrays
};
```

### 2. Pre-allocation

Pre-allocate memory pools for performance:

```cpp
// ❌ Dynamic allocation
std::vector<NeuronState> neurons;
neurons.reserve(1000000);  // Still has overhead

// ✅ Pre-allocation
NeuronPool neuronPool(1000000);  // Exact size, no reallocation
```

### 3. Event-Driven Processing

Only process neurons that have input:

```cpp
// ❌ Process all neurons every step
for (auto& neuron : neurons) {
    neuron.update(step);
}

// ✅ Event-driven processing
for (auto spike : incomingSpikes) {
    neuronPools[spike.target].deliverInput(spike.input);
}
```

### 4. Parallel Processing

Parallelize computation across cores:

```cpp
// ❌ Sequential
for (int i = 0; i < neuronCount; ++i) {
    processNeuron(i);
}

// ✅ Parallel
ThreadPool pool(std::thread::hardware_concurrency());
for (int i = 0; i < neuronCount; ++i) {
    pool.submit([i]() { processNeuron(i); });
}
pool.waitForCompletion();
```

### 5. SIMD Vectorization

Vectorize operations where possible:

```cpp
// ❌ Scalar
for (int i = 0; i < neuronCount; ++i) {
    updateNeuron(neuronStates[i]);
}

// ✅ SIMD
VectorizedLIF lif;
lif.processNeurons(membranePotentials, inputCurrents,
                  newMembranePotentials, timeConstants,
                  thresholds, neuronCount);
```

## Performance Monitoring

### Profiling Tools

```bash
# Linux performance profiling
perf record -g ./nlm --benchmark
perf report

# Valgrind memory profiling
valgrind --tool=massif ./nlm --benchmark

# GNU Profile
gprof nlm nlm.profile

# Intel VTune
amplxe-cl -collect hotspots -- ./nlm --benchmark
```

### Performance Metrics Collection

```cpp
class PerformanceMonitor {
public:
    struct PerformanceStats {
        double simulationSpeed;      // neurons/second
        double spikeThroughput;      // spikes/second
        double memoryUsage;          // bytes
        double cpuUsage;             // percentage
        double cacheMissRate;        // percentage
        std::chrono::duration<double> wallTime;
    };
    
    void recordStartTime();
    void recordEndTime();
    void recordNeuronCount(size_t count);
    void recordSpikeCount(size_t count);
    void recordMemoryUsage(size_t bytes);
    void recordCPUTime(std::chrono::duration<double> time);
    
    PerformanceStats getStats() const;
    std::string getReport() const;
};
```

## Performance Tuning

### CPU-Specific Optimizations

```cpp
class CPUBenchmark {
public:
    enum class Architecture {
        Unknown,
        x86_64,
        ARM64,
        RISC_V
    };
    
    static Architecture detectArchitecture();
    static bool hasAVX512();
    static bool hasAVX2();
    static bool hasSSE4();
    static size_t getCacheLineSize();
    static size_t getL1CacheSize();
    static size_t getL2CacheSize();
    static size_t getL3CacheSize();
};
```

### Memory Optimization

```cpp
class MemoryOptimizer {
public:
    // Align data to cache lines
    template<typename T>
    static T* alignMemory(size_t count) {
        void* ptr = std::aligned_alloc(alignof(T), sizeof(T) * count);
        return static_cast<T*>(ptr);
    }
    
    // Prefetch data
    static void prefetch(const void* addr, int strategy = 0);
    
    // Cache-friendly data layout
    template<typename T>
    class CacheFriendlyVector {
        std::vector<T> data_;
        size_t alignment_;
        
        T& operator[](size_t index) { return data_[index]; }
        const T& operator[](size_t index) const { return data_[index]; }
    };
};
```

### I/O Optimization

```cpp
class IOOptimizer {
public:
    // Buffered file I/O
    static bool saveState(const std::string& filename, const BrainState& state);
    static bool loadState(const std::string& filename, BrainState& state);
    
    // Memory-mapped files
    static bool mmapFile(const std::string& filename, void*& mappedMemory, size_t& fileSize);
    static void unmapFile(void* mappedMemory);
    
    // Compress checkpoints
    static bool saveCompressed(const std::string& filename, const BrainState& state);
    static bool loadCompressed(const std::string& filename, BrainState& state);
};
```

## Future Optimizations

### Phase 6 and Beyond

1. **GPU Acceleration**: CUDA/HIP for select operations
2. **Distributed Simulation**: Multi-machine scaling
3. **FPGA Acceleration**: Custom neural accelerators
4. **Approximate Computing**: Trading precision for speed
5. **Adaptive Load Balancing**: Dynamic work distribution
6. **Hardware-Specific Optimizations**: Custom kernels for specific CPUs

### Research Areas

1. **Neuron Models**: More efficient LIF variants
2. **Connectivity Patterns**: Optimal wiring patterns
3. **Plasticity Rules**: Faster learning algorithms
4. **Event Scheduling**: Better time management
5. **Memory Management**: More efficient allocation strategies

## Performance Validation

### Correctness Verification

```bash
# Verify numerical correctness
./nlm --validate --reference=baseline --neurons=1000 --steps=1000

# Compare with serial implementation
./nlm --compare-serial --neurons=10000 --steps=100
```

### Performance Regression Testing

```bash
# Performance regression tests
cat > performance_regression.sh << 'EOF'
#!/bin/bash

# Run performance tests
./nlm --benchmark --neurons=10000 --steps=10000 > results.json

# Check against baseline
python3 check_performance.py results.json baseline.json

# Generate performance report
python3 generate_report.py results.json > performance_report.html
EOF

chmod +x performance_regression.sh
```

## Conclusion

The NLM performance infrastructure provides comprehensive performance optimizations enabling neural simulations at scales from thousands to millions of neurons. The system is designed with performance in mind from the ground up, using event-driven processing, memory efficiency, parallelism, SIMD vectorization, and sparse connectivity.

Key benefits:

1. **Scalability**: Supports simulations from thousands to millions of neurons
2. **Performance**: Achieves high throughput with efficient memory usage
3. **Flexibility**: Modular design allows selective optimization
4. **Accuracy**: Maintains numerical correctness while optimizing performance
5. **Extensibility**: Foundation for future performance enhancements
6. **Monitoring**: Comprehensive performance monitoring and profiling

The performance system is production-ready and provides a solid foundation for large-scale NLM simulations and research.

## References

1. <system>/src/performance/Performance.hpp - Performance infrastructure header
2. <system>/src/performance/Performance.cpp - Performance infrastructure implementation
3. <system>/benchmark.cpp - Benchmark framework implementation
4. <system>/src/benchmark.h - Benchmark header
5. <system>/docs/PERFORMANCE.md - Performance documentation