#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

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
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Implements dopamine-dependent plasticity
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP() = default;
    ~RewardModulatedSTDP() override = default;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        // Apply STDP for spike timing, then modulate by reward
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Apply base STDP
        float totalDelta = 0.0f;
        float tau = timeConstant_;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (dt > 0) {
                    // Potentiation
                    totalDelta += ltpWeight_ * std::exp(-dt / tau);
                } else if (dt < 0) {
                    // Depression
                    totalDelta -= ltdWeight_ * std::exp(dt / tau);
                }
            }
        }
        
        // Apply reward modulation if eligibility trace exists
        float eligibility = synapse->getEligibilityTrace();
        float rewardFactor = 1.0f;
        if (rewardEnabled_ && eligibility > 0.0f) {
            rewardFactor *= (1.0f + rewardModulation_ * eligibility);
        }
        
        // Apply modulated weight change
        synapse->addToWeight(totalDelta * rewardFactor);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        delta *= rewardFactor_;
        delta = std::clamp(delta, minWeight_, maxWeight_);
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "R-STDP"; }
    
    void setRewardModulation(float modulation) { rewardModulation_ = modulation; }
    float getRewardModulation() const { return rewardModulation_; }
    
    void enableReward(bool enable) { rewardEnabled_ = enable; }
    bool isRewardEnabled() const { return rewardEnabled_; }
    
private:
    float rewardModulation_ = 1.0f;  // Reward modulation strength
    bool rewardEnabled_ = true;
};

// Spike-timing dependent plasticity with triplet interactions
// Implements the triplet STDP model (g-b, g-p, a-p traces)
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP() = default;
    ~TripletSTDP() override = default;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Get latest spike times
        Timestamp latestPre = preSpikes.back();
        Timestamp latestPost = postSpikes.back();
        float totalDelta = 0.0f;
        
        // Apply triple interactions
        // g-b trace: tracks pre-before-post interactions (potentiation)
        for (Timestamp preTime : preSpikes) {
            if (preTime < latestPost) {
                float tau_gb = 100.0f;  // g-b time constant
                totalDelta += w_gb_ * std::exp(-(latestPost - preTime) / tau_gb);
            }
        }
        
        // g-p trace: tracks post-before-pre interactions (depression)
        for (Timestamp postTime : postSpikes) {
            if (postTime < latestPre) {
                float tau_gp = 100.0f;  // g-p time constant
                totalDelta -= w_gp_ * std::exp(-(latestPre - postTime) / tau_gp);
            }
        }
        
        // a-p trace: tracks pre-before-pre interactions (facilitation)
        for (size_t i = 0; i < preSpikes.size() - 1; ++i) {
            if (preSpikes[i] < latestPre) {
                float tau_ap = 20.0f;   // a-p time constant
                totalDelta += w_ap_ * std::exp(-(latestPre - preSpikes[i]) / tau_ap);
            }
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        delta = std::clamp(delta, minWeight_, maxWeight_);
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "TripletSTDP"; }
    
    void setWeights(float gb, float gp, float ap) {
        w_gb_ = gb; w_gp_ = gp; w_ap_ = ap;
    }
    
private:
    float w_gb_ = 0.005f;   // g-b weight (potentiation)
    float w_gp_ = 0.005f;   // g-p weight (depression)
    float w_ap_ = 0.0005f;  // a-p weight (facilitation)
    float minWeight_ = -1.0f;
    float maxWeight_ = 1.0f;
};

} // namespace nlm
