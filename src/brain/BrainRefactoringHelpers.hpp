/**
 * Refactoring Plan for NLM Brain.cpp Architecture Improvements
 * 
 * This document outlines the comprehensive refactoring plan for the NLM (熙然) brain simulation framework.
 * The goal is to improve code quality, maintainability, and extensibility while preserving all functionality.
 * 
 * Current Issues Identified:
 * 1. Brain.cpp is 1117 lines - monolithic design
 * 2. step() method is 283 lines with 15+ responsibilities
 * 3. Code duplication in triple nested loops throughout the file
 * 4. Missing Doxygen documentation for most methods
 * 5. Basic error handling insufficient for production use
 * 6. Configuration system is basic, limited flexibility
 * 7. CLI interface limited, hard to extend
 * 8. Limited performance monitoring and visualization
 * 9. Documentation focused on usage, not code quality
 */

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <cmath>
#include <sstream>

using namespace nlm;

// Helper function declarations for refactored Brain class
namespace brain_helpers {
    // Helper functions for common operations
    void updateAllNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                         const TimestepDuration& timestep, const Timestamp& currentTime);
    
    void detectAndQueueSpikes(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             std::unique_ptr<SpikeSystem>& spikeSystem,
                             SimulationStep currentStep, Timestamp currentTime,
                             TimestepDuration timestep);
    
    void applyNeuromodulationEffects(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                     std::unique_ptr<Dopamine>& dopamine,
                                     float currentTime);
    
    void applyPlasticityRules(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             std::unique_ptr<STDP>& stdp,
                             std::unique_ptr<Hebbian>& hebbian,
                             std::unique_ptr<Dopamine>& dopamine,
                             TimestepDuration timestep, Timestamp currentTime);
    
    void storeEpisodeInMemory(std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                             std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             SimulationStep currentStep, float dopamineLevel);
    
    void updateIntegratedSystems(std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                 std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                                 std::unique_ptr<PredictionSystem>& predictionSystem,
                                 std::unique_ptr<NeuralPlanner>& planner,
                                 std::unique_ptr<ConceptFormation>& conceptFormation,
                                 std::unique_ptr<AttentionalSelection>& attention,
                                 std::unique_ptr<DevelopmentSystem>& developmentSystem,
                                 std::unique_ptr<Novelty>& novelty,
                                 std::unique_ptr<Curiosity>& curiosity,
                                 TimestepDuration timestep, SimulationStep currentStep,
                                 Timestamp currentTime);
    
    void replayMemories(std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                        SimulationStep currentStep, size_t replayInterval);
    
    void processDevelopmentEffects(std::unique_ptr<DevelopmentSystem>& developmentSystem,
                                   std::unique_ptr<StructuralPlasticity>& structuralPlasticity,
                                   DevelopmentalStage& developmentalStage,
                                   SimulationStep currentStep, TimestepDuration timestep);
    
    void consolidateMemories(std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                             SimulationStep currentStep, size_t consolidationInterval);
    
    void manageCheckpoints(std::unique_ptr<CheckpointManager>& checkpointManager,
                           SimulationStep currentStep, Timestamp currentTime);
    
    void logStepStatistics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                           size_t totalSpikesThisStep, size_t totalSpikesTotal,
                           float averageFiringRate, float eiratio);
    
    bool validateSystemState(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             const std::unique_ptr<SpikeSystem>& spikeSystem);
    
    Neuron* findNeuronById(std::vector<std::unique_ptr<NeuralRegion>>& regions, NeuronId neuronId);
    
    std::vector<Neuron*> collectSensoryNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    std::vector<Neuron*> collectMotorNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    float calculateAverageSynapticWeight(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    size_t countActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
}

// Implementation of helper functions
namespace brain_helpers {

void updateAllNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                     const TimestepDuration& timestep, const Timestamp& currentTime) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void detectAndQueueSpikes(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                         std::unique_ptr<SpikeSystem>& spikeSystem,
                         SimulationStep currentStep, Timestamp currentTime,
                         TimestepDuration timestep) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (region->getWorkingMemory()) {
                        region->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void applyNeuromodulationEffects(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                 std::unique_ptr<Dopamine>& dopamine,
                                 float currentTime) {
    if (!dopamine) return;
    
    // Update novelty detection
    if (dopamine->getNovelty()) {
        dopamine->getNovelty()->update(currentTime);
    }
    
    // Update curiosity
    if (dopamine->getCuriosity()) {
        dopamine->getCuriosity()->update(currentTime);
    }
    
    // Update dopamine (reward prediction error)
    if (dopamine) {
        dopamine->update(currentTime);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void applyPlasticityRules(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                         std::unique_ptr<STDP>& stdp,
                         std::unique_ptr<Hebbian>& hebbian,
                         std::unique_ptr<Dopamine>& dopamine,
                         TimestepDuration timestep, Timestamp currentTime) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void storeEpisodeInMemory(std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                         std::vector<std::unique_ptr<NeuralRegion>>& regions,
                         SimulationStep currentStep, float dopamineLevel) {
    if (!episodicMemory) return;
    
    // Capture current brain state as an episode
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = dopamineLevel;
    
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
    
    // Store reward in episode
    episode.reward = dopamineLevel;
    
    episodicMemory->storeEpisode(episode);
}

void updateIntegratedSystems(std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                             std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                             std::unique_ptr<PredictionSystem>& predictionSystem,
                             std::unique_ptr<NeuralPlanner>& planner,
                             std::unique_ptr<ConceptFormation>& conceptFormation,
                             std::unique_ptr<AttentionalSelection>& attention,
                             std::unique_ptr<DevelopmentSystem>& developmentSystem,
                             std::unique_ptr<Novelty>& novelty,
                             std::unique_ptr<Curiosity>& curiosity,
                             TimestepDuration timestep, SimulationStep currentStep,
                             Timestamp currentTime) {
    // Update working memory
    if (workingMemory) {
        workingMemory->update(timestep);
    }
    
    // Update novelty detection
    if (novelty) {
        novelty->update(timestep);
    }
    
    // Update curiosity
    if (curiosity) {
        curiosity->update(timestep);
    }
    
    // Update attention system
    if (attention) {
        attention->update(timestep);
    }
    
    // Note: Development system updates are handled separately to control frequency
}

void replayMemories(std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                    SimulationStep currentStep, size_t replayInterval) {
    if (!episodicMemory) return;
    
    // Get episodes for replay
    auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        episodicMemory->replayEpisode(episode);
    }
}

void processDevelopmentEffects(std::unique_ptr<DevelopmentSystem>& developmentSystem,
                               std::unique_ptr<StructuralPlasticity>& structuralPlasticity,
                               DevelopmentalStage& developmentalStage,
                               SimulationStep currentStep, TimestepDuration timestep) {
    if (!developmentSystem || !structuralPlasticity) return;
    
    // Update development system
    developmentSystem->update(nullptr, nullptr, timestep * 1000);
    
    // Development affects plasticity rates
    auto* sp = structuralPlasticity.get();
    if (sp) {
        float plasticityMod = 1.0f;
        
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void consolidateMemories(std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                         SimulationStep currentStep, size_t consolidationInterval) {
    if (!episodicMemory) return;
    
    // Consolidate important memories, remove weak ones
    episodicMemory->consolidate(0.3f);
}

void manageCheckpoints(std::unique_ptr<CheckpointManager>& checkpointManager,
                       SimulationStep currentStep, Timestamp currentTime) {
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

void logStepStatistics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                       size_t totalSpikesThisStep, size_t totalSpikesTotal,
                       float averageFiringRate, float eiratio) {
    // Simple statistics logging - can be enhanced with more detailed metrics
    if (totalSpikesThisStep > 0) {
        std::cout << "  Step stats: " << totalSpikesThisStep << " spikes this step, " 
                  << totalSpikesTotal << " total, " << averageFiringRate << " avg firing rate" << std::endl;
    }
}

bool validateSystemState(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                         const std::unique_ptr<SpikeSystem>& spikeSystem) {
    bool isValid = true;
    
    // Check for NaN or infinite values in neuron states
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
                    std::cerr << "ERROR: Invalid neuron membrane potential: " << state.membranePotential << std::endl;
                    isValid = false;
                }
            }
        }
    }
    
    // Check spike system state
    if (spikeSystem) {
        if (spikeSystem->getPendingSpikeCount() > 1000) {
            std::cerr << "WARNING: High number of pending spike events: " << spikeSystem->getPendingSpikeCount() << std::endl;
        }
    }
    
    return isValid;
}

Neuron* findNeuronById(std::vector<std::unique_ptr<NeuralRegion>>& regions, NeuronId neuronId) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                return neuron;
            }
        }
    }
    return nullptr;
}

std::vector<Neuron*> collectSensoryNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    std::vector<Neuron*> sensoryNeurons;
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getType() == NeuronType::Sensory) {
                sensoryNeurons.push_back(neuron);
            }
        }
    }
    return sensoryNeurons;
}

std::vector<Neuron*> collectMotorNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    std::vector<Neuron*> motorNeurons;
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getType() == NeuronType::Motor) {
                motorNeurons.push_back(neuron);
            }
        }
    }
    return motorNeurons;
}

float calculateAverageSynapticWeight(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    float totalWeight = 0.0f;
    size_t totalSynapses = 0;
    
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            totalWeight += std::abs(syn->getWeight());
            totalSynapses++;
        }
    }
    
    return totalSynapses > 0 ? totalWeight / totalSynapses : 0.0f;
}

size_t countActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    size_t activeCount = 0;
    for (const auto& region : regions) {
        activeCount += region->getActiveNeuronCount();
    }
    return activeCount;
}

}