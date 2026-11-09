#include "PerformanceMonitor.hpp"
#include <numeric>

namespace nlm {

PerformanceMonitor::PerformanceMonitor(uint32_t sampleSize) 
    : sampleSize(sampleSize), stepCount(0) {
    timingHistory.reserve(sampleSize);
    memoryHistory.reserve(sampleSize);
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::recordStep(double wallTime, uint32_t activeNeurons, 
                                    uint32_t spikeCount, size_t memoryUsage) {
    std::lock_guard<std::mutex> lock(mutex);
    
    timingHistory.push_back(wallTime);
    neuronHistory.push_back(static_cast<double>(activeNeurons));
    spikeHistory.push_back(static_cast<double>(spikeCount));
    memoryHistory.push_back(static_cast<double>(memoryUsage));
    
    stepCount++;
    
    // Maintain fixed sample size
    if (timingHistory.size() > sampleSize) {
        timingHistory.erase(timingHistory.begin());
        neuronHistory.erase(neuronHistory.begin());
        spikeHistory.erase(spikeHistory.begin());
        memoryHistory.erase(memoryHistory.begin());
    }
}

PerformanceMetrics PerformanceMonitor::getMetrics() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    PerformanceMetrics metrics;
    
    if (timingHistory.empty()) {
        return metrics;
    }
    
    // Calculate statistics for each metric
    metrics.avgWallTime = std::accumulate(timingHistory.begin(), timingHistory.end(), 0.0) / timingHistory.size();
    metrics.avgActiveNeurons = std::accumulate(neuronHistory.begin(), neuronHistory.end(), 0.0) / neuronHistory.size();
    metrics.avgSpikeCount = std::accumulate(spikeHistory.begin(), spikeHistory.end(), 0.0) / spikeHistory.size();
    metrics.avgMemoryUsage = std::accumulate(memoryHistory.begin(), memoryHistory.end(), 0.0) / memoryHistory.size();
    
    // Calculate standard deviations
    double timeVariance = 0.0, neuronVariance = 0.0, spikeVariance = 0.0, memoryVariance = 0.0;
    
    for (size_t i = 0; i < timingHistory.size(); ++i) {
        timeVariance += std::pow(timingHistory[i] - metrics.avgWallTime, 2);
        neuronVariance += std::pow(neuronHistory[i] - metrics.avgActiveNeurons, 2);
        spikeVariance += std::pow(spikeHistory[i] - metrics.avgSpikeCount, 2);
        memoryVariance += std::pow(memoryHistory[i] - metrics.avgMemoryUsage, 2);
    }
    
    metrics.stdWallTime = std::sqrt(timeVariance / timingHistory.size());
    metrics.stdActiveNeurons = std::sqrt(neuronVariance / neuronHistory.size());
    metrics.stdSpikeCount = std::sqrt(spikeVariance / spikeHistory.size());
    metrics.stdMemoryUsage = std::sqrt(memoryVariance / memoryHistory.size());
    
    metrics.stepsRecorded = stepCount;
    metrics.samplingPeriod = timingHistory.size();
    
    return metrics;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    timingHistory.clear();
    neuronHistory.clear();
    spikeHistory.clear();
    memoryHistory.clear();
    stepCount = 0;
}

bool PerformanceMonitor::isReady() const {
    std::lock_guard<std::mutex> lock(mutex);
    return !timingHistory.empty();
}

} // namespace nlm