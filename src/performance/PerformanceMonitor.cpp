#include "PerformanceMonitor.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace nlm {

PerformanceMonitor::PerformanceMonitor()
    : isMonitoring_(false)
    , maxRecentMetrics_(1000)
    , stepCount_(0)
    , totalWallClockTime_(0.0)
    , totalSimulationTime_(0.0)
    , maxFiringRate_(0.0f)
    , minFiringRate_(0.0f)
    , firingRateSum_(0.0f)
    , firingRateStdDev_(0.0f)
    , peakMemoryUsage_(0)
    , averageMemoryUsage_(0.0f) {}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startMonitoring() {
    if (!isMonitoring_) {
        isMonitoring_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceMonitor::stopMonitoring() {
    if (isMonitoring_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime_);
        totalWallClockTime_ += duration.count();
        isMonitoring_ = false;
    }
}

void PerformanceMonitor::recordStep(const PerformanceMetrics& metrics) {
    if (!isMonitoring_) return;
    
    currentMetrics_ = metrics;
    
    // Add to recent metrics
    recentMetrics_.push_back(metrics);
    if (recentMetrics_.size() > maxRecentMetrics_) {
        recentMetrics_.pop_front();
    }
    
    // Update statistics
    stepCount_++;
    totalSimulationTime_ += metrics.simulationTime;
    
    // Update firing rate statistics
    updateFiringRateStats(metrics.firingRate);
    
    // Update memory usage statistics
    updateMemoryUsageStats(metrics.workingMemorySize + metrics.episodicMemoryCount + metrics.associativeMemorySize);
    
    // Update time statistics
    updateTimeStats(metrics.wallClockTime, metrics.simulationTime);
}

float PerformanceMonitor::getAverageFiringRate(size_t steps) const {
    if (recentMetrics_.empty()) return 0.0f;
    
    size_t count = std::min(steps, recentMetrics_.size());
    float sum = 0.0f;
    for (auto it = recentMetrics_.rbegin(); it != recentMetrics_.rbegin() + count; ++it) {
        sum += it->firingRate;
    }
    
    return sum / static_cast<float>(count);
}

float PerformanceMonitor::getAverageReward(size_t steps) const {
    if (recentMetrics_.empty()) return 0.0f;
    
    size_t count = std::min(steps, recentMetrics_.size());
    float sum = 0.0f;
    for (auto it = recentMetrics_.rbegin(); it != recentMetrics_.rbegin() + count; ++it) {
        sum += it->reward;
    }
    
    return sum / static_cast<float>(count);
}

void PerformanceMonitor::updateFiringRateStats(float newFiringRate) {
    firingRateSum_ += newFiringRate;
    
    if (stepCount_ == 1) {
        maxFiringRate_ = newFiringRate;
        minFiringRate_ = newFiringRate;
    } else {
        maxFiringRate_ = std::max(maxFiringRate_, newFiringRate);
        minFiringRate_ = std::min(minFiringRate_, newFiringRate);
    }
    
    // Update standard deviation
    if (stepCount_ > 1) {
        float mean = firingRateSum_ / static_cast<float>(stepCount_);
        float variance = 0.0f;
        
        for (const auto& metrics : recentMetrics_) {
            float diff = metrics.firingRate - mean;
            variance += diff * diff;
        }
        
        variance /= static_cast<float>(stepCount_);
        firingRateStdDev_ = std::sqrt(variance);
    }
}

void PerformanceMonitor::updateMemoryUsageStats(size_t memoryUsage) {
    if (memoryUsage > peakMemoryUsage_) {
        peakMemoryUsage_ = memoryUsage;
    }
    
    averageMemoryUsage_ = (averageMemoryUsage_ * static_cast<float>(stepCount_ - 1) + static_cast<float>(memoryUsage)) / static_cast<float>(stepCount_);
}

void PerformanceMonitor::updateTimeStats(double wallClockTime, double simulationTime) {
    totalWallClockTime_ += wallClockTime;
    totalSimulationTime_ += simulationTime;
}

std::string PerformanceMonitor::getSummary() const {
    std::stringstream ss;
    ss << "=== Performance Summary ===" << std::endl;
    ss << "Total Steps: " << stepCount_ << std::endl;
    ss << "Average Firing Rate: " << getAverageFiringRate() << " Hz" << std::endl;
    ss << "Max Firing Rate: " << maxFiringRate_ << " Hz" << std::endl;
    ss << "Min Firing Rate: " << minFiringRate_ << " Hz" << std::endl;
    ss << "Firing Rate StdDev: " << firingRateStdDev_ << " Hz" << std::endl;
    ss << "Average Reward: " << getAverageReward() << std::endl;
    ss << "Total Wall Clock Time: " << totalWallClockTime_ << " s" << std::endl;
    ss << "Total Simulation Time: " << totalSimulationTime_ << " s" << std::endl;
    ss << "Peak Memory Usage: " << peakMemoryUsage_ << " units" << std::endl;
    ss << "Average Memory Usage: " << averageMemoryUsage_ << " units" << std::endl;
    return ss.str();
}

std::string PerformanceMonitor::getDetailedStats() const {
    std::stringstream ss;
    ss << "=== Detailed Performance Statistics ===" << std::endl;
    ss << "Step Count: " << stepCount_ << std::endl;
    ss << std::endl;
    
    // Current metrics
    ss << "Current Metrics:" << std::endl;
    ss << "  Wall Clock Time: " << currentMetrics_.wallClockTime << " s" << std::endl;
    ss << "  Simulation Time: " << currentMetrics_.simulationTime << " s" << std::endl;
    ss << "  Firing Rate: " << currentMetrics_.firingRate << " Hz" << std::endl;
    ss << "  Firing Neuron Count: " << currentMetrics_.firingNeuronCount << std::endl;
    ss << "  Total Spike Count: " << currentMetrics_.totalSpikeCount << std::endl;
    ss << "  Working Memory Size: " << currentMetrics_.workingMemorySize << std::endl;
    ss << "  Episodic Memory Count: " << currentMetrics_.episodicMemoryCount << std::endl;
    ss << "  Associative Memory Size: " << currentMetrics_.associativeMemorySize << std::endl;
    ss << "  Dopamine Level: " << currentMetrics_.dopamineLevel << std::endl;
    ss << "  Novelty Level: " << currentMetrics_.noveltyLevel << std::endl;
    ss << "  Curiosity Level: " << currentMetrics_.curiosityLevel << std::endl;
    ss << "  Prediction Error: " << currentMetrics_.predictionError << std::endl;
    ss << "  Learning Rate: " << currentMetrics_.learningRate << std::endl;
    ss << "  Plasticity Change: " << currentMetrics_.plasticityChange << std::endl;
    ss << "  Reward: " << currentMetrics_.reward << std::endl;
    ss << "  Energy: " << currentMetrics_.energy << std::endl;
    ss << "  Action Count: " << currentMetrics_.actionCount << std::endl;
    ss << std::endl;
    
    // Recent metrics summary
    ss << "Recent Metrics (last " << std::min(recentMetrics_.size(), size_t(10)) << " steps):" << std::endl;
    for (auto it = recentMetrics_.rbegin(); it != recentMetrics_.rend() && it != recentMetrics_.rbegin() + 10; ++it) {
        ss << "  Step " << (stepCount_ - (std::distance(recentMetrics_.rbegin(), it) + 1)) << 
            ": Firing Rate = " << it->firingRate << ", Reward = " << it->reward << ", Novelty = " << it->noveltyLevel << std::endl;
    }
    
    return ss.str();
}

void PerformanceMonitor::reset() {
    isMonitoring_ = false;
    stepCount_ = 0;
    totalWallClockTime_ = 0.0;
    totalSimulationTime_ = 0.0;
    maxFiringRate_ = 0.0f;
    minFiringRate_ = 0.0f;
    firingRateSum_ = 0.0f;
    firingRateStdDev_ = 0.0f;
    peakMemoryUsage_ = 0;
    averageMemoryUsage_ = 0.0f;
    recentMetrics_.clear();
}

std::vector<PerformanceMetrics> PerformanceMonitor::getRecentMetrics(size_t steps) const {
    std::vector<PerformanceMetrics> result;
    size_t count = std::min(steps, recentMetrics_.size());
    
    for (auto it = recentMetrics_.rbegin(); it != recentMetrics_.rbegin() + count; ++it) {
        result.push_back(*it);
    }
    
    return result;
}

float PerformanceMonitor::calculateLearningProgress(size_t window) const {
    if (recentMetrics_.size() < window * 2) return 0.0f;
    
    // Calculate the average reward over the first half and second half of the window
    size_t half = window / 2;
    float firstHalfSum = 0.0f;
    float secondHalfSum = 0.0f;
    
    for (size_t i = 0; i < half; ++i) {
        firstHalfSum += recentMetrics_.at(recentMetrics_.size() - 1 - i).reward;
    }
    
    for (size_t i = half; i < window; ++i) {
        secondHalfSum += recentMetrics_.at(recentMetrics_.size() - 1 - i).reward;
    }
    
    float firstHalfAvg = firstHalfSum / static_cast<float>(half);
    float secondHalfAvg = secondHalfSum / static_cast<float>(window - half);
    
    return secondHalfAvg - firstHalfAvg;
}

} // namespace nlm
