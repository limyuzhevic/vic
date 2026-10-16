#include "BrainMemoryManager.hpp"
#include "BrainNeuralCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct BrainMemoryManager::Impl {
    std::shared_ptr<Config> config;
    
    // Memory system components
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Memory parameters
    size_t workingMemoryCapacity;
    size_t episodicMemoryMaxEpisodes;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , workingMemoryCapacity(1000)
        , episodicMemoryMaxEpisodes(1000)
    {}
};

BrainMemoryManager::BrainMemoryManager(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainMemoryManager::~BrainMemoryManager() = default;

bool BrainMemoryManager::initialize(BrainNeuralCore* neuralCore) {
    NLM_LOG_INFO("Initializing NLM Brain Memory Manager...");
    
    // Initialize memory systems
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize with neural core reference
    workingMemory->initialize(neuralCore);
    episodicMemory->initialize(neuralCore);
    associativeMemory->initialize(neuralCore);
    
    // Configure from config
    setWorkingMemoryCapacity(pImpl->config->getOr<size_t>("working_memory_capacity", 1000));
    setEpisodicMemoryMaxEpisodes(pImpl->config->getOr<size_t>("episodic_memory_max_episodes", 1000));
    
    NLM_LOG_INFO("NLM Brain Memory Manager initialization complete");
    return true;
}

void BrainMemoryManager::update(TimestepDuration timestep) {
    // Update working memory
    if (workingMemory) {
        workingMemory->update(timestep);
    }
    
    // Working memory updates happen automatically during spike processing
    // No need for additional update here
}

void BrainMemoryManager::storeToWorkingMemory(NeuronId neuronId, float activation) {
    if (workingMemory && activation > 0.1f) {
        workingMemory->storeToNeuron(neuronId, activation);
    }
}

void BrainMemoryManager::updateEpisodicMemory(const EpisodicMemoryItem& episode) {
    if (episodicMemory) {
        episodicMemory->storeEpisode(episode);
    }
}

void BrainMemoryManager::storeAssociativeMemory(const std::vector<NeuronId>& pattern1,
                                               const std::vector<NeuronId>& pattern2,
                                               float strength) {
    if (associativeMemory && strength > 0.01f) {
        associativeMemory->storeAssociation(pattern1, pattern2, strength);
    }
}

std::vector<NeuronId> BrainMemoryManager::retrieveFromWorkingMemory(size_t maxTraces) const {
    if (workingMemory) {
        return workingMemory->getMemoryNeurons();
    }
    return std::vector<NeuronId>();
}

const std::vector<EpisodicMemoryItem>* BrainMemoryManager::getEpisodesForReplay(size_t maxEpisodes) const {
    if (episodicMemory) {
        return episodicMemory->getEpisodesForReplay(maxEpisodes);
    }
    return nullptr;
}

float BrainMemoryManager::retrieveAssociativeMemory(const std::vector<NeuronId>& pattern) const {
    if (associativeMemory) {
        return associativeMemory->retrieveAssociation(pattern);
    }
    return 0.0f;
}

void BrainMemoryManager::clearAllMemory() {
    NLM_LOG_INFO("Clearing all memory systems...");
    
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    
    NLM_LOG_INFO("All memory systems cleared");
}

void BrainMemoryManager::consolidateMemory(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

void BrainMemoryManager::replayEpisodes(BrainNeuralCore* neuralCore, size_t episodeCount) {
    if (episodicMemory) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(episodeCount);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

NeuralWorkingMemory* BrainMemoryManager::getWorkingMemory() {
    return workingMemory.get();
}

const NeuralWorkingMemory* BrainMemoryManager::getWorkingMemory() const {
    return workingMemory.get();
}

NeuralEpisodicMemory* BrainMemoryManager::getEpisodicMemory() {
    return episodicMemory.get();
}

const NeuralEpisodicMemory* BrainMemoryManager::getEpisodicMemory() const {
    return episodicMemory.get();
}

NeuralAssociativeMemory* BrainMemoryManager::getAssociativeMemory() {
    return associativeMemory.get();
}

const NeuralAssociativeMemory* BrainMemoryManager::getAssociativeMemory() const {
    return associativeMemory.get();
}

size_t BrainMemoryManager::getWorkingMemorySize() const {
    return workingMemory ? workingMemory->getActiveTraces() : 0;
}

size_t BrainMemoryManager::getEpisodicMemorySize() const {
    return episodicMemory ? episodicMemory->getEpisodeCount() : 0;
}

size_t BrainMemoryManager::getAssociativeMemorySize() const {
    return associativeMemory ? associativeMemory->getAssociationCount() : 0;
}

void BrainMemoryManager::setWorkingMemoryCapacity(size_t capacity) {
    pImpl->workingMemoryCapacity = capacity;
    if (workingMemory) {
        workingMemory->setCapacity(capacity);
    }
}

void BrainMemoryManager::setEpisodicMemoryMaxEpisodes(size_t maxEpisodes) {
    pImpl->episodicMemoryMaxEpisodes = maxEpisodes;
    if (episodicMemory) {
        episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

void BrainMemoryManager::reset() {
    NLM_LOG_INFO("Resetting NLM Brain Memory Manager...");
    
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    
    NLM_LOG_INFO("NLM Brain Memory Manager reset complete");
}

std::shared_ptr<const Config> BrainMemoryManager::getConfig() const {
    return pImpl->config;
}

void BrainMemoryManager::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Memory Manager Status ===");
    NLM_LOG_INFO("Working memory traces: " + std::to_string(getWorkingMemorySize()));
    NLM_LOG_INFO("Working memory capacity: " + std::to_string(pImpl->workingMemoryCapacity));
    NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(getEpisodicMemorySize()));
    NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(pImpl->episodicMemoryMaxEpisodes));
    NLM_LOG_INFO("Associative memory associations: " + std::to_string(getAssociativeMemorySize()));
}

} // namespace nlm