#include "NeuromodulationController.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NeuromodulationController::NeuromodulationController(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , enabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
{
}

void NeuromodulationController::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !enabled_) return;
    
    predictionError_ = reward - predictedReward;
    
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    float plasticityFactor = calculatePlasticityFactor(dopamineLevel_);
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void NeuromodulationController::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = calculateSynaptogenesisRate(plasticityModifier_);
            float pruneRate = calculatePruningRate(plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

float NeuromodulationController::calculatePlasticityFactor(float dopamineLevel) {
    return 0.5f + 0.5f * dopamineLevel;
}

float NeuromodulationController::calculateSynaptogenesisRate(float plasticityModifier) {
    return 0.0001f * plasticityModifier;
}

float NeuromodulationController::calculatePruningRate(float plasticityModifier) {
    return 0.00001f * (2.0f - plasticityModifier);
}

void NeuromodulationController::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

} // namespace nlm