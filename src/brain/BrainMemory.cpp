// Brain memory implementation - handles all memory system components
#include "BrainMemory.hpp"
#include "Brain.hpp"
#include "MemorySystem.hpp"
#include <stdexcept>

namespace nlm {

BrainMemory::BrainMemory(std::shared_ptr<Config> config) : Brain(config) {
    memorySystem = std::make_unique<MemorySystem>(*this);
}

BrainMemory::~BrainMemory() = default;

bool BrainMemory::initializeMemorySystems() {
    if (!memorySystem) {
        return false;
    }
    
    return memorySystem->initialize();
}

void BrainMemory::updateMemorySystems(TimestepDuration dt) {
    if (memorySystem) {
        memorySystem->update(dt);
    }
}

bool BrainMemory::initialize() {
    if (!Brain::initialize()) {
        return false;
    }
    
    if (!initializeMemorySystems()) {
        return false;
    }
    
    return true;
}

void BrainMemory::step(SimulationStep currentStep) {
    Brain::step(currentStep);
}

void BrainMemory::step(SimulationStep currentStep, Timestamp currentTime) {
    Brain::step(currentStep, currentTime);
    
    if (memorySystem) {
        memorySystem->update(static_cast<TimestepDuration>(currentTime));
    }
}

NeuralWorkingMemory* BrainMemory::getWorkingMemory() {
    if (memorySystem) {
        return memorySystem->getWorkingMemory();
    }
    return nullptr;
}

NeuralEpisodicMemory* BrainMemory::getEpisodicMemory() {
    if (memorySystem) {
        return memorySystem->getEpisodicMemory();
    }
    return nullptr;
}

NeuralAssociativeMemory* BrainMemory::getAssociativeMemory() {
    if (memorySystem) {
        return memorySystem->getAssociativeMemory();
    }
    return nullptr;
}

} // namespace nlm
