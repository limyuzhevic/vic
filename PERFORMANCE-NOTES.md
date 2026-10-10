# NLM Performance Notes

## Introduction

This document provides detailed performance optimization notes for the Neural Learning Machine (NLM) framework. It covers:

- Performance bottlenecks and solutions
- Hardware-specific optimizations
- Benchmarking methodologies
- Profiling techniques
- Scalability considerations
- Memory optimization strategies

## 1. Performance Architecture Overview

### 1.1 Event-Driven Processing

NLM uses event-driven simulation to achieve high performance:

```cpp
// Event system architecture
class EventQueue {
public:
    // Lock-free SPSC ring buffer
    bool push(Event event);
    bool pop(Event& event);
    
    // Time-bucketed delayed events
    void scheduleDelayed(Event event, Timestamp delay);
    void deliverEvents(Timestamp currentTime);
};
```

**Key Optimizations:**

1. **Ring Buffer Size**: 16,384 events (power of 2 for efficient modulo)
2. **Cache Alignment**: 16-byte aligned event structures
3. **O(1) Operations**: Push/pop with minimal overhead
4. **Time Buckets**: 1,024 buckets for up to 100ms delays

### 1.2 Memory Layout Strategies

```cpp
// Structure-of-Arrays (SoA) for cache efficiency
struct NeuronState {
    std::vector<MembranePotential> potentials;   // All potentials contiguous
    std::vector<MembranePotential> thresholds;   // All thresholds contiguous
    std::vector<FiringState> states;           // All states contiguous
    // ... other state variables
};
```

**Benefits:**

- **Spatial Locality**: All neuron properties accessed together
- **Prefetching**: Predictable memory access patterns
- **SIMD Compatibility**: Vectorized operations possible
- **Memory Bandwidth**: Efficient utilization of memory bandwidth

## 2. CPU-Specific Optimizations

### 2.1 SIMD Vectorization

#### AVX-512 Optimization
```cpp
// Vectorized neuron updates
void updateNeuronsAVX512(const NeuronState& state, 
                         std::vector<NeuronState>& newState,
                         size_t startIdx, size_t count) {
    size_t iterations = count / 16;
    size_t remainder = count % 16;
    
    for (size_t i = 0; i < iterations; ++i) {
        size_t idx = startIdx + i * 16;
        
        // Load neuron states into AVX-512 registers
        __m512 v_pot = _mm512_loadu_ps(&state.potentials[idx]);
        __m512 v_thres = _mm512_loadu_ps(&state.thresholds[idx]);
        __m512 v_rest = _mm512_loadu_ps(&state.resting[idx]);
        __m512 v_leak = _mm512_loadu_ps(&state.leakConductance[idx]);
        
        // Perform vector operations
        __m512 v_leak_contrib = _mm512_sub_ps(v_rest, v_pot);
        v_leak_contrib = _mm512_div_ps(v_leak_contrib, v_leak);
        
        // Update membrane potentials
        __m512 v_new_pot = _mm512_add_ps(v_pot, 
                                        _mm512_mul_ps(v_leak_contrib, dt_vec));
        
        // Check for spikes
        __mmask16 spike_mask = _mm512_cmpge_ps_mask(v_new_pot, v_thres);
        
        if ( spike_mask ) {
            // Handle spikes (more complex logic)
        }
        
        // Store results
        _mm512_storeu_ps(&newState.potentials[idx], v_new_pot);
    }
    
    // Handle remainder with scalar code
    for (size_t i = 0; i < remainder; ++i) {
        size_t idx = startIdx + iterations * 16 + i;
        // Scalar update
    }
}
```

#### Cache-Aware Blocking
```cpp
// Blocking for cache optimization
void processNeuronsCacheAware(NeuronState& state,
                              size_t start, size_t end) {
    const size_t BLOCK_SIZE = 64;  // 64 neurons per block
    
    for (size_t blockStart = start; blockStart < end; 
         blockStart += BLOCK_SIZE) {
        size_t blockEnd = std::min(blockStart + BLOCK_SIZE, end);
        
        // Process block with all data in cache
        for (size_t i = blockStart; i < blockEnd; ++i) {
            NeuronState& neuron = state.neurons[i];
            
            // Keep frequently accessed data in registers
            MembranePotential V = neuron.state.membranePotential;
            MembranePotential threshold = neuron.state.threshold;
            
            // Update
            updateNeuronState(neuron, V, threshold);
            
            // Check for spike
            if (V >= threshold) {
                handleSpike(neuron, V);
            }
        }
        
        // Ensure data written back to cache
        _mm_sfence();  // Store fence for memory ordering
    }
}
```

### 2.2 Branch Prediction Optimization

#### Minimize Branches in Hot Paths
```cpp
// Before: Many branches
bool checkThreshold() const {
    if (membranePotential >= threshold) {
        return true;
    } else {
        return false;
    }
}

// After: Use bit operations
bool checkThreshold() const {
    return (membranePotential >= threshold) != 0;
}

// Or use lookup table for common cases
const bool thresholdTable[2][2] = {
    {false, false},  // threshold < membranePotential < threshold
    {true, true}     // threshold >= membranePotential
};

bool checkThreshold() const {
    return thresholdTable[membranePotential >= threshold][threshold == threshold];
}
```

### 2.3 Memory Bandwidth Optimization

#### Stream Processing
```cpp
// Process neurons in streams for prefetching
void processNeuronStream(NeuronState& state,
                         size_t start, size_t count) {
    const size_t STREAM_SIZE = 1024;
    
    for (size_t pos = start; pos < start + count; pos += STREAM_SIZE) {
        size_t currentStream = std::min(pos + STREAM_SIZE, start + count);
        
        // Prefetch next stream
        if (pos + STREAM_SIZE < start + count) {
            _mm_prefetch(&state.potentials[pos + STREAM_SIZE], _MM_HINT_T0);
            _mm_prefetch(&state.thresholds[pos + STREAM_SIZE], _MM_HINT_T0);
        }
        
        // Process current stream
        for (size_t i = pos; i < currentStream; ++i) {
            // Update neuron i
            updateNeuron(i);
        }
    }
}
```

## 3. Hardware-Specific Optimizations

### 3.1 x86_64 Optimizations

#### CPU Feature Detection
```cpp
class CPUFeatures {
public:
    static CPUFeatures detect() {
        CPUFeatures features;
        
        // AVX-512 support
        features.hasAVX512 = checkCPUID(7, 0x00, 0x00, 0x20);
        
        // AVX2 support
        features.hasAVX2 = checkCPUID(7, 0x00, 0x00, 0x20);
        
        // SSE4.2 support
        features.hasSSE42 = checkCPUID(1, 0x00, 0x00, 0x00);
        
        // Large pages support
        features.hasLargePages = checkOSSupport();
        
        return features;
    }
    
private:
    bool checkCPUID(uint32_t leaf, uint32_t subleaf,
                    uint32_t bit, uint32_t mask) {
        // CPUID assembly implementation
    }
    
    bool checkOSSupport();
};
```

#### Dynamic Dispatch
```cpp
class VectorizedNeuronProcessor {
public:
    enum class Implementation {
        Scalar,
        SSE,
        AVX2,
        AVX512
    };
    
    VectorizedNeuronProcessor() {
        features = CPUFeatures::detect();
        impl = selectBestImplementation(features);
    }
    
    void updateNeurons(NeuronState& state, size_t count) {
        switch (impl) {
            case Implementation::Scalar:
                updateScalar(state, count);
                break;
            case Implementation::SSE:
                updateSSE(state, count);
                break;
            case Implementation::AVX2:
                updateAVX2(state, count);
                break;
            case Implementation::AVX512:
                updateAVX512(state, count);
                break;
        }
    }
    
private:
    CPUFeatures features;
    Implementation impl;
    
    Implementation selectBestImplementation(const CPUFeatures& features) {
        if (features.hasAVX512) return Implementation::AVX512;
        if (features.hasAVX2) return Implementation::AVX2;
        if (features.hasSSE42) return Implementation::SSE;
        return Implementation::Scalar;
    }
};
```

### 3.2 NUMA Optimization

#### Memory Affinity
```cpp
class NUMAProcessor {
public:
    NUMAProcessor() {
        numNodes = getNumberOfNUMANodes();
        allocateLocalMemory();
    }
    
    template<typename T>
    T* allocateOnNode(size_t size, int node) {
        // Allocate memory on specific NUMA node
        void* ptr = numa_alloc_onnode(sizeof(T) * size, node);
        return static_cast<T*>(ptr);
    }
    
    void processNeurons(NeuronState& state, int preferredNode) {
        // Assign neurons to NUMA nodes based on affinity
        size_t neuronsPerNode = state.getNeuronCount() / numNodes;
        
        for (int node = 0; node < numNodes; ++node) {
            size_t start = node * neuronsPerNode;
            size_t end = (node == numNodes - 1) ? 
                        state.getNeuronCount() : (node + 1) * neuronsPerNode;
            
            // Bind thread to NUMA node
            bindThreadToNUMANode(node);
            
            // Process neurons on this node
            processNeuronRange(state, start, end);
        }
    }
    
private:
    int numNodes;
    std::vector<void*> nodeMemory;
};
```

## 4. Algorithm Optimization

### 4.1 Sparse Connectivity

#### Adjacency List Optimization
```cpp
class SparseConnectivity {
public:
    // Source -> Synapse indices
    std::vector<std::vector<size_t>> outgoing;
    // Target -> Synapse indices  
    std::vector<std::vector<size_t>> incoming;
    std::vector<SynapseRecord> synapses;
    
    // Build connectivity efficiently
    void buildConnectivity(const std::vector<NeuronConnection>& connections) {
        outgoing.resize(numNeurons);
        incoming.resize(numNeurons);
        synapses.reserve(connections.size());
        
        for (size_t i = 0; i < connections.size(); ++i) {
            const auto& conn = connections[i];
            
            size_t synapseId = synapses.size();
            
            outgoing[conn.source].push_back(synapseId);
            incoming[conn.target].push_back(synapseId);
            
            synapses.push_back(SynapseRecord{
                .source = conn.source,
                .target = conn.target,
                .weight = conn.weight,
                .delay = conn.delay,
                .plasticityFlags = conn.plasticityFlags
            });
        }
    }
    
    // Efficient synapse iteration
    void forEachSynapse(const std::function<void(const SynapseRecord&)>& func) {
        for (const auto& synapse : synapses) {
            func(synapse);
        }
    }
    
    // Efficient incoming synapse iteration
    void forEachIncomingSynapse(NeuronId target, 
                               const std::function<void(const SynapseRecord&)>& func) {
        for (size_t synapseId : incoming[target]) {
            func(synapses[synapseId]);
        }
    }
};
```

#### Time Bucket Optimization
```cpp
class TimeBucketQueue {
public:
    static constexpr size_t NUM_BUCKETS = 1024;
    static constexpr Timestamp MAX_DELAY = 100.0;  // ms
    
    struct Event {
        Timestamp time;
        EventType type;
        void* data;
    };
    
    // Event queue for immediate events
    std::vector<Event> immediateEvents;
    
    // Time-bucketed queues for delayed events
    std::vector<std::vector<Event>> timeBuckets;
    
    TimeBucketQueue() {
        timeBuckets.resize(NUM_BUCKETS);
    }
    
    void schedule(Event event, Timestamp delay) {
        if (delay <= 0) {
            // Immediate event
            immediateEvents.push_back(event);
            return;
        }
        
        // Convert delay to bucket index
        size_t bucket = static_cast<size_t>(delay / (MAX_DELAY / NUM_BUCKETS));
        bucket = std::min(bucket, NUM_BUCKETS - 1);
        
        timeBuckets[bucket].push_back(event);
    }
    
    void deliverEvents(Timestamp currentTime) {
        // Process immediate events
        for (const auto& event : immediateEvents) {
            processEvent(event);
        }
        immediateEvents.clear();
        
        // Process time-bucketed events
        for (auto& bucket : timeBuckets) {
            for (const auto& event : bucket) {
                processEvent(event);
            }
            bucket.clear();
        }
    }
};
```

### 4.2 Event Processing Optimization

#### Spike Processing
```cpp
class SpikeProcessor {
public:
    // Event queue for spikes
    EventQueue spikeQueue;
    
    // Time-bucketed delayed spikes
    TimeBucketQueue delayedSpikes;
    
    void processSpikes(Timestamp currentTime) {
        // Process immediate spikes
        SpikeEvent spike;
        while (spikeQueue.pop(spike)) {
            handleImmediateSpike(spike.neuronId, spike.time);
        }
        
        // Process delayed spikes
        delayedSpikes.deliverEvents(currentTime);
    }
    
    void handleImmediateSpike(NeuronId neuronId, Timestamp time) {
        // Fast path for immediate spikes
        Neuron& neuron = getNeuron(neuronId);
        
        if (neuron.isRefractory()) {
            return;
        }
        
        // Generate spike output
        generateSpikeOutput(neuronId, time);
        
        // Update synaptic connections
        neuron.forEachOutgoingSynapse([this, time](Synapse& synapse) {
            // Create delayed spike event
            SpikeEvent delayedSpike{
                .neuronId = synapse.target,
                .time = time + synapse.delay
            };
            
            delayedSpikes.schedule(delayedSpike, synapse.delay);
        });
        
        // Update plasticity
        neuron.updatePlasticityForSpike(time);
    }
};
```

## 5. Profiling and Benchmarking

### 5.1 Performance Metrics

#### Key Performance Indicators
```cpp
struct PerformanceMetrics {
    double simulationSpeed;        // Steps per second
    double neuronsPerSecond;       // Effective neuron processing rate
    double spikesPerSecond;        // Spike processing rate
    double memoryBandwidth;        // GB/s
    double cacheEfficiency;        // Cache hit rate
    double cpuUtilization;         // CPU core utilization
    
    // Performance counters
    uint64_t instructionsRetired;
    uint64_t cacheMisses;
    uint64_t branchMispredictions;
    uint64_t memoryStalls;
    
    // Hardware-specific counters
    uint64_t l1dCacheHits;
    uint64_t l1dCacheMisses;
    uint64_t l2CacheHits;
    uint64_t l2CacheMisses;
    uint64_t l3CacheHits;
    uint64_t l3CacheMisses;
    
    double calculateIPS() const {
        return (double)instructionsRetired / simulationSpeed;
    }
    
    double calculateCacheMissRate() const {
        uint64_t totalAccesses = l1dCacheHits + l1dCacheMisses;
        if (totalAccesses == 0) return 0.0;
        return (double)l1dCacheMisses / totalAccesses;
    }
};
```

### 5.2 Benchmarking Methodology

#### Standard Benchmarks
```bash
# Build with performance flags
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run benchmark
./nlm --benchmark --neurons=10000 --steps=10000

# Run scaling benchmark
./nlm --scaling-benchmark

# Run memory bandwidth test
./nlm --memory-bandwidth-test

# Run threading test
./nlm --threading-test --threads=4
```

#### Performance Analysis Script
```bash
#!/bin/bash
# analyze_performance.sh

# Run performance tests
./nlm --benchmark --neurons=1000 --steps=1000 > perf_results.txt

# Extract key metrics
SIMULATION_SPEED=$(grep "Simulation speed:" perf_results.txt | awk '{print $3}')
NEURONS_PER_SEC=$(grep "Neurons/sec:" perf_results.txt | awk '{print $3}')
SPIKES_PER_SEC=$(grep "Spikes/sec:" perf_results.txt | awk '{print $3}')

# Generate performance report
cat > performance_report.html << EOF
<!DOCTYPE html>
<html>
<head><title>NLM Performance Report</title></head>
<body>
    <h1>NLM Performance Analysis</h1>
    <p>Benchmark: 1,000 neurons, 1,000 steps</p>
    <table>
        <tr><th>Metric</th><th>Value</th></tr>
        <tr><td>Simulation Speed</td><td>${SIMULATION_SPEED} steps/sec</td></tr>
        <tr><td>Neurons/sec</td><td>${NEURONS_PER_SEC}</td></tr>
        <tr><td>Spikes/sec</td><td>${SPIKES_PER_SEC}</td></tr>
    </table>
</body>
</html>
EOF

echo "Performance report generated: performance_report.html"
```

### 5.3 Profiling Tools

#### GPROF Profiling
```bash
# Generate profile data
./nlm --benchmark --neurons=1000 --steps=1000

# Generate gprof output
gprof nlm gmon.out > profile.txt

# Analyze profile
~/tools/gprof2html profile.txt -o profile.html
```

#### Valgrind Memory Profiling
```bash
# Run with Valgrind
valgrind --tool=massif --stacks=yes ./nlm --benchmark

# Analyze memory usage
ms_print massif.out.12345 > memory_profile.txt

# Generate visualization
ms_print --format=gui massif.out.12345
```

#### Intel VTune
```bash
# Collect hotspots
amplxe-cl --collect hotspots --result-dir=vtune_results ./nlm --benchmark

# Generate report
amplxe-cl --report top hotspots --result-dir=vtune_results

# Analyze memory usage
amplxe-cl --collect memory-access --result-dir=vtune_mem ./nlm --benchmark
```

## 6. Common Performance Issues and Solutions

### 6.1 Memory Bloat

#### Problem: Excessive Memory Usage
```cpp
// Problem: Allocating new objects in hot path
void processNeurons(std::vector<Neuron*>& neurons) {
    for (Neuron* neuron : neurons) {
        // Allocate new synapse each time
        synapse = new Synapse(neuron->getId(), target->getId());
        neuron->addOutgoingSynapse(synapse);
    }
}

// Solution: Use memory pools
class SynapsePool {
public:
    Synapse* allocate(NeuronId source, NeuronId target) {
        if (freeList_.empty()) {
            // Allocate new batch
            for (size_t i = 0; i < BATCH_SIZE; ++i) {
                Synapse* synapse = new Synapse(freeList_.size(), source, target);
                freeList_.push_back(synapse);
            }
        }
        
        Synapse* synapse = freeList_.back();
        freeList_.pop_back();
        return synapse;
    }
    
    void deallocate(Synapse* synapse) {
        freeList_.push_back(synapse);
    }
    
private:
    static constexpr size_t BATCH_SIZE = 1024;
    std::vector<Synapse*> freeList_;
};
```

### 6.2 Cache Misses

#### Problem: Poor Cache Locality
```cpp
// Problem: Scattered data access
void updateNeurons(const std::vector<Neuron>& neurons) {
    for (const Neuron& neuron : neurons) {
        // Access scattered fields
        float potential = neuron.getState().membranePotential;
        float threshold = neuron.getState().threshold;
        // ... other fields
        
        // Update
        updateNeuronState(neuron, potential, threshold);
    }
}

// Solution: Structure of Arrays
struct NeuronState {
    std::vector<float> membranePotentials;
    std::vector<float> thresholds;
    std::vector<float> restingPotentials;
    // ... other arrays
    
    // Helper functions for cache-friendly access
    float getMembranePotential(size_t idx) const {
        return membranePotentials[idx];
    }
    
    void setMembranePotential(size_t idx, float value) {
        membranePotentials[idx] = value;
    }
    
    // Other helper functions...
};
```

### 6.3 Thread Contention

#### Problem: Lock Contention
```cpp
// Problem: Global lock for all neurons
class GlobalNeuronLock {
public:
    void lockAll() {
        std::lock_guard<std::mutex> lock(globalMutex_);
        // Lock all neurons - terrible performance!
    }
    
    void unlockAll() {
        // Unlock all neurons
    }
    
private:
    static std::mutex globalMutex_;
};

// Solution: Fine-grained locking
class NeuronLock {
public:
    void lock() {
        // Lock specific neuron
        neuronMutex_.lock();
    }
    
    void unlock() {
        neuronMutex_.unlock();
    }
    
    bool tryLock() {
        return neuronMutex_.try_lock();
    }
    
private:
    std::mutex neuronMutex_;
};

class FineGrainedNeuronManager {
public:
    void updateNeurons() {
        // Use per-neuron locks
        for (auto& neuron : neurons_) {
            if (neuron.tryLock()) {
                neuron.update();
                neuron.unlock();
            }
        }
    }
    
private:
    std::vector<Neuron> neurons_;
    std::vector<std::unique_ptr<NeuronLock>> locks_;
};
```

### 6.4 Inefficient Algorithms

#### Problem: O(N²) Complexity
```cpp
// Problem: O(N²) spike processing
void processAllSpikes(const std::vector<Neuron>& neurons) {
    for (size_t i = 0; i < neurons.size(); ++i) {
        for (size_t j = i + 1; j < neurons.size(); ++j) {
            // Process interaction between neurons i and j
            processInteraction(neurons[i], neurons[j]);
        }
    }
}

// Solution: Sparse connectivity O(k) where k << N²
class SparseSpikeProcessor {
public:
    void processSpikes(const std::vector<Neuron>& neurons) {
        for (const Neuron& neuron : neurons) {
            // Process only active neurons
            if (neuron.isActive()) {
                // Process outgoing connections (sparse)
                for (const auto& connection : neuron.getOutgoingConnections()) {
                    processSpike(connection.target, connection.time);
                }
            }
        }
    }
    
private:
    // Use adjacency list for sparse connectivity
    std::vector<std::vector<size_t>> outgoingConnections_;
};
```

## 7. Advanced Optimization Techniques

### 7.1 Precomputation

#### Precompute Common Values
```cpp
class PrecomputedNeuronState {
public:
    PrecomputedNeuronState(const NeuronState& state, TimestepDuration dt) {
        // Precompute frequently used values
        for (size_t i = 0; i < state.getNeuronCount(); ++i) {
            const Neuron& neuron = state.getNeuron(i);
            
            float tau = neuron.getState().leakConductance;
            float C = neuron.getState().membraneCapacitance;
            
            // Precompute integration factors
            leakFactors[i] = 1.0f / tau;
            currentFactors[i] = 1.0f / C;
            
            // Precompute spike thresholds
            spikeThresholds[i] = neuron.getState().threshold;
        }
    }
    
    float getLeakFactor(size_t idx) const {
        return leakFactors[idx];
    }
    
    float getCurrentFactor(size_t idx) const {
        return currentFactors[idx];
    }
    
    float getSpikeThreshold(size_t idx) const {
        return spikeThresholds[idx];
    }
    
private:
    std::vector<float> leakFactors;
    std::vector<float> currentFactors;
    std::vector<float> spikeThresholds;
};
```

### 7.2 Loop Unrolling

#### Unroll Hot Loops
```cpp
// Manually unroll hot loop
void processNeuronsUnrolled(const NeuronState& state) {
    const size_t NEURONS_PER_ITERATION = 8;
    
    for (size_t i = 0; i < state.getNeuronCount(); i += NEURONS_PER_ITERATION) {
        // Unroll loop body 8 times
        for (size_t j = 0; j < NEURONS_PER_ITERATION && i + j < state.getNeuronCount(); ++j) {
            size_t idx = i + j;
            Neuron& neuron = state.getNeuron(idx);
            
            // Process neuron j
            processNeuron(j, neuron);
        }
    }
}
```

### 7.3 Software Pipelining

#### Pipeline Processing
```cpp
class SoftwarePipeline {
public:
    void processNeurons(const NeuronState& input,
                       NeuronState& output,
                       size_t pipelineDepth = 4) {
        // Pipeline stages
        std::array<NeuronState, 4> pipeline;
        
        for (size_t i = 0; i < input.getNeuronCount(); ++i) {
            // Stage 1: Load
            pipeline[0] = loadNeuronState(input, i);
            
            // Stages 2-4: Process (pipelined)
            for (size_t stage = 1; stage < pipelineDepth; ++stage) {
                pipeline[stage] = processStage(pipeline[stage - 1]);
            }
            
            // Stage 5: Store
            storeNeuronState(output, pipeline[0]);
        }
    }
    
private:
    NeuronState loadNeuronState(const NeuronState& state, size_t idx) {
        NeuronState neuron = state.getNeuron(idx);
        // Load and prepare
        return neuron;
    }
    
    NeuronState processStage(const NeuronState& input) {
        // Process one pipeline stage
        return input;
    }
    
    void storeNeuronState(NeuronState& output, const NeuronState& input) {
        // Store processed result
        output.setNeuron(input.getId(), input);
    }
};
```

## 8. Hardware-Specific Code

### 8.1 AVX-512 Assembly Intrinsics

#### Vectorized Neuron Update
```cpp
// Inline assembly for maximum performance
void updateNeuronsAVX512Asm(const NeuronState& state,
                           std::vector<NeuronState>& newState,
                           size_t startIdx, size_t count) {
    __asm__ volatile (
        "// AVX-512 neuron update\n"
        "vxorps %%zmm0, %%zmm0, %%zmm0\n"          // Clear zmm0
        "vxorps %%zmm1, %%zmm1, %%zmm1\n"          // Clear zmm1
        
        // Load neuron data
        "lea (%[state]), %%rsi\n"                 // RSI = state pointer
        "mov %k[count], %%r9d\n"                 // R9 = count
        
        "loop_start:\n"
        "test %%r9d, %%r9d\n"                    // Check count
        "je loop_end\n"
        
        // Process 16 neurons
        "mov (%[state]), %%xmm2\n"               // Load first neuron
        // ... more assembly code
        
        "dec %%r9d\n"
        "jmp loop_start\n"
        "loop_end:\n"
        
        : // No outputs
        : [state]("r"(&state)),
          [count]("r"(count))
        : "zmm0", "zmm1", "zmm2", "zmm3", "zmm4", 
          "zmm5", "zmm6", "zmm7", "zmm8", "zmm9",
          "zmm10", "zmm11", "zmm12", "zmm13", "zmm14", "zmm15",
          "rsi", "r9", "memory"
    );
}
```

### 8.2 Memory-Mapped Files

#### For Large Network Storage
```cpp
class MemoryMappedNeuronState {
public:
    MemoryMappedNeuronState(const std::string& filename,
                           size_t neuronCount) {
        // Open file
        fileDescriptor_ = open(filename.c_str(), O_RDWR | O_CREAT, 0644);
        
        // Set file size
        ftruncate(fileDescriptor_, sizeof(NeuronState) * neuronCount);
        
        // Memory map
        mappedData_ = mmap(nullptr, sizeof(NeuronState) * neuronCount,
                          PROT_READ | PROT_WRITE, MAP_SHARED,
                          fileDescriptor_, 0);
        
        neuronCount_ = neuronCount;
    }
    
    ~MemoryMappedNeuronState() {
        munmap(mappedData_, sizeof(NeuronState) * neuronCount_);
        close(fileDescriptor_);
    }
    
    NeuronState& getNeuron(size_t idx) {
        return reinterpret_cast<NeuronState*>(mappedData_)[idx];
    }
    
    const NeuronState& getNeuron(size_t idx) const {
        return reinterpret_cast<const NeuronState*>(mappedData_)[idx];
    }
    
private:
    int fileDescriptor_;
    void* mappedData_;
    size_t neuronCount_;
};
```

## 9. Performance Monitoring

### 9.1 Hardware Performance Counters

#### Performance Counter Collection
```cpp
class PerformanceCounter {
public:
    enum Counter {
        InstructionsRetired,
        CacheMisses,
        BranchMispredictions,
        MemoryStalls,
        L1DCacheHits,
        L1DCacheMisses,
        L2CacheHits,
        L2CacheMisses,
        L3CacheHits,
        L3CacheMisses
    };
    
    void start() {
        // Read performance counters
        for (auto counter : allCounters_) {
            readCounter(counter);
        }
    }
    
    void stop() {
        // Calculate differences
        for (auto counter : allCounters_) {
            uint64_t current = getCounterValue(counter);
            deltaValues_[counter] = current - previousValues_[counter];
            previousValues_[counter] = current;
        }
    }
    
    uint64_t getCounterValue(Counter counter) const {
        // Read from MSR or performance monitoring hardware
        return readMSR(counter);
    }
    
    double getInstructionsPerSecond() const {
        // Calculate instructions per second
        return (double)deltaValues_[InstructionsRetired] / timeInterval_;
    }
    
    double getCacheMissRate() const {
        // Calculate cache miss rate
        uint64_t cacheHits = deltaValues_[L1DCacheHits] + 
                           deltaValues_[L2CacheHits] + 
                           deltaValues_[L3CacheHits];
        uint64_t cacheMisses = deltaValues_[CacheMisses];
        uint64_t totalAccesses = cacheHits + cacheMisses;
        
        if (totalAccesses == 0) return 0.0;
        return (double)cacheMisses / totalAccesses;
    }
    
private:
    std::vector<Counter> allCounters_ = {
        InstructionsRetired, CacheMisses, BranchMispredictions,
        MemoryStalls, L1DCacheHits, L1DCacheMisses, L2CacheHits,
        L2CacheMisses, L3CacheHits, L3CacheMisses
    };
    
    std::map<Counter, uint64_t> previousValues_;
    std::map<Counter, uint64_t> deltaValues_;
    double timeInterval_ = 1.0;  // seconds
    
    uint64_t readMSR(Counter counter);
};
```

## 10. Testing and Validation

### 10.1 Performance Regression Testing

#### Automated Performance Testing
```bash
#!/bin/bash
# run_performance_tests.sh

# Run baseline performance test
./nlm --benchmark --neurons=1000 --steps=1000 > baseline_results.txt

# Run current version
./nlm --benchmark --neurons=1000 --steps=1000 > current_results.txt

# Compare results
python3 analyze_performance.py baseline_results.txt current_results.txt > comparison.txt

# Check for regressions
if python3 check_regression.py comparison.txt; then
    echo "Performance regression detected!"
    exit 1
else
    echo "Performance within acceptable limits."
fi
```

#### Regression Analysis
```python
#!/usr/bin/env python3
import json
import numpy as np
from scipy import stats

def analyze_performance(baseline_file, current_file):
    # Load baseline and current results
    baseline = load_results(baseline_file)
    current = load_results(current_file)
    
    # Compare key metrics
    metrics = ['simulation_speed', 'neurons_per_sec', 'spikes_per_sec']
    
    for metric in metrics:
        baseline_value = baseline[metric]
        current_value = current[metric]
        
        # Calculate percentage change
        change = (current_value - baseline_value) / baseline_value * 100;
        
        # Check if change is within acceptable limits
        if abs(change) > 5.0:  # 5% threshold
            print(f"WARNING: {metric} changed by {change:.2f}%")
            
        print(f"{metric}: {baseline_value} -> {current_value} ({change:+.2f}%)")

def load_results(filename):
    # Parse results file
    results = {}
    with open(filename, 'r') as f:
        for line in f:
            if ':' in line:
                key, value = line.split(':', 1)
                results[key.strip()] = float(value.strip())
    return results

if __name__ == '__main__':
    import sys
    analyze_performance(sys.argv[1], sys.argv[2])
```

## 11. Performance Tuning Guidelines

### 11.1 Target Scales

#### Expected Performance at Different Scales
```cpp
struct PerformanceTargets {
    size_t neurons;
    size_t synapses;
    double memoryGB;
    double stepsPerSec;
    double neuronsPerSec;
    
    // Scale benchmarks
    static PerformanceTargets getScale(size_t neuronCount) {
        if (neuronCount <= 1000) {
            return {1000, ~10000, 1.0, 10000, 1000};
        } else if (neuronCount <= 10000) {
            return {10000, ~100000, 10.0, 5000, 500};
        } else if (neuronCount <= 100000) {
            return {100000, ~1000000, 100.0, 1000, 100};
        } else {
            return {1000000, ~10000000, 1000.0, 100, 10};
        }
    }
};
```

### 11.2 Tuning Checklist

#### For Each Performance Issue
1. **Profile first**: Always measure before optimizing
2. **Measure impact**: Compare before/after results
3. **Isolate bottleneck**: Test components separately
4. **Optimize hot path**: Focus on frequently called code
5. **Consider cache**: Optimize memory access patterns
6. **Balance tradeoffs**: Speed vs. readability vs. maintainability
7. **Test on target hardware**: Verify optimizations work on actual hardware

## 12. Documentation and Examples

### 12.1 Performance Examples

#### Example: Neuron Update Optimization
```cpp
// Before: Inefficient implementation
void updateNeuronSlow(NeuronState& state) {
    // Multiple branches in hot path
    if (state.refractory > 0) {
        decrementRefractory(state);
        return;
    }
    
    if (state.potential >= state.threshold) {
        handleSpike(state);
        return;
    }
    
    // Integration logic
    integratePotential(state);
}

// After: Optimized implementation
void updateNeuronFast(NeuronState& state) {
    // Early exit for common case
    if (state.refractory > 0) {
        decrementRefractory(state);
        return;
    }
    
    // Fast path for non-spiking
    integratePotential(state);
    
    // Check for spike
    if (state.potential >= state.threshold) {
        handleSpike(state);
    }
}
```

#### Example: Cache-Friendly Processing
```cpp
// Process neurons in cache-friendly blocks
void processNeuronsCacheFriendly(NeuronState& state) {
    const size_t BLOCK_SIZE = 64;  // 64 neurons per block
    
    for (size_t blockStart = 0; blockStart < state.neuronCount;
         blockStart += BLOCK_SIZE) {
        
        size_t blockEnd = std::min(blockStart + BLOCK_SIZE,
                                   state.neuronCount);
        
        // Process block - all data should fit in cache
        for (size_t i = blockStart; i < blockEnd; ++i) {
            Neuron& neuron = state.neurons[i];
            
            // Access frequently used data
            float potential = neuron.getState().potential;
            float threshold = neuron.getState().threshold;
            
            // Update
            updateNeuronState(neuron, potential, threshold);
        }
        
        // Ensure cache coherence
        _mm_sfence();
    }
}
```

## 13. Future Performance Improvements

### 13.1 Hardware Acceleration

#### GPU Computing
```cpp
// CUDA implementation for GPU acceleration
__global__ void updateNeuronsGPU(const NeuronState* deviceState,
                                NeuronState* deviceNewState,
                                size_t neuronCount) {
    size_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (idx < neuronCount) {
        // Update neuron idx
        updateNeuronGPU(deviceState[idx], deviceNewState[idx]);
    }
}

// CPU-GPU hybrid processing
class HybridProcessor {
public:
    void processNeurons(const NeuronState& state,
                        NeuronState& output,
                        bool useGPU = true) {
        if (useGPU && hasGPU()) {
            // Process on GPU
            processOnGPU(state, output);
        } else {
            // Process on CPU
            processOnCPU(state, output);
        }
    }
    
private:
    bool hasGPU() const {
        // Check GPU availability
    }
    
    void processOnGPU(const NeuronState& state,
                     NeuronState& output) {
        // Copy to GPU
        copyToGPU(state);
        
        // Launch GPU kernel
        updateNeuronsGPU<<<blocks, threads>>>(deviceState_, deviceNewState_, neuronCount_);
        
        // Copy back
        copyFromGPU(output);
    }
    
    void processOnCPU(const NeuronState& state,
                     NeuronState& output) {
        // CPU implementation
    }
    
    void* deviceState_;
    void* deviceNewState_;
    size_t neuronCount_;
};
```

### 13.2 Software-Pipelined Processing

#### Advanced pipelining
```cpp
class SoftwarePipelineProcessor {
public:
    void processNeuronsPipeline(const NeuronState& input,
                                NeuronState& output,
                                size_t pipelineDepth = 8) {
        // Multiple pipeline stages
        std::array<NeuronState, 8> pipeline;
        
        for (size_t i = 0; i < input.neuronCount; ++i) {
            // Stage 1: Input
            pipeline[0] = loadInput(input, i);
            
            // Stages 2-7: Process
            for (size_t stage = 1; stage < pipelineDepth; ++stage) {
                pipeline[stage] = processStage(pipeline[stage - 1], stage);
            }
            
            // Stage 8: Output
            storeOutput(output, pipeline[0]);
        }
    }
    
private:
    // Different processing stages
    NeuronState stageLoad(const NeuronState& input, size_t idx) {
        return input.getNeuron(idx);
    }
    
    NeuronState stagePreprocess(const NeuronState& input, size_t stage) {
        // Preprocessing specific to stage
        return input;
    }
    
    NeuronState stageCompute(const NeuronState& input, size_t stage) {
        // Core computation
        return input;
    }
    
    NeuronState stagePostprocess(const NeuronState& input, size_t stage) {
        // Postprocessing specific to stage
        return input;
    }
    
    NeuronState stageOutput(const NeuronState& input, size_t stage) {
        // Output preparation
        return input;
    }
};
```

## 14. Conclusion

Performance optimization is an ongoing process in NLM. As hardware evolves and new algorithms are developed, performance will continue to improve. Key principles for successful performance optimization:

1. **Measure first**: Always profile before optimizing
2. **Focus on hot paths**: Optimize frequently executed code
3. **Consider memory**: Optimize memory access patterns
4. **Balance tradeoffs**: Consider correctness, maintainability, and performance
5. **Test on target hardware**: Verify optimizations work on actual deployment hardware
6. **Profile continuously**: Performance characteristics change over time

The NLM framework provides a solid foundation for high-performance neural simulation that can scale from thousands to millions of neurons. Through careful optimization and hardware-specific tuning, NLM can achieve performance suitable for real-time and large-scale neural modeling applications.

This document will be continuously updated as new optimization techniques are developed and hardware capabilities evolve.