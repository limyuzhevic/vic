// NLM Brain Step Implementation - Focused on simulation loop and integration

#include "BrainImpl.hpp"
#include "brain/NeuralRegion.hpp"
#include "sensory/SensoryInput.hpp"

namespace nlm {

// Helper functions for brain simulation steps
inline void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
inline void updateNeurons(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
inline void detectAndQueueSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
inline void storeSpikesToWorkingMemory(Brain::Impl* impl, Neuron* neuron);
inline void processImmediateSpikes(Brain::Impl* impl, SimulationStep currentStep);
inline void updateWorkingMemory(Brain::Impl* impl, TimestepDuration dt);
inline void applyNeuromodulationEffects(Brain::Impl* impl, TimestepDuration dt);
inline void applyPlasticityRules(Brain::Impl* impl, TimestepDuration dt, float plasticityMod);
inline void applyHomeostaticSynapticScaling(Brain::Impl* impl, SimulationStep currentStep);
inline void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep);
inline void updatePredictionSystem(Brain::Impl* impl);
inline void updateAttentionSystem(Brain::Impl* impl);
inline void updateConceptFormation(Brain::Impl* impl);
inline void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep);
inline void replayMemories(Brain::Impl* impl, SimulationStep currentStep);
inline void applyDevelopmentEffects(Brain::Impl* impl, SimulationStep currentStep);
inline void applyMemoryConsolidation(Brain::Impl* impl, SimulationStep currentStep);
inline void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);

// Main brain step implementation
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

    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;

    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(pImpl, currentStep, currentTime);

    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(pImpl, currentStep, currentTime);

    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndQueueSpikes(pImpl, currentStep, currentTime);

    // Process immediate spikes
    processImmediateSpikes(pImpl, currentStep);

    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(pImpl, pImpl->timestep);

    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulationEffects(pImpl, pImpl->timestep);

    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }

    applyPlasticityRules(pImpl, pImpl->timestep, plasticityMod);

    // ========== STEP 6b: Homeostatic synaptic scaling (Phase 4) ==========
    applyHomeostaticSynapticScaling(pImpl, currentStep);

    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(pImpl, currentStep);

    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(pImpl);

    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(pImpl);

    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(pImpl);

    // ========== STEP 11: Apply structural plasticity periodically ==========
    applyStructuralPlasticity(pImpl, currentStep);

    // ========== STEP 12: Replay important memories ==========
    replayMemories(pImpl, currentStep);

    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(pImpl, currentStep);

    // ========== STEP 14: Periodic memory consolidation ==========
    applyMemoryConsolidation(pImpl, currentStep);

    // ========== STEP 15: Checkpoint management ==========
    updateCheckpointManager(pImpl, currentStep, currentTime);
}

inline void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    impl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

inline void updateNeurons(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, impl->timestep);
            }
        }
    }
}

inline void detectAndQueueSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < impl->timestep * 2.0f);

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
                        Timestamp deliveryTime = currentTime + delay * impl->timestep;

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
                        impl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }
                }
            }
        }
    }
}

inline void processImmediateSpikes(Brain::Impl* impl, SimulationStep currentStep) {
    impl->spikeSystem->processSpikes(currentStep);
}

inline void updateWorkingMemory(Brain::Impl* impl, TimestepDuration dt) {
    if (impl->workingMemory) {
        impl->workingMemory->update(dt);
    }
}

inline void applyNeuromodulationEffects(Brain::Impl* impl, TimestepDuration dt) {
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(dt);
    }

    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(impl->novelty ? impl->novelty->getLevel() : 0.0f, 
                               impl->predictionError ? impl->predictionError->getError() : 0.0f, dt);
    }

    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(dt);

        // Apply dopamine effects on neural excitability
        float dopamineLevel = impl->dopamine->getLevel();
        for (auto& region : impl->regions) {
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

inline void applyPlasticityRules(Brain::Impl* impl, TimestepDuration dt, float plasticityMod) {
    for (auto& region : impl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();

                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    impl->stdp->update(syn.get(), preSpikes, postSpikes, dt);
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
                    impl->hebbian->update(syn.get(), preSpikes, postSpikes, dt);
                }
            }

            // Update synapse state
            syn->step(impl->currentTime);
        }
    }
}

inline void applyHomeostaticSynapticScaling(Brain::Impl* impl, SimulationStep currentStep) {
    // Every 100 steps, cap each neuron's total incoming excitatory
    // weight. STDP/Hebbian with soft bounds stop individual weights
    // saturating, but they do not stop the *sum* of ~25 incoming
    // weights growing until every neuron is driven over threshold on
    // every refractory cycle — the periodic attractor of Phase 3.
    // Rescaling only when the sum exceeds the cap, and scaling all
    // incoming excitatory weights by the same factor, preserves the
    // *relative* pattern of strengths (selectivity) while bounding
    // total drive. Inhibitory weights are left untouched, so scaling
    // also moves the E/I balance toward inhibition rather than away.
    // Cap 3.0: with ~5 co-active presynaptic neurons per step, total
    // drive is then at most ~5 * (3.0/25) * 10 = 6 mV per step before
    // leak — below the ~15 mV spike gap, so a neuron must integrate
    // over several steps to fire instead of firing every cycle.
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : impl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

inline void updateEpisodicMemory(Brain::Impl* impl, SimulationStep currentStep) {
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

inline void updatePredictionSystem(Brain::Impl* impl) {
    if (impl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

inline void updateAttentionSystem(Brain::Impl* impl) {
    if (impl->attention) {
        impl->attention->update(impl->timestep);

        // Apply attention to working memory winners
        if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
            impl->attention->processCompetition(competitors);
        }
    }
}

inline void updateConceptFormation(Brain::Impl* impl) {
    if (impl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

inline void applyStructuralPlasticity(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        impl->structuralPlasticity->update(nullptr, *impl->rng);
    }
}

inline void replayMemories(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % impl->replayInterval == 0 && impl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            impl->episodicMemory->replayEpisode(episode);
        }
    }
}

inline void applyDevelopmentEffects(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        impl->developmentSystem->update(nullptr, *impl->rng, impl->timestep * 1000);

        // Development affects plasticity rates
        auto* sp = impl->structuralPlasticity.get();
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

inline void applyMemoryConsolidation(Brain::Impl* impl, SimulationStep currentStep) {
    if (currentStep % impl->consolidationInterval == 0 && impl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        impl->episodicMemory->consolidate(0.3f);
    }
}

inline void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    if (impl->checkpointManager) {
        impl->checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm
