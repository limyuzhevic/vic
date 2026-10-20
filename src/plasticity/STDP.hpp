#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

#pragma once

#include "PlasticityRule.hpp"
#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

// Advanced STDP implementation with multiple variants
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
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    void configureAdvanced(float ltpWeight, float ltdWeight, float tau,
                           float tripletTau, float rewardMod, float activityDepletion,
                           float neuromodulatorGain);
    
    // Advanced STDP features
    void enableRewardModulation(bool enable, float modulationFactor);
    void enableTripletInteractions(bool enable, float tripletTau);
    void enableSpikeDependentLearning(bool enable, float depletionRate);
    void enableNeuromodulatorGating(bool enable, float gain);
    
    // Eligibility trace management
    float getEligibilityTrace() const;
    void updateEligibilityTrace(float delta);
    void resetEligibilityTrace();
    
    // Helper methods for advanced STDP implementations
    float applyTripletSTDP(const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt);
    float applyRewardModulatedSTDP(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt);
    float applySpikeDependentSTDP(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP) - Fully implemented
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Configuration
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setRewardModulationFactor(float factor);
    float getRewardModulationFactor() const;
    void setEligibilityTraceDecay(float decay);
    float getEligibilityTraceDecay() const;
    
    // Advanced features
    void enableEligibilityTrace(bool enable);
    void setTraceDecay(float decay);
    float getEligibilityTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions - Fully implemented
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Configuration
    void setTripletTimeConstant(float tau);
    float getTripletTimeConstant() const;
    void setPrePostWeight(float weight);
    float getPrePostWeight() const;
    void setPostPreWeight(float weight);
    float getPostPreWeight() const;
    void setPostPostWeight(float weight);
    float getPostPostWeight() const;
    
    // Advanced triplet interactions
    void enableTripletLearning(bool enable);
    void setTripletWeightFactors(float prePost, float postPre, float postPost);
    float calculateTripletInteraction(const Timestamp& t_pre, const Timestamp& t_post1, const Timestamp& t_post2);
    
    // Rule selection
    enum class TripletRuleType {
        STANDARD,
        CA1,
        CA3,
        PYRAMIDAL
    };
    
    void setTripletRuleType(TripletRuleType type);
    const char* getTripletRuleName() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
