// Brain Memory System implementation
#include "BrainMemorySystem.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

BrainMemorySystem::BrainMemorySystem(Brain* brain) : brain(brain), activeMemoryTraces(0), totalEpisodes(0) {
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
}

void BrainMemorySystem::initializeMemorySystems() {
    if (workingMemory) {
        workingMemory->initialize(this);
        workingMemory->setCapacity(100);
    }
    
    if (episodicMemory) {
        episodicMemory->initialize(this);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    if (associativeMemory) {
        associativeMemory->initialize(this);
    }
    
    NLM_LOG_INFO("Memory systems initialized");
}

void BrainMemorySystem::updateWorkingMemory(TimestepDuration timestep) {
    if (workingMemory) {
        workingMemory->update(timestep);
        activeMemoryTraces = workingMemory->getActiveTraces();
    }
}

void BrainMemorySystem::updateEpisodicMemory(SimulationStep currentStep, float reward) {
    if (shouldStoreEpisode(currentStep)) {
        storeCurrentStateAsEpisode(currentStep, reward);
        totalEpisodes = episodicMemory ? episodicMemory->getEpisodeCount() : 0;
    }
}

void BrainMemorySystem::storeCurrentStateAsEpisode(SimulationStep currentStep, float reward) {
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = reward;
    
    // Capture current brain state
    captureCurrentBrainState(episode);
    
    if (episodicMemory) {
        episodicMemory->storeEpisode(episode);
        totalEpisodes = episodicMemory->getEpisodeCount();
    }
}

void BrainMemorySystem::captureCurrentBrainState(EpisodicMemoryItem& episode) {
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
}

bool BrainMemorySystem::shouldStoreEpisode(SimulationStep currentStep) {
    return currentStep % 10 == 0;  // Store episode every 10 steps
}

std::vector<const EpisodicMemoryItem*> BrainMemorySystem::getEpisodesForReplay(size_t count) {
    if (!episodicMemory) return {};
    return episodicMemory->getEpisodesForReplay(count);
}

void BrainMemorySystem::replayEpisodes(const std::vector<const EpisodicMemoryItem*>& episodes) {
    for (const auto* episode : episodes) {
        if (episodicMemory) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainMemorySystem::consolidateMemories(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
        totalEpisodes = episodicMemory->getEpisodeCount();
    }
}

} // namespace nlm