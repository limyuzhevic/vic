// MemorySystem.cpp - Implementation of memory system
#include "MemorySystem.h"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct MemorySystem::Impl {
    Brain* brain_ = nullptr;
    std::shared_ptr<Config> config;
    size_t updateCount = 0;
};

MemorySystem::MemorySystem() : pImpl(std::make_unique<Impl>()) {}

MemorySystem::~MemorySystem() = default;

bool MemorySystem::initialize(Brain* brain) {
    if (!brain) return false;
    
    pImpl->brain_ = brain;
    pImpl->config = brain->getConfig();
    pImpl->updateCount = 0;
    
    // Configure memory systems
    size_t workingMemCapacity = pImpl->config->getOr<size_t>("working_memory_capacity", 1000);
    size_t episodicMaxEpisodes = pImpl->config->getOr<size_t>("episodic_max_episodes", 10000);
    
    setCapacity(workingMemCapacity);
    setMaxEpisodes(episodicMaxEpisodes);
    
    // Initialize memory components
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    
    initialized_ = true;
    NLM_LOG_INFO("MemorySystem initialized");
    return true;
}

void MemorySystem::update(const TimestepDuration& dt) {
    if (!initialized_ || !pImpl->brain_) return;
    
    pImpl->updateCount++;
    
    // Update each memory component
    if (workingMemory_) {
        workingMemory_->update(dt);
    }
    
    if (episodicMemory_) {
        episodicMemory_->update(dt);
    }
    
    NLM_LOG_TRACE("MemorySystem updated, trace count: " + std::to_string(getActiveTraces()));
}

void MemorySystem::reset() {
    if (workingMemory_) workingMemory_->clear();
    if (episodicMemory_) episodicMemory_->clear();
    if (associativeMemory_) associativeMemory_->clear();
    
    initialized_ = false;
    pImpl->updateCount = 0;
    NLM_LOG_INFO("MemorySystem reset");
}

void MemorySystem::logStatus() const {
    NLM_LOG_INFO("=== Memory System Status ===");
    if (workingMemory_) {
        NLM_LOG_INFO("Working memory: " + std::to_string(getActiveTraces()) + " active traces");
    }
    if (episodicMemory_) {
        NLM_LOG_INFO("Episodic memory: " + std::to_string(getEpisodeCount()) + " episodes stored");
    }
}

void MemorySystem::storeToNeuron(NeuronId neuron, float value) {
    if (workingMemory_) {
        workingMemory_->store(neuron, value);
    }
}

float MemorySystem::retrieveFromNeuron(NeuronId neuron) const {
    if (workingMemory_) {
        return workingMemory_->retrieve(neuron);
    }
    return 0.0f;
}

void MemorySystem::decay(float decayRate) {
    if (workingMemory_) {
        workingMemory_->decay(decayRate);
    }
}

size_t MemorySystem::getActiveTraces() const {
    return workingMemory_ ? workingMemory_->getActiveTraces() : 0;
}

void MemorySystem::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episodicMemory_) {
        episodicMemory_->storeEpisode(episode);
    }
}

EpisodicMemoryItem MemorySystem::retrieveEpisode(size_t index) const {
    return episodicMemory_ ? episodicMemory_->retrieveEpisode(index) : EpisodicMemoryItem();
}

size_t MemorySystem::getEpisodeCount() const {
    return episodicMemory_ ? episodicMemory_->getEpisodeCount() : 0;
}

std::vector<EpisodicMemoryItem> MemorySystem::getRecentEpisodes(size_t count) const {
    return episodicMemory_ ? episodicMemory_->getRecentEpisodes(count) : std::vector<EpisodicMemoryItem>();
}

void MemorySystem::consolidate(float relevanceThreshold) {
    if (episodicMemory_) {
        episodicMemory_->consolidate(relevanceThreshold);
    }
}

void MemorySystem::replayEpisode(const EpisodicMemoryItem* episode) {
    if (episodicMemory_ && episode) {
        episodicMemory_->replayEpisode(*episode);
    }
}

std::vector<EpisodicMemoryItem> MemorySystem::getEpisodesForReplay(size_t count) const {
    return episodicMemory_ ? episodicMemory_->getEpisodesForReplay(count) : std::vector<EpisodicMemoryItem>();
}

void MemorySystem::associate(NeuronId a, NeuronId b, float strength) {
    if (associativeMemory_) {
        associativeMemory_->associate(a, b, strength);
    }
}

std::vector<NeuronId> MemorySystem::getAssociations(NeuronId neuron) const {
    return associativeMemory_ ? associativeMemory_->getAssociations(neuron) : std::vector<NeuronId>();
}

float MemorySystem::getAssociationStrength(NeuronId a, NeuronId b) const {
    return associativeMemory_ ? associativeMemory_->getAssociationStrength(a, b) : 0.0f;
}

void MemorySystem::updateAssociation(NeuronId a, NeuronId b, float delta) {
    if (associativeMemory_) {
        associativeMemory_->updateAssociation(a, b, delta);
    }
}

} // namespace nlm
