#include "BrainMemory.hpp"
#include "Brain.hpp"

namespace nlm {

// Brain::Impl memory - BrainMemory class implementation

BrainMemory::BrainMemory(Brain::Impl* impl) : pImpl(impl) {}

void BrainMemory::integrateMemorySystems(Brain* brain) {
    // ========== INTEGRATE ALL MEMORY SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(brain);
    pImpl->workingMemory->setCapacity(1000);  // Default capacity, will be updated in Brain::initialize()
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(brain);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(brain);
}

void BrainMemory::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
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

void BrainMemory::replayMemories(SimulationStep currentStep) {
    // Replay important memories
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainMemory::consolidateMemories(SimulationStep currentStep) {
    // Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

} // namespace nlm
