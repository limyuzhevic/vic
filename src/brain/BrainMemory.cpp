// Brain memory implementation
#include "BrainMemory.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainMemory::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (!brain || !brain->getWorkingMemory()) return;
    brain->getWorkingMemory()->update(timestep);
}

void BrainMemory::storeToWorkingMemory(Brain* brain, NeuronId neuronId, float activationLevel) {
    if (!brain || !brain->getWorkingMemory()) return;
    brain->getWorkingMemory()->storeToNeuron(neuronId, activationLevel);
}

void BrainMemory::updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain || !brain->getEpisodicMemory()) return;
    
    // This is now handled by BrainSimulation - calling this for backward compatibility
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        
        if (auto* dopamine = brain->getDopamine()) {
            episode.reward = dopamine->getLevel();
        }
        
        // Store active neurons
        for (auto& region : brain->getRegions()) {
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
        
        brain->getEpisodicMemory()->storeEpisode(episode);
    }
}

void BrainMemory::storeEpisode(Brain* brain, const EpisodicMemoryItem& episode) {
    if (!brain || !brain->getEpisodicMemory()) return;
    brain->getEpisodicMemory()->storeEpisode(episode);
}

std::vector<const EpisodicMemoryItem*> BrainMemory::getEpisodesForReplay(Brain* brain, size_t count) {
    if (!brain || !brain->getEpisodicMemory()) return {};
    return brain->getEpisodicMemory()->getEpisodesForReplay(count);
}

void BrainMemory::replayEpisode(Brain* brain, const EpisodicMemoryItem* episode) {
    if (!brain || !brain->getEpisodicMemory()) return;
    brain->getEpisodicMemory()->replayEpisode(episode);
}

void BrainMemory::consolidate(Brain* brain, float threshold) {
    if (!brain || !brain->getEpisodicMemory()) return;
    brain->getEpisodicMemory()->consolidate(threshold);
}

size_t BrainMemory::getActiveWorkingMemoryTraces(Brain* brain) {
    if (!brain || !brain->getWorkingMemory()) return 0;
    return brain->getWorkingMemory()->getActiveTraces();
}

size_t BrainMemory::getEpisodeCount(Brain* brain) {
    if (!brain || !brain->getEpisodicMemory()) return 0;
    return brain->getEpisodicMemory()->getEpisodeCount();
}

} // namespace nlm