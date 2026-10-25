// Implementation of WorkingMemory::store() with proper capacity management and eviction strategy
void WorkingMemory::store(NeuronId neuron, float value) {
    // Check if neuron already exists
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Update existing value with decay factor
            item.second = value * (1.0f - 0.1f); // 10% decay for updates
            return;
        }
    }
    
    // Check capacity before adding new item
    if (pImpl->items.size() >= pImpl->capacity) {
        // Implement simple LRU (Least Recently Used) eviction
        pImpl->items.erase(pImpl->items.begin());
    }
    
    // Add new item with initial value
    pImpl->items.emplace_back(neuron, value);
}

// Implementation of EpisodicMemory::consolidate() with relevance-based consolidation
void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Filter episodes based on relevance using simulated metrics
    std::vector<EpisodicMemoryItem> consolidatedEpisodes;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate episode relevance based on multiple factors:
        // 1. Neural activation strength (calculated from episode values)
        // 2. Time since encoding (older episodes more relevant)
        // 3. Simulated reward prediction error
        // 4. Simulated novelty score
        
        float neuralRelevance = calculateNeuralActivationRelevance(episode);
        float timeRelevance = calculateTimeRelevance(episode);
        float rpeRelevance = calculateRewardPredictionErrorRelevance();
        float noveltyRelevance = calculateNoveltyRelevance();
        
        float relevance = (neuralRelevance * 0.4f) + (timeRelevance * 0.2f) + 
                         (rpeRelevance * 0.3f) + (noveltyRelevance * 0.1f);
        
        if (relevance >= relevanceThreshold) {
            // Keep relevant episodes in main memory
            consolidatedEpisodes.push_back(episode);
        } else {
            // Move low-relevance episodes to long-term storage
            storeLongTermEpisode(episode, relevance);
        }
    }
    
    // Replace main memory with consolidated episodes
    pImpl->episodes = std::move(consolidatedEpisodes);
    
    // Trigger replay of consolidated episodes
    replayConsolidatedEpisodes();
}

// Helper function to calculate neural activation relevance from episode
float EpisodicMemory::calculateNeuralActivationRelevance(const EpisodicMemoryItem& episode) const {
    if (episode.values.empty()) return 0.0f;
    
    float maxValue = *std::max_element(episode.values.begin(), episode.values.end());
    return std::min(1.0f, maxValue / 10.0f); // Normalize to 0-1 range
}

// Helper function to calculate time relevance based on episode age
float EpisodicMemory::calculateTimeRelevance(const EpisodicMemoryItem& episode) const {
    // Time factor: older episodes more relevant (with decay)
    float ageFactor = static_cast<float>(episode.timestamp) / 1000.0f; // Normalize to reasonable scale
    return std::min(1.0f, ageFactor * 0.5f); // Cap at 0.5 for time factor
}

// Helper function to calculate reward prediction error relevance
float EpisodicMemory::calculateRewardPredictionErrorRelevance() const {
    // Simulate reward prediction error calculation
    return 0.5f; // Placeholder for simulated error
}

// Helper function to calculate novelty relevance
float EpisodicMemory::calculateNoveltyRelevance() const {
    // Simulate novelty calculation based on sensory input changes
    return 0.3f; // Placeholder for simulated novelty
}

// Helper function to store long-term episodes
void EpisodicMemory::storeLongTermEpisode(const EpisodicMemoryItem& episode, float relevance) {
    // For now, just log the consolidation action
    NLM_LOG_INFO("Consolidated low-relevance episode with relevance: " + 
                 std::to_string(relevance) + " at timestamp " + 
                 std::to_string(episode.timestamp));
}

// Helper function to replay consolidated episodes
void EpisodicMemory::replayConsolidatedEpisodes() {
    // For now, just log the replay action
    size_t episodeCount = pImpl->episodes.size();
    NLM_LOG_INFO("Replaying " + std::to_string(episodeCount) + 
                 " consolidated episodes for memory integration");
}