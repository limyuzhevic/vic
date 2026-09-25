#include "BrainStepProcessor.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

void BrainStepProcessor::processStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
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
    
    if (!brain) return;
    
    auto* pImpl = brain->pImpl.get();
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(brain, currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(brain, currentTime, pImpl->timestep);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndQueueSpikes(brain, currentStep, currentTime);
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(brain, pImpl->timestep);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulation(brain, pImpl->timestep);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    applyPlasticity(brain, currentTime, pImpl->timestep);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(brain, currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(brain);
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(brain, pImpl->timestep);
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(brain);
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    applyStructuralPlasticity(brain, currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(brain, currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(brain, currentStep, pImpl->timestep);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(brain, currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    updateCheckpoints(brain, currentStep, currentTime);
    
    // ========== STEP 16: Collect statistics ==========
    collectStatistics(brain);
}

void BrainStepProcessor::processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainStepProcessor::updateNeurons(Brain* brain, Timestamp currentTime, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainStepProcessor::detectAndQueueSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto* pImpl = brain->pImpl.get();
    
    for (auto& region : pImpl->regions) {
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
                    pImpl->spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

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

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainStepProcessor::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->update(timestep);
    }
}

void BrainStepProcessor::applyNeuromodulation(Brain* brain, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
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

void BrainStepProcessor::applyPlasticity(Brain* brain, Timestamp currentTime, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainStepProcessor::updateEpisodicMemory(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto* pImpl = brain->pImpl.get();
    
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
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
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainStepProcessor::updatePredictionSystem(Brain* brain) {
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

void BrainStepProcessor::updateAttentionSystem(Brain* brain, TimestepDuration timestep) {
    if (auto* attention = brain->getAttention()) {
        attention->update(timestep);
        
        // Apply attention to working memory winners
        if (auto* workingMemory = brain->getWorkingMemory() && 
            !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void BrainStepProcessor::updateConceptFormation(Brain* brain) {
    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
}

void BrainStepProcessor::applyStructuralPlasticity(Brain* brain, SimulationStep currentStep) {
    auto* pImpl = brain->pImpl.get();
    
    if (currentStep % 100 == 0) {
        if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
            structuralPlasticity->update(brain, *pImpl->rng);
        }
    }
}

void BrainStepProcessor::replayMemories(Brain* brain, SimulationStep currentStep) {
    auto* pImpl = brain->pImpl.get();
    
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainStepProcessor::applyDevelopmentEffects(Brain* brain, SimulationStep currentStep, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (auto* developmentSystem = brain->getDevelopmentSystem()) {
            developmentSystem->update(brain, *pImpl->rng, timestep * 1000);
            
            // Development affects plasticity rates
            auto* sp = pImpl->structuralPlasticity;
            if (sp) {
                DevelopmentalStage stage = pImpl->developmentalStage;
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
}

void BrainStepProcessor::consolidateMemories(Brain* brain, SimulationStep currentStep) {
    auto* pImpl = brain->pImpl.get();
    
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void BrainStepProcessor::updateCheckpoints(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto* pImpl = brain->pImpl.get();
    
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainStepProcessor::collectStatistics(Brain* brain) {
    // Statistics are collected in logStatus() method
}

void BrainStepProcessor::processStep(Brain* brain, SimulationStep currentStep) {
    processStep(brain, currentStep, static_cast<Timestamp>(currentStep) * brain->pImpl->timestep);
}

} // namespace nlm
