// BrainMemoryManager.cpp - Implementation of BrainMemoryManager class

#include "BrainMemoryManager.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainMemoryManager::initializeWorkingMemory(Brain& brain) {
    brain.pImpl->workingMemory->initialize(&brain);
}

void BrainMemoryManager::initializeEpisodicMemory(Brain& brain) {
    brain.pImpl->episodicMemory->initialize(&brain);
}

void BrainMemoryManager::initializeAssociativeMemory(Brain& brain) {
    brain.pImpl->associativeMemory->initialize(&brain);
}

void BrainMemoryManager::updateWorkingMemory(Brain& brain) {
    if (brain.pImpl->workingMemory) {
        brain.pImpl->workingMemory->update(brain.pImpl->timestep);
    }
}

void BrainMemoryManager::storeEpisodicMemory(Brain& brain, SimulationStep currentStep) {
    // Update steps since last episode
    brain.pImpl->stepsSinceLastEpisode++;
    
    if (brain.pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brain.pImpl->stepsSinceLastEpisode = 0;
        
        if (brain.pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brain.pImpl->dopamine ? brain.pImpl->dopamine->getLevel() : 0.0f;
            
            // Capture active neurons
            captureActiveNeurons(brain, episode);
            
            brain.pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainMemoryManager::replayMemories(Brain& brain, SimulationStep currentStep) {
    // Replay important memories
    if (currentStep % brain.pImpl->replayInterval == 0 && brain.pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = brain.pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain.pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainMemoryManager::consolidateMemories(Brain& brain, SimulationStep currentStep) {
    // Periodic memory consolidation
    if (currentStep % brain.pImpl->consolidationInterval == 0 && brain.pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        brain.pImpl->episodicMemory->consolidate(0.3f);
    }
}

void BrainMemoryManager::clearAllMemories(Brain& brain) {
    if (brain.pImpl->workingMemory) brain.pImpl->workingMemory->clear();
    if (brain.pImpl->episodicMemory) brain.pImpl->episodicMemory->clear();
    if (brain.pImpl->associativeMemory) brain.pImpl->associativeMemory->clear();
}

void BrainMemoryManager::restoreWorkingMemorySnapshot(Brain& brain) {
    // Placeholder for working memory restoration
}

void BrainMemoryManager::restoreEpisodicMemoryEpisodes(Brain& brain) {
    // Placeholder for episodic memory restoration
}

void BrainMemoryManager::captureActiveNeurons(Brain& brain, const EpisodicMemoryItem& episode) {
    // Capture active neurons for episode
    for (auto& region : brain.pImpl->regions) {
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
}

float BrainMemoryManager::calculateNeuronActivation(Brain& brain, const Neuron* neuron) {
    return std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
}
