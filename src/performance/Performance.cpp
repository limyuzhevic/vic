#include "Performance.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

PerformanceStatistics::PerformanceStatistics()
    : totalSamples_(0)
    , maxSamples_(1000)
    , spikeCount_(0)
    , neuronCount_(0)
    , activeNeurons_(0)
    , firingNeurons_(0)
    , averageFiringRate_(0.0f)
    , averageStepTime_(0.0f)
    , minStepTime_(FLT_MAX)
    , maxStepTime_(0.0f)
    , totalWallClockTime_(0.0)
    , cpuUsage_(0.0f)
    , memoryUsage_(0.0f)
    , energyConsumption_(0.0f)
    , thermalLoad_(0.0f)
    , accuracy_(0.0f)
    , convergenceRate_(0.0f)
    , learningRate_(0.0f)
    , explorationRate_(0.0f)
    , rewardSignal_(0.0f)
    , predictionError_(0.0f)
    , noveltyScore_(0.0f)
    , curiosityDrive_(0.0f)
    , dopamineLevel_(0.0f)
    , serotoninLevel_(0.0f)
    , norepinephrineLevel_(0.0f)
    , acetylcholineLevel_(0.0f)
    , coherenceScore_(0.0f)
    , stabilityScore_(0.0f)
    , complexityScore_(0.0f)
    , adaptabilityScore_(0.0f)
{
}

PerformanceStatistics::~PerformanceStatistics() = default;

void PerformanceStatistics::addSample(const PerformanceSample& sample) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (totalSamples_ >= maxSamples_) {
        // Remove oldest sample
        samples_.pop_front();
        totalSamples_--;
    }
    
    samples_.push_back(sample);
    totalSamples_++;
    
    // Update running statistics
    spikeCount_ = 0;
    neuronCount_ = 0;
    activeNeurons_ = 0;
    firingNeurons_ = 0;
    
    double sumFiringRate = 0.0;
    double sumStepTime = 0.0;
    
    for (const auto& s : samples_) {
        spikeCount_ += s.spikeCount;
        neuronCount_ = std::max(neuronCount_, s.neuronCount);
        activeNeurons_ = std::max(activeNeurons_, s.activeNeurons);
        firingNeurons_ = std::max(firingNeurons_, s.firingNeurons);
        
        sumFiringRate += s.averageFiringRate;
        sumStepTime += s.stepTimeMs;
    }
    
    averageFiringRate_ = totalSamples_ > 0 ? sumFiringRate / totalSamples_ : 0.0f;
    averageStepTime_ = totalSamples_ > 0 ? sumStepTime / totalSamples_ : 0.0f;
    
    // Update extremes
    if (!samples_.empty()) {
        minStepTime_ = samples_.front().stepTimeMs;
        maxStepTime_ = samples_.front().stepTimeMs;
        
        for (const auto& s : samples_) {
            minStepTime_ = std::min(minStepTime_, s.stepTimeMs);
            maxStepTime_ = std::max(maxStepTime_, s.stepTimeMs);
        }
    }
    
    // Update other metrics
    updateDerivedMetrics();
}

PerformanceSample PerformanceStatistics::getLastSample() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (samples_.empty()) {
        return PerformanceSample();
    }
    return samples_.back();
}

PerformanceSample PerformanceStatistics::getOldestSample() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (samples_.empty()) {
        return PerformanceSample();
    }
    return samples_.front();
}

PerformanceSample PerformanceStatistics::getSample(size_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (index >= samples_.size()) {
        return PerformanceSample();
    }
    
    auto it = samples_.begin();
    std::advance(it, index);
    return *it;
}

PerformanceSample PerformanceStatistics::getSampleByStep(size_t step) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& sample : samples_) {
        if (sample.stepNumber == step) {
            return sample;
        }
    }
    
    return PerformanceSample();
}

PerformanceSample PerformanceStatistics::getSampleByTime(double time) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& sample : samples_) {
        if (sample.timestamp >= time) {
            return sample;
        }
    }
    
    return PerformanceSample();
}

std::vector<PerformanceSample> PerformanceStatistics::getSamplesByTimeRange(double startTime, double endTime) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceSample> result;
    for (const auto& sample : samples_) {
        if (sample.timestamp >= startTime && sample.timestamp <= endTime) {
            result.push_back(sample);
        }
    }
    
    return result;
}

std::vector<PerformanceSample> PerformanceStatistics::getSamplesByStepRange(size_t startStep, size_t endStep) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceSample> result;
    for (const auto& sample : samples_) {
        if (sample.stepNumber >= startStep && sample.stepNumber <= endStep) {
            result.push_back(sample);
        }
    }
    
    return result;
}

std::vector<PerformanceSample> PerformanceStatistics::getAllSamples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<PerformanceSample>(samples_.begin(), samples_.end());
}

void PerformanceStatistics::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    samples_.clear();
    totalSamples_ = 0;
    spikeCount_ = 0;
    neuronCount_ = 0;
    activeNeurons_ = 0;
    firingNeurons_ = 0;
    averageFiringRate_ = 0.0f;
    averageStepTime_ = 0.0f;
    minStepTime_ = FLT_MAX;
    maxStepTime_ = 0.0f;
    totalWallClockTime_ = 0.0;
    cpuUsage_ = 0.0f;
    memoryUsage_ = 0.0f;
    energyConsumption_ = 0.0f;
    thermalLoad_ = 0.0f;
    accuracy_ = 0.0f;
    convergenceRate_ = 0.0f;
    learningRate_ = 0.0f;
    explorationRate_ = 0.0f;
    rewardSignal_ = 0.0f;
    predictionError_ = 0.0f;
    noveltyScore_ = 0.0f;
    curiosityDrive_ = 0.0f;
    dopamineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    norepinephrineLevel_ = 0.0f;
    acetylcholineLevel_ = 0.0f;
    coherenceScore_ = 0.0f;
    stabilityScore_ = 0.0f;
    complexityScore_ = 0.0f;
    adaptabilityScore_ = 0.0f;
}

bool PerformanceStatistics::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return samples_.empty();
}

size_t PerformanceStatistics::getSampleCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return samples_.size();
}

size_t PerformanceStatistics::getTotalSamples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalSamples_;
}

size_t PerformanceStatistics::getMaxSamples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return maxSamples_;
}

void PerformanceStatistics::setMaxSamples(size_t max) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxSamples_ = max;
    
    // Trim samples if necessary
    while (totalSamples_ > maxSamples_) {
        samples_.pop_front();
        totalSamples_--;
    }
}

double PerformanceStatistics::getAverageStepTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return averageStepTime_;
}

double PerformanceStatistics::getMinStepTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return minStepTime_;
}

double PerformanceStatistics::getMaxStepTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return maxStepTime_;
}

double PerformanceStatistics::getTotalWallClockTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalWallClockTime_;
}

void PerformanceStatistics::addWallClockTime(double time) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalWallClockTime_ += time;
}

float PerformanceStatistics::getAverageFiringRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return averageFiringRate_;
}

size_t PerformanceStatistics::getSpikeCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return spikeCount_;
}

size_t PerformanceStatistics::getNeuronCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return neuronCount_;
}

size_t PerformanceStatistics::getActiveNeurons() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activeNeurons_;
}

size_t PerformanceStatistics::getFiringNeurons() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return firingNeurons_;
}

void PerformanceStatistics::setStepTime(double timeMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    double avg = averageStepTime_;
    size_t count = totalSamples_;
    
    if (count > 0) {
        averageStepTime_ = (avg * count + timeMs) / (count + 1);
    } else {
        averageStepTime_ = timeMs;
    }
    
    minStepTime_ = std::min(minStepTime_, timeMs);
    maxStepTime_ = std::max(maxStepTime_, timeMs);
}

void PerformanceStatistics::updateDerivedMetrics() {
    // Update derived metrics based on current samples
    // This is a placeholder implementation
    
    if (totalSamples_ > 0) {
        double avgFiringRate = averageFiringRate_;
        double avgStepTime = averageStepTime_;
        
        // Calculate derived metrics
        // These are just placeholder calculations
        cpuUsage_ = std::min(100.0f, avgFiringRate_ * 50.0f);  // Example: 50% firing rate = 50% CPU
        memoryUsage_ = avgFiringRate_ * 0.1f;  // Example: 10% of firing rate
        energyConsumption_ = avgStepTime_ * 0.01f;  // Example: 0.01 energy per ms
        thermalLoad_ = cpuUsage_ * 0.8f;  // Example: 80% thermal load at 100% CPU
        accuracy_ = std::min(1.0f, avgFiringRate_ / 10.0f);  // Example: higher firing rate = higher accuracy
        
        // Neuromodulator levels (placeholder)
        dopamineLevel_ = avgFiringRate_ * 0.3f;
        serotoninLevel_ = avgFiringRate_ * 0.2f;
        norepinephrineLevel_ = avgFiringRate_ * 0.25f;
        acetylcholineLevel_ = avgFiringRate_ * 0.2f;
        
        // System scores (placeholder)
        coherenceScore_ = accuracy_ * 0.7f + (1.0f - avgStepTime_ / maxStepTime_) * 0.3f;
        stabilityScore_ = 1.0f - (maxStepTime_ - minStepTime_) / maxStepTime_;
        complexityScore_ = std::log2(neuronCount_) / 20.0f;  // Example scaling
        adaptabilityScore_ = avgFiringRate_ * 0.5f + (1.0f - cpuUsage_ / 100.0f) * 0.5f;
    }
}

std::string PerformanceStatistics::generateReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "=== NLM Performance Report ===" << std::endl;
    ss << "Total Samples: " << totalSamples_ << std::endl;
    ss << "Spike Count: " << spikeCount_ << std::endl;
    ss << "Neuron Count: " << neuronCount_ << std::endl;
    ss << "Active Neurons: " << activeNeurons_ << std::endl;
    ss << "Firing Neurons: " << firingNeurons_ << std::endl;
    ss << "Average Firing Rate: " << averageFiringRate_ << std::endl;
    ss << "Average Step Time: " << averageStepTime_ << " ms" << std::endl;
    ss << "Min Step Time: " << minStepTime_ << " ms" << std::endl;
    ss << "Max Step Time: " << maxStepTime_ << " ms" << std::endl;
    ss << "Total Wall Clock Time: " << totalWallClockTime_ << " s" << std::endl;
    ss << "CPU Usage: " << cpuUsage_ << "%" << std::endl;
    ss << "Memory Usage: " << memoryUsage_ << " MB" << std::endl;
    ss << "Energy Consumption: " << energyConsumption_ << " J" << std::endl;
    ss << "Thermal Load: " << thermalLoad_ << "%" << std::endl;
    ss << "Accuracy: " << accuracy_ << std::endl;
    ss << "Coherence Score: " << coherenceScore_ << std::endl;
    ss << "Stability Score: " << stabilityScore_ << std::endl;
    ss << "Complexity Score: " << complexityScore_ << std::endl;
    ss << "Adaptability Score: " << adaptabilityScore_ << std::endl;
    ss << "Novelty Score: " << noveltyScore_ << std::endl;
    ss << "Curiosity Drive: " << curiosityDrive_ << std::endl;
    ss << "Dopamine Level: " << dopamineLevel_ << std::endl;
    ss << "Serotonin Level: " << serotoninLevel_ << std::endl;
    ss << "Norepinephrine Level: " << norepinephrineLevel_ << std::endl;
    ss << "Acetylcholine Level: " << acetylcholineLevel_ << std::endl;
    
    return ss.str();
}

} // namespace nlm