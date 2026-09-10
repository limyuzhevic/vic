#include "MemorySystemUpdater.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void MemorySystemUpdater::update(Brain& brain, SimulationStep currentStep,
                               Timestamp currentTime, TimestepDuration timestep) {
    // Step 4: Update working memory
    if (brain.pImpl->workingMemory) {
        brain.pImpl->workingMemory->update(brain.pImpl->timestep);
    }
    
    // Step 7: Update episodic memory
    brain.pImpl->stepsSinceLastEpisode++;
    if (brain.pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brain.pImpl->stepsSinceLastEpisode = 0;
        
        if (brain.pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brain.pImpl->dopamine ? brain.pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
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
            
            // Store reward in episode
            episode.reward = brain.pImpl->dopamine ? brain.pImpl->dopamine->getLevel() : 0.0f;
            
            brain.pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

} // namespace nlm
