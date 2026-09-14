    void resetDevelopmentalStage() {
        // Reset developmental stage to Initial
        developmentSystem->setStage(DevelopmentalStage::Initial);
        developmentalAge = 0.0;
    }
    
    void resetNeuralSubstrate() {
        // Reset all neural populations
        for (auto& region : regions) {
            for (auto& population : region->getPopulations()) {
                population->reset();
                population->initializeRandom(*rng);
            }
            region->reset();
        }
        
        // Reset all synapses
        for (auto& synapse : synapses) {
            synapse->reset();
        }
        
        // Reset spike system
        spikeSystem->reset();
    }
    
    void resetPlasticities() {
        // Reset all plasticity systems
        stdp->reset();
        hebbian->reset();
        structuralPlasticity->reset();
    }
    
    void resetNeuromodulators() {
        // Reset neuromodulators to baseline
        neuromodulator->reset();
        curiosity->reset();
        predictionError->reset();
    }
    
    void resetMemorySystems() {
        // Reset all memory systems
        workingMemory->reset();
        episodicMemory->reset();
        semanticMemory->reset();
        proceduralMemory->reset();
        associativeMemory->reset();
    }
    
    void resetPredictionSystem() {
        // Reset prediction system
        predictionSystem->reset();
        predictionError->reset();
    }
    
    void updateCognitionSystems() {
        // Reset cognitive systems
        neuralPlanner->reset();
        conceptFormation->reset();
    }

    void resetPerformance() {
        // Reset performance systems
        checkpointSystem->reset();
        stepsSinceLastPlasticityCheck = 0;
    }
    
    // Brain memory replay methods (for episodic memory integration)
    void replayMemory(const std::vector<size_t>& episodeIds, ReplayMode mode);
    void replayForConsolidation(const std::vector<size_t>& episodeIds);
    void replayDuringSleep(float startTime, float duration, SleepPhase phase);
    void injectCurrentToNeurons(NeuronType neuronType, float current);
    void strengthenConnections(NeuronId source, const std::vector<float>& pattern, float delta);
    void activatePattern(const std::vector<float>& pattern, float strength);
    void updateWorkingMemoryIntegration();
    std::vector<const EpisodicMemoryItem*> getRecentEpisodesForReplay(size_t count) const;

    void cleanupAllSystems() {
        // Cleanup systems to free memory
        workingMemory->clear();
        episodicMemory->clear();
        semanticMemory->clear();
        proceduralMemory->clear();
        associativeMemory->clear();
        
        // Clear spike history
        spikeSystem->reset();
    }
}; // struct Brain::Impl

Brain::Brain() : pImpl(new Impl) {
}

Brain::~Brain() {
    // Cleanup will happen automatically with unique_ptr
}

void Brain::initialize(Config config) {
    pImpl->config = config;
    
    // Reinitialize with provided config
    reset();
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& population : region->getPopulations()) {
            population->step(currentTime);
        }
    }
    
    // 3. Detect spikes and schedule spike events
    pImpl->spikeSystem->processSpikes(currentStep, currentTime);
    
    // 4. Apply plasticity rules (STDP and Hebbian)
    for (auto& region : pImpl->regions) {
        for (auto& synapse : region->getSynapses()) {
            pImpl->stdp->applySTDP(*synapse, currentStep, currentTime);
            pImpl->hebbian->applyHebbian(*synapse, currentStep, currentTime);
        }
    }
    
    // 5. Apply structural plasticity periodically (every 100 steps)
    pImpl->stepsSinceLastPlasticityCheck++;
    if (pImpl->stepsSinceLastPlasticityCheck >= 100) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
        pImpl->stepsSinceLastPlasticityCheck = 0;
    }
    
    // 6. Update integrated systems
    pImpl->developmentSystem->update(this, currentStep);
    pImpl->neuromodulator->update(*pImpl->rng, currentTime);
    
    // Update curiosity with current levels
    float novelty = 0.0f; // Would come from sensory input
    float predictionError = pImpl->predictionError->getError();
    pImpl->curiosity->update(novelty, predictionError, currentTime);
    
    // 7. Update memory systems
    pImpl->workingMemory->update(currentStep, currentTime);
    pImpl->episodicMemory->update(currentStep, currentTime);
    
    // 8. Update prediction system
    pImpl->predictionSystem->update(currentStep, currentTime);
    
    // 9. Update cognitive systems
    pImpl->neuralPlanner->update(currentStep, currentTime);
    pImpl->conceptFormation->update(currentStep, currentTime);
    
    // 10. Update developmental age
    pImpl->developmentalAge += 0.001; // Approximate timestep
}

void Brain::reset() {
    // Store reset start time for logging
    Timestamp resetStartTime = pImpl->currentTime;
    
    NLM_LOG_INFO("Starting brain reset - creating fresh starting state");
    
    // 1. Reset developmental stage to Initial
    resetDevelopmentalStage();
    
    // 2. Reset neural substrate (neurons and synapses)
    resetNeuralSubstrate();
    
    // 3. Reset plasticities (STDP, Hebbian, structural)
    resetPlasticities();
    
    // 4. Reset neuromodulators
    resetNeuromodulators();
    
    // 5. Reset memory systems
    resetMemorySystems();
    
    // 6. Reset prediction system
    resetPredictionSystem();
    
    // 7. Reset cognitive systems
    resetCognitionSystems();
    
    // 8. Reset performance systems
    resetPerformance();
    
    // 9. Reinitialize all systems
    pImpl->initializeAllSystems();
    
    // 10. Reset state tracking
    pImpl->currentStep = 0;
    pImpl->currentTime = resetStartTime;
    pImpl->stepsSinceLastPlasticityCheck = 0;
    pImpl->developmentalAge = 0.0;
    
    NLM_LOG_INFO("Brain reset complete - fresh starting state established");
}