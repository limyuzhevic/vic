// NLM/熙然 - Brain.cpp Method Implementation
// This file implements the critical Brain methods that were missing

// Main implementation file for Brain class
// Contains the step() method and all getter functions

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Add the missing getter method implementations

// Memory Systems
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// Prediction System
PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// Cognition Systems
NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// Development System
DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

// Neuromodulation Systems
Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

// Core systems
RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// Checkpointing system implementations

bool Brain::save(const std::string& filepath) const {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for saving");
        return false;
    }
    return pImpl->checkpointManager->save(pImpl, filepath);
}

bool Brain::load(const std::string& filepath) {
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for loading");
        return false;
    }
    return pImpl->checkpointManager->load(pImpl, filepath);
}

// Additional method implementations can be added here as needed

} // namespace nlm