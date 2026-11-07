#include "MemorySystemsManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MemorySystemsManager::MemorySystemsManager(Brain* brain)
    : brain_(brain)
    , workingMemory(std::make_unique<NeuralWorkingMemory>())
    , episodicMemory(std::make_unique<NeuralEpisodicMemory>())
    , associativeMemory(std::make_unique<NeuralAssociativeMemory>()) {
}

MemorySystemsManager::~MemorySystemsManager() = default;

bool MemorySystemsManager::initialize() {
    NLM_LOG_INFO("Initializing Memory Systems Manager...");
    
    if (!brain_) return false;
    
    // Initialize memory systems with brain reference
    workingMemory->initialize(brain_);
    episodicMemory->initialize(brain_);
    associativeMemory->initialize(brain_);
    
    NLM_LOG_INFO("Memory Systems Manager initialized successfully");
    return true;
}

void MemorySystemsManager::reset() {
    workingMemory->clear();
    episodicMemory->clear();
    associativeMemory->clear();
}

void MemorySystemsManager::step(double timestep) {
    updateWorkingMemory(timestep);
}

void MemorySystemsManager::updateWorkingMemory(double timestep) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void MemorySystemsManager::updateEpisodicMemory(SimulationStep currentStep) {
    // Store episode every 10 steps
    static SimulationStep lastEpisodeStep = 0;
    
    if (currentStep - lastEpisodeStep >= 10) {
        lastEpisodeStep = currentStep;
        
        if (episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0. sportivoValue = 0.0f;
            
            // Capture current brain state as episode
            for (auto& region : brain_->pImpl->regions) {
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
            
            episode.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f;
            episodicMemory->storeEpisode(episode);
        }
    }
}

void MemorySystemsManager::storeToWorkingMemory(NeuronId neuron, float activation) {
    if (workingMemory) {
        workingMemory->storeToNeuron(neuron, activation);
    }
}

void MemorySystemsManager::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episodicMemory) {
        episodicMemory->storeEpisode(episode);
    }
}

std::vector<NeuronId> MemorySystemsManager::getWorkingMemoryNeurons() const {
    if (workingMemory) {
        return workingMemory->getMemoryNeurons();
    }
    return {};
}

std::vector<EpisodicMemoryItem*> MemorySystemsManager::getEpisodesForReplay(size_t count) {
    if (episodicMemory) {
        return episodicMemory->getEpisodesForReplay(count);
    }
    return {};
}

void MemorySystemsManager::consolidateMemories(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

} // namespace nlm
