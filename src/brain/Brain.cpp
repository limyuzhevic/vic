// Helper methods for improved modularity and maintainability

// Configuration validation and setup
void Brain::validateConfiguration() {
    if (!pImpl->config) {
        NLM_LOG_ERROR("Configuration is null - cannot initialize brain");
        return;
    }
    
    // Validate neuron count
    auto neuronCountOpt = pImpl->config->get<size_t>("neuron_count");
    if (!neuronCountOpt || neuronCountOpt.value() == 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + (neuronCountOpt ? std::to_string(neuronCountOpt.value()) : "0"));
        // Set to safe default
        pImpl->config->set("neuron_count", static_cast<int64_t>(1), ConfigSource::Runtime);
    }
    
    // Validate region count
    auto regionCountOpt = pImpl->config->get<size_t>("region_count");
    if (!regionCountOpt || regionCountOpt.value() == 0) {
        NLM_LOG_ERROR("Invalid region_count: " + (regionCountOpt ? std::to_string(regionCountOpt.value()) : "0"));
        pImpl->config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
    }
    
    // Validate connection probability
    auto connectionProbOpt = pImpl->config->get<float>("connection_probability");
    if (!connectionProbOpt || connectionProbOpt.value() < 0.0f || connectionProbOpt.value() > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + 
                     (connectionProbOpt ? std::to_string(connectionProbOpt.value()) : "0"));
        pImpl->config->set("connection_probability", 0.1f, ConfigSource::Runtime);
    }
}

void Brain::initializeMemorySystems() {
    if (!pImpl->workingMemory) {
        NLM_LOG_WARN("Working memory not available - skipping initialization");
    } else {
        if (!pImpl->workingMemory->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize working memory");
        } else {
            size_t neuronCount = getTotalNeuronCount();
            pImpl->workingMemory->setCapacity(neuronCount / 10);
            NLM_LOG_INFO("Working memory initialized with capacity: " + std::to_string(neuronCount / 10));
        }
    }
    
    if (!pImpl->episodicMemory) {
        NLM_LOG_WARN("Episodic memory not available - skipping initialization");
    } else {
        if (!pImpl->episodicMemory->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize episodic memory");
        } else {
            pImpl->episodicMemory->setMaxEpisodes(1000);
            NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
        }
    }
    
    if (!pImpl->associativeMemory) {
        NLM_LOG_WARN("Associative memory not available - skipping initialization");
    } else {
        if (!pImpl->associativeMemory->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize associative memory");
        } else {
            NLM_LOG_INFO("Associative memory initialized");
        }
    }
}

void Brain::initializePredictionSystem() {
    if (!pImpl->predictionSystem) {
        NLM_LOG_WARN("Prediction system not available - skipping initialization");
    } else {
        NLM_LOG_INFO("Prediction system initialized");
    }
}

void Brain::initializeCognitionSystems() {
    if (!pImpl->planner) {
        NLM_LOG_WARN("Neural planner not available - skipping initialization");
    } else {
        if (!pImpl->planner->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize neural planner");
        } else {
            pImpl->planner->setPlanningDepth(5);
            NLM_LOG_INFO("Neural planner initialized with planning depth: 5");
        }
    }
    
    if (!pImpl->conceptFormation) {
        NLM_LOG_WARN("Concept formation not available - skipping initialization");
    } else {
        if (!pImpl->conceptFormation->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize concept formation");
        } else {
            NLM_LOG_INFO("Concept formation initialized");
        }
    }
    
    if (!pImpl->attention) {
        NLM_LOG_WARN("Attentional selection not available - skipping initialization");
    } else {
        if (!pImpl->attention->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize attentional selection");
        } else {
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
            NLM_LOG_INFO("Attentional selection initialized with inhibition: 0.5, excitation: 1.5");
        }
    }
}

void Brain::initializeNeuromodulation() {
    if (!pImpl->novelty) {
        NLM_LOG_WARN("Novelty detector not available - skipping initialization");
    } else {
        if (!pImpl->novelty->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize novelty detector");
        } else {
            NLM_LOG_INFO("Novelty detector initialized");
        }
    }
    
    if (!pImpl->curiosity) {
        NLM_LOG_WARN("Curiosity system not available - skipping initialization");
    } else {
        if (!pImpl->curiosity->initialize(this)) {
            NLM_LOG_WARN("Failed to initialize curiosity system");
        } else {
            NLM_LOG_INFO("Curiosity system initialized");
        }
    }
    
    if (!pImpl->dopamine) {
        NLM_LOG_WARN("Dopamine system not available - skipping initialization");
    } else {
        // Dopamine is already initialized in Impl constructor
        NLM_LOG_INFO("Dopamine system initialized");
    }
    
    if (!pImpl->predictionError) {
        NLM_LOG_WARN("Prediction error system not available - skipping initialization");
    } else {
        NLM_LOG_INFO("Prediction error system initialized");
    }
}

void Brain::registerSpikeHandlers() {
    if (!pImpl->spikeSystem) {
        NLM_LOG_WARN("Spike system not available - skipping spike handler registration");
        return;
    }
    
    try {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
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
        NLM_LOG_INFO("Spike handlers registered successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to register spike handlers: " + std::string(e.what()));
    }
}

void Brain::configureCheckpointManager() {
    if (!pImpl->checkpointManager) {
        NLM_LOG_WARN("Checkpoint manager not available - skipping configuration");
        return;
    }
    
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    try {
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        NLM_LOG_INFO("Checkpoint manager configured with directory: " + checkpointDir);
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to configure checkpoint manager: " + std::string(e.what()));
    }
}

void Brain::configureDevelopment() {
    if (!pImpl->developmentSystem) {
        NLM_LOG_WARN("Development system not available - skipping configuration");
        return;
    }
    
    // Configure development parameters
    pImpl->developmentSystem->setPlasticityRate(0.01f);
    pImpl->developmentSystem->setCriticalPeriod(100);
    NLM_LOG_INFO("Development system configured");
}

// Step processing methods
void Brain::processStep1(SimulationStep currentStep, Timestamp currentTime) {
    // Process pending delayed spikes (deliver synaptic input)
    try {
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        } else {
            NLM_LOG_WARN("Spike system not available during delayed spike processing");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in delayed spike processing: " + std::string(e.what()));
    }
}

void Brain::processStep2(SimulationStep currentStep, Timestamp currentTime) {
    // Update all neurons (LIF dynamics)
    try {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    try {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Error updating neuron " + std::to_string(neuron->getId()) + ": " + std::string(e.what()));
                        try {
                            neuron->setMembranePotential(neuron->getState().restingPotential);
                        } catch (...) {}
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in neuron dynamics update: " + std::string(e.what()));
    }
}

void Brain::processStep3(SimulationStep currentStep, Timestamp currentTime) {
    // Detect spikes and schedule spike events
    try {
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    try {
                        const auto& state = neuron->getState();
                        bool justFired = (state.firingState == FiringState::Refractory &&
                                         state.lastSpikeTime >= 0.0f &&
                                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                        if (justFired) {
                            // Neuron fired this step - queue the spike
                            SpikeEvent event(neuron->getId(), currentTime, currentStep);
                            if (pImpl->spikeSystem) {
                                pImpl->spikeSystem->queueSpike(event);
                            }

                            // Record post-synaptic spike for incoming synapses
                            auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                            for (Synapse* syn : incomingSynapses) {
                                try {
                                    syn->recordPostSpike(currentTime);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR("Error recording post-synaptic spike for synapse " + std::to_string(syn->getId()) + ": " + std::string(e.what()));
                                }
                            }

                            // Get outgoing synapses and schedule delayed spike events
                            auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                            for (Synapse* syn : outgoingSynapses) {
                                try {
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

                                    if (pImpl->spikeSystem) {
                                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                                    }

                                    // Record pre-synaptic spike for plasticity
                                    syn->recordPreSpike(currentTime);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR("Error scheduling delayed spike for synapse " + std::to_string(syn->getId()) + ": " + std::string(e.what()));
                                }
                            }
                            
                            // Store to working memory
                            if (pImpl->workingMemory) {
                                try {
                                    pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                        std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR("Error storing neuron to working memory: " + std::string(e.what()));
                                }
                            }
                        }
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Error detecting spike for neuron " + std::to_string(neuron->getId()) + ": " + std::string(e.what()));
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in spike detection and scheduling: " + std::string(e.what()));
    }
}

void Brain::processStep4() {
    // Update working memory
    if (pImpl->workingMemory) {
        try {
            pImpl->workingMemory->update(pImpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating working memory: " + std::string(e.what()));
        }
    }
}

void Brain::processStep5() {
    // Update novelty detection
    if (pImpl->novelty) {
        try {
            pImpl->novelty->update(pImpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating novelty detector: " + std::string(e.what()));
        }
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        try {
            pImpl->curiosity->update(pImpl->timestep);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating curiosity system: " + std::string(e.what()));
        }
    }
    
    // Update dopamine
    if (pImpl->dopamine) {
        try {
            pImpl->dopamine->update(pImpl->timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = pImpl->dopamine->getLevel();
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        try {
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        } catch (const std::exception& e) {
                            NLM_LOG_ERROR("Error applying dopamine modulation to neuron " + std::to_string(neuron->getId()) + ": " + std::string(e.what()));
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating dopamine system: " + std::string(e.what()));
        }
    }
}

void Brain::processStep6() {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        try {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error getting dopamine plasticity factor: " + std::string(e.what()));
        }
    }
    
    try {
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                try {
                    // Apply STDP with neuromodulation
                    if (syn->getPlasticityFlags().stdp) {
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        
                        if (!preSpikes.empty() && !postSpikes.empty()) {
                            if (pImpl->stdp) {
                                try {
                                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                                    float weight = syn->getWeight();
                                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                                    syn->setWeight(weight);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR("Error applying STDP to synapse " + std::to_string(syn->getId()) + ": " + std::string(e.what()));
                                }
                            }
                        }
                    }
                    
                    // Apply Hebbian learning
                    if (syn->getPlasticityFlags().hebbian) {
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        
                        if (!preSpikes.empty() && !postSpikes.empty()) {
                            if (pImpl->hebbian) {
                                try {
                                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                                } catch (const std::exception& e) {
                                    NLM_LOG_ERROR("Error applying Hebbian learning to synapse " + std::to_string(syn->getId()) + ": " + std::string(e.what()));
                                }
                            }
                        }
                    }
                    
                    // Update synapse state
                    try {
                        syn->step(pImpl->currentTime);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Error updating synapse " + std::to_string(syn->getId()) + ": " + std::string(e.what()));
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error processing synapse " + std::to_string(syn->getId()) + ": " + std::string(e.what()));
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in plasticity rule application: " + std::string(e.what()));
    }
}

void Brain::processStep7(SimulationStep currentStep) {
    // Update episodic memory
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        try {
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
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating episodic memory: " + std::string(e.what()));
        }
    }
}

void Brain::processStep8() {
    // Update prediction system
    if (pImpl->predictionSystem) {
        try {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating prediction system: " + std::string(e.what()));
        }
    }
}

void Brain::processStep9() {
    // Update attention system
    if (pImpl->attention) {
        try {
            pImpl->attention->update(pImpl->timestep);
            
            // Apply attention to working memory winners
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                try {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(competitors);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error in attention competition: " + std::string(e.what()));
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating attention system: " + std::string(e.what()));
        }
    }
}

void Brain::processStep10() {
    // Update concept formation
    if (pImpl->conceptFormation) {
        try {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error updating concept formation: " + std::string(e.what()));
        }
    }
}

void Brain::processStep11() {
    // Apply structural plasticity periodically
    try {
        if (pImpl->pImpl->currentStep % 100 == 0) {
            if (pImpl->structuralPlasticity) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error applying structural plasticity: " + std::string(e.what()));
    }
}

void Brain::processStep12() {
    // Replay important memories
    try {
        if (pImpl->pImpl->currentStep % pImpl->pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            // Get episodes for replay
            try {
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during memory replay: " + std::string(e.what()));
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in memory replay processing: " + std::string(e.what()));
    }
}

void Brain::processStep13() {
    // Apply development effects
    try {
        if (pImpl->pImpl->currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->pImpl->timestep * 1000);
                
                // Development affects plasticity rates
                auto* sp = pImpl->pImpl->structuralPlasticity;
                if (sp) {
                    DevelopmentalStage stage = pImpl->pImpl->developmentalStage;
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
                    
                    try {
                        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Error updating development effects: " + std::string(e.what()));
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error applying development effects: " + std::string(e.what()));
    }
}

void Brain::processStep14() {
    // Periodic memory consolidation
    try {
        if (pImpl->pImpl->currentStep % pImpl->pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
            // Consolidate important memories, remove weak ones
            try {
                pImpl->episodicMemory->consolidate(0.3f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error during memory consolidation: " + std::string(e.what()));
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in memory consolidation processing: " + std::string(e.what()));
    }
}

void Brain::processStep15() {
    // Checkpoint management
    try {
        if (pImpl->pImpl->checkpointManager) {
            pImpl->pImpl->checkpointManager->update(pImpl->pImpl->currentStep, pImpl->pImpl->currentTime);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in checkpoint management: " + std::string(e.what()));
    }
}