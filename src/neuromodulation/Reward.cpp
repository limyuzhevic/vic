namespace nlm {

// Implementation of helper methods for biologically realistic reward computation

float Reward::computePolicyValue(const Observation& observation) const {
    // Compute policy value using actor-critic framework
    // Estimates value of current policy given state
    
    float value = 0.0f;
    
    // Get current state representation
    const auto& resources = observation.getResources();
    const auto& hazards = observation.getHazards();
    size_t stepsSinceNew = observation.getStepsSinceLastNewLocation();
    
    // Resource value based on proximity and quantity
    float resourceValue = 0.0f;
    for (const auto& resource : resources) {
        float dist = std::sqrt(resource.x * resource.x + resource.y * resource.y);
        resourceValue += std::exp(-dist / 10.0f);  // Distance-based decay
    }
    
    // Hazard penalty
    float hazardValue = 0.0f;
    for (const auto& hazard : hazards) {
        float dist = std::sqrt(hazard.x * hazard.x + hazard.y * hazard.y);
        hazardValue += std::exp(-dist / 5.0f);  // Distance-based penalty
    }
    
    // Novelty factor
    float noveltyValue = 1.0f - std::exp(-static_cast<float>(stepsSinceNew) / 50.0f);
    
    // Policy value as weighted combination
    value = resourceValue * 0.5f + hazardValue * 0.3f + noveltyValue * 0.2f;
    
    // Normalize to reasonable range
    value = value / 10.0f;  // Scale down
    value = std::max(0.0f, std::min(1.0f, value));
    
    return value;
}

float Reward::computeStateValue(const Observation& observation) const {
    // Compute state value using learned value function
    // Estimates true expected return from this state
    
    float value = 0.0f;
    
    // Get environmental features
    const auto& resources = observation.getResources();
    const auto& hazards = observation.getHazards();
    size_t stepsSinceNew = observation.getStepsSinceLastNewLocation();
    
    // State value based on environmental statistics
    float resourceDensity = static_cast<float>(resources.size()) / 100.0f;  // Normalize
    float hazardDensity = static_cast<float>(hazards.size()) / 50.0f;      // Normalize
    float explorationLevel = 1.0f - std::exp(-static_cast<float>(stepsSinceNew) / 100.0f);
    
    // State value as combination of features
    value = resourceDensity * 0.4f + hazardDensity * 0.3f + explorationLevel * 0.3f;
    
    // Add temporal dynamics
    if (pImpl->history.size() > 1) {
        float recentAvg = 0.0f;
        for (size_t i = std::max(size_t(1), pImpl->history.size() - 10); i < pImpl->history.size(); ++i) {
            recentAvg += pImpl->history[i];
        }
        recentAvg /= std::min(static_cast<float>(pImpl->history.size()), 10.0f);
        
        // Temporal difference between recent rewards and current estimate
        float temporalDifference = value - recentAvg;
        value += temporalDifference * 0.1f;  // Adaptive learning
    }
    
    return std::max(0.0f, std::min(1.0f, value));
}

void Reward::updateActorCritic(const Observation& observation, float policyValue, float stateValue, float rewardPredictionError) {
    // Update actor-critic parameters using reward prediction error
    // Core of temporal difference learning
    
    // Actor update (policy improvement)
    // In real implementation: update policy parameters based on RPE
    float actorLearningRate = 0.01f + std::abs(rewardPredictionError) * 0.05f;
    
    // Critic update (value function improvement)
    // In real implementation: update value function parameters
    float criticLearningRate = 0.05f + rewardPredictionError * 0.02f;
    
    // Eligibility traces for efficient credit assignment
    float eligibilityTrace = 0.9f;  // Decay rate
    if (std::abs(rewardPredictionError) > 0.1f) {
        // Strong RPE increases eligibility trace
        eligibilityTrace = 0.95f;
    }
    
    // Store learning statistics
    // In real implementation: store actor/critic weights, eligibility traces
    
    NLM_LOG_INFO("Reward: Actor-critic update - RPE: " + std::to_string(rewardPredictionError) +
                 ", actor LR: " + std::to_string(actorLearningRate) +
                 ", critic LR: " + std::to_string(criticLearningRate) +
                 ", eligibility: " + std::to_string(eligibilityTrace));
}

float Reward::computeCuriosityBonus(const Observation& observation) const {
    // Compute curiosity bonus for intrinsic motivation
    // Encourages exploration of novel and uncertain environments
    
    float curiosity = 0.0f;
    
    // Novelty-based curiosity
    size_t stepsSinceNew = observation.getStepsSinceLastNewLocation();
    float noveltyCuriosity = 1.0f - std::exp(-static_cast<float>(stepsSinceNew) / 30.0f);
    
    // Surprise-based curiosity (prediction error)
    float predictionError = std::abs(pImpl->currentValue - computeStateValue(observation));
    float surpriseCuriosity = predictionError * 0.5f;
    
    // Information gain curiosity
    float informationGain = 0.0f;
    if (pImpl->history.size() > 5) {
        // Estimate information gain from recent history
        float recentStd = 0.0f;
        float recentMean = 0.0f;
        for (size_t i = pImpl->history.size() - 5; i < pImpl->history.size(); ++i) {
            recentMean += pImpl->history[i];
        }
        recentMean /= 5.0f;
        
        for (size_t i = pImpl->history.size() - 5; i < pImpl->history.size(); ++i) {
            recentStd += (pImpl->history[i] - recentMean) * (pImpl->history[i] - recentMean);
        }
        recentStd = std::sqrt(recentStd / 5.0f);
        
        informationGain = recentStd / (recentMean + 0.1f);  // Coefficient of variation
    }
    
    // Combine curiosity sources
    curiosity = noveltyCuriosity * 0.3f + surpriseCuriosity * 0.4f + informationGain * 0.3f;
    
    // Apply curiosity modulation (depends on current motivation level)
    float motivationLevel = std::min(1.0f, pImpl->accumulatedReward / 10.0f + 0.5f);
    curiosity *= motivationLevel;
    
    return curiosity * 0.5f;  // Scale to reasonable range
}

float Reward::computeMetaLearningBonus(const Observation& observation) const {
    // Compute meta-reinforcement learning bonus
    // Encourages learning about learning, adaptation strategies
    
    float metaBonus = 0.0f;
    
    // Learning speed bonus (adapt quickly to changing environment)
    float learningSpeed = 0.0f;
    if (pImpl->history.size() > 10) {
        // Measure learning rate from reward history
        float firstHalf = 0.0f, secondHalf = 0.0f;
        size_t halfSize = pImpl->history.size() / 2;
        
        for (size_t i = 0; i < halfSize; ++i) {
            firstHalf += pImpl->history[i];
        }
        for (size_t i = halfSize; i < pImpl->history.size(); ++i) {
            secondHalf += pImpl->history[i];
        }
        
        firstHalf /= halfSize;
        secondHalf /= (pImpl->history.size() - halfSize);
        
        learningSpeed = std::abs(secondHalf - firstHalf);
    }
    
    // Adaptation bonus
    float adaptationBonus = 1.0f - learningSpeed * 0.5f;
    
    // Strategy exploration bonus
    float strategyExploration = 0.0f;
    if (pImpl->history.size() > 20) {
        // Check for exploration of different strategies
        float uniqueRewards = 0.0f;
        std::unordered_set<float> rewardSet;
        for (float reward : pImpl->history) {
            // Quantize rewards for categorical analysis
            int quantized = static_cast<int>(std::round(reward * 2.0f));  // 5 bins
            rewardSet.insert(static_cast<float>(quantized));
        }
        strategyExploration = static_cast<float>(rewardSet.size()) / 5.0f;  // Max 5 distinct rewards
    }
    
    // Combine meta-learning components
    metaBonus = adaptationBonus * 0.4f + strategyExploration * 0.6f;
    
    return metaBonus * 0.3f;  // Scale to reasonable range
}

} // namespace nlm