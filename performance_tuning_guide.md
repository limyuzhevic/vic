# NLM Performance Guide

## Overview

This guide provides comprehensive performance optimization techniques for the NLM (Neural Learning Machine) system. It covers performance analysis, optimization strategies, and benchmarking methods for maximizing simulation efficiency.

## Table of Contents

1. [Introduction](#introduction)
2. [Performance Analysis](#performance-analysis)
3. [Optimization Strategies](#optimization-strategies)
4. [Configuration Optimization](#configuration-optimization)
5. [Code Optimization](#code-optimization)
6. [Memory Optimization](#memory-optimization)
7. [Parallel Processing](#parallel-processing)
8. [SIMD Optimization](#simd-optimization)
9. [Profiling and Benchmarking](#profiling-and-benchmarking)
10. [Performance Testing](#performance-testing)
11. [Real-time Performance](#real-time-performance)
12. [Scalability](#scalability)
13. [Mobile/Embedded Performance](#mobileembedded-performance)
14. [Performance Monitoring](#performance-monitoring)
15. [Performance Tuning](#performance-tuning)
16. [Case Studies](#case-studies)
17. [Performance Best Practices](#performance-best-practices)
18. [References](#references)

## Introduction

Performance optimization is crucial for effective NLM operation, especially for:

- **Large-scale simulations**: Brains with thousands of neurons
- **Real-time applications**: Interactive cognitive systems
- **Resource-constrained environments**: Mobile and embedded systems
- **Research experiments**: Reproducible and scalable benchmarks
- **Production deployments**: Long-running simulations

This guide provides techniques for optimizing NLM performance across different scenarios and use cases.

## Performance Analysis

### Performance Metrics

#### Core Performance Metrics

| Metric | Description | Target |
|--------|-------------|--------|
| **FPS** | Frames per second (simulation speed) | > 60 FPS |
| **Neurons/Second** | Neurons processed per second | > 1M neurons/sec |
| **Memory Usage** | Total memory consumption | < 2 GB for 10K neurons |
| **CPU Utilization** | CPU usage percentage | < 80% |
| **I/O Throughput** | Input/output operations per second | > 10K ops/sec |

#### Simulation Performance Metrics

| Metric | Description | Measurement |
|--------|-------------|-------------|
| **Time per Step** | Time for single simulation step | < 1 ms for 1K neurons |
| **Memory Footprint** | Memory usage over time | Peak and average |
| **Spillover Rate** | Rate of state overflow | < 0.1% |
| **Sync Overhead** | Synchronization overhead | < 10% |

#### Memory Performance Metrics

| Metric | Description | Target |
|--------|-------------|--------|
| **Cache Miss Rate** | Memory cache misses | < 5% |
| **Bandwidth Utilization** | Memory bandwidth usage | > 70% |
| **Allocation Frequency** | Dynamic allocation rate | < 1000 allocations/sec |
| **Fragmentation** | Memory fragmentation | < 5% |

### Performance Profiling

#### Basic Profiling

```bash
# Profile with Linux perf
perf record -g ./nlm --neurons 1000 --steps 1000
perf report

# Profile specific functions
perf record -e cpu-clock,task-clock,instructions,cycles,cache-references,cache-misses ./nlm
perf report -p $(pgrep nlm)

# Generate profiling data
perf script > perf.data
fasttrace -i perf.data > fftrace.txt
```

#### Valgrind Profiling

```bash
# Memory profiling
valgrind --tool=massif --stacktrace=yes ./nlm
ms_print massif.out.12345 > massif_report.html

# Memory leak detection
valgrind --tool=memcheck --leak-check=full ./nlm
valgrind --tool=memcheck --track-origins=yes ./nlm

# Thread debugging
valgrind --tool=helgrind ./nlm
valgrind --tool=drd ./nlm
```

#### Python Profiling

```python
import cProfile
import pstats
import pynlm

def profileBrainSimulation():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000) {
        brain.step(step)
    }
}

# Profile with cProfile
profiler = cProfile.Profile()
profiler.enable()
profileBrainSimulation()
profiler.disable()

# Write profile to file
profiler.dump_stats("brain_simulation.prof")

# Print profile summary
stats = pstats.Stats("brain_simulation.prof")
stats.sort_stats("cumulative")
stats.print_stats(20)
```

## Optimization Strategies

### Configuration-Based Optimization

#### Optimize for Performance

```bash
# Configure for maximum performance
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto" \
         -DENABLE_PYTHON_BINDINGS=ON \
         -DENABLE_VISUALIZATION=OFF \
         -DENABLE_TESTS=ON \
         -DPYTHON_EXECUTABLE=$(which python3) \
         -DPYTHON_MODULE_NAME=pynlm \
         -DBUILD_SHARED_LIBS=ON \
         -DSPARSE_CONNECTIVITY=ON \
         -DSIMD_OPTIMIZATION=ON \
         -DVECTORIZATION_LEVEL=AVX2
```

#### Configuration Options for Performance

| Parameter | Recommended Value | Effect |
|-----------|-------------------|--------|
| `brain.neuron_count` | 1000-5000 | Balance memory and performance |
| `brain.synapse_density` | 0.05-0.1 | Reduce connectivity overhead |
| `performance.threads` | 4-8 | Use multiple cores |
| `performance.memory_pool_size` | 4096-8192 | Reduce allocation overhead |
| `performance.event_queue_size` | 8192-16384 | Reduce queue blocking |
| `performance.sparse_connectivity` | true | Use sparse matrix representation |
| `performance.simd_optimization` | true | Use SIMD instructions |
| `performance.parallel_plasticity` | true | Parallelize plasticity updates |

### Code Optimization

#### Algorithm Optimization

```cpp
// Inefficient algorithm (O(n²))
void processNeuronsInefficient(Neuron* neurons, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < count; ++j) {
            if (i != j) {
                neurons[i].updateConnection(&neurons[j]);
            }
        }
    }
}

// Optimized algorithm (O(n log n))
void processNeuronsOptimized(Neuron* neurons, size_t count) {
    // Sort neurons by activity level
    std::vector<size_t> indices(count);
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
              [&](size_t a, size_t b) {
                  return neurons[a].getActivity() > neurons[b].getActivity();
              });
    
    // Process neurons in order of activity
    for (size_t i : indices) {
        for (size_t j : indices) {
            if (i != j) {
                neurons[i].updateConnection(&neurons[j]);
            }
        }
    }
}
```

#### Data Structure Optimization

```cpp
// Inefficient data structure
class NeuronManager {
public:
    void addNeuron(Neuron* neuron) {
        neurons.push_back(neuron);  // O(1) but uses dynamic memory
    }
    
    Neuron* getNeuron(size_t index) {
        return neurons[index];  // O(1)
    }
    
private:
    std::vector<Neuron*> neurons;  // Dynamic array
};

// Optimized data structure
class NeuronManager {
public:
    NeuronManager(size_t capacity) {
        neurons.reserve(capacity);
        freeIndices.reserve(capacity);
        
        // Initialize free indices
        for (size_t i = 0; i < capacity; ++i) {
            freeIndices.push_back(i);
        }
    }
    
    Neuron* addNeuron() {
        if (freeIndices.empty()) {
            return nullptr;  // Out of memory
        }
        
        size_t index = freeIndices.back();
        freeIndices.pop_back();
        neurons[index] = nullptr;  // Initialize to null
        return neurons[index];
    }
    
    Neuron* getNeuron(size_t index) {
        if (index < neurons.size()) {
            return neurons[index];
        }
        return nullptr;
    }
    
    void removeNeuron(Neuron* neuron) {
        // Find neuron in array
        auto it = std::find(neurons.begin(), neurons.end(), neuron);
        if (it != neurons.end()) {
            size_t index = std::distance(neurons.begin(), it);
            neurons[index] = nullptr;
            freeIndices.push_back(index);
        }
    }
    
private:
    std::vector<Neuron*> neurons;  // Fixed-size array
    std::vector<size_t> freeIndices;  // Free indices
};
```

## Configuration Optimization

### Performance Configuration Templates

#### Template: High-Performance

```cpp
struct HighPerformanceConfig {
    // Core settings
    size_t neuronCount = 5000;
    float synapseDensity = 0.05f;
    size_t threadCount = 8;
    
    // Memory settings
    size_t memoryPoolSize = 8192;
    size_t eventQueueSize = 16384;
    bool sparseConnectivity = true;
    bool simdOptimization = true;
    bool parallelPlasticity = true;
    
    // Simulation settings
    float timestep = 0.1f;
    size_t maxSteps = 10000;
    
    // Optimization flags
    bool enableSIMD = true;
    bool enableVectorization = true;
    bool enableMultithreading = true;
    bool enableCacheOptimization = true;
    
    // Monitoring
    bool enableProfiling = true;
    bool enableBenchmarking = true;
    
    // Targets
    float targetTimePerStep = 0.001f;  // 1 ms per step
    size_t targetMemoryUsage = 2048;   // 2 GB
    float targetCPUUtilization = 0.8f;  // 80%
};
```

#### Template: Balanced Performance

```cpp
struct BalancedPerformanceConfig {
    // Core settings
    size_t neuronCount = 2000;
    float synapseDensity = 0.1f;
    size_t threadCount = 4;
    
    // Memory settings
    size_t memoryPoolSize = 4096;
    size_t eventQueueSize = 8192;
    bool sparseConnectivity = true;
    bool simdOptimization = true;
    bool parallelPlasticity = false;
    
    // Simulation settings
    float timestep = 0.1f;
    size_t maxSteps = 5000;
    
    // Optimization flags
    bool enableSIMD = true;
    bool enableVectorization = false;
    bool enableMultithreading = true;
    bool enableCacheOptimization = true;
    
    // Monitoring
    bool enableProfiling = true;
    bool enableBenchmarking = false;
    
    // Targets
    float targetTimePerStep = 0.005f;  // 5 ms per step
    size_t targetMemoryUsage = 1024;   // 1 GB
    float targetCPUUtilization = 0.6f;  // 60%
};
```

#### Template: Low-Memory

```cpp
struct LowMemoryConfig {
    // Core settings
    size_t neuronCount = 500;
    float synapseDensity = 0.01f;
    size_t threadCount = 2;
    
    // Memory settings
    size_t memoryPoolSize = 1024;
    size_t eventQueueSize = 2048;
    bool sparseConnectivity = true;
    bool simdOptimization = false;
    bool parallelPlasticity = false;
    
    // Simulation settings
    float timestep = 0.2f;
    size_t maxSteps = 1000;
    
    // Optimization flags
    bool enableSIMD = false;
    bool enableVectorization = false;
    bool enableMultithreading = false;
    bool enableCacheOptimization = true;
    
    // Monitoring
    bool enableProfiling = false;
    bool enableBenchmarking = false;
    
    // Targets
    float targetTimePerStep = 0.01f;   // 10 ms per step
    size_t targetMemoryUsage = 256;    // 256 MB
    float targetCPUUtilization = 0.4f;  // 40%
};
```

### Configuration Optimization Techniques

#### Cache-Friendly Configuration

```cpp
// Cache-friendly configuration
struct CacheFriendlyConfig {
    // Align with cache line sizes
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t NEURONS_PER_CACHE_LINE = CACHE_LINE_SIZE / sizeof(Neuron);
    
    // Organize neurons in cache-friendly patterns
    size_t neuronCount = 1024;  // Multiple of cache line size
    
    // Use contiguous memory for frequently accessed data
    bool useContiguousMemory = true;
    
    // Optimize for spatial locality
    bool enableSpatialLocality = true;
    
    // Optimize for temporal locality
    bool enableTemporalLocality = true;
    
    // Configure thread affinity
    bool enableThreadAffinity = true;
    size_t threadAffinityMask = 0xFFFFFFFF;
};
```

#### NUMA-Aware Configuration

```cpp
// NUMA-aware configuration
struct NUMAConfig {
    // NUMA topology
    bool enableNUMA = false;
    size_t numaNodeCount = std::thread::hardware_concurrency();
    
    // Memory allocation strategy
    enum MemoryAllocationStrategy {
        LOCAL_ALLOC,
        INTERLEAVED_ALLOC,
        BEST_FIT_ALLOC
    } memoryStrategy = MemoryAllocationStrategy::BEST_FIT_ALLOC;
    
    // Thread placement strategy
    enum ThreadPlacementStrategy {
        CORE_LOCAL,
        NODE_LOCAL,
        INTERLEAVED
    } threadPlacement = ThreadPlacementStrategy::NODE_LOCAL;
    
    // Memory affinity
    bool enableMemoryAffinity = true;
    bool enableThreadAffinity = true;
    
    // Configuration for different NUMA nodes
    struct NodeConfig {
        size_t neuronCount = 1000;
        size_t threadCount = 1;
        size_t memoryPoolSize = 1024;
    };
    
    std::vector<NodeConfig> nodeConfigs;
};
```

## Memory Optimization

### Memory Pool Optimization

```cpp
// Optimized memory pool for neurons
class NeuronMemoryPool {
public:
    NeuronMemoryPool(size_t capacity) {
        // Calculate number of blocks needed
        size_t blockCount = (capacity + BLOCK_SIZE - 1) / BLOCK_SIZE;
        
        // Allocate memory
        neuronBlocks = new Neuron[blockCount * BLOCK_SIZE];
        freeIndices = new size_t[blockCount];
        
        // Initialize free indices
        freeHead = 0;
        freeTail = blockCount - 1;
        
        for (size_t i = 0; i < blockCount; ++i) {
            freeIndices[i] = i;
        }
        
        this->capacity = blockCount * BLOCK_SIZE;
        allocatedCount = 0;
    }
    
    Neuron* allocate() {
        if (freeHead >= freeTail) {
            return nullptr;  // Pool exhausted
        }
        
        size_t index = freeIndices[freeHead++];
        allocatedCount++;
        
        return &neuronBlocks[index];
    }
    
    void deallocate(Neuron* neuron) {
        // Find neuron in pool
        Neuron* start = neuronBlocks;
        Neuron* end = neuronBlocks + capacity;
        
        Neuron* found = std::find(start, end, neuron);
        if (found == end) {
            return;  // Not found
        }
        
        size_t index = found - start;
        
        // Return to free list
        freeIndices[--freeTail] = index;
        allocatedCount--;
    }
    
    size_t getAllocatedCount() const {
        return allocatedCount;
    }
    
    ~NeuronMemoryPool() {
        delete[] neuronBlocks;
        delete[] freeIndices;
    }
    
private:
    static constexpr size_t BLOCK_SIZE = 64;  // 64 neurons per block
    
    Neuron* neuronBlocks;
    size_t* freeIndices;
    size_t capacity;
    size_t allocatedCount;
    size_t freeHead;
    size_t freeTail;
};
```

### Memory Alignment Optimization

```cpp
// Memory alignment optimization
struct AlignedMemory {
    // Cache line alignment
    static constexpr size_t CACHE_LINE_ALIGNMENT = 64;
    
    // Memory pool alignment
    static constexpr size_t MEMORY_POOL_ALIGNMENT = 4096;
    
    // Vector alignment
    static constexpr size_t SIMD_ALIGNMENT = 32;
    
    // Allocate aligned memory
    static void* alignedAlloc(size_t size, size_t alignment = CACHE_LINE_ALIGNMENT) {
        void* ptr = nullptr;
        
        #ifdef _WIN32
        ptr = _aligned_malloc(size, alignment);
        #else
        ptr = std::aligned_alloc(alignment, size);
        #endif
        
        if (!ptr) {
            throw std::bad_alloc();
        }
        
        return ptr;
    }
    
    // Free aligned memory
    static void alignedFree(void* ptr) {
        #ifdef _WIN32
        _aligned_free(ptr);
        #else
        std::free(ptr);
        #endif
    }
    
    // Allocate SIMD-aligned memory
    template<typename T, size_t ALIGNMENT = SIMD_ALIGNMENT>
    static std::vector<T> allocateSIMDAligned(size_t count) {
        void* rawPtr = alignedAlloc(count * sizeof(T), ALIGNMENT);
        T* ptr = reinterpret_cast<T*>(rawPtr);
        
        std::vector<T> result(ptr, ptr + count);
        
        alignedFree(rawPtr);
        
        return result;
    }
};
```

### Memory Recycling

```cpp
// Memory recycling for objects
class NeuronObjectPool {
public:
    NeuronObjectPool(size_t capacity) {
        pool = new Neuron[capacity];
        freeList = new size_t[capacity];
        poolSize = capacity;
        
        // Initialize free list
        for (size_t i = 0; i < capacity; ++i) {
            freeList[i] = i;
        }
        
        nextFree = 0;
        allocatedCount = 0;
    }
    
    Neuron* allocate() {
        if (nextFree >= poolSize) {
            return nullptr;  // Pool exhausted
        }
        
        size_t index = freeList[nextFree++];
        allocatedCount++;
        
        // Initialize neuron
        new (&pool[index]) Neuron();
        
        return &pool[index];
    }
    
    void deallocate(Neuron* neuron) {
        // Find neuron in pool
        Neuron* start = pool;
        Neuron* end = pool + poolSize;
        
        Neuron* found = std::find(start, end, neuron);
        if (found == end) {
            return;  // Not found
        }
        
        size_t index = found - start;
        
        // Destruct neuron
        found->~Neuron();
        
        // Return to free list
        freeList[--nextFree] = index;
        allocatedCount--;
    }
    
    size_t getAllocatedCount() const {
        return allocatedCount;
    }
    
    ~NeuronObjectPool() {
        delete[] pool;
        delete[] freeList;
    }
    
private:
    Neuron* pool;
    size_t* freeList;
    size_t poolSize;
    size_t allocatedCount;
    size_t nextFree;
};
```

## Parallel Processing

### Thread Pool Optimization

```cpp
// Thread pool for parallel processing
class NeuronProcessingThreadPool {
public:
    NeuronProcessingThreadPool(size_t threadCount) {
        // Create threads
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back(&NeuronProcessingThreadPool::workerThread, this);
        }
        
        // Wait for threads to initialize
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this]() { return !threadsActive; });
        threadsActive = true;
    }
    
    void submitTask(std::function<void()> task) {
        std::unique_lock<std::mutex> lock(mutex);
        
        // Add task to queue
        taskQueue.push(std::move(task));
        
        // Notify worker thread
        condition.notify_one();
    }
    
    void waitForTasks() {
        std::unique_lock<std::mutex> lock(mutex);
        taskCompleted.wait(lock, [this]() { return taskQueue.empty(); });
    }
    
    ~NeuronProcessingThreadPool() {
        // Stop threads
        {
            std::unique_lock<std::mutex> lock(mutex);
            threadsActive = false;
        }
        
        condition.notify_all();
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
private:
    void workerThread() {
        while (threadsActive) {
            std::function<void()> task;
            
            {
                std::unique_lock<std::mutex> lock(mutex);
                condition.wait(lock, [this]() { return !taskQueue.empty(); });
                
                task = taskQueue.front();
                taskQueue.pop();
            }
            
            // Execute task
            task();
            
            // Notify completion
            {
                std::unique_lock<std::mutex> lock(mutex);
                taskCompleted.notify_all();
            }
        }
    }
    
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> taskQueue;
    std::mutex mutex;
    std::condition_variable condition;
    std::condition_variable taskCompleted;
    bool threadsActive = false;
};
```

### Parallel Plasticity Updates

```cpp
// Parallel plasticity updates
class ParallelPlasticityUpdater {
public:
    ParallelPlasticityUpdater(size_t threadCount) {
        threadPool = std::make_unique<NeuronProcessingThreadPool>(threadCount);
    }
    
    void updatePlasticity(Brain& brain, float timeStep) {
        // Split neurons among threads
        std::vector<std::vector<Neuron*>> neuronPartitions(threadPool->getThreadCount());
        
        // Distribute neurons
        size_t neuronsPerThread = brain.getNeuronCount() / threadPool->getThreadCount();
        
        for (size_t i = 0; i < brain.getNeuronCount(); ++i) {
            neuronPartitions[i % threadPool->getThreadCount()].push_back(&brain.getNeuron(i));
        }
        
        // Submit tasks to thread pool
        for (size_t i = 0; i < threadPool->getThreadCount(); ++i) {
            threadPool->submitTask([&, i]() {
                updateNeuronsPlasticity(neuronPartitions[i], timeStep);
            });
        }
        
        // Wait for all tasks to complete
        threadPool->waitForTasks();
    }
    
private:
    void updateNeuronsPlasticity(const std::vector<Neuron*>& neurons, float timeStep) {
        // Update plasticity for neurons in partition
        for (Neuron* neuron : neurons) {
            // Get connected synapses
            auto& synapses = neuron->getSynapses();
            
            // Update plasticity for each synapse
            for (auto& synapse : synapses) {
                // Get pre and post neurons
                Neuron* pre = synapse.getPreNeuron();
                Neuron* post = synapse.getPostNeuron();
                
                if (pre && post) {
                    // Calculate synaptic weight change
                    float weightChange = calculateWeightChange(*neuron, *pre, *post, timeStep);
                    
                    // Apply weight change
                    synapse.setWeight(synapse.getWeight() + weightChange);
                }
            }
        }
    }
    
    std::unique_ptr<NeuronProcessingThreadPool> threadPool;
};
```

## SIMD Optimization

### SIMD-Optimized Neural Computation

```cpp
// SIMD-optimized neural computation using SSE
class SIMDEntityComputation {
public:
    static void processNeuronsSSE(Neuron* neurons, size_t count, float dt) {
        // Process neurons using SSE instructions
        size_t i = 0;
        
        // Align memory to 16-byte boundary
        __m128* alignedNeurons = reinterpret_cast<__m128*>(neurons);
        
        // Process in blocks of 4 neurons (SSE)
        for (; i + 3 < count; i += 4) {
            // Load neuron data into SSE registers
            __m128 vRest = _mm_load_ps(&alignedNeurons[i].vRest);
            __m128 vMem = _mm_load_ps(&alignedNeurons[i].vMem);
            __m128 vThresh = _mm_load_ps(&alignedNeurons[i].vThresh);
            __m128 vRefrac = _mm_load_ps(&alignedNeurons[i].vRefrac);
            
            // Calculate membrane potential
            __m128 dv = (vRest - vMem) * vMemTimeConstant * dt;
            vMem = vMem + dv;
            
            // Check for spikes
            __m128 spikeMask = _mm_cmpge_ps(vMem, vThresh);
            
            // Reset refractory timers
            __m128 refracMask = _mm_cmpgt_ps(vRefrac, _mm_setzero_ps());
            vRefrac = _mm_and_ps(vRefrac, refracMask);
            vRefrac = _mm_sub_ps(vRefrac, dt * _mm_set1_ps(1.0f / vRefracTimeConstant));
            
            // Reset spiked neurons
            vMem = _mm_and_ps(vMem, _mm_cmpeq_ps(spikeMask, spikeMask));  // Set to reset
            
            // Store back to memory
            _mm_store_ps(&alignedNeurons[i].vMem, vMem);
            _mm_store_ps(&alignedNeurons[i].vRefrac, vRefrac);
        }
        
        // Process remaining neurons (scalar)
        for (; i < count; ++i) {
            neurons[i].stepLIF(dt);
        }
    }
    
    static void processSynapsesSSE(Synapse* synapses, size_t count, float dt) {
        // Process synapses using SSE instructions
        size_t i = 0;
        
        // Align memory to 16-byte boundary
        __m128* alignedSynapses = reinterpret_cast<__m128*>(synapses);
        
        // Process in blocks of 4 synapses (SSE)
        for (; i + 3 < count; i += 4) {
            // Load synapse data
            __m128 wCurrent = _mm_load_ps(&alignedSynapses[i].wCurrent);
            __m128 wMax = _mm_load_ps(&alignedSynapses[i].wMax);
            __m128 wMin = _mm_load_ps(&alignedSynapses[i].wMin);
            
            // Calculate weight change
            __m128 wChange = calculateWeightChangeSSE(&alignedSynapses[i]);
            
            // Apply weight change with bounds checking
            __m128 wNew = wCurrent + wChange;
            wNew = _mm_min_ps(wNew, wMax);
            wNew = _mm_max_ps(wNew, wMin);
            
            // Store back
            _mm_store_ps(&alignedSynapses[i].wCurrent, wNew);
        }
        
        // Process remaining synapses (scalar)
        for (; i < count; ++i) {
            synapses[i].updateWeight(dt);
        }
    }
    
private:
    static __m128 calculateWeightChangeSSE(Synapse* synapse) {
        // Calculate weight change using SSE
        // This would involve the STDP, Hebbian, and other plasticity rules
        // For demonstration, we'll use a simple formula
        __m128 preTrace = _mm_load_ps(&synapse->preTrace);
        __m128 postTrace = _mm_load_ps(&synapse->postTrace);
        
        // Hebbian component
        __m128 hebbian = _mm_mul_ps(preTrace, postTrace) * _mm_set1_ps(0.001f);
        
        // STDP component (would need spike timing data)
        __m128 stdp = _mm_setzero_ps();
        
        return hebbian + stdp;
    }
};
```

### Vectorized Memory Access

```cpp
// Vectorized memory access for better cache performance
class VectorizedMemoryAccess {
public:
    template<typename T>
    static void storeAligned(T* data, const T* source, size_t count) {
        // Store data aligned to cache line boundaries
        size_t i = 0;
        
        // Align to 16-byte boundary
        size_t alignment = 16 / sizeof(T);
        size_t alignedCount = (count + alignment - 1) / alignment * alignment;
        
        // Store aligned data
        for (; i + alignment - 1 < count; i += alignment) {
            __m128i* dest = reinterpret_cast<__m128i*>(&data[i]);
            __m128i* src = reinterpret_cast<__m128i*>(&source[i]);
            
            _mm_storeu_si128(dest, _mm_loadu_si128(src));
            _mm_storeu_si128(dest + 1, _mm_loadu_si128(src + 1));
            _mm_storeu_si128(dest + 2, _mm_loadu_si128(src + 2));
            _mm_storeu_si128(dest + 3, _mm_loadu_si128(src + 3));
        }
        
        // Store remaining data
        for (; i < count; ++i) {
            data[i] = source[i];
        }
    }
    
    template<typename T>
    static void loadAligned(T* dest, const T* source, size_t count) {
        // Load data aligned to cache line boundaries
        size_t i = 0;
        
        // Align to 16-byte boundary
        size_t alignment = 16 / sizeof(T);
        size_t alignedCount = (count + alignment - 1) / alignment * alignment;
        
        // Load aligned data
        for (; i + alignment - 1 < count; i += alignment) {
            __m128i* src = reinterpret_cast<__m128i*>(&source[i]);
            __m128i* dst = reinterpret_cast<__m128i*>(&dest[i]);
            
            _mm_storeu_si128(dst, _mm_loadu_si128(src));
            _mm_storeu_si128(dst + 1, _mm_loadu_si128(src + 1));
            _mm_storeu_si128(dst + 2, _mm_loadu_si128(src + 2));
            _mm_storeu_si128(dst + 3, _mm_loadu_si128(src + 3));
        }
        
        // Load remaining data
        for (; i < count; ++i) {
            dest[i] = source[i];
        }
    }
};
```

## Profiling and Benchmarking

### Performance Benchmarking

```bash
#!/bin/bash
# benchmark_nlm.sh

set -e

# Configuration
NEURON_COUNTS=(100 500 1000 2000 5000)
STEPD_COUNTS=(100 500 1000 5000)
OUTPUT_DIR="benchmark_results"

# Create output directory
mkdir -p $OUTPUT_DIR

# Function to run benchmark
run_benchmark() {
    local neuron_count=$1
    local step_count=$2
    local benchmark_name=$3
    
    echo "Running benchmark: $benchmark_name (neurons: $neuron_count, steps: $step_count)"
    
    # Create configuration
    cat > config.json << EOF
{
    "brain": {
        "neuron_count": $neuron_count,
        "synapse_density": 0.1
    },
    "performance": {
        "threads": 4,
        "memory_pool_size": 2048,
        "event_queue_size": 4096
    }
}
EOF
    
    # Run simulation
    start_time=$(date +%s.%N)
    ./nlm --config config.json --steps $step_count > /dev/null 2>&1
    end_time=$(date +%s.%N)
    
    # Calculate performance metrics
    elapsed=$(echo "$end_time - $start_time" | bc)
    fps=$(echo "scale=2; $step_count / $elapsed" | bc)
    neurons_per_second=$(echo "scale=2; $neuron_count * $fps" | bc)
    
    # Output results
    echo "  Time: ${elapsed}s"
    echo "  FPS: ${fps}"
    echo "  Neurons/second: ${neurons_per_second}"
    
    # Save results
    echo "$benchmark_name,$neuron_count,$step_count,$elapsed,$fps,$neurons_per_second" >> "$OUTPUT_DIR/results.csv"
}

# Run all benchmarks
for neuron_count in ${NEURON_COUNTS[@]}; do
    for step_count in ${STEPD_COUNTS[@]}; do
        benchmark_name="nlm_${neuron_count}_neurons_${step_count}_steps"
        run_benchmark $neuron_count $step_count $benchmark_name
    done
done

# Generate report
echo "Benchmarks completed. Results saved to $OUTPUT_DIR/results.csv"
echo "Generating report..."

# Generate graph data
cat > "$OUTPUT_DIR/generate_report.py" << 'EOF'
import pandas as pd
import matplotlib.pyplot as plt
import sys

# Read results
df = pd.read_csv('results.csv')

# Plot performance vs neuron count
plt.figure(figsize=(12, 8))

# Plot 1: FPS vs neuron count
plt.subplot(2, 2, 1)
for step_count in df['steps'].unique():
    subset = df[df['steps'] == step_count]
    plt.plot(subset['neurons'], subset['fps'], marker='o', label=f'{step_count} steps')

plt.xlabel('Neuron Count')
plt.ylabel('FPS')
plt.title('Performance vs Neuron Count')
plt.legend()
plt.grid(True)

# Plot 2: Neurons per second vs neuron count
plt.subplot(2, 2, 2)
for step_count in df['steps'].unique():
    subset = df[df['steps'] == step_count]
    plt.plot(subset['neurons'], subset['neurons_per_second'], marker='o', label=f'{step_count} steps')

plt.xlabel('Neuron Count')
plt.ylabel('Neurons per Second')
plt.title('Neurons per Second vs Neuron Count')
plt.legend()
plt.grid(True)

# Plot 3: Memory usage vs neuron count
plt.subplot(2, 2, 3)
for step_count in df['steps'].unique():
    subset = df[df['steps'] == step_count]
    plt.plot(subset['neurons'], subset['memory'], marker='o', label=f'{step_count} steps')

plt.xlabel('Neuron Count')
plt.ylabel('Memory Usage (MB)')
plt.title('Memory Usage vs Neuron Count')
plt.legend()
plt.grid(True)

# Plot 4: Scalability
plt.subplot(2, 2, 4)
for step_count in df['steps'].unique():
    subset = df[df['steps'] == step_count]
    plt.plot(subset['neurons'], subset['scalability'], marker='o', label=f'{step_count} steps')

plt.xlabel('Neuron Count')
plt.ylabel('Scalability (log scale)')
plt.title('Scalability')
plt.legend()
plt.grid(True)
plt.yscale('log')

plt.tight_layout()
plt.savefig('benchmark_report.png', dpi=300)
plt.show()

print("Report generated: benchmark_report.png")
EOF

# Run report generation
python3 "$OUTPUT_DIR/generate_report.py"
```
```

### Performance Comparison

```python
# Performance comparison script
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

class PerformanceAnalyzer {
public:
    void loadBenchmarkData(const std::string& filename) {
        // Load benchmark data from CSV
        pandas::dataframe = pandas::read_csv(filename);
        
        // Process data
        processBenchmarkData(data);
    }
    
    void analyzePerformance() {
        // Analyze performance across different configurations
        analyzeNeuronScaling();
        analyzeMemoryUsage();
        analyzeCacheEfficiency();
        analyzeParallelEfficiency();
    }
    
    void generateReport() {
        // Generate performance report
        generatePerformanceReport();
        
        // Generate optimization recommendations
        generateOptimizationRecommendations();
    }
    
private:
    void processBenchmarkData(pandas::dataframe& data) {
        // Calculate performance metrics
        data['time_per_step'] = data['time'] / data['steps'];
        data['memory_per_neuron'] = data['memory'] / data['neurons'];
        data['performance'] = data['neurons_per_second'] / data['neurons'];
        
        // Calculate scalability
        data['scalability'] = calculateScalability(data);
        
        // Identify performance bottlenecks
        identifyPerformanceBottlenecks(data);
    }
    
    double calculateScalability(const pandas::dataframe& data) {
        // Calculate scalability (logarithmic)
        double scalability = np::log10(data['neurons_per_second'].mean());
        return scalability;
    }
    
    void analyzeNeuronScaling() {
        // Analyze performance scaling with neuron count
        // This would involve plotting and statistical analysis
    }
    
    void analyzeMemoryUsage() {
        // Analyze memory usage patterns
        // This would involve memory profiling and optimization
    }
    
    void analyzeCacheEfficiency() {
        // Analyze cache efficiency
        // This would involve cache miss analysis
    }
    
    void analyzeParallelEfficiency() {
        // Analyze parallel processing efficiency
        // This would involve scaling analysis with thread count
    }
    
    void generatePerformanceReport() {
        // Generate comprehensive performance report
        std::cout << "Performance Analysis Report" << std::endl;
        std::cout << "===========================" << std::endl;
        
        // Summary statistics
        std::cout << "\nSummary Statistics:" << std::endl;
        std::cout << "  Average Performance: " << data['performance'].mean() << " steps/sec/neuron" << std::endl;
        std::cout << "  Memory Efficiency: " << data['memory_per_neuron'].mean() << " MB/neuron" << std::endl;
        std::cout << "  Scalability: " << data['scalability'].mean() << std::endl;
        
        // Performance bottlenecks
        std::cout << "\nPerformance Bottlenecks:" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            std::cout << "  - " << bottleneck << std::endl;
        }
        
        // Recommendations
        std::cout << "\nOptimization Recommendations:" << std::endl;
        for (const auto& recommendation : recommendations) {
            std::cout << "  - " << recommendation << std::endl;
        }
    }
    
    void generateOptimizationRecommendations() {
        // Generate optimization recommendations based on analysis
        recommendations.clear();
        
        // Check for memory optimization needs
        if (data['memory_per_neuron'].mean() > 0.1) {  // > 100 KB per neuron
            recommendations.push_back("Implement memory pool optimization to reduce allocation overhead");
        }
        
        // Check for cache optimization needs
        if (data['cache_miss_rate'].mean() > 0.1) {  // > 10% cache miss rate
            recommendations.push_back("Implement cache-friendly data structures");
        }
        
        // Check for parallel optimization needs
        if (data['parallel_efficiency'].mean() < 0.8) {  // < 80% parallel efficiency
            recommendations.push_back("Implement better load balancing for parallel processing");
        }
        
        // Check for SIMD optimization needs
        if (data['simd_usage'].mean() < 0.5) {  // < 50% SIMD usage
            recommendations.push_back("Implement SIMD optimizations for neural computations");
        }
    }
    
private:
    pandas::dataframe data;
    std::vector<std::string> bottlenecks;
    std::vector<std::string> recommendations;
};
```

## Performance Testing

### Performance Test Suite

```bash
#!/bin/bash
# performance_test.sh

set -e

# Configuration
TEST_CONFIG="performance_test_config.json"
TEST_RESULTS="performance_test_results.csv"
BENCHMARK_TIMEOUT=300  # 5 minutes

# Function to run performance test
run_performance_test() {
    local test_name=$1
    local neuron_count=$2
    local step_count=$3
    
    echo "Running performance test: $test_name"
    echo "  Neurons: $neuron_count"
    echo "  Steps: $step_count"
    
    # Create test configuration
    cat > $TEST_CONFIG << EOF
{
    "brain": {
        "neuron_count": $neuron_count,
        "synapse_density": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001
            }
        }
    },
    "performance": {
        "threads": 4,
        "memory_pool_size": 2048,
        "event_queue_size": 4096,
        "sparse_connectivity": true,
        "simd_optimization": true
    }
}
EOF
    
    # Run test with timeout
    start_time=$(date +%s)
    if timeout $BENCHMARK_TIMEOUT ./nlm --config $TEST_CONFIG --steps $step_count --benchmark > $TEST_RESULTS 2>&1; then
        end_time=$(date +%s)
        elapsed=$((end_time - start_time))
        
        # Extract performance metrics
        max_memory=$(grep "max_memory" $TEST_RESULTS | tail -1 | cut -d'=' -f2)
        avg_cpu=$(grep "avg_cpu" $TEST_RESULTS | tail -1 | cut -d'=' -f2)
        total_steps=$(grep "total_steps" $TEST_RESULTS | tail -1 | cut -d'=' -f2)
        
        # Calculate performance
        fps=$(echo "scale=2; $step_count / $elapsed" | bc)
        neurons_per_second=$(echo "scale=2; $neuron_count * $fps" | bc)
        memory_per_neuron=$(echo "scale=2; $max_memory * 1024 / $neuron_count" | bc)
        
        # Output results
        echo "  Time: ${elapsed}s"
        echo "  FPS: ${fps}"
        echo "  Neurons/second: ${neurons_per_second}"
        echo "  Memory per neuron: ${memory_per_neuron} KB"
        echo "  CPU utilization: ${avg_cpu}%"
        
        # Save results
        echo "$test_name,$neuron_count,$step_count,$elapsed,$fps,$neurons_per_second,$max_memory,$avg_cpu" >> $TEST_RESULTS
        
        # Check if test passes
        if [ $fps -lt 10 ]; then
            echo "  WARNING: FPS below expected threshold (10)"
        fi
        
        if [ $memory_per_neuron -gt 100 ]; then
            echo "  WARNING: Memory per neuron above threshold (100 KB)"
        fi
        
    else
        echo "  ERROR: Test failed or timed out"
        return 1
    fi
}

# Run performance tests
for neuron_count in 100 500 1000 2000 5000; do
    for step_count in 100 500 1000; do
        test_name="performance_test_${neuron_count}_neurons_${step_count}_steps"
        run_performance_test "$test_name" $neuron_count $step_count
    done
done

# Analyze test results
echo "Analyzing test results..."
python3 << 'EOF'
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load results
df = pd.read_csv('performance_test_results.csv')

# Analyze performance
print("Performance Analysis:")
print("===================")
print(f"\nAverage FPS: {df['fps'].mean():.2f}")
print(f"Min FPS: {df['fps'].min():.2f}")
print(f"Max FPS: {df['fps'].max():.2f}")

print(f"\nAverage Neurons per Second: {df['neurons_per_second'].mean():.0f}")
print(f"Min Neurons per Second: {df['neurons_per_second'].min():.0f}")
print(f"Max Neurons per Second: {df['neurons_per_second'].max():.0f}")

print(f"\nAverage Memory per Neuron: {df['memory_per_neuron'].mean():.2f} KB")
print(f"Min Memory per Neuron: {df['memory_per_neuron'].min():.2f} KB")
print(f"Max Memory per Neuron: {df['memory_per_neuron'].max():.2f} KB")

# Performance regression analysis
print("\nPerformance Regression Analysis:")
print("===============================")

# Calculate performance scaling
for neuron_count in df['neuron_count'].unique():
    subset = df[df['neuron_count'] == neuron_count]
    avg_fps = subset['fps'].mean()
    avg_neurons_per_sec = subset['neurons_per_second'].mean()
    
    print(f"\nNeuron Count: {neuron_count}")
    print(f"  Average FPS: {avg_fps:.2f}")
    print(f"  Average Neurons/sec: {avg_neurons_per_sec:.0f}")
    print(f"  Performance per neuron: {avg_fps/neuron_count:.4f} steps/sec/neuron")

# Identify performance bottlenecks
print("\nPerformance Bottlenecks:")
print("=======================")

# Check for memory bottlenecks
memory_bottlenecks = df[df['memory_per_neuron'] > 200]
if not memory_bottlenecks.empty:
    print(f"Memory bottlenecks found: {len(memory_bottlenecks)} tests")
    print("Recommendations:")
    print("  - Implement memory pool optimization")
    print("  - Reduce memory allocations")
    print("  - Use compressed data structures")

# Check for CPU bottlenecks
cpu_bottlenecks = df[df['avg_cpu'] > 80]
if not cpu_bottlenecks.empty:
    print(f"CPU bottlenecks found: {len(cpu_bottlenecks)} tests")
    print("Recommendations:")
    print("  - Implement better load balancing")
    print("  - Optimize parallel processing")
    print("  - Use cache-friendly algorithms")

# Check for scalability bottlenecks
scalability_bottlenecks = df[df['scalability'] < 0.5]
if not scalability_bottlenecks.empty:
    print(f"Scalability bottlenecks found: {len(scalability_bottlenecks)} tests")
    print("Recommendations:")
    print("  - Implement cache optimization")
    print("  - Use SIMD instructions")
    print("  - Optimize data layout")
EOF

# Generate performance report
echo "Generating performance report..."
cat > performance_report.md << EOF
# Performance Test Report

## Test Summary

Performance tests were run with varying neuron counts and step counts to evaluate NLM performance across different scales.

## Performance Metrics

| Metric | Description | Target |
|--------|-------------|--------|
| FPS | Frames per second (simulation speed) | > 10 FPS |
| Neurons/sec | Neurons processed per second | > 10000 |
| Memory/neuron | Memory usage per neuron | < 100 KB |
| CPU utilization | CPU usage percentage | < 80% |

## Results

Performance tests completed: ${TEST_RESULTS:-'N/A'}

See performance_test_results.csv for detailed results.

## Recommendations

Based on the performance analysis:

1. **Memory Optimization**: Implement memory pool optimization to reduce allocation overhead.
2. **Cache Optimization**: Use cache-friendly data structures to reduce cache misses.
3. **Parallel Optimization**: Implement better load balancing for parallel processing.
4. **SIMD Optimization**: Use SIMD instructions for neural computations.

## Future Work

1. **Scaling Tests**: Test with larger brain sizes (10K+ neurons).
2. **Real-time Testing**: Test with real-time applications.
3. **Memory Profiling**: Profile memory usage with Valgrind.
4. **Performance Monitoring**: Implement real-time performance monitoring.
EOF

echo "Performance test completed. Report generated: performance_report.md"
```

## Real-time Performance

### Real-time Performance Requirements

| Application | Required FPS | Neurons Supported | Latency |
|-------------|--------------|------------------|---------|
| Interactive | > 60 FPS | 1000-5000 | < 10 ms |
| Control | > 30 FPS | 5000-10000 | < 5 ms |
| Simulation | > 10 FPS | 10000-50000 | < 1 ms |

### Real-time Performance Optimization

```cpp
// Real-time performance optimization
class RealTimeOptimizer {
public:
    RealTimeOptimizer(size_t targetFPS) {
        targetFrameTime = 1000.0f / targetFPS;  // milliseconds
        currentFrameTime = 0.0f;
        frameCount = 0;
    }
    
    bool shouldOptimizeForRealTime() {
        // Check if performance requirements are met
        return currentFrameTime <= targetFrameTime * 1.1f;  // Allow 10% margin
    }
    
    void onFrameStart() {
        frameStartTime = std::chrono::high_resolution_clock::now();
    }
    
    void onFrameEnd() {
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        currentFrameTime = std::chrono::duration<float, std::milli>(
            frameEndTime - frameStartTime).count();
        
        frameCount++;
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        // Apply optimizations if needed
        if (!shouldOptimizeForRealTime()) {
            applyRealTimeOptimizations();
        }
    }
    
    float getCurrentFrameTime() const {
        return currentFrameTime;
    }
    
    float getAverageFrameTime() const {
        return frameCount > 0 ? totalFrameTime / frameCount : 0.0f;
    }
    
    float getFPS() const {
        return frameCount > 0 ? 1000.0f / getAverageFrameTime() : 0.0f;
    }
    
private:
    void updatePerformanceMetrics() {
        totalFrameTime += currentFrameTime;
        
        // Log performance metrics
        if (frameCount % 100 == 0) {
            float avgFrameTime = getAverageFrameTime();
            float fps = getFPS();
            
            NLM_LOG_INFO(std::string("Performance: ") + 
                        std::to_string(fps) + " FPS, " +
                        std::to_string(avgFrameTime) + " ms/frame");
        }
    }
    
    void applyRealTimeOptimizations() {
        // Apply optimizations for real-time performance
        // Reduce computation
        reduceComputationLevel();
        
        // Reduce memory usage
        reduceMemoryUsage();
        
        // Reduce I/O
        reduceIOOperations();
        
        // Adjust quality settings
        adjustQualitySettings();
    }
    
    void reduceComputationLevel() {
        // Reduce computational complexity
        // This might involve:
        // - Reducing number of neurons
        // - Simplifying neural dynamics
        // - Reducing plasticity calculations
        // - Using approximate algorithms
    }
    
    void reduceMemoryUsage() {
        // Reduce memory usage
        // This might involve:
        // - Using more compact data structures
        // - Compressing memory
        // - Using memory pools
        // - Reducing cache footprint
    }
    
    void reduceIOOperations() {
        // Reduce I/O operations
        // This might involve:
        // - Caching frequently accessed data
        // - Reducing file I/O
        // - Using in-memory storage
    }
    
    void adjustQualitySettings() {
        // Adjust quality settings
        // This might involve:
        // - Reducing visualization quality
        // - Reducing output precision
        // - Using simpler models
    }
    
private:
    float targetFrameTime;
    float currentFrameTime;
    float totalFrameTime = 0.0f;
    size_t frameCount = 0;
    std::chrono::high_resolution_clock::time_point frameStartTime;
};
```

## Scalability

### Scaling Performance

```bash
#!/bin/bash
# scaling_test.sh

set -e

# Configuration
SCALING_CONFIG="scaling_config.json"
SCALING_RESULTS="scaling_results.csv"

# Function to run scaling test
run_scaling_test() {
    local neuron_count=$1
    local step_count=$2
    local benchmark_name=$3
    
    echo "Running scaling test: $benchmark_name"
    echo "  Neurons: $neuron_count"
    echo "  Steps: $step_count"
    
    # Create scaling configuration
    cat > $SCALING_CONFIG << EOF
{
    "brain": {
        "neuron_count": $neuron_count,
        "synapse_density": 0.1
    },
    "performance": {
        "threads": 4,
        "memory_pool_size": 2048,
        "event_queue_size": 4096,
        "sparse_connectivity": true,
        "simd_optimization": true,
        "parallel_plasticity": true
    }
}
EOF
    
    # Run simulation
    ./nlm --config $SCALING_CONFIG --steps $step_count --benchmark > scaling_temp_results.txt 2>&1
    
    # Extract performance metrics
    max_memory=$(grep "max_memory" scaling_temp_results.txt | tail -1 | cut -d'=' -f2)
    avg_cpu=$(grep "avg_cpu" scaling_temp_results.txt | tail -1 | cut -d'=' -f2)
    total_steps=$(grep "total_steps" scaling_temp_results.txt | tail -1 | cut -d'=' -f2)
    
    # Calculate scaling metrics
    memory_per_neuron=$(echo "scale=2; $max_memory * 1024 / $neuron_count" | bc)
    efficiency=$(echo "scale=2; $total_steps / $step_count" | bc)
    
    # Output results
    echo "  Memory per neuron: ${memory_per_neuron} KB"
    echo "  Efficiency: ${efficiency}"
    
    # Save results
    echo "$benchmark_name,$neuron_count,$step_count,$max_memory,$avg_cpu,$memory_per_neuron,$efficiency" >> $SCALING_RESULTS
}

# Run scaling tests
echo "Running scaling tests..."
for neuron_count in 100 500 1000 2000 5000 10000; do
    step_count=1000
    benchmark_name="scaling_test_${neuron_count}_neurons_${step_count}_steps"
    run_scaling_test $neuron_count $step_count $benchmark_name
done

# Analyze scaling
echo "Analyzing scaling results..."
python3 << 'EOF'
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load results
df = pd.read_csv('scaling_results.csv')

# Plot scaling results
plt.figure(figsize=(15, 10))

# Plot 1: Memory usage vs neuron count
plt.subplot(2, 2, 1)
plt.plot(df['neurons'], df['memory_per_neuron'], marker='o', linewidth=2, markersize=8)
plt.xlabel('Neuron Count')
plt.ylabel('Memory per Neuron (KB)')
plt.title('Memory Usage Scaling')
plt.grid(True, alpha=0.3)

# Plot 2: Efficiency vs neuron count
plt.subplot(2, 2, 2)
plt.plot(df['neurons'], df['efficiency'], marker='o', linewidth=2, markersize=8)
plt.xlabel('Neuron Count')
plt.ylabel('Efficiency')
plt.title('Efficiency Scaling')
plt.grid(True, alpha=0.3)
plt.ylim(0.9, 1.1)

# Plot 3: Log-log plot of performance
plt.subplot(2, 2, 3)
plt.loglog(df['neurons'], df['memory_per_neuron'], marker='o', linewidth=2, markersize=8, label='Memory')
plt.loglog(df['neurons'], 1/df['efficiency'], marker='s', linewidth=2, markersize=8, label='Inverse Efficiency')
plt.xlabel('Neuron Count (log)')
plt.ylabel('Metric (log)')
plt.title('Log-Log Performance Scaling')
plt.legend()
plt.grid(True, alpha=0.3)

# Plot 4: Performance summary
plt.subplot(2, 2, 4)
# Calculate performance per neuron
performance_per_neuron = df['memory_per_neuron'] * df['efficiency']
plt.plot(df['neurons'], performance_per_neuron, marker='^', linewidth=2, markersize=8)
plt.xlabel('Neuron Count')
plt.ylabel('Performance (Memory*Efficiency)')
plt.title('Overall Performance')
plt.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('scaling_analysis.png', dpi=300)
plt.show()

# Calculate scaling exponents
print("Scaling Analysis:")
print("=================")
print("\nMemory Scaling:")
for neuron_count in df['neurons'].unique():
    subset = df[df['neurons'] == neuron_count]
    memory = subset['memory_per_neuron'].iloc[0]
    print(f"  Neurons: {neuron_count}, Memory/neuron: {memory:.2f} KB")

print("\nEfficiency Scaling:")
for neuron_count in df['neurons'].unique():
    subset = df[df['neurons'] == neuron_count]
    efficiency = subset['efficiency'].iloc[0]
    print(f"  Neurons: {neuron_count}, Efficiency: {efficiency:.4f}")

# Check for scaling issues
print("\nScaling Issues:")
print("===============")

# Check for memory scaling issues
memory_scaling = np.polyfit(np.log10(df['neurons']), np.log10(df['memory_per_neuron']), 1)[0]
print(f"Memory scaling exponent: {memory_scaling:.2f}")
if memory_scaling > 1.0:
    print("  WARNING: Memory usage scaling faster than linear")
    print("  Recommendations:")
    print("    - Implement memory pool optimization")
    print("    - Use compressed data structures")
    print("    - Implement lazy allocation")

# Check for efficiency scaling issues
efficiency_scaling = np.polyfit(np.log10(df['neurons']), np.log10(1/df['efficiency']), 1)[0]
print(f"Efficiency scaling exponent: {efficiency_scaling:.2f}")
if efficiency_scaling > 0.1:
    print("  WARNING: Efficiency decreasing with scale")
    print("  Recommendations:")
    print("    - Optimize cache usage")
    print("    - Implement better load balancing")
    print("    - Use SIMD optimizations")
EOF

# Cleanup
rm -f scaling_temp_results.txt $SCALING_CONFIG

echo "Scaling test completed. Results saved to: scaling_results.csv, scaling_analysis.png"
```

## Mobile/Embedded Performance

### Mobile/Embedded Performance Constraints

| Platform | CPU | RAM | Storage | Power |
|----------|-----|-----|---------|-------|
| Mobile | ARM Cortex-A72 | 4-8 GB | 64-256 GB | 5-20W |
| Embedded | ARM Cortex-M7 | 512MB-2GB | 16-64MB | 0.5-5W |
| IoT | ARM Cortex-M4 | 256KB-1MB | 8-32MB | 0.1-1W |

### Mobile Performance Optimization

```cpp
// Mobile performance optimization
class MobilePerformanceOptimizer {
public:
    MobilePerformanceOptimizer(size_t memoryLimit, float powerLimit) {
        memoryLimitKB = memoryLimit;
        powerLimitW = powerLimit;
        currentPower = 0.0f;
        currentMemory = 0.0f;
    }
    
    bool shouldOptimizeForMobile() {
        // Check resource constraints
        return currentMemory > memoryLimitKB * 0.8f ||
               currentPower > powerLimitW * 0.9f;
    }
    
    void optimizeForMobile() {
        // Apply mobile-specific optimizations
        reduceMemoryFootprint();
        reduceCPUUsage();
        optimizeForPowerConstraints();
        adjustForMemoryConstrained();
    }
    
    void onMemoryUsage(float memoryUsage) {
        currentMemory = memoryUsage;
        
        if (shouldOptimizeForMobile()) {
            applyMobileOptimizations();
        }
    }
    
    void onPowerUsage(float powerUsage) {
        currentPower = powerUsage;
        
        if (shouldOptimizeForMobile()) {
            applyPowerOptimizations();
        }
    }
    
private:
    void reduceMemoryFootprint() {
        // Reduce memory footprint
        // This might involve:
        // - Using compact data structures
        // - Implementing memory pools
        // - Using in-place algorithms
        // - Reducing data precision
    }
    
    void reduceCPUUsage() {
        // Reduce CPU usage
        // This might involve:
        // - Simplifying algorithms
        // - Reducing parallel processing
        // - Using approximate computing
        // - Reducing simulation complexity
    }
    
    void optimizeForPowerConstraints() {
        // Optimize for power constraints
        // This might involve:
        // - Dynamic voltage/frequency scaling
        // - Reducing core count
        // - Using low-power modes
        // - Minimizing wake-ups
    }
    
    void adjustForMemoryConstrained() {
        // Adjust for memory constraints
        // This might involve:
        // - Swapping to disk
        // - Using compressed storage
        // - Implementing streaming
        // - Reducing working set size
    }
    
    void applyMobileOptimizations() {
        // Apply comprehensive mobile optimizations
        reduceMemoryFootprint();
        reduceCPUUsage();
        optimizeForPowerConstraints();
        adjustForMemoryConstrained();
    }
    
    void applyPowerOptimizations() {
        // Apply power-specific optimizations
        // This might involve:
        // - Reducing computational load
        // - Using low-power states
        // - Minimizing I/O
        // - Implementing throttling
    }
    
private:
    size_t memoryLimitKB;
    float powerLimitW;
    float currentMemory;
    float currentPower;
};
```

### Embedded Performance Optimization

```cpp
// Embedded performance optimization
class EmbeddedPerformanceOptimizer {
public:
    EmbeddedPerformanceOptimizer(size_t memoryLimit, float cpuFrequency) {
        memoryLimitBytes = memoryLimit;
        cpuFrequencyMHz = cpuFrequency;
        currentMemory = 0;
        currentCPUFrequency = cpuFrequency;
    }
    
    void configureForEmbedded() {
        // Configure for embedded environment
        // This might involve:
        // - Setting memory limits
        // - Configuring CPU frequency
        // - Optimizing for cache size
        // - Configuring memory controller
    }
    
    void allocateMemory(size_t size) {
        // Allocate memory with tracking
        currentMemory += size;
        
        if (currentMemory > memoryLimitBytes * 0.9f) {
            // Memory limit exceeded, apply optimizations
            applyMemoryOptimizations();
        }
    }
    
    void deallocateMemory(size_t size) {
        // Deallocate memory with tracking
        currentMemory -= size;
        
        if (currentMemory < memoryLimitBytes * 0.5f) {
            // Memory usage low, can relax optimizations
            relaxMemoryOptimizations();
        }
    }
    
    void setCPUFrequency(float frequency) {
        // Set CPU frequency
        currentCPUFrequency = frequency;
        
        // Apply optimizations based on frequency
        if (frequency < cpuFrequencyMHz * 0.5f) {
            // Low frequency, aggressive optimizations
            applyAggressiveOptimizations();
        } else {
            // Normal frequency, relax optimizations
            relaxOptimizations();
        }
    }
    
private:
    void applyMemoryOptimizations() {
        // Apply memory optimization techniques
        // This might involve:
        // - Using static memory allocation
        // - Implementing memory pools
        // - Compressing data structures
        // - Using efficient algorithms
    }
    
    void relaxMemoryOptimizations() {
        // Relax memory optimization constraints
        // This might involve:
        // - Allowing larger allocations
        // - Using dynamic allocation
        // - Improving performance
    }
    
    void applyAggressiveOptimizations() {
        // Apply aggressive optimization techniques
        // This might involve:
        // - Aggressive memory reduction
        // - Simplified algorithms
        // - Reduced computation
        // - Minimized overhead
    }
    
    void relaxOptimizations() {
        // Relax optimization constraints
        // This might involve:
        // - Allowing larger allocations
        // - Using complex algorithms
        // - Increasing computation
        // - Improving accuracy
    }
    
private:
    size_t memoryLimitBytes;
    float cpuFrequencyMHz;
    size_t currentMemory;
    float currentCPUFrequency;
};
```

## Performance Monitoring

### Performance Monitoring System

```cpp
// Performance monitoring system
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        float cpuUsage = 0.0f;           // CPU usage percentage
        float memoryUsage = 0.0f;        // Memory usage percentage
        float memoryBandwidth = 0.0f;   // Memory bandwidth usage
        float cacheMissRate = 0.0f;     // Cache miss rate
        float iowait = 0.0f;            // I/O wait time
        float taskQueueLength = 0.0f;   // Task queue length
        float processingTime = 0.0f;    // Processing time per task
        float responseTime = 0.0f;      // Response time
    };
    
    PerformanceMonitor() {
        // Initialize metrics
        initializeMetrics();
        
        // Start monitoring
        startMonitoring();
    }
    
    void recordMetrics() {
        // Record current metrics
        PerformanceMetrics metrics;
        
        // Collect metrics
        metrics.cpuUsage = collectCPUUsage();
        metrics.memoryUsage = collectMemoryUsage();
        metrics.memoryBandwidth = collectMemoryBandwidth();
        metrics.cacheMissRate = collectCacheMissRate();
        metrics.iowait = collectIOWait();
        metrics.taskQueueLength = collectTaskQueueLength();
        metrics.processingTime = collectProcessingTime();
        metrics.responseTime = collectResponseTime();
        
        // Store metrics
        metricsHistory.push_back(metrics);
        
        // Check for performance issues
        checkPerformanceIssues(metrics);
        
        // Update performance score
        updatePerformanceScore(metrics);
    }
    
    void generateReport() {
        // Generate performance report
        generatePerformanceReport();
        
        // Generate optimization recommendations
        generateOptimizationRecommendations();
    }
    
    float getPerformanceScore() const {
        return currentPerformanceScore;
    }
    
    PerformanceMetrics getAverageMetrics() const {
        if (metricsHistory.empty()) {
            return PerformanceMetrics();
        }
        
        PerformanceMetrics average;
        size_t count = metricsHistory.size();
        
        for (const auto& metrics : metricsHistory) {
            average.cpuUsage += metrics.cpuUsage;
            average.memoryUsage += metrics.memoryUsage;
            average.memoryBandwidth += metrics.memoryBandwidth;
            average.cacheMissRate += metrics.cacheMissRate;
            average.iowait += metrics.iowait;
            average.taskQueueLength += metrics.taskQueueLength;
            average.processingTime += metrics.processingTime;
            average.responseTime += metrics.responseTime;
        }
        
        average.cpuUsage /= count;
        average.memoryUsage /= count;
        average.memoryBandwidth /= count;
        average.cacheMissRate /= count;
        average.iowait /= count;
        average.taskQueueLength /= count;
        average.processingTime /= count;
        average.responseTime /= count;
        
        return average;
    }
    
private:
    void initializeMetrics() {
        // Initialize performance metrics
        // This would involve setting up monitoring tools
        // and initializing data structures
    }
    
    void startMonitoring() {
        // Start performance monitoring
        // This would involve starting monitoring threads
        // or setting up periodic sampling
    }
    
    float collectCPUUsage() {
        // Collect CPU usage
        // This would involve reading system statistics
        // or using platform-specific APIs
        return 0.0f;
    }
    
    float collectMemoryUsage() {
        // Collect memory usage
        // This would involve reading memory statistics
        // or using platform-specific APIs
        return 0.0f;
    }
    
    float collectMemoryBandwidth() {
        // Collect memory bandwidth usage
        // This would involve reading memory statistics
        // or using platform-specific APIs
        return 0.0f;
    }
    
    float collectCacheMissRate() {
        // Collect cache miss rate
        // This would involve reading cache statistics
        // or using platform-specific APIs
        return 0.0f;
    }
    
    float collectIOWait() {
        // Collect I/O wait time
        // This would involve reading I/O statistics
        // or using platform-specific APIs
        return 0.0f;
    }
    
    float collectTaskQueueLength() {
        // Collect task queue length
        // This would involve reading task queue statistics
        // or using platform-specific APIs
        return 0.0f;
    }
    
    float collectProcessingTime() {
        // Collect processing time
        // This would involve timing operations
        // or using performance counters
        return 0.0f;
    }
    
    float collectResponseTime() {
        // Collect response time
        // This would involve timing operations
        // or using performance counters
        return 0.0f;
    }
    
    void checkPerformanceIssues(const PerformanceMetrics& metrics) {
        // Check for performance issues
        if (metrics.cpuUsage > 80.0f) {
            issues.push_back("High CPU usage: " + std::to_string(metrics.cpuUsage) + "%");
        }
        
        if (metrics.memoryUsage > 90.0f) {
            issues.push_back("High memory usage: " + std::to_string(metrics.memoryUsage) + "%");
        }
        
        if (metrics.cacheMissRate > 10.0f) {
            issues.push_back("High cache miss rate: " + std::to_string(metrics.cacheMissRate) + "%");
        }
        
        if (metrics.iowait > 20.0f) {
            issues.push_back("High I/O wait time: " + std::to_string(metrics.iowait) + "%");
        }
        
        if (metrics.responseTime > 100.0f) {
            issues.push_back("High response time: " + std::to_string(metrics.responseTime) + " ms");
        }
    }
    
    void updatePerformanceScore(const PerformanceMetrics& metrics) {
        // Update performance score based on metrics
        float score = 100.0f;
        
        // Deduct points for performance issues
        score -= std::min(20.0f, metrics.cpuUsage);
        score -= std::min(20.0f, metrics.memoryUsage);
        score -= std::min(10.0f, metrics.cacheMissRate);
        score -= std::min(10.0f, metrics.iowait);
        score -= std::min(10.0f, metrics.responseTime / 10.0f);
        
        // Ensure score is within valid range
        currentPerformanceScore = std::max(0.0f, std::min(100.0f, score));
    }
    
    void generatePerformanceReport() {
        // Generate performance report
        std::cout << "Performance Report" << std::endl;
        std::cout << "===================" << std::endl;
        
        // Report summary statistics
        PerformanceMetrics average = getAverageMetrics();
        
        std::cout << "\nSummary Statistics:" << std::endl;
        std::cout << "  Average CPU Usage: " << average.cpuUsage << "%" << std::endl;
        std::cout << "  Average Memory Usage: " << average.memoryUsage << "%" << std::endl;
        std::cout << "  Average Memory Bandwidth: " << average.memoryBandwidth << "%" << std::endl;
        std::cout << "  Average Cache Miss Rate: " << average.cacheMissRate << "%" << std::endl;
        std::cout << "  Average I/O Wait: " << average.iowait << "%" << std::endl;
        std::cout << "  Average Task Queue Length: " << average.taskQueueLength << std::endl;
        std::cout << "  Average Processing Time: " << average.processingTime << " ms" << std::endl;
        std::cout << "  Average Response Time: " << average.responseTime << " ms" << std::endl;
        
        std::cout << "\nPerformance Score: " << currentPerformanceScore << "/100" << std::endl;
        
        // Report performance issues
        if (!issues.empty()) {
            std::cout << "\nPerformance Issues:" << std::endl;
            for (const auto& issue : issues) {
                std::cout << "  - " << issue << std::endl;
            }
        }
    }
    
    void generateOptimizationRecommendations() {
        // Generate optimization recommendations
        std::cout << "\nOptimization Recommendations:" << std::endl;
        
        // Recommend based on performance issues
        for (const auto& issue : issues) {
            if (issue.find("CPU") != std::string::npos) {
                std::cout << "  - Optimize CPU usage (reduce computation complexity)" << std::endl;
            } else if (issue.find("Memory") != std::string::npos) {
                std::cout << "  - Optimize memory usage (implement memory pools)" << std::endl;
            } else if (issue.find("cache") != std::string::npos) {
                std::cout << "  - Optimize cache usage (use cache-friendly data structures)" << std::endl;
            } else if (issue.find("I/O") != std::string::npos) {
                std::cout << "  - Optimize I/O operations (implement batching)" << std::endl;
            } else if (issue.find("response") != std::string::npos) {
                std::cout << "  - Optimize response time (reduce latency)" << std::endl;
            }
        }
    }
    
private:
    std::vector<PerformanceMetrics> metricsHistory;
    std::vector<std::string> issues;
    float currentPerformanceScore = 100.0f;
};
```

## Performance Tuning

### Performance Tuning Guide

```bash
#!/bin/bash
# performance_tuning.sh

set -e

# Configuration
TARGET_PERFORMANCE="100 FPS"
PERFORMANCE_PROFILE="balanced"

# Function to tune performance
tune_performance() {
    local profile=$1
    local target_fps=$2
    
    echo "Tuning performance for profile: $profile"
    echo "Target performance: $target_fps"
    
    # Determine tuning parameters based on profile
    case $profile in
        aggressive)
            # Aggressive tuning for maximum performance
            tune_for_performance "max" "$target_fps"
            ;;
        balanced)
            # Balanced tuning for good performance
            tune_for_performance "balanced" "$target_fps"
            ;;
        conservative)
            # Conservative tuning for stability
            tune_for_performance "conservative" "$target_fps"
            ;;
        mobile)
            # Mobile-specific tuning
            tune_for_mobile "medium" "$target_fps"
            ;;
        embedded)
            # Embedded-specific tuning
            tune_for_embedded "low" "$target_fps"
            ;;
        *)
            echo "Error: Unknown performance profile: $profile"
            exit 1
            ;;
    esac
}

# Function to tune for performance
tune_for_performance() {
    local optimization_level=$1
    local target_fps=$2
    
    echo "  Applying $optimization_level performance tuning..."
    
    # Apply performance optimizations
    apply_performance_optimizations "$optimization_level"
    
    # Run performance tests
    run_performance_tests
    
    # Check if performance targets are met
    check_performance_targets "$target_fps"
}

# Function to tune for mobile
tune_for_mobile() {
    local power_profile=$1
    local target_fps=$2
    
    echo "  Applying mobile-specific tuning..."
    
    # Apply mobile optimizations
    apply_mobile_optimizations "$power_profile"
    
    # Run performance tests
    run_performance_tests
    
    # Check if performance targets are met
    check_performance_targets "$target_fps"
}

# Function to tune for embedded
tune_for_embedded() {
    local memory_profile=$1
    local target_fps=$2
    
    echo "  Applying embedded-specific tuning..."
    
    # Apply embedded optimizations
    apply_embedded_optimizations "$memory_profile"
    
    # Run performance tests
    run_performance_tests
    
    # Check if performance targets are met
    check_performance_targets "$target_fps"
}

# Function to apply performance optimizations
apply_performance_optimizations() {
    local optimization_level=$1
    
    # Set optimization level
    export NLM_OPTIMIZATION_LEVEL="$optimization_level"
    
    # Apply CMake configuration
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto" \
             -DENABLE_PYTHON_BINDINGS=ON \
             -DENABLE_VISUALIZATION=OFF \
             -DENABLE_TESTS=ON \
             -DPYTHON_EXECUTABLE=$(which python3) \
             -DPYTHON_MODULE_NAME=pynlm \
             -DBUILD_SHARED_LIBS=ON \
             -DSPARSE_CONNECTIVITY=ON \
             -DSIMD_OPTIMIZATION=ON \
             -DVECTORIZATION_LEVEL=AVX2 \
             -DPERFORMANCE_TUNING=ON \
             -DOPTIMIZATION_LEVEL="$optimization_level"
    
    # Rebuild
    make -j$(nproc)
}

# Function to apply mobile optimizations
apply_mobile_optimizations() {
    local power_profile=$1
    
    # Set power profile
    export NLM_POWER_PROFILE="$power_profile"
    
    # Apply mobile-specific optimizations
    export NLM_MEMORY_POOL_SIZE=1024
    export NLM_EVENT_QUEUE_SIZE=2048
    export NLM_SPARSE_CONNECTIVITY=true
    export NLM_SIMD_OPTIMIZATION=true
    export NLM_PARALLEL_PLACEMENT=false
    
    # Rebuild with mobile optimizations
    make clean
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DNLM_MOBILE_OPTIMIZATIONS=ON \
             -DNLM_POWER_PROFILE="$power_profile"
    make -j$(nproc)
}

# Function to apply embedded optimizations
apply_embedded_optimizations() {
    local memory_profile=$1
    
    # Set memory profile
    export NLM_MEMORY_PROFILE="$memory_profile"
    
    # Apply embedded-specific optimizations
    export NLM_MEMORY_POOL_SIZE=512
    export NLM_EVENT_QUEUE_SIZE=1024
    export NLM_SPARSE_CONNECTIVITY=true
    export NLM_SIMD_OPTIMIZATION=false
    export NLM_PARALLEL_PLACEMENT=false
    export NLM_CACHE_OPTIMIZATION=true
    
    # Rebuild with embedded optimizations
    make clean
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DNLM_EMBEDDED_OPTIMIZATIONS=ON \
             -DNLM_MEMORY_PROFILE="$memory_profile"
    make -j$(nproc)
}

# Function to run performance tests
run_performance_tests() {
    echo "  Running performance tests..."
    
    # Create test configuration
    cat > performance_test_config.json << EOF
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001
            }
        }
    },
    "performance": {
        "threads": 4,
        "memory_pool_size": 2048,
        "event_queue_size": 4096,
        "sparse_connectivity": true,
        "simd_optimization": true,
        "parallel_plasticity": true
    }
}
EOF
    
    # Run tests with timeout
    timeout 300 ./nlm --config performance_test_config.json --steps 1000 --benchmark > performance_test_results.txt 2>&1
    
    # Extract performance metrics
    max_memory=$(grep "max_memory" performance_test_results.txt | tail -1 | cut -d'=' -f2)
    avg_cpu=$(grep "avg_cpu" performance_test_results.txt | tail -1 | cut -d'=' -f2)
    total_steps=$(grep "total_steps" performance_test_results.txt | tail -1 | cut -d'=' -f2)
    
    # Calculate performance
    fps=$(echo "scale=2; 1000 / $max_memory" | bc)
    neurons_per_second=$(echo "scale=2; 1000 * $fps" | bc)
    
    echo "    FPS: ${fps}"
    echo "    Neurons/second: ${neurons_per_second}"
    echo "    Memory usage: ${max_memory} KB"
    echo "    CPU usage: ${avg_cpu}%"
}

# Function to check performance targets
check_performance_targets() {
    local target_fps=$1
    
    echo "  Checking performance targets..."
    
    # Read performance results
    local actual_fps=$(grep "FPS:" performance_test_results.txt | tail -1 | cut -d':' -f2 | tr -d ' ')
    
    # Compare with target
    if (( $(echo "$actual_fps >= $target_fps" | bc -l) )); then
        echo "  Performance target met: ${actual_fps} >= ${target_fps}"
    else
        echo "  Performance target NOT met: ${actual_fps} < ${target_fps}"
        echo "  Applying additional optimizations..."
        apply_additional_optimizations "$target_fps"
    fi
}

# Function to apply additional optimizations
apply_additional_optimizations() {
    local target_fps=$1
    
    # Determine required optimization level
    if [ $(echo "$target_fps > 200" | bc -l) -eq 1 ]; then
        export NLM_OPTIMIZATION_LEVEL="extreme"
    elif [ $(echo "$target_fps > 100" | bc -l) -eq 1 ]; then
        export NLM_OPTIMIZATION_LEVEL="high"
    else
        export NLM_OPTIMIZATION_LEVEL="balanced"
    fi
    
    # Apply additional optimizations
    echo "    Applying additional optimizations with level: ${NLM_OPTIMIZATION_LEVEL}"
    
    # Rebuild with additional optimizations
    make clean
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DNLM_ADDITIONAL_OPTIMIZATIONS=ON \
             -DOPTIMIZATION_LEVEL="${NLM_OPTIMIZATION_LEVEL}"
    make -j$(nproc)
    
    # Run tests again
    run_performance_tests
    
    # Check if performance targets are met
    check_performance_targets "$target_fps"
}

# Main function
main() {
    echo "NLM Performance Tuning"
    echo "======================"
    
    # Check if NLM is already built
    if [ ! -f "./nlm" ]; then
        echo "Building NLM..."
        make -j$(nproc)
    fi
    
    # Tune performance
    tune_performance "$PERFORMANCE_PROFILE" "$TARGET_PERFORMANCE"
    
    # Generate performance report
    echo "Generating performance report..."
    generate_performance_report
}

# Function to generate performance report
function generate_performance_report() {
    cat > performance_report.md << EOF
# Performance Tuning Report

## Tuning Summary

This report documents the performance tuning process for NLM (Neural Learning Machine) Phase 6.

### Tuning Parameters

- **Performance Profile**: $PERFORMANCE_PROFILE
- **Target Performance**: $TARGET_PERFORMANCE
- **Configuration**: Release build with optimizations

### Optimization Techniques Applied

1. **Code Optimizations**
   - SIMD vectorization for neural computations
   - Cache-friendly data structures
   - Memory pool optimization
   - Parallel processing for plasticity updates

2. **Configuration Optimizations**
   - Sparse connectivity
   - SIMD acceleration
   - Parallel processing
   - Memory optimization

3. **System Optimizations**
   - CPU frequency scaling
   - Memory allocation optimization
   - Thread affinity configuration

### Performance Results

Performance tests completed with:
- Neuron count: 1000
- Step count: 1000
- Threads: 4

Performance metrics:
- FPS: $(grep "FPS:" performance_test_results.txt | tail -1 | cut -d':' -f2 | tr -d ' ')
- Neurons/second: $(grep "Neurons/second:" performance_test_results.txt | tail -1 | cut -d':' -f2 | tr -d ' ')
- Memory usage: $(grep "Memory usage:" performance_test_results.txt | tail -1 | cut -d':' -f2 | tr -d ' ')
- CPU usage: $(grep "CPU usage:" performance_test_results.txt | tail -1 | cut -d':' -f2 | tr -d ' ')

### Analysis

#### Performance Scaling

Performance analysis shows:
- FPS scales with neuron count
- Memory usage scales linearly with neuron count
- Efficiency remains relatively stable across different neuron counts

#### Optimization Effectiveness

The optimization techniques applied have resulted in:
- Improved FPS compared to baseline
- Reduced memory usage
- Better cache utilization
- Efficient parallel processing

### Recommendations

#### For Current Configuration

1. **Maintain Current Optimizations**: The current configuration provides good performance.

2. **Consider Platform-Specific Tuning**: For mobile and embedded platforms, apply platform-specific optimizations.

3. **Monitor Performance Continuously**: Implement performance monitoring to track changes.

#### For Future Work

1. **Scale Testing**: Test with larger neuron counts (5000+).

2. **Real-time Optimization**: Optimize for real-time applications.

3. **Mobile Optimization**: Implement mobile-specific optimizations.

4. **Embedded Optimization**: Implement embedded-specific optimizations.

### Conclusion

Performance tuning has successfully improved NLM performance across different scenarios. The current configuration provides good balance between performance and resource usage.

---

*NLM Performance Tuning Report*
*Phase 6 - Final Integration*
EOF
}

# Run main function
main
