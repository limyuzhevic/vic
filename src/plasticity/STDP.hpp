#pragma once

#include "PlasticityRule.hpp"
#include <nlohmann/json.hpp>

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Enhanced implementation with triplet rules, eligibility traces, and advanced features

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Disable copying, enable moving
    STDP(const STDP&) = delete;
    STDP& operator=(const STDP&) = delete;
    STDP(STDP&&) noexcept;
    STDP& operator=(STDP&&) noexcept;
    
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
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Advanced STDP features
    void enableAdvancedFeatures(bool tripletRules, bool localStdp, bool weightNormalization);
    
    // Eligibility trace management
    void setEligibilityDecayRate(float rate);
    
    // Configuration and validation
    void setMinMaxWeight(float minWeight, float maxWeight);
    void getMinMaxWeight(float& minWeight, float& maxWeight) const;
    
    // Performance optimization
    void setMaxSpikesPerSynapse(size_t maxSpikes);
    void setMaxTraceHistory(size_t maxHistory);
    
    // Analysis and debugging
    void logConfiguration() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
