#include "BrainHeaders.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainMemory::initializeWorkingMemory(Brain* brain, size_t neuronCount) {
    if (!brain) return;
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        workingMem->initialize(brain);
        workingMem->setCapacity(neuronCount / 10);
    }
}

void BrainMemory::initializeEpisodicMemory(Brain* brain) {
    if (!brain) return;
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        episodicMem->initialize(brain);
        episodicMem->setMaxEpisodes(1000);
    }
}

void BrainMemory::initializeAssociativeMemory(Brain* brain) {
    if (!brain) return;
    auto* associativeMem = brain->getAssociativeMemory();
    if (associativeMem) {
        associativeMem->initialize(brain);
    }
}

void BrainMemory::updateMemorySystems(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        workingMem->update(timestep);
    }
    
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        episodicMem->update(timestep);
    }
}

void BrainMemory::storeToWorkingMemory(Brain* brain, NeuronId neuronId, float activation) {
    if (!brain) return;
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        workingMem->storeToNeuron(neuronId, activation);
    }
}

void BrainMemory::replayMemory(Brain* brain) {
    if (!brain) return;
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        episodicMem->replay();
    }
}

void BrainMemory::consolidateMemory(Brain* brain) {
    if (!brain) return;
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        episodicMem->consolidate();
    }
}

} // namespace nlm