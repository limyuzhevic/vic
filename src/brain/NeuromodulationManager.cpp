#include "NeuromodulationManager.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NeuromodulationManager::NeuromodulationManager()
    : dopamine(nullptr), curiosity(nullptr), novelty(nullptr), predictionError(nullptr)
    , dopamineLevel(0.0f), currentDopamineLevel(0.0f), currentCuriosityLevel(0.0f)
    , currentNoveltyLevel(0.0f), plasticityFactor(1.0f) {}

NeuromodulationManager::~NeuromodulationManager() {}

void NeuromodulationManager::update(TimestepDuration dt) {
    updateDopamine(dt);
    updateCuriosity(dt);
    updateNovelty(dt);
    calculateNeuromodulationFactor();
}

void NeuromodulationManager::updateDopamine(TimestepDuration dt) {
    if (dopamine) {
        dopamine->update(dt);
        currentDopamineLevel = dopamine->getLevel();
    }
}

void NeuromodulationManager::updateCuriosity(TimestepDuration dt) {
    if (curiosity) {
        curiosity->update(dt);
        currentCuriosityLevel = curiosity->getLevel();
    }
}

void NeuromodulationManager::updateNovelty(TimestepDuration dt) {
    if (novelty) {
        novelty->update(dt);
        currentNoveltyLevel = novelty->getLevel();
    }
}

void NeuromodulationManager::calculateNeuromodulationFactor() {
    // Calculate overall neuromodulation factor based on all neuromodulators
    plasticityFactor = 1.0f;
    
    // Dopamine increases plasticity factor
    if (dopamine) {
        float dopamineLevel = dopamine->getPlasticityFactor();
        plasticityFactor *= dopamineLevel;
    }
    
    // Other neuromodulators could also affect plasticity
    // For now, just combine them
    if (curiosity) {
        float curiosityLevel = curiosity->getLevel();
        plasticityFactor *= (1.0f + curiosityLevel * 0.1f);
    }
    
    if (novelty) {
        float noveltyLevel = novelty->getLevel();
        plasticityFactor *= (1.0f + noveltyLevel * 0.1f);
    }
    
    // Clamp to reasonable range
    plasticityFactor = std::max(0.0f, std::min(plasticityFactor, 3.0f));
}

void NeuromodulationManager::applyGlobalNeuromodulation(float factor) {
    // Apply neuromodulation factor to neurons
    // This would be called from the action system
    // For now, just store it
    dopamineLevel = factor;
}

} // namespace nlm
