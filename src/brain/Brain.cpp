// Protected method to allow other classes to access brain components
void Brain::registerExternalListener(std::function<void(const DetailedSpikeEvent&)> callback) {
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerHandler(std::move(callback));
    }
}

// Performance monitoring and metrics collection
namespace {
    struct PerformanceMetrics {
        size_t stepsProcessed;
        size_t neuronsActivated;
        size_t spikesGenerated;
        size_t memoryAllocations;
        size_t checkpointSaves;
        double averageProcessingTime;
        double peakMemoryUsage;
        std::chrono::system_clock::time_point lastPerformanceCheck;
        
        PerformanceMetrics() : stepsProcessed(0), neuronsActivated(0), spikesGenerated(0),
                               memoryAllocations(0), checkpointSaves(0), averageProcessingTime(0.0),
                               peakMemoryUsage(0.0), lastPerformanceCheck(std::chrono::system_clock::now()) {}
    };
}

// Start performance monitoring
void Brain::startPerformanceMonitoring(size_t samplingIntervalSteps) {
    NLM_LOG_INFO("Performance monitoring started with sampling interval: " + std::to_string(samplingIntervalSteps) + " steps");
    
    // Record initial performance metrics
    auto now = std::chrono::system_clock::now();
    pImpl->lastPerformanceCheck = now;
    
    // Reset performance counters
    pImpl->performanceMetrics.stepsProcessed = 0;
    pImpl->performanceMetrics.neuronsActivated = 0;
    pImpl->performanceMetrics.spikesGenerated = 0;
    pImpl->performanceMetrics.memoryAllocations = 0;
    pImpl->performanceMetrics.checkpointSaves = 0;
}

// Collect performance metrics
PerformanceMetrics Brain::collectPerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Basic performance counters
    metrics.stepsProcessed = pImpl->currentStep;
    metrics.neuronsActivated = getActiveNeuronCount();
    metrics.spikesGenerated = getTotalSpikeCount();
    
    // Memory usage approximation (simplified)
    metrics.peakMemoryUsage = static_cast<double>(getTotalNeuronCount() * 100);  // rough estimate
    
    // Calculate average processing time
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastPerformanceCheck).count();
    metrics.averageProcessingTime = elapsed > 0 ? metrics.spikesGenerated / elapsed * 1000.0 : 0.0;
    
    // Increment performance check timestamp
    pImpl->lastPerformanceCheck = now;
    
    return metrics;
}

// Log performance metrics
void Brain::logPerformanceMetrics() const {
    PerformanceMetrics metrics = collectPerformanceMetrics();
    
    NLM_LOG_INFO("=== Performance Metrics ===");
    NLM_LOG_INFO("Steps processed: " + std::to_string(metrics.stepsProcessed));
    NLM_LOG_INFO("Neurons activated: " + std::to_string(metrics.neuronsActivated));
    NLM_LOG_INFO("Spikes generated: " + std::to_string(metrics.spikesGenerated));
    NLM_LOG_INFO("Average spikes per second: " + std::to_string(metrics.averageProcessingTime));
    NLM_LOG_INFO("Estimated memory usage: " + std::to_string(metrics.peakMemoryUsage) + " bytes");
    
    // Calculate additional metrics
    if (metrics.stepsProcessed > 0) {
        double neuronsPerStep = static_cast<double>(metrics.neuronsActivated) / metrics.stepsProcessed;
        double spikesPerStep = static_cast<double>(metrics.spikesGenerated) / metrics.stepsProcessed;
        
        NLM_LOG_INFO("Average neurons activated per step: " + std::to_string(neuronsPerStep));
        NLM_LOG_INFO("Average spikes generated per step: " + std::to_string(spikesPerStep));
    }
    
    // Log learning metrics if plasticity is enabled
    if (pImpl->stdp && pImpl->stdp->getLTPWeight() > 0.0f) {
        NLM_LOG_INFO("Plasticity active - STDP LTP weight: " + std::to_string(pImpl->stdp->getLTPWeight()));
    }
    
    // Log memory system performance
    if (pImpl->workingMemory) {
        auto workingMemoryMetrics = pImpl->workingMemory->getMetrics();
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMemoryMetrics.capacity));
        NLM_LOG_INFO("Working memory active traces: " + std::to_string(workingMemoryMetrics.activeTraces));
    }
}

// Increment performance counter for neurons activated
void Brain::incrementNeuronActivationCount(size_t count) {
    pImpl->performanceMetrics.neuronsActivated += count;
}

// Increment performance counter for spikes generated
void Brain::incrementSpikeCount(size_t count) {
    pImpl->performanceMetrics.spikesGenerated += count;
}

// Increment performance counter for memory allocations
void Brain::incrementMemoryAllocationCount(size_t count) {
    pImpl->performanceMetrics.memoryAllocations += count;
}

// Increment performance counter for checkpoint saves
void Brain::incrementCheckpointSaveCount(size_t count) {
    pImpl->performanceMetrics.checkpointSaves += count;
}

// Get current performance metrics
PerformanceMetrics Brain::getPerformanceMetrics() const {
    return collectPerformanceMetrics();
}

// Reset performance metrics
void Brain::resetPerformanceMetrics() {
    auto now = std::chrono::system_clock::now();
    pImpl->performanceMetrics.stepsProcessed = 0;
    pImpl->performanceMetrics.neuronsActivated = 0;
    pImpl->performanceMetrics.spikesGenerated = 0;
    pImpl->performanceMetrics.memoryAllocations = 0;
    pImpl->performanceMetrics.checkpointSaves = 0;
    pImpl->performanceMetrics.averageProcessingTime = 0.0;
    pImpl->performanceMetrics.peakMemoryUsage = 0.0;
    pImpl->performanceMetrics.lastPerformanceCheck = now;
    
    NLM_LOG_INFO("Performance metrics reset");
}

} // namespace nlm