#include "STDP.hpp"

namespace nlm {

// Reward-modulated STDP (R-STDP)
// Implements reward-modulated learning where synaptic changes are gated by reward signals
// Biological basis: Dopamine signaling in reinforcement learning
// Mathematical formulation: Δw = R * E(Δt) * f(Δt)
// Where R is reward signal, E(Δt) is eligibility trace, f(Δt) is STDP window

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP() : rewardFactor_(1.0f), traceDecayRate_(0.01f) {}
    ~RewardModulatedSTDP() override = default;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse) return;
        
        float totalDelta = 0.0f;
        float tau = timeConstant_;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt_val = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (dt_val > 0) {
                    // Pre before post: positive contribution to eligibility trace
                    float contribution = ltpWeight_ * std::exp(-dt_val / tau);
                    totalDelta += contribution;
                } else if (dt_val < 0) {
                    // Post before pre: negative contribution to eligibility trace
                    float contribution = -ltdWeight_ * std::exp(dt_val / tau);
                    totalDelta += contribution;
                }
                // dt_val == 0: no change (simultaneous spikes)
            }
        }
        
        if (std::abs(totalDelta) > 1e-6f) {
            // Update eligibility trace based on spike timing
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        // Apply reward factor to weight change
        float weightedDelta = delta * rewardFactor_;
        
        float newWeight = synapse->getWeight() + weightedDelta;
        newWeight = std::clamp(newWeight, minWeight_, maxWeight_);
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override { return "R-STDP"; }
    
    // Reward-modulated STDP specific parameters
    void setRewardFactor(float factor) { rewardFactor_ = factor; }
    float getRewardFactor() const { return rewardFactor_; }
    void setTraceDecayRate(float rate) { traceDecayRate_ = rate; }
    float getTraceDecayRate() const { return traceDecayRate_; }
    void setTraceBaseline(float baseline) { traceBaseline_ = baseline; }
    float getTraceBaseline() const { return traceBaseline_; }
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau,
                    float rewardFactor, float traceDecayRate) {
        ltpWeight_ = std::clamp(ltpWeight, 0.0f, 1.0f);
        ltdWeight_ = std::clamp(ltdWeight, 0.0f, 1.0f);
        timeConstant_ = std::clamp(tau, 1.0f, 100.0f);
        rewardFactor_ = std::clamp(rewardFactor, 0.0f, 10.0f);
        traceDecayRate_ = std::clamp(traceDecayRate, 0.0f, 1.0f);
    }
    
    // Decay eligibility trace over time (should be called periodically)
    void decayTrace(Synapse* synapse, TimestepDuration dt) {
        if (!synapse) return;
        
        float trace = synapse->getEligibilityTrace();
        float decayFactor = std::exp(-traceDecayRate_ * static_cast<float>(dt));
        synapse->setEligibilityTrace(trace * decayFactor);
    }
    
    // Apply reward signal (converts eligibility trace to weight change)
    void applyReward(Synapse* synapse, float rewardSignal) {
        if (!synapse) return;
        
        float trace = synapse->getEligibilityTrace();
        
        // Convert trace to weight change using reward signal
        float weightChange = trace * rewardFactor_ * rewardSignal;
        
        applyWeightChange(synapse, weightChange);
        
        // Reset eligibility trace after reward application
        synapse->setEligibilityTrace(traceBaseline_);
    }

private:
    float ltpWeight_ = 0.01f;      // LTP amplitude
    float ltdWeight_ = 0.012f;     // LTD amplitude
    float timeConstant_ = 20.0f;   // STDP time window (ms)
    float minWeight_ = -1.0f;      // Minimum synaptic weight
    float maxWeight_ = 1.0f;       // Maximum synaptic weight
    float rewardFactor_ = 1.0f;    // Reward modulation strength
    float traceDecayRate_ = 0.01f; // Eligibility trace decay rate
    float traceBaseline_ = 0.0f;  // Baseline eligibility trace
};
