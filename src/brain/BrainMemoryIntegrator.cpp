#include "BrainMemoryIntegrator.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainMemoryIntegrator::BrainMemoryIntegrator(std::shared_ptr<Config> config)
    : workingMemoryCapacity(0)
    , episodicMemoryMaxEpisodes(0)
    , associativeMemoryCapacity(0)
{
}

void BrainMemoryIntegrator::initializeWorkingMemory(Brain* brain, size_t neuronCount) {
    workingMemoryCapacity = neuronCount / 10;
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    workingMemory->initialize(brain);
    workingMemory->setCapacity(workingMemoryCapacity);
}

void BrainMemoryIntegrator::initializeEpisodicMemory(Brain* brain) {
    episodicMemoryMaxEpisodes = 1000;
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory->initialize(brain);
    episodicMemory->setMaxEpisodes(episodicMemoryMaxEpisodes);
}

void BrainMemoryIntegrator::initializeAssociativeMemory(Brain* brain) {
    associativeMemoryCapacity = 1000;
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory->initialize(brain);
}

void BrainMemoryIntegrator::initializePredictionSystem(Brain* brain) {
    predictionSystem = std::make_unique<PredictionSystem>();
}

void BrainMemoryIntegrator::updateMemoryFromActivity(Brain* brain, SimulationStep step, Timestamp time) {
    updateWorkingMemoryFromNeuralActivity(brain);
    captureCurrentStateForEpisodicMemory(brain, step, time);
}

void BrainMemoryIntegrator::storeToWorkingMemory(Brain* brain, NeuronId neuronId, float activation) {
    if (workingMemory) {
        workingMemory->storeToNeuron(neuronId, activation);
    }
}

void BrainMemoryIntegrator::storeActiveNeuronsToEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time) {
    captureCurrentStateForEpisodicMemory(brain, step, time);
}

void BrainMemoryIntegrator::replayMemories(Brain* brain, SimulationStep step, size_t maxEpisodes) {
    if (!episodicMemory) return;
    
    size_t replayInterval = 100;  // Should come from config
    if (step % replayInterval == 0) {
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(maxEpisodes);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainMemoryIntegrator::consolidateMemories(Brain* brain, float consolidationFactor) {
    if (episodicMemory) {
        episodicMemory->consolidate(consolidationFactor);
    }
}

void BrainMemoryIntegrator::configureWorkingMemoryCapacity(Brain* brain, size_t capacity) {
    workingMemoryCapacity = capacity;
    if (workingMemory) {
        workingMemory->setCapacity(capacity);
    }
}

void BrainMemoryIntegrator::configureEpisodicMemoryMaxEpisodes(Brain* brain, size_t maxEpisodes) {
    episodicMemoryMaxEpisodes = maxEpisodes;
    if (episodicMemory) {
        episodicMemory->setMaxEpisodes(maxEpisodes);
    }
}

void BrainMemoryIntegrator::configureAssociativeMemoryCapacity(Brain* brain, size_t capacity) {
    associativeMemoryCapacity = capacity;
    if (associativeMemory) {
        // Add capacity configuration method if available
    }
}

void BrainMemoryIntegrator::resetMemorySystems(Brain* brain) {
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
}

void BrainMemoryIntegrator::clearWorkingMemory(Brain* brain) {
    if (workingMemory) workingMemory->clear();
}

void BrainMemoryIntegrator::clearEpisodicMemory(Brain* brain) {
    if (episodicMemory) episodicMemory->clear();
}

void BrainMemoryIntegrator::clearAssociativeMemory(Brain* brain) {
    if (associativeMemory) associativeMemory->clear();
}

void BrainMemoryIntegrator::setupMemoryConnections(Brain* brain) {
    // Setup connections between memory systems
    if (workingMemory && episodicMemory) {
        // Add memory system interconnections
    }
}

void BrainMemoryIntegrator::updateWorkingMemoryFromNeuralActivity(Brain* brain) {
    if (workingMemory) {
        workingMemory->update(0.001);  // Should use actual timestep
    }
}

void BrainMemoryIntegrator::captureCurrentStateForEpisodicMemory(Brain* brain, SimulationStep step, Timestamp time) {
    if (!episodicMemory) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = step;
    episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
    
    // Store active neurons (simplified)
    auto& regions = brain->pImpl->brainInitializer->getRegions();
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
    
    episodicMemory->storeEpisode(episode);
}

void BrainMemoryIntegrator::processReplayBatch(Brain* brain, size_t batchSize) {
    if (!episodicMemory) return;
    
    auto episodes = episodicMemory->getEpisodesForReplay(batchSize);
    for (const auto* episode : episodes) {
        episodicMemory->replayEpisode(episode);
    }
}

void BrainMemoryIntegrator::consolidateWeakMemories(Brain* brain, float consolidationFactor) {
    if (episodicMemory) {
        episodicMemory->consolidate(consolidationFactor);
    }
}

void BrainMemoryIntegrator::integrateNeuromodulation(Brain* brain) {
    // Integrate neuromodulation signals with memory systems
    if (brain->getDopamine()) {
        // Apply dopamine effects to memory systems
    }
}

float BrainMemoryIntegrator::getModulatedLearningRate(Brain* brain, float baseRate) {
    if (brain->getDopamine()) {
        return baseRate * brain->getDopamine()->getPlasticityFactor();
    }
    return baseRate;
}