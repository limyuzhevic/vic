#include "BrainStatistics.hpp"
#include "../core/Logger/Logger.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

BrainStatistics::BrainStatistics(BrainStatistics&& other) noexcept
    : totalSteps_(other.totalSteps_)
    , totalSpikes_(other.totalSpikes_)
    , averageFiringRate_(other.averageFiringRate_)
    , averageMemoryUsage_(other.averageMemoryUsage_)
    , averageNeuromodulationLevel_(other.averageNeuromodulationLevel_)
    , averagePlasticityChange_(other.averagePlasticityChange_)
    , averageConceptActivation_(other.averageConceptActivation_)
    , developmentalAge_(other.developmentalAge_)
    , spikeHistory_(std::move(other.spikeHistory_))
    , firingRateHistory_(std::move(other.firingRateHistory_))
    , memoryHistory_(std::move(other.memoryHistory_))
    , neuromodulationHistory_(std::move(other.neuromodulationHistory_))
    , plasticityHistory_(std::move(other.plasticityHistory_))
    , cognitionHistory_(std::move(other.cognitionHistory_))
    , developmentHistory_(std::move(other.developmentHistory_))
    , startTime_(std::move(other.startTime_)) {
}

BrainStatistics& BrainStatistics::operator=(BrainStatistics&& other) noexcept {
    if (this != &other) {
        totalSteps_ = other.totalSteps_;
        totalSpikes_ = other.totalSpikes_;
        averageFiringRate_ = other.averageFiringRate_;
        averageMemoryUsage_ = other.averageMemoryUsage_;
        averageNeuromodulationLevel_ = other.averageNeuromodulationLevel_;
        averagePlasticityChange_ = other.averagePlasticityChange_;
        averageConceptActivation_ = other.averageConceptActivation_;
        developmentalAge_ = other.developmentalAge_;
        
        spikeHistory_ = std::move(other.spikeHistory_);
        firingRateHistory_ = std::move(other.firingRateHistory_);
        memoryHistory_ = std::move(other.memoryHistory_);
        neuromodulationHistory_ = std::move(other.neuromodulationHistory_);
        plasticityHistory_ = std::move(other.plasticityHistory_);
        cognitionHistory_ = std::move(other.cognitionHistory_);
        developmentHistory_ = std::move(other.developmentHistory_);
        
        startTime_ = std::move(other.startTime_);
    }
    return *this;
}

void BrainStatistics::recordStep(SimulationStep step, Timestamp time, size_t spikes, float firingRate) {
    totalSteps_++;
    totalSpikes_ += spikes;
    
    // Update averages (exponential moving average for smoothing)
    if (totalSteps_ == 1) {
        averageFiringRate_ = firingRate;
    } else {
        averageFiringRate_ = 0.95f * averageFiringRate_ + 0.05f * firingRate;
    }
    
    // Record history
    spikeHistory_.emplace_back(step, spikes);
    firingRateHistory_.emplace_back(step, firingRate);
    
    // Keep only recent history (last 1000 steps)
    if (spikeHistory_.size() > 1000) {
        spikeHistory_.erase(spikeHistory_.begin());
    }
    if (firingRateHistory_.size() > 1000) {
        firingRateHistory_.erase(firingRateHistory_.begin());
    }
}

void BrainStatistics::recordMemoryEvent(const std::string& event, double value) {
    memoryHistory_.emplace_back(event, value);
    
    // Update average memory usage
    if (memoryHistory_.size() > 0) {
        float sum = 0.0f;
        for (const auto& entry : memoryHistory_) {
            sum += static_cast<float>(entry.second);
        }
        averageMemoryUsage_ = sum / memoryHistory_.size();
    }
    
    // Keep only recent history
    if (memoryHistory_.size() > 1000) {
        memoryHistory_.erase(memoryHistory_.begin());
    }
}

void BrainStatistics::recordNeuromodulation(const std::string& type, float level) {
    neuromodulationHistory_.emplace_back(type, level);
    
    // Update average neuromodulation level
    if (neuromodulationHistory_.size() > 0) {
        float sum = 0.0f;
        for (const auto& entry : neuromodulationHistory_) {
            sum += entry.second;
        }
        averageNeuromodulationLevel_ = sum / neuromodulationHistory_.size();
    }
    
    // Keep only recent history
    if (neuromodulationHistory_.size() > 1000) {
        neuromodulationHistory_.erase(neuromodulationHistory_.begin());
    }
}

void BrainStatistics::recordPlasticityEvent(const std::string& type, float change) {
    plasticityHistory_.emplace_back(type, change);
    
    // Update average plasticity change
    if (plasticityHistory_.size() > 0) {
        float sum = 0.0f;
        for (const auto& entry : plasticityHistory_) {
            sum += entry.second;
        }
        averagePlasticityChange_ = sum / plasticityHistory_.size();
    }
    
    // Keep only recent history
    if (plasticityHistory_.size() > 1000) {
        plasticityHistory_.erase(plasticityHistory_.begin());
    }
}

void BrainStatistics::recordCognitionEvent(const std::string& type, float value) {
    cognitionHistory_.emplace_back(type, value);
    
    // Update average concept activation
    if (cognitionHistory_.size() > 0) {
        float sum = 0.0f;
        for (const auto& entry : cognitionHistory_) {
            sum += entry.second;
        }
        averageConceptActivation_ = sum / cognitionHistory_.size();
    }
    
    // Keep only recent history
    if (cognitionHistory_.size() > 1000) {
        cognitionHistory_.erase(cognitionHistory_.begin());
    }
}

void BrainStatistics::recordDevelopmentEvent(const std::string& type, float value) {
    developmentHistory_.emplace_back(type, value);
    
    // Update developmental age (if applicable)
    if (type == "age_update") {
        developmentalAge_ = value;
    }
    
    // Keep only recent history
    if (developmentHistory_.size() > 1000) {
        developmentHistory_.erase(developmentHistory_.begin());
    }
}

void BrainStatistics::generateReport() const {
    std::stringstream report;
    report << "=== Brain Statistics Report ===" << std::endl;
    report << "Total Steps: " << totalSteps_ << std::endl;
    report << "Total Spikes: " << totalSpikes_ << std::endl;
    report << "Average Firing Rate: " << std::fixed << std::setprecision(2) << averageFiringRate_ << " Hz" << std::endl;
    report << "Average Memory Usage: " << std::fixed << std::setprecision(2) << averageMemoryUsage_ << " traces" << std::endl;
    report << "Average Neuromodulation Level: " << std::fixed << std::setprecision(3) << averageNeuromodulationLevel_ << std::endl;
    report << "Average Plasticity Change: " << std::fixed << std::setprecision(4) << averagePlasticityChange_ << std::endl;
    report << "Average Concept Activation: " << std::fixed << std::setprecision(3) << averageConceptActivation_ << std::endl;
    report << "Developmental Age: " << std::fixed << std::setprecision(1) << developmentalAge_ << " seconds" << std::endl;
    
    report << std::endl;
    report << "=== Recent Spike History ===" << std::endl;
    for (size_t i = std::max<size_t>(0, spikeHistory_.size() - 10); i < spikeHistory_.size(); ++i) {
        report << "Step " << spikeHistory_[i].first << ": " << spikeHistory_[i].second << " spikes" << std::endl;
    }
    
    NLM_LOG_INFO(report.str());
}

void BrainStatistics::generatePerformanceReport() const {
    std::stringstream report;
    report << "=== Brain Performance Report ===" << std::endl;
    report << "Firing Rate Stability: " << calculateFiringRateStability() << std::endl;
    report << "Memory Retention Efficiency: " << calculateMemoryEfficiency() << std::endl;
    report << "Learning Rate: " << calculateLearningRate() << std::endl;
    report << "Adaptation Speed: " << calculateAdaptationSpeed() << std::endl;
    
    report << std::endl;
    report << "=== Performance Metrics ===" << std::endl;
    report << "Spikes per Second: " << std::fixed << std::setprecision(1) << 
        (totalSteps_ > 0 ? static_cast<float>(totalSpikes_) / totalSteps_ : 0.0f) << std::endl;
    report << "Memory Efficiency: " << std::fixed << std::setprecision(3) << 
        (averageMemoryUsage_ > 0 ? 1.0f / averageMemoryUsage_ : 0.0f) << std::endl;
    
    NLM_LOG_INFO(report.str());
}

float BrainStatistics::calculateFiringRateStability() const {
    if (firingRateHistory_.size() < 2) return 1.0f;
    
    float sum = 0.0f;
    float mean = 0.0f;
    
    for (const auto& entry : firingRateHistory_) {
        sum += entry.second;
    }
    mean = sum / firingRateHistory_.size();
    
    float variance = 0.0f;
    for (const auto& entry : firingRateHistory_) {
        float diff = entry.second - mean;
        variance += diff * diff;
    }
    variance /= firingRateHistory_.size();
    
    float stdDev = std::sqrt(variance);
    return (mean > 0.0f) ? (1.0f - stdDev / mean) : 1.0f;
}

float BrainStatistics::calculateMemoryEfficiency() const {
    if (averageMemoryUsage_ == 0.0f) return 0.0f;
    return 1.0f / averageMemoryUsage_;  // Inverse: lower usage = higher efficiency
}

float BrainStatistics::calculateLearningRate() const {
    if (plasticityHistory_.empty()) return 0.0f;
    
    float totalChange = 0.0f;
    for (const auto& entry : plasticityHistory_) {
        totalChange += std::abs(entry.second);
    }
    
    return totalChange / plasticityHistory_.size();
}

float BrainStatistics::calculateAdaptationSpeed() const {
    if (developmentHistory_.empty()) return 0.0f;
    
    // Calculate how quickly developmental age increases
    float ageChange = 0.0f;
    size_t count = 0;
    
    for (const auto& entry : developmentHistory_) {
        if (entry.first == "age_update") {
            ageChange += entry.second;
            count++;
        }
    }
    
    return (count > 0) ? ageChange / count : 0.0f;
}

} // namespace nlm