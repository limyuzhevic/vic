#include <nlm/Brain.hpp>

namespace nlm {

// Helper functions for step() method
void Brain::Impl::processDelayedSpikeEvents(SimulationStep currentStep, Timestamp currentTime) {
    // STEP 1: Process pending delayed spike events (deliver synaptic input)
    spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::Impl::updateNeuronDynamics() {
    // STEP 2: Update all neurons (LIF dynamics)
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void Brain::Impl::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // STEP 3: Detect spikes and schedule spike events
    for (auto& region : regions) {
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
                    if (workingMemory) {
                        workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }

    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);
}

void Brain::Impl::updateWorkingMemory() {
    // STEP 4: Update working memory
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void Brain::Impl::applyNeuromodulationEffects() {
    // STEP 5: Apply neuromodulation effects on neural excitability
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

void Brain::Impl::applyPlasticityRules() {
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
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

void Brain::Impl::updateEpisodicMemory(SimulationStep currentStep) {
    // STEP 7: Update episodic memory with current experience
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
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
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

void Brain::Impl::updatePredictionSystem() {
    // STEP 8: Update prediction system
    if (predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::Impl::updateAttentionSystem() {
    // STEP 9: Update attention system
    if (attention) {
        attention->update(timestep);
        
        // Apply attention to working memory winners
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void Brain::Impl::updateConceptFormation() {
    // STEP 10: Update concept formation
    if (conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::Impl::applyStructuralPlasticity(SimulationStep currentStep) {
    // STEP 11: Apply structural plasticity (synaptogenesis, pruning)
    if (currentStep % 100 == 0) {
        structuralPlasticity->update(this, *rng);
    }
}

void Brain::Impl::replayImportantMemories(SimulationStep currentStep) {
    // STEP 12: Replay important memories (during rest or periodically)
    if (currentStep % replayInterval == 0 && episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::Impl::applyDevelopmentEffects(SimulationStep currentStep) {
    // STEP 13: Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        developmentSystem->update(this, *rng, timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = developmentalStage;
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

void Brain::Impl::consolidateMemories(SimulationStep currentStep) {
    // STEP 14: Periodic memory consolidation
    if (currentStep % consolidationInterval == 0 && episodicMemory) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(0.3f);
    }
}

void Brain::Impl::checkpointManagement(SimulationStep currentStep, Timestamp currentTime) {
    // STEP 15: Checkpoint management
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm
