// Brain memory system implementation
// Handles memory system initialization and operations

#include "brain/BrainMemorySystem.hpp"
#include "brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

void BrainMemorySystem::initializeIntegratedSystems(Brain* brain) {
    // Initialize working memory
    if (!brain->pImpl->workingMemory) {
        brain->pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    }
    brain->pImpl->workingMemory->initialize(brain);
    brain->pImpl->workingMemory->setCapacity(brain->getTotalNeuronCount() / 10);
    
    // Initialize episodic memory
    if (!brain->pImpl->episodicMemory) {
        brain->pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    }
    brain->pImpl->episodicMemory->initialize(brain);
    brain->pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    if (!brain->pImpl->associativeMemory) {
        brain->pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    }
    brain->pImpl->associativeMemory->initialize(brain);
}

void BrainMemorySystem::updateEpisodicMemory(Brain* brain, Timestamp currentTime) {
    brain->pImpl->stepsSinceLastEpisode++;
    if (brain->pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brain->pImpl->stepsSinceLastEpisode = 0;
        
        if (brain->pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = brain->pImpl->currentStep;
            episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
            
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
            
            // Store reward in episode
            episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
            
            brain->pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

} // namespace nlm
