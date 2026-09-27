#include "PerformanceOptimizer.hpp"
#include "MemoryManager.hpp"
#include "CacheManager.hpp"
#include "BatchProcessor.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

PerformanceOptimizer::PerformanceOptimizer(std::shared_ptr<PerformanceConfig> config)
    : config_(config)
    , brain_(nullptr)
    , operationCount_(0)
    , totalOperationTime_(0.0)
    , debugOutput_(false)
    , stepsSinceLastReport_(0)
    , totalSteps_(0)
{
    // Initialize optimization status
    optimizationEnabled_["memory_pools"] = true;
    optimizationEnabled_["cache_management"] = true;
    optimizationEnabled_["batch_processing"] = true;
    optimizationEnabled_["simd_vectorization"] = config_->enableSIMD;
    optimizationEnabled_["prefetching"] = config_->enablePrefetching;
    
    // Initialize timing
    lastOperationTime_ = std::chrono::steady_clock::now();
    lastAdjustmentTime_ = std::chrono::steady_clock::now();
}

PerformanceOptimizer::~PerformanceOptimizer() {
    cleanup();
}

bool PerformanceOptimizer::initialize(Brain* brain) {
    if (!brain) {
        if (debugOutput_) {
            std::cout << "PerformanceOptimizer: Cannot initialize with null brain" << std::endl;
        }
        return false;
    }
    
    brain_ = brain;
    
    // Initialize memory manager
    memoryManager_ = std::make_unique<MemoryManager>();
    
    // Initialize cache manager
    cacheManager_ = std::make_unique<CacheManager>(config_->cacheSize, config_->cacheLineSize);
    
    // Initialize batch processor
    batchProcessor_ = std::make_unique<BatchProcessor>(config_->batchSize);
    
    // Initialize memory pools
    initializeMemoryPools();
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Initialized successfully" << std::endl;
        std::cout << "  - Memory pools configured" << std::endl;
        std::cout << "  - Cache manager initialized" << std::endl;
        std::cout << "  - Batch processor ready" << std::endl;
    }
    
    return true;
}

void PerformanceOptimizer::update(uint64_t step, double time) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    totalSteps_++;
    stepsSinceLastReport_++;
    
    // Update memory pool statistics
    updateMemoryStats();
    
    // Update performance statistics
    updatePerformanceStats(step, time);
    
    // Check cache efficiency
    checkCacheEfficiency();
    
    // Check memory utilization
    checkMemoryUtilization();
    
    // Check operation throughput
    checkThroughput();
    
    // Adjust optimization if needed
    if (shouldAdjustOptimization()) {
        adjustOptimization();
    }
    
    // Report stats if interval reached
    if (stepsSinceLastReport_ >= config_->statsReportInterval) {
        if (debugOutput_) {
            std::cout << "PerformanceOptimizer: Stats report - Step " << step << std::endl;
            std::cout << "  - Memory utilization: " << stats_.memoryUtilization * 100 << "%" << std::endl;
            std::cout << "  - Cache efficiency: " << stats_.cacheEfficiency * 100 << "%" << std::endl;
            std::cout << "  - Operations/sec: " << stats_.operationsPerSecond << std::endl;
        }
        stepsSinceLastReport_ = 0;
    }
}

PerformanceStats PerformanceOptimizer::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void PerformanceOptimizer::configure(const PerformanceConfig& newConfig) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    config_ = std::make_shared<PerformanceConfig>(newConfig);
    
    // Re-initialize components with new configuration
    memoryManager_.reset();
    cacheManager_.reset();
    batchProcessor_.reset();
    
    memoryManager_ = std::make_unique<MemoryManager>();
    cacheManager_ = std::make_unique<CacheManager>(config_->cacheSize, config_->cacheLineSize);
    batchProcessor_ = std::make_unique<BatchProcessor>(config_->batchSize);
    
    // Update optimization enabled flags
    optimizationEnabled_["simd_vectorization"] = config_->enableSIMD;
    optimizationEnabled_["prefetching"] = config_->enablePrefetching;
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Configuration updated" << std::endl;
    }
}

void PerformanceOptimizer::enableOptimization(const std::string& name, bool enable) {
    optimizationEnabled_[name] = enable;
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Optimization '" << name << "' " 
                  << (enable ? "enabled" : "disabled") << std::endl;
    }
}

bool PerformanceOptimizer::isOptimizationEnabled(const std::string& name) const {
    auto it = optimizationEnabled_.find(name);
    return (it != optimizationEnabled_.end()) ? it->second : false;
}

template<typename Operation>
void PerformanceOptimizer::processBatch(const std::vector<Operation>& operations) {
    if (isOptimizationEnabled("batch_processing")) {
        batchProcessor_->processBatch(operations);
    } else {
        // Process individually
        for (const auto& operation : operations) {
            // In a real implementation, this would call the actual operation
        }
    }
}

bool PerformanceOptimizer::shouldAdjustOptimization() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastAdjustmentTime_).count();
    
    // Adjust if enough time has passed
    if (elapsed < 60) return false;
    
    // Or if performance metrics are outside thresholds
    if (stats_.memoryUtilization > config_->memoryUtilizationThreshold) return true;
    if (stats_.cacheEfficiency < config_->cacheEfficiencyThreshold) return true;
    if (stats_.operationsPerSecond < config_->operationsPerSecondThreshold) return true;
    
    return false;
}

void PerformanceOptimizer::adjustOptimization() {
    lastAdjustmentTime_ = std::chrono::steady_clock::now();
    
    // Adjust memory pool sizes based on utilization
    if (stats_.memoryUtilization > config_->memoryUtilizationThreshold * 1.2f) {
        // Memory pressure - increase pool sizes
        if (memoryManager_) {
            memoryManager_->reserve(
                config_->neuronPoolInitial * 2,
                config_->synapsePoolInitial * 2,
                config_->workingMemoryPoolInitial * 2,
                config_->plasticityPoolInitial * 2
            );
        }
    } else if (stats_.memoryUtilization < config_->memoryUtilizationThreshold * 0.5f) {
        // Underutilized - could reduce pool sizes
        // For now, we'll keep them allocated for performance
    }
    
    // Adjust batch size based on cache efficiency
    if (stats_.cacheEfficiency < config_->cacheEfficiencyThreshold) {
        // Poor cache efficiency - increase batch size to improve locality
        batchProcessor_->setBatchSize(config_->batchSize * 2);
    }
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Optimization adjusted" << std::endl;
    }
}

void PerformanceOptimizer::collectGarbage() {
    if (memoryManager_) {
        memoryManager_->clear();
    }
    
    if (batchProcessor_) {
        batchProcessor_->cleanup();
    }
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Garbage collection completed" << std::endl;
    }
}

std::vector<std::string> PerformanceOptimizer::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (stats_.memoryUtilization > config_->memoryUtilizationThreshold) {
        recommendations.push_back("Increase memory pool sizes to reduce allocation pressure");
    }
    
    if (stats_.cacheEfficiency < config_->cacheEfficiencyThreshold) {
        recommendations.push_back("Improve data locality - consider Structure of Arrays (SoA) layout");
    }
    
    if (stats_.operationsPerSecond < config_->operationsPerSecondThreshold) {
        recommendations.push_back("Consider SIMD vectorization for compute-bound operations");
        recommendations.push_back("Enable prefetching for memory-bound operations");
    }
    
    if (recommendations.empty()) {
        recommendations.push_back("Performance is optimal - maintain current configuration");
    }
    
    return recommendations;
}

void PerformanceOptimizer::resetStats() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    stats_ = PerformanceStats();
    operationCount_ = 0;
    totalOperationTime_ = 0.0;
    stepsSinceLastReport_ = 0;
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Statistics reset" << std::endl;
    }
}

void PerformanceOptimizer::updateMemoryStats() {
    if (!memoryManager_) return;
    
    auto memoryStats = memoryManager_->getStatistics();n    
    stats_.totalMemoryUsage = memoryStats.totalMemoryUsage;
    stats_.neuronMemoryUsage = memoryStats.neuronPool.memoryUsage;
    stats_.synapseMemoryUsage = memoryStats.synapsePool.memoryUsage;
    stats_.workingMemoryUsage = memoryStats.workingMemoryPool.memoryUsage;
    stats_.plasticityMemoryUsage = memoryStats.plasticityPool.memoryUsage;
    
    stats_.neuronCount = memoryStats.neuronPool.allocatedCount;
    stats_.synapseCount = memoryStats.synapsePool.allocatedCount;
    stats_.workingMemoryCount = memoryStats.workingMemoryPool.allocatedCount;
    stats_.plasticityStateCount = memoryStats.plasticityPool.allocatedCount;
    
    if (memoryStats.totalAllocated > 0) {
        stats_.memoryUtilization = static_cast<float>(memoryStats.totalMemoryUsage) / 
                                 static_cast<float>(memoryStats.totalAllocated * sizeof(PlasticityState));
    }
}

void PerformanceOptimizer::updatePerformanceStats(uint64_t step, double time) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastOperationTime_).count();
    
    if (elapsed > 0) {
        stats_.operationsPerSecond = operationCount_.load() / elapsed;
        stats_.avgOperationTime = (totalOperationTime_ / operationCount_.load()) * 1000.0; // Convert to ms
    }
    
    stats_.simulationStep = step;
    stats_.simulationTime = time;
    
    // Update last operation time for next calculation
    lastOperationTime_ = now;
}

void PerformanceOptimizer::checkCacheEfficiency() {
    if (cacheManager_) {
        stats_.cacheEfficiency = cacheManager_->getEfficiency();
    }
}

void PerformanceOptimizer::checkMemoryUtilization() {
    // Already updated in updateMemoryStats
}

void PerformanceOptimizer::checkThroughput() {
    // Throughput is updated in updatePerformanceStats
}

void PerformanceOptimizer::recordOperation(double duration) {
    operationCount_.fetch_add(1, std::memory_order_relaxed);
    totalOperationTime_ += duration;
}

void PerformanceOptimizer::initializeMemoryPools() {
    if (!memoryManager_) return;
    
    // Reserve initial objects based on configuration
    memoryManager_->reserve(
        config_->neuronPoolInitial,
        config_->synapsePoolInitial,
        config_->workingMemoryPoolInitial,
        config_->plasticityPoolInitial
    );
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Memory pools initialized" << std::endl;
        std::cout << "  - Neuron pool: " << config_->neuronPoolInitial << " objects" << std::endl;
        std::cout << "  - Synapse pool: " << config_->synapsePoolInitial << " objects" << std::endl;
        std::cout << "  - Working memory pool: " << config_->workingMemoryPoolInitial << " objects" << std::endl;
        std::cout << "  - Plasticity pool: " << config_->plasticityPoolInitial << " objects" << std::endl;
    }
}

void PerformanceOptimizer::initializeCacheManager() {
    if (!cacheManager_) return;
    
    // Cache manager is initialized in constructor
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Cache manager configured" << std::endl;
        std::cout << "  - Cache size: " << config_->cacheSize << " bytes" << std::endl;
        std::cout << "  - Cache line size: " << config_->cacheLineSize << " bytes" << std::endl;
    }
}

void PerformanceOptimizer::initializeBatchProcessor() {
    if (!batchProcessor_) return;
    
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Batch processor configured" << std::endl;
        std::cout << "  - Batch size: " << config_->batchSize << " operations" << std::endl;
    }
}

void PerformanceOptimizer::cleanup() {
    if (debugOutput_) {
        std::cout << "PerformanceOptimizer: Cleaning up" << std::endl;
    }
    
    memoryManager_.reset();
    cacheManager_.reset();
    batchProcessor_.reset();
    
    brain_ = nullptr;
}

} // namespace nlm
