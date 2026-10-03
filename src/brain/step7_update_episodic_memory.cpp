// Step 7: Update episodic memory
// This module updates episodic memory with current experience
// Stores episodes of neural activity and reward signals

#include "step7_update_episodic_memory.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

namespace Step7 {

void CaptureBrainStateForEpisode(Brain::Impl* impl,
                                 SimulationStep currentStep,
                                 Timestamp currentTime,
                                 EpisodicMemoryItem& episode) {
    if (!impl) {
        NLM_LOG_ERROR("CaptureBrainStateForEpisode called with null impl pointer");
        return;
    }
    
    episode.timestamp = currentStep;
    episode.activeNeurons.clear();
    episode.neuronActivations.clear();
    
    // Capture active neurons from all regions
    ExtractActiveNeurons(impl->regions, episode.activeNeurons, episode.neuronActivations);
    
    // Store reward in episode
    UpdateEpisodeReward(episode, impl->dopamine.get());
}

void ExtractActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                          std::vector<NeuronId>& activeNeurons,
                          std::vector<float>& activations) {
    for (const auto& region : regions) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered during episode capture");
            continue;
        }
        
        for (const auto& pop : region->getPopulations()) {
            if (!pop) {
                continue;
            }
            
            for (const auto* neuron : pop->getNeurons()) {
                if (!neuron) {
                    continue;
                }
                
                if (neuron->isFiring()) {
                    activeNeurons.push_back(neuron->getId());
                    activations.push_back(1.0f);
                } else {
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential);
                    if (activation > 5.0f) {
                        activeNeurons.push_back(neuron->getId());
                        activations.push_back(activation / 20.0f);
                    }
                }
            }
        }
    }
}

void UpdateEpisodeReward(EpisodicMemoryItem& episode, Dopamine* dopamine) {
    episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
}

void StoreCurrentEpisode(Brain::Impl* impl,
                         SimulationStep currentStep,
                         Timestamp currentTime) {
    if (!impl || !impl->episodicMemory) {
        return;
    }
    
    if (impl->stepsSinceLastEpisode < 10) {
        return; // Don't store episode too frequently
    }
    
    EpisodicMemoryItem episode;
    CaptureBrainStateForEpisode(impl, currentStep, currentTime, episode);
    
    impl->episodicMemory->storeEpisode(episode);
    impl->stepsSinceLastEpisode = 0;
}

void UpdateEpisodicMemory(Brain::Impl* impl,
                          SimulationStep currentStep,
                          Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("UpdateEpisodicMemory called with null impl pointer");
        return;
    }
    
    if (!impl->episodicMemory) {
        NLM_LOG_DEBUG("Episodic memory not initialized, skipping update");
        return;
    }
    
    NLM_LOG_DEBUG("Updating episodic memory (Step " + std::to_string(currentStep) + ")");
    
    impl->stepsSinceLastEpisode++;
    
    if (impl->stepsSinceLastEpisode >= 10) {
        StoreCurrentEpisode(impl, currentStep, currentTime);
    }
}

} // namespace Step7

} // namespace nlm