#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Neuromodulator::Neuromodulator(std::shared_ptr<Config> config)
    : config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
{
}

Neuromodulator::~Neuromodulator() = default;

void Neuromodulator::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

void Neuromodulator::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void Neuromodulator::update(double timestep) {
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
    } else {
        plasticityModifier_ = 0.2f;
    }
}

void Neuromodulator::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

void Neuromodulator::applyRewardModulatedPlasticity(float plasticityMod, const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        // Apply plasticity effects based on dopamine level
        float excitabilityMod = dopamineLevel_ * 0.5f;
        if (excitabilityMod > 0.0f) {
            n->injectCurrent(excitabilityMod);
        }
    }
}

} // namespace nlm
