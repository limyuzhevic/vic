// Additional helper functions for advanced memory management

namespace nlm {

// Advanced memory operations that enhance existing systems

// 1. Adaptive memory consolidation that respects learning goals
void adaptiveConsolidation(
    NeuralEpisodicMemory& episodicMemory,
    NeuralWorkingMemory& workingMemory,
    const std::vector<float>& rewardHistory,
    float consolidationRate = 0.1f,
    float ageThreshold = 100.0f) {
    
    // Calculate consolidation priority based on reward and age
    float totalReward = std::accumulate(rewardHistory.begin(), rewardHistory.end(), 0.0f);
    
    // Prioritize consolidation of high-reward, recent episodes
    float consolidationThreshold = 
        (rewardHistory.empty() ? 0.0f : totalReward / rewardHistory.size()) * 0.5f;
    
    // Also consider working memory load - don't consolidate too much when working memory is full
    size_t workingMemoryCapacity = workingMemory.getCapacity();
    size_t currentWorkingMemoryLoad = workingMemory.getMemoryNeuronCount();
    
    if (currentWorkingMemoryLoad > workingMemoryCapacity * 0.8f) {
        // Working memory is near capacity, be conservative with consolidation
        consolidationThreshold *= 0.7f;
    }
    
    episodicMemory.consolidate(consolidationThreshold);
}

// 2. Advanced retrieval with context-based filtering
std::vector<const EpisodicMemoryItem*> advancedRetrieve(
    const NeuralEpisodicMemory& episodicMemory,
    const std::vector<float>& queryPattern,
    size_t maxResults = 10,
    float minSimilarity = 0.3f,
    const std::vector<float>* contextVector = nullptr,
    float contextWeight = 0.5f) {
    
    std::vector<const EpisodicMemoryItem*> allResults;
    
    // Get candidates based on pattern similarity
    allResults = episodicMemory.retrieveSimilar(queryPattern, maxResults * 2);
    
    // Apply context filtering if provided
    if (contextVector) {
        std::vector<const EpisodicMemoryItem*> filteredResults;
        
        for (const auto* episode : allResults) {
            // Calculate combined similarity (pattern + context)
            float patternSimilarity = 0.0f;
            for (size_t i = 0; i < queryPattern.size() && i < episode->values.size(); ++i) {
                patternSimilarity += queryPattern[i] * episode->values[i];
            }
            
            float contextSimilarity =  PKA;
            
            float combinedSimilarity = patternSimilarity * (1.0f - contextWeight) + 
                                      contextSimilarity * contextWeight;
            
            if (combinedSimilarity > minSimilarity) {
                filteredResults.push_back(episode);
            }
        }
        
        allResults = filteredResults;
    }
    
    // Sort by combined relevance and return top results
    std::vector<std::pair<float, const EpisodicMemoryItem*>> scoredResults;
    for (const auto* episode : allResults) {
        float score = 0.0f;
        
        // Pattern match score
        for (size_t i = 0; i < queryPattern.size() && i < episode->values.size(); ++i) {
            score += queryPattern[i] * episode->values[i];
        }
        
        // Recency bonus
        float recencyBonus = 1.0f / (1.0f + static_cast<float>(episode->timestamp) / 100.0f);
        
        // Reward bonus
        float rewardBonus = episode->metadata.empty() ? 0.0f : std::stof(episode->metadata) * 0.5f;
        
        score = score * 0.7f + recencyBonus * 0.2f + rewardBonus * 0.1f;
        
        scoredResults.emplace_back(score, episode);
    }
    
    std::sort(scoredResults.begin(), scoredResults.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<const EpisodicMemoryItem*> finalResults;
    size_t resultCount = std::min(maxResults, scoredResults.size());
    for (size_t i = 0; i < resultCount; ++i) {
        finalResults.push_back(scoredResults[i].second);
    }
    
    return finalResults;
}

// 3. Memory capacity management with adaptive allocation
void adaptiveMemoryManagement(
    NeuralWorkingMemory& workingMemory,
    NeuralEpisodicMemory& episodicMemory,
    const std::vector<float>& recentActivity,
    size_t targetWorkingMemorySize = 50,
    size_t targetEpisodicLimit = 1000) {
    
    // Adjust working memory capacity based on recent activity
    if (!recentActivity.empty()) {
        float avgActivity = std::accumulate(recentActivity.begin(), recentActivity.end(), 0.0f) / recentActivity.size();
        
        // Higher activity -> larger working memory capacity
        size_t newCapacity = static_cast<size_t>(targetWorkingMemorySize * (1.0f + avgActivity));
        workingMemory.setCapacity(std::max(targetWorkingMemorySize, std::min(newCapacity, targetWorkingMemorySize * 2)));
    }
    
    // Adjust episodic memory capacity based on learning
    size_t currentEpisodicCount = episodicMemory.getEpisodeCount();
    
    if (currentEpisodicCount < targetEpisodicLimit * 0.5f) {
        // Still have room - be conservative
        episodicMemory.maxEpisodes_ = targetEpisodicLimit;
    } else if (currentEpisodicCount > targetEpisodicLimit * 1.5f) {
        // Too full - be aggressive with consolidation
        episodicMemory.consolidate(0.2f);
        episodicMemory.maxEpisodes_ = targetEpisodicLimit;
    }
}

// 4. Cross-memory correlation analysis
struct MemoryCorrelation {
    float patternSimilarity;
    float temporalProximity;
    float rewardCorrelation;
    float overallScore;
    
    MemoryCorrelation() : patternSimilarity(0.0f), temporalProximity(0.0f), rewardCorrelation(0.0f), overallScore(0.0f) {}
};

MemoryCorrelation analyzeMemoryCorrelation(
    const NeuralWorkingMemory& workingMemory,
    const NeuralEpisodicMemory& episodicMemory,
    const std::vector<float>& currentContext) {
    
    MemoryCorrelation correlation;
    
    // Calculate pattern similarity between working memory and episodic memory
    if (!workingMemory.getMemoryNeurons().empty() && !episodicMemory.getEpisodeCount()) {
        for (size_t i = 0; i < workingMemory.getMemoryNeuronCount(); ++i) {
            // This would require accessing episode neural patterns, simplified here
            correlation.patternSimilarity += workingMemory.getNeuronActivation(
                workingMemory.getMemoryNeurons()[i]) * 0.1f;
        }
    }
    
    // Calculate temporal proximity (how recent episodes correlate with current working memory)
    const auto& recentEpisodes = episodicMemory.getRecentEpisodes(5);
    for (const auto* episode : recentEpisodes) {
        correlation.temporalProximity += 1.0f / (1.0f + static_cast<float>(episode->timestamp) / 100.0f);
    }
    
    // Calculate reward correlation if available in context
    for (float value : currentContext) {
        correlation.rewardCorrelation += std::max(0.0f, value);
    }
    
    // Combine scores
    correlation.overallScore = (correlation.patternSimilarity * 0.4f + 
                               correlation.temporalProximity * 0.3f + 
                               correlation.rewardCorrelation * 0.3f);
    
    return correlation;
}

// 5. Advanced memory search with multiple criteria
struct MemorySearchCriteria {
    std::vector<float> patternQuery;
    float minSimilarity;
    SimulationStep timeWindowStart;
    SimulationStep timeWindowEnd;
    std::vector<ActionType> targetActions;
    float minReward;
    
    MemorySearchCriteria() : minSimilarity(0.3f), timeWindowStart(0), timeWindowEnd(0), minReward(0.0f) {}
};

std::vector<const EpisodicMemoryItem*> advancedMemorySearch(
    const NeuralEpisodicMemory& episodicMemory,
    const MemorySearchCriteria& criteria) {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    // Temporal filtering
    std::vector<const EpisodicMemoryItem*> temporalResults;
    for (const auto& episode : episodicMemory.getAllEpisodes()) {
        if (episode.timestamp >= criteria.timeWindowStart && 
            episode.timestamp <= criteria.timeWindowEnd) {
            temporalResults.push_back(&episode);
        }
    }
    
    // Pattern filtering
    std::vector<const EpisodicMemoryItem*> patternResults;
    for (const auto& episode : temporalResults) {
        float similarity = 0.0f;
        for (size_t i = 0; i < criteria.patternQuery.size() && i < episode->values.size(); ++i) {
            similarity += criteria.patternQuery[i] * episode->values[i];
        }
        
        if (similarity >= criteria.minSimilarity) {
            patternResults.push_back(&episode);
        }
    }
    
    // Action filtering
    std::vector<const EpisodicMemoryItem*> actionResults;
    for (const auto& episode : patternResults) {
        if (criteria.targetActions.empty() || 
            std::find(criteria.targetActions.begin(), criteria.targetActions.end(), episode->action) != criteria.targetActions.end()) {
            actionResults.push_back(&episode);
        }
    }
    
    // Reward filtering
    for (const auto& episode : actionResults) {
        float reward = episode->metadata.empty() ? 0.0f : std::stof(episode->metadata);
        if (reward >= criteria.minReward) {
            results.push_back(episode);
        }
    }
    
    // Sort by relevance (pattern similarity first, then recency)
    std::sort(results.begin(), results.end(),
              [criteria](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  float similarityA = 0.0f, similarityB = 0.0f;
                  for (size_t i = 0; i < criteria.patternQuery.size() && i < a->values.size(); ++i) {
                      similarityA += criteria.patternQuery[i] * a->values[i];
                  }
                  for (size_t i = 0; i < criteria.patternQuery.size() && i < b->values.size(); ++i) {
                      similarityB += criteria.patternQuery[i] * b->values[i];
                  }
                  
                  if (std::abs(similarityA - similarityB) > 0.01f) {
                      return similarityA > similarityB;
                  }
                  return a->timestamp > b->timestamp;
              });
    
    return results;
}

} // namespace nlm
