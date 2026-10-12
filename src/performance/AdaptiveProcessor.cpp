#include "AdaptiveProcessor.hpp"
#include <iostream>

namespace nlm {

AdaptiveProcessor::AdaptiveProcessor()
    : workloadThreshold_(0.7)
    , memoryPressureThreshold_(0.8)
    , minThreadsToEnableMT_(2) {}

void AdaptiveProcessor::updateConfiguration(const PerformanceMetrics& metrics) {
    // Update adaptive thresholds based on current workload
    if (metrics.cpuUsagePercent > 80.0) {
        workloadThreshold_ = std::max(0.8, workloadThreshold_ * 1.1);
    } else if (metrics.cpuUsagePercent < 30.0) {
        workloadThreshold_ = std::max(0.5, workloadThreshold_ * 0.9);
    }
    
    if (metrics.memoryUsageBytes > (memoryPressureThreshold_ * 1024 * 1024 * 1024)) {
        memoryPressureThreshold_ = std::min(0.95, memoryPressureThreshold_ * 1.1);
    }
    
    // Adjust thread count based on workload
    if (metrics.cpuUsagePercent > workloadThreshold_ * 100.0) {
        defaultConfig_.threadCount = std::min(defaultConfig_.threadCount * 2, 
                                             static_cast<size_t>(std::thread::hardware_concurrency()));
    }
    
    if (AdaptiveProcessor::getInstance().isEnabled()) {
        std::cout << "[ADAPTIVE] Updated adaptive configuration:" << std::endl;
        std::cout << "[ADAPTIVE]   Workload threshold: " << workloadThreshold_ << std::endl;
        std::cout << "[ADAPTIVE]   Memory pressure threshold: " << memoryPressureThreshold_ << std::endl;
        std::cout << "[ADAPTIVE]   Thread count: " << defaultConfig_.threadCount << std::endl;
    }
}

RuntimePerformanceConfig AdaptiveProcessor::getOptimalConfiguration(const PerformanceMetrics& metrics) const {
    RuntimePerformanceConfig config = defaultConfig_;
    
    // Enable multithreading based on workload
    if (shouldEnableMultithreading(metrics)) {
        config.useMultithreading = true;
        // Scale thread count based on workload
        config.threadCount = std::min(config.threadCount, 
                                     static_cast<size_t>(metrics.cpuUsagePercent > 80.0 ? 
                                         std::thread::hardware_concurrency() : std::thread::hardware_concurrency() / 2));
    }
    
    // Enable SIMD based on workload
    if (shouldEnableSIMD(metrics)) {
        config.useSIMD = true;
        // Prefer vector instructions based on workload
        if (metrics.cpuUsagePercent > 70.0) {
            config.preferAVX512 = true;
            config.preferAVX2 = true;
        }
    }
    
    // Enable event-driven processing based on spike frequency
    if (shouldEnableEventDriven(metrics)) {
        config.useEventQueue = true;
        // Adjust event queue size based on spike rate
        if (metrics.spikesGenerated > 10000) {
            config.eventQueueCapacity = std::min(config.eventQueueCapacity * 2, 32768);
        }
    }
    
    // Enable sparse connectivity based on connectivity
    if (shouldEnableSparseConnectivity(metrics)) {
        config.useSparseConnectivity = true;
    }
    
    // Apply performance budget
    if (metrics.stepTimeMs > config.timeBudgetPerStep * 1000) {
        // Reduce complexity if over time budget
        config.useMultithreading = false;
        config.useSIMD = false;
        config.eventQueueCapacity = std::max(config.eventQueueCapacity / 2, 4096);
    }
    
    return config;
}

bool AdaptiveProcessor::shouldEnableMultithreading(const PerformanceMetrics& metrics) const {
    return metrics.cpuUsagePercent > workloadThreshold_ * 100.0 &&
           metrics.totalThreads >= minThreadsToEnableMT_;
}

bool AdaptiveProcessor::shouldEnableSIMD(const PerformanceMetrics& metrics) const {
    return metrics.cpuUsagePercent > 50.0 &&
           (hasAVX512_() || hasAVX2_() || hasSSE_());
}

bool AdaptiveProcessor::shouldEnableEventDriven(const PerformanceMetrics& metrics) const {
    // Enable event-driven if spike rate is high (many events)
    return metrics.spikesGenerated > 1000;
}

bool AdaptiveProcessor::shouldEnableSparseConnectivity(const PerformanceMetrics& metrics) const {
    // Enable sparse connectivity based on memory efficiency
    return metrics.synapticEfficiency < 0.5;
}

// Helper functions for SIMD detection (would be provided by SIMDEngine)
inline bool hasAVX512_() { return SIMDEngine::getInstance().hasAVX512(); }
inline bool hasAVX2_() { return SIMDEngine::getInstance().hasAVX2(); }
inline bool hasSSE_() { return SIMDEngine::getInstance().hasSSE(); }

} // namespace nlm