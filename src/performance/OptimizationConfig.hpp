// Performance configuration for advanced optimization
#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <atomic>
#include <vector>
#include <mutex>
#include <algorithm>

namespace nlm {

class PerformanceMonitor;

class OptimizationConfig {
public:
    enum class OptimizationLevel {
        None,
        Basic,
        Standard,
        High,
        Maximum
    };
    
    enum class SIMDFeatures {
        SSE,
        AVX,
        AVX512,
        AutoDetect
    };
    
    enum class ParallelStrategy {
        None,
        ThreadPool,
        TaskGraph,
        AsyncStreams
    };
    
    enum class MemoryStrategy {
        Default,
        PoolBased,
        ThreadLocal,
        NUMAAware
    };
    
    enum class CacheStrategy {
        Default,
        Aggressive,
        Conservative,
        Adaptive
    };
    
    struct Configuration {
        OptimizationLevel optimizationLevel;
        SIMDFeatures simdFeatures;
        ParallelStrategy parallelStrategy;
        MemoryStrategy memoryStrategy;
        CacheStrategy cacheStrategy;
        
        // Performance parameters
        size_t threadCount;
        size_t maxQueueSize;
        size_t memoryPoolSize;
        size_t cacheLineSize;
        double cacheHitTarget;
        double targetUtilization;
        
        // SIMD configuration
        bool enableSIMD;
        bool enableVectorization;
        bool enablePrefetching;
        
        // Parallel configuration
        bool enableWorkStealing;
        bool enableDynamicScheduling;
        bool enableTaskAffinity;
        
        // Memory configuration
        bool enableMemoryPools;
        bool enableNUMAPooling;
        size_t memoryAlignment;
        size_t maxMemoryFragmentation;
        
        // Cache configuration
        bool enableCacheOptimization;
        bool enableDataPrefetching;
        size_t prefetchDistance;
        bool enableCacheCoherency;
        
        // Batch processing
        bool enableBatchProcessing;
        size_t batchSize;
        size_t maxBatchQueueSize;
        
        // Profiling
        bool enableProfiling;
        bool enableDetailedProfiling;
        size_t profilingSampleRate;
        std::chrono::milliseconds profilingInterval;
        
        // Validation
        bool enableValidation;
        bool validateOnSet;
        bool validateOnGet;
        double validationTolerance;
        
        // Monitoring
        bool enableMonitoring;
        bool enableRealTimeMonitoring;
        size_t monitoringSampleRate;
        std::string monitoringEndpoint;
        
        Configuration() : optimizationLevel(OptimizationLevel::Standard),
                         simdFeatures(SIMDFeatures::AutoDetect),
                         parallelStrategy(ParallelStrategy::ThreadPool),
                         memoryStrategy(MemoryStrategy::ThreadLocal),
                         cacheStrategy(CacheStrategy::Adaptive),
                         threadCount(std::thread::hardware_concurrency()),
                         maxQueueSize(10000),
                         memoryPoolSize(65536),
                         cacheLineSize(64),
                         cacheHitTarget(0.95),
                         targetUtilization(0.85),
                         enableSIMD(true),
                         enableVectorization(true),
                         enablePrefetching(true),
                         enableWorkStealing(true),
                         enableDynamicScheduling(true),
                         enableTaskAffinity(false),
                         enableMemoryPools(true),
                         enableNUMAPooling(false),
                         memoryAlignment(64),
                         maxMemoryFragmentation(10),
                         enableCacheOptimization(true),
                         enableDataPrefetching(true),
                         prefetchDistance(4),
                         enableCacheCoherency(true),
                         enableBatchProcessing(true),
                         batchSize(1024),
                         maxBatchQueueSize(5000),
                         enableProfiling(true),
                         enableDetailedProfiling(false),
                         profilingSampleRate(1000),
                         profilingInterval(1000ms),
                         enableValidation(true),
                         validateOnSet(true),
                         validateOnGet(false),
                         validationTolerance(0.001),
                         enableMonitoring(true),
                         enableRealTimeMonitoring(true),
                         monitoringSampleRate(100) {}
    };
    
    OptimizationConfig() : currentLevel(OptimizationLevel::Standard), 
                          monitor(nullptr), dirty(false) {
        loadDefaults();
    }
    
    explicit OptimizationConfig(OptimizationLevel level) : 
        currentLevel(level), monitor(nullptr), dirty(false) {
        loadDefaults();
        applyLevel(level);
    }
    
    void setOptimizationLevel(OptimizationLevel level) {
        currentLevel = level;
        applyLevel(level);
        dirty = true;
    }
    
    OptimizationLevel getOptimizationLevel() const {
        return currentLevel;
    }
    
    void setSIMDEnabled(bool enable) {
        config.enableSIMD = enable;
        dirty = true;
    }
    
    bool isSIMDEnabled() const {
        return config.enableSIMD;
    }
    
    void setSIMDFeatures(SIMDFeatures features) {
        config.simdFeatures = features;
        dirty = true;
    }
    
    SIMDFeatures getSIMDFeatures() const {
        return config.simdFeatures;
    }
    
    void setParallelStrategy(ParallelStrategy strategy) {
        config.parallelStrategy = strategy;
        dirty = true;
    }
    
    ParallelStrategy getParallelStrategy() const {
        return config.parallelStrategy;
    }
    
    void setThreadCount(size_t count) {
        config.threadCount = count;
        dirty = true;
    }
    
    size_t getThreadCount() const {
        return config.threadCount;
    }
    
    void setMemoryStrategy(MemoryStrategy strategy) {
        config.memoryStrategy = strategy;
        dirty = true;
    }
    
    MemoryStrategy getMemoryStrategy() const {
        return config.memoryStrategy;
    }
    
    void setCacheStrategy(CacheStrategy strategy) {
        config.cacheStrategy = strategy;
        dirty = true;
    }
    
    CacheStrategy getCacheStrategy() const {
        return config.cacheStrategy;
    }
    
    void setBatchProcessingEnabled(bool enable) {
        config.enableBatchProcessing = enable;
        dirty = true;
    }
    
    bool isBatchProcessingEnabled() const {
        return config.enableBatchProcessing;
    }
    
    void setBatchSize(size_t size) {
        config.batchSize = size;
        dirty = true;
    }
    
    size_t getBatchSize() const {
        return config.batchSize;
    }
    
    void setProfilingEnabled(bool enable) {
        config.enableProfiling = enable;
        dirty = true;
    }
    
    bool isProfilingEnabled() const {
        return config.enableProfiling;
    }
    
    void setMonitoringEnabled(bool enable) {
        config.enableMonitoring = enable;
        dirty = true;
    }
    
    bool isMonitoringEnabled() const {
        return config.enableMonitoring;
    }
    
    void setValidationEnabled(bool enable) {
        config.enableValidation = enable;
        dirty = true;
    }
    
    bool isValidationEnabled() const {
        return config.enableValidation;
    }
    
    const Configuration& getConfig() const {
        return config;
    }
    
    Configuration& getConfig() {
        return config;
    }
    
    bool isDirty() const {
        return dirty;
    }
    
    void clearDirty() {
        dirty = false;
    }
    
    void applyConfig(const Configuration& newConfig) {
        config = newConfig;
        dirty = false;
    }
    
    Configuration getRecommendedConfig(OptimizationLevel level) {
        Configuration recommended;
        applyLevel(level, recommended);
        return recommended;
    }
    
    std::string getConfigSummary() const {
        std::stringstream ss;
        ss << "Optimization Level: " << static_cast<int>(currentLevel) << std::endl;
        ss << "SIMD Enabled: " << (config.enableSIMD ? "Yes" : "No") << std::endl;
        ss << "SIMD Features: " << static_cast<int>(config.simdFeatures) << std::endl;
        ss << "Parallel Strategy: " << static_cast<int>(config.parallelStrategy) << std::endl;
        ss << "Thread Count: " << config.threadCount << std::endl;
        ss << "Memory Strategy: " << static_cast<int>(config.memoryStrategy) << std::endl;
        ss << "Cache Strategy: " << static_cast<int>(config.cacheStrategy) << std::endl;
        ss << "Batch Processing: " << (config.enableBatchProcessing ? "Yes" : "No") << std::endl;
        ss << "Profiling Enabled: " << (config.enableProfiling ? "Yes" : "No") << std::endl;
        ss << "Monitoring Enabled: " << (config.enableMonitoring ? "Yes" : "No") << std::endl;
        ss << "Validation Enabled: " << (config.enableValidation ? "Yes" : "No") << std::endl;
        
        return ss.str();
    }
    
    void exportConfig(std::ostream& out) const {
        // In a real implementation, this would serialize the config to JSON/YAML
        out << getConfigSummary();
    }
    
    void importConfig(std::istream& in) {
        // In a real implementation, this would deserialize the config from JSON/YAML
        // For now, we'll just read and parse a simple format
        std::string line;
        while (std::getline(in, line)) {
            if (line.find("Optimization Level:") != std::string::npos) {
                // Parse optimization level
            } else if (line.find("SIMD Enabled:") != std::string::npos) {
                // Parse SIMD enabled
            } // Add other parsing as needed
        }
    }
    
    void validate() const {
        if (!config.enableSIMD && !config.enableVectorization) {
            std::cout << "Warning: SIMD is disabled. Performance may be impacted." << std::endl;
        }
        
        if (config.threadCount == 0) {
            std::cout << "Warning: Thread count is 0. Performance may be impacted." << std::endl;
        }
        
        if (config.batchSize == 0) {
            std::cout << "Warning: Batch size is 0. Performance may be impacted." << std::endl;
        }
        
        if (config.cacheHitTarget < 0.0 || config.cacheHitTarget > 1.0) {
            std::cout << "Warning: Invalid cache hit target. Must be between 0 and 1." << std::endl;
        }
        
        if (config.targetUtilization < 0.0 || config.targetUtilization > 1.0) {
            std::cout << "Warning: Invalid target utilization. Must be between 0 and 1." << std::endl;
        }
        
        if (config.validationTolerance < 0.0) {
            std::cout << "Warning: Invalid validation tolerance. Must be non-negative." << std::endl;
        }
    }
    
    void setMonitor(PerformanceMonitor* monitor) {
        this->monitor = monitor;
    }
    
    void notifyChange() {
        if (monitor) {
            monitor->onOptimizationConfigChanged(*this);
        }
    }
    
private:
    void loadDefaults() {
        config = Configuration();
    }
    
    void applyLevel(OptimizationLevel level) {
        applyLevel(level, config);
    }
    
    void applyLevel(OptimizationLevel level, Configuration& config) {
        switch (level) {
            case OptimizationLevel::None:
                config.enableSIMD = false;
                config.enableVectorization = false;
                config.enablePrefetching = false;
                config.parallelStrategy = ParallelStrategy::None;
                config.threadCount = 1;
                config.enableMemoryPools = false;
                config.enableCacheOptimization = false;
                config.enableBatchProcessing = false;
                config.enableProfiling = false;
                config.enableMonitoring = false;
                config.enableValidation = false;
                break;
                
            case OptimizationLevel::Basic:
                config.enableSIMD = true;
                config.enableVectorization = true;
                config.enablePrefetching = false;
                config.parallelStrategy = ParallelStrategy::ThreadPool;
                config.threadCount = std::thread::hardware_concurrency();
                config.enableMemoryPools = true;
                config.enableCacheOptimization = true;
                config.enableBatchProcessing = true;
                config.enableProfiling = false;
                config.enableMonitoring = true;
                config.enableValidation = true;
                break;
                
            case OptimizationLevel::Standard:
                config.enableSIMD = true;
                config.enableVectorization = true;
                config.enablePrefetching = true;
                config.parallelStrategy = ParallelStrategy::ThreadPool;
                config.threadCount = std::thread::hardware_concurrency();
                config.enableMemoryPools = true;
                config.enableNUMAPooling = false;
                config.enableCacheOptimization = true;
                config.enableDataPrefetching = true;
                config.prefetchDistance = 2;
                config.enableBatchProcessing = true;
                config.batchSize = 512;
                config.enableProfiling = true;
                config.enableMonitoring = true;
                config.enableValidation = true;
                break;
                
            case OptimizationLevel::High:
                config.enableSIMD = true;
                config.enableVectorization = true;
                config.enablePrefetching = true;
                config.parallelStrategy = ParallelStrategy::TaskGraph;
                config.threadCount = std::thread::hardware_concurrency() * 2;
                config.enableMemoryPools = true;
                config.enableNUMAPooling = true;
                config.enableCacheOptimization = true;
                config.enableDataPrefetching = true;
                config.prefetchDistance = 4;
                config.enableBatchProcessing = true;
                config.batchSize = 1024;
                config.enableProfiling = true;
                config.enableMonitoring = true;
                config.enableDetailedProfiling = true;
                config.enableValidation = true;
                break;
                
            case OptimizationLevel::Maximum:
                config.enableSIMD = true;
                config.enableVectorization = true;
                config.enablePrefetching = true;
                config.parallelStrategy = ParallelStrategy::AsyncStreams;
                config.threadCount = std::thread::hardware_concurrency() * 4;
                config.enableMemoryPools = true;
                config.enableNUMAPooling = true;
                config.enableCacheOptimization = true;
                config.enableDataPrefetching = true;
                config.prefetchDistance = 8;
                config.enableBatchProcessing = true;
                config.batchSize = 2048;
                config.enableProfiling = true;
                config.enableMonitoring = true;
                config.enableDetailedProfiling = true;
                config.enableValidation = true;
                config.maxQueueSize = 50000;
                config.maxMemoryFragmentation = 5;
                break;
        }
    }
    
    OptimizationLevel currentLevel;
    Configuration config;
    PerformanceMonitor* monitor;
    bool dirty;
};

} // namespace nlm