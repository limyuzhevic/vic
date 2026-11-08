// Check if memory systems have been properly initialized
    NLM_LOG_INFO("=== Memory System Integration Check ===");
    
    bool memorySystemsOK = true;
    
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Working memory not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Working memory initialized with " + std::to_string(pImpl->workingMemory->getActiveTraces()) + " active traces");
    }
    
    if (!pImpl->episodicMemory) {
        NLM_LOG_ERROR("Episodic memory not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Episodic memory initialized with " + std::to_string(pImpl->episodicMemory->getEpisodeCount()) + " episodes");
    }
    
    if (!pImpl->associativeMemory) {
        NLM_LOG_ERROR("Associative memory not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Associative memory initialized with " + std::to_string(pImpl->associativeMemory->getAssociationCount()) + " associations");
    }
    
    if (!pImpl->predictionSystem) {
        NLM_LOG_ERROR("Prediction system not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Prediction system initialized");
    }
    
    if (!pImpl->attention) {
        NLM_LOG_ERROR("Attention system not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Attention system initialized");
    }
    
    if (!pImpl->novelty) {
        NLM_LOG_ERROR("Novelty system not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Novelty system initialized with level " + std::to_string(pImpl->novelty->getLevel()));
    }
    
    if (!pImpl->curiosity) {
        NLM_LOG_ERROR("Curiosity system not initialized!");
        memorySystemsOK = false;
    } else {
        NLM_LOG_INFO("Curiosity system initialized with level " + std::to_string(pImpl->curiosity->getLevel()));
    }
    
    if (memorySystemsOK) {
        NLM_LOG_INFO("✓ All memory systems properly integrated and initialized");
    } else {
        NLM_LOG_ERROR("✗ Some memory systems failed to initialize properly");
    }
    
    return memorySystemsOK;
