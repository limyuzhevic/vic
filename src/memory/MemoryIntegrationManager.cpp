#include "MemoryIntegrationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

struct MemoryIntegrationManager::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Integration state
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    Impl(std::shared_ptr<Config> cfg) 
        : config(cfg)
        , brain(nullptr)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000) {
        
        // Configure intervals from config
        replayInterval = cfg->getOr<size_t>("replay_interval", 100);
        consolidationInterval = cfg->getOr<size_t>("consolidation_interval", 1000);
    }
    
    ~Impl() = default;
};

MemoryIntegrationManager::MemoryIntegrationManager(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

MemoryIntegrationManager::~MemoryIntegrationManager() {
    delete pImpl;
}

bool MemoryIntegrationManager::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing MemoryIntegrationManager...");
    
    pImpl->brain = brain;
    
    // Initialize working memory
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->workingMemory->initialize(brain);
    pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("working_memory_capacity", 1000));
    
    // Initialize episodic memory
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->episodicMemory->initialize(brain);
    pImpl->episodicMemory->setMaxEpisodes(pImpl->config->getOr<size_t>("max_episodes", 1000));
    
    // Initialize associative memory
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    pImpl->associativeMemory->initialize(brain);
    
    NLM_LOG_INFO("MemoryIntegrationManager initialized successfully");
    return true;
}

void MemoryIntegrationManager::update(Timestamp time, TimestepDuration timestep) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
    
    // Update episodic memory storage
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->brain->getCurrentStep();
            episode.reward = pImpl->brain->getDopamine() ? pImpl->brain->getDopamine()->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void MemoryIntegrationManager::replay(Timestamp time, TimestepDuration timestep) {
    if (pImpl->currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void MemoryIntegrationManager::consolidate(float consolidationStrength) {
    if (pImpl->currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(consolidationStrength);
    }
}

size_t MemoryIntegrationManager::getActiveMemoryTraces() const {
    return pImpl->workingMemory ? pImpl->workingMemory->getActiveTraces() : 0;
}

size_t MemoryIntegrationManager::getEpisodeCount() const {
    return pImpl->episodicMemory ? pImpl->episodicMemory->getEpisodeCount() : 0;
}

size_t MemoryIntegrationManager::getMemoryNeurons() const {
    return pImpl->workingMemory ? pImpl->workingMemory->getMemoryNeurons() : 0;
}

void MemoryIntegrationManager::reset() {
    NLM_LOG_INFO("Resetting MemoryIntegrationManager...");
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    NLM_LOG_INFO("MemoryIntegrationManager reset complete");
}

} // namespace nlm