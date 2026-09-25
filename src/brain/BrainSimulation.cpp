#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Brain simulation implementation - main step functions

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
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
    
    try {
        // Validate inputs
        if (currentStep < 0) {
            NLM_LOG_ERROR("Current step cannot be negative");
            return;
        }
        if (currentTime < 0.0f) {
            NLM_LOG_ERROR("Current time cannot be negative");
            return;
        }
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // ========== PERFORMANCE OPTIMIZATION: STEP 2: Update all neurons (LIF dynamics) ==========
        // OPTIMIZED: Create flattened list of all neurons to avoid triple nested loops
        std::vector<Neuron*> allNeurons;
        allNeurons.reserve(pImpl->regions.size() * 10);  // Estimate
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                const auto& neurons = pop->getNeurons();
                allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
            }
        }
        
        // Update all neurons using single loop (O(n) instead of O(n³))
        for (auto* neuron : allNeurons) {
            if (neuron) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
        
        // ========== PERFORMANCE OPTIMIZATION: STEP 3: Detect spikes and schedule spike events ==========
        // OPTIMIZED: Use pre-computed neuron list and cache region references
        std::vector<Neuron*> spikingNeurons;
        
        for (auto* neuron : allNeurons) {
            if (!neuron) continue;
            
            const auto& state = neuron->getState();
            bool justFired = (state.firingState == FiringState::Refractory &&
                             state.lastSpikeTime >= 0.0f &&
                             std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

            if (justFired) {
                // Neuron fired this step - queue the spike
                SpikeEvent event(neuron->getId(), currentTime, currentStep);
                pImpl->spikeSystem->queueSpike(event);
                spikingNeurons.push_back(neuron);
                
                // Record post-synaptic spike for incoming synapses (plasticity)
                // OPTIMIZED: Cache region reference
                auto* region = neuron->getRegionId() ? getRegion(neuron->getRegionId()) : nullptr;
                if (region) {
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }
                }
                
                // Get outgoing synapses and schedule delayed spike events
                // OPTIMIZED: Cache region reference and use more efficient synapse access
                if (region) {
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
                }
                
                // Store to working memory - neurons that fire become part of working memory
                if (pImpl->workingMemory) {
                    pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                        std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                }
            }
        }
        
        // Process immediate spikes
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        // ========== STEP 4: Update working memory ==========
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(pImpl->timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
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
            if (dopamineLevel > 0.0f) {
                // OPTIMIZED: Batch update for neurons affected by neuromodulation
                float excitabilityMod = dopamineLevel * 0.5f;
                for (auto* neuron : allNeurons) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
        
        // ========== PERFORMANCE OPTIMIZATION: STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        // OPTIMIZED: Cache synapse lists and use more efficient iteration
        std::vector<Synapse*> allSynapses;
        for (auto& region : pImpl->regions) {
            const auto& synapses = region->getSynapses();
            allSynapses.insert(allSynapses.end(), synapses.begin(), synapses.end());
        }
        
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        // Process all synapses in a single loop
        for (Synapse* syn : allSynapses) {
            if (!syn) continue;
            
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
        
        // ========== STEP 7: Update episodic memory ==========
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Store active neurons - OPTIMIZED: Use cached neuron list
                for (auto* neuron : allNeurons) {
                    if (!neuron) continue;
                    const auto& state = neuron->getState();
                    if (neuron->isFiring() || 
                        std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                    }
                }
                
                // Store reward in episode
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                pImpl->episodicMemory->storeEpisode(episode);
            }
        }
        
        // ========== STEP 8: Update prediction system ==========
        if (pImpl->predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
        
        // ========== STEP 9: Update attention system ==========
        if (pImpl->attention) {
            pImpl->attention->update(pImpl->timestep);
            
            // Apply attention to working memory winners
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                pImpl->attention->processCompetition(competitors);
            }
        }
        
        // ========== STEP 10: Update concept formation ==========
        if (pImpl->conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0) {
            if (pImpl->structuralPlasticity) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        }
        
        // ========== STEP 12: Replay important memories ==========
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
        
        // ========== STEP 13: Apply development effects ==========
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
                
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
        
        // ========== STEP 14: Periodic memory consolidation ==========
        if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
            // Consolidate important memories, remove weak ones
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // ========== STEP 15: Checkpoint management ==========
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->update(currentStep, currentTime);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain step: ") + e.what());
        // Continue with default behavior even if optimization fails
    }
}

} // namespace nlm