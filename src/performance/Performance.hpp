#ifndef NLM_PERFORMANCE_HPP
#define NLM_PERFORMANCE_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <omp.h>
#include <immintrin.h>
#include <numa.h>
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Performance configuration
struct PerformanceConfig {
    // Memory optimization
    bool enableMemoryPools;
    bool enableNUMAPinning;
    bool enableMemoryAccessOptimization;
    bool enableMemoryPrefetching;
    
    // Computation optimization
    bool enableSIMD;
    bool enableVectorization;
    bool enableOMP;
    bool enableOpenMPParallelization;
    int ompNumThreads;
    
    // Cache optimization
    bool enableCacheAffinity;
    bool enableLoopUnrolling;
    bool enableInstructionCache;
    
    // Event system optimization
    bool enableEventPooling;
    bool enableEventCompression;
    bool enableAsyncEventProcessing;
    
    // Performance monitoring
    bool enableProfiling;
    bool enableSampling;
    double profilingSampleRate;
    
    // Resource management
    size_t maxMemoryUsage;
    size_t maxNeuronMemory;
    size_t maxSynapseMemory;
    size_t maxEventMemory;
    
    PerformanceConfig()
        : enableMemoryPools(true)
        , enableNUMAPinning(false)
        , enableMemoryAccessOptimization(true)
        , enableMemoryPrefetching(true)
        , enableSIMD(true)
        , enableVectorization(true)
        , enableOMP(true)
        , enableOpenMPParallelization(true)
        , ompNumThreads(std::thread::hardware_concurrency())
        , enableCacheAffinity(true)
        , enableLoopUnrolling(true)
        , enableInstructionCache(true)
        , enableEventPooling(true)
        , enableEventCompression(true)
        , enableAsyncEventProcessing(false)
        , enableProfiling(true)
        , enableSampling(false)
        , profilingSampleRate(0.1)
        , maxMemoryUsage(1024 * 1024 * 1024)  // 1GB default
        , maxNeuronMemory(256 * 1024 * 1024)  // 256MB
        , maxSynapseMemory(128 * 1024 * 1024)  // 128MB
        , maxEventMemory(64 * 1024 * 1024)     // 64MB
    {}
};

// Memory pool allocator for neurons
class NeuronMemoryPool {
public:
    NeuronMemoryPool(size_t capacity);
    ~NeuronMemoryPool();
    
    bool initialize();
    void shutdown();
    
    // Allocate neurons
    NeuronId allocateNeuron(size_t count = 1);
    bool deallocateNeuron(NeuronId id);
    bool deallocateAll();
    
    // Memory statistics
    size_t getAllocatedCount() const;
    size_t getCapacity() const;
    size_t getUsedMemory() const;
    
    // Memory management
    void optimizeMemoryLayout();
    void compactMemory();
    
    // NUMA awareness
    void setNUMANode(int node);
    int getNUMANode() const;
    
    // Memory access optimization
    void enablePrefetching(bool enable);
    void setPrefetchDistance(size_t distance);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory pool allocator for synapses
class SynapseMemoryPool {
public:
    SynapseMemoryPool(size_t capacity);
    ~SynapseMemoryPool();
    
    bool initialize();
    void shutdown();
    
    // Allocate synapses
    SynapseId allocateSynapse(size_t count = 1);
    bool deallocateSynapse(SynapseId id);
    bool deallocateAll();
    
    // Memory statistics
    size_t getAllocatedCount() const;
    size_t getCapacity() const;
    size_t getUsedMemory() const;
    
    // Memory management
    void optimizeMemoryLayout();
    void compactMemory();
    
    // Connection management
    void markConnectionDirty(SynapseId id);
    void flushConnections();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Event pool for spike events
class EventPool {
public:
    EventPool(size_t capacity);
    ~EventPool();
    
    bool initialize();
    void shutdown();
    
    // Allocate events
    SpikeEvent* allocateEvent();
    DelayedSpikeEvent* allocateDelayedEvent();
    bool deallocateEvent(SpikeEvent* event);
    bool deallocateDelayedEvent(DelayedSpikeEvent* event);
    
    // Event management
    void reset();
    void compressEvents();
    
    // Statistics
    size_t getAllocatedEvents() const;
    size_t getCapacity() const;
    double getCompressionRatio() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// SIMD-optimized neuron computations
class SIMDNeuronComputations {
public:
    SIMDNeuronComputations();
    ~SIMDNeuronComputations();
    
    // Vectorized LIF computation
    static void computeLIF(const float* membranePotentials, const float* restingPotentials,
                          const float* thresholds, const float* inputs, float* outputs,
                          size_t count, float timestep, float leakConductance);
    
    // Vectorized spike detection
    static void detectSpikes(const float* membranePotentials, const float* thresholds,
                           bool* spikes, size_t count);
    
    // Vectorized current injection
    static void injectCurrent(float* membranePotentials, const float* currents,
                             size_t count);
    
    // Vectorized synaptic weight updates
    static void updateWeights(float* weights, const float* preTrace, const float* postTrace,
                            const float* learningRate, size_t count, float timestep);
    
    // AVX-512 optimizations
    static void computeLIF_AVX512(const float* membranePotentials, const float* restingPotentials,
                                 const float* thresholds, const float* inputs, float* outputs,
                                 size_t count, float timestep, float leakConductance);
    
    // SSE optimizations
    static void computeLIF_SSE(const float* membranePotentials, const float* restingPotentials,
                              const float* thresholds, const float* inputs, float* outputs,
                              size_t count, float timestep, float leakConductance);
    
    // Memory optimization
    static void optimizeMemoryAccess(const float* data, size_t size, int cacheLineSize = 64);
    
    // Prefetching
    static void prefetchData(const float* data, size_t size);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Cache-aware memory allocator
class CacheAwareAllocator {
public:
    CacheAwareAllocator();
    ~CacheAwareAllocator();
    
    bool initialize();
    void shutdown();
    
    // Allocate with cache optimization
    void* allocate(size_t size, int alignment = 64);
    void deallocate(void* ptr);
    
    // Memory access patterns
    void setAccessPattern(int pattern);
    int getAccessPattern() const;
    
    // Performance monitoring
    double getCacheMissRate() const;
    double getMemoryBandwidth() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// OpenMP parallelization utilities
class ParallelProcessor {
public:
    ParallelProcessor();
    ~ParallelProcessor();
    
    bool initialize();
    void shutdown();
    
    // Parallel for loops
    template<typename Func>
    void parallelFor(size_t start, size_t end, const Func& func);
    
    template<typename Func>
    void parallelFor(size_t count, const Func& func);
    
    template<typename InputIt, typename OutputIt, typename Func>
    void parallelTransform(InputIt first, InputIt last, OutputIt d_first, const Func& op);
    
    template<typename Container, typename Func>
    void parallelForEach(const Container& container, const Func& func);
    
    // Task-based parallelism
    void submitTask(const std::function<void()>& task);
    void waitForTasks();
    
    // Nested parallelism
    void enableNestedParallelism(bool enable);
    bool isNestedParallelismEnabled() const;
    
    // Load balancing
    void setLoadBalancing(const std::string& policy);
    std::string getLoadBalancing() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    bool initialize();
    void shutdown();
    
    // Profile a function
    template<typename Func, typename... Args>
    auto profile(const std::string& name, const Func& func, Args&&... args);
    
    template<typename Func>
    auto profile(const std::string& name, const Func& func);
    
    // Profile a code block
    class ProfilerScope {
    public:
        ProfilerScope(PerformanceProfiler& profiler, const std::string& name);
        ~ProfilerScope();
        
        void stop();
        double getDuration() const;
        
    private:
        PerformanceProfiler& profiler;
        std::string name;
        std::chrono::steady_clock::time_point startTime;
    };
    
    ProfilerScope createScope(const std::string& name);
    
    // Get statistics
    std::map<std::string, double> getStatistics() const;
    std::vector<std::string> getTopFunctions(size_t limit) const;
    
    // Sampling
    void setSamplingRate(double rate);
    double getSamplingRate() const;
    
    // Export
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main performance manager
class PerformanceManager {
public:
    PerformanceManager();
    ~PerformanceManager();
    
    bool initialize(std::shared_ptr<Brain> brain, const PerformanceConfig& config = PerformanceConfig());
    void shutdown();
    
    // Performance configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Performance monitoring
    double getFPS() const;
    double getAverageStepTime() const;
    size_t getMemoryUsage() const;
    size_t getCacheMisses() const;
    
    // Resource management
    void setMemoryLimit(size_t limit);
    void setTimeStepLimit(double limit);
    
    // Performance analysis
    std::map<std::string, double> analyzePerformance() const;
    std::vector<std::string> getBottlenecks() const;
    
    // System integration
    void updatePerformanceData();
    void optimizeForCurrentWorkload();
    
    // Export
    bool exportPerformanceReport(const std::string& filepath) const;
    
    // Global instance
    static PerformanceManager& getInstance();
    static bool initializeGlobal(std::shared_ptr<Brain> brain, const PerformanceConfig& config = PerformanceConfig());
    static void shutdownGlobal();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory allocator utilities
namespace MemoryUtils {
    // Aligned allocation
    void* alignedAlloc(size_t alignment, size_t size);
    void alignedFree(void* ptr);
    
    // NUMA-aware allocation
    void* numaAlloc(size_t size, int node = -1);
    void numaFree(void* ptr, size_t size);
    
    // Memory copying with optimization
    void optimizedCopy(const void* src, void* dst, size_t size);
    
    // Memory prefetching
    void prefetch(const void* ptr, size_t size, int hint = 3);
    
    // Cache optimization
    void optimizeCacheUsage(const void* data, size_t size);
    
    // Memory statistics
    struct MemoryStats {
        size_t totalMemory;
        size_t usedMemory;
        size_t freeMemory;
        double usagePercentage;
        size_t allocationCount;
        size_t deallocationCount;
    };
    
    MemoryStats getMemoryStats();
    
    // Thread-local storage
    template<typename T>
    T& getTLS() {
        static thread_local T instance;
        return instance;
    }
}

// CPU feature detection
namespace CPUFeatures {
    bool supportsAVX512();
    bool supportsAVX2();
    bool supportsSSE4_2();
    bool supportsSSE4_1();
    bool supportsSSE3();
    
    std::string getCPUName();
    size_t getCacheLineSize();
    size_t getL1CacheSize();
    size_t getL2CacheSize();
    size_t getL3CacheSize();
    
    int getCoreCount();
    int getThreadCount();
    
    double getClockSpeed();
    
    // Performance optimization
    void optimizeForCPU();
    void setCPUAffinity(const std::vector<int>& cores);
    void setThreadAffinity(std::thread& thread, const std::vector<int>& cores);
}

// Event system optimization
namespace EventUtils {
    // Event pooling
    class EventPoolManager {
    public:
        static EventPoolManager& getInstance();
        
        bool initialize(size_t poolSize);
        void shutdown();
        
        SpikeEvent* acquireSpikeEvent();
        DelayedSpikeEvent* acquireDelayedEvent();
        
        void releaseSpikeEvent(SpikeEvent* event);
        void releaseDelayedEvent(DelayedSpikeEvent* event);
        
        size_t getAvailableEvents() const;
        
    private:
        EventPoolManager();
        ~EventPoolManager();
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Event compression
    class EventCompressor {
    public:
        static EventCompressor& getInstance();
        
        void compressEvents(std::vector<SpikeEvent>& events);
        void decompressEvents(std::vector<SpikeEvent>& events);
        
        double getCompressionRatio() const;
        
    private:
        EventCompressor();
        ~EventCompressor();
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

// Main performance optimization interface
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    // Optimize brain for performance
    bool optimizeBrain(std::shared_ptr<Brain> brain);
    bool optimizeMemoryUsage(std::shared_ptr<Brain> brain);
    bool optimizeComputation(std::shared_ptr<Brain> brain);
    bool optimizeEvents(std::shared_ptr<Brain> brain);
    
    // Performance analysis
    std::map<std::string, double> benchmarkBrain(std::shared_ptr<Brain> brain);
    std::vector<std::string> getOptimizationRecommendations(std::shared_ptr<Brain> brain) const;
    
    // Real-time monitoring
    void startMonitoring(std::shared_ptr<Brain> brain);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Performance tuning
    void tuneForLoad(double cpuUsage, size_t memoryUsage);
    void tuneForLatency(double targetLatency);
    void tuneForThroughput(double targetThroughput);
    
    // Profiling
    void startProfiling(std::shared_ptr<Brain> brain, const std::string& outputFile = "");
    void stopProfiling();
    bool isProfiling() const;
    
    // Configuration
    void setPerformanceConfig(const PerformanceConfig& config);
    PerformanceConfig getPerformanceConfig() const;
    
    // Global instance
    static PerformanceOptimizer& getInstance();
    static bool initializeGlobal(const PerformanceConfig& config = PerformanceConfig());
    static void shutdownGlobal();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP
