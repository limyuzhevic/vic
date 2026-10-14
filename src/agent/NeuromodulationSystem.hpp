#pragma once

#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include "AgentBrainConfig.hpp"
#include <cmath>

namespace nlm {

class NeuromodulationSystem {
public:
    NeuromodulationSystem(std::shared_ptr<Brain> brain,
                         float& dopamineLevel,
                         float& noveltyLevel,
                         float& predictionError,
                         float& expectedReward,
                         float& plasticityModifier)
        : brain_(brain)
        , dopamineLevel_(dopamineLevel)
        , noveltyLevel_(noveltyLevel)
        , predictionError_(predictionError)
        , expectedReward_(expectedReward)
        , plasticityModifier_(plasticityModifier) {
        validateSetup();
    }
    
    // Apply reward-based neuromodulation
    void applyRewardModulation(float reward, float predictedReward, bool enabled) {
        if (!enabled || !brain_) {
            resetNeuromodulationState();
            return;
        }
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = AgentBrainConfig::EXPECTED_REWARD_BETA * expectedReward_ +
                         AgentBrainConfig::EXPECTED_REWARD_ALPHA * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_,
                                   AgentBrainConfig::DOPAMINE_CLAMP_MIN,
                                   AgentBrainConfig::DOPAMINE_CLAMP_MAX);
        
        // Apply to all synapses with eligibility traces
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > AgentBrainConfig::ELIGIBILITY_THRESHOLD) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(AgentBrainConfig::ELIGIBILITY_TRACE_DECAY);
                }
            }
        }
        
        // Modulate plasticity based on dopamine
        // Positive dopamine increases plasticity, negative decreases
        float plasticityFactor = AgentBrainConfig::PLASTICITY_FACTOR_BASE +
                                AgentBrainConfig::PLASTICITY_RANGE * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
    
    void resetNeuromodulationState() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
    }
    
    // Reset method for testing and state clearing
    void reset() {
        resetNeuromodulationState();
    }
    
    // Get current neuromodulation levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Update novelty level based on sensory changes
    void updateNovelty(const std::vector<float>& currentVision,
                      std::vector<float>& previousVision,
                      float& noveltyLevel) {
        if (currentVision.empty()) {
            noveltyLevel = 0.0f;
            return;
        }
        
        // Calculate total difference from previous vision
        float totalDiff = 0.0f;
        for (size_t i = 0; i < currentVision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(currentVision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel = totalDiff / static_cast<float>(std::max<size_t>(currentVision.size(), 1));
        
        // Apply exponential decay
        noveltyLevel *= AgentBrainConfig::SENSORY_NOVELTY_DECAY;
        
        // Update previous vision
        previousVision = currentVision;
    }
    
    bool validateSetup() {
        if (!brain_) {
            throw AgentBrainException::nullPointerError("Brain reference");
        }
        
        return true;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    float& dopamineLevel_;
    float& noveltyLevel_;
    float& predictionError_;
    float& expectedReward_;
    float& plasticityModifier_;
};

} // namespace nlm
