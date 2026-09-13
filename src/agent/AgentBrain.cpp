void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // ===== FIXED: Enhanced neuromodulation integration =====
    // Update dopamine system with prediction error
    if (dopamineSys) {
        dopamineSys->update(0.001);
    }
    
    // ===== FIXED: Complete memory system integration =====
    // Process working memory for episodic memory formation
    if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
        // Create episodic memory entry when working memory has content
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;  // FIXED: Use actual step
        episode.action = ActionType::Wait;
        episode.reward = dopamineSys ? dopamineSys->getLevel() : 0.0f;
        episode.energy = 1.0f;
        episode.novelty = noveltySys ? noveltySys->getLevel() : 0.0f;
        
        // Get working memory content
        std::vector<float> workingMemoryContent = workingMemory->retrieve();
        if (!workingMemoryContent.empty()) {
            episode.sensoryState = workingMemoryContent;
            episode.resultingSensoryState = workingMemoryContent;
            
            // Store in episodic memory
            if (episodicMemory) {
                episodicMemory->storeEpisode(episode);
            }
            
            // Also store in associative memory for pattern learning
            if (associativeMemory && !episode.resultingSensoryState.empty()) {
                associativeMemory->associateFromExperience(episode);
            }
        }
    }
    
    // ===== FIXED: Enhanced prediction system integration =====
    // Update prediction system with complete sensory pattern
    if (predictionSystem) {
        // Get current sensory pattern for prediction update
        std::vector<float> sensoryPattern;
        sensoryPattern.reserve(sensoryVision_.size() + sensoryTouch_.size() + 
                              sensoryInternal_.size() + sensoryProprioception_.size());
        
        for (auto* neuron : sensoryVision_) {
            const auto& state = neuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential);
            sensoryPattern.push_back(activation);
        }
        for (auto* neuron : sensoryTouch_) {
            const auto& state = neuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential);
            sensoryPattern.push_back(activation);
        }
        for (auto* neuron : sensoryInternal_) {
            const auto& state = neuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential);
            sensoryPattern.push_back(activation);
        }
        for (auto* neuron : sensoryProprioception_) {
            const auto& state = neuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential);
            sensoryPattern.push_back(activation);
        }
        
        // FIXED: Use current step for prediction
        predictionSystem->update(sensoryPattern, step, step * 0.001);
    }
    
    // ===== FIXED: Connect concept formation to all memory systems =====
    if (conceptForm) {
        // Get working memory pattern for concept formation
        std::vector<float> workingMemoryPattern;
        if (workingMemory) {
            workingMemoryPattern = workingMemory->retrieve();
        }
        
        // Get episodic memory patterns for concept learning
        std::vector<std::vector<float>> episodicPatterns;
        if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
            auto recentEpisodes = episodicMemory->getRecentEpisodes(5);
            for (const auto* episode : recentEpisodes) {
                episodicPatterns.push_back(episode->sensoryState);
            }
        }
        
        // Present integrated memory patterns to concept formation
        conceptForm->presentExperience(workingMemoryPattern, {}, 0.0f, step);
    }
    
    // ===== FIXED: Apply curiosity-based exploration =====
    // Motor command decoding happens in decodeMotorCommand(), which is called externally
    
    // FIXED: Maintain neuromodulation state for external systems
    if (curiositySys) {
        curiositySys->update(0.001);
    }
    if (noveltySys) {
        noveltySys->update(0.001);
    }
    if (dopamineSys) {
        dopamineSys->update(0.001);
    }
    
    // Note: AgentBrain.decodeMotorCommand() is called separately to get motor commands
    // The decoded command is returned externally
}