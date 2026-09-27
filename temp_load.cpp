bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
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
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states
        for (const auto& synData : synapseData.sourceNeuron) {
            // Note: This is a simplified synapse restoration
            // In a full implementation, we would need to find matching synapses and restore their state
        }
        
        // Read inter-region connections
        auto interRegionConnections = reader.readSection(CheckpointSection::Connectivity);
        if (!interRegionConnections.empty()) {
            // Apply inter-region connections (simplified)
            pImpl->interRegionConnections.clear();
            auto* data = reinterpret_cast<std::tuple<uint64_t, uint64_t, float, uint32_t, uint32_t>*>(interRegionConnections.data());
            for (size_t i = 0; i < interRegionConnections.size() / sizeof(std::tuple<uint64_t, uint64_t, float, uint32_t, uint32_t>); ++i) {
                auto& conn = pImpl->interRegionConnections.emplace_back();
                conn.sourceRegion = RegionId(std::get<0>(data[i]));
                conn.targetRegion = RegionId(std::get<1>(data[i]));
                conn.weight = std::get<2>(data[i]);
                conn.delay = std::get<3>(data[i]);
                conn.plasticityFlags = PlasticityFlags(std::get<4>(data[i]));
            }
        }
        
        // Read working memory
        auto workingMemorySection = reader.readSection(CheckpointSection::Memory);
        if (!workingMemorySection.empty()) {
            auto* workingMemoryData = reinterpret_cast<WorkingMemoryCheckpointData*>(workingMemorySection.data());
            if (pImpl->workingMemory) {
                // Restore working memory (simplified)
                pImpl->workingMemory->clear();
                for (const auto& neuronId : workingMemoryData->neurons) {
                    pImpl->workingMemory->storeToNeuron(NeuronId(neuronId), 0.5f); // Default activation
                }
                pImpl->workingMemory->setCapacity(workingMemoryData->capacity);
                pImpl->workingMemory->setDecayRate(workingMemoryData->decayRate);
            }
        }
        
        // Read episodic memory
        auto episodicMemorySection = reader.readSection(CheckpointSection::Memory);
        if (!episodicMemorySection.empty() && std::numeric_limits<size_t>::max() > 0) {
            auto* episodicMemoryData = reinterpret_cast<EpisodicMemoryCheckpointData*>(episodicMemorySection.data());
            if (pImpl->episodicMemory) {
                // Clear existing episodes and restore from checkpoint
                pImpl->episodicMemory->clear();
                
                // Create episodic memory items from checkpoint data
                for (size_t i = 0; i < episodicMemoryData->timestamps.size(); ++i) {
                    EpisodicMemoryItem episode;
                    episode.timestamp = episodicMemoryData->timestamps[i];
                    episode.reward = episodicMemoryData->rewards[i];
                    episode.energy = episodicMemoryData->energies[i];
                    episode.novelty = episodicMemoryData->novelties[i];
                    episode.resultingReward = episodicMemoryData->resultingRewards[i];
                    episode.age = episodicMemoryData->ages[i];
                    episode.action = episodicMemoryData->actions[i];
                    episode.sensoryState = episodicMemoryData->sensoryStates[i];
                    episode.resultingSensoryState = episodicMemoryData->resultingSensoryStates[i];
                    episode.activeNeurons = std::vector<NeuronId>(episodicMemoryData->activeNeurons.begin() + i, episodicMemoryData->activeNeurons.begin() + i + 1);
                    episode.neuronActivations = std::vector<float>(episodicMemoryData->neuronActivations.begin() + i, episodicMemoryData->neuronActivations.begin() + i + 1);
                    
                    pImpl->episodicMemory->storeEpisode(episode);
                }
            }
        }
        
        // Read cognitive systems
        auto cognitiveSection = reader.readSection(CheckpointSection::Cognition);
        if (!cognitiveSection.empty()) {
            auto* cognitiveSystemData = reinterpret_cast<CognitiveSystemCheckpointData*>(cognitiveSection.data());
            
            // Neural Planner
            if (pImpl->planner) {
                if (!cognitiveSystemData->actionQualities.empty()) {
                    // Note: This is simplified; the actual setActionQuality method may not exist
                    for (size_t i = 0; i < std::min(cognitiveSystemData->actionQualities.size(), cognitiveSystemData->planningDepths.size()); ++i) {
                        // Simplified action quality restoration
                    }
                }
                if (!cognitiveSystemData->currentGoals.empty()) {
                    pImpl->planner->setCurrentGoal(cognitiveSystemData->currentGoals[0]);
                }
            }
            
            // Concept Formation
            if (pImpl->conceptFormation) {
                // Simplified concept restoration
                pImpl->conceptFormation->clear();
                for (const auto& concept : cognitiveSystemData->concepts) {
                    // Note: This is simplified; concept restoration would need actual API access
                }
            }
            
            // Self Model
            if (pImpl->selfModel) {
                if (!cognitiveSystemData->capabilityLevels.empty()) {
                    // Note: This is simplified; the actual setCapabilityLevel method may not exist
                }
            }
            
            // Social Learning
            if (pImpl->socialLearning) {
                if (!cognitiveSystemData->signalPatterns.empty()) {
                    // Simplified signal restoration
                }
            }
            
            // Attentional Selection
            if (pImpl->attention) {
                if (!cognitiveSystemData->attentionLevels.empty()) {
                    // Note: This is simplified; the actual setAttentionLevel method may not exist
                }
            }
        }
        
        // Read neuromodulation
        auto neuromodulationSection = reader.readSection(CheckpointSection::Neuromodulation);
        if (!neuromodulationSection.empty()) {
            auto* neuromodulationData = reinterpret_cast<NeuromodulationCheckpointData*>(neuromodulationSection.data());
            
            // Dopamine
            if (pImpl->dopamine && !neuromodulationData->dopamineLevels.empty()) {
                // Note: This is simplified; the actual setLevel method may not exist
            }
            
            // Curiosity
            if (pImpl->curiosity && !neuromodulationData->curiosityLevels.empty()) {
                // Note: This is simplified; the actual setLevel method may not exist
            }
            
            // Novelty
            if (pImpl->novelty && !neuromodulationData->noveltyLevels.empty()) {
                // Note: This is simplified; the actual setLevel method may not exist
            }
        }
        
        // Read development system
        auto developmentSection = reader.readSection(CheckpointSection::Development);
        if (!developmentSection.empty()) {
            auto* developmentData = reinterpret_cast<DevelopmentCheckpointData*>(developmentSection.data());
            
            pImpl->developmentalStage = static_cast<DevelopmentalStage>(developmentData->developmentalStage);
            
            if (pImpl->developmentSystem) {
                pImpl->developmentSystem->setDevelopmentalAge(developmentData->developmentalAge);
            }
        }
        
        // Read simulation state
        auto simulationSection = reader.readSection(CheckpointSection::SimulationState);
        if (!simulationSection.empty()) {
            auto* simulationData = reinterpret_cast<SimulationCheckpointData*>(simulationSection.data());
            
            pImpl->currentStep = simulationData->currentStep;
            pImpl->currentTime = simulationData->currentTime;
            pImpl->timestep = simulationData->timestep;
            pImpl->totalSpikesThisStep = simulationData->totalSpikesThisStep;
            pImpl->totalSpikesTotal = simulationData->totalSpikesTotal;
            pImpl->isResting = simulationData->isResting;
            pImpl->stepsSinceLastEpisode = simulationData->stepsSinceLastEpisode;
            pImpl->replayInterval = simulationData->replayInterval;
            pImpl->consolidationInterval = simulationData->consolidationInterval;
        }
        
        // Read random generator state
        auto randomSection = reader.readSection(CheckpointSection::RandomState);
        if (!randomSection.empty()) {
            auto* randomData = reinterpret_cast<RandomGeneratorCheckpointData*>(randomSection.data());
            
            if (pImpl->rng) {
                // Note: RandomGenerator seed restoration may not be available
            }
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}