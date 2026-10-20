#include "brain/BrainMemory.hpp"
#include "brain/Brain.hpp"

namespace nlm {

void BrainMemory::updateWorkingMemory(NeuralWorkingMemory* workingMemory,
                                     std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                     TimestepDuration timestep) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void BrainMemory::updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory,
                                      std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                      SimulationStep currentStep,
                                      size_t stepsSinceLastEpisode,
                                      Dopamine* dopamine) {
    if (!episodicMemory) return;
    
    // Store episode every 10 steps
    if (stepsSinceLastEpisode >= 10) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : regions) {
            if (!region) continue;
            
            for (auto& population : region->getPopulations()) {
                if (!population) continue;
                
                for (auto* neuron : population->getNeurons()) {
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
        
        episodicMemory->storeEpisode(episode);
    }
}

void BrainMemory::handleReplayAndConsolidation(NeuralEpisodicMemory* episodicMemory,
                                              size_t currentStep,
                                              size_t replayInterval,
                                              size_t consolidationInterval) {
    if (!episodicMemory) return;
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(0.3f);
    }
}

void BrainMemory::initializeMemorySystems(NeuralWorkingMemory* workingMemory,
                                         NeuralEpisodicMemory* episodicMemory,
                                         NeuralAssociativeMemory* associativeMemory,
                                         size_t neuronCount,
                                         Brain* brain) {
    if (!workingMemory || !brain) return;
    
    // Initialize working memory
    workingMemory->initialize(brain);
    workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    if (episodicMemory) {
        episodicMemory->initialize(brain);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (associativeMemory) {
        associativeMemory->initialize(brain);
    }
}

} // namespace nlm
