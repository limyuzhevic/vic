#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include "../core/Config/Config.hpp"

namespace nlm {

class OptimizationHooks {
public:
    OptimizationHooks();
    ~OptimizationHooks();
    
    // Disable copying, enable moving
    OptimizationHooks(const OptimizationHooks&) = delete;
    OptimizationHooks& operator=(const OptimizationHooks&) = delete;
    OptimizationHooks(OptimizationHooks&&) noexcept;
    OptimizationHooks& operator=(OptimizationHooks&&) noexcept;
    
    // Initialization and lifecycle
    bool initialize(const Config& config);
    void update(double timestep);
    void shutdown();
    
    // SIMD optimization hooks
    bool enableSIMDOptimization(bool enable);
    bool isSIMDOptimizationEnabled() const;
    void setSIMDLevel(const std::string& level);
    std::string getSIMDLevel() const;
    
    // Parallel processing extensions
    bool enableParallelProcessing(bool enable);
    bool isParallelProcessingEnabled() const;
    void setParallelThreads(int threads);
    int getParallelThreads() const;
    bool setParallelBackend(const std::string& backend);
    std::string getParallelBackend() const;
    
    // Custom memory management integration
    bool enableCustomMemoryManager(bool enable);
    bool isCustomMemoryManagerEnabled() const;
    void setMemoryPoolSize(size_t poolSize);
    size_t getMemoryPoolSize() const;
    bool setMemoryOptimizationStrategy(const std::string& strategy);
    std::string getMemoryOptimizationStrategy() const;
    
    // Performance profiling integration
    bool enablePerformanceProfiling(bool enable);
    bool isPerformanceProfilingEnabled() const;
    void setProfilingLevel(const std::string& level);
    std::string getProfilingLevel() const;
    std::string getPerformanceProfile() const;
    
    // Custom optimization hooks
    bool addOptimizationHook(const std::string& hookName,
                           std::function<bool(const Config&)> preHook,
                           std::function<bool(const Config&)> postHook);
    bool removeOptimizationHook(const std::string& hookName);
    std::vector<std::string> getOptimizationHooks() const;
    
    // Optimization statistics
    size_t getOptimizationCount() const;
    double getOptimizationSuccessRate() const;
    std::string getOptimizationStats() const;
    
    // Custom code generation
    bool enableCustomCodeGeneration(bool enable);
    bool isCustomCodeGenerationEnabled() const;
    std::string generateOptimizedCode(const std::string& inputCode) const;
    bool compileCustomCode(const std::string& codePath) const;
    
    // Runtime optimization
    bool enableRuntimeOptimization(bool enable);
    bool isRuntimeOptimizationEnabled() const;
    void setOptimizationThreshold(double threshold);
    double getOptimizationThreshold() const;
    bool shouldOptimize(const Config& config) const;
    
    // Profiling data collection
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double executionTime;
        size_t memoryAllocated;
        size_t cacheHits;
        size_t cacheMisses;
        std::chrono::system_clock::time_point timestamp;
        
        PerformanceMetrics()
            : cpuUsage(0.0), memoryUsage(0.0), executionTime(0.0), memoryAllocated(0),
              cacheHits(0), cacheMisses(0), timestamp(std::chrono::system_clock::now()) {}
    };
    
    void recordPerformanceMetrics(const PerformanceMetrics& metrics);
    PerformanceMetrics getLastPerformanceMetrics() const;
    std::vector<PerformanceMetrics> getPerformanceHistory() const;
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    std::map<std::string, double> getPerformanceBottlenecks() const;
    
    // Export/import optimization settings
    bool exportOptimizationSettings(const std::string& filepath) const;
    bool importOptimizationSettings(const std::string& filepath);
    std::string exportOptimizationSettingsToString() const;
    bool importOptimizationSettingsFromString(const std::string& settingsString);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Custom parallelism interface
class CustomParallelism {
public:
    virtual ~CustomParallelism() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    
    // Parallel processing methods
    virtual bool parallelFor(size_t start, size_t end,
                           std::function<void(size_t)> task,
                           size_t grainSize = 0) = 0;
    virtual bool parallelMap(const std::vector<size_t>& inputs,
                           std::function<size_t(size_t)> task,
                           std::vector<size_t>& outputs) = 0;
    virtual bool parallelReduce(const std::vector<size_t>& inputs,
                              std::function<size_t(size_t, size_t)> reduceOp,
                              size_t& result) = 0;
    
    // Thread pool management
    virtual size_t getThreadCount() const = 0;
    virtual bool setThreadCount(size_t count) = 0;
    virtual size_t getAvailableCores() const = 0;
    
    // Load balancing
    virtual bool enableLoadBalancing(bool enable) = 0;
    virtual bool isLoadBalancingEnabled() const = 0;
    virtual std::string getLoadBalancingStrategy() const = 0;
};

// Custom memory manager interface
class CustomMemoryManager {
public:
    virtual ~CustomMemoryManager() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    
    // Memory allocation/deallocation
    virtual void* allocate(size_t size, size_t alignment = 0) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual void* allocateAligned(size_t size, size_t alignment) = 0;
    
    // Memory pool management
    virtual size_t getPoolSize() const = 0;
    virtual size_t getUsedPoolSize() const = 0;
    virtual size_t getAvailablePoolSize() const = 0;
    virtual bool preallocate(size_t size) = 0;
    virtual bool deallocateAll() = 0;
    
    // Memory optimization
    virtual bool enableMemoryOptimization(bool enable) = 0;
    virtual bool isMemoryOptimizationEnabled() const = 0;
    virtual bool compact() = 0;
    virtual bool setMemoryPoolStrategy(const std::string& strategy) = 0;
    virtual std::string getMemoryPoolStrategy() const = 0;
    
    // Profiling and statistics
    virtual size_t getTotalAllocations() const = 0;
    virtual size_t getTotalDeallocations() const = 0;
    virtual double getMemoryEfficiency() const = 0;
    virtual std::string getMemoryStats() const = 0;
};

// Built-in parallelism implementations
class OpenMPParallelism : public CustomParallelism {
public:
    OpenMPParallelism() = default;
    ~OpenMPParallelism() override = default;
    
    std::string getName() const override { return "OpenMP"; }
    std::string getVersion() const override { return "4.5"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool parallelFor(size_t start, size_t end, std::function<void(size_t)> task, size_t grainSize) override;
    bool parallelMap(const std::vector<size_t>& inputs, std::function<size_t(size_t)> task, std::vector<size_t>& outputs) override;
    bool parallelReduce(const std::vector<size_t>& inputs, std::function<size_t(size_t, size_t)> reduceOp, size_t& result) override;
    
    size_t getThreadCount() const override;
    bool setThreadCount(size_t count) override;
    size_t getAvailableCores() const override;
    bool enableLoadBalancing(bool enable) override;
    bool isLoadBalancingEnabled() const override;
    std::string getLoadBalancingStrategy() const override;
};

class ThreadPoolParallelism : public CustomParallelism {
public:
    ThreadPoolParallelism() = default;
    ~ThreadPoolParallelism() override = default;
    
    std::string getName() const override { return "ThreadPool"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool parallelFor(size_t start, size_t end, std::function<void(size_t)> task, size_t grainSize) override;
    bool parallelMap(const std::vector<size_t>& inputs, std::function<size_t(size_t)> task, std::vector<size_t>& outputs) override;
    bool parallelReduce(const std::vector<size_t>& inputs, std::function<size_t(size_t, size_t)> reduceOp, size_t& result) override;
    
    size_t getThreadCount() const override;
    bool setThreadCount(size_t count) override;
    size_t getAvailableCores() const override;
    bool enableLoadBalancing(bool enable) override;
    bool isLoadBalancingEnabled() const override;
    std::string getLoadBalancingStrategy() const override;
};

class TBBParallelism : public CustomParallelism {
public:
    TBBParallelism() = default;
    ~TBBParallelism() override = default;
    
    std::string getName() const override { return "TBB"; }
    std::string getVersion() const override { return "2021.4"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    bool parallelFor(size_t start, size_t end, std::function<void(size_t)> task, size_t grainSize) override;
    bool parallelMap(const std::vector<size_t>& inputs, std::function<size_t(size_t)> task, std::vector<size_t>& outputs) override;
    bool parallelReduce(const std::vector<size_t>& inputs, std::function<size_t(size_t, size_t)> reduceOp, size_t& result) override;
    
    size_t getThreadCount() const override;
    bool setThreadCount(size_t count) override;
    size_t getAvailableCores() const override;
    bool enableLoadBalancing(bool enable) override;
    bool isLoadBalancingEnabled() const override;
    std::string getLoadBalancingStrategy() const override;
};

// Built-in memory manager implementations
class PoolMemoryManager : public CustomMemoryManager {
public:
    PoolMemoryManager() = default;
    ~PoolMemoryManager() override = default;
    
    std::string getName() const override { return "PoolManager"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    void* allocate(size_t size, size_t alignment) override;
    void deallocate(void* ptr) override;
    void* allocateAligned(size_t size, size_t alignment) override;
    
    size_t getPoolSize() const override;
    size_t getUsedPoolSize() const override;
    size_t getAvailablePoolSize() const override;
    bool preallocate(size_t size) override;
    bool deallocateAll() override;
    
    bool enableMemoryOptimization(bool enable) override;
    bool isMemoryOptimizationEnabled() const override;
    bool compact() override;
    bool setMemoryPoolStrategy(const std::string& strategy) override;
    std::string getMemoryPoolStrategy() const override;
    
    size_t getTotalAllocations() const override;
    size_t getTotalDeallocations() const override;
    double getMemoryEfficiency() const override;
    std::string getMemoryStats() const override;
    
private:
    struct MemoryBlock {
        void* ptr;
        size_t size;
        size_t alignment;
        bool used;
        std::chrono::system_clock::time_point allocatedTime;
        
        MemoryBlock() : ptr(nullptr), size(0), alignment(0), used(false) {}
    };
    
    std::vector<MemoryBlock> memoryPool;
    std::mutex mutex;
    size_t totalAllocations;
    size_t totalDeallocations;
};

class ArenaMemoryManager : public CustomMemoryManager {
public:
    ArenaMemoryManager() = default;
    ~ArenaMemoryManager() override = default;
    
    std::string getName() const override { return "ArenaManager"; }
    std::string getVersion() const override { return "1.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    void* allocate(size_t size, size_t alignment) override;
    void deallocate(void* ptr) override;
    void* allocateAligned(size_t size, size_t alignment) override;
    
    size_t getPoolSize() const override;
    size_t getUsedPoolSize() const override;
    size_t getAvailablePoolSize() const override;
    bool preallocate(size_t size) override;
    bool deallocateAll() override;
    
    bool enableMemoryOptimization(bool enable) override;
    bool isMemoryOptimizationEnabled() const override;
    bool compact() override;
    bool setMemoryPoolStrategy(const std::string& strategy) override;
    std::string getMemoryPoolStrategy() const override;
    
    size_t getTotalAllocations() const override;
    size_t getTotalDeallocations() const override;
    double getMemoryEfficiency() const override;
    std::string getMemoryStats() const override;
    
private:
    struct Arena {
        void* basePtr;
        size_t size;
        size_t usedSize;
        std::vector<void*> allocations;
        
        Arena() : basePtr(nullptr), size(0), usedSize(0) {}
    };
    
    Arena currentArena;
    std::mutex mutex;
    size_t totalAllocations;
    size_t totalDeallocations;
};

} // namespace nlm
