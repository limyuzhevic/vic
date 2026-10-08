// Brain Step Processing - Extracted from Brain.cpp
// This file contains the step processing logic for the brain simulation

#include "Brain.hpp"

namespace nlm {

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons (LIF dynamics)
    processNeurons(pImpl->regions, currentTime, pImpl->timestep);
    
    // Detect spikes and schedule spike events
    detectAndProcessSpikes(pImpl->regions, currentTime, currentStep, pImpl->timestep, pImpl->spikeSystem.get());
    
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Update integrated systems
    updateAllIntegratedSystems(pImpl.get());
    
    // Apply structural and developmental changes
    applyPeriodicPlasticity(pImpl.get(), currentStep);
    
    // Handle memory consolidation and replay
    handleMemorySystems(pImpl.get(), currentStep);
    
    // Checkpoint management
    updateCheckpointManager(pImpl.get(), currentStep, currentTime);
}

// Helper function implementations
namespace {

void processNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                   Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void detectAndProcessSpikes(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                           Timestamp currentTime, SimulationStep currentStep,
                           TimestepDuration timestep, SpikeSystem* spikeSystem) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.refractoryRemaining <= 2 &&
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
                    if (region->getWorkingMemory()) {
                        region->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void updateAllIntegratedSystems(Brain::Impl* impl) {
    if (!impl) return;
    
    // Update working memory
    if (impl->workingMemory) {
        impl->workingMemory->update(impl->timestep);
    }
    
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(impl->timestep);
    }
    
    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(impl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(impl->timestep);
        
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
    
    // Apply plasticity rules
    updatePlasticitySystem(impl->regions, impl);
    
    // Update attention system
    if (impl->attention) {
        impl->attention->update(impl->timestep);
        
        // Apply attention to working memory winners
        if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
            impl->attention->processCompetition(competitors);
        }
    }
}

void updatePlasticitySystem(std::vector<std::unique_ptr<NeuralRegion>>& regions, Brain::Impl* impl) {
    if (!impl) return;
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (impl->dopamine) {
        plasticityMod = impl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : regions) {
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
            syn->step(impl->currentTime);
        }
    }
}

void applyPeriodicPlasticity(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return;
    
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        impl->structuralPlasticity->update(nullptr, *impl->rng);  // Need proper Brain* reference
    }
}

void handleMemorySystems(Brain::Impl* impl, SimulationStep currentStep) {
    if (!impl) return;
    
    // Update episodic memory
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
    
    // Replay important memories
    if (currentStep % impl->replayInterval == 0 && impl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            impl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        impl->developmentSystem->update(nullptr, *impl->rng, impl->timestep * 1000);  // Need proper Brain* reference
        
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
    
    // Periodic memory consolidation
    if (currentStep % impl->consolidationInterval == 0 && impl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        impl->episodicMemory->consolidate(0.3f);
    }
}

void updateCheckpointManager(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    if (!impl || !impl->checkpointManager) return;
    
    // Checkpoint management
    impl->checkpointManager->update(currentStep, currentTime);
}

} // anonymous namespace

} // namespace nlm
