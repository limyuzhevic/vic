#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <unordered_map>
#include <string>

namespace nlm {

// Forward declarations
class Neuron;
class PerformanceProfiler;

// Synapse performance metrics
struct SynapsePerformanceMetrics {
    // Identity
    SynapseId id{0};
    NeuronId source{0};
    NeuronId destination{0};
    
    // Weight metrics
    SynapticWeight weight{0.0f};
    SynapticWeight averageWeight{0.0f};
    SynapticWeight weightChange{0.0f};
    SynapticWeight weightChangeRate{0.0f};
    SynapticWeight weightVariance{0.0f};
    SynapticWeight minWeight{0.0f};
    SynapticWeight maxWeight{0.0f};
    
    // Transmission metrics
    float transmissionDelay{0.0f};
    float averageTransmissionDelay{0.0f};
    size_t totalTransmissions{0};
    float transmissionSuccessRate{0.0f};
    size_t failedTransmissions{0};
    
    // Plasticity metrics
    float eligibilityTrace{0.0f};
    float eligibilityTraceDecay{0.0f};
    float efficacy{0.0f};
    float plasticityStrength{0.0f};
    
    // Activity metrics
    size_t preSpikeCount{0};
    size_t postSpikeCount{0};
    float spikeSynchronization{0.0f};
    float crossCorrelation{0.0f};
    float burstFrequency{0.0f};
    
    // Type metrics
    SynapseType type{SynapseType::Excitatory};
    bool isExcitatory{true};
    bool isInhibitory{false};
    
    // Performance metrics
    float efficiency{0.0f};
    float reliability{0.0f};
    float compatibility{0.0f};
    float maturity{0.0f};
    
    // Temporal metrics
    std::chrono::steady_clock::time_point lastUpdateTime;
    float timeSinceLastSpike{0.0f};
    float averageInterSpikeInterval{0.0f};
    float isiCoefficientOfVariation{0.0f};
    
    // Plasticity flags
    bool hebbianEnabled{false};
    bool stdpEnabled{false};
    bool rewardModulated{false};
    
    // Counters
    size_t updateCount{0};
    size_t stepCount{0};
    
    // Custom metrics for profiling
    std::unordered_map<std::string, double> customMetrics;
    
    // Reset metrics
    void reset() {
        weight = 0.0f;
        averageWeight = 0.0f;
        weightChange = 0.0f;
        weightChangeRate = 0.0f;
        weightVariance = 0.0f;
        minWeight = 0.0f;
        maxWeight = 0.0f;
        
        transmissionDelay = 0.0f;
        averageTransmissionDelay = 0.0f;
        totalTransmissions = 0;
        transmissionSuccessRate = 0.0f;
        failedTransmissions = 0;
        
        eligibilityTrace = 0.0f;
        eligibilityTraceDecay = 0.0f;
        efficacy = 0.0f;
        plasticityStrength = 0.0f;
        
        preSpikeCount = 0;
        postSpikeCount = 0;
        spikeSynchronization = 0.0f;
        crossCorrelation = 0.0f;
        burstFrequency = 0.0f;
        
        type = SynapseType::Excitatory;
        isExcitatory = true;
        isInhibitory = false;
        
        efficiency = 0.0f;
        reliability = 0.0f;
        compatibility = 0.0f;
        maturity = 0.0f;
        
        timeSinceLastSpike = 0.0f;
        averageInterSpikeInterval = 0.0f;
        isiCoefficientOfVariation = 0.0f;
        
        hebbianEnabled = false;
        stdpEnabled = false;
        rewardModulated = false;
        
        updateCount = 0;
        stepCount = 0;
        
        customMetrics.clear();
        
        lastUpdateTime = std::chrono::steady_clock::now();
    }
    
    // Update metrics
    void updateWeight(SynapticWeight newWeight) {
        weightChange = newWeight - weight;
        weight = newWeight;
        
        if (weight > maxWeight) maxWeight = weight;
        if (weight < minWeight) minWeight = weight;
        
        if (updateCount > 0) {
            weightVariance = (weightVariance * (updateCount - 1) + (weight - averageWeight) * (weight - averageWeight)) / static_cast<float>(updateCount);
        }
    }
    
    void updateTransmission(float delay, bool success) {
        transmissionDelay = delay;
        averageTransmissionDelay = (averageTransmissionDelay * totalTransmissions + delay) / (totalTransmissions + 1);
        totalTransmissions++;
        
        if (success) {
            transmissionSuccessRate = static_cast<float>(totalTransmissions - failedTransmissions) / totalTransmissions;
        }
        
        if (!success) {
            failedTransmissions++;
            transmissionSuccessRate = static_cast<float>(totalTransmissions - failedTransmissions) / totalTransmissions;
        }
    }
    
    void updateEfficacy(float eff) {
        efficacy = eff;
        reliability = (efficacy + reliability) / 2.0f;
    }
    
    void updatePlasticity(float trace, float strength) {
        eligibilityTrace = trace;
        plasticityStrength = strength;
    }
    
    void updateSpikeActivity(bool preSpike, bool postSpike, float crossCorr, float burstFreq) {
        if (preSpike) preSpikeCount++;
        if (postSpike) postSpikeCount++;
        
        crossCorrelation = crossCorr;
        burstFrequency = burstFreq;
        
        if (preSpikeCount > 0 && postSpikeCount > 0) {
            spikeSynchronization = crossCorr;
        }
    }
    
    std::unordered_map<std::string, double> getPerformanceSummary() const {
        std::unordered_map<std::string, double> summary;
        
        summary["synapse_id"] = static_cast<double>(id);
        summary["source_neuron"] = static_cast<double>(source);
        summary["destination_neuron"] = static_cast<double>(destination);
        summary["weight"] = static_cast<double>(weight);
        summary["weight_change"] = static_cast<double>(weightChange);
        summary["weight_change_rate"] = static_cast<double>(weightChangeRate);
        summary["weight_variance"] = static_cast<double>(weightVariance);
        summary["transmission_delay_ms"] = static_cast<double>(transmissionDelay);
        summary["transmission_success_rate"] = static_cast<double>(transmissionSuccessRate);
        summary["efficacy"] = static_cast<double>(efficacy);
        summary["plasticity_strength"] = static_cast<double>(plasticityStrength);
        summary["spike_synchronization"] = static_cast<double>(spikeSynchronization);
        summary["cross_correlation"] = static_cast<double>(crossCorrelation);
        summary["burst_frequency_hz"] = static_cast<double>(burstFrequency);
        summary["efficiency"] = static_cast<double>(efficiency);
        summary["reliability"] = static_cast<double>(reliability);
        summary["compatibility"] = static_cast<double>(compatibility);
        summary["maturity"] = static_cast<double>(maturity);
        summary["isi_coefficient_of_variation"] = static_cast<double>(isiCoefficientOfVariation);
        
        return summary;
    }
};

// Synapse class representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity
class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement real synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Performance monitoring
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    SynapsePerformanceMetrics getPerformanceMetrics() const;
    void recordPerformanceMetrics();
    std::unordered_map<std::string, double> getPerformanceSummary() const;
    
    // Helper methods for profiling
    void recordStepMetrics();
    void recordSpikeMetrics(bool preSpike, bool postSpike);
    void recordPlasticityMetrics(float weightChange, float strength);
    void recordTransmissionMetrics(float delay, bool success);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Performance monitoring integration
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable SynapsePerformanceMetrics perfMetrics;
};

} // namespace nlm
