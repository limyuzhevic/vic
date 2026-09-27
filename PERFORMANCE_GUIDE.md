# NLM Performance Guide
# ======================

This comprehensive guide documents performance optimization techniques for the NLM (Neural Learning Machine) framework, covering memory management, SIMD optimization, cache optimization, parallel processing, and performance monitoring for both Python and C++ implementations.

## Overview

Performance optimization is crucial for achieving biologically realistic simulation speeds while maintaining accuracy and scalability. The NLM framework provides multiple optimization strategies across different layers of the system.

### Performance Optimization Layers

1. **Memory Management**: Zero-allocation memory pools and efficient data structures
2. **SIMD Optimization**: Vectorized operations for neural computations
3. **Cache Optimization**: Cache-friendly data layouts and memory access patterns
4. **Parallel Processing**: Multi-core optimization and distributed computing
5. **Performance Monitoring**: Real-time profiling and benchmarking tools

## Memory Pool System

### Zero-Allocation Memory Management

The NLM framework implements a sophisticated memory pool system to eliminate allocation overhead and improve cache locality:

```cpp
class MemoryPool {
public:
    MemoryPool(size_t blockSize, size_t numBlocks);
    ~MemoryPool();
    
    void* allocate();
    void deallocate(void* ptr);
    void clear();
    size_t getUsedMemory() const;
    size_t getTotalMemory() const;
};

// Example usage in neural components
class NeuralPopulation {
private:
    MemoryPool neuronPool;
    MemoryPool synapsePool;
    
public:
    NeuralPopulation(size_t neuronCount) 
        : neuronPool(sizeof(Neuron), neuronCount),
          synapsePool(sizeof(Synapse), neuronCount * 10) // Approximate synapse count
    {
    }
    
    Neuron* createNeuron() {
        return static_cast<Neuron*>(neuronPool.allocate());
    }
    
    void deleteNeuron(Neuron* neuron) {
        neuronPool.deallocate(neuron);
    }
};
```

### Memory Pool Optimization Strategies

#### Pool Sizing
```cpp
// Calculate optimal pool sizes based on configuration
void calculatePoolSizes(const Config& config, size_t& neuronPoolSize, 
                       size_t& synapsePoolSize, size_t& eventPoolSize) {
    
    // Neuron pool: based on neuron count with safety margin
    neuronPoolSize = config.getNeuronCount() * 2; // 2x safety margin
    
    // Synapse pool: based on connectivity density
    double connectivity = config.getSynapseDensity();
    size_t estimatedSynapses = static_cast<size_t>(
        config.getNeuronCount() * config.getNeuronCount() * connectivity
    );
    synapsePoolSize = estimatedSynapses * 2; // 2x safety margin
    
    // Event pool: based on expected event rate
    double eventRate = config.getEventRate();
    double simulationTime = config.getSimulationTime();
    size_t estimatedEvents = static_cast<size_t>(eventRate * simulationTime);
    eventPoolSize = estimatedEvents * 4; // 4x for burst handling
}
```

#### Pool Reuse Strategies
```cpp
class MemoryManager {
public:
    enum ReusePolicy {
        AGGRESSIVE,  // Reuse as much as possible
        BALANCED,    // Balanced reuse and allocation
        CONSERVATIVE  // Minimize reuse for stability
    };
    
    MemoryManager(ReusePolicy policy = BALANCED);
    
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        void* memory = getPool(sizeof(T)).allocate();
        return new(memory) T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    void deallocate(T* ptr) {
        ptr->~T();
        getPool(sizeof(T)).deallocate(ptr);
    }
    
    void clear() {
        for (auto& pool : memoryPools) {
            pool.clear();
        }
    }
    
    size_t getTotalMemoryUsage() const;
    size_t getFragmentationRatio() const;

private:
    std::vector<MemoryPool> memoryPools;
    ReusePolicy policy;
};
```

## SIMD Optimization

### Vectorized Neural Operations

The NLM framework leverages SIMD (Single Instruction, Multiple Data) optimizations for computationally intensive neural operations:

```cpp
class VectorizedOperations {
public:
    // Vectorized membrane potential integration
    static void integrateMembranePotentialSIMD(
        float* v_membrane, 
        const float* I_syn, 
        const float* g_leak,
        const float* tau_mem,
        size_t neuronCount,
        float dt
    ) {
        constexpr size_t SIMD_WIDTH = 8; // AVX2
        size_t alignedCount = (neuronCount / SIMD_WIDTH) * SIMD_WIDTH;
        
        // Process in SIMD chunks
        for (size_t i = 0; i < alignedCount; i += SIMD_WIDTH) {
            // Load SIMD registers
            __m256 v_current = _mm256_loadu_ps(&v_membrane[i]);
            __m256 I_total = _mm256_loadu_ps(&I_syn[i]);
            __m256 g_leak_val = _mm256_loadu_ps(&g_leak[i]);
            __m256 tau_val = _mm256_loadu_ps(&tau_mem[i]);
            
            // Vectorized computation: dv/dt = (-(v - v_rest) + I_total) / tau
            __m256 v_rest = _mm256_set1(-65.0f); // Typical resting potential
            __m256 dv = _mm256_mul_ps(_mm256_sub_ps(_mm256_sub_ps(v_current, v_rest), I_total), 
                                     _mm256_div_ps(_mm256_set1(1.0f), tau_val));
            
            // Update membrane potential
            v_current = _mm256_add_ps(v_current, _mm256_mul_ps(dv, _mm256_set1(dt)));
            _mm256_storeu_ps(&v_membrane[i], v_current);
        }
        
        // Handle remaining neurons (non-SIMD)
        for (size_t i = alignedCount; i < neuronCount; ++i) {
            v_membrane[i] += dt * (-(v_membrane[i] - (-65.0f)) + I_syn[i]) / tau_mem[i];
        }
    }
    
    // Vectorized spike detection
    static void detectSpikesSIMD(
        const float* v_membrane,
        bool* spikeFlags,
        const float* v_thresh,
        size_t neuronCount
    ) {
        constexpr size_t SIMD_WIDTH = 16; // AVX-512
        size_t alignedCount = (neuronCount / SIMD_WIDTH) * SIMD_WIDTH;
        
        // Process in SIMD chunks
        for (size_t i = 0; i < alignedCount; i += SIMD_WIDTH) {
            __m512 v_current = _mm512_loadu_ps(&v_membrane[i]);
            __m512 threshold = _mm512_loadu_ps(&v_thresh[i]);
            
            // Vectorized spike detection: v > v_thresh
            __m512 spike_mask = _mm512_cmpgt_ps(v_current, threshold);
            
            // Store spike flags
            int spike_bits = _mm512_movemask_ps(spike_mask);
            for (int bit = 0; bit < SIMD_WIDTH; ++bit) {
                if (spike_bits & (1 << bit)) {
                    spikeFlags[i + bit] = true;
                }
            }
        }
        
        // Handle remaining neurons
        for (size_t i = alignedCount; i < neuronCount; ++i) {
            spikeFlags[i] = v_membrane[i] > v_thresh[i];
        }
    }
    
    // Vectorized synaptic weight updates (STDP)
    static void updateSTDPSIMD(
        float* weights,
        const float* preSpikes,
        const float* postSpikes,
        const float* learningRate,
        const float* tau_plus,
        const float* tau_minus,
        size_t neuronCount,
        float dt
    ) {
        // Pre- and post-synaptic spike traces
        static thread_local std::vector<float> trace_plus(neuronCount, 0.0f);
        static thread_local std::vector<float> trace_minus(neuronCount, 0.0f);
        
        // Update traces
        for (size_t i = 0; i < neuronCount; ++i) {
            trace_plus[i] = std::exp(-dt / tau_plus[i]) * trace_plus[i];
            trace_minus[i] = std::exp(-dt / tau_minus[i]) * trace_minus[i];
            
            if (preSpikes[i]) {
                trace_minus[i] += 1.0f;
            }
            if (postSpikes[i]) {
                trace_plus[i] += 1.0f;
            }
        }
        
        // Vectorized weight updates
        constexpr size_t SIMD_WIDTH = 8;
        size_t alignedCount = (neuronCount / SIMD_WIDTH) * SIMD_WIDTH;
        
        for (size_t i = 0; i < alignedCount; i += SIMD_WIDTH) {
            __m256 w_current = _mm256_loadu_ps(&weights[i]);
            __m256 trace_plus_vec = _mm256_loadu_ps(&trace_plus[i]);
            __m256 trace_minus_vec = _mm256_loadu_ps(&trace_minus[i]);
            __m256 lr_vec = _mm256_loadu_ps(&learningRate[i]);
            
            // STDP update: Δw = A_plus * trace_plus - A_minus * trace_minus
            __m256 dw = _mm256_sub_ps(
                _mm256_mul_ps(_mm256_set1(0.01f), trace_plus_vec),  // A_plus
                _mm256_mul_ps(_mm256_set1(0.01f), trace_minus_vec)  // A_minus
            );
            
            w_current = _mm256_add_ps(w_current, dw);
            _mm256_storeu_ps(&weights[i], w_current);
        }
        
        // Handle remaining synapses
        for (size_t i = alignedCount; i < neuronCount; ++i) {
            weights[i] += 0.01f * trace_plus[i] - 0.01f * trace_minus[i];
        }
    }
};
```

### Auto-Detection of SIMD Support

```cpp
class SIMDDetector {
public:
    enum class SIMDLevel {
        NONE,
        SSE2,
        AVX,
        AVX2,
        AVX512
    };
    
    static SIMDLevel detectSIMD() {
        int cpuInfo[4];
        
        // Check SSE2 (always available on x86_64)
        SIMDLevel level = SIMDLevel::SSE2;
        
        // Check AVX
        if (checkCPUExtension("avx")) {
            level = SIMDLevel::AVX;
            
            // Check AVX2
            if (checkCPUExtension("avx2")) {
                level = SIMDLevel::AVX2;
                
                // Check AVX-512
                if (checkCPUExtension("avx512f")) {
                    level = SIMDLevel::AVX512;
                }
            }
        }
        
        return level;
    }
    
    static std::string SIMDLevelToString(SIMDLevel level) {
        switch (level) {
            case SIMDLevel::NONE: return "None";
            case SIMDLevel::SSE2: return "SSE2";
            case SIMDLevel::AVX: return "AVX";
            case SIMDLevel::AVX2: return "AVX2";
            case SIMDLevel::AVX512: return "AVX-512";
            default: return "Unknown";
        }
    }
    
    static bool checkCPUExtension(const std::string& extension) {
        // Implementation varies by platform
        #ifdef __x86_64__
        unsigned long long featureBits = getFeatureBits();
        
        if (extension == "sse2") return featureBits & (1 << 26);
        if (extension == "avx") return featureBits & (1 << 28);
        if (extension == "avx2") {
            int cpuInfo[4];
            __cpuid(7, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
            return cpuInfo[1] & (1 << 5);
        }
        if (extension == "avx512f") {
            int cpuInfo[4];
            __cpuid(7, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
            return cpuInfo[1] & (1 << 16);
        }
        #endif
        return false;
    }
};
```

## Cache Optimization

### Cache-Friendly Data Layout

The NLM framework uses several strategies to optimize cache usage:

#### Structure of Arrays (SoA) vs Array of Structures (AoS)

```cpp
// Traditional AoS (poor cache locality for parallel operations)
struct Neuron {
    float x, y, z;           // Position (3 floats)
    float v_membrane;        // Membrane potential (1 float)
    float v_threshold;       // Threshold (1 float)
    float v_resting;         // Resting potential (1 float)
    float tau_membrane;      // Time constant (1 float)
    float g_leak;            // Leak conductance (1 float)
    // ... other properties
};

Neuron neurons[1000];  // 8 floats per neuron, poor for vector operations

// Optimized SoA layout (excellent cache locality)
struct NeuronSoA {
    // All neurons' properties in separate arrays
    float* positions;      // Array of position vectors
    float* membranePotentials;
    float* thresholds;
    float* restingPotentials;
    float* timeConstants;
    float* leakConductances;
    // ... other properties as arrays
};

// Cache-aligned allocation
void allocateAlignedMemory(NeuronSoA& neurons, size_t count) {
    constexpr size_t ALIGNMENT = 64; // Cache line size
    
    neurons.positions = static_cast<float*>(aligned_alloc(ALIGNMENT, count * 3 * sizeof(float)));
    neurons.membranePotentials = static_cast<float*>(aligned_alloc(ALIGNMENT, count * sizeof(float)));
    // ... allocate other arrays
    
    // Initialize to zero
    memset(neurons.positions, 0, count * 3 * sizeof(float));
    memset(neurons.membranePotentials, 0, count * sizeof(float));
    // ... etc.
}
```

#### Cache Line Padding
```cpp
class CacheLineOptimized {
public:
    // Natural cache line padding (64 bytes on most architectures)
    struct AlignedFloat {
        alignas(64) float value;
        
        AlignedFloat(float v = 0.0f) : value(v) {}
    };
    
    // Cache-efficient synaptic connections
    struct Synapse {
        alignas(64) int preNeuronId;
        alignas(64) int postNeuronId;
        alignas(64) float weight;
        alignas(64) float delay;
        alignas(64) float efficacy;
    };
    
    // Neuron population with cache-aware layout
    class NeuronPopulation {
    private:
        // One cache line per neuron for exclusive access
        struct NeuronEntry {
            float v_membrane;        // Cache line 1
            float v_threshold;       // Cache line 2
            float v_resting;         // Cache line 3
            float I_syn;             // Cache line 4
            float tau_membrane;      // Cache line 5
            // ... other properties
        };
        
        NeuronEntry* neurons;
        size_t neuronCount;
        size_t lineSize;
        
    public:
        NeuronPopulation(size_t count) : neuronCount(count) {
            lineSize = sizeof(NeuronEntry);
            size_t totalMemory = neuronCount * lineSize;
            neurons = static_cast<NeuronEntry*>(aligned_alloc(64, totalMemory));
        }
        
        ~NeuronPopulation() {
            free(neurons);
        }
        
        // Access methods ensure cache locality
        float getMembranePotential(size_t id) const {
            return neurons[id].v_membrane;
        }
        
        void setMembranePotential(size_t id, float value) {
            neurons[id].v_membrane = value;
        }
        
        // Vectorized operations on cache-aligned data
        void updateAllMembranePotentials(const float* I_syn, float dt, size_t chunkSize) {
            // Process in chunks that fit in cache
            for (size_t i = 0; i < neuronCount; i += chunkSize) {
                size_t end = std::min(i + chunkSize, neuronCount);
                updateChunk(i, end, I_syn, dt);
            }
        }
    };
};
```

### Memory Access Pattern Optimization

```cpp
class AccessPatternOptimizer {
public:
    // Sequential memory access pattern
    static void sequentialWrite(float* data, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = computeValue(i);
        }
    }
    
    // Vectorized sequential access (best cache performance)
    static void vectorizedWriteSIMD(float* data, size_t size) {
        constexpr size_t SIMD_WIDTH = 16;
        size_t alignedSize = (size / SIMD_WIDTH) * SIMD_WIDTH;
        
        for (size_t i = 0; i < alignedSize; i += SIMD_WIDTH) {
            __m512 values = computeValuesSIMD(i);
            _mm512_storeu_ps(&data[i], values);
        }
        
        // Tail handling
        for (size_t i = alignedSize; i < size; ++i) {
            data[i] = computeValue(i);
        }
    }
    
    // Cache-aware read-modify-write pattern
    static void readModifyWrite(float* data, const float* updates, 
                               size_t size, float (*operation)(float, float)) {
        // Prefetch next cache line
        for (size_t i = 0; i < size; ++i) {
            if (i + CACHE_LINE_SIZE < size) {
                _mm_prefetch(&data[i + CACHE_LINE_SIZE], _MM_HINT_T0);
            }
            
            data[i] = operation(data[i], updates[i]);
        }
    }
    
    // Transpose for better cache utilization
    static void matrixTranspose(const float* input, float* output, 
                               size_t rows, size_t cols) {
        // Process in cache-friendly blocks
        constexpr size_t BLOCK_SIZE = 32;
        
        for (size_t row = 0; row < rows; row += BLOCK_SIZE) {
            for (size_t col = 0; col < cols; col += BLOCK_SIZE) {
                size_t endRow = std::min(row + BLOCK_SIZE, rows);
                size_t endCol = std::min(col + BLOCK_SIZE, cols);
                
                transposeBlock(input, output, row, col, endRow, endCol);
            }
        }
    }
};
```

## Parallel Processing

### Multi-Core Optimization

```cpp
class ParallelProcessor {
public:
    enum class ThreadingModel {
        SINGLE_THREAD,
        OPENMP,
        THREAD_POOL,
        ASYNC
    };
    
    ParallelProcessor(ThreadingModel model = ThreadingModel::OPENMP);
    
    // Parallel neural simulation
    void simulateParallel(float* v_membrane, const float* I_syn, 
                         size_t neuronCount, float dt, size_t threadCount) {
        #ifdef _OPENMP
        #pragma omp parallel for num_threads(threadCount)
        for (size_t i = 0; i < neuronCount; ++i) {
            // Simulate individual neuron
            simulateNeuron(v_membrane[i], I_syn[i], dt);
        }
        #else
        // Fallback to sequential
        simulateSequential(v_membrane, I_syn, neuronCount, dt);
        #endif
    }
    
    // Parallel STDP learning
    void updateSTDPPParallel(float* weights, const float* traces, 
                           size_t synapseCount, size_t threadCount) {
        #ifdef _OPENMP
        #pragma omp parallel for num_threads(threadCount)
        for (size_t i = 0; i < synapseCount; ++i) {
            weights[i] = std::min(1.0f, weights[i] + traces[i] * 0.01f);
        }
        #else
        // Sequential fallback
        for (size_t i = 0; i < synapseCount; ++i) {
            weights[i] = std::min(1.0f, weights[i] + traces[i] * 0.01f);
        }
        #endif
    }
    
    // Work-stealing thread pool
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        
        template<typename Func>
        void submit(Func&& task) {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<Func>(task));
            lock.unlock();
            condition.notify_one();
        }
        
        void waitForCompletion();
        
    private:
        void workerThread();
        
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<size_t> completedTasks{0};
        std::atomic<size_t> totalTasks{0};
    };
    
    // Asynchronous computation for non-blocking operations
    class AsyncComputation {
    public:
        template<typename Func>
        Future<typename std::result_of<Func()>::type> computeAsync(Func&& task) {
            return std::async(std::launch::async, std::forward<Func>(task));
        }
        
        // Batch processing with async completion
        template<typename InputIterator, typename OutputIterator, typename Func>
        void processBatchAsync(InputIterator begin, InputIterator end, 
                              OutputIterator out, Func&& func) {
            size_t batchSize = std::distance(begin, end);
            size_t chunkSize = batchSize / std::thread::hardware_concurrency();
            
            std::vector<std::future<void>> futures;
            
            for (size_t i = 0; i < batchSize; i += chunkSize) {
                size_t currentChunkSize = std::min(chunkSize, batchSize - i);
                InputIterator chunkBegin = begin + i;
                InputIterator chunkEnd = chunkBegin + currentChunkSize;
                
                futures.push_back(std::async(
                    std::launch::async,
                    [chunkBegin, chunkEnd, out, func]() {
                        OutputIterator currentOut = out + (chunkBegin - begin);
                        for (auto it = chunkBegin; it != chunkEnd; ++it, ++currentOut) {
                            *currentOut = func(*it);
                        }
                    }
                ));
            }
            
            // Wait for all computations to complete
            for (auto& future : futures) {
                future.wait();
            }
        }
    };
};
```

### Dynamic Load Balancing

```cpp
class DynamicLoadBalancer {
public:
    struct WorkUnit {
        size_t startIndex;
        size_t endIndex;
        double estimatedWork;
        
        WorkUnit(size_t s, size_t e) : startIndex(s), endIndex(e), 
                                       estimatedWork(end - start) {}
    };
    
    DynamicLoadBalancer(size_t numWorkers);
    
    template<typename WorkFunc>
    void distributeWork(const std::vector<WorkUnit>& workUnits, WorkFunc&& workFunc) {
        std::vector<std::future<void>> futures;
        futures.reserve(workUnits.size());
        
        for (const auto& unit : workUnits) {
            futures.push_back(std::async(std::launch::async, 
                [unit, &workFunc]() {
                    for (size_t i = unit.startIndex; i < unit.endIndex; ++i) {
                        workFunc(i);
                    }
                }
            ));
        }
        
        // Wait for all work to complete
        for (auto& future : futures) {
            future.wait();
        }
    }
    
    // Adaptive load balancing based on work completion
    template<typename WorkFunc>
    void adaptiveDistributeWork(WorkFunc&& workFunc, size_t totalWork) {
        std::vector<std::thread> workers;
        std::vector<std::atomic<size_t>> workCounters;
        
        for (size_t i = 0; i < numWorkers; ++i) {
            workCounters.emplace_back(0);
            workers.emplace_back([this, &workFunc, &workCounters, i]() {
                while (true) {
                    size_t workIndex = getWorkIndex(workCounters, i);
                    if (workIndex >= totalWork) break;
                    
                    workFunc(workIndex);
                }
            });
        }
        
        for (auto& worker : workers) {
            worker.join();
        }
    }
    
private:
    size_t getWorkIndex(std::vector<std::atomic<size_t>>& counters, size_t workerId) {
        // Atomic increment with work stealing
        for (size_t attempt = 0; attempt < counters.size() * 2; ++attempt) {
            size_t current = counters[workerId].load();
            if (current >= totalWork) return totalWork;
            
            if (counters[workerId].compare_exchange_weak(current, current + 1)) {
                return current;
            }
            
            // Work stealing: find another worker with available work
            for (size_t otherId = 0; otherId < counters.size(); ++otherId) {
                if (otherId == workerId) continue;
                
                size_t otherWork = counters[otherId].load();
                if (otherWork < totalWork) {
                    if (counters[otherId].compare_exchange_weak(otherWork, otherWork - 1)) {
                        return otherWork;
                    }
                }
            }
        }
        
        return totalWork;
    }
    
    size_t numWorkers;
    size_t totalWork;
};
```

## Performance Monitoring

### Real-Time Profiling

```cpp
class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double cpuUsage;           // CPU utilization percentage
        double memoryUsage;        // Memory usage in MB
        double cacheMissRate;      // Cache miss rate (0-1)
        double simdUtilization;    // SIMD instruction utilization
        double memoryBandwidth;    // Memory bandwidth in GB/s
        double computationTime;    // Time spent in computation
        double memoryTime;         // Time spent in memory operations
        double idleTime;           // Time spent idle
        
        double getIPC() const {    // Instructions per cycle
            return (computationTime + memoryTime) / (computationTime + memoryTime + idleTime);
        }
        
        double getMemoryEfficiency() const {  // Bytes processed per byte transferred
            return (computationTime * memoryBandwidth) / (memoryTime * memoryBandwidth + 1.0);
        }
    };
    
    PerformanceProfiler();
    
    void startMeasurement(const std::string& operationName);
    void stopMeasurement(const std::string& operationName);
    
    void recordCacheStats(size_t reads, size_t writes, size_t misses);
    void recordSIMDStats(size_t vectorizedOps, size_t scalarOps);
    void recordMemoryStats(size_t allocatedBytes, size_t deallocatedBytes);
    
    PerformanceMetrics getCurrentMetrics() const;
    PerformanceMetrics getAverageMetrics() const;
    
    void generateReport(const std::string& filename) const;
    void printReport() const;
    
private:
    struct OperationStats {
        std::string name;
        std::vector<double> timings;
        size_t cacheReads = 0;
        size_t cacheWrites = 0;
        size_t cacheMisses = 0;
        size_t simdOperations = 0;
        size_t scalarOperations = 0;
        size_t allocatedBytes = 0;
        size_t deallocatedBytes = 0;
    };
    
    std::vector<OperationStats> operationStats;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> activeMeasurements;
    mutable std::mutex statsMutex;
};
```

### Benchmark Suite

```cpp
class BenchmarkSuite {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTime;      // Seconds
        double throughput;         // Operations per second
        double memoryBandwidth;    // GB/s
        double cacheEfficiency;    // Hit rate
        size_t memoryAllocations;   // Number of allocations
        size_t memoryDeallocations; // Number of deallocations
        std::string details;
    };
    
    BenchmarkSuite();
    
    // Core benchmarks
    BenchmarkResult benchmarkMembranePotential(const Config& config);
    BenchmarkResult benchmarkSynapticTransmission(const Config& config);
    BenchmarkResult benchmarkSTDP(const Config& config);
    BenchmarkResult benchmarkCuriosity(const Config& config);
    BenchmarkResult benchmarkDevelopment(const Config& config);
    
    // Scalability benchmarks
    BenchmarkResult benchmarkScalability(const Config& config, 
                                        const std::vector<size_t>& neuronCounts);
    
    // Memory efficiency benchmarks
    BenchmarkResult benchmarkMemoryEfficiency(const Config& config,
                                              size_t duration);  // Seconds
    
    // SIMD effectiveness
    BenchmarkResult benchmarkSIMDEffectiveness(const Config& config);
    
    // Composite benchmarks
    BenchmarkResult benchmarkCompleteSimulation(const Config& config,
                                                size_t duration);  // Seconds
    
    std::vector<BenchmarkResult> runAllBenchmarks(const Config& config);
    
    void saveBenchmarkResults(const std::string& filename,
                             const std::vector<BenchmarkResult>& results);
    
    void printBenchmarkSummary(const std::vector<BenchmarkResult>& results) const;
    
    double calculateBenchmarkScore(const std::vector<BenchmarkResult>& results) const;
};  
```

### Performance Optimization Tips

#### Memory Management
- Use memory pools to eliminate allocation overhead
- Reuse objects instead of creating new ones
- Pre-allocate memory based on expected usage
- Use aligned memory allocation for SIMD operations
- Implement custom deleters for smart pointers

#### Computation Optimization
- Use SIMD instructions for vector operations
- Optimize memory access patterns for cache locality
- Implement loop unrolling for small, tight loops
- Use compiler optimizations (-O2, -O3, -march=native)
- Profile to identify bottlenecks before optimization

#### Parallel Processing
- Use appropriate threading models for different workloads
- Balance load across cores to avoid idle time
- Minimize synchronization overhead
- Consider NUMA effects for large-scale systems
- Use async computation for non-blocking operations

#### Monitoring and Analysis
- Continuously monitor performance metrics
- Profile different optimization levels
- Compare performance across different hardware
- Set up automated performance regression testing
- Establish performance baselines and targets

## Python Performance Optimization

### NumPy Integration
```python
import numpy as np
import time
from typing import Optional, Tuple

class OptimizedNPLEngine:
    """Optimized neural network engine using NumPy."""
    
    def __init__(self, config: Optional[dict] = None):
        self.config = config or {}
        self._setup_memory_pool()
        self._setup_simd_support()
    
    def _setup_memory_pool(self):
        """Setup memory pool for NumPy arrays."""
        # Pre-allocate frequently used arrays
        self._cache = {}
        self._max_cache_size = self.config.get('max_cache_size', 100)
    
    def _setup_simd_support(self):
        """Setup SIMD-optimized NumPy operations."""
        self._has_avx2 = hasattr(np, 'matmul')  # Basic check
        self._has_avx512 = hasattr(np, 'vectorize')
    
    def optimized_neural_update(self, v_membrane: np.ndarray, 
                               I_syn: np.ndarray,
                               weights: np.ndarray,
                               dt: float) -> np.ndarray:
        """
        Optimized neural membrane potential update.
        
        Uses NumPy's vectorized operations for maximum performance.
        """
        # Cache frequently used arrays
        cache_key = 'membrane_update'
        if cache_key not in self._cache:
            self._cache[cache_key] = np.ones_like(v_membrane) * 0.01
        
        # Vectorized computation (fully SIMD-optimized in NumPy)
        dv = (-(v_membrane - self._cache['v_rest']) + I_syn) / self._cache['tau_mem']
        v_membrane += dv * dt
        
        # Cache the result
        if len(self._cache) >= self._max_cache_size:
            # Remove oldest cache entry
            oldest_key = next(iter(self._cache))
            del self._cache[oldest_key]
        
        return v_membrane
    
    def benchmark_performance(self, iterations: int = 1000) -> dict:
        """
        Benchmark NumPy-based neural operations.
        
        Returns performance metrics and optimization suggestions.
        """
        start_time = time.perf_counter()
        
        # Setup test data
        neuron_count = self.config.get('neuron_count', 10000)
        v_membrane = np.random.uniform(-70, -50, neuron_count)
        I_syn = np.random.uniform(-10, 10, neuron_count)
        weights = np.random.uniform(0.01, 0.1, neuron_count)
        
        # Run benchmark
        for _ in range(iterations):
            self.optimized_neural_update(v_membrane, I_syn, weights, 0.1)
        
        end_time = time.perf_counter()
        
        execution_time = end_time - start_time
        throughput = iterations * neuron_count / execution_time  # operations per second
        
        return {
            'execution_time': execution_time,
            'throughput': throughput,
            'avg_time_per_iteration': execution_time / iterations,
            'neurons_per_second': throughput * neuron_count,
            'simd_optimized': self._has_avx2,
            'recommendations': self._get_optimization_recommendations()
        }
    
    def _get_optimization_recommendations(self) -> list:
        """Generate performance optimization recommendations."""
        recommendations = []
        
        # Check for memory issues
        if len(self._cache) >= self._max_cache_size * 0.9:
            recommendations.append(
                "Memory cache approaching capacity. Consider reducing cache size "
                "or implementing LRU eviction."
            )
        
        # Check NumPy version and features
        if not self._has_avx2:
            recommendations.append(
                "NumPy may not be utilizing AVX2 optimizations. "
                "Consider upgrading NumPy or checking CPU capabilities."
            )
        
        # Check array sizes
        for key, array in self._cache.items():
            if array.nbytes > 100 * 1024 * 1024:  # 100 MB
                recommendations.append(
                    f"Large cached array '{key}' ({array.nbytes / 1024 / 1024:.1f} MB). "
                    "Consider splitting or reducing frequency."
                )
        
        return recommendations
```

### Memory Efficiency
```python
class MemoryEfficientProcessor:
    """Memory-efficient neural processing."""
    
    def __init__(self):
        self._memory_usage = 0
        self._peak_memory = 0
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.cleanup()
    
    def process_in_chunks(self, data, chunk_size, processor_func):
        """
        Process large datasets in memory-efficient chunks.
        
        Args:
            data: Input data (numpy array or similar)
            chunk_size: Size of processing chunks
            processor_func: Function to apply to each chunk
        """
        for i in range(0, len(data), chunk_size):
            chunk = data[i:i + chunk_size]
            
            # Process chunk
            result = processor_func(chunk)
            
            # Yield result (lazy processing)
            yield result
            
            # Explicitly clean up chunk
            del chunk
            self._update_memory_usage()
    
    def reuse_arrays(self, arrays):
        """
        Reuse pre-allocated arrays to minimize allocations.
        
        Args:
            arrays: Dictionary of array names to pre-allocated arrays
        """
        return ArrayReuser(arrays)
    
    def cleanup(self):
        """Clean up all resources."""
        import gc
        gc.collect()
    
    def get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        return self._memory_usage / 1024 / 1024
    
    def get_peak_memory(self) -> float:
        """Get peak memory usage in MB."""
        return self._peak_memory / 1024 / 1024
    
    def _update_memory_usage(self):
        """Update memory usage statistics."""
        import psutil
        process = psutil.Process()
        current = process.memory_info().rss
        
        self._memory_usage = current
        if current > self._peak_memory:
            self._peak_memory = current

class ArrayReuser:
    """Helper class for array reuse."""
    
    def __init__(self, arrays: dict):
        self.arrays = arrays
        self._current_index = 0
    
    def get_array(self, name: str) -> np.ndarray:
        """Get an array for reuse."""
        if name not in self.arrays:
            raise ValueError(f"Array '{name}' not found in pool")
        
        return self.arrays[name]
    
    def return_array(self, name: str):
        """Return an array to the pool."""
        # Implementation depends on specific reuse strategy
        pass
```

## License

See the LICENSE file in the root directory for full license information.

## Version

- Performance Guide Version: 1.0.0
- Last Updated: 2026-09-27

## Contact

For performance-related questions, optimization issues, or benchmarking requests, please refer to the NLM documentation or performance team contact information.