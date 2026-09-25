// PlasticityRule.cpp - Standardized plasticity rule implementations
// 
// This file provides improved, consistent implementations of plasticity rules
// with standardized interfaces, better error handling, and enhanced features.

#include "PlasticityRule.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// === Base class implementations ===

// Default implementations for common methods
void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void PlasticityRule::onParametersChanged() {
    // Rule-specific parameter initialization
    // Can be overridden by derived classes
}

// === Enhanced STDP implementation ===

class EnhancedSTDP : public PlasticityRule {
public:
    EnhancedSTDP() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // STDP-specific parameters
        ltpWeight_ = 0.01f;
        ltdWeight_ = 0.012f;
        timeConstant_ = 20.0f;
    }
    
    const char* getName() const override { return "EnhancedSTDP"; }
    
    void setLTPWeight(float weight) {
        ltpWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getLTPWeight() const { return ltpWeight_; }
    
    void setLTDWeight(float weight) {
        ltdWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getLTDWeight() const { return ltdWeight_; }
    
    void setTimeConstant(float tau) {
        timeConstant_ = std::clamp(tau, 1.0f, 100.0f);
    }
    
    float getTimeConstant() const { return timeConstant_; }
    
    void configure(float ltp, float ltd, float tau) override {
        setLTPWeight(ltp);
        setLTDWeight(ltd);
        setTimeConstant(tau);
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        float totalDelta = 0.0f;
        float tau = timeConstant_;
        
        // Enhanced STDP with biological accuracy
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt_ms = static_cast<float>(postTime - preTime);
                
                if (dt_ms > 0) {
                    // Pre before post: LTP
                    float delta = ltpWeight_ * std::exp(-dt_ms / tau);
                    totalDelta += delta;
                } else if (dt_ms < 0) {
                    // Post before pre: LTD
                    float delta = -ltdWeight_ * std::exp(dt_ms / tau);
                    totalDelta += delta;
                }
            }
        }
        
        // Apply with eligibility trace and efficacy scaling
        if (std::abs(totalDelta) > 1e-6f) {
            float efficacy = getSynapticEfficacy(synapse);
            totalDelta *= efficacy;
            
            // Record learning event
            recordLearningEvent(totalDelta, static_cast<float>(preSpikes.back()));
            
            // Apply eligibility trace
            updateEligibilityTrace(synapse, totalDelta);
            
            // Apply weight change
            applyWeightChange(synapse, totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = clampWeight(newWeight);
        synapse->setWeight(newWeight);
    }
    
protected:
    void onParametersChanged() override {
        // Update STDP parameters from PlasticityParameters
        ltpWeight_ = params_.learningRate * 1.0f;  // LTP proportional to learning rate
        ltdWeight_ = params_.learningRate * 1.2f;  // LTD slightly higher than LTP
        timeConstant_ = params_.timeConstant;
    }
    
    void onUpdate(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) override {
        // Enhanced STDP update logic
        update(synapse, preSpikes, postSpikes, dt);
    }
    
    void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        applyWeightChange(synapse, delta);
    }
    
private:
    float ltpWeight_;
    float ltdWeight_;
    float timeConstant_;
};

// === Enhanced Hebbian implementation ===

class EnhancedHebbian : public PlasticityRule {
public:
    EnhancedHebbian() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // Hebbian-specific parameters
        correlationWindow_ = 100.0f;
        useCovarianceRule_ = true;
        maxCorrelations_ = 1000;
    }
    
    const char* getName() const override { return "EnhancedHebbian"; }
    
    void setCorrelationWindow(float window) {
        correlationWindow_ = std::max(10.0f, window);
    }
    
    float getCorrelationWindow() const { return correlationWindow_; }
    
    void enableCovarianceRule(bool enable) {
        useCovarianceRule_ = enable;
    }
    
    bool isCovarianceRuleEnabled() const { return useCovarianceRule_; }
    
    void setMaxCorrelations(size_t max) {
        maxCorrelations_ = std::max(100, static_cast<int>(max));
    }
    
    size_t getMaxCorrelations() const { return maxCorrelations_; }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        float totalDelta = 0.0f;
        
        if (useCovarianceRule_) {
            // Enhanced covariance-based Hebbian learning
            totalDelta = computeCovarianceLearning(synapse, preSpikes, postSpikes);
        } else {
            // Simpler correlation-based learning
            totalDelta = computeCorrelationLearning(synapse, preSpikes, postSpikes);
        }
        
        // Apply weight change
        if (std::abs(totalDelta) > 1e-6f) {
            totalDelta = computeSafeDelta(totalDelta);
            recordLearningEvent(totalDelta, static_cast<float>(preSpikes.back()));
            applyWeightChange(synapse, totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = clampWeight(newWeight);
        synapse->setWeight(newWeight);
    }
    
protected:
    void onParametersChanged() override {
        // Update Hebbian parameters
        correlationWindow_ = params_.timeConstant * 5.0f; // 5x time constant
    }
    
    void onUpdate(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) override {
        update(synapse, preSpikes, postSpikes, dt);
    }
    
    void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        applyWeightChange(synapse, delta);
    }
    
private:
    float computeCovarianceLearning(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes) {
        // Enhanced covariance rule: Δw = η * (⟨pre·post⟩ - ⟨pre⟩⟨post⟩)
        
        size_t preSpikesCount = preSpikes.size();
        size_t postSpikesCount = postSpikes.size();
        
        if (preSpikesCount == 0 || postSpikesCount == 0) {
            return 0.0f;
        }
        
        // Compute mean firing rates
        float preMeanRate = static_cast<float>(preSpikesCount) / preSpikesCount;
        float postMeanRate = static_cast<float>(postSpikesCount) / postSpikesCount;
        
        // Compute cross-correlation (simplified)
        float crossCorrelation = 0.0f;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (std::abs(dt) < correlationWindow_) {
                    crossCorrelation += 1.0f;
                }
            }
        }
        
        // Normalize and apply learning rule
        float expectedCorrelation = preMeanRate * postMeanRate * correlationWindow_;
        float covariance = crossCorrelation - expectedCorrelation;
        
        float delta = params_.learningRate * covariance;
        
        // Limit maximum learning per update
        delta = std::clamp(delta, -1.0f, 1.0f);
        
        return delta;
    }
    
    float computeCorrelationLearning(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes) {
        // Simplified correlation-based learning
        
        size_t correlationCount = 0;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (std::abs(dt) < correlationWindow_) {
                    ++correlationCount;
                }
            }
        }
        
        // Apply learning rate with saturation
        float delta = params_.learningRate * static_cast<float>(correlationCount);
        
        // Apply non-linear saturation to prevent runaway
        delta = std::tanh(delta / 10.0f) * params_.learningRate;
        
        return delta;
    }
    
private:
    float correlationWindow_;
    bool useCovarianceRule_;
    size_t maxCorrelations_;
};

// === Reward-modulated STDP ===

class RewardModulatedSTDP : public EnhancedSTDP {
public:
    RewardModulatedSTDP() {
        PlasticityParameters p;
        p.learningRate = 0.01f;
        p.minWeight = -1.0f;
        p.maxWeight = 1.0f;
        p.timeConstant = 20.0f;
        setParameters(p);
        
        // Reward modulation parameters
        rewardWeight_ = 0.5f;
        predictionWeight_ = 0.3f;
        eligibilityDecay_ = 0.95f;
        maxEligibilityTrace_ = 10.0f;
    }
    
    const char* getName() const override { return "RewardModulatedSTDP"; }
    
    void setRewardWeight(float weight) {
        rewardWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getRewardWeight() const { return rewardWeight_; }
    
    void setPredictionWeight(float weight) {
        predictionWeight_ = std::clamp(weight, 0.0f, 1.0f);
    }
    
    float getPredictionWeight() const { return predictionWeight_; }
    
    void setEligibilityDecay(float decay) {
        eligibilityDecay_ = std::clamp(decay, 0.5f, 1.0f);
    }
    
    float getEligibilityDecay() const { return eligibilityDecay_; }
    
    void applyRewardModulation(float reward, float predictionError) {
        // Enhance STDP weights based on reward and prediction error
        float rewardFactor = 1.0f + reward * rewardWeight_;
        float predictionFactor = 1.0f + predictionError * predictionWeight_;
        
        float weightMultiplier = rewardFactor * predictionFactor;
        weightMultiplier = std::clamp(weightMultiplier, 0.5f, 2.0f);
        
        // Scale STDP weights
        float currentLTP = getLTPWeight();
        float currentLTD = getLTDWeight();
        
        setLTPWeight(currentLTP * weightMultiplier);
        setLTDWeight(currentLTD * weightMultiplier);
        
        NLM_LOG_INFO("R-STDP modulation: reward=" + std::to_string(reward) +
                    " pred_err=" + std::to_string(predictionError) +
                    " factor=" + std::to_string(weightMultiplier));
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Update with eligibility trace decay
        if (synapse) {
            float currentTrace = synapse->getEligibilityTrace();
            currentTrace *= eligibilityDecay_;
            synapse->setEligibilityTrace(currentTrace);
        }
        
        // Apply enhanced STDP
        EnhancedSTDP::update(synapse, preSpikes, postSpikes, dt);
    }
    
protected:
    void onParametersChanged() override {
        EnhancedSTDP::onParametersChanged();
        // Adjust reward weights based on learning rate
        rewardWeight_ = std::min(1.0f, params_.learningRate * 10.0f);
    }
    
private:
    float rewardWeight_;
    float predictionWeight_;
    float eligibilityDecay_;
    float maxEligibilityTrace_;
};

// === Factory function implementations ===

std::unique_ptr<PlasticityRule> createSTDP(float ltpWeight, float ltdWeight, float timeConstant) {
    auto rule = std::make_unique<EnhancedSTDP>();
    rule->configure(ltpWeight, ltdWeight, timeConstant);
    return rule;
}

std::unique_ptr<PlasticityRule> createHebbian(float learningRate, float correlationWindow) {
    auto rule = std::make_unique<EnhancedHebbian>();
    PlasticityParameters p;
    p.learningRate = learningRate;
    p.timeConstant = correlationWindow / 5.0f;
    rule->setParameters(p);
    return rule;
}

std::unique_ptr<PlasticityRule> createRewardModulatedSTDP(float ltpWeight, float ltdWeight, 
                                                         float timeConstant, float rewardWeight) {
    auto rule = std::make_unique<RewardModulatedSTDP>();
    rule->configure(ltpWeight, ltdWeight, timeConstant);
    rule->setRewardWeight(rewardWeight);
    return rule;
}

} // namespace nlm