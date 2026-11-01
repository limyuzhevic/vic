#include "MemorySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct MemorySystem::Impl {
    Brain* brain_;
    SimulationStep stepsSinceLastEpisode_;
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    Impl() : brain_(nullptr), stepsSinceLastEpisode_(0) {}
};

MemorySystem::MemorySystem() : pImpl(std::make_unique<Impl>()) {}
MemorySystem::~MemorySystem() = default;

void MemorySystem::initialize(Brain* brain, std::shared_ptr<Config> config) {
    pImpl->brain_ = brain;
}

void MemorySystem::updateWorkingMemory(TimestepDuration dt, SimulationStep currentStep) {
    if (pImpl->brain_ && pImpl->brain_->getWorkingMemory()) {
        pImpl->brain_->getWorkingMemory()->update(dt);
    }
}

void MemorySystem::updateEpisodicMemory(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       SimulationStep currentStep, Timestamp currentTime,
                                       const Dopamine* dopamine) {
    pImpl->stepsSinceLastEpisode_++;
    if (pImpl->stepsSinceLastEpisode_ >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode_ = 0;
        
        if (pImpl->brain_ && pImpl->brain_->getEpisodicMemory()) {
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
            
            pImpl->brain_->getEpisodicMemory()->storeEpisode(episode);
        }
    }
}

void MemorySystem::handleMemoryReplay(SimulationStep currentStep, SimulationStep replayInterval,
                                      std::unique_ptr<NeuralEpisodicMemory>& episodicMemory) {
    if (currentStep % replayInterval == 0 && episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void MemorySystem::handleMemoryConsolidation(SimulationStep currentStep,
                                             SimulationStep consolidationInterval,
                                             std::unique_ptr<NeuralEpisodicMemory>& episodicMemory) {
    if (currentStep % consolidationInterval == 0 && episodicMemory) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(0.3f);
    }
}

size_t MemorySystem::getEpisodicMemoryCount() const {
    return pImpl->brain_ && pImpl->brain_->getEpisodicMemory() ? 
        pImpl->brain_->getEpisodicMemory()->getEpisodeCount() : 0;
}

const std::vector<NeuronId>& MemorySystem::getWorkingMemoryNeurons() const {
    pImpl->memoryNeurons_.clear();
    if (pImpl->brain_ && pImpl->brain_->getWorkingMemory()) {
        pImpl->memoryNeurons_ = pImpl->brain_->getWorkingMemory()->getMemoryNeurons();
    }
    return pImpl->memoryNeurons_;
}

void MemorySystem::reset() {
    pImpl->stepsSinceLastEpisode_ = 0;
    if (pImpl->brain_ && pImpl->brain_->getWorkingMemory()) {
        pImpl->brain_->getWorkingMemory()->clear();
    }
}

void MemorySystem::storeToWorkingMemory(NeuronId neuron, float activation, SimulationStep timestamp) {
    if (pImpl->brain_ && pImpl->brain_->getWorkingMemory()) {
        pImpl->brain_->getWorkingMemory()->storeToNeuron(neuron, activation);
    }
}

float MemorySystem::getWorkingMemoryState(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    float totalActivity = 0.0f;
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            }
        }
    }
    return totalActivity / 100.0f;  // Normalize
}

} // namespace nlm
