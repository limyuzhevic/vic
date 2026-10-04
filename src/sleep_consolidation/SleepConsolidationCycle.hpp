// ==============================================================================
// SLEEP/CONSOLIDATION CYCLE IMPLEMENTATION
// ==============================================================================

// Sleep/Consolidation Cycle Implementation
// 
// This implements a biologically inspired sleep/rest cycle for the NLM brain:
// 1. Detects when to enter sleep (high novelty, memory load, stress)
// 2. Enters resting state with reduced sensory processing
// 3. Performs memory consolidation (transferring to long-term storage)
// 4. Replays memories for strengthening connections
// 5. Resets neuromodulatory systems for next awake cycle
// 6. Allows development system to progress
// 
// Key mechanisms:
// - Energy conservation during sleep
// - Memory replay and strengthening
// - Synaptic homeostasis
// - Neuromodulatory system reset
// - Development progression
// 
// Sleep is beneficial for:
// - Memory consolidation and integration
// - Synaptic renormalization
// - Processing of recent experiences
// - Preparation for new learning
// - Emotional regulation

void Brain::sleepConsolidationCycle() {
    """
    Implement the sleep/rest consolidation cycle for the NLM brain.
    
    This cycle is essential for:
    1. Memory consolidation - transferring working memory to episodic memory
    2. Synaptic homeostasis - renormalizing synaptic strengths
    3. Memory replay - rehearsing experiences to strengthen connections
    4. Neuromodulatory reset - preparing brain for new learning
    5. Development progression - allowing brain maturation
    
    Sleep is triggered by:
    - High novelty levels (brain wants to process new information)
    - High memory load (brain needs to consolidate)
    - Elevated stress/dopamine (need for regulatory reset)
    - Passage of time (biological sleep-wake cycles)
    
    During sleep:
    - Sensory input is minimized
    - Memory systems work in the background
    - Neuromodulatory systems reset
    - Synaptic strength is normalized
    - Development progresses
    """
    
    // Check if we should enter sleep
    if (!shouldEnterSleep()) {
        return;
    }
    
    // Enter sleep state
    enterSleepState();
    
    // Perform memory consolidation
    consolidateMemoriesDuringSleep();
    
    // Replay and strengthen memories
    replayMemoriesDuringSleep();
    
    // Reset neuromodulatory systems
    resetNeuromodulationDuringSleep();
    
    // Allow development to progress
    developmentDuringSleep();
    
    // Exit sleep state
    exitSleepState();
}

bool Brain::shouldEnterSleep() const {
    """Determine if the brain should enter sleep based on various factors."""
    
    // High novelty levels
    float novelty = 0.0f;
    if (pImpl->novelty) {
        novelty = pImpl->novelty->getLevel();
    }
    if (novelty > 0.3f) {  // High novelty triggers sleep
        NLM_LOG_INFO("High novelty levels (" + std::to_string(novelty) + ") triggering sleep");
        return true;
    }
    
    // High memory load
    size_t memoryLoad = 0;
    if (pImpl->workingMemory) {
        memoryLoad += pImpl->workingMemory->getActiveTraces();
    }
    if (pImpl->episodicMemory) {
        memoryLoad += pImpl->episodicMemory->getEpisodeCount();
    }
    if (memoryLoad > 100) {  // High memory load triggers sleep
        NLM_LOG_INFO("High memory load (" + std::to_string(memoryLoad) + ") triggering sleep");
        return true;
    }
    
    // Elevated stress (negative dopamine)
    if (pImpl->dopamine) {
        float dopamine = pImpl->dopamine->getLevel();
        if (dopamine < -0.5f) {  // High stress triggers sleep
            NLM_LOG_INFO("Elevated stress (dopamine: " + std::to_string(dopamine) + ") triggering sleep");
            return true;
        }
    }
    
    // Time-based sleep (every N steps)
    if (pImpl->currentStep > 0 && pImpl->currentStep % 500 == 0) {  // Sleep every 500 steps
        NLM_LOG_INFO("Time-based sleep trigger (step " + std::to_string(pImpl->currentStep) + ")");
        return true;
    }
    
    // Default: no sleep
    return false;
}

void Brain::enterSleepState() {
    """Prepare the brain for sleep."""
    
    // Set resting flag
    pImpl->isResting = true;
    
    // Log sleep entry
    NLM_LOG_INFO("Brain entering sleep/rest cycle");
    
    // Store pre-sleep state for comparison
    pImpl->preSleepNovelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
    pImpl->preSleepDopamine = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    pImpl->preSleepCuriosity = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
}

void Brain::consolidateMemoriesDuringSleep() {
    """Consolidate memories during sleep for long-term storage."""
    
    // Working memory consolidation
    if (pImpl->workingMemory) {
        // Transfer important working memory traces to episodic memory
        std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        size_t memoryCapacity = pImpl->workingMemory->getCapacity();
        
        if (!memoryNeurons.empty()) {
            // Create episodes from working memory
            for (size_t i = 0; i < std::min(memoryNeurons.size(), 10UL); ++i) {
                // Create episodic memory from working memory
                EpisodicMemoryItem episode;
                episode.timestamp = pImpl->currentStep;
                episode.action = ActionType::Wait;  // Default action
                
                // Store in episodic memory
                if (pImpl->episodicMemory) {
                    pImpl->episodicMemory->storeEpisode(episode);
                }
            }
        }
        
        // Clear working memory after consolidation
        pImpl->workingMemory->clear();
    }
    
    // Episodic memory consolidation
    if (pImpl->episodicMemory) {
        // Consolidate by removing weak episodes
        // Use a higher threshold during sleep for more aggressive consolidation
        float consolidationThreshold = 0.2f;  // Lower threshold = more consolidation
        pImpl->episodicMemory->consolidate(consolidationThreshold);
        
        // Log consolidation stats
        NLM_LOG_INFO("Consolidated episodic memory: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()) + " episodes remaining");
    }
}

void Brain::replayMemoriesDuringSleep() {
    """Replay memories during sleep to strengthen connections."""
    
    if (!pImpl->episodicMemory) {
        return;
    }
    
    // Get recent episodes for replay
    auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
    
    // Replay each episode to strengthen neural connections
    for (const auto* episode : recentEpisodes) {
        if (episode) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    NLM_LOG_INFO("Replayed " + std::to_string(recentEpisodes.size()) + " episodes during sleep");
}

void Brain::resetNeuromodulationDuringSleep() {
    """Reset neuromodulatory systems during sleep."""
    
    // Reset dopamine levels
    if (pImpl->dopamine) {
        // Decay dopamine levels toward zero
        float currentDopamine = pImpl->dopamine->getLevel();
        if (currentDopamine > 0.0f) {
            pImpl->dopamine->setLevel(currentDopamine * 0.3f);  // Significant decay
        } else if (currentDopamine < 0.0f) {
            pImpl->dopamine->setLevel(currentDopamine * 0.7f);  // Less decay for negative
        }
    }
    
    // Reset curiosity levels
    if (pImpl->curiosity) {
        // Curiosity should decay during sleep
        float currentCuriosity = pImpl->curiosity->getLevel();
        pImpl->curiosity->setLevel(currentCuriosity * 0.5f);
    }
    
    // Reset novelty detection (will restart upon waking)
    // Novelty naturally resets during sleep
    
    // Reset prediction error
    if (pImpl->predictionError) {
        // Prediction error should stabilize during sleep
        float currentPredictionError = pImpl->predictionError->getMagnitude();
        pImpl->predictionError->setLevel(currentPredictionError * 0.7f);
    }
    
    // Log neuromodulation reset
    NLM_LOG_INFO("Neuromodulation systems reset during sleep");
}

void Brain::developmentDuringSleep() {
    """Allow development system to progress during sleep."""
    
    if (!pImpl->developmentSystem) {
        return;
    }
    
    // Progress development during sleep
    // Sleep provides time for developmental changes
    double sleepDuration = 100.0;  // Simulation steps of sleep
    pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * sleepDuration);
    
    // Update developmental stage
    DevelopmentalStage currentStage = getDevelopmentalStage();
    
    // Simple developmental progression during sleep
    if (currentStage == DevelopmentalStage::Initial) {
        // Progress to Critical Period after sufficient time
        if (pImpl->developmentalAge >= 60.0) {  // 1 minute of simulated time
            setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            NLM_LOG_INFO("Brain progressed to Critical Period development stage");
        }
    } else if (currentStage == DevelopmentalStage::CriticalPeriod) {
        // Progress to Maturation after sufficient time
        if (pImpl->developmentalAge >= 300.0) {  // 5 minutes of simulated time
            setDevelopmentalStage(DevelopmentalStage::Maturation);
            NLM_LOG_INFO("Brain progressed to Maturation development stage");
        }
    } else if (currentStage == DevelopmentalStage::Maturation) {
        // Progress to Adult after sufficient time
        if (pImpl->developmentalAge >= 900.0) {  // 15 minutes of simulated time
            setDevelopmentalStage(DevelopmentalStage::Adult);
            NLM_LOG_INFO("Brain progressed to Adult development stage");
        }
    }
}

void Brain::exitSleepState() {
    """Exit sleep state and prepare for awake cycle."""
    
    // Clear resting flag
    pImpl->isResting = false;
    
    // Reset novelty detection for new experiences
    // Novelty will start fresh after sleep
    
    // Log sleep exit
    NLM_LOG_INFO("Brain exited sleep/rest cycle, ready for new learning");
    
    // Increment developmental age during sleep
    pImpl->developmentalAge += 100.0;  // Sleep contributes to development
    
    // Update development system with new age
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setDevelopmentalAge(pImpl->developmentalAge);
    }
}