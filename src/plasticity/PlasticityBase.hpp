#ifndef NLM_PLASTICITY_BASE_H
#define NLM_PLASTICITY_BASE_H

#include "core/Utilities.hpp"
#include "brain/Synapse.hpp"
#include <memory>

namespace nlm {

class PlasticityRuleBase {
public:
    virtual ~PlasticityRuleBase() = default;
    
    virtual const char* getName() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
    
    virtual void update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) = 0;
    
    virtual void reset() {}
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) {
        if (!synapse) return;
        
        // Clamp weight to valid range
        float newWeight = synapse->getWeight() + delta;
        newWeight = clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
    
protected:
    float learningRate_ = 0.01f;
    bool enabled_ = true;
};

// Concrete implementations with common patterns
class HebbianRule : public PlasticityRuleBase {
public:
    HebbianRule() : PlasticityRuleBase() {
        learningRate_ = 0.01f;
    }
    
    const char* getName() const override {
        return "Hebbian";
    }
    
    void setLearningRate(float rate) override {
        learningRate_ = clamp(rate, 0.0f, 1.0f);
    }
    
    float getLearningRate() const override {
        return learningRate_;
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty() || !enabled_) {
            return;
        }
        
        // Calculate correlation between pre- and post-synaptic activity
        float correlation = calculateCorrelation(preSpikes, postSpikes);
        
        if (!isNear(correlation, 0.0f, 0.01f)) {
            float weightChange = learningRate_ * correlation;
            applyWeightChange(synapse, weightChange);
            
            // Update eligibility trace for reward-modulated learning
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + weightChange);
        }
    }
    
private:
    float calculateCorrelation(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes) {
        if (preSpikes.size() < 2 || postSpikes.size() < 2) {
            return 0.0f;
        }
        
        // Find coincident spikes within 10ms window
        size_t coincidences = 0;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                if (isNear(preTime, postTime, 10.0f)) {
                    coincidences++;
                }
            }
        }
        
        float maxPossible = std::min(preSpikes.size(), postSpikes.size());
        if (maxPossible == 0) return 0.0f;
        
        return static_cast<float>(coincidences) / maxPossible;
    }
};

class STDPRule : public PlasticityRuleBase {
public:
    STDPRule() : PlasticityRuleBase() {
        ltpWeight_ = 0.01f;
        ltdWeight_ = 0.012f;
        timeConstant_ = 20.0f;
    }
    
    const char* getName() const override {
        return "STDP";
    }
    
    void setLTPWeight(float weight) {
        ltpWeight_ = clamp(weight, 0.0f, 1.0f);
    }
    
    void setLTDWeight(float weight) {
        ltdWeight_ = clamp(weight, 0.0f, 1.0f);
    }
    
    void setTimeConstant(float tau) {
        timeConstant_ = clamp(tau, 1.0f, 100.0f);
    }
    
    float getLTPWeight() const {
        return ltpWeight_;
    }
    
    float getLTDWeight() const {
        return ltdWeight_;
    }
    
    float getTimeConstant() const {
        return timeConstant_;
    }
    
    void configure(float ltpWeight, float ltdWeight, float tau) {
        setLTPWeight(ltpWeight);
        setLTDWeight(ltdWeight);
        setTimeConstant(tau);
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty() || !enabled_) {
            return;
        }
        
        float totalDelta = 0.0f;
        
        // Apply pairwise STDP updates
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = postTime - preTime;
                
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    float delta = ltpWeight_ * std::exp(-dt / timeConstant_);
                    totalDelta += delta;
                } else if (dt < 0) {
                    // Post before pre: DEPRESSION
                    float delta = -ltdWeight_ * std::exp(dt / timeConstant_);
                    totalDelta += delta;
                }
            }
        }
        
        // Apply weight change with efficacy scaling
        if (!isNear(totalDelta, 0.0f, EPSILON)) {
            float efficacy = synapse->getEfficacy();
            totalDelta *= efficacy;
            applyWeightChange(synapse, totalDelta);
            
            // Update eligibility trace
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + totalDelta);
        }
    }
    
private:
    float ltpWeight_;
    float ltdWeight_;
    float timeConstant_;
};

class RewardModulatedRule : public PlasticityRuleBase {
public:
    RewardModulatedRule() : PlasticityRuleBase() {
        learningRate_ = 0.05f;
        baselineTraceDecay_ = 0.001f;
        rewardThreshold_ = 0.1f;
    }
    
    const char* getName() const override {
        return "RewardModulated";
    }
    
    void setLearningRate(float rate) override {
        learningRate_ = clamp(rate, 0.0f, 1.0f);
    }
    
    void setBaselineTraceDecay(float decay) {
        baselineTraceDecay_ = clamp(decay, 0.0f, 1.0f);
    }
    
    void setRewardThreshold(float threshold) {
        rewardThreshold_ = clamp(threshold, 0.0f, 1.0f);
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse || !enabled_) {
            return;
        }
        
        // Get current reward prediction error
        float reward = getRewardSignal();
        
        if (std::abs(reward) > rewardThreshold_) {
            // Calculate standard weight change
            float weightChange = learningRate_ * reward;
            
            // Scale by spike correlation
            float correlation = calculateCorrelation(preSpikes, postSpikes);
            weightChange *= correlation;
            
            // Apply modulated weight change
            if (!isNear(weightChange, 0.0f, EPSILON)) {
                // Scale eligibility trace by reward prediction error
                float currentTrace = synapse->getEligibilityTrace();
                float modulatedTrace = currentTrace * reward;
                synapse->setEligibilityTrace(modulatedTrace);
                
                // Apply weight change
                applyWeightChange(synapse, weightChange);
                
                // Decay eligibility trace
                synapse->decayEligibilityTrace(baselineTraceDecay_);
            }
        }
    }
    
    virtual float getRewardSignal() const {
        // This should be overridden by actual reward signals
        // For now return a baseline value
        return 0.0f;
    }
    
private:
    float baselineTraceDecay_;
    float rewardThreshold_;
};

// Factory function for creating plasticity rules
inline std::unique_ptr<PlasticityRuleBase> createPlasticityRule(const std::string& name) {
    if (name == "Hebbian") {
        return std::make_unique<HebbianRule>();
    } else if (name == "STDP") {
        return std::make_unique<STDPRule>();
    } else if (name == "RewardModulated") {
        return std::make_unique<RewardModulatedRule>();
    } else {
        throw ConfigurationException("Unknown plasticity rule: " + name);
    }
}

} // namespace nlm

#endif // NLM_PLASTICITY_BASE_H
