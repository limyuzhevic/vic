#pragma once

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>
#include <cmath>

namespace nlm {

// Advanced plasticity rule: Multi-timescale learning
// Implements multiple plasticity mechanisms operating at different timescales:
// - Fast STDP for immediate synaptic changes
// - Medium Hebbian for pattern stabilization
// - Slow homeostatic regulation for stability
class MultiTimescalePlasticity : public PlasticityRule {
public:
    MultiTimescalePlasticity() : ftdpEnabled_(true), hebbianEnabled_(true), 
                                 homeostaticEnabled_(true), metaLearningEnabled_(false) {
        // Fast timescaling parameters
        setSTDPParams(0.01f, 0.012f, 20.0f);
        
        // Medium timescaling parameters  
        setHebbianParams(0.005f, 0.5f, 0.1f);
        
        // Slow timescaling parameters
        setHomeostaticParams(0.001f, 0.01f, 0.05f);
        
        // Meta-learning parameters
        setMetaLearningParams(0.001f, 0.01f, 0.1f);
    }
    
    ~MultiTimescalePlasticity() override = default;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse) {
            NLM_LOG_WARNING("MultiTimescalePlasticity::update called with null synapse");
            return;
        }
        
        float totalDelta = 0.0f;
        
        // Fast: STDP for immediate synaptic changes
        if (ftdpEnabled_) {
            float stdpDelta = computeSTDP(synapse, preSpikes, postSpikes, dt);
            totalDelta += stdpDelta;
            
            // Meta-learning: adjust STDP parameters based on performance
            if (metaLearningEnabled_) {
                adjustSTDPPARAMS(synapse, stdpDelta);
            }
        }
        
        // Medium: Hebbian for pattern stabilization
        if (hebbianEnabled_) {
            float hebbianDelta = computeHebbian(synapse, preSpikes, postSpikes, dt);
            totalDelta += hebbianDelta;
            
            // Meta-learning: adjust Hebbian parameters
            if (metaLearningEnabled_) {
                adjustHebbianPARAMS(synapse, hebbianDelta);
            }
        }
        
        // Slow: Homeostatic regulation for stability
        if (homeostaticEnabled_) {
            float homeostaticDelta = computeHomeostatic(synapse, preSpikes, postSpikes, dt);
            totalDelta += homeostaticDelta;
            
            // Meta-learning: adjust homeostatic parameters
            if (metaLearningEnabled_) {
                adjustHomeostaticPARAMS(synapse, homeostaticDelta);
            }
        }
        
        // Apply combined weight change
        if (std::abs(totalDelta) > 1e-6f) {
            applyWeightChange(synapse, totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) {
            NLM_LOG_ERROR("MultiTimescalePlasticity::applyWeightChange called with null synapse");
            return;
        }
        
        // Clamp delta to reasonable bounds
        if (std::abs(delta) > 0.1f) {
            NLM_LOG_WARNING("MultiTimescalePlasticity::applyWeightChange: Large delta " + std::to_string(delta) + " may cause instability");
        }
        
        // Apply with combined plasticity rules
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
        
        // Update eligibility trace for meta-learning
        if (metaLearningEnabled_) {
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + delta * 0.1f);
        }
    }
    
    const char* getName() const override {
        return "MultiTimescalePlasticity";
    }
    
    // Fast timescale: STDP parameters
    void setSTDPParams(float ltpWeight, float ltdWeight, float timeConstant) {
        ftdpLTPWeight_ = std::clamp(ltpWeight, 0.001f, 0.1f);
        ftdpLTDWeight_ = std::clamp(ltdWeight, 0.001f, 0.1f);
        ftdpTimeConstant_ = std::clamp(timeConstant, 5.0f, 100.0f);
    }
    
    void setFTDPEnabled(bool enabled) { ftdpEnabled_ = enabled; }
    bool isFTDPEnabled() const { return ftdpEnabled_; }
    
    // Medium timescale: Hebbian parameters
    void setHebbianParams(float learningRate, float maxWeight, float minWeight) {
        hebbianLearningRate_ = std::clamp(learningRate, 0.001f, 0.1f);
        hebbianMaxWeight_ = std::clamp(maxWeight, 0.0f, 2.0f);
        hebbianMinWeight_ = std::clamp(minWeight, -2.0f, 0.0f);
    }
    
    void setHebbianEnabled(bool enabled) { hebbianEnabled_ = enabled; }
    bool isHebbianEnabled() const { return hebbianEnabled_; }
    
    // Slow timescale: Homeostatic parameters
    void setHomeostaticParams(float decayRate, float targetWeight, float stability) {
        homeostaticDecayRate_ = std::clamp(decayRate, 0.0001f, 0.01f);
        homeostaticTargetWeight_ = std::clamp(targetWeight, -1.0f, 1.0f);
        homeostaticStability_ = std::clamp(stability, 0.01f, 0.5f);
    }
    
    void setHomeostaticEnabled(bool enabled) { homeostaticEnabled_ = enabled; }
    bool isHomeostaticEnabled() const { return homeostaticEnabled_; }
    
    // Meta-learning parameters
    void setMetaLearningParams(float learningRate, float adaptationRate, float memoryDecay) {
        metaLearningRate_ = std::clamp(learningRate, 0.0001f, 0.01f);
        metaAdaptationRate_ = std::clamp(adaptationRate, 0.001f, 0.1f);
        metaMemoryDecay_ = std::clamp(memoryDecay, 0.9f, 0.999f);
    }
    
    void setMetaLearningEnabled(bool enabled) { metaLearningEnabled_ = enabled; }
    bool isMetaLearningEnabled() const { return metaLearningEnabled_; }
    
    // Meta-learning: learn from experience
    void learnFromExperience(Synapse* synapse, float predictionError, float outcome) {
        if (!metaLearningEnabled_ || !synapse) return;
        
        // Update meta-parameters based on prediction error
        float errorSignal = predictionError * outcome;
        
        // Adapt STDP parameters
        float stdpAdaptation = metaLearningRate_ * errorSignal;
        ftdpLTPWeight_ = std::clamp(ftdpLTPWeight_ + stdpAdaptation * metaAdaptationRate_, 0.001f, 0.1f);
        ftdpLTDWeight_ = std::clamp(ftdpLTDWeight_ - stdpAdaptation * metaAdaptationRate_, 0.001f, 0.1f);
        
        // Adapt Hebbian parameters
        float hebbianAdaptation = metaLearningRate_ * errorSignal * 0.5f;
        hebbianLearningRate_ = std::clamp(hebbianLearningRate_ + hebbianAdaptation * metaAdaptationRate_, 0.001f, 0.1f);
        
        // Decay memory traces
        eligibilityTrace_ *= metaMemoryDecay_;
        
        NLM_LOG_INFO("Meta-learning: Adapted plasticity parameters based on experience");
    }
    
private:
    // Fast timescale: STDP computation
    float computeSTDP(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) return 0.0f;
        
        float totalDelta = 0.0f;
        float tau = ftdpTimeConstant_;
        
        // Optimized: sort and merge-like processing
        std::vector<Timestamp> sortedPre = preSpikes;
        std::vector<Timestamp> sortedPost = postSpikes;
        std::sort(sortedPre.begin(), sortedPre.end());
        std::sort(sortedPost.begin(), sortedPost.end());
        
        size_t preIdx = 0, postIdx = 0;
        
        while (preIdx < sortedPre.size() && postIdx < sortedPost.size()) {
            float deltaT = static_cast<float>(sortedPost[postIdx] - sortedPre[preIdx]);
            
            if (deltaT > 0) {
                // Pre before post: LTP
                totalDelta += ftdpLTPWeight_ * tau * std::exp(-deltaT / tau);
                ++postIdx;
            } else {
                // Post before pre: LTD
                totalDelta += -ftdpLTDWeight_ * tau * std::exp(deltaT / tau);
                ++preIdx;
            }
        }
        
        return totalDelta;
    }
    
    void adjustSTDPPARAMS(Synapse* synapse, float delta) {
        // Simple adaptive mechanism for STDP parameters
        float adaptation = std::abs(delta) * metaLearningRate_;
        ftdpLTPWeight_ = std::clamp(ftdpLTPWeight_ * (1.0f + adaptation), 0.001f, 0.1f);
        ftdpLTDWeight_ = std::clamp(ftdpLTDWeight_ * (1.0f - adaptation), 0.001f, 0.1f);
    }
    
    // Medium timescale: Hebbian computation
    float computeHebbian(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) return 0.0f;
        
        // Count correlated spikes efficiently
        std::vector<Timestamp> sortedPre = preSpikes;
        std::vector<Timestamp> sortedPost = postSpikes;
        std::sort(sortedPre.begin(), sortedPre.end());
        std::sort(sortedPost.begin(), sortedPost.end());
        
        size_t correlationCount = 0;
        const float window = 100.0f;
        
        for (size_t i = 0; i < sortedPre.size(); ++i) {
            auto lower = std::lower_bound(sortedPost.begin(), sortedPost.end(), 
                                         sortedPre[i] - window);
            auto upper = std::upper_bound(sortedPost.begin(), sortedPost.end(), 
                                         sortedPre[i] + window);
            correlationCount += std::distance(lower, upper);
        }
        
        return hebbianLearningRate_ * static_cast<float>(correlationCount);
    }
    
    void adjustHebbianPARAMS(Synapse* synapse, float delta) {
        // Adapt Hebbian learning rate based on correlation strength
        float correlation = std::min(std::abs(delta) / hebbianLearningRate_, 1.0f);
        hebbianLearningRate_ = std::clamp(hebbianLearningRate_ * (1.0f + correlation * 0.1f), 0.001f, 0.1f);
    }
    
    // Slow timescale: Homeostatic computation
    float computeHomeostatic(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
        if (!synapse) return 0.0f;
        
        float currentWeight = synapse->getWeight();
        float homeostaticDrive = currentWeight - homeostaticTargetWeight_;
        
        // Decay towards target weight
        float decay = homeostaticDecayRate_ * dt;
        float weightChange = -homeostaticDrive * decay;
        
        // Stability modulation based on recent activity
        float activityLevel = computeActivityLevel(preSpikes, postSpikes);
        float stabilityMod = 1.0f - homeostaticStability_ * activityLevel;
        
        return weightChange * stabilityMod;
    }
    
    void adjustHomeostaticPARAMS(Synapse* synapse, float delta) {
        // Adjust homeostatic parameters based on learning dynamics
        float learningRate = std::min(std::abs(delta), 0.1f);
        homeostaticTargetWeight_ = std::clamp(homeostaticTargetWeight_ + learningRate * 0.01f, -1.0f, 1.0f);
        homeostaticDecayRate_ = std::clamp(homeostaticDecayRate_ * (1.0f + learningRate * 0.1f), 0.0001f, 0.01f);
    }
    
    float computeActivityLevel(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes) {
        // Simple activity level computation
        size_t totalSpikes = preSpikes.size() + postSpikes.size();
        return std::min(totalSpikes * 0.001f, 1.0f);
    }
    
    // Meta-learning state
    float eligibilityTrace_ = 0.0f;
    
    // Fast timescale (STDP) parameters
    bool ftdpEnabled_ = true;
    float ftdpLTPWeight_ = 0.01f;
    float ftdpLTDWeight_ = 0.012f;
    float ftdpTimeConstant_ = 20.0f;
    
    // Medium timescale (Hebbian) parameters
    bool hebbianEnabled_ = true;
    float hebbianLearningRate_ = 0.005f;
    float hebbianMaxWeight_ = 0.5f;
    float hebbianMinWeight_ = 0.1f;
    
    // Slow timescale (homeostatic) parameters
    bool homeostaticEnabled_ = true;
    float homeostaticDecayRate_ = 0.001f;
    float homeostaticTargetWeight_ = 0.0f;
    float homeostaticStability_ = 0.05f;
    
    // Meta-learning parameters
    bool metaLearningEnabled_ = false;
    float metaLearningRate_ = 0.001f;
    float metaAdaptationRate_ = 0.01f;
    float metaMemoryDecay_ = 0.95f;
    
}; // namespace nlm