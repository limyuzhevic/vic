// Brain Integration Functions Implementation
// Modular implementation of the brain step loop components

#include "BrainHelperFunctions.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {
namespace BrainHelpers {

// ========== STEP 1: Process pending delayed spikes ==========
void processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    brain.getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
}

// ========== STEP 2: Update all neurons (LIF dynamics) ==========
void updateNeurons(Brain& brain, Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

// ========== STEP 3: Detect spikes and schedule spike events ==========
void detectSpikesAndScheduleEvents(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brain.getSpikeSystem()->queueSpike(event);

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

                        brain.getSpikeSystem()->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brain.getWorkingMemory()) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

// ========== STEP 4: Update working memory ==========
void updateWorkingMemory(Brain& brain, TimestepDuration timestep) {
    if (brain.getWorkingMemory()) {
        brain.getWorkingMemory()->update(timestep);
    }
}

// ========== STEP 5: Apply neuromodulation effects ==========
void applyNeuromodulation(Brain& brain, TimestepDuration timestep) {
    // Update novelty detection
    if (brain.getNovelty()) {
        brain.getNovelty()->update(timestep);
    }
    
    // Update curiosity
    if (brain.getCuriosity()) {
        brain.getCuriosity()->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (brain.getDopamine()) {
        brain.getDopamine()->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brain.getDopamine()->getLevel();
        applyDopamineEffects(brain, dopamineLevel);
    }
}

// ========== STEP 6: Apply plasticity rules ==========
void applyPlasticity(Brain& brain, TimestepDuration timestep, float plasticityMod) {
    // Calculate neuromodulation factor for plasticity
    float actualPlasticityMod = plasticityMod;
    if (brain.getDopamine()) {
        actualPlasticityMod = brain.getDopamine()->getPlasticityFactor();
    }
    
    updateSynapticPlasticity(brain, 0, actualPlasticityMod);
}

// ========== STEP 7: Update episodic memory ==========
void updateEpisodicMemory(Brain& brain, SimulationStep currentStep) {
    brain.getEpisodicMemory()->step(currentStep);
    
    // Store episode periodically
    static SimulationStep stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        captureBrainStateForMemory(brain, currentStep, static_cast<Timestamp>(currentStep) * brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
}

// ========== STEP 8: Update prediction system ==========
void updatePredictionSystem(Brain& brain) {
    if (brain.getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

// ========== STEP 9: Update attention system ==========
void updateAttentionSystem(Brain& brain) {
    if (brain.getAttention()) {
        brain.getAttention()->update(brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply attention to working memory winners
        applyAttentionToWorkingMemory(brain);
    }
}

// ========== STEP 10: Update concept formation ==========
void updateConceptFormation(Brain& brain) {
    if (brain.getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

// ========== STEP 11: Apply structural plasticity ==========
void applyStructuralPlasticity(Brain& brain, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        brain.getStructuralPlasticity()->update(&brain, *brain.getRandomGenerator());
    }
}

// ========== STEP 12: Replay important memories ==========
void replayImportantMemories(Brain& brain, SimulationStep currentStep) {
    if (currentStep % brain.getConfig()->getOr<size_t>("replay_interval", 100) == 0 && brain.getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = brain.getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain.getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

// ========== STEP 13: Apply development effects ==========
void applyDevelopmentEffects(Brain& brain, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        brain.getDevelopmentSystem()->update(&brain, *brain.getRandomGenerator(), brain.getConfig()->getOr<double>("simulation_timestep", 0.001) * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain.getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brain.getDevelopmentalStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
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
}

// ========== STEP 14: Memory consolidation ==========
void memoryConsolidation(Brain& brain, SimulationStep currentStep) {
    if (currentStep % brain.getConfig()->getOr<size_t>("consolidation_interval", 1000) == 0 && brain.getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        brain.getEpisodicMemory()->consolidate(0.3f);
    }
}

// ========== STEP 15: Checkpoint management ==========
void checkpointManagement(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    if (brain.getCheckpointManager()) {
        brain.getCheckpointManager()->update(currentStep, currentTime);
    }
}

// ========== ADDITIONAL HELPERS ==========

// Process immediate spikes from spike system
void processImmediateSpikes(Brain& brain, SimulationStep currentStep) {
    brain.getSpikeSystem()->processSpikes(currentStep);
}

// Collect and record statistics
void collectStatistics(Brain& brain) {
    // Statistics collection would go here
}

// Apply dopamine effects on neural excitability
void applyDopamineEffects(Brain& brain, float dopamineLevel) {
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : brain.getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

// Update synaptic plasticity with neuromodulation
void updateSynapticPlasticity(Brain& brain, SimulationStep currentStep, float plasticityMod) {
    for (auto& region : brain.getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brain.getSTDP()->update(&syn, preSpikes, postSpikes, brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
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
                    brain.getHebbian()->update(&syn, preSpikes, postSpikes, brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
                }
            }
            
            // Update synapse state
            syn->step(static_cast<Timestamp>(currentStep) * brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
        }
    }
}

// Capture current brain state for episodic memory
void captureBrainStateForMemory(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f;
    
    // Store active neurons
    for (auto& region : brain.getRegions()) {
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
    
    brain.getEpisodicMemory()->storeEpisode(episode);
}

// Apply attention to working memory winners
void applyAttentionToWorkingMemory(Brain& brain) {
    if (brain.getWorkingMemory() && !brain.getWorkingMemory()->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = brain.getWorkingMemory()->getMemoryNeurons();
        brain.getAttention()->processCompetition(competitors);
    }
}

// Get spike events from neurons and queue them
void queueSpikeEventsFromNeurons(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain.getConfig()->getOr<double>("simulation_timestep", 0.001) * 2.0f);

                if (justFired) {
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brain.getSpikeSystem()->queueSpike(event);
                }
            }
        }
    }
}

// Deliver synaptic input from delayed spike events
void deliverSynapticInputFromDelayedEvents(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain.getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            // This would be handled by the spike system's delayed event processing
        }
    }
}

// Update development system and adjust plasticity rates
void updateDevelopmentAndPlasticity(Brain& brain, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {
        brain.getDevelopmentSystem()->update(&brain, *brain.getRandomGenerator(), brain.getConfig()->getOr<double>("simulation_timestep", 0.001) * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain.getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brain.getDevelopmentalStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

// Get episodes for replay and trigger memory consolidation
void manageMemoryReplayAndConsolidation(Brain& brain, SimulationStep currentStep) {
    // Replay important memories
    replayImportantMemories(brain, currentStep);
    
    // Memory consolidation
    memoryConsolidation(brain, currentStep);
}

} // namespace BrainHelpers

} // namespace nlm