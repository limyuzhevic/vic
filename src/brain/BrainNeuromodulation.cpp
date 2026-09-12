// Brain neuromodulation implementation
#include "BrainNeuromodulation.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainNeuromodulation::updateNeuromodulation(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Update novelty detection
    if (auto* novelty = brain->getNovelty()) {
        novelty->update(timestep);
    }
    
    // Update curiosity
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->update(timestep);
    }
}

void BrainNeuromodulation::applyNeuromodulation(Brain* brain, const Neuromodulator& neuromodulator) {
    if (!brain) return;
    brain->applyNeuromodulation(neuromodulator);
}

float BrainNeuromodulation::getDopamineLevel(Brain* brain) {
    if (!brain || !brain->getDopamine()) return 0.0f;
    return brain->getDopamine()->getLevel();
}

float BrainNeuromodulation::getCuriosityLevel(Brain* brain) {
    if (!brain || !brain->getCuriosity()) return 0.0f;
    return brain->getCuriosity()->getLevel();
}

float BrainNeuromodulation::getNoveltyLevel(Brain* brain) {
    if (!brain || !brain->getNovelty()) return 0.0f;
    return brain->getNovelty()->getLevel();
}

float BrainNeuromodulation::getPredictionErrorLevel(Brain* brain) {
    if (!brain || !brain->getPredictionErrorSignal()) return 0.0f;
    return brain->getPredictionErrorSignal()->getLevel();
}

float BrainNeuromodulation::getPlasticityFactor(Brain* brain) {
    if (!brain || !brain->getDopamine()) return 1.0f;
    return brain->getDopamine()->getPlasticityFactor();
}

} // namespace nlm