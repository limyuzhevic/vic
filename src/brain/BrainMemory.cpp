#include "BrainMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

BrainMemory::BrainMemory(std::shared_ptr<Config> cfg)
    : config(cfg)
    , workingMemory(nullptr)
    , episodicMemory(nullptr)
    , associativeMemory(nullptr)
    , workingMemorySize(0)
    , episodicMemorySize(0)
    , episodeCount(0)
    , stepsSinceLastEpisode(0)
    , lastReplayStep(0)
{
    // Initialize from config
    initializeFromConfig();
}

BrainMemory::~BrainMemory() = default;

void BrainMemory::initialize(Brain* brain) {
    if (!brain) return;
    
    // Initialize memory systems
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Configure from config
    initializeFromConfig();
}

void BrainMemory::updateWorkingMemory(TimestepDuration timestep) {
    if (!workingMemory) return;
    
    // Collect active neurons for working memory
    collectActiveNeuronsForWorkingMemory(nullptr);
    
    // Update working memory
    workingMemory->update(timestep);
    
    // Update competition
    updateMemoryCompetition(nullptr);
}

void BrainMemory::storeSensoryInput(NeuronId neuronId, float activation) {
    if (workingMemory) {
        workingMemory->storeToNeuron(neuronId, activation);
    }
}

void BrainMemory::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
    stepsSinceLastEpisode++;
    
    // Store episode periodically
    if (stepsSinceLastEpisode >= 10) {
        stepsSinceLastEpisode = 0;
        captureCurrentStateAsEpisode(currentStep, currentTime, brain);
    }
}

void BrainMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episodicMemory) {
        episodicMemory->storeEpisode(episode);
        episodeCount++;
    }
}

void BrainMemory::updateAssociativeMemory() {
    if (associativeMemory) {
        associativeMemory->update();
    }
}

std::vector<const EpisodicMemoryItem*> BrainMemory::getEpisodesForReplay(size_t count) {
    std::vector<const EpisodicMemoryItem*> episodes;
    if (episodicMemory) {
        episodes = episodicMemory->getEpisodesForReplay(count);
    }
    return episodes;
}

void BrainMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (episodicMemory && episode) {
        episodicMemory->replayEpisode(episode);
    }
}

void BrainMemory::consolidateMemories(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

void BrainMemory::collectActiveNeuronsForWorkingMemory(Brain* brain) {
    if (!brain || !workingMemory) return;
    
    // Collect active neurons from brain regions
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    workingMemory->storeToNeuron(neuron->getId(), 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
        }
    }
}

void BrainMemory::captureCurrentStateAsEpisode(SimulationStep currentStep, Timestamp currentTime, Brain* brain) {
    if (!brain || !episodicMemory) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    
    // Get reward from dopamine system
    if (brain->getDopamine()) {
        episode.reward = brain->getDopamine()->getLevel();
    }
    
    // Store active neurons
    for (const auto& region : brain->getRegions()) {
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
    
    storeEpisode(episode);
}

void BrainMemory::updateMemoryCompetition(Brain* brain) {
    if (!brain || !workingMemory) return;
    
    // Apply attention to working memory
    if (attention) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        attention->processCompetition(competitors);
    }
}

void BrainMemory::reset() {
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    
    workingMemorySize = 0;
    episodicMemorySize = 0;
    episodeCount = 0;
    stepsSinceLastEpisode = 0;
    lastReplayStep = 0;
}

void BrainMemory::logStatus() const {
    // TODO: Add logging implementation
}

void BrainMemory::initializeFromConfig() {
    if (!config) return;
    
    // Get memory system parameters from config
    workingMemorySize = config->getOr<size_t>("working_memory_capacity", 100);
    episodicMemorySize = config->getOr<size_t>("episodic_memory_capacity", 1000);
    
    // Configure memory systems
    if (workingMemory) {
        workingMemory->setCapacity(workingMemorySize);
    }
    if (episodicMemory) {
        episodicMemory->setMaxEpisodes(episodicMemorySize);
    }
}

} // namespace nlm