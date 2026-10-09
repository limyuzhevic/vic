#include "BatchRunner.hpp"
#include <algorithm>
#include <cstring>

namespace nlm {

SharedMemoryPool::SharedMemoryPool(size_t totalMemoryMB)
    : totalMemory_(totalMemoryMB * 1024 * 1024), // Convert MB to bytes
      freeMemory_(totalMemory_)
{
    // Allocate memory blocks in reasonable chunks
    const size_t blockSize = 64 * 1024; // 64 KB blocks
    size_t numBlocks = totalMemory_ / blockSize;
    
    for (size_t i = 0; i < numBlocks; ++i) {
        void* ptr = nullptr;
        
        // Try to allocate aligned memory
        if (posix_memalign(&ptr, 64, blockSize) == 0) {
            memoryBlocks_.emplace_back(std::make_unique<MemoryBlock>(ptr, blockSize));
            freeMemory_ -= blockSize;
        } else {
            // Fallback to regular allocation
            ptr = std::malloc(blockSize);
            if (ptr) {
                memoryBlocks_.emplace_back(std::make_unique<MemoryBlock>(ptr, blockSize));
                freeMemory_ -= blockSize;
            }
        }
    }
}

SharedMemoryPool::~SharedMemoryPool() {
    for (auto& block : memoryBlocks_) {
        if (block->data) {
            std::free(block->data);
        }
    }
}

MemoryBlock* SharedMemoryPool::allocate(size_t size, const std::string& owner) {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    // Find first suitable free block
    for (auto& block : memoryBlocks_) {
        if (block->isFree && block->size >= size) {
            block->isFree = false;
            block->ownerThread = owner;
            threadMemory_[owner].push_back(block.get());
            freeMemory_ -= block->size;
            return block.get();
        }
    }
    
    return nullptr; // No suitable memory found
}

void SharedMemoryPool::deallocate(MemoryBlock* block, const std::string& owner) {
    if (!block) return;
    
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    // Find and remove from owner's list
    auto it = threadMemory_.find(owner);
    if (it != threadMemory_.end()) {
        auto& blocks = it->second;
        blocks.erase(std::remove(blocks.begin(), blocks.end(), block), blocks.end());
        if (blocks.empty()) {
            threadMemory_.erase(it);
        }
    }
    
    block->isFree = true;
    block->ownerThread.clear();
    freeMemory_ += block->size;
}

ThreadSafeConfigManager::ThreadSafeConfigManager(std::shared_ptr<Config> baseConfig)
    : baseConfig_(std::move(baseConfig))
{
    if (baseConfig_) {
        baseConfig_->configure_for_task("research", std::vector<std::string>());
    }
}

std::shared_ptr<Config> ThreadSafeConfigManager::getThreadConfig(size_t threadId) {
    std::lock_guard<std::shared_mutex> lock(configMutex_);
    
    auto it = threadConfigs_.find(threadId);
    if (it != threadConfigs_.end()) {
        return it->second;
    }
    
    // Create new thread-specific config
    auto threadConfig = std::make_shared<Config>();
    if (baseConfig_) {
        // Copy base config (simplified - assumes Config has copy constructor)
        threadConfig->configure_for_task("research", std::vector<std::string>());
    }
    
    threadConfigs_[threadId] = threadConfig;
    return threadConfig;
}

void ThreadSafeConfigManager::updateGlobalConfig(const std::map<std::string, std::string>& updates) {
    std::lock_guard<std::shared_mutex> lock(configMutex_);
    
    if (!baseConfig_) return;
    
    // Apply updates (simplified implementation)
    for (const auto& [key, value] : updates) {
        baseConfig_->clear();
        // Note: This assumes Config has methods to set key-value pairs
    }
}

void ThreadSafeConfigManager::resetConfig() {
    std::lock_guard<std::shared_mutex> lock(configMutex_);
    
    threadConfigs_.clear();
    if (baseConfig_) {
        baseConfig_->configure_for_task("research", std::vector<std::string>());
    }
}

ParallelSimulationRunner::ParallelSimulationRunner(
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    std::shared_ptr<SharedMemoryPool> memoryPool)
    : brainTemplate_(std::move(brainTemplate))
    , environmentTemplate_(std::move(environmentTemplate))
    , memoryPool_(std::move(memoryPool))
    , configManager_(std::make_shared<ThreadSafeConfigManager>(brainTemplate_->getConfig()))
{
}

void ParallelSimulationRunner::setConfigManager(std::shared_ptr<ThreadSafeConfigManager> configManager) {
    if (configManager) {
        std::lock_guard<std::mutex> lock(runnerMutex_);
        configManager_ = std::move(configManager);
    }
}

std::vector<BatchSimulationResult> ParallelSimulationRunner::runParallelSweep(
    const std::vector<std::map<std::string, double>>& parameterSets,
    const std::string& paramKey,
    int timesteps,
    size_t maxThreads,
    bool useSharedMemory) {
    std::vector<BatchSimulationResult> results;
    
    if (maxThreads == 0) {
        maxThreads = std::thread::hardware_concurrency();
    }
    
    ThreadPool threadPool(maxThreads);
    BatchProgress progress(parameterSets.size());
    
    std::vector<std::future<BatchSimulationResult>> futures;
    futures.reserve(parameterSets.size());
    
    for (size_t i = 0; i < parameterSets.size(); ++i) {
        futures.push_back(threadPool.submit(
            [this, i, paramSets = parameterSets, paramKey, timesteps, useSharedMemory]() {
                return runSingleSimulationImpl(i, paramSets[i], timesteps, paramKey, useSharedMemory);
            }
        ));
    }
    
    results.reserve(futures.size());
    for (auto& future : futures) {
        try {
            results.push_back(future.get());
        } catch (const std::exception& e) {
            // Create failed result
            BatchSimulationResult failedResult(0, nullptr, nullptr, "error", 0.0, "failed");
            failedResult.errorMessage = e.what();
            results.push_back(std::move(failedResult));
        }
    }
    
    return results;
}

std::vector<BatchSimulationResult> ParallelSimulationRunner::runParallelBatch(
    const std::vector<std::shared_ptr<Config>>& configs,
    int timesteps,
    size_t maxThreads,
    bool useSharedMemory) {
    std::vector<BatchSimulationResult> results;
    
    if (maxThreads == 0) {
        maxThreads = std::thread::hardware_concurrency();
    }
    
    ThreadPool threadPool(maxThreads);
    BatchProgress progress(configs.size());
    
    std::vector<std::future<BatchSimulationResult>> futures;
    futures.reserve(configs.size());
    
    for (size_t i = 0; i < configs.size(); ++i) {
        futures.push_back(threadPool.submit(
            [this, i, config = configs[i], timesteps, useSharedMemory]() {
                return runSingleSimulationImpl(i, config, timesteps, useSharedMemory);
            }
        ));
    }
    
    results.reserve(futures.size());
    for (auto& future : futures) {
        try {
            results.push_back(future.get());
        } catch (const std::exception& e) {
            BatchSimulationResult failedResult(0, nullptr, nullptr, "error", 0.0, "failed");
            failedResult.errorMessage = e.what();
            results.push_back(std::move(failedResult));
        }
    }
    
    return results;
}

BatchSimulationResult ParallelSimulationRunner::runSingleSimulationImpl(
    size_t batchIndex,
    std::shared_ptr<Config> config,
    int timesteps,
    bool useSharedMemory) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Get thread-local config
        size_t threadId = ThreadPool::getCurrentThreadId();
        std::shared_ptr<Config> threadConfig = configManager_->getThreadConfig(threadId);
        
        // Create brain with config
        auto brain = std::make_shared<Brain>(threadConfig);
        auto environment = std::make_shared<SimpleWorld>();
        
        // Initialize brain
        brain->initialize();
        
        // Use shared memory if requested
        std::shared_ptr<SharedMemoryPool> memoryPool = nullptr;
        if (useSharedMemory && memoryPool_) {
            memoryPool = memoryPool_;
        }
        
        // Run simulation
        brain->run_simulation(timesteps, environment.get());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get metrics
        std::map<std::string, double> metrics;
        metrics["execution_time"] = executionTime;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        
        std::string configName = "config_" + std::to_string(batchIndex);
        
        BatchSimulationResult result(batchIndex, brain, environment, configName, executionTime, "completed");
        result.metrics = metrics;
        
        return result;
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::string configName = "config_" + std::to_string(batchIndex);
        
        BatchSimulationResult result(batchIndex, nullptr, nullptr, configName, executionTime, "failed");
        result.errorMessage = e.what();
        
        return result;
    }
}

BatchSimulationResult ParallelSimulationRunner::runSingleSimulationImpl(
    size_t batchIndex,
    std::map<std::string, double> parameters,
    int timesteps,
    const std::string& paramKey,
    bool useSharedMemory) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create config from parameters
        auto config = std::make_shared<Config>();
        config->configure_for_task("research", std::vector<std::string>());
        
        // Apply parameters (simplified)
        if (parameters.find(paramKey) != parameters.end()) {
            // Note: This assumes Config has methods to set specific parameters
        }
        
        // Create brain with config
        auto brain = std::make_shared<Brain>(config);
        auto environment = std::make_shared<SimpleWorld>();
        
        // Initialize brain
        brain->initialize();
        
        // Run simulation
        brain->run_simulation(timesteps, environment.get());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get metrics
        std::map<std::string, double> metrics;
        metrics["execution_time"] = executionTime;
        metrics["neuron_count"] = brain->getTotalNeuronCount();
        metrics["synapse_count"] = brain->getTotalSynapseCount();
        metrics["spike_count"] = brain->getTotalSpikeCount();
        
        std::string configName = "param_" + std::to_string(batchIndex) + "_" + paramKey;
        
        BatchSimulationResult result(batchIndex, brain, environment, configName, executionTime, "completed");
        result.metrics = metrics;
        
        return result;
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::string configName = "param_" + std::to_string(batchIndex);
        
        BatchSimulationResult result(batchIndex, nullptr, nullptr, configName, executionTime, "failed");
        result.errorMessage = e.what();
        
        return result;
    }
}

ParallelParameterSweep::ParallelParameterSweep(
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    size_t numWorkers)
    : brainTemplate_(std::move(brainTemplate))
    , environmentTemplate_(std::move(environmentTemplate))
    , threadPool_(numWorkers > 0 ? numWorkers : std::thread::hardware_concurrency())
{
}

std::vector<BatchSimulationResult> ParallelParameterSweep::sweepParameters(
    const std::vector<std::map<std::string, double>>& parameterSets,
    const std::vector<std::string>& parameterNames,
    int timesteps,
    const std::string& outputMetric) {
    std::vector<BatchSimulationResult> results;
    results.reserve(parameterSets.size());
    
    totalJobs_ = parameterSets.size();
    completedJobs_ = 0;
    
    // Distribute work
    size_t chunkSize = (parameterSets.size() + threadPool_.getNumThreads() - 1) / threadPool_.getNumThreads();
    
    std::vector<std::future<void>> futures;
    size_t startIndex = 0;
    
    while (startIndex < parameterSets.size()) {
        size_t endIndex = std::min(startIndex + chunkSize, parameterSets.size());
        
        futures.push_back(threadPool_.submitVoid(
            [this, &parameterSets, &results, startIndex, endIndex, timesteps, &outputMetric]() {
                distributeWork(parameterSets, results, startIndex, endIndex, timesteps, outputMetric);
            }
        ));
        
        startIndex = endIndex;
    }
    
    // Wait for all futures to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    return results;
}

void ParallelParameterSweep::distributeWork(
    const std::vector<std::map<std::string, double>>& parameterSets,
    std::vector<BatchSimulationResult>& results,
    size_t startIndex, size_t endIndex,
    int timesteps, const std::string& outputMetric) {
    // Create thread-local copies of templates
    auto brain = std::make_shared<Brain>(brainTemplate_->getConfig());
    auto environment = std::make_shared<SimpleWorld>();
    
    // Process assigned parameter sets
    for (size_t i = startIndex; i < endIndex; ++i) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create config from parameters
            auto config = std::make_shared<Config>();
            config->configure_for_task("research", std::vector<std::string>());
            
            // Apply parameters (simplified)
            for (const auto& [key, value] : parameterSets[i]) {
                // Note: This assumes Config has methods to set key-value pairs
            }
            
            // Run simulation
            brain->run_simulation(timesteps, environment.get());
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get output metric
            double metricValue = 0.0;
            if (outputMetric == "performance") {
                metricValue = brain->getTotalSpikeCount();
            } else if (outputMetric == "stability") {
                metricValue = brain->getAverageFiringRate();
            }
            
            // Store result
            BatchSimulationResult result(i, brain, environment,
                                        "sweep_" + std::to_string(i),
                                        executionTime, "completed");
            result.metrics["output_metric"] = metricValue;
            
            {
                std::lock_guard<std::mutex> lock(statusMutex_);
                results.push_back(std::move(result));
                completedJobs_++;
            }
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            BatchSimulationResult failedResult(i, nullptr, nullptr,
                                              "sweep_" + std::to_string(i),
                                              executionTime, "failed");
            failedResult.errorMessage = e.what();
            
            {
                std::lock_guard<std::mutex> lock(statusMutex_);
                results.push_back(std::move(failedResult));
                completedJobs_++;
            }
        }
    }
}