#include "BrainAdapter.hpp"
#include <stdexcept>
#include <iostream>

namespace nlm {

// Implementation of BrainAdapter constructor
BrainAdapter::BrainAdapter(std::shared_ptr<Config> config)
    : config_(config)
    , eventDrivenEnabled_(false)
    , multithreadingEnabled_(false)
    , simdEnabled_(false)
    , sparseEnabled_(false)
    , performanceMonitoringEnabled_(false)
{
    // Create optimized brain internally
    optimizedBrain_ = std::make_unique<OptimizedBrain>(config);
    
    // Initialize optimized brain
    if (!optimizedBrain_->initialize()) {
        throw std::runtime_error("Failed to initialize OptimizedBrain");
    }
    
    // Setup memory pool manager
    memoryPoolManager_ = &MemoryPoolManager::getInstance();
    memoryPoolManager_->setConfig(config);
    
    // Setup event system
    eventSystem_ = optimizedBrain_->getEventSystem();
    
    // Setup connectivity manager
    connectivity_ = optimizedBrain_->getConnectivityManager();
    
    // Setup parallel processor
    parallelProcessor_ = optimizedBrain_->getParallelProcessor();
    
    // Setup SIMD vectorizer
    simdVectorizer_ = optimizedBrain_->getSIMDVectorizer();
    
    // Copy optimization flags from optimized brain
    eventDrivenEnabled_ = optimizedBrain_->isEventDrivenEnabled();
    multithreadingEnabled_ = optimizedBrain_->isMultithreadingEnabled();
    simdEnabled_ = optimizedBrain_->isSIMDEnabled();
    sparseEnabled_ = optimizedBrain_->isSparseEnabled();
}

// Additional implementation details for BrainAdapter

void BrainAdapter::enableEventDriven(bool enable) {
    optimizedBrain_->enableEventDriven(enable);
    eventDrivenEnabled_ = enable;
}

void BrainAdapter::enableMultithreading(bool enable) {
    optimizedBrain_->enableMultithreading(enable);
    multithreadingEnabled_ = enable;
}

void BrainAdapter::enableSIMD(bool enable) {
    optimizedBrain_->enableSIMD(enable);
    simdEnabled_ = enable;
}

void BrainAdapter::enableSparseConnectivity(bool enable) {
    optimizedBrain_->enableSparseConnectivity(enable);
    sparseEnabled_ = enable;
}

void BrainAdapter::setThreadCount(size_t count) {
    optimizedBrain_->setThreadCount(count);
}

size_t BrainAdapter::getThreadCount() const {
    return optimizedBrain_->getThreadCount();
}

void BrainAdapter::enablePerformanceMonitoring(bool enable) {
    performanceMonitoringEnabled_ = enable;
    optimizedBrain_->enablePerformanceMonitoring(enable);
}

void BrainAdapter::reportPerformance() {
    optimizedBrain_->reportPerformance();
}

} // namespace nlm