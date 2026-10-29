#include "BrainHeaders.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainStep::executeStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;
    
    brain->pImpl->currentStep = currentStep;
    brain->pImpl->currentTime = currentTime;
    brain->pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(brain, currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(brain, currentStep, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndQueueSpikes(brain, currentStep, currentTime);
    
    // ========== STEP 4: Process immediate spikes ==========
    processImmediateSpikes(brain, currentStep);
    
    // ========== STEP 5: Update working memory ==========
    updateWorkingMemory(brain, brain->pImpl->timestep);
    
    // ========== STEP 6: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(brain, brain->pImpl->timestep);
    
    // ========== STEP 7: Apply plasticity rules (STDP and Hebbian) ==========
    float plasticityMod = calculatePlasticityModulation(brain);
    applyPlasticityRules(brain, plasticityMod, brain->pImpl->timestep);
    
    // ========== STEP 8: Update episodic memory ==========
    updateEpisodicMemory(brain, currentStep);
    
    // ========== STEP 9-14: Additional integrated systems (prediction, cognition, etc.) ==========
    // These would be implemented in the respective modules
    
    // ========== STEP 15: Collect statistics ==========
    collectStatistics(brain);
}

void BrainStep::processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain || !brain->getSpikeSystem()) return;
    brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
}

void BrainStep::updateNeurons(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain->pImpl->timestep);
            }
        }
    }
}

void BrainStep::detectAndQueueSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain->pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    brain->getSpikeSystem()->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * brain->pImpl->timestep;

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

                        brain->getSpikeSystem()->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (brain->pImpl->workingMemory) {
                        brain->pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainStep::processImmediateSpikes(Brain* brain, SimulationStep currentStep) {
    if (!brain || !brain->getSpikeSystem()) return;
    brain->getSpikeSystem()->processSpikes(currentStep);
}

void BrainStep::updateWorkingMemory(Brain* brain, TimestepDuration timestep) {
    if (!brain || !brain->pImpl->workingMemory) return;
    brain->pImpl->workingMemory->update(timestep);
}

void BrainStep::applyNeuromodulationEffects(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Update novelty detection
    auto* novelty = brain->getNovelty();
    if (novelty) {
        novelty->update(timestep);
    }
    
    // Update curiosity
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = dopamine->getLevel();
        for (auto& region : brain->pImpl->regions) {
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

void BrainStep::applyPlasticityRules(Brain* brain, float plasticityMod, TimestepDuration timestep) {
    if (!brain) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    brain->getSTDP()->update(syn, preSpikes, postSpikes, timestep);
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
                    brain->getHebbian()->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(brain->pImpl->currentTime);
        }
    }
}

void BrainStep::updateEpisodicMemory(Brain* brain, SimulationStep currentStep) {
    if (!brain || !brain->pImpl->episodicMemory) return;
    
    brain->pImpl->stepsSinceLastEpisode++;
    if (brain->pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brain->pImpl->stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : brain->pImpl->regions) {
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
        
        brain->pImpl->episodicMemory->storeEpisode(episode);
    }
}

void BrainStep::collectStatistics(Brain* brain) {
    if (!brain) return;
    
    brain->getTotalSpikeCount(); // This updates internal statistics
    brain->getAverageFiringRate(); // This updates internal statistics
    
    // Log status periodically
    static int stepCounter = 0;
    if (++stepCounter % 100 == 0) {
        brain->logStatus();
    }
}

} // namespace nlm