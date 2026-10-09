#include "MemorySystem.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

MemorySystem::MemorySystem(Brain* brain, 
                           NeuralWorkingMemory* workingMemory,
                           NeuralEpisodicMemory* episodicMemory,
                           NeuralAssociativeMemory* associativeMemory)
    : brain(brain), workingMemory(workingMemory), episodicMemory(episodicMemory),
      associativeMemory(associativeMemory), stepsSinceLastEpisode(0),
      consolidationInterval(1000), replayEnabled(true) {
}

void MemorySystem::updateWorkingMemory(TimestepDuration dt) {
    if (workingMemory) {
        workingMemory->update(dt);
    }
}

void MemorySystem::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    stepsSinceLastEpisode++;
    
    // Store episode every 10 steps (as per current Brain implementation)
    if (stepsSinceLastEpisode >= 10) {
        stepsSinceLastEpisode = 0;
        
        if (episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = 0.0f;  // Would get from dopamine
            
            // Store active neurons
            for (auto& region : brain->pImpl->regions) {
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
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

void MemorySystem::storeCurrentEpisode(SimulationStep currentStep, Timestamp currentTime) {
    // Alias for updateEpisodicMemory with automatic stepping
    updateEpisodicMemory(currentStep, currentTime);
}

void MemorySystem::consolidateMemories(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

void MemorySystem::replayMemories(size_t count, SimulationStep currentStep) {
    if (episodicMemory && replayEnabled) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(count);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void MemorySystem::updateDevelopment(double timestep) {
    // Memory systems can have developmental effects
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

size_t MemorySystem::getWorkingMemoryActiveTraces() const {
    return workingMemory ? workingMemory->getActiveTraces() : 0;
}

size_t MemorySystem::getEpisodicMemoryEpisodeCount() const {
    return episodicMemory ? episodicMemory->getEpisodeCount() : 0;
}

float MemorySystem::getMemoryConsolidationLevel() const {
    // Simple placeholder - could be based on episode count and consolidation progress
    return 0.5f;
}

void MemorySystem::clearAllMemories() {
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
}

void MemorySystem::enableReplay(bool enable) {
    replayEnabled = enable;
}

void MemorySystem::setWorkingMemoryCapacity(size_t capacity) {
    if (workingMemory) workingMemory->setCapacity(capacity);
}

void MemorySystem::setEpisodicMemoryMaxEpisodes(size_t maxEpisodes) {
    if (episodicMemory) episodicMemory->setMaxEpisodes(maxEpisodes);
}

void MemorySystem::setConsolidationInterval(size_t interval) {
    consolidationInterval = interval;
}

} // namespace nlm