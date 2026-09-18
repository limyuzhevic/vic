// Update concept formation from sensory input
void ConceptFormation::updateFromSensory(const SensoryInput& input) {
    NLM_LOG_INFO("ConceptFormation: Updating from sensory input");
    
    // Extract pattern and features from sensory input
    std::vector<float> pattern = input.getData();
    std::vector<float> features;
    
    // Simple feature extraction (could be more sophisticated)
    if (!pattern.empty()) {
        // For each dimension, compute simple statistics as features
        float min = *std::min_element(pattern.begin(), pattern.end());
        float max = *std::max_element(pattern.begin(), pattern.end());
        float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
        
        features.push_back(min);
        features.push_back(max);
        features.push_back(mean);
        
        // Add variance as a feature
        float variance = 0.0f;
        for (float val : pattern) {
            variance += (val - mean) * (val - mean);
        }
        variance /= pattern.size();
        features.push_back(std::sqrt(variance));
        
        // Present this experience for concept formation
        // Use a neutral reward for general sensory processing
        presentExperience(pattern, features, 0.0f, 0);
    }
}

// Update concept formation from episodic memory
void ConceptFormation::updateFromEpisodicMemory(const NeuralEpisodicMemory& em) {
    NLM_LOG_INFO("ConceptFormation: Updating from episodic memory");
    
    // Get recent episodes to update concepts
    const auto& episodes = em.getRecentEpisodes(5);
    
    for (const auto* episode : episodes) {
        if (episode) {
            // Extract sensory state and reward from episode
            std::vector<float> pattern = episode->sensoryState;
            std::vector<float> features;
            
            if (!pattern.empty()) {
                // Extract simple features from episode sensory state
                float min = *std::min_element(pattern.begin(), pattern.end());
                float max = *std::max_element(pattern.begin(), pattern.end());
                float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
                
                features.push_back(min);
                features.push_back(max);
                features.push_back(mean);
                
                // Present episode with its actual reward for learning
                presentExperience(pattern, features, episode->reward, episode->timestamp);
            }
        }
    }
}

// Get current concepts as strings
void ConceptFormation::getCurrentConcepts(std::vector<std::string>& concepts) const {
    concepts.clear();
    
    // Convert concept IDs to meaningful labels based on category hints
    for (const auto& concept : concepts_) {
        if (!concept.categoryHint.empty()) {
            concepts.push_back(concept.categoryHint);
        } else {
            // Create a generic label for concepts without explicit category
            std::string label = "Concept_" + std::to_string(concept.id);
            concepts.push_back(label);
        }
    }
}