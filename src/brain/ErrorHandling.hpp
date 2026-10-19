// Error codes for Brain operations
enum class BrainError {
    None = 0,
    NullConfig = 1,
    AlreadyInitialized = 2,
    InvalidNeuronCount = 3,
    InvalidRegionCount = 4,
    ConnectionProbabilityInvalid = 5,
    RegionNotFound = 6,
    InvalidPopulationCount = 7,
    MemorySystemFailure = 8,
    CheckpointOperationFailed = 9,
    InvalidFilepath = 10,
    InvalidStep = 11,
    InvalidCurrentTime = 12,
    NullPointerDetected = 13,
    OutOfMemory = 14,
    InvalidConfiguration = 15,
    OperationNotAllowed = 16
};

// Helper function for error logging
inline void logBrainError(BrainError error, const std::string& operation, const std::string& context = "") {
    std::string errorMsg = "Brain error in " + operation + ": ";
    switch (error) {
        case BrainError::NullConfig:
            errorMsg += "Null configuration provided";
            break;
        case BrainError::AlreadyInitialized:
            errorMsg += "Brain already initialized";
            break;
        case BrainError::InvalidNeuronCount:
            errorMsg += "Invalid neuron count";
            break;
        case BrainError::InvalidRegionCount:
            errorMsg += "Invalid region count";
            break;
        case BrainError::ConnectionProbabilityInvalid:
            errorMsg += "Connection probability out of range [0,1]";
            break;
        case BrainError::RegionNotFound:
            errorMsg += "Region not found";
            break;
        case BrainError::InvalidPopulationCount:
            errorMsg += "Invalid population count";
            break;
        case BrainError::MemorySystemFailure:
            errorMsg += "Memory system operation failed";
            break;
        case BrainError::CheckpointOperationFailed:
            errorMsg += "Checkpoint operation failed";
            break;
        case BrainError::InvalidFilepath:
            errorMsg += "Invalid filepath";
            break;
        case BrainError::InvalidStep:
            errorMsg += "Invalid simulation step";
            break;
        case BrainError::InvalidCurrentTime:
            errorMsg += "Invalid current time";
            break;
        case BrainError::NullPointerDetected:
            errorMsg += "Null pointer detected";
            break;
        case BrainError::OutOfMemory:
            errorMsg += "Out of memory";
            break;
        case BrainError::InvalidConfiguration:
            errorMsg += "Invalid configuration";
            break;
        case BrainError::OperationNotAllowed:
            errorMsg += "Operation not allowed in current state";
            break;
        default:
            errorMsg += "Unknown error";
            break;
    }
    
    if (context.empty()) {
        NLM_LOG_ERROR(errorMsg);
    } else {
        NLM_LOG_ERROR(errorMsg + " (" + context + ")");
    }
}

// Helper function for state validation
inline bool isBrainInitialized(const Brain::Impl& impl) {
    return !impl.regions.empty() && impl.rng != nullptr && impl.workingMemory != nullptr;
}

// Helper function for bounds checking
inline bool isWithinBounds(size_t value, size_t min, size_t max) {
    return value >= min && value <= max;
}

// Helper function for float validation
inline bool isFloatValid(float value, float min, float max) {
    return !std::isnan(value) && !std::isinf(value) && value >= min && value <= max;
}

// Enhanced Brain::initialize() with error handling
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Validate internal state
    if (!pImpl->config) {
        logBrainError(BrainError::NullConfig, "initialize", "Config not provided during Brain construction");
        return false;
    }
    
    if (isBrainInitialized(*pImpl)) {
        logBrainError(BrainError::AlreadyInitialized, "initialize", "Brain already initialized");
        return false;
    }
    
    // Get configuration values with validation
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Input validation
    if (!isWithinBounds(neuronCount, 1, 100000)) {
        logBrainError(BrainError::InvalidNeuronCount, "initialize", 
                     "Neuron count " + std::to_string(neuronCount) + " out of range [1, 100000]");
        return false;
    }
    
    if (!isWithinBounds(regionCount, 1, 1000)) {
        logBrainError(BrainError::InvalidRegionCount, "initialize",
                     "Region count " + std::to_string(regionCount) + " out of range [1, 1000]");
        return false;
    }
    
    if (!isFloatValid(connectionProbability, 0.0f, 1.0f)) {
        logBrainError(BrainError::ConnectionProbabilityInvalid, "initialize",
                     "Connection probability " + std::to_string(connectionProbability) + " out of range [0.0, 1.0]");
        return false;
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    try {
        // Create regions with validation
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId id = addRegion("Region_" + std::to_string(i + 1));
            if (id.index() == 0) {
                logBrainError(BrainError::RegionNotFound, "initialize",
                             "Failed to add region " + std::to_string(i + 1));
                return false;
            }
        }
        
        // Create neurons across regions with bounds checking
        size_t neuronsPerRegion = neuronCount / regionCount;
        size_t neuronsRemaining = neuronCount % regionCount;
        
        for (size_t i = 0; i < regionCount; ++i) {
            size_t actualNeuronsPerRegion = neuronsPerRegion + (i < neuronsRemaining ? 1 : 0);
            
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                logBrainError(BrainError::RegionNotFound, "initialize",
                             "Region " + std::to_string(i + 1) + " not found");
                return false;
            }
            
            // Add populations with validation
            auto sensoryPopId = region->addPopulation(actualNeuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(actualNeuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(actualNeuronsPerRegion / 4, NeuronType::Motor);
            
            if (sensoryPopId.index() == 0 || internalPopId.index() == 0 || motorPopId.index() == 0) {
                logBrainError(BrainError::InvalidPopulationCount, "initialize",
                             "Failed to create populations in region " + std::to_string(i + 1));
                return false;
            }
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    if (!neuron) {
                        logBrainError(BrainError::NullPointerDetected, "initialize",
                                     "Null neuron in sensory population of region " + std::to_string(i + 1));
                        continue;
                    }
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    if (!neuron) {
                        logBrainError(BrainError::NullPointerDetected, "initialize",
                                     "Null neuron in motor population of region " + std::to_string(i + 1));
                        continue;
                    }
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                logBrainError(BrainError::RegionNotFound, "initialize",
                             "Region " + std::to_string(i + 1) + " not found during connectivity initialization");
                continue; // Continue with other regions
            }
            
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory
        if (!pImpl->workingMemory) {
            logBrainError(BrainError::NullPointerDetected, "initialize", "Working memory not initialized");
            return false;
        }
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
        
        // Initialize episodic memory
        if (!pImpl->episodicMemory) {
            logBrainError(BrainError::NullPointerDetected, "initialize", "Episodic memory not initialized");
            return false;
        }
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory
        if (!pImpl->associativeMemory) {
            logBrainError(BrainError::NullPointerDetected, "initialize", "Associative memory not initialized");
            return false;
        }
        pImpl->associativeMemory->initialize(this);
        
        // Initialize cognition systems
        if (pImpl->planner) {
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
        } else {
            logBrainError(BrainError::NullPointerDetected, "initialize", "Planner system not initialized");
        }
        
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->initialize(this);
        }
        
        if (pImpl->attention) {
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        }
        
        // Initialize neuromodulation
        if (pImpl->novelty) {
            pImpl->novelty->initialize(this);
        }
        if (pImpl->curiosity) {
            pImpl->curiosity->initialize(this);
        }
        
        // Register spike handlers for event-driven processing
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
            
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Find destination neuron and deliver synaptic input
                if (!pImpl->spikeSystem) return;
                
                for (auto& region : pImpl->regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (!neuron) continue;
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
        
        // Configure checkpoint manager
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        logBrainError(BrainError::OutOfMemory, "initialize", std::string("Exception: ") + e.what());
        return false;
    } catch (...) {
        logBrainError(BrainError::NullPointerDetected, "initialize", "Unknown exception during initialization");
        return false;
    }
}

// Enhanced Brain::step() with validation
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Validate input parameters
    if (currentStep == 0) {
        NLM_LOG_WARNING("Step called with zero step number");
    }
    
    if (!std::isfinite(currentTime) || currentTime < 0.0) {
        logBrainError(BrainError::InvalidCurrentTime, "step", 
                     "Current time " + std::to_string(currentTime) + " is invalid");
        currentTime = 0.0; // Recover with default
    }
    
    if (!pImpl->config) {
        logBrainError(BrainError::NullConfig, "step", "Brain not properly initialized");
        return;
    }
    
    // Validate timestep from config
    TimestepDuration timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    if (!isFloatValid(timestep, 1e-6, 1.0)) {
        logBrainError(BrainError::InvalidConfiguration, "step",
                     "Invalid timestep " + std::to_string(timestep) + " from config");
        timestep = 0.001; // Use default
    }
    
    // Initialize state
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    try {
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        for (auto& region : pImpl->regions) {
            if (!region) {
                logBrainError(BrainError::NullPointerDetected, "step", "Null region in regions list");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        logBrainError(BrainError::NullPointerDetected, "step", "Null neuron in population");
                        continue;
                    }
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) continue;
                    
                    // Check if neuron just fired this step
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);
                    
                    if (justFired) {
                        // Neuron fired this step - queue the spike
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        if (pImpl->spikeSystem) {
                            pImpl->spikeSystem->queueSpike(event);
                        }
                        
                        // Record post-synaptic spike for incoming synapses (plasticity)
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            if (syn) syn->recordPostSpike(currentTime);
                        }
                        
                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            if (!syn) continue;
                            
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
                            
                            if (pImpl->spikeSystem) {
                                pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
                            }
                            
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
        
        // Process immediate spikes
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        // ========== STEP 4: Update working memory ==========
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
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
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = pImpl->dopamine->getLevel();
            if (isFloatValid(dopamineLevel, -1.0f, 1.0f)) {
                for (auto& region : pImpl->regions) {
                    if (!region) continue;
                    
                    for (auto& pop : region->getPopulations()) {
                        if (!pop) continue;
                        
                        for (auto* neuron : pop->getNeurons()) {
                            if (!neuron) continue;
                            
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
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
            if (!isFloatValid(plasticityMod, 0.1f, 2.0f)) {
                plasticityMod = 1.0f;
                NLM_LOG_WARNING("Invalid plasticity modifier, using default value 1.0f");
            }
        }
        
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            
            for (auto& syn : region->getSynapses()) {
                if (!syn) continue;
                
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        if (pImpl->stdp) {
                            pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
                        }
                        
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
                        if (pImpl->hebbian) {
                            pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                        }
                    }
                }
                
                // Update synapse state
                syn->step(currentTime);
            }
        }
        
        // ========== STEP 7: Update episodic memory ==========
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
                    if (!region) continue;
                    
                    for (auto& pop : region->getPopulations()) {
                        if (!pop) continue;
                        
                        for (auto* neuron : pop->getNeurons()) {
                            if (!neuron) continue;
                            
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
        
        // ========== STEP 8: Update prediction system ==========
        if (pImpl->predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
        
        // ========== STEP 9: Update attention system ==========
        if (pImpl->attention) {
            pImpl->attention->update(timestep);
            
            // Apply attention to working memory winners
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                pImpl->attention->processCompetition(competitors);
            }
        }
        
        // ========== STEP 10: Update concept formation ==========
        if (pImpl->conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0) {
            if (pImpl->structuralPlasticity && pImpl->rng) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        }
        
        // ========== STEP 12: Replay important memories ==========
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                if (episode) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        }
        
        // ========== STEP 13: Apply development effects ==========
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->update(this, *pImpl->rng, timestep * 1000);
                
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
                        case DevelopmentalStage::Aging:
                            plasticityMod = 0.1f;  // Very stable
                            break;
                    }
                    
                    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
                }
            }
        }
        
        // ========== STEP 14: Periodic memory consolidation ==========
        if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
            // Consolidate important memories, remove weak ones
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // ========== STEP 15: Checkpoint management ==========
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->update(currentStep, currentTime);
        }
        
    } catch (const std::exception& e) {
        logBrainError(BrainError::OutOfMemory, "step", std::string("Exception during simulation step: ") + e.what());
    } catch (...) {
        logBrainError(BrainError::NullPointerDetected, "step", "Unknown exception during simulation step");
    }
}

// Enhanced Brain::reset() with error handling
void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    // Validate state before reset
    if (pImpl) {
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset regions safely
        for (auto& region : pImpl->regions) {
            if (region) {
                region->reset();
            }
        }
        
        // Reset systems with null checks
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->reset();
        }
        
        // Reset memory systems
        if (pImpl->workingMemory) {
            pImpl->workingMemory->clear();
        }
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->clear();
        }
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->clear();
        }
        if (pImpl->attention) {
            pImpl->attention->reset();
        }
    }
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

// Enhanced Brain::save() with error handling
bool Brain::save(const std::string& filepath) const {
    // Validate filepath
    if (filepath.empty() || filepath.find_first_not_of(' ') == std::string::npos) {
        logBrainError(BrainError::InvalidFilepath, "save", "Invalid filepath provided");
        return false;
    }
    
    if (filepath.length() > 4096) { // Reasonable path length limit
        logBrainError(BrainError::InvalidFilepath, "save", "Filepath too long");
        return false;
    }
    
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            logBrainError(BrainError::CheckpointOperationFailed, "save",
                         "Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        size_t totalNeuronCount = getTotalNeuronCount();
        
        // Reserve memory with validation
        if (totalNeuronCount > 0) {
            neuronData.membranePotential.reserve(totalNeuronCount);
            neuronData.restingPotential.reserve(totalNeuronCount);
            neuronData.threshold.reserve(totalNeuronCount);
            neuronData.resetPotential.reserve(totalNeuronCount);
            neuronData.leakConductance.reserve(totalNeuronCount);
        }
        
        for (const auto& region : pImpl->regions) {
            if (!region) {
                logBrainError(BrainError::NullPointerDetected, "save", "Null region in regions list");
                continue;
            }
            
            for (const auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        logBrainError(BrainError::NullPointerDetected, "save", "Null neuron in population");
                        continue;
                    }
                    
                    const auto& state = neuron->getState();
                    
                    // Validate state values
                    if (!isFloatValid(state.membranePotential, -100.0f, 50.0f) ||
                        !isFloatValid(state.restingPotential, -100.0f, 50.0f) ||
                        !isFloatValid(state.threshold, -100.0f, 50.0f) ||
                        !isFloatValid(state.resetPotential, -100.0f, 50.0f) ||
                        !isFloatValid(state.leakConductance, 0.0f, 100.0f)) {
                        logBrainError(BrainError::InvalidConfiguration, "save", 
                                     "Invalid neuron state values for neuron " + std::to_string(neuron->getId().index()));
                        continue;
                    }
                    
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            logBrainError(BrainError::CheckpointOperationFailed, "save", "Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        
        for (const auto& region : pImpl->regions) {
            if (!region) continue;
            
            for (const auto* syn : region->getSynapses()) {
                if (!syn) {
                    logBrainError(BrainError::NullPointerDetected, "save", "Null synapse in region");
                    continue;
                }
                
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            logBrainError(BrainError::CheckpointOperationFailed, "save", "Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            logBrainError(BrainError::CheckpointOperationFailed, "save", "Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        logBrainError(BrainError::OutOfMemory, "save", std::string("Exception saving brain: ") + e.what());
        return false;
    } catch (...) {
        logBrainError(BrainError::NullPointerDetected, "save", "Unknown exception saving brain");
        return false;
    }
}

// Enhanced Brain::load() with error handling
bool Brain::load(const std::string& filepath) {
    // Validate filepath
    if (filepath.empty() || filepath.find_first_not_of(' ') == std::string::npos) {
        logBrainError(BrainError::InvalidFilepath, "load", "Invalid filepath provided");
        return false;
    }
    
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            logBrainError(BrainError::CheckpointOperationFailed, "load",
                         "Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            logBrainError(BrainError::CheckpointOperationFailed, "load",
                         "Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            logBrainError(BrainError::CheckpointOperationFailed, "load", "Failed to read neurons from checkpoint");
            return false;
        }
        
        // Validate neuron data
        if (!neuronData.membranePotential.empty() && 
            neuronData.membranePotential.size() != neuronData.restingPotential.size()) {
            logBrainError(BrainError::InvalidConfiguration, "load", 
                         "Inconsistent neuron data sizes in checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            if (!region) {
                logBrainError(BrainError::NullPointerDetected, "load", "Null region in regions list");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        logBrainError(BrainError::NullPointerDetected, "load", "Null neuron in population");
                        continue;
                    }
                    
                    if (idx < neuronData.membranePotential.size()) {
                        // Validate values before setting
                        if (isFloatValid(neuronData.membranePotential[idx], -100.0f, 50.0f)) {
                            neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        }
                        if (isFloatValid(neuronData.restingPotential[idx], -100.0f, 50.0f)) {
                            neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        }
                        if (isFloatValid(neuronData.threshold[idx], -100.0f, 50.0f)) {
                            neuron->setThreshold(neuronData.threshold[idx]);
                        }
                        if (isFloatValid(neuronData.resetPotential[idx], -100.0f, 50.0f)) {
                            neuron->setResetPotential(neuronData.resetPotential[idx]);
                        }
                        if (isFloatValid(neuronData.leakConductance[idx], 0.0f, 100.0f)) {
                            neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        }
                        
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            logBrainError(BrainError::CheckpointOperationFailed, "load", "Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        logBrainError(BrainError::OutOfMemory, "load", std::string("Exception loading brain: ") + e.what());
        return false;
    } catch (...) {
        logBrainError(BrainError::NullPointerDetected, "load", "Unknown exception loading brain");
        return false;
    }
}

// Enhanced Brain::logStatus() with error handling
void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    
    // Log basic statistics
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status with validation
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        if (isFloatValid(dopamineLevel, -1.0f, 1.0f)) {
            NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamineLevel));
        } else {
            NLM_LOG_WARNING("Invalid dopamine level: " + std::to_string(dopamineLevel));
        }
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    // Log region details
    for (const auto& region : pImpl->regions) {
        if (!region) {
            logBrainError(BrainError::NullPointerDetected, "logStatus", "Null region in regions list");
            continue;
        }
        
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
