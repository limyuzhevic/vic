#include "BrainSimulation.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ===========================================
// SIMULATION LOOP MANAGEMENT
// ===========================================

void BrainSimulation::step(Brain* brain, SimulationStep currentStep) {
    step(brain, currentStep, static_cast<Timestamp>(currentStep) * 0.001);
}

void BrainSimulation::step(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;

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

    auto* spikeSystem = brain->getSpikeSystem();
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* predictionSystem = brain->getPredictionSystem();
    auto* attention = brain->getAttention();
    auto* conceptFormation = brain->getConceptFormation();
    auto* novelty = brain->getNovelty();
    auto* curiosity = brain->getCuriosity();
    auto* dopamine = brain->getDopamine();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    auto* developmentSystem = brain->getDevelopmentSystem();

    // Update simulation state
    brain->setCurrentStep(currentStep);
    brain->setCurrentTime(currentTime);
    brain->setTotalSpikesThisStep(0);

    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    spikeSystem->processDelayedSpikes(currentStep, currentTime);

    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(brain, currentTime, 0.001);

    // ========== STEP 3: Detect spikes and schedule spike events ==========
    processSpikes(brain, currentStep, spikeSystem);

    // Process immediate spikes
    spikeSystem->processSpikes(currentStep);

    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(workingMemory, 0.001);

    // ========== STEP 5: Apply neuromodulation effects ==========
    updateNovelty(novelty, currentTime, 0.001);
    updateCuriosity(curiosity, currentTime, 0.001);

    // Update dopamine (reward prediction error)
    if (dopamine) {
        dopamine->update(currentTime, 0.001);
        applyDopamineToNeurons(dopamine, brain->getNeurons());
    }

    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    applyPlasticityRules(brain, currentTime, 0.001);

    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(episodicMemory, currentStep, dopamine ? dopamine->getLevel() : 0.0f);

    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(predictionSystem);

    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(attention, workingMemory, currentTime, 0.001);

    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(conceptFormation, brain->getNeurons());

    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        applyStructuralPlasticity(brain);
    }

    // ========== STEP 12: Replay important memories ==========
    if (currentStep % 100 == 0 && episodicMemory) {  // Using default replay interval
        replayMemories(episodicMemory, 3);
    }

    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        developmentSystem->update(brain, *brain->getRandomGenerator(), 0.001 * 1000);
        applyDevelopmentEffects(brain);
    }

    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % 1000 == 0 && episodicMemory) {  // Using default consolidation interval
        consolidateMemories(episodicMemory, 0.3f);
    }

    // ========== STEP 15: Checkpoint management ==========
    auto* checkpointManager = brain->getCheckpointManager();
    if (checkpointManager) {
        checkpointManager->update(currentStep, currentTime);
    }
}

// ===========================================
// UTILITY FUNCTIONS FOR SIMULATION
// ===========================================

void BrainSimulation::updateNeurons(Brain* brain, Timestamp currentTime, TimestepDuration dt) {
    if (!brain) return;

    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, dt);
            }
        }
    }
}

void BrainSimulation::processSpikes(Brain* brain, SimulationStep currentStep,
                                    SpikeSystem* spikeSystem) {
    if (!brain || !spikeSystem) return;

    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 0.002f);

                if (justFired) {
                    // Count spike
                    brain->setTotalSpikesThisStep(brain->getTotalSpikesThisStep() + 1);
                    brain->setTotalSpikesTotal(brain->getTotalSpikesTotal() + 1);

                    // Record spike for plasticity
                    neuron->recordPostSpike(currentTime);
                }
            }
        }
    }
}

void BrainSimulation::updateWorkingMemory(NeuralWorkingMemory* workingMemory, TimestepDuration dt) {
    if (!workingMemory) return;

    workingMemory->update(dt);
}

void BrainSimulation::updateNovelty(Novelty* novelty, Timestamp currentTime, TimestepDuration dt) {
    if (!novelty) return;

    novelty->update(dt);
}

void BrainSimulation::updateCuriosity(Curiosity* curiosity, Timestamp currentTime, TimestepDuration dt) {
    if (!curiosity) return;

    curiosity->update(dt);
}

void BrainSimulation::applyPlasticityRules(Brain* brain, Timestamp currentTime, TimestepDuration dt) {
    if (!brain) return;

    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) return;

    float plasticityMod = 1.0f;
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }

    for (auto& region : brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();

                if (!preSpikes.empty() && !postSpikes.empty()) {
                    spikeSystem->getSTDP()->update(syn, preSpikes, postSpikes, dt);
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
                    spikeSystem->getHebbian()->update(syn, preSpikes, postSpikes, dt);
                }
            }

            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainSimulation::updateEpisodicMemory(NeuralEpisodicMemory* episodicMemory,
                                          size_t currentStep, float reward) {
    if (!episodicMemory) return;

    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;

    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;

        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = reward;

        // Would capture current neural activity patterns here
        episodicMemory->storeEpisode(episode);
    }
}

void BrainSimulation::updatePredictionSystem(PredictionSystem* predictionSystem) {
    if (!predictionSystem) return;

    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

void BrainSimulation::updateAttentionSystem(AttentionalSelection* attention,
                                           NeuralWorkingMemory* workingMemory,
                                           Timestamp currentTime, TimestepDuration dt) {
    if (!attention) return;

    attention->update(dt);

    // Apply attention to working memory winners
    if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        attention->processCompetition(competitors);
    }
}

void BrainSimulation::updateConceptFormation(ConceptFormation* conceptFormation,
                                            const std::vector<Neuron*>& neurons) {
    if (!conceptFormation) return;

    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
}

void BrainSimulation::applyStructuralPlasticity(Brain* brain) {
    if (!brain) return;

    brain->getStructuralPlasticity()->update(brain, *brain->getRandomGenerator());
}

void BrainSimulation::replayMemories(NeuralEpisodicMemory* episodicMemory, size_t replayCount) {
    if (!episodicMemory) return;

    auto episodesToReplay = episodicMemory->getEpisodesForReplay(replayCount);
    for (const auto* episode : episodesToReplay) {
        episodicMemory->replayEpisode(episode);
    }
}

void BrainSimulation::consolidateMemories(NeuralEpisodicMemory* episodicMemory, float consolidationRate) {
    if (!episodicMemory) return;

    episodicMemory->consolidate(consolidationRate);
}

void BrainSimulation::applyDevelopmentEffects(Brain* brain) {
    if (!brain) return;

    // Development affects plasticity rates
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        DevelopmentalStage stage = brain->getDevelopmentalStage();
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

void BrainSimulation::applyDopamineToNeurons(Dopamine* dopamine, const std::vector<Neuron*>& neurons) {
    if (!dopamine) return;

    float dopamineLevel = dopamine->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto* neuron : neurons) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
}

} // namespace nlm
