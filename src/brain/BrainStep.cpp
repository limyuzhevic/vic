// Phase 6 Integration
// Implementation of the complete integrated brain simulation

#include "brain_step.h"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);

    // STEP 2: Update all neurons (LIF dynamics)
    updateNeurons(currentTime);

    // STEP 3: Detect spikes and schedule spike events
    processSpikes(currentTime, currentStep);

    // STEP 4: Update working memory (maintenance and competition)
    updateWorkingMemory();

    // STEP 5: Apply neuromodulation effects on neural excitability
    updateNeuromodulation(currentTime);

    // STEP 6: Apply plasticity rules (STDP, Hebbian)
    updatePlasticity(currentTime);

    // STEP 7: Update episodic memory with current experience
    updateEpisodicMemory();

    // STEP 8: Update prediction system
    updatePredictionSystem();

    // STEP 9: Update attention system
    updateAttention();

    // STEP 10: Update concept formation
    updateConceptFormation();

    // STEP 11: Apply structural plasticity (synaptogenesis, pruning)
    updateStructuralPlasticity(currentTime);

    // STEP 12: Replay important memories (during rest or periodically)
    replayMemories();

    // STEP 13: Apply development effects
    updateDevelopment(currentTime);

    // STEP 14: Collect statistics
    collectStatistics();
}

void Brain::updateNeurons(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::processSpikes(Timestamp currentTime, SimulationStep currentStep) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
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

    pImpl->spikeSystem->processSpikes(currentStep);
}

void Brain::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::updateNeuromodulation(Timestamp currentTime) {
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

void Brain::updatePlasticity(Timestamp currentTime) {
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

void Brain::updateEpisodicMemory() {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;

        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
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

void Brain::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateAttention() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);

        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::updateStructuralPlasticity(Timestamp currentTime) {
    if (pImpl->currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayMemories() {
    if (pImpl->currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::updateDevelopment(Timestamp currentTime) {
    if (pImpl->currentStep % 1000 == 0) {  // Update development every 1000 steps
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

void Brain::collectStatistics() {
    // Periodic memory consolidation
    if (pImpl->currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }

    // Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(pImpl->currentStep, pImpl->currentTime);
    }
}

} // namespace nlm