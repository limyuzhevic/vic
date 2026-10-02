    // ========== STEP 14: Memory replay and consolidation ==========
    // During rest phases (low activity), memories are replayed and consolidated
    bool shouldEnterRest = false;
    
    // Check if we should enter rest based on low neural activity
    size_t totalActiveNeurons = getActiveNeuronCount();
    size_t firingNeurons = getFiringNeuronCount();
    
    // Enter rest if activity is low and we haven't been resting for long
    if (!pImpl->isResting && totalActiveNeurons < getTotalNeuronCount() * 0.1f) {
        // Also check neuromodulation levels (curiosity and novelty should be low during rest)
        if ((pImpl->curiosity && pImpl->curiosity->getCuriosityLevel() < 0.2f) ||
            (pImpl->novelty && pImpl->novelty->getNoveltyLevel() < 0.1f)) {
            
            shouldEnterRest = true;
        }
    }
    
    if (shouldEnterRest) {
        pImpl->isResting = true;
        NLM_LOG_INFO("Brain entering rest phase for memory consolidation and replay");
        
        // During rest, reduce neural excitability and increase memory processes
        if (pImpl->dopamine) {
            // Reduce dopamine during rest to decrease excitability
            pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() * 0.1f);
        }
        
        if (pImpl->curiosity) {
            // Reset curiosity during rest
            pImpl->curiosity->reset();
        }
        
        if (pImpl->novelty) {
            // Reduce novelty detection during rest
            pImpl->novelty->update(0.0f);
        }
        
        if (pImpl->predictionError) {
            // Reset prediction error during rest
            pImpl->predictionError->reset();
        }
    }
    
    // ========== STEP 15: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 16: Memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
        
        // Also consolidate working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->setDecayRate(0.005f);
        }
    }
    
    // ========== STEP 17: Sleep/rest state management ==========
    // During rest, periodically wake up if activity increases
    if (pImpl->isResting) {
        // Check if we should wake up (activity increased)
        if (totalActiveNeurons > getTotalNeuronCount() * 0.3f ||
            firingNeurons > 0) {
            
            // Wake up from rest
            pImpl->isResting = false;
            NLM_LOG_INFO("Brain waking up from rest phase");
            
            // Reset neuromodulation levels
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.5f);
            }
            
            if (pImpl->curiosity) {
                pImpl->curiosity->reset();
            }
            
            if (pImpl->novelty) {
                pImpl->novelty->update(0.5f);
            }
        }
    }
    
    // ========== STEP 18: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
    
    // ========== STEP 19: Rest phase summary ==========
    if (pImpl->isResting) {
        NLM_LOG_INFO("Rest phase: " + std::to_string(pImpl->stepsSinceLastEpisode) + " steps since last episodic memory update");
        if (pImpl->workingMemory) {
            NLM_LOG_INFO("Rest phase: Working memory contains " + 
                         std::to_string(pImpl->workingMemory->getMemoryNeurons().size()) + " traces");
        }
        if (pImpl->episodicMemory) {
            NLM_LOG_INFO("Rest phase: Episodic memory contains " + 
                         std::to_string(pImpl->episodicMemory->getEpisodeCount()) + " episodes");
        }
    }
