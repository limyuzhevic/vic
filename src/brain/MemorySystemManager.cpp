#include "MemorySystemManager.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MemorySystemManager::MemorySystemManager()
    : workingMemory(nullptr), episodicMemory(nullptr), associativeMemory(nullptr)
    , predictionSystem(nullptr), planner(nullptr), conceptFormation(nullptr), attention(nullptr)
    , lastEpisodeStep(0) {}

MemorySystemManager::~MemorySystemManager() {}

void MemorySystemManager::updateWorkingMemory(TimestepDuration dt) {
    if (workingMemory) {
        workingMemory->update(dt);
    }
}

void MemorySystemManager::storeToWorkingMemory(NeuronId neuronId, float activation) {
    if (workingMemory) {
        workingMemory->storeToNeuron(neuronId, activation);
    }
}

void MemorySystemManager::updateWorkingMemoryFromNeurons(const std::vector<Neuron*>& neurons) {
    if (workingMemory) {
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                float activation = (neuron->getState().membranePotential - 
                                   neuron->getState().restingPotential) / 10.0f;
                workingMemory->storeToNeuron(neuron->getId(), activation);
            }
        }
    }
}

void MemorySystemManager::storeEpisodicMemory(SimulationStep step, Timestamp time,
                                             const std::vector<NeuronId>& activeNeurons,
                                             const std::vector<float>& activations,
                                             float reward) {
    if (episodicMemory) {
        EpisodicMemoryItem episode;
        episode.timestamp = step;
        episode.reward = reward;
        episode.activeNeurons = activeNeurons;
        episode.neuronActivations = activations;
        
        episodicMemory->storeEpisode(episode);
    }
}

void MemorySystemManager::replayEpisodicMemory(size_t count) {
    if (episodicMemory) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(count);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void MemorySystemManager::consolidateEpisodicMemory(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

void MemorySystemManager::consolidateMemories(float threshold) {
    if (episodicMemory) {
        episodicMemory->consolidate(threshold);
    }
}

void MemorySystemManager::replayImportantMemories(size_t count, SimulationStep currentStep) {
    if (episodicMemory && (currentStep % 100 == 0)) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(count);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void MemorySystemManager::processMemoryCompetition() {
    if (workingMemory) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        if (!competitors.empty() && attention) {
            attention->processCompetition(competitors);
        }
    }
}

NeuronId MemorySystemManager::selectMemoryWinner() {
    if (workingMemory) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        if (attention && !competitors.empty()) {
            return attention->selectWinner(competitors);
        }
    }
    return INVALID_NEURON_ID;
}

} // namespace nlm
