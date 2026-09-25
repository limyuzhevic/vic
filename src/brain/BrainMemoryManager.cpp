#include "BrainMemoryManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

void BrainMemoryManager::initialize(Brain* brain) {
    if (!brain) return;
    
    size_t neuronCount = brain->getTotalNeuronCount();
    
    // Initialize working memory
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->initialize(brain);
        workingMemory->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        episodicMemory->initialize(brain);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        associativeMemory->initialize(brain);
    }
}

void BrainMemoryManager::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->update(timestep);
    }
}

void BrainMemoryManager::updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto* pImpl = brain->pImpl.get();
    
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
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
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainMemoryManager::collectMemoryStats(Brain* brain) const {
    if (!brain) return;
    
    // Memory statistics will be collected in logStatus()
}

void BrainMemoryManager::reset(Brain* brain) {
    if (!brain) return;
    
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->clear();
    }
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        episodicMemory->clear();
    }
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        associativeMemory->clear();
    }
}

void BrainMemoryManager::storeEpisode(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    updateEpisodicMemory(brain, currentStep, currentTime);
}

void BrainMemoryManager::replayEpisodes(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    auto* pImpl = brain->pImpl.get();
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainMemoryManager::consolidateMemories(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    auto* pImpl = brain->pImpl.get();
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

} // namespace nlm
