#include "BrainSimulation.hpp"
#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainSimulation::step(Brain* parent, SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    step1_processDelayedSpikes(parent, currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    step2_updateNeurons(parent, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    step3_detectSpikesAndSchedule(parent, currentStep, currentTime);
    
    // ========== STEP 4: Update working memory ==========
    step4_updateWorkingMemory(parent);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    step5_applyNeuromodulation(parent);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    step6_applyPlasticity(parent, currentTime);
    
    // ========== STEP 7: Update episodic memory ==========
    step7_updateEpisodicMemory(parent, currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    step8_updatePredictionSystem(parent);
    
    // ========== STEP 9: Update attention system ==========
    step9_updateAttention(parent);
    
    // ========== STEP 10: Update concept formation ==========
    step10_updateConceptFormation(parent);
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    step11_applyStructuralPlasticity(parent, currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    step12_replayMemories(parent, currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    step13_applyDevelopment(parent, currentStep, currentTime);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    step14_consolidateMemories(parent, currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    step15_manageCheckpoints(parent, currentStep, currentTime);
    
    // Collect statistics
    collectStatistics(parent);
}

void BrainSimulation::step1_processDelayedSpikes(Brain* parent, SimulationStep step, Timestamp time) {
    if (spikeSystem) {
        spikeSystem->processDelayedSpikes(step, time);
    }
}

void BrainSimulation::step2_updateNeurons(Brain* parent, Timestamp time) {
    if (!parent) return;
    
    // Update all neurons in all regions
    for (const auto& region : parent->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(time, timestep);
            }
        }
    }
}

void BrainSimulation::step3_detectSpikesAndSchedule(Brain* parent, SimulationStep step, Timestamp time) {
    if (!parent) return;
    
    for (const auto& region : parent->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(time) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), time, step);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(time);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = step + delay;
                        Timestamp deliveryTime = time + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            time,
                            deliveryTime,
                            step,
                            deliveryStep
                        );

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(time);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (workingMemory) {
                        workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }

                    totalSpikesThisStep++;
                }
            }
        }
    }
}

void BrainSimulation::step4_updateWorkingMemory(Brain* parent) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void BrainSimulation::step5_applyNeuromodulation(Brain* parent) {
    // Update novelty detection
    if (novelty) {
        novelty->update(timestep);
    }
    
    // Update curiosity
    if (curiosity) {
        curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (dopamine) {
        dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = dopamine->getLevel();
        for (const auto& region : parent->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
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

void BrainSimulation::step6_applyPlasticity(Brain* parent, Timestamp time) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    for (const auto& region : parent->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    stdp->update(&syn, preSpikes, postSpikes, timestep);
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
                    hebbian->update(&syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(time);
        }
    }
}

void BrainSimulation::step7_updateEpisodicMemory(Brain* parent, SimulationStep step, Timestamp time) {
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (const auto& region : parent->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
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
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainSimulation::step8_updatePredictionSystem(Brain* parent) {
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

void BrainSimulation::step9_updateAttention(Brain* parent) {
    if (attention) {
        attention->update(timestep);
        
        // Apply attention to working memory winners
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void BrainSimulation::step10_updateConceptFormation(Brain* parent) {
    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
}

void BrainSimulation::step11_applyStructuralPlasticity(Brain* parent, SimulationStep step) {
    if (structuralPlasticity && step % 100 == 0) {
        structuralPlasticity->update(parent, *parent->getRandomGenerator());
    }
}

void BrainSimulation::step12_replayMemories(Brain* parent, SimulationStep step) {
    if (episodicMemory && step % replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainSimulation::step13_applyDevelopment(Brain* parent, SimulationStep step, Timestamp time) {
    if (step % 1000 == 0) {  // Update development every 1000 steps
        developmentSystem->update(parent, *parent->getRandomGenerator(), timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = parent->getDevelopmentalStage();
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

void BrainSimulation::step14_consolidateMemories(Brain* parent, SimulationStep step) {
    if (episodicMemory && step % consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(0.3f);
    }
}

void BrainSimulation::step15_manageCheckpoints(Brain* parent, SimulationStep step, Timestamp time) {
    if (auto checkpointManager = parent->getCheckpointManager()) {
        checkpointManager->update(step, time);
    }
}

void BrainSimulation::collectStatistics(Brain* parent) {
    totalSpikesTotal += totalSpikesThisStep;
}

void BrainSimulation::reset(Brain* parent) {
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
}

void BrainSimulation::initialize(Brain* parent) {
    // Initialize simulation components
}

} // namespace nlm