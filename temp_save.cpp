bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
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
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        neuronData.firingState.reserve(getTotalNeuronCount());
        neuronData.refractoryRemaining.reserve(getTotalNeuronCount());
        neuronData.refractoryPeriod.reserve(getTotalNeuronCount());
        neuronData.lastSpikeTime.reserve(getTotalNeuronCount());
        neuronData.neuronType.reserve(getTotalNeuronCount());
        neuronData.regionId.reserve(getTotalNeuronCount());
        neuronData.populationId.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                    neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                    neuronData.regionId.push_back(static_cast<uint64_t>(region->getId().index()));
                    neuronData.populationId.push_back(static_cast<uint64_t>(pop->getId()));
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.plasticityFlags.push_back(static_cast<uint8_t>(syn->getPlasticityFlags().value()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        // Write inter-region connections
        std::vector<std::tuple<uint64_t, uint64_t, float, uint32_t, uint32_t>> interRegionConnections;
        for (const auto& conn : pImpl->interRegionConnections) {
            interRegionConnections.push_back(std::make_tuple(
                conn.sourceRegion.index(),
                conn.targetRegion.index(),
                conn.weight,
                conn.delay,
                conn.plasticityFlags.value()
            ));
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        if (!writer.writeSection(CheckpointSection::Connectivity,
                               interRegionConnections.data(),
                               interRegionConnections.size() * sizeof(std::tuple<uint64_t, uint64_t, float, uint32_t, uint32_t>))) {
            NLM_LOG_ERROR("Failed to write inter-region connections to checkpoint");
            return false;
        }
        
        // Write working memory
        if (pImpl->workingMemory) {
            WorkingMemoryCheckpointData workingMemoryData;
            auto neurons = pImpl->workingMemory->getMemoryNeurons();
            auto activations = std::vector<float>(neurons.size());
            for (size_t i = 0; i < neurons.size(); ++i) {
                workingMemoryData.neurons.push_back(neurons[i].index());
                activations[i] = pImpl->workingMemory->getNeuronActivation(neurons[i]);
            }
            workingMemoryData.activations = activations;
            workingMemoryData.capacity = pImpl->workingMemory->getCapacity();
            workingMemoryData.decayRate = pImpl->workingMemory->getDecayRate();
            
            if (!writer.writeSection(CheckpointSection::Memory, &workingMemoryData, sizeof(WorkingMemoryCheckpointData))) {
                NLM_LOG_ERROR("Failed to write working memory to checkpoint");
                return false;
            }
        }
        
        // Write episodic memory
        if (pImpl->episodicMemory) {
            EpisodicMemoryCheckpointData episodicMemoryData;
            size_t episodeCount = pImpl->episodicMemory->getEpisodeCount();
            episodicMemoryData.timestamps.reserve(episodeCount);
            episodicMemoryData.rewards.reserve(episodeCount);
            episodicMemoryData.energies.reserve(episodeCount);
            episodicMemoryData.novelties.reserve(episodeCount);
            episodicMemoryData.resultingRewards.reserve(episodeCount);
            episodicMemoryData.activeNeurons.reserve(episodeCount * 10); // Estimate
            episodicMemoryData.neuronActivations.reserve(episodeCount * 10);
            episodicMemoryData.ages.reserve(episodeCount);
            episodicMemoryData.actions.reserve(episodeCount);
            episodicMemoryData.sensoryStates.reserve(episodeCount);
            episodicMemoryData.resultingSensoryStates.reserve(episodeCount);
            
            for (size_t i = 0; i < episodeCount; ++i) {
                const auto* episode = pImpl->episodicMemory->getEpisode(i);
                episodicMemoryData.timestamps.push_back(episode->timestamp);
                episodicMemoryData.rewards.push_back(episode->reward);
                episodicMemoryData.energies.push_back(episode->energy);
                episodicMemoryData.novelties.push_back(episode->novelty);
                episodicMemoryData.resultingRewards.push_back(episode->resultingReward);
                episodicMemoryData.activeNeurons.insert(episodicMemoryData.activeNeurons.end(),
                                                         episode->activeNeurons.begin(), episode->activeNeurons.end());
                episodicMemoryData.neuronActivations.insert(episodicMemoryData.neuronActivations.end(),
                                                           episode->neuronActivations.begin(), episode->neuronActivations.end());
                episodicMemoryData.ages.push_back(episode->age);
                episodicMemoryData.actions.push_back(episode->action);
                episodicMemoryData.sensoryStates.push_back(episode->sensoryState);
                episodicMemoryData.resultingSensoryStates.push_back(episode->resultingSensoryState);
            }
            
            if (!writer.writeSection(CheckpointSection::Memory, &episodicMemoryData, sizeof(EpisodicMemoryCheckpointData))) {
                NLM_LOG_ERROR("Failed to write episodic memory to checkpoint");
                return false;
            }
        }
        
        // Write associative memory
        if (pImpl->associativeMemory) {
            AssociativeMemoryCheckpointData associativeMemoryData;
            size_t associationCount = pImpl->associativeMemory->getAssociationCount();
            associativeMemoryData.patternNeurons.reserve(associationCount * 2);
            associativeMemoryData.patterns.reserve(associationCount * 2);
            associativeMemoryData.associations.reserve(associationCount);
            associativeMemoryData.strengths.reserve(associationCount);
            
            // Note: This is a simplified representation since we don't have direct access to internal data
            // In a full implementation, we would need to expose associative memory data structures
        }
        
        // Write prediction system
        if (pImpl->predictionSystem) {
            PredictionSystemCheckpointData predictionSystemData;
            predictionSystemData.predictionErrors = pImpl->predictionSystem->getErrorHistory();
            predictionSystemData.confidences = std::vector<float>(1, pImpl->predictionSystem->getConfidence());
            
            if (!writer.writeSection(CheckpointSection::Prediction, &predictionSystemData, sizeof(PredictionSystemCheckpointData))) {
                NLM_LOG_ERROR("Failed to write prediction system to checkpoint");
                return false;
            }
        }
        
        // Write cognitive systems
        CognitiveSystemCheckpointData cognitiveSystemData;
        
        // Neural Planner
        if (pImpl->planner) {
            cognitiveSystemData.actionQualities = pImpl->planner->getActionQualities();
            cognitiveSystemData.planningDepths = std::vector<float>(1, pImpl->planner->getPlanningDepth());
            cognitiveSystemData.planningConfidences = std::vector<float>(1, pImpl->planner->getPlanningConfidence());
            cognitiveSystemData.currentGoals = pImpl->planner->getCurrentGoal();
            cognitiveSystemData.recentPlanSuccesses = std::vector<std::deque<bool>>(1, std::deque<bool>()); // Simplified
        }
        
        // Concept Formation
        if (pImpl->conceptFormation) {
            cognitiveSystemData.concepts = pImpl->conceptFormation->getConcepts();
            cognitiveSystemData.nextConceptIds = std::vector<size_t>(1, pImpl->conceptFormation->getConceptCount() + 1);
        }
        
        // Self Model
        if (pImpl->selfModel) {
            cognitiveSystemData.capabilityLevels = std::vector<float>(1, pImpl->selfModel->getCapabilityLevel());
            // Simplified action effects
        }
        
        // Social Learning
        if (pImpl->socialLearning) {
            cognitiveSystemData.observationCounts = std::vector<size_t>(1, pImpl->socialLearning->getObservationCount());
            cognitiveSystemData.signalPatterns = std::vector<std::vector<float>>(1, pImpl->socialLearning->getSignalPattern());
            cognitiveSystemData.signalMeanings = std::vector<float>(1, pImpl->socialLearning->getSignalMeaning());
        }
        
        // Attentional Selection
        if (pImpl->attention) {
            cognitiveSystemData.attentionLevels = std::vector<float>(1, pImpl->attention->getAttentionLevel());
            cognitiveSystemData.attentionThresholds = std::vector<float>(1, 0.5f); // Default value
            cognitiveSystemData.inhibitionStrengths = std::vector<float>(1, 0.5f); // Default value
            cognitiveSystemData.excitationStrengths = std::vector<float>(1, 1.5f); // Default value
            cognitiveSystemData.winners = std::vector<std::vector<NeuronId>>(1, std::vector<NeuronId>()); // Simplified
            cognitiveSystemData.neuronSalience = std::vector<float>(1, 0.5f); // Default value
        }
        
        if (!writer.writeSection(CheckpointSection::Cognition, &cognitiveSystemData, sizeof(CognitiveSystemCheckpointData))) {
            NLM_LOG_ERROR("Failed to write cognitive systems to checkpoint");
            return false;
        }
        
        // Write neuromodulation
        NeuromodulationCheckpointData neuromodulationData;
        
        if (pImpl->dopamine) {
            neuromodulationData.dopamineLevels = std::vector<float>(1, pImpl->dopamine->getLevel());
        }
        if (pImpl->curiosity) {
            neuromodulationData.curiosityLevels = std::vector<float>(1, pImpl->curiosity->getLevel());
        }
        if (pImpl->novelty) {
            neuromodulationData.noveltyLevels = std::vector<float>(1, pImpl->novelty->getLevel());
        }
        neuromodulationData.predictionErrorMagnitudes = std::vector<float>(1, 0.0f); // TODO: Get actual prediction error
        
        if (!writer.writeSection(CheckpointSection::Neuromodulation, &neuromodulationData, sizeof(NeuromodulationCheckpointData))) {
            NLM_LOG_ERROR("Failed to write neuromodulation to checkpoint");
            return false;
        }
        
        // Write development system
        DevelopmentCheckpointData developmentData;
        developmentData.developmentalAge = pImpl->developmentSystem ? pImpl->developmentSystem->getDevelopmentalAge() : 0.0;
        developmentData.developmentalStage = static_cast<uint32_t>(pImpl->developmentalStage);
        developmentData.plasticityModifier = pImpl->developmentSystem ? pImpl->developmentSystem->getPlasticityModifier() : 1.0f;
        developmentData.isCriticalPeriod = pImpl->developmentSystem ? pImpl->developmentSystem->isCriticalPeriod() : false;
        developmentData.criticalPeriodProgress = pImpl->developmentSystem ? pImpl->developmentSystem->getCriticalPeriodProgress() : 0.0f;
        
        if (!writer.writeSection(CheckpointSection::Development, &developmentData, sizeof(DevelopmentCheckpointData))) {
            NLM_LOG_ERROR("Failed to write development system to checkpoint");
            return false;
        }
        
        // Write simulation state
        SimulationCheckpointData simulationData;
        simulationData.currentStep = pImpl->currentStep;
        simulationData.currentTime = pImpl->currentTime;
        simulationData.timestep = pImpl->timestep;
        simulationData.totalSpikesThisStep = pImpl->totalSpikesThisStep;
        simulationData.totalSpikesTotal = pImpl->totalSpikesTotal;
        simulationData.isResting = pImpl->isResting;
        simulationData.stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
        simulationData.replayInterval = pImpl->replayInterval;
        simulationData.consolidationInterval = pImpl->consolidationInterval;
        
        if (!writer.writeSection(CheckpointSection::SimulationState, &simulationData, sizeof(SimulationCheckpointData))) {
            NLM_LOG_ERROR("Failed to write simulation state to checkpoint");
            return false;
        }
        
        // Write random generator state
        RandomGeneratorCheckpointData randomData;
        randomData.seed = pImpl->rng ? pImpl->rng->getSeed() : 42;
        // Note: RandomGenerator state would need to be exposed for full serialization
        
        if (!writer.writeSection(CheckpointSection::RandomState, &randomData, sizeof(RandomGeneratorCheckpointData))) {
            NLM_LOG_ERROR("Failed to write random generator state to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}