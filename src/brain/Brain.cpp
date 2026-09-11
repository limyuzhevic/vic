// Connect working memory to attention - ACh enhances competition
    if (pImpl->attention && pImpl->workingMemory) {
        std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
        pImpl->attention->processCompetition(competitors);
        
        // Apply ACh effects on attention based on working memory state
        if (pImpl->acetylcholine && !competitors.empty()) {
            // ACh enhances attention to strong working memory traces
            float totalActivation = 0.0f;
            for (NeuronId neuronId : competitors) {
                // Find activation level from working memory
                float activation = pImpl->workingMemory->getMemoryActivation(neuronId);
                totalActivation += activation;
            }
            
            if (totalActivation > 0.0f) {
                // Boost attention based on working memory strength
                pImpl->acetylcholine->enhanceAttention(totalActivation);
            }
        }
    }
    
    // Update concept formation with NE and ACh
    if (pImpl->conceptFormation) {
        // NE increases exploration for new concept formation
        if (pImpl->norepinephrine) {
            pImpl->conceptFormation->setExplorationDrive(pImpl->norepinephrine->getLevel() * 0.5f);
        }
        
        // ACh enhances consolidation of formed concepts
        if (pImpl->acetylcholine) {
            pImpl->conceptFormation->setConsolidationStrength(pImpl->acetylcholine->getLevel());
        }
        
        pImpl->conceptFormation->update(pImpl->timestep);
    }
    
    // Apply serotonin effects on planning
    if (pImpl->serotonin && pImpl->planner) {
        // High serotonin reduces impulsivity in planning
        float impulsivity = pImpl->serotonin->getImpulsivity();
        pImpl->planner->setImpulsivityControl(1.0f - impulsivity);
    }
    
    // Neural planner integration with curiosity
    if (pImpl->planner && pImpl->curiosity) {
        // Curiosity drives exploration in planning
        float curiosityLevel = pImpl->curiosity->getLevel();
        pImpl->planner->setExplorationBias(curiosityLevel);
        
        // Use planner for action selection
        std::vector<float> currentState;
        if (getTotalNeuronCount() > 0) {
            // Simple state representation from neuron activities
            currentState.reserve(10);
            size_t neuronsToSample = std::min(static_cast<size_t>(10), getTotalNeuronCount());
            for (size_t i = 0; i < neuronsToSample; ++i) {
                currentState.push_back(static_cast<float>(i) / neuronsToSample);
            }
        }
        
        ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
        
        // Apply ACh to enhance planned action execution
        if (pImpl->acetylcholine && plannedAction != ActionType::Wait) {
            pImpl->acetylcholine->enhanceFocus(0.8f);
        }
        
        // Apply dopamine reward signal for successful planning
        if (plannedAction != ActionType::Wait) {
            float rewardValue = 0.1f; // Base reward for planning and execution
            if (pImpl->dopamine) {
                pImpl->dopamine->signalReward(rewardValue);
            }
        }
    }
    
    // Connect episodic memory to learning
    if (pImpl->episodicMemory) {
        // Store sensory input in episodic memory
        if (currentStep % 5 == 0) { // Store every 5 steps
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            
            // Capture current neural state
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
            
            // Apply neuromodulation effects
            if (pImpl->dopamine) {
                episode.reward = pImpl->dopamine->getLevel();
            }
            if (pImpl->acetylcholine) {
                episode.memoryStrength = pImpl->acetylcholine->getLevel();
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // Apply curiosity-driven exploration
    if (pImpl->curiosity && pImpl->novelty) {
        float noveltyLevel = pImpl->novelty->getLevel();
        float predictionError = 0.0f; // Would come from prediction system
        
        pImpl->curiosity->update(noveltyLevel, predictionError, pImpl->timestep);
        
        // Apply curiosity effects on neural excitability
        float curiosityLevel = pImpl->curiosity->getLevel();
        if (curiosityLevel > 0.3f) {
            // Boost exploration through neuromodulation
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->increaseArousal(curiosityLevel * 0.5f);
            }
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->enhanceFocus(curiosityLevel * 0.3f);
            }
        }
    }
    
    // Apply serotonin effects on action selection
    if (pImpl->serotonin && pImpl->motorNeurons.size() > 0) {
        float impulsivity = pImpl->serotonin->getImpulsivity();
        float socialFactor = pImpl->serotonin->getSocialBehavior();
        
        // Modulate motor neuron activity based on serotonin
        for (auto* neuron : pImpl->motorNeurons) {
            // Higher impulsivity leads to more random actions
            if (impulsivity < 0.5f) {
                // Reduce threshold for more exploratory actions
                neuron->setThreshold(neuron->getState().threshold * 0.95f);
            }
            
            // Social behavior affects action selection
            if (socialFactor > 1.2f) {
                // More social behaviors
                neuron->injectCurrent(0.2f * socialFactor);
            }
        }
    }
    
    // Connect prediction system to dopamine
    if (pImpl->dopamine && pImpl->predictionError) {
        // Prediction error drives dopamine learning
        float predictionErrorLevel = pImpl->predictionError->getMagnitude();
        
        if (predictionErrorLevel > 0.1f) {
            // Strong prediction error -> dopamine burst for learning
            pImpl->dopamine->signalRewardPredictionError(predictionErrorLevel * 2.0f);
        }
    }
    
    // Apply development effects on plasticity
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Periodically consolidate episodic memories
        pImpl->episodicMemory->consolidate(0.3f);
        
        // Development affects neuromodulator baseline levels
        if (pImpl->acetylcholine) {
            // Development increases ACh capacity
            float currentLevel = pImpl->acetylcholine->getLevel();
            pImpl->acetylcholine->setLevel(std::min(1.0f, currentLevel + 0.05f));
        }
    }