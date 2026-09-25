#include "BrainNeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

void BrainNeuromodulationManager::initialize(Brain* brain) {
    // Initialize neuromodulation systems
    if (auto* novelty = brain->getNovelty()) {
        novelty->initialize(brain);
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->initialize(brain);
    }
    
    // Dopamine doesn't have initialize method
}

void BrainNeuromodulationManager::update(Brain* brain, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
    }
    
    // Update prediction error
    if (pImpl->predictionError) {
        pImpl->predictionError->update(timestep);
    }
}

void BrainNeuromodulationManager::applyNeuromodulationEffects(Brain* brain) {
    auto* pImpl = brain->pImpl.get();
    
    if (pImpl->dopamine) {
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainNeuromodulationManager::updateCuriosity(Brain* brain) {
    if (!brain) return;
    
    auto* pImpl = brain->pImpl.get();
    if (pImpl->curiosity) {
        // Get novelty level from novelty detector
        float noveltyLevel = pImpl->novelty ? pImpl->novelty->getNoveltyLevel() : 0.0f;
        
        // Get prediction error
        float predictionError = pImpl->predictionError ? 
                                pImpl->predictionError->getPredictionError() : 0.0f;
        
        // Update curiosity
        pImpl->curiosity->setCuriosityLevel(noveltyLevel * 2.0f + std::abs(predictionError) * 0.5f);
    }
}

void BrainNeuromodulationManager::applyDopaminePlasticity(Brain* brain) {
    auto* pImpl = brain->pImpl.get();
    
    if (pImpl->dopamine) {
        // Scale STDP learning rates
        float plasticityFactor = pImpl->dopamine->getPlasticityFactor();
        
        if (pImpl->stdp) {
            pImpl->stdp->setLTPWeight(0.01f * plasticityFactor);
            pImpl->stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
}

void BrainNeuromodulationManager::collectNeuromodulationStats(Brain* brain) const {
    if (!brain) return;
    
    auto* pImpl = brain->pImpl.get();
    
    // Neuromodulation statistics will be collected in logStatus()
}

} // namespace nlm
