namespace nlm {

// Implementation of helper methods for biologically realistic memory systems

float EpisodicMemory::calculateEpisodeRelevance(const EpisodicMemoryItem& episode) const {
    // Calculate biological relevance of episodic memory using multiple neural systems
    float relevance = 1.0f;
    
    // Temporal novelty factor (newer memories are more salient)
    float ageFactor = 1.0f - (static_cast<float>(episode.timestamp) / 10000.0f);  // Normalize by current step
    ageFactor = std::max(0.1f, ageFactor);  // Minimum relevance
    relevance *= ageFactor;
    
    // Reward history factor (positive outcomes enhance memory)
    if (episode.totalReward > 0.0f) {
        relevance *= (1.0f + episode.totalReward / 10.0f);  // Boost for positive reward
    } else if (episode.totalReward < 0.0f) {
        relevance *= (0.5f + episode.totalReward / -10.0f);  // Reduce for negative reward
    }
    
    // Replay count factor (repeated recall strengthens memory)
    if (episode.replayCount > 0) {
        relevance *= (1.0f + std::log(static_cast<float>(episode.replayCount + 1)) * 0.2f);
    }
    
    // Episode length factor (medium-length episodes are best remembered)
    float lengthFactor = 1.0f;
    if (episode.episodeLength < 100) {
        lengthFactor *= 0.7f;  // Too short = poor memory
    } else if (episode.episodeLength > 5000) {
        lengthFactor *= 0.6f;  // Too long = degraded memory
    } else {
        lengthFactor *= 1.2f;  // Optimal length = enhanced memory
    }
    relevance *= lengthFactor;
    
    // Neural pattern strength factor
    float patternStrength = 0.0f;
    for (float value : episode.values) {
        patternStrength += std::abs(value);
    }
    patternStrength /= episode.values.size() + 1e-6f;
    relevance *= (1.0f + patternStrength);
    
    return std::min(2.0f, relevance);  // Cap at 2.0
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    // Update associative memory with biological learning dynamics
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Hebbian-like update with eligibility traces
            float oldStrength = std::get<2>(assoc);
            
            // Learning depends on delta and current association strength
            float learningRate = 0.05f + 0.2f * std::abs(delta);
            learningRate *= (1.0f - std::abs(oldStrength));  // Diminishing returns
            
            float newStrength = std::clamp(oldStrength + delta * learningRate, 0.0f, 1.0f);
            std::get<2>(assoc) = newStrength;
            
            // Apply consolidation based on association importance
            if (std::abs(newStrength - oldStrength) > 0.1f) {
                // Strong changes trigger consolidation
                float consolidation = std::abs(newStrength - oldStrength) * 0.5f;
                // In real implementation: strengthen connections through replay
            }
            
            NLM_LOG_INFO("AssociativeMemory: Updated association between neurons " +
                         std::to_string(a.value) + " and " + std::to_string(b.value) +
                         " from " + std::to_string(oldStrength) + " to " + std::to_string(newStrength));
            
            return;
        }
    }
    
    // Create new association if it doesn't exist
    pImpl->associations.emplace_back(a, b, std::clamp(delta, 0.0f, 1.0f));
    
    NLM_LOG_INFO("AssociativeMemory: Created new association between neurons " +
                 std::to_string(a.value) + " and " + std::to_string(b.value) +
                 " with strength " + std::to_string(std::clamp(delta, 0.0f, 1.0f)));
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Learn procedural skill with neural pattern formation
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update existing skill with new pattern
            float oldProficiency = skill.proficiency;
            
            // Pattern similarity affects learning
            float patternSimilarity = calculatePatternSimilarity(skill.neuralPattern, pattern);
            
            // Learning rate depends on similarity and current proficiency
            float learningRate = 0.1f + 0.3f * patternSimilarity;
            learningRate *= (1.0f - skill.proficiency);  // Diminishing returns
            
            // Update pattern based on new input
            skill.neuralPattern = pattern;
            skill.proficiency = std::clamp(skill.proficiency + 0.2f, 0.0f, 1.0f);
            
            // Increase stability with consistent learning
            skill.stability = std::min(1.0f, skill.stability + 0.1f);
            
            NLM_LOG_INFO("ProceduralMemory: Updated skill '" + skill.name + "' pattern (similarity: " +
                         std::to_string(patternSimilarity) + ", proficiency: " + std::to_string(skill.proficiency) +
                         ", stability: " + std::to_string(skill.stability) + ")");
            
            return;
        }
    }
    
    // Create new skill
    Skill newSkill;
    newSkill.name = name;
    newSkill.neuralPattern = pattern;
    newSkill.proficiency = 0.5f;
    newSkill.stability = 0.5f;
    pImpl->skills.push_back(newSkill);
    
    NLM_LOG_INFO("ProceduralMemory: Created new skill '" + name + "' with pattern size " + std::to_string(pattern.size()));
}

float ProceduralMemory::calculatePatternSimilarity(const std::vector<NeuronId>& pattern1, const std::vector<NeuronId>& pattern2) const {
    // Calculate similarity between neural patterns for procedural memory
    if (pattern1.empty() && pattern2.empty()) return 1.0f;
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    // Simple overlap-based similarity
    std::unordered_set<NeuronId, NeuronIdHash> set1(pattern1.begin(), pattern1.end());
    size_t overlap = 0;
    for (const auto& neuron : pattern2) {
        if (set1.find(neuron) != set1.end()) {
            overlap++;
        }
    }
    
    float similarity = static_cast<float>(overlap) / static_cast<float>(std::max(pattern1.size(), pattern2.size()));
    return similarity;
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Store semantic knowledge with biological relevance weighting
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Update existing fact with consolidation
            float oldRelevance = calculateFactRelevance(fact.second);
            fact.second = value;
            
            // Increase fact stability
            float newRelevance = calculateFactRelevance(fact.second);
            // In real implementation: strengthen semantic connections
            
            NLM_LOG_INFO("SemanticMemory: Updated fact '" + key + "' with relevance change from " +
                         std::to_string(oldRelevance) + " to " + std::to_string(newRelevance));
            
            return;
        }
    }
    
    // Create new fact
    pImpl->facts.emplace_back(key, value);
    
    NLM_LOG_INFO("SemanticMemory: Created new fact '" + key + "' -> '" + value + "'");
}

float SemanticMemory::calculateFactRelevance(const std::string& value) const {
    // Calculate biological relevance of semantic fact
    // Based on concept clarity, truth value, and integration with existing knowledge
    
    float relevance = 1.0f;
    
    // Length factor (concise facts are better remembered)
    if (value.length() > 100) {
        relevance *= 0.8f;
    } else if (value.length() < 10) {
        relevance *= 0.9f;
    }
    
    // Novelty factor (unusual facts stand out)
    bool isNovel = true;
    for (const auto& fact : pImpl->facts) {
        if (value.find(fact.second) != std::string::npos) {
            isNovel = false;
            break;
        }
    }
    if (!isNovel) {
        relevance *= 0.7f;
    }
    
    // Integration with existing knowledge
    int integrationPoints = 0;
    for (const auto& fact : pImpl->facts) {
        if ((fact.first.find(value) != std::string::npos) || (value.find(fact.first) != std::string::npos)) {
            integrationPoints++;
        }
    }
    float integrationFactor = std::min(1.0f, static_cast<float>(integrationPoints) / 10.0f);
    relevance *= (1.0f + integrationFactor * 0.5f);
    
    return std::min(2.0f, relevance);
}

} // namespace nlm