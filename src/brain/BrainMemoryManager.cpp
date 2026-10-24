#include "BrainMemoryManager.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct BrainMemoryManager::Impl {
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    size_t stepsSinceLastEpisode;
    
    Impl() : stepsSinceLastEpisode(0) {}
};

BrainMemoryManager::BrainMemoryManager() : pImpl(std::make_unique<Impl>()) {}

BrainMemoryManager::~BrainMemoryManager() = default;

void BrainMemoryManager::initialize(Brain* brain) {
    // Initialize memory systems through brain
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->initialize(brain);
    }
    if (brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->initialize(brain);
    }
    if (brain->getAssociativeMemory()) {
        brain->getAssociativeMemory()->initialize(brain);
    }
}

void BrainMemoryManager::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->update(timestep);
    }
}

void BrainMemoryManager::updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    pImpl->stepsSinceLastEpisode++;
    
    // Store episode every 10 steps (as per original logic)
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (brain->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
            
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
            
            // Store reward in episode
            episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
            
            brain->getEpisodicMemory()->storeEpisode(episode);
        }
    }
}

void BrainMemoryManager::applyNeuromodulationToMemory(Brain* brain, float neuromodulationLevel) {
    // Apply neuromodulation effects on memory systems
    if (brain->getWorkingMemory()) {
        // Dopamine enhances working memory consolidation
        if (neuromodulationLevel > 0.0f) {
            brain->getWorkingMemory()->setConsolidationFactor(neuromodulationLevel);
        }
    }
    
    if (brain->getEpisodicMemory()) {
        // Dopamine enhances episodic memory encoding
        if (neuromodulationLevel > 0.0f) {
            brain->getEpisodicMemory()->setEncodingStrength(neuromodulationLevel);
        }
    }
}

} // namespace nlm
