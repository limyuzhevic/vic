#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Neuromodulator::Neuromodulator(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
    : brain_(std::move(brain))
    , world_(std::move(world))
    , dopamineLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , neuromodulationMin_(-1.0f)
    , neuromodulationMax_(1.0f)
    , developmentalAge_(0.0)
    , developmentalStage_(DevelopmentalStage::Initial)
    , plasticityModifier_(1.0f)
    , structuralPlasticityEnabled_(true)
    , initialDuration_(60.0)      // 1 minute
    , criticalPeriodDuration_(240.0) // 4 minutes
    , maturationDuration_(600.0)   // 10 minutes
{
    if (!brain_ || !world_) {
        throw std::invalid_argument("Neuromodulator requires valid brain and world pointers");
    }
}

void Neuromodulator::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_) return;
    
    predictionError_ = computePredictionError(reward, predictedReward);
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = clampNeuromodulation(dopamineLevel_);
    
    // Apply neuromodulation
    applyDopamineModulation(dopamineLevel_);
}

void Neuromodulator::updateDevelopment(double timestep) {
    if (!brain_ || timestep < 0.0) return;
    
    developmentalAge_ += timestep;
    
    // Determine developmental stage based on age
    if (developmentalAge_ < initialDuration_) {
        developmentalStage_ = DevelopmentalStage::Initial;
        plasticityModifier_ = 1.0f; // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < initialDuration_ + criticalPeriodDuration_) {
        developmentalStage_ = DevelopmentalStage::CriticalPeriod;
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < initialDuration_ + criticalPeriodDuration_ + maturationDuration_) {
        developmentalStage_ = DevelopmentalStage::Maturation;
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        developmentalStage_ = DevelopmentalStage::Adult;
        plasticityModifier_ = 0.2f; // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Update plasticity for current stage
    updatePlasticityForStage(developmentalStage_);
}

void Neuromodulator::reset() {
    dopamineLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    developmentalStage_ = DevelopmentalStage::Initial;
    plasticityModifier_ = 1.0f;
}

float Neuromodulator::computePredictionError(float reward, float predictedReward) {
    return reward - predictedReward;
}

void Neuromodulator::applyDopamineModulation(float dopamineLevel) {
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void Neuromodulator::updatePlasticityForStage(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High plasticity for initial learning
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0002f);
            brain_->getStructuralPlasticity()->setPruningRate(0.00002f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Moderate plasticity with balanced synaptogenesis/pruning
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f);
            brain_->getStructuralPlasticity()->setPruningRate(0.000015f);
            break;
        case DevelopmentalStage::Maturation:
            // Lower plasticity, more pruning
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00005f);
            brain_->getStructuralPlasticity()->setPruningRate(0.00002f);
            break;
        case DevelopmentalStage::Adult:
        case DevelopmentalStage::Aging:
            // Low plasticity, mostly pruning
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.00001f);
            brain_->getStructuralPlasticity()->setPruningRate(0.00003f);
            break;
    }
}

void Neuromodulator::applyStructuralPlasticity(DevelopmentalStage stage) {
    if (!structuralPlasticityEnabled_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Higher synaptogenesis in early development
        float synRate = 0.0001f * plasticityModifier_;
        float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

} // namespace nlm
