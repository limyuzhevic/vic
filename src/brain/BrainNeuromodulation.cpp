#include "BrainHeaders.hpp"

namespace nlm {

void BrainNeuromodulation::updateNeuromodulationSystems(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (dopamine) dopamine->update(timestep);
    if (curiosity) curiosity->update(timestep);
    if (novelty) novelty->update(timestep);
    if (predictionError) predictionError->update(timestep);
}

void BrainNeuromodulation::updateDopamineSystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->update(timestep);
    }
}

void BrainNeuromodulation::updateCuriositySystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->update(timestep);
    }
}

void BrainNeuromodulation::updateNoveltySystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* novelty = brain->getNovelty();
    if (novelty) {
        novelty->update(timestep);
    }
}

void BrainNeuromodulation::updatePredictionErrorSystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* predictionError = brain->getPredictionErrorSignal();
    if (predictionError) {
        predictionError->update(timestep);
    }
}

float BrainNeuromodulation::calculatePlasticityModulation(Brain* brain) {
    if (!brain) return 1.0f;
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        return dopamine->getPlasticityFactor();
    }
    return 1.0f;
}

} // namespace nlm