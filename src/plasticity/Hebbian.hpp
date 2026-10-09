#pragma once

#include "PlasticityRule.hpp"
#include <chrono>
#include <unordered_map>
#include <string>

namespace nlm {

// Forward declarations
class Synapse;
class PerformanceProfiler;

// Hebbian performance metrics
struct HebbianPerformanceMetrics {
    // Basic metrics
    size_t totalHebbianEvents{0};           // Total Hebbian learning events
    float learningRate{0.01f};              // Learning rate
    float maxWeight{1.0f};                  // Maximum weight
    float minWeight{-1.0f};                 // Minimum weight
    
    // Weight metrics
    SynapticWeight currentWeight{0.0f};
    SynapticWeight averageWeight{0.0f};
    SynapticWeight weightChange{0.0f};
    SynapticWeight weightChangeRate{0.0f};
    SynapticWeight weightVariance{0.0f};
    SynapticWeight minObservedWeight{-1.0f};
    SynapticWeight maxObservedWeight{1.0f};
    
    // Spike metrics
    size_t preSpikeCount{0};                // Number of pre-synaptic spikes
    size_t postSpikeCount{0};               // Number of post-synaptic spikes
    float averageFiringRate{0.0f};         // Average firing rate
    float firingRateVariability{0.0f};    // Variability in firing rate
    float spikeSynchronization{0.0f};     // Measure of spike timing correlation
    
    // Correlation metrics
    float covariance{0.0f};                // Covariance between pre and post spikes
    float correlationCoefficient{0.0f};   // Correlation coefficient
    
    // Plasticity metrics
    float plasticityEfficiency{0.0f};     // Efficiency of Hebbian learning
    float learningEffectiveness{0.0f};    // Effectiveness of learning
    float weightStability{0.0f};          // Stability of synaptic weights
    
    // Performance metrics
    float transmissionSuccessRate{0.0f};  // Success rate of synaptic transmission
    float compatibility{0.0f};            // Compatibility with other plasticity rules
    float maturity{0.0f};                 // Maturity of the synapse
    
    // Temporal metrics
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::chrono::steady_clock::time_point firstEventTime;
    float totalActiveTime{0.0f};          // Total time synapse was active
    
    // Statistical metrics
    size_t totalSpikePairs{0};            // Total spike pairs processed
    float averageHebbianChange{0.0f};     // Average weight change per spike pair
    float stdDevHebbianChange{0.0f};     // Standard deviation of weight changes
    
    // Custom metrics for profiling
    std::unordered_map<std::string, double> customMetrics;
    
    // Reset metrics
    void reset() {
        totalHebbianEvents = 0;
        learningRate = 0.01f;
        maxWeight = 1.0f;
        minWeight = -1.0f;
        
        currentWeight = 0.0f;
        averageWeight = 0.0f;
        weightChange = 0.0f;
        weightChangeRate = 0.0f;
        weightVariance = 0.0f;
        minObservedWeight = -1.0f;
        maxObservedWeight = 1.0f;
        
        preSpikeCount = 0;
        postSpikeCount = 0;
        averageFiringRate = 0.0f;
        firingRateVariability = 0.0f;
        spikeSynchronization = 0.0f;
        
        covariance = 0.0f;
        correlationCoefficient = 0.0f;
        
        plasticityEfficiency = 0.0f;
        learningEffectiveness = 0.0f;
        weightStability = 0.0f;
        
        transmissionSuccessRate = 0.0f;
        compatibility = 0.0f;
        maturity = 0.0f;
        
        lastUpdateTime = std::chrono::steady_clock::now();
        firstEventTime = lastUpdateTime;
        totalActiveTime = 0.0f;
        
        totalSpikePairs = 0;
        averageHebbianChange = 0.0f;
        stdDevHebbianChange = 0.0f;
        
        customMetrics.clear();
    }
    
    // Update metrics
    void recordHebbianEvent(float weightChange) {
        totalHebbianEvents++;
        this->weightChange += weightChange;
        updateMetrics();
    }
    
    void updateMetrics() {
        // Update average weight
        if (totalSpikePairs > 0) {
            averageWeight = currentWeight;
        }
        
        // Calculate weight variance
        if (updateCount > 0) {
            weightVariance = (weightVariance * (updateCount - 1) + (currentWeight - averageWeight) * (currentWeight - averageWeight)) / updateCount;
        }
        
        // Update last update time
        lastUpdateTime = std::chrono::steady_clock::now();
        totalActiveTime = std::chrono::duration<double>(lastUpdateTime - firstEventTime).count();
        
        updateCount++;
    }
    
    // Helper methods
    float getLearningEffectiveness() const {
        return (averageHebbianChange > 0.0f) ? std::min(100.0f, averageHebbianChange * 1000.0f) : 0.0f;
    }
    
    float getWeightStability() const {
        return std::max(0.0f, 1.0f - (weightVariance / (maxObservedWeight - minObservedWeight)));
    }
    
    std::unordered_map<std::string, double> getPerformanceSummary() const {
        std::unordered_map<std::string, double> summary;
        
        summary["hebbian_events"] = static_cast<double>(totalHebbianEvents);
        summary["learning_rate"] = static_cast<double>(learningRate);
        summary["current_weight"] = static_cast<double>(currentWeight);
        summary["average_weight"] = static_cast<double>(averageWeight);
        summary["weight_change"] = static_cast<double>(weightChange);
        summary["weight_change_rate"] = static_cast<double>(weightChangeRate);
        summary["weight_variance"] = static_cast<double>(weightVariance);
        summary["average_firing_rate_hz"] = static_cast<double>(averageFiringRate);
        summary["firing_rate_variability"] = static_cast<double>(firingRateVariability);
        summary["spike_synchronization"] = static_cast<double>(spikeSynchronization);
        summary["correlation_coefficient"] = static_cast<double>(correlationCoefficient);
        summary["plasticity_efficiency"] = static_cast<double>(plasticityEfficiency);
        summary["learning_effectiveness"] = static_cast<double>(learningEffectiveness);
        summary["weight_stability"] = static_cast<double>(weightStability);
        summary["transmission_success_rate"] = static_cast<double>(transmissionSuccessRate);
        summary["compatibility"] = static_cast<double>(compatibility);
        summary["maturity"] = static_cast<double>(maturity);
        summary["total_active_time_s"] = totalActiveTime;
        summary["total_spike_pairs"] = static_cast<double>(totalSpikePairs);
        summary["average_hebbian_change"] = static_cast<double>(averageHebbianChange);
        summary["stddev_hebbian_change"] = static_cast<double>(stdDevHebbianChange);
        
        return summary;
    }
    
private:
    size_t updateCount{0};
};

// Hebbian configuration
struct HebbianConfig {
    float learningRate{0.01f};             // Learning rate
    float maxWeight{1.0f};                 // Maximum weight
    float minWeight{-1.0f};                // Minimum weight
    float covarianceThreshold{0.0f};      // Covariance threshold for learning
    bool enableHebbian{true};              // Enable Hebbian learning
    float learningRateDecay{0.0f};        // Learning rate decay per unit time
    float weightChangeThreshold{0.001f};  // Weight change threshold
    
    // Validation
    bool validate() const {
        return learningRate >= 0.0f && maxWeight > minWeight && timeConstant > 0.0f;
    }
};

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Performance monitoring
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    HebbianPerformanceMetrics getPerformanceMetrics() const;
    void recordPerformanceMetrics();
    std::unordered_map<std::string, double> getPerformanceSummary() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Performance monitoring integration
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable HebbianPerformanceMetrics perfMetrics;
};

} // namespace nlm