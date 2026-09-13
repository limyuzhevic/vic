#include "BrainNeuromodulation.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>

namespace nlm {

void BrainNeuromodulation::initialize(class Brain* brain) {
    brainRef = brain;
    dopamineLevel = 0.0f;
    curiosityLevel = 0.0f;
    noveltyLevel = 0.0f;
    predictionErrorLevel = 0.0f;
    
    if (brainRef) {
        dopamine = brainRef->getDopamine();
        curiosity = brainRef->getCuriosity();
        novelty = brainRef->getNovelty();
        predictionError = brainRef->getPredictionErrorSignal();
    }
}

void BrainNeuromodulation::update(float timestep) {
    // Update neuromodulation systems
    if (dopamine) dopamine->update(timestep);
    if (curiosity) curiosity->update(timestep);
    if (novelty) novelty->update(timestep);
    if (predictionError) predictionError->update(timestep);
    
    // Calculate integrated neuromodulation levels
    dopamineLevel = dopamine ? dopamine->getLevel() : 0.0f;
    curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
    noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
    predictionErrorLevel = predictionError ? predictionError->getLevel() : 0.0f;
}

void BrainNeuromodulation::applyDopamineEffects(class Brain* brain) {
    if (!brain || !dopamine) return;
    
    // Apply dopamine effects on neural excitability
    float level = dopamine->getLevel();
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Dopamine modulates excitability by injecting additional current
                float excitabilityMod = level * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

float BrainNeuromodulation::getDopamineLevel() const {
    return dopamineLevel;
}

float BrainNeuromodulation::getPlasticityFactor() const {
    // Plasticity factor based on dopamine and other neuromodulators
    float factor = 1.0f;
    
    if (dopamineLevel > 0.5f) {
        factor *= 1.2f;
    }
    
    if (curiosityLevel > 0.3f) {
        factor *= 1.1f;
    }
    
    if (noveltyLevel > 0.3f) {
        factor *= 1.05f;
    }
    
    return factor;
}

void BrainNeuromodulation::initializeCuriosity(class Brain* brain) {
    if (brain) {
        curiosity = brain->getCuriosity();
    }
}

void BrainNeuromodulation::update(float timestep) {
    if (curiosity) curiosity->update(timestep);
    curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
}

void BrainNeuromodulation::processNovelty(const std::vector<float>& pattern) {
    if (!novelty) return;
    
    // Process novelty detection based on pattern
    // In a real system, this would compare against stored patterns
    float novelty = 0.5f; // Simplified
    noveltyLevel = novelty;
    
    if (novelty) novelty->update(0.001f);
}

float BrainNeuromodulation::getCuriosityLevel() const {
    return curiosityLevel;
}

void BrainNeuromodulation::initializeNovelty(class Brain* brain) {
    if (brain) {
        novelty = brain->getNovelty();
    }
}

void BrainNeuromodulation::update(float timestep) {
    if (novelty) novelty->update(timestep);
    noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
}

float BrainNeuromodulation::getNoveltyLevel() const {
    return noveltyLevel;
}

void BrainNeuromodulation::initializePredictionError(class Brain* brain) {
    if (brain) {
        predictionError = brain->getPredictionErrorSignal();
    }
}

void BrainNeuromodulation::update(float timestep) {
    if (predictionError) predictionError->update(timestep);
    predictionErrorLevel = predictionError ? predictionError->getLevel() : 0.0f;
}

float BrainNeuromodulation::getPredictionError() const {
    return predictionErrorLevel;
}

} // namespace nlm