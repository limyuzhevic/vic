#include "NeuromodulationManager.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuromodulationManager::initialize(Brain* brain) {
    brain_ = brain;
    // Initialize statistics
    stats_.totalSteps = 0;
    stats_.plasticityUpdateCount = 0;
    stats_.stateUpdateCount = 0;
}

void NeuromodulationManager::updateNeuromodulators(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update novelty detection
    if (brain_->getNovelty()) {
        brain_->getNovelty()->update(dt);
    }
    
    // Update curiosity system
    if (brain_->getCuriosity()) {
        brain_->getCuriosity()->update(dt);
    }
    
    // Update prediction error signal
    if (brain_->getPredictionErrorSignal()) {
        brain_->getPredictionErrorSignal()->update(dt);
    }
    
    // Update dopamine for reward prediction error
    if (brain_->getDopamine()) {
        brain_->getDopamine()->update(dt);
    }
}

void NeuromodulationManager::applyNeuromodulationEffects() {
    if (!brain_) return;
    
    // Apply dopamine effects on neural excitability
    if (brain_->getDopamine()) {
        applyDopamineExcitabilityEffects();
    }
    
    // Apply other neuromodulator effects (curiosity, novelty, prediction error)
    applyCuriosityEffects();
    applyNoveltyEffects();
    applyPredictionErrorEffects();
}

void NeuromodulationManager::applyDopamineExcitabilityEffects() {
    if (!brain_->getDopamine()) return;
    
    float dopamineLevel = brain_->getDopamine()->getLevel();
    
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Dopamine modulates excitability by injecting additional current
                // Positive dopamine increases excitability (lower effective threshold)
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void NeuromodulationManager::applyCuriosityEffects() {
    if (!brain_->getCuriosity()) return;
    
    float curiosityLevel = brain_->getCuriosity()->getLevel();
    // Placeholder for curiosity effects implementation
}

void NeuromodulationManager::applyNoveltyEffects() {
    if (!brain_->getNovelty()) return;
    
    float noveltyLevel = brain_->getNovelty()->getLevel();
    // Placeholder for novelty effects implementation
}

void NeuromodulationManager::applyPredictionErrorEffects() {
    if (!brain_->getPredictionErrorSignal()) return;
    
    float predictionError = brain_->getPredictionErrorSignal()->getLevel();
    // Placeholder for prediction error effects implementation
}

void NeuromodulationManager::applyPlasticityFactors() {
    if (!brain_ || !brain_->getDopamine()) return;
    
    // Calculate plasticity modulation factor
    float plasticityMod = brain_->getDopamine()->getPlasticityFactor();
    
    // Update plasticity manager with current time
    if (brain_->getNeuralDynamics()) {
        brain_->getNeuralDynamics()->updateStep(
            brain_->getCurrentStep(), 
            brain_->getCurrentTime()
        );
    }
    
    // Apply plasticity rules to all regions
    brain_->getPlasticityManager()->applyToAllRegions(
        brain_->getRegions(), 
        brain_->getTimestep(), 
        plasticityMod, 
        brain_->getCurrentStep()
    );
    
    // Update statistics
    stats_.plasticityUpdateCount++;
}

} // namespace nlm