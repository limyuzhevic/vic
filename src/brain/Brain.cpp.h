#pragma once

#include "Brain.hpp"
#include "Brain.cpp" // Include the main brain.cpp to get the Impl struct

namespace nlm {

namespace BrainInitialization {
    
    bool initialize(Brain& brain) {
        auto& pImpl = *brain.pImpl;
        
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
        
        // Get configuration values
        size_t neuronCount = pImpl.config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl.config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl.config->getOr<float>("connection_probability", 0.1f);
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            brain.addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = brain.getRegion(RegionId(i + 1));
            if (region) {
                // Add populations
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        pImpl.sensoryNeurons.push_back(neuron);
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        pImpl.motorNeurons.push_back(neuron);
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                             ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons");
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = brain.getRegion(RegionId(i + 1));
            if (region) {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl.rng, connectionProbability, 0.2f, 0.1f);
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory
        pImpl.workingMemory->initialize(&brain);
        pImpl.workingMemory->setCapacity(neuronCount / 10);
        
        // Initialize episodic memory
        pImpl.episodicMemory->initialize(&brain);
        pImpl.episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory
        pImpl.associativeMemory->initialize(&brain);
        
        // Initialize prediction system
        // (PredictionSystem doesn't have initialize method currently)
        
        // Initialize cognition systems
        pImpl.planner->initialize(&brain);
        pImpl.planner->setPlanningDepth(5);
        
        pImpl.conceptFormation->initialize(&brain);
        
        pImpl.attention->initialize(&brain);
        pImpl.attention->setInhibitionStrength(0.5f);
        pImpl.attention->setExcitationStrength(1.5f);
        
        // Initialize neuromodulation
        pImpl.novelty->initialize(&brain);
        pImpl.curiosity->initialize(&brain);
        
        // Register spike handlers for event-driven processing
        pImpl.spikeSystem->registerHandler([&brain](const DetailedSpikeEvent& event) {
            // Count spikes
            ++brain.pImpl->totalSpikesThisStep;
            ++brain.pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        pImpl.spikeSystem->registerDelayedHandler([&brain](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : brain.pImpl->regions) {
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
        
        // Configure checkpoint manager
        std::string checkpointDir = pImpl.config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl.checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl.sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl.motorNeurons.size()));
        
        return true;
    }
}

namespace BrainIO {
    
    void addRegion(Brain& brain, const std::string& name) {
        brain.pImpl->regions.push_back(std::make_unique<NeuralRegion>(name, brain.pImpl->nextRegionId++));
    }
    
    NeuralRegion* getRegion(Brain& brain, RegionId regionId) {
        for (auto& region : brain.pImpl->regions) {
            if (region->getId() == regionId) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegion(const Brain& brain, RegionId regionId) {
        for (const auto& region : brain.pImpl->regions) {
            if (region->getId() == regionId) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    std::vector<NeuralRegion*> getAllRegions(Brain& brain) {
        std::vector<NeuralRegion*> regions;
        regions.reserve(brain.pImpl->regions.size());
        for (auto& region : brain.pImpl->regions) {
            regions.push_back(region.get());
        }
        return regions;
    }
    
    const std::vector<NeuralRegion*>& getAllRegions(const Brain& brain) {
        return brain.pImpl->regions;
    }
    
    void receiveSensoryInput(Brain& brain, const class SensoryInput& input) {
        // Inject current into sensory neurons based on input
        // This is a simple mapping - sensory encoding
        
        const auto& values = input.getData();
        if (values.empty()) return;
        
        size_t numSensory = brain.pImpl->sensoryNeurons.size();
        if (numSensory == 0) return;
        
        // Distribute input across sensory neurons
        for (size_t i = 0; i < numSensory; ++i) {
            // Normalize input value to range [-10, 10] mV
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Inject current into this sensory neuron
            brain.pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            if (brain.pImpl->workingMemory && normalizedValue > 0.5f) {
                brain.pImpl->workingMemory->storeToNeuron(brain.pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
    
    void injectCurrent(Brain& brain, NeuronId neuron, MembranePotential current) {
        for (auto& region : brain.pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    }
    
    void injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current) {
        for (auto& region : brain.pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
    
    std::unique_ptr<class Action> produceAction(Brain& brain) {
        // Simple action selection based on motor neuron activity
        // The motor neuron population with highest average activity determines action
        
        if (brain.pImpl->motorNeurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        float maxActivity = 0.0f;
        NeuronId selectedNeuron;
        
        for (auto* neuron : brain.pImpl->motorNeurons) {
            if (neuron->getState().firingRate > maxActivity) {
                maxActivity = neuron->getState().firingRate;
                selectedNeuron = neuron->getId();
            }
        }
        
        if (maxActivity > 0.0f) {
            return std::make_unique<Action>(ActionType::MoveForward, maxActivity);
        }
        
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    float getExcitationInhibitionRatio(const Brain& brain) {
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (const auto& region : brain.pImpl->regions) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                if (weight > 0) {
                    totalExcitatory += weight;
                } else {
                    totalInhibitory += std::abs(weight);
                }
            }
        }
        
        if (totalInhibitory > 0.0f) {
            return totalExcitatory / totalInhibitory;
        }
        return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
    }
    
    void reset(Brain& brain) {
        auto& pImpl = *brain.pImpl;
        
        // Reset neuron states
        for (auto& region : pImpl.regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->reset();
                }
            }
        }
        
        // Reset memories
        if (pImpl.workingMemory) {
            pImpl.workingMemory->clear();
        }
        if (pImpl.episodicMemory) {
            pImpl.episodicMemory->clear();
        }
        if (pImpl.associativeMemory) {
            pImpl.associativeMemory->clear();
        }
        
        // Reset simulation state
        pImpl.currentStep = 0;
        pImpl.currentTime = 0.0;
        pImpl.totalSpikesThisStep = 0;
        pImpl.totalSpikesTotal = 0;
        pImpl.isResting = false;
        pImpl.stepsSinceLastEpisode = 0;
        
        // Reset neuromodulation systems
        if (pImpl.dopamine) {
            pImpl.dopamine->reset();
        }
        if (pImpl.curiosity) {
            pImpl.curiosity->reset();
        }
        if (pImpl.predictionError) {
            pImpl.predictionError->reset();
        }
        if (pImpl.novelty) {
            pImpl.novelty->reset();
        }
        
        // Reset plasticity systems
        if (pImpl.spikeSystem) {
            pImpl.spikeSystem->clearEvents();
        }
        if (pImpl.stdp) {
            pImpl.stdp->reset();
        }
        if (pImpl.hebbian) {
            pImpl.hebbian->reset();
        }
        
        NLM_LOG_INFO("Brain reset complete");
    }
}

namespace BrainSimulation {
    
    void step(Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
        auto& pImpl = *brain.pImpl;
        
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
        
        pImpl.currentStep = currentStep;
        pImpl.currentTime = currentTime;
        pImpl.totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        pImpl.spikeSystem->processDelayedSpikes(currentStep, currentTime);
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        for (auto& region : pImpl.regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl.timestep);
                }
            }
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        for (auto& region : pImpl.regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Check if neuron just fired this step
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl.timestep * 2.0f);
                    
                    if (justFired) {
                        // Neuron fired this step - queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        pImpl.spikeSystem->queueSpike(event);
                        
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
                            Timestamp deliveryTime = currentTime + delay * pImpl.timestep;
                            
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
                            
                            pImpl.spikeSystem->queueDelayedSpike(delayedEvent);
                            
                            // Record pre-synaptic spike for plasticity
                            syn->recordPreSpike(currentTime);
                        }
                        
                        // Store to working memory - neurons that fire become part of working memory
                        if (pImpl.workingMemory) {
                            pImpl.workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        pImpl.spikeSystem->processSpikes(currentStep);
        
        // ========== STEP 4: Update working memory ==========
        if (pImpl.workingMemory) {
            pImpl.workingMemory->update(pImpl.timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        // Update novelty detection
        if (pImpl.novelty) {
            pImpl.novelty->update(pImpl.timestep);
        }
        
        // Update curiosity
        if (pImpl.curiosity) {
            pImpl.curiosity->update(pImpl.timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (pImpl.dopamine) {
            pImpl.dopamine->update(pImpl.timestep);
            
            // Apply dopamine effects on neural excitability
            // Dopamine modulates neural excitability by adjusting effective current injection
            // Higher dopamine increases excitability (lower effective threshold)
            float dopamineLevel = pImpl.dopamine->getLevel();
            for (auto& region : pImpl.regions) {
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
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl.dopamine) {
            plasticityMod = pImpl.dopamine->getPlasticityFactor();
        }
        
        for (auto& region : pImpl.regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        pImpl.stdp->update(syn, preSpikes, postSpikes, pImpl.timestep);
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
                        pImpl.hebbian->update(syn, preSpikes, postSpikes, pImpl.timestep);
                    }
                }
                
                // Update synapse state
                syn->step(currentTime);
            }
        }
        
        // ========== STEP 7: Update episodic memory ==========
        pImpl.stepsSinceLastEpisode++;
        if (pImpl.stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl.stepsSinceLastEpisode = 0;
            
            if (pImpl.episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl.dopamine ? pImpl.dopamine->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : pImpl.regions) {
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
                episode.reward = pImpl.dopamine ? pImpl.dopamine->getLevel() : 0.0f;
                
                pImpl.episodicMemory->storeEpisode(episode);
            }
        }
        
        // ========== STEP 8: Update prediction system ==========
        if (pImpl.predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
        
        // ========== STEP 9: Update attention system ==========
        if (pImpl.attention) {
            pImpl.attention->update(pImpl.timestep);
            
            // Apply attention to working memory winners
            if (pImpl.workingMemory && !pImpl.workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = pImpl.workingMemory->getMemoryNeurons();
                pImpl.attention->processCompetition(competitors);
            }
        }
        
        // ========== STEP 10: Update concept formation ==========
        if (pImpl.conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0) {
            pImpl.structuralPlasticity->update(&brain, *pImpl.rng);
        }
        
        // ========== STEP 12: Replay important memories ==========
        if (currentStep % pImpl.replayInterval == 0 && pImpl.episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = pImpl.episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                pImpl.episodicMemory->replayEpisode(episode);
            }
        }
        
        // ========== STEP 13: Apply development effects ==========
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            pImpl.developmentSystem->update(&brain, *pImpl.rng, pImpl.timestep * 1000);
            
            // Development affects plasticity rates
            auto* sp = pImpl.structuralPlasticity;
            if (sp) {
                DevelopmentalStage stage = pImpl.developmentalStage;
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
        
        // ========== STEP 14: Periodic memory consolidation ==========
        if (currentStep % pImpl.consolidationInterval == 0 && pImpl.episodicMemory) {
            // Consolidate important memories, remove weak ones
            pImpl.episodicMemory->consolidate(0.3f);
        }
        
        // ========== STEP 15: Checkpoint management ==========
        if (pImpl.checkpointManager) {
            pImpl.checkpointManager->update(currentStep, currentTime);
        }
    }
}

} // namespace nlm
