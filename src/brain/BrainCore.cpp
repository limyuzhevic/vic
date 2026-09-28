#include "BrainCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/ExperimentRunner.hpp"

namespace nlm {

bool BrainCore::initialize(Brain::Impl& pImpl) {
    try {
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Core Neural Computation)...");
        
        // Validate configuration
        if (!pImpl->config) {
            NLM_LOG_ERROR("Failed to initialize brain: Configuration is null");
            return false;
        }
        
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration values
        if (neuronCount == 0) {
            NLM_LOG_WARNING("Zero neuron count specified, using default of 1000");
            neuronCount = 1000;
        }
        if (regionCount == 0) {
            NLM_LOG_WARNING("Zero region count specified, using default of 1");
            regionCount = 1;
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_WARNING("Invalid connection probability (" + std::to_string(connectionProbability) + "), using default of 0.1");
            connectionProbability = 0.1f;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " +
                     std::to_string(connectionProbability) + " connection probability");
        
        // Validate random generator
        if (!pImpl->rng) {
            NLM_LOG_ERROR("Failed to initialize brain: Random generator is not initialized");
            return false;
        }
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                pImpl->addRegion("Region_" + std::to_string(i + 1));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1) + ": " + e.what());
                return false;
            }
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = pImpl->getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1));
                return false;
            }
            
            try {
                // Add populations
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        if (neuron) {
                            pImpl->sensoryNeurons.push_back(neuron);
                        }
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        if (neuron) {
                            pImpl->motorNeurons.push_back(neuron);
                        }
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                             ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to create neurons in region " + std::to_string(i + 1) + ": " + e.what());
                return false;
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = pImpl->getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " for connectivity initialization");
                return false;
            }
            
            try {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to initialize connectivity in region " + std::to_string(i + 1) + ": " + e.what());
                return false;
            }
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Core)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain initialization: ") + e.what());
        return false;
    }
}

void BrainCore::step(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: CORE BRAIN LOOP
     * 
     * This implements the complete core brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Process immediate spikes
     * 5. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
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
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainCore::reset(Brain::Impl& pImpl) {
    try {
        NLM_LOG_INFO("Resetting NLM Brain Core...");
        
        // Reset simulation state
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        
        // Reset regions
        for (auto& region : pImpl->regions) {
            try {
                region->reset();
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to reset region: " + e.what());
            }
        }
        
        // Reset spike system
        try {
            pImpl->spikeSystem->reset();
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to reset spike system: " + e.what());
        }
        
        // Reset developmental stage
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (pImpl->workingMemory) {
            try {
                pImpl->workingMemory->clear();
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to clear working memory: " + e.what());
            }
        }
        
        if (pImpl->episodicMemory) {
            try {
                pImpl->episodicMemory->clear();
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to clear episodic memory: " + e.what());
            }
        }
        
        if (pImpl->associativeMemory) {
            try {
                pImpl->associativeMemory->clear();
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to clear associative memory: " + e.what());
            }
        }
        
        if (pImpl->attention) {
            try {
                pImpl->attention->reset();
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to reset attention system: " + e.what());
            }
        }
        
        NLM_LOG_INFO("NLM Brain Core reset complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain reset: ") + e.what());
    }
}

void BrainCore::develop(Brain::Impl& pImpl, double timestep) {
    pImpl->developmentalStage = pImpl->developmentSystem->update(pImpl.get(), *pImpl->rng, timestep);
    
    // Development affects plasticity rates
    auto* sp = pImpl->structuralPlasticity.get();
    if (sp) {
        float plasticityMod = 1.0f;
        
        switch (pImpl->developmentalStage) {
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

void BrainCore::applyNeuromodulation(Brain::Impl& pImpl, const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void BrainCore::updatePlasticity(Brain::Impl& pImpl) {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

std::unique_ptr<Action> BrainCore::produceAction(const Brain::Impl& pImpl) {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void BrainCore::processSpikes(Brain::Impl& pImpl, SimulationStep currentStep) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainCore::processDelayedSpikes(Brain::Impl& pImpl, SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainCore::updateMemorySystems(Brain::Impl& pImpl, double timestep) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
}

void BrainCore::updatePredictionSystem(Brain::Impl& pImpl) {
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

void BrainCore::updateCognitionSystems(Brain::Impl& pImpl, double timestep) {
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    if (pImpl->planner) {
        pImpl->planner->update(timestep);
    }
}

void BrainCore::updateDevelopmentSystem(Brain::Impl& pImpl, double timestep, SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(pImpl.get(), *pImpl->rng, timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity.get();
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

void BrainCore::applyStructuralPlasticity(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(pImpl.get(), *pImpl->rng);
    }
}

void BrainCore::replayMemories(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainCore::consolidateMemories(Brain::Impl& pImpl, SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

} // namespace nlm
