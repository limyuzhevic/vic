// Memory system implementation - handles working memory, episodic memory, and associative memory
#include "MemorySystem.hpp"
#include "Brain.hpp"
#include <memory>

namespace nlm {

MemorySystem::MemorySystem(Brain& brainRef) : brain(brainRef) {
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
}

MemorySystem::~MemorySystem() = default;

bool MemorySystem::initialize() {
    if (!brain.getWorkingMemory() || !brain.getEpisodicMemory() || !brain.getAssociativeMemory()) {
        return false;
    }
    
    return true;
}

void MemorySystem::update(TimestepDuration dt) {
    if (workingMemory) {
        workingMemory->update(dt);
    }
    if (episodicMemory) {
        episodicMemory->update(dt);
    }
    if (associativeMemory) {
        associativeMemory->update(dt);
    }
}

size_t MemorySystem::getWorkingMemorySize() const {
    return workingMemory ? workingMemory->getActiveTraces() : 0;
}

size_t MemorySystem::getEpisodicMemorySize() const {
    return episodicMemory ? episodicMemory->getEpisodeCount() : 0;
}

size_t MemorySystem::getAssociativeMemorySize() const {
    return associativeMemory ? associativeMemory->getAssociationCount() : 0;
}

void MemorySystem::storeObservation(const class Observation& observation) {
    if (workingMemory) {
        workingMemory->storeObservation(observation);
    }
    if (episodicMemory) {
        episodicMemory->storeObservation(observation);
    }
    if (associativeMemory) {
        associativeMemory->storeObservation(observation);
    }
}

void MemorySystem::storeAction(const class Action& action) {
    if (workingMemory) {
        workingMemory->storeAction(action);
    }
    if (episodicMemory) {
        episodicMemory->storeAction(action);
    }
    if (associativeMemory) {
        associativeMemory->storeAction(action);
    }
}

} // namespace nlm
