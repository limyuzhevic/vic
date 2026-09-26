// BrainHelpers.cpp - Implementation of helper classes for Brain operations

#include "BrainHelpers.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuronUpdater::updateAllNeurons(Brain::Impl* pImpl, Timestamp currentTime, TimestepDuration timestep) {
    // Update all neurons using efficient iteration
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                updateNeuron(neuron, currentTime, timestep);
            }
        }
    }
}

void NeuronUpdater::updateNeuron(Neuron* neuron, Timestamp currentTime, TimestepDuration timestep) {
    neuron->stepLIF(currentTime, timestep);
}

void NeuronUpdater::detectSpikes(Brain::Impl* pImpl, SimulationStep currentStep,
                                Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : pImpl->regions) {
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

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    storeToWorkingMemory(pImpl, neuron, state);
                }
            }
        }
    }
}

void NeuronUpdater::storeToWorkingMemory(Brain::Impl* pImpl, Neuron* neuron,
                                         const NeuronState& state) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
    }
}

void NeuronUpdater::applyDopamineEffects(Brain::Impl* pImpl) {
    if (!pImpl->dopamine) return;
    
    float dopamineLevel = pImpl->dopamine->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    
    if (excitabilityMod > 0.0f) {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void SpikeHandler::processDelayedSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void SpikeHandler::processSpikes(Brain::Impl* pImpl, SimulationStep currentStep) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void SpikeHandler::queueSpike(Brain::Impl* pImpl, const SpikeEvent& event) {
    pImpl->spikeSystem->queueSpike(event);
}

void SpikeHandler::queueDelayedSpike(Brain::Impl* pImpl, const DelayedSpikeEvent& event) {
    pImpl->spikeSystem->queueDelayedSpike(event);
}

void SpikeHandler::registerHandlers(Brain::Impl* pImpl) {
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([pImpl](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });

    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([pImpl](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

void MemorySystemUpdater::updateWorkingMemory(Brain::Impl* pImpl, TimestepDuration timestep) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
}

void MemorySystemUpdater::updateEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep) {
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
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void MemorySystemUpdater::consolidateMemories(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void MemorySystemUpdater::replayMemories(Brain::Impl* pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void NeuromodulatorApplier::updateAllNeuromodulators(Brain::Impl* pImpl, TimestepDuration timestep) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        applyNeuromodulationToNeurons(pImpl, pImpl->dopamine->getLevel());
    }
}

void NeuromodulatorApplier::applyNeuromodulationToNeurons(Brain::Impl* pImpl, float dopamineLevel) {
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void PlasticityUpdater::updateAllPlasticity(Brain::Impl* pImpl, SimulationStep currentStep,
                                           Timestamp currentTime, TimestepDuration timestep) {
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
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void PlasticityUpdater::applySTDP(Brain::Impl* pImpl, Synapse* syn, const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes, float plasticityMod) {
    if (syn->getPlasticityFlags().stdp && !preSpikes.empty() && !postSpikes.empty()) {
        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
        float weight = syn->getWeight();
        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
        syn->setWeight(weight);
    }
}

void PlasticityUpdater::applyHebbian(Brain::Impl* pImpl, Synapse* syn, const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes) {
    if (syn->getPlasticityFlags().hebbian && !preSpikes.empty() && !postSpikes.empty()) {
        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
    }
}

void DevelopmentUpdater::updateDevelopment(Brain::Impl* pImpl, SimulationStep currentStep,
                                           TimestepDuration timestep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        // DevelopmentSystem::update expects Brain* as first parameter, but doesn't use it
        // Pass nullptr since the implementation doesn't actually need the brain pointer
        pImpl->developmentSystem->update(nullptr, *pImpl->rng, timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            updatePlasticityBasedOnDevelopment(pImpl, pImpl->developmentalStage);
        }
    }
}
    }
}

void DevelopmentUpdater::updatePlasticityBasedOnDevelopment(Brain::Impl* pImpl, DevelopmentalStage stage) {
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
    
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

} // namespace nlm