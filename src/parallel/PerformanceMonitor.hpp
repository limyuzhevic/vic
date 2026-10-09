#pragma once

#include "../performance/ParallelProcessing.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <shared_mutex>

namespace nlm {

// Shared memory utilities for parallel access
class SharedMemoryPool {
public:
    struct MemoryBlock {
        void* data;
        size_t size;
        bool isFree;
        std::mutex accessMutex;
        std::string ownerThread;
        
        MemoryBlock() : data(nullptr), size(0), isFree(true) {}
        MemoryBlock(void* ptr, size_t sz) : data(ptr), size(sz), isFree(true) {}
    };
    
    SharedMemoryPool(size_t totalMemoryMB = 1024);
    ~SharedMemoryPool();
    
    // Allocate memory block for thread
    MemoryBlock* allocate(size_t size, const std::string& owner);
    
    // Release memory block
    void deallocate(MemoryBlock* block, const std::string& owner);
    
    // Get total available memory
    size_t getTotalMemory() const { return totalMemory_; }
    
    // Get free memory
    size_t getFreeMemory() const { return freeMemory_; }
    
private:
    size_t totalMemory_;
    size_t freeMemory_;
    std::mutex poolMutex;
    std::vector<std::unique_ptr<MemoryBlock>> memoryBlocks_;
    std::unordered_map<std::string, std::vector<MemoryBlock*>> threadMemory_;
};

// Thread-safe configuration management
class ThreadSafeConfigManager {
public:
    ThreadSafeConfigManager(std::shared_ptr<Config> baseConfig);
    
    // Get thread-local configuration
    std::shared_ptr<Config> getThreadConfig(size_t threadId);
    
    // Update configuration for all threads
    void updateGlobalConfig(const std::map<std::string, std::string>& updates);
    
    // Get global configuration
    std::shared_ptr<Config> getGlobalConfig() const { return baseConfig_; }
    
    // Reset configuration
    void resetConfig();
    
private:
    std::shared_ptr<Config> baseConfig_;
    mutable std::shared_mutex configMutex_;
    std::unordered_map<size_t, std::shared_ptr<Config>> threadConfigs_;
    std::mutex threadConfigsMutex_;
};

// Parallel simulation runner using threading
class ParallelSimulationRunner {
public:
    ParallelSimulationRunner(std::shared_ptr<Brain> brainTemplate,
                            std::shared_ptr<Environment> environmentTemplate,
                            std::shared_ptr<SharedMemoryPool> memoryPool = nullptr);
    
    // Run parallel simulation with parameter sweep
    std::vector<BatchSimulationResult> runParallelSweep(
        const std::vector<std::map<std::string, double>>& parameterSets,
        const std::string& paramKey,
        int timesteps = 100,
        size_t maxThreads = 0,
        bool useSharedMemory = false);
    
    // Run parallel batch simulations
    std::vector<BatchSimulationResult> runParallelBatch(
        const std::vector<std::shared_ptr<Config>>& configs,
        int timesteps = 100,
        size_t maxThreads = 0,
        bool useSharedMemory = false);
    
    // Set thread-safe config manager
    void setConfigManager(std::shared_ptr<ThreadSafeConfigManager> configManager);
    
    // Get current thread-safe config manager
    std::shared_ptr<ThreadSafeConfigManager> getConfigManager() const { return configManager_; }
    
private:
    std::shared_ptr<Brain> brainTemplate_;
    std::shared_ptr<Environment> environmentTemplate_;
    std::shared_ptr<SharedMemoryPool> memoryPool_;
    std::shared_ptr<ThreadSafeConfigManager> configManager_;
    mutable std::mutex runnerMutex_;
    
    BatchSimulationResult runSingleSimulationImpl(
        size_t batchIndex,
        std::shared_ptr<Config> config,
        int timesteps,
        bool useSharedMemory);
};

// Parallel parameter sweep with work distribution
class ParallelParameterSweep {
public:
    ParallelParameterSweep(std::shared_ptr<Brain> brainTemplate,
                          std::shared_ptr<Environment> environmentTemplate,
                          size_t numWorkers = 0);
    
    // Perform parallel parameter sweep with dynamic work distribution
    std::vector<BatchSimulationResult> sweepParameters(
        const std::vector<std::map<std::string, double>>& parameterSets,
        const std::vector<std::string>& parameterNames,
        int timesteps = 100,
        const std::string& outputMetric = "performance");
    
    // Distribute work across worker threads
    void distributeWork(const std::vector<std::map<std::string, double>>& parameterSets,
                       std::vector<BatchSimulationResult>& results,
                       size_t startIndex, size_t endIndex,
                       int timesteps, const std::string& outputMetric);
    
    // Get work queue status
    size_t getCompletedJobs() const { return completedJobs_.load(); }
    size_t getTotalJobs() const { return totalJobs_.load(); }
    
private:
    std::shared_ptr<Brain> brainTemplate_;
    std::shared_ptr<Environment> environmentTemplate_;
    ThreadPool threadPool_;
    std::atomic<size_t> completedJobs_{0};
    std::atomic<size_t> totalJobs_{0};
    std::mutex statusMutex_;
};nlm