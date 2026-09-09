void NeuralPlanner::setMemoryContext(MemoryContext* context) {
    memoryContext_ = context;
}

float NeuralPlanner::evaluateActionWithContext(ActionType action, 
                                              const std::vector<float>& state,
                                              const std::vector<float>& currentGoal) {
    // Base evaluation from existing method
    float value = evaluateAction(action, state);
    
    // Integration with memory systems for enhanced evaluation
    if (memoryContext_ && !memoryContext_->getRecentWorkingMemoryTraces().empty()) {
        // Get relevant working memory traces
        auto traces = memoryContext_->retrieveRelevantWorkingMemory(state, 5);
        
        if (!traces.empty()) {
            // Integrate with episodic memory for context
            auto episodes = memoryContext_->retrieveRelevantEpisodes(state, 3);
            
            // Combine trace relevance with episodic relevance for enhanced evaluation
            float memoryBoost = 0.0f;
            for (const auto& trace : traces) {
                memoryBoost += trace.relevance;
            }
            
            float episodeBoost = 0.0f;
            for (const auto& episode : episodes) {
                episodeBoost += episode.relevance;
            }
            
            // Apply memory-based boost to action evaluation
            value += (memoryBoost * 0.3f + episodeBoost * 0.2f);
        }
    }
    
    // If goal is set, consider episodic memory that matches the goal
    if (!currentGoal.empty() && memoryContext_) {
        auto goalEpisodes = memoryContext_->retrieveRelevantEpisodes(currentGoal, 3);
        if (!goalEpisodes.empty()) {
            // Strong goal alignment bonus
            value += goalEpisodes[0].relevance * 0.4f;
        }
    }
    
    return value;
}