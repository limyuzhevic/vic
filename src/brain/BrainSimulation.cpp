#include "BrainSimulation.hpp"

namespace nlm {

void BrainSimulation::step(Brain& brain, SimulationStep currentStep) {
    stepWithTime(brain, currentStep, static_cast<Timestamp>(currentStep) * brain.pImpl->timestep);
}

void BrainSimulation::stepWithTime(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
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
    
    brain.pImpl->currentStep = currentStep;
    brain.pImpl->currentTime = currentTime;
    brain.pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(brain, currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(brain, currentTime);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectSpikes(brain, currentStep, currentTime);
    
    // Process immediate spikes
    processSpikes(brain, currentStep);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(brain);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(brain);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    applyPlasticity(brain, currentStep, currentTime);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(brain, currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(brain);
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(brain);
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(brain);
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    applyStructuralPlasticity(brain, currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(brain, currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(brain, currentStep);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(brain, currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    checkpointManagement(brain, currentStep, currentTime);
}

void BrainSimulation::processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    brain.getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
}

void BrainSimulation::updateNeurons(Brain& brain, Timestamp currentTime) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain.pImpl->timestep);
            }
        }
    }
}

void BrainSimulation::detectSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain.pImpl->timestep * 2.0f);

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
                        Timestamp deliveryTime = currentTime + delay * brain.pImpl->timestep;

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
                        brain.getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainSimulation::processSpikes(Brain& brain, SimulationStep currentStep) {
    brain.getSpikeSystem()->processSpikes(currentStep);
}

void BrainSimulation::updateWorkingMemory(Brain& brain) {
    if (brain.getWorkingMemory()) {
        brain.getWorkingMemory()->update(brain.pImpl->timestep);
    }
}

void BrainSimulation::applyNeuromodulationEffects(Brain& brain) {
    // Update novelty detection
    if (brain.getNovelty()) {
        brain.getNovelty()->update(brain.pImpl->timestep);
    }

    // Update curiosity
    if (brain.getCuriosity()) {
        brain.getCuriosity()->update(brain.pImpl->timestep);
    }

    // Update dopamine (reward prediction error)
    if (brain.getDopamine()) {
        brain.getDopamine()->update(brain.pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = brain.getDopamine()->getLevel();
        for (auto& region : brain.pImpl->regions) {
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

void BrainSimulation::applyPlasticity(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (brain.getDopamine()) {
        plasticityMod = brain.getDopamine()->getPlasticityFactor();
    }

    for (auto& region : brain.pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    brain.getSTDP()->update(&syn, preSpikes, postSpikes, brain.pImpl->timestep);
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
                    brain.getHebbian()->update(&syn, preSpikes, postSpikes, brain.pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainSimulation::updateEpisodicMemory(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    brain.pImpl->stepsSinceLastEpisode++;
    if (brain.pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brain.pImpl->stepsSinceLastEpisode = 0;
        
        if (brain.getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : brain.pImpl->regions) {
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
            episode.reward = brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f;
            
            brain.getEpisodicMemory()->storeEpisode(episode);
        }
    }
}

void BrainSimulation::updatePredictionSystem(Brain& brain) {
    if (brain.getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainSimulation::updateAttentionSystem(Brain& brain) {
    if (brain.getAttention()) {
        brain.getAttention()->update(brain.pImpl->timestep);
        
        // Apply attention to working memory winners
        if (brain.getWorkingMemory() && !brain.getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain.getWorkingMemory()->getMemoryNeurons();
            brain.getAttention()->processCompetition(competitors);
        }
    }
}

void BrainSimulation::updateConceptFormation(Brain& brain) {
    if (brain.getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainSimulation::applyStructuralPlasticity(Brain& brain, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        brain.getStructuralPlasticity()->update(&brain, *brain.getRandomGenerator());
    }
}

void BrainSimulation::replayMemories(Brain& brain, SimulationStep currentStep) {
    if (currentStep % brain.pImpl->replayInterval == 0 && brain.getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = brain.getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain.getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void BrainSimulation::applyDevelopmentEffects(Brain& brain, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        brain.getDevelopmentSystem()->update(&brain, *brain.getRandomGenerator(), brain.pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain.getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brain.pImpl->developmentalStage;
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

void BrainSimulation::consolidateMemories(Brain& brain, SimulationStep currentStep) {
    if (currentStep % brain.pImpl->consolidationInterval == 0 && brain.getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        brain.getEpisodicMemory()->consolidate(0.3f);
    }
}

void BrainSimulation::checkpointManagement(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
    if (brain.getDevelopmentSystem()->getCheckpointManager()) {
        brain.getDevelopmentSystem()->getCheckpointManager()->update(currentStep, currentTime);
    }
}

} // namespace nlm