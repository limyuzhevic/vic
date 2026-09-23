#include "MemoryManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

struct MemoryManager::Impl {
    Brain* brain{nullptr};
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    Impl() = default;
};

MemoryManager::MemoryManager() : pImpl(new Impl()) {}

MemoryManager::~MemoryManager() {
    delete pImpl;
}

bool MemoryManager::initialize(Brain* brain) {
    if (!brain) {
        return false;
    }
    
    pImpl->brain = brain;
    
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    status_.workingMemoryTraces = 0;
    status_.episodicMemoryEpisodes = 0;
    status_.associativeMemoryAssociations = 0;
    
    return true;
}

void MemoryManager::update(float timestep) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
        status_.workingMemoryTraces = pImpl->workingMemory->getActiveTraces();
    }
    
    // Update episodic memory if needed (during episodes)
    status_.episodicMemoryEpisodes = pImpl->episodicMemory ? 
        pImpl->episodicMemory->getEpisodeCount() : 0;
}

void MemoryManager::storeToNeuron(NeuronId neuronId, float value) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeToNeuron(neuronId, value);
        status_.workingMemoryTraces = pImpl->workingMemory->getActiveTraces();
    }
}

float MemoryManager::retrieveFromNeuron(NeuronId neuronId) const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->retrieveFromNeuron(neuronId);
    }
    return 0.0f;
}

bool MemoryManager::containsNeuron(NeuronId neuronId) const {
    if (pImpl->workingMemory) {
        return pImpl->workingMemory->containsNeuron(neuronId);
    }
    return false;
}

void MemoryManager::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->storeEpisode(episode);
        status_.episodicMemoryEpisodes = pImpl->episodicMemory->getEpisodeCount();
    }
}

std::vector<EpisodicMemoryItem*> MemoryManager::getEpisodesForReplay(size_t count) {
    std::vector<EpisodicMemoryItem*> episodes;
    if (pImpl->episodicMemory) {
        episodes = pImpl->episodicMemory->getEpisodesForReplay(count);
    }
    return episodes;
}

void MemoryManager::replayEpisode(const EpisodicMemoryItem* episode) {
    if (pImpl->episodicMemory && episode) {
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void MemoryManager::consolidate(float relevanceThreshold) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(relevanceThreshold);
        status_.episodicMemoryEpisodes = pImpl->episodicMemory->getEpisodeCount();
    }
}

size_t MemoryManager::getEpisodeCount() const {
    return pImpl->episodicMemory ? pImpl->episodicMemory->getEpisodeCount() : 0;
}

void MemoryManager::setWorkingMemoryCapacity(size_t capacity) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(capacity);
    }
}

void MemoryManager::setMaxEpisodes(size_t maxEpisodes) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

MemorySystemStatus MemoryManager::getStatus() const {
    return status_;
}

void MemoryManager::clear() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
    }
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->clear();
    }
    status_.workingMemoryTraces = 0;
    status_.episodicMemoryEpisodes = 0;
    status_.associativeMemoryAssociations = 0;
}

} // namespace nlm
