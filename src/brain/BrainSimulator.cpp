#include "BrainSimulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

void BrainSimulator::simulate(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Reset per-step statistics
    stats.totalSpikesProcessed = 0;
    stats.neuronsUpdated = 0;
    stats.spikesDetected = 0;
    stats.workingMemoryUpdates = 0;
    stats.neuromodulationUpdates = 0;
    stats.plasticityUpdates = 0;
    stats.memoryUpdates = 0;

    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikes(brain, currentStep, currentTime);

    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeurons(brain, currentTime);

    // ========== STEP 3: Detect spikes and schedule spike events ==========
    processSpikes(brain, currentTime);

    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(brain);

    // ========== STEP 5: Apply neuromodulation effects ==========
    updateNeuromodulation(brain);

    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    float plasticityMod = calculatePlasticityModulation(brain);
    applyPlasticity(brain, currentStep, currentTime, plasticityMod);

    // ========== STEP 6b: Homeostatic synaptic scaling ==========
    applySynapticScaling(brain, currentStep);

    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(brain, currentStep);

    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem(brain);

    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem(brain);

    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation(brain);

    // ========== STEP 11: Apply structural plasticity ==========
    applyStructuralPlasticity(brain, currentStep);

    // ========== STEP 12: Replay important memories ==========
    replayMemories(brain, currentStep);

    // ========== STEP 13: Apply development effects ==========
    applyDevelopment(brain, currentStep);

    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(brain, currentStep);

    // ========== STEP 15: Checkpoint management ==========
    updateCheckpoint(brain, currentStep, currentTime);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    stats.simulationTime = duration;
}

void BrainSimulator::processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    brain->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    stats.totalSpikesProcessed += brain->pImpl->spikeSystem->getPendingDelayedCount();
}

void BrainSimulator::updateNeurons(Brain* brain, Timestamp currentTime) {
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain->pImpl->timestep);
            }
        }
    }
    stats.neuronsUpdated = brain->getTotalNeuronCount();
}

void BrainSimulator::processSpikes(Brain* brain, Timestamp currentTime) {
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
                    SpikeEvent event(neuron->getId(), currentTime, brain->pImpl->currentStep);
                    brain->pImpl->spikeSystem->queueSpike(event);

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
                        SimulationStep deliveryStep = brain->pImpl->currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * brain->pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            brain->pImpl->currentStep,
                            deliveryStep
                        );

                        brain->pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }

                    // Store to working memory - neurons that fire become part of working memory
                    if (brain->pImpl->workingMemory) {
                        brain->pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
                    }

                    stats.spikesDetected++;
                }
            }
        }
    }

    brain->pImpl->spikeSystem->processSpikes(brain->pImpl->currentStep);
}

void BrainSimulator::updateWorkingMemory(Brain* brain) {
    if (brain->pImpl->workingMemory) {
        brain->pImpl->workingMemory->update(brain->pImpl->timestep);
        stats.workingMemoryUpdates++;
    }
}

void BrainSimulator::updateNeuromodulation(Brain* brain) {
    // Update novelty detection
    if (brain->pImpl->novelty) {
        brain->pImpl->novelty->update(brain->pImpl->timestep);
        stats.neuromodulationUpdates++;
    }

    // Update curiosity
    if (brain->pImpl->curiosity) {
        float noveltyLevel = brain->pImpl->novelty ? brain->pImpl->novelty->getLevel() : 0.0f;
        float predictionError = brain->pImpl->predictionError ? brain->pImpl->predictionError->getError() : 0.0f;
        brain->pImpl->curiosity->update(noveltyLevel, predictionError, brain->pImpl->timestep);
    }

    // Update dopamine (reward prediction error)
    if (brain->pImpl->dopamine) {
        brain->pImpl->dopamine->update(brain->pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brain->pImpl->dopamine->getLevel();
        for (auto& region : brain->pImpl->regions) {
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

void BrainSimulator::applyPlasticity(Brain* brain, SimulationStep currentStep, Timestamp currentTime, float plasticityMod) {
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();

                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    brain->pImpl->stdp->update(syn.get(), preSpikes, postSpikes, brain->pImpl->timestep);
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
                    brain->pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, brain->pImpl->timestep);
                }
            }

            // Update synapse state
            syn->step(currentTime);
        }
    }
    stats.plasticityUpdates++;
}

void BrainSimulator::applySynapticScaling(Brain* brain, SimulationStep currentStep) {
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : brain->pImpl->regions) {
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

void BrainSimulator::updateEpisodicMemory(Brain* brain, SimulationStep currentStep) {
    brain->pImpl->stepsSinceLastEpisode++;
    if (brain->pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        brain->pImpl->stepsSinceLastEpisode = 0;

        if (brain->pImpl->episodicMemory) {
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

            // Store reward in episode
            episode.reward = brain->pImpl->dopamine ? brain->pImpl->dopamine->getLevel() : 0.0f;

            brain->pImpl->episodicMemory->storeEpisode(episode);
            stats.memoryUpdates++;
        }
    }
}

void BrainSimulator::updatePredictionSystem(Brain* brain) {
    if (brain->pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainSimulator::updateAttentionSystem(Brain* brain) {
    if (brain->pImpl->attention) {
        brain->pImpl->attention->update(brain->pImpl->timestep);

        // Apply attention to working memory winners
        if (brain->pImpl->workingMemory && !brain->pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain->pImpl->workingMemory->getMemoryNeurons();
            brain->pImpl->attention->processCompetition(competitors);
        }
    }
}

void BrainSimulator::updateConceptFormation(Brain* brain) {
    if (brain->pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainSimulator::applyStructuralPlasticity(Brain* brain, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        brain->pImpl->structuralPlasticity->update(brain, *brain->pImpl->rng);
    }
}

void BrainSimulator::replayMemories(Brain* brain, SimulationStep currentStep) {
    if (currentStep % brain->pImpl->replayInterval == 0 && brain->pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = brain->pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain->pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainSimulator::applyDevelopment(Brain* brain, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        brain->pImpl->developmentSystem->update(brain, *brain->pImpl->rng, brain->pImpl->timestep * 1000);

        // Development affects plasticity rates
        auto* sp = brain->pImpl->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = brain->pImpl->developmentalStage;
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

void BrainSimulator::consolidateMemories(Brain* brain, SimulationStep currentStep) {
    if (currentStep % brain->pImpl->consolidationInterval == 0 && brain->pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        brain->pImpl->episodicMemory->consolidate(0.3f);
    }
}

void BrainSimulator::updateCheckpoint(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (brain->pImpl->checkpointManager) {
        brain->pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

float BrainSimulator::calculatePlasticityModulation(Brain* brain) const {
    float plasticityMod = 1.0f;
    if (brain->pImpl->dopamine) {
        plasticityMod = brain->pImpl->dopamine->getPlasticityFactor();
    }
    return plasticityMod;
}
