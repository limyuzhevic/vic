#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Brain step simulation methods

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    NLM_LOG_DEBUG("Starting brain step " + std::to_string(currentStep) + " at time " + 
                 std::to_string(currentTime) + "s");
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    step1ProcessDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    step2UpdateNeuronDynamics(currentStep, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    step3DetectSpikesAndScheduleEvents(currentStep, currentTime);
    
    // ========== STEP 4: Update working memory ==========
    step4UpdateWorkingMemory(currentStep, currentTime);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    step5ApplyNeuromodulationEffects(currentStep, currentTime);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    step6ApplyPlasticityRules(currentStep, currentTime);
    
    // ========== STEP 7: Update episodic memory ==========
    step7UpdateEpisodicMemory(currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    step8UpdatePredictionSystem(currentStep, currentTime);
    
    // ========== STEP 9: Update attention system ==========
    step9UpdateAttentionSystem(currentStep, currentTime);
    
    // ========== STEP 10: Update concept formation ==========
    step10UpdateConceptFormation(currentStep, currentTime);
    
    // ========== STEP 11: Apply structural plasticity ==========
    step11ApplyStructuralPlasticity(currentStep, currentTime);
    
    // ========== STEP 12: Replay important memories ==========
    step12ReplayMemories(currentStep, currentTime);
    
    // ========== STEP 13: Apply development effects ==========
    step13ApplyDevelopmentEffects(currentStep, currentTime);
    
    // ========== STEP 14: Collect statistics ==========
    step14CollectStatistics(currentStep, currentTime);
    
    NLM_LOG_DEBUG("Completed brain step " + std::to_string(currentStep));
}

// Individual step methods

void Brain::step1ProcessDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::step2UpdateNeuronDynamics(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::step3DetectSpikesAndScheduleEvents(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

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
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
}

void Brain::step4UpdateWorkingMemory(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::step5ApplyNeuromodulationEffects(SimulationStep currentStep, Timestamp currentTime) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Brain::step6ApplyPlasticityRules(SimulationStep currentStep, Timestamp currentTime) {
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
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void Brain::step7UpdateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
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
            
            // Store episode in episodic memory
            pImpl->episodicMemory->storeEpisode(episode);
            
            // Trigger memory consolidation if needed
            if (pImpl->stepsSinceLastEpisode >= pImpl->consolidationInterval) {
                pImpl->episodicMemory->consolidate(pImpl->stepsSinceLastEpisode);
            }
        }
    }
}

void Brain::step8UpdatePredictionSystem(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(pImpl->timestep);
        
        // Update prediction error signal
        if (pImpl->predictionError) {
            pImpl->predictionError->update(pImpl->timestep);
        }
    }
}

void Brain::step9UpdateAttentionSystem(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Update attentional selection based on predicted errors
        if (pImpl->predictionError) {
            pImpl->attention->updatePredictionInfluence(pImpl->predictionError->getLevel());
        }
    }
}

void Brain::step10UpdateConceptFormation(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(pImpl->timestep);
    }
}

void Brain::step11ApplyStructuralPlasticity(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(pImpl->timestep);
        
        // Apply synaptogenesis and pruning
        pImpl->structuralPlasticity->applySynaptogenesis(pImpl->timestep);
        pImpl->structuralPlasticity->applyPruning(pImpl->timestep);
    }
}

void Brain::step12ReplayMemories(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->stepsSinceLastEpisode >= pImpl->replayInterval && 
        pImpl->episodicMemory && !pImpl->isResting) {
        
        // Replay stored episodes
        pImpl->episodicMemory->replay(pImpl->timestep);
        
        // Update working memory with replayed memories
        if (pImpl->workingMemory) {
            pImpl->workingMemory->replay(pImpl->timestep);
        }
    }
}

void Brain::step13ApplyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(pImpl->timestep);
        
        // Apply developmental stage effects
        pImpl->developmentSystem->applyStageEffects(pImpl->developmentalStage, pImpl->timestep);
    }
}

void Brain::step14CollectStatistics(SimulationStep currentStep, Timestamp currentTime) {
    // Update statistics
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t firingNeurons = 0;
    
    for (auto& region : pImpl->regions) {
        totalNeurons += region->getTotalNeuronCount();
        totalSynapses += region->getTotalSynapseCount();
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    firingNeurons++;
                }
            }
        }
    }
    
    // Update global statistics (for backward compatibility)
    pImpl->totalSpikesThisStep = pImpl->totalSpikesTotal;
}

} // namespace nlm