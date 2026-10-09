#pragma once

#include "PlasticityRule.hpp"
#include <chrono>
#include <unordered_map>
#include <string>

namespace nlm {

// Forward declarations
class Synapse;
class PerformanceProfiler;

// STDP performance metrics
struct STDPPerformanceMetrics {
    // Basic metrics
    size_t ltpEvents{0};                    // Long-term potentiation events
    size_t ltdEvents{0};                    // Long-term depression events
    float ltpWeight{0.01f};                 // LTP weight
    float ltdWeight{0.012f};                // LTD weight
    float timeConstant{20.0f};              // STDP time constant (ms)
    
    // Weight metrics
    SynapticWeight currentWeight{0.0f};
    SynapticWeight averageWeight{0.0f};
    SynapticWeight weightChange{0.0f};
    SynapticWeight weightChangeRate{0.0f};
    SynapticWeight weightVariance{0.0f};
    SynapticWeight minWeight{-1.0f};
    SynapticWeight maxWeight{1.0f};
    
    // Timing metrics
    float averagePrePostInterval{0.0f};    // Average pre-post spike interval
    float averagePostPreInterval{0.0f};   // Average post-pre spike interval
    float spikeSynchronization{0.0f};     // Measure of spike timing correlation
    float crossCorrelation{0.0f};         // Cross-correlation between pre and post spikes
    
    // Plasticity metrics
    float plasticityEfficiency{0.0f};     // Efficiency of plasticity rule
    float learningRate{0.0f};             // Effective learning rate
    float plasticityStrength{0.0f};      // Overall plasticity strength
    
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
    float averageSTDPChange{0.0f};        // Average weight change per spike pair
    float stdDevSTDPChange{0.0f};         // Standard deviation of weight changes
    
    // Custom metrics for profiling
    std::unordered_map<std::string, double> customMetrics;
    
    // Reset metrics
    void reset() {
        ltpEvents = 0;
        ltdEvents = 0;
        ltpWeight = 0.01f;
        ltdWeight = 0.012f;
        timeConstant = 20.0f;
        
        currentWeight = 0.0f;
        averageWeight = 0.0f;
        weightChange = 0.0f;
        weightChangeRate = 0.0f;
        weightVariance = 0.0f;
        minWeight = -1.0f;
        maxWeight = 1.0f;
        
        averagePrePostInterval = 0.0f;
        averagePostPreInterval = 0.0f;
        spikeSynchronization = 0.0f;
        crossCorrelation = 0.0f;
        
        plasticityEfficiency = 0.0f;
        learningRate = 0.0f;
        plasticityStrength = 0.0f;
        
        transmissionSuccessRate = 0.0f;
        compatibility = 0.0f;
        maturity = 0.0f;
        
        lastUpdateTime = std::chrono::steady_clock::now();
        firstEventTime = lastUpdateTime;
        totalActiveTime = 0.0f;
        
        totalSpikePairs = 0;
        averageSTDPChange = 0.0f;
        stdDevSTDPChange = 0.0f;
        
        customMetrics.clear();
    }
    
    // Update metrics
    void recordLTP(float weightChange) {
        ltpEvents++;
        this->weightChange += weightChange;
        updateMetrics();
    }
    
    void recordLTD(float weightChange) {
        ltdEvents++;
        this->weightChange += weightChange;
        updateMetrics();
    }
    
    void updateMetrics() {
        // Update average weight
        if (totalSpikePairs > 0) {
            averageWeight = currentWeight;
        }
        
        // Calculate weight change rate
        if (firstEventTime.time_since_epoch().count() > 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - firstEventTime).count();
            if (elapsed > 0) {
                weightChangeRate = static_cast<float>(weightChange) / elapsed;
            }
        }
        
        // Calculate weight variance
        if (updateCount > 0) {
            weightVariance = (weightVariance * (updateCount - 1) + (currentWeight - averageWeight) * (currentWeight - averageWeight)) / updateCount;
        }
        
        // Update last update time
        lastUpdateTime = std::chrono::steady_clock::now();
        totalActiveTime = std::chrono::duration<double>(lastUpdateTime - firstEventTime).count();
    }
    
    // Helper methods
    float getLearningRate() const {
        return ltpWeight * ltpEvents + ltdWeight * ltdEvents;
    }
    
    float getPlasticityEfficiency() const {
        return (static_cast<float>(ltpEvents) / (static_cast<float>(ltpEvents) + static_cast<float>(ltdEvents))) * 100.0f;
    }
    
    std::unordered_map<std::string, double> getPerformanceSummary() const {
        std::unordered_map<std::string, double> summary;
        
        summary["ltp_events"] = static_cast<double>(ltpEvents);
        summary["ltd_events"] = static_cast<double>(ltdEvents);
        summary["current_weight"] = static_cast<double>(currentWeight);
        summary["average_weight"] = static_cast<double>(averageWeight);
        summary["weight_change"] = static_cast<double>(weightChange);
        summary["weight_change_rate"] = static_cast<double>(weightChangeRate);
        summary["weight_variance"] = static_cast<double>(weightVariance);
        summary["average_pre_post_interval_ms"] = static_cast<double>(averagePrePostInterval);
        summary["average_post_pre_interval_ms"] = static_cast<double>(averagePostPreInterval);
        summary["spike_synchronization"] = static_cast<double>(spikeSynchronization);
        summary["cross_correlation"] = static_cast<double>(crossCorrelation);
        summary["plasticity_efficiency"] = static_cast<double>(plasticityEfficiency);
        summary["learning_rate"] = static_cast<double>(learningRate);
        summary["plasticity_strength"] = static_cast<double>(plasticityStrength);
        summary["transmission_success_rate"] = static_cast<double>(transmissionSuccessRate);
        summary["compatibility"] = static_cast<double>(compatibility);
        summary["maturity"] = static_cast<double>(maturity);
        summary["total_active_time_s"] = totalActiveTime;
        summary["total_spike_pairs"] = static_cast<double>(totalSpikePairs);
        summary["average_stdp_change"] = static_cast<double>(averageSTDPChange);
        summary["stddev_stdp_change"] = static_cast<double>(stdDevSTDPChange);
        
        return summary;
    }
    
private:
    size_t updateCount{0};
};

// STDP configuration
struct STDPConfig {
    float ltpWeight{0.01f};                // Long-term potentiation weight
    float ltdWeight{0.012f};               // Long-term depression weight
    float timeConstant{20.0f};             // STDP time constant (ms)
    bool enableSTDP{true};                 // Enable STDP
    float threshold{0.001f};              // Weight change threshold for learning
    float learningRate{0.01f};             // Learning rate
    float plasticityStrength{0.85f};     // Plasticity strength
    
    // Validation
    bool validate() const {
        return ltpWeight >= 0.0f && ltdWeight >= 0.0f && timeConstant > 0.0f;
    }
};

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Performance monitoring
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    STDPPerformanceMetrics getPerformanceMetrics() const;
    void recordPerformanceMetrics();
    std::unordered_map<std::string, double> getPerformanceSummary() const;
    
    // Configuration
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Performance monitoring integration
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable STDPPerformanceMetrics perfMetrics;
};

// Reward-modulated STDP (R-STDP)
// PLACEHOLDER - Phase 2
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm