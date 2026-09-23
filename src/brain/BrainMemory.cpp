#include "BrainMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// ===========================================
// WORKING MEMORY SYSTEM
// ===========================================

void BrainMemory::initializeWorkingMemory(Brain* brain, NeuralWorkingMemory* workingMemory,
                                         size_t capacity) {
    if (!workingMemory || !brain) return;

    workingMemory->initialize(brain);
    workingMemory->setCapacity(capacity);
}

void BrainMemory::updateWorkingMemory(NeuralWorkingMemory* workingMemory,
                                      TimestepDuration dt) {
    if (!workingMemory) return;

    workingMemory->update(dt);
}

void BrainMemory::storeActiveNeuronsToWorkingMemory(NeuralWorkingMemory* workingMemory,
                                                   const std::vector<Neuron*>& neurons) {
    if (!workingMemory) return;

    for (auto* neuron : neurons) {
        const auto& state = neuron->getState();
        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
        workingMemory->storeToNeuron(neuron->getId(), activation);
    }
}

// ===========================================
// EPISODIC MEMORY SYSTEM
// ===========================================

void BrainMemory::updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory,
                                       size_t currentStep, float reward) {
    if (!episodicMemory) return;

    // Store episode every 10 steps
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;

    if (stepsSinceLastEpisode >= 10) {
        stepsSinceLastEpisode = 0;

        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = reward;

        // Capture current brain state
        // This would be implemented to store neural activity patterns
        episodicMemory->storeEpisode(episode);
    }
}

void BrainMemory::replayMemories(NeuralEpisodicMemory* episodicMemory, size_t replayCount) {
    if (!episodicMemory) return;

    auto episodesToReplay = episodicMemory->getEpisodesForReplay(replayCount);
    for (const auto* episode : episodesToReplay) {
        episodicMemory->replayEpisode(episode);
    }
}

void BrainMemory::consolidateMemories(NeuralEpisodicMemory* episodicMemory, float consolidationRate) {
    if (!episodicMemory) return;

    episodicMemory->consolidate(consolidationRate);
}

// ===========================================
// ASSOCIATIVE MEMORY SYSTEM
// ===========================================

void BrainMemory::initializeAssociativeMemory(NeuralAssociativeMemory* associativeMemory,
                                             Brain* brain) {
    if (!associativeMemory || !brain) return;

    associativeMemory->initialize(brain);
}

void BrainMemory::storeAssociations(NeuralAssociativeMemory* associativeMemory,
                                    const std::vector<Neuron*>& neurons,
                                    float strength) {
    if (!associativeMemory) return;

    // Store patterns and associations
    for (auto* neuron : neurons) {
        associativeMemory->associatePattern(neuron->getId(), strength);
    }
}

// ===========================================
// MEMORY STATISTICS AND ACCESSORS
// ===========================================

void BrainMemory::logMemoryStatus(NeuralWorkingMemory* workingMemory,
                                  NeuralEpisodicMemory* episodicMemory,
                                  NeuralAssociativeMemory* associativeMemory) {
    if (workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    if (associativeMemory) {
        NLM_LOG_INFO("Associative memory patterns: " + std::to_string(associativeMemory->getPatternCount()));
    }
}

} // namespace nlm
