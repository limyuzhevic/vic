#include "MemorySystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct MemorySystem::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    Brain* brain;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr) {}
};

MemorySystem::MemorySystem(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

MemorySystem::~MemorySystem() = default;

bool MemorySystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize working memory
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    if (workingMemory) {
        workingMemory->initialize(brain);
    }
    
    // Initialize episodic memory
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    if (episodicMemory) {
        episodicMemory->initialize(brain);
    }
    
    // Initialize associative memory
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    if (associativeMemory) {
        associativeMemory->initialize(brain);
    }
    
    NLM_LOG_INFO("MemorySystem initialized");
    return true;
}

void MemorySystem::update(double timestep) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void MemorySystem::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episodicMemory) {
        episodicMemory->storeEpisode(episode);
    }
}

const EpisodicMemoryItem* MemorySystem::getEpisode(size_t index) const {
    if (episodicMemory) {
        return episodicMemory->getEpisode(index);
    }
    return nullptr;
}

size_t MemorySystem::getEpisodeCount() const {
    if (episodicMemory) {
        return episodicMemory->getEpisodeCount();
    }
    return 0;
}

const std::vector<const EpisodicMemoryItem*>& MemorySystem::getEpisodesForReplay(size_t maxCount) const {
    static const std::vector<const EpisodicMemoryItem*> empty;
    if (episodicMemory) {
        return episodicMemory->getEpisodesForReplay(maxCount);
    }
    return empty;
}

void MemorySystem::replayEpisode(const EpisodicMemoryItem* episode) {
    if (episodicMemory && episode) {
        episodicMemory->replayEpisode(episode);
    }
}

void MemorySystem::consolidate(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

void MemorySystem::clear() {
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
}

void MemorySystem::setWorkingMemoryCapacity(size_t capacity) {
    if (workingMemory) {
        workingMemory->setCapacity(capacity);
    }
}

void MemorySystem::setEpisodicMemoryCapacity(size_t maxEpisodes) {
    if (episodicMemory) {
        episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

} // namespace nlm
