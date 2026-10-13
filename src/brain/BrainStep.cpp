// BrainStep.cpp - Implementation of modular Brain step execution

#include "BrainStep.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Main step execution function for the integrated brain loop
void executeStep(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
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
    
    impl->currentStep = currentStep;
    impl->currentTime = currentTime;
    impl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(impl, currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(impl, currentTime, impl->timestep);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    processSpikes(impl, currentStep);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(impl, impl->timestep);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(impl, impl->timestep);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (impl->dopamine) {
        plasticityMod = impl->dopamine->getPlasticityFactor();
    }
    
    applyPlasticity(impl, plasticityMod, currentTime);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(impl, currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(impl);
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(impl);
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(impl);
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    applyStructuralPlasticity(impl, currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(impl, currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopment(impl, currentStep, currentTime, plasticityMod);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(impl, currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    updateCheckpointManager(impl, currentStep, currentTime);
}

// Process pending delayed spike events to deliver synaptic input
void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    impl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

// Update all neurons using Leaky Integrate-and-Fire dynamics
void updateNeurons(Brain::Impl* impl, Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

// Detect spikes and schedule outgoing spike events
void processSpikes(Brain::Impl* impl, SimulationStep currentStep) {
    for (auto& region : impl->regions) {
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
                    impl->spikeSystem->queueSpike(event);

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

                        impl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (impl->workingMemory) {
                        impl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    impl->spikeSystem->processSpikes(currentStep);
}

// Update working memory system for short-term information storage
void updateWorkingMemory(Brain::Impl* impl, TimestepDuration timestep) {
    if (impl->workingMemory) {
        impl->workingMemory->update(timestep);
    }
}

// Apply neuromodulation effects on neural excitability and plasticity
void applyNeuromodulationEffects(Brain::Impl* impl, TimestepDuration timestep) {
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = impl->dopamine->getLevel();
        for (auto& region : impl->regions) {
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

// Apply synaptic plasticity rules (STDP and Hebbian)
void applyPlasticity(Brain::Impl* impl, float plasticityMod, Timestamp currentTime) {
    for (auto& region : impl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    impl->stdp->update(syn, preSpikes, postSpikes, impl->timestep);
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
                    impl->hebbian->update(syn, preSpikes, postSpikes, impl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

// Update episodic memory with current experience
void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    impl->stepsSinceLastEpisode++;
    if (impl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        impl->stepsSinceLastEpisode = 0;
        
        if (impl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : impl->regions) {
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
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            impl->episodicMemory->storeEpisode(episode);
        }
    }
}

// Update prediction system
void updatePredictionSystem(Brain::Impl* impl) {
    if (impl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

// Update attention system for competitive selection
void updateAttentionSystem(Brain::Impl* impl) {
    if (impl->attention) {
        impl->attention->update(impl->timestep);
        
        // Apply attention to working memory winners
        if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
            impl->attention->processCompetition(competitors);
        }
    }
}

// Update concept formation
void updateConceptFormation(Brain::Impl* impl) {
    if (impl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

// Apply structural plasticity periodically
void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        impl->structuralPlasticity->update(nullptr, *impl->rng);  // Note: would need Brain pointer
    }
}

// Replay important memories
void replayMemories(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % impl->replayInterval == 0 && impl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            impl->episodicMemory->replayEpisode(episode);
        }
    }
}

// Apply development effects
void applyDevelopment(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime, float plasticityMod) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        impl->developmentSystem->update(nullptr, *impl->rng, impl->timestep * 1000);  // Note: would need Brain pointer
        
        // Development affects plasticity rates
        auto* sp = impl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = impl->developmentalStage;
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

// Consolidate memories, removing weak ones
void consolidateMemories(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % impl->consolidationInterval == 0 && impl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        impl->episodicMemory->consolidate(0.3f);
    }
}

// Update checkpoint system
void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    if (impl->checkpointManager) {
        impl->checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm