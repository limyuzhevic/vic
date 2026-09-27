// BrainMain.cpp - Enhanced with memory integration
#include "BrainMain.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations for BrainMain implementation
struct BrainMain::Impl {
    Brain* brain;
    
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    Impl() 
        : brain(nullptr)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000) {}
};

BrainMain::BrainMain() : pImpl(new Impl()) {}

BrainMain::~BrainMain() = default;

void BrainMain::initializeSensoryNeurons(std::vector<Neuron*>& sensoryNeurons, 
                                       std::vector<Neuron*>& motorNeurons) {
    if (!pImpl) return;
    pImpl->sensoryNeurons = sensoryNeurons;
    pImpl->motorNeurons = motorNeurons;
}

void BrainMain::storeToWorkingMemory(NeuronId neuronId, float activation) {
    if (!pImpl || !pImpl->brain) return;
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->storeToNeuron(neuronId, activation);
    }
}

void BrainMain::updateWorkingMemory(double timestep) {
    if (!pImpl || !pImpl->brain) return;
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void BrainMain::updateEpisodicMemory(SimulationStep step, Timestamp time) {
    if (!pImpl || !pImpl->brain) return;
    auto* episodicMemory = pImpl->brain->getEpisodicMemory();
    if (!episodicMemory) return;
    
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = step;
        episode.reward = pImpl->brain->getDopamine() ? 
            pImpl->brain->getDopamine()->getLevel() : 0.0f;
        
        // Store active neurons
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store the episode
        episodicMemory->storeEpisode(episode);
        
        // Process memory consolidation periodically
        if (step % pImpl->consolidationInterval == 0) {
            episodicMemory->consolidate(0.3f);
        }
    }
}

void BrainMain::processMemoryReplay(SimulationStep step) {
    if (!pImpl || !pImpl->brain) return;
    auto* episodicMemory = pImpl->brain->getEpisodicMemory();
    if (!episodicMemory) return;
    
    // Replay important memories periodically
    if (step % pImpl->replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainMain::updatePredictionSystem(const class SensoryInput& input) {
    if (!pImpl || !pImpl->brain) return;
    auto* predictionSystem = pImpl->brain->getPredictionSystem();
    if (!predictionSystem) return;
    
    // Update prediction system with current sensory input
    // This would typically involve predicting expected sensory input
    // based on current neural activity and comparing to actual input
    
    // For now, create a simple prediction update based on novelty
    float novelty = pImpl->brain->getNovelty() ? 
        pImpl->brain->getNovelty()->getLevel() : 0.0f;
    
    // Update prediction error based on surprise/novelty
    auto* predictionError = pImpl->brain->getPredictionErrorSignal();
    if (predictionError) {
        predictionError->update(novelty);
    }
}

void BrainMain::updateCognitionSystems(double timestep) {
    if (!pImpl || !pImpl->brain) return;
    
    if (auto* planner = pImpl->brain->getPlanner()) {
        planner->update(timestep);
    }
    if (auto* conceptFormation = pImpl->brain->getConceptFormation()) {
        conceptFormation->update(timestep);
    }
    if (auto* attention = pImpl->brain->getAttention()) {
        attention->update(timestep);
    }
}

} // namespace nlm