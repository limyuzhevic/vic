#include "core/PerformanceMetrics/PerformanceMetrics.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() 
    : running_(false)
    , avgStepTimeMs_(0.0)
    , avgSpikesPerStep_(0.0)
    , avgActiveNeurons_(0.0)
    , totalSteps_(0)
    , totalSpikes_(0) {
}

PerformanceMonitor::~PerformanceMonitor() {
}

void PerformanceMonitor::start() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
    currentMetrics_.startTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

void PerformanceMonitor::stop() {
    if (running_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        currentMetrics_.totalExecutionTimeMs = static_cast<double>(duration.count());
        currentMetrics_.endTime = std::chrono::duration_cast<std::chrono::seconds>(
            endTime.time_since_epoch()
        ).count();
        running_ = false;
    }
}

void PerformanceMonitor::recordStep(double stepTimeMs, uint64_t spikes, uint64_t activeNeurons) {
    stepTimes_.push_back(stepTimeMs);
    spikeCountsPerStep_.push_back(spikes);
    activeNeuronCountsPerStep_.push_back(activeNeurons);
    
    totalSteps_++;
    totalSpikes_ += spikes;
    
    // Update running averages
    if (totalSteps_ > 0) {
        // Simple moving average approach
        avgStepTimeMs_ = ((avgStepTimeMs_ * (totalSteps_ - 1)) + stepTimeMs) / totalSteps_;
        avgSpikesPerStep_ = ((avgSpikesPerStep_ * (totalSteps_ - 1)) + static_cast<double>(spikes)) / totalSteps_;
        avgActiveNeurons_ = ((avgActiveNeurons_ * (totalSteps_ - 1)) + static_cast<double>(activeNeurons)) / totalSteps_;
    }
    
    // Record in current metrics
    currentMetrics_.stepTimesMs.push_back(stepTimeMs);
    currentMetrics_.spikeCountsPerStep.push_back(spikes);
    currentMetrics_.activeNeuronCountsPerStep.push_back(activeNeurons);
}

void PerformanceMonitor::recordPlasticityTime(double timeMs, uint64_t events) {
    currentMetrics_.plasticityComputationTimeMs += timeMs;
    currentMetrics_.plasticityEvents += events;
}

void PerformanceMonitor::recordPredictionTime(double timeMs) {
    currentMetrics_.predictionComputationTimeMs += timeMs;
    currentMetrics_.predictionSteps++;
}

void PerformanceMonitor::recordComponentTime(const std::string& component, double timeMs) {
    currentMetrics_.componentTimesMs[component] += timeMs;
    componentTimes_[component].push_back(timeMs);
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    PerformanceMetrics metrics = currentMetrics_;
    metrics.calculateDerivedMetrics();
    return metrics;
}

PerformanceMetrics PerformanceMonitor::getSnapshot() const {
    return currentMetrics_;
}

void PerformanceMonitor::reset() {
    stepTimes_.clear();
    spikeCountsPerStep_.clear();
    activeNeuronCountsPerStep_.clear();
    
    for (auto& [component, times] : componentTimes_) {
        times.clear();
    }
    
    avgStepTimeMs_ = 0.0;
    avgSpikesPerStep_ = 0.0;
    avgActiveNeurons_ = 0.0;
    totalSteps_ = 0;
    totalSpikes_ = 0;
    
    currentMetrics_.reset();
}

bool PerformanceMonitor::isStable(size_t windowSize, double threshold) const {
    if (stepTimes_.size() < windowSize) {
        return false;
    }
    
    // Check last N steps
    size_t startIdx = stepTimes_.size() - windowSize;
    double avgTime = 0.0;
    for (size_t i = startIdx; i < stepTimes_.size(); ++i) {
        avgTime += stepTimes_[i];
    }
    avgTime /= static_cast<double>(windowSize);
    
    // Check variation
    double varianceSum = 0.0;
    for (size_t i = startIdx; i < stepTimes_.size(); ++i) {
        double diff = stepTimes_[i] - avgTime;
        varianceSum += diff * diff;
    }
    double stdDev = std::sqrt(varianceSum / static_cast<double>(windowSize - 1));
    
    return (stdDev / avgTime) < threshold;
}

} // namespace nlm
