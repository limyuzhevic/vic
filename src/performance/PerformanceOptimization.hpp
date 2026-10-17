#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace nlm {

// Performance optimizer class
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    // Memory optimization
    void enableMemoryPooling(bool enabled);
    void setMemoryPoolSize(size_t size);
    void optimizeMemoryUsage();
    size_t getEstimatedMemoryUsage() const;
    
    // CPU optimization
    void enableSIMDProcessing(bool enabled);
    void setThreadCount(size_t count);
    void optimizeCPUUsage();
    double getEstimatedCPUUsage() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enabled);
    void setPerformanceSampleRate(double rate);
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Optimization profiles
    enum class OptimizationProfile {
        Balanced,
        Performance,
        Memory,
        Power,
        Custom
    };
    
    void setOptimizationProfile(OptimizationProfile profile);
    OptimizationProfile getOptimizationProfile() const;
    
    // Profile management
    void saveOptimizationProfile(const std::string& name);
    bool loadOptimizationProfile(const std::string& name);
    std::vector<std::string> getAvailableProfiles() const;
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    double calculateOptimizationScore() const;
    
    // Real-time optimization
    void update(float deltaTime);
    void applyOptimizations();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Memory pool allocator
class MemoryPoolAllocator {
public:
    MemoryPoolAllocator();
    ~MemoryPoolAllocator();
    
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    bool isInitialized() const;
    size_t getTotalMemoryUsed() const;
    size_t getPeakMemoryUsage() const;
    
    // Pool management
    void clear();
    void shrink();
    void grow(size_t additionalSize);
    
    // Type-specific pools
    template<typename T>
    T* allocateArray(size_t count);
    
    template<typename T>
    void deallocateArray(T* ptr, size_t count);
    
    // Memory statistics
    struct MemoryStats {
        size_t totalAllocated;
        size_t peakAllocated;
        size_t currentAllocated;
        size_t allocationCount;
        size_t deallocationCount;
        double averageAllocationSize;
    };
    
    MemoryStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// SIMD processor
class SIMDProcessor {
public:
    SIMDProcessor();
    ~SIMDProcessor();
    
    bool isAvailable() const;
    std::string getCapabilities() const;
    
    // SIMD operations
    void vectorMultiply(const float* a, const float* b, float* result, size_t size);
    void vectorAdd(const float* a, const float* b, float* result, size_t size);
    void vectorDotProduct(const float* a, const float* b, float* result, size_t size);
    void vectorNormalize(float* vector, size_t size);
    
    // Neural network operations
    void convolution2D(const float* input, const float* kernel, float* output,
                      int inputWidth, int inputHeight, int kernelWidth, int kernelHeight,
                      int stride, int padding);
    
    void matrixMultiply(const float* a, const float* b, float* result,
                       size_t aRows, size_t aCols, size_t bCols);
    
    // Memory operations
    void copyMemory(const void* src, void* dst, size_t size);
    void fillMemory(void* dst, float value, size_t size);
    
    // Performance optimization
    void optimizeMemoryAccess(const void* data, size_t size);
    void prefetchMemory(const void* addr);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Thread pool
class ThreadPool {
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    
    template<typename F, typename... Args>
    std::future<decltype(std::invoke(std::declval<F>(), std::declval<Args>()...))>
    enqueue(F&& f, Args&&... args);
    
    void waitForAll();
    void waitForTask(std::future<void>& future);
    size_t getThreadCount() const;
    size_t getActiveThreadCount() const;
    
    // Task management
    void submitTask(std::function<void()> task);
    void removeTask(size_t taskId);
    bool isTaskRunning(size_t taskId) const;
    
    // Load balancing
    void balanceLoad();
    double getLoadFactor() const;
    
    // Performance monitoring
    struct ThreadStats {
        size_t tasksCompleted;
        double totalExecutionTime;
        double averageExecutionTime;
        double loadFactor;
    };
    
    std::vector<ThreadStats> getThreadStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// CPU cache optimizer
class CPUCacheOptimizer {
public:
    CPUCacheOptimizer();
    ~CPUCacheOptimizer();
    
    void optimizeDataLayout(const void* data, size_t size, size_t elementSize);
    void optimizeAccessPattern(const void* base, size_t stride, size_t count);
    void prefetchCacheLine(const void* addr);
    
    // Memory access patterns
    void optimizeSequentialAccess(const void* data, size_t size);
    void optimizeRandomAccess(const void* data, size_t size);
    void optimizeStridedAccess(const void* data, size_t stride, size_t count);
    
    // Cache statistics
    struct CacheStats {
        size_t cacheHits;
        size_t cacheMisses;
        double hitRate;
        double missRate;
    };
    
    CacheStats getCacheStats() const;
    
    // Configuration
    void setCacheLineSize(size_t size);
    void setAssociativity(size_t associativity);
    void setCacheSize(size_t size);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Power optimization
class PowerOptimizer {
public:
    PowerOptimizer();
    ~PowerOptimizer();
    
    void enablePowerSaving(bool enabled);
    void setCPUFrequencyLimit(double frequency);
    void setCorePowerLimit(double limit);
    void setMemoryPowerLimit(double limit);
    
    // Power management
    void throttleCPU();
    void enableTurboBoost(bool enabled);
    void optimizeForPowerConsumption();
    
    // Performance power trade-off
    double optimizePerformancePowerTradeoff(double performanceTarget);
    void balancePerformanceAndPower(double performanceWeight, double powerWeight);
    
    // Power statistics
    struct PowerStats {
        double cpuPowerUsage;
        double corePowerUsage;
        double memoryPowerUsage;
        double totalPowerUsage;
        double performanceScore;
    };
    
    PowerStats getPowerStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startProfiling(const std::string& profileName);
    void stopProfiling(const std::string& profileName);
    bool isProfiling(const std::string& profileName) const;
    
    void setProfileSampleRate(double rate);
    void enableRealTimeProfiling(bool enabled);
    
    // Profile data
    struct ProfileData {
        std::string name;
        double totalTime;
        double cpuTime;
        double memoryTime;
        size_t callCount;
        double averageTime;
        double minTime;
        double maxTime;
    };
    
    std::vector<ProfileData> getProfileData(const std::string& profileName) const;
    std::vector<ProfileData> getAllProfileData() const;
    
    // Performance analysis
    void analyzePerformanceBottlenecks();
    void identifyPerformanceHotspots();
    std::vector<std::string> getPerformanceRecommendations() const;
    
    // Profile export
    void exportProfileData(const std::string& filepath);
    void importProfileData(const std::string& filepath);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Performance metrics
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    double diskUsage;
    double networkUsage;
    double powerUsage;
    double temperature;
    double executionTime;
    size_t memoryAllocated;
    size_t cacheHits;
    size_t cacheMisses;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), diskUsage(0.0),
                          networkUsage(0.0), powerUsage(0.0), temperature(0.0),
                          executionTime(0.0), memoryAllocated(0), cacheHits(0), 
                          cacheMisses(0) {}
};

// Performance benchmarks
class PerformanceBenchmarks {
public:
    PerformanceBenchmarks();
    ~PerformanceBenchmarks();
    
    // Neural network benchmarks
    double benchmarkMatrixMultiplication(size_t size);
    double benchmarkConvolution2D(int width, int height, int kernelSize);
    double benchmarkNeuraLNetworkSimulation(size_t neurons, size_t connections);
    
    // Memory benchmarks
    double benchmarkMemoryAllocation(size_t count, size_t size);
    double benchmarkMemoryCopy(size_t size);
    double benchmarkMemoryBandwidth(size_t size);
    
    // CPU benchmarks
    double benchmarkFloatingPointOperations(size_t count);
    double benchmarkVectorOperations(size_t size);
    double benchmarkBranchPrediction(size_t count);
    
    // System benchmarks
    double benchmarkSystemPerformance();
    double benchmark scalability(size_t threadCount);
    double benchmarkMemoryUsage(size_t memoryPressure);
    
    // Performance regression testing
    bool isPerformanceRegression(const std::string& benchmarkName, double currentTime);
    void trackPerformanceRegression(const std::string& benchmarkName, double time);
    std::vector<std::string> getPerformanceRegressions() const;
    
    // Benchmark configuration
    void setBenchmarkIterations(size_t iterations);
    void enableBenchmarkCaching(bool enabled);
    void setBenchmarkTimeout(double timeout);
    
    // Benchmark export
    void exportBenchmarks(const std::string& filepath);
    void importBenchmarks(const std::string& filepath);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Performance monitor
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    void setMonitoringInterval(double interval);
    void setThresholds(const PerformanceMetrics& thresholds);
    PerformanceMetrics getCurrentMetrics() const;
    
    // Alert system
    void addPerformanceAlert(const std::string& name, std::function<bool(const PerformanceMetrics&)> condition);
    void removePerformanceAlert(const std::string& name);
    std::vector<std::string> getActiveAlerts() const;
    
    // Performance trends
    std::vector<PerformanceMetrics> getPerformanceHistory() const;
    double calculatePerformanceTrend() const;
    double calculatePerformanceVolatility() const;
    
    // Performance optimization
    void optimizeForCurrentWorkload();
    void adjustPerformanceParameters();
    void applyPerformanceFixes();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm