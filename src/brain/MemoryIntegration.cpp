#include "Integration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void MemoryIntegration::initialize(NeuralWorkingMemory* workingMemoryPtr,
                                   NeuralEpisodicMemory* episodicMemoryPtr,
                                   NeuralAssociativeMemory* associativeMemoryPtr,
                                   Brain* brainPtr) {
    workingMemory = workingMemoryPtr;
    episodicMemory = episodicMemoryPtr;
    associativeMemory = associativeMemoryPtr;
    brain = brainPtr;
    stepsSinceLastEpisode = 0;
    replayInterval = 100;
    consolidationInterval = 1000;
}

void MemoryIntegration::updateWorkingMemory(NeuralWorkingMemory* workingMemoryPtr, float timestep) {
    if (!workingMemoryPtr) return;
    
    // Step 4: Update working memory (maintenance and competition)
    workingMemoryPtr->update(timestep);
}

void MemoryIntegration::updateEpisodicMemory(NeuralEpisodicMemory* episodicMemoryPtr,
                                           Timestamp currentTime, SimulationStep currentStep,
                                           const std::vector<NeuralRegion*>& regions,
                                           Dopamine* dopamine) {
    if (!episodicMemoryPtr) return;
    
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : regions) {
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
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        episodicMemoryPtr->storeEpisode(episode);
    }
}

void MemoryIntegration::replayMemories(NeuralEpisodicMemory* episodicMemoryPtr, 
                                       SimulationStep currentStep, size_t replayInterval) {
    if (!episodicMemoryPtr) return;
    
    // Step 12: Replay important memories (during rest or periodically)
    if (currentStep % replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemoryPtr->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemoryPtr->replayEpisode(episode);
        }
    }
}

void MemoryIntegration::consolidateMemories(NeuralEpisodicMemory* episodicMemoryPtr, 
                                           SimulationStep currentStep, size_t consolidationInterval) {
    if (!episodicMemoryPtr) return;
    
    // Step 14: Periodic memory consolidation
    if (currentStep % consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemoryPtr->consolidate(0.3f);
    }
}

} // namespace nlm