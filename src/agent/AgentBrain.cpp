// Advanced cognitive system integration methods for AgentBrain

namespace nlm {

void AgentBrain::updateCognitiveSystems() {
    if (!brain_) return;
    
    // Update neural planning
    if (neuralPlannerEnabled_) {
        updateNeuralPlanner(decodeMotorCommand());
    }
    
    // Update concept formation
    if (conceptFormationEnabled_) {
        updateConceptFormation();
    }
    
    // Update self-model
    if (selfModelEnabled_) {
        updateSelfModel();
    }
    
    // Update social learning
    if (socialLearningEnabled_) {
        updateSocialLearning();
    }
    
    // Update attention with cognitive context
    updateAttentionWithCognitiveContext(noveltyLevel_, predictionError_);
}

void AgentBrain::updateNeuralPlanner(MotorCommand currentAction) {
    // Neural planning for action sequences and future evaluation
    if (!brain_->getPredictionSystem() || !brain_->getPlanner()) {
        return;
    }
    
    // Use the brain's prediction system to evaluate future states
    // In a full implementation, this would:
    // 1. Generate multiple action sequences
    // 2. Predict outcomes for each sequence
    // 3. Select the action with highest predicted value
    
    // For now, maintain plan confidence based on curiosity and experience
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration = lower plan confidence
        planConfidence_ = std::max(0.1f, planConfidence_ - 0.05f);
    } else {
        // Normal operation = increase plan confidence
        planConfidence_ = std::min(1.0f, planConfidence_ + 0.02f);
    }
    
    // Store current action for future planning
    if (brain_->getPlanner()) {
        brain_->getPlanner()->recordActionSequence(currentAction);
    }
}

void AgentBrain::updateConceptFormation() {
    // Concept formation from accumulated experiences
    if (experienceBuffer_.size() < 10) {
        return;  // Need more experiences to form concepts
    }
    
    // Analyze stored experiences to find patterns
    // This would use the brain's concept formation system
    if (brain_->getConceptFormation()) {
        // Extract percept patterns from experiences
        std::vector<std::vector<float>> perceptPatterns;
        std::vector<MotorCommand> actions;
        std::vector<float> rewards;
        
        for (const auto& exp : experienceBuffer_) {
            perceptPatterns.push_back(exp.percept);
            actions.push_back(exp.action);
            rewards.push_back(exp.reward);
        }
        
        // Form concepts from patterns
        brain_->getConceptFormation()->formConcepts(perceptPatterns, rewards);
        
        // Update concept stability based on consistency
        float conceptStability = calculateConceptStability() / perceptPatterns.size();
        conceptStability_ = conceptStability;
    }
}

void AgentBrain::updateSelfModel() {
    // Self-model for body schema and action consequences
    if (!brain_ || !brain_->getConceptFormation()) {
        return;
    }
    
    // Update body schema based on proprioception and sensory feedback
    // This helps the agent understand its own body structure and capabilities
    for (const auto& proprio : previousVision_) {
        // Simple integration of proprioceptive information
        float bodyComponent = proprio * 0.1f;
        bodySchema_.push_back(bodyComponent);
    }
    
    // Update action consequences based on rewards
    // This helps predict the outcomes of different actions
    if (brain_->getPlanner()) {
        float actionConsequence = (expectedReward_ > 0.0f) ? 1.0f : -1.0f;
        actionConsequences_.push_back(actionConsequence);
        
        // Update the planner's self-model
        brain_->getPlanner()->updateSelfModel(bodySchema_, actionConsequences_);
    }
}

void AgentBrain::updateSocialLearning() {
    // Social learning from observations of other agents
    if (socialObservations_.empty()) {
        return;
    }
    
    // Analyze observed behaviors to learn social rules
    if (brain_->getConceptFormation()) {
        // Extract patterns from observations
        std::vector<std::vector<float>> socialContexts;
        std::vector<MotorCommand> observedActions;
        
        for (const auto& obs : socialObservations_) {
            socialContexts.push_back(obs.socialContext);
            observedActions.push_back(obs.observedAction);
        }
        
        // Learn social rules from observed patterns
        brain_->getConceptFormation()->learnSocialRules(socialContexts, observedActions);
        
        // Update social learning influence based on observation quality
        float observationQuality = calculateSocialLearningQuality();
        socialLearningInfluence_ = std::min(1.0f, socialLearningInfluence_ + observationQuality * 0.01f);
    }
}

void AgentBrain::updateAttentionWithCognitiveContext(float novelty, float predictionError) {
    // Update attention based on cognitive context and prediction error
    // This affects which sensory inputs and neural activities receive priority
    
    // Novelty increases attention to new stimuli
    float noveltyAttention = novelty * 0.4f;
    
    // Prediction error signals learning opportunities
    float predictionAttention = std::abs(predictionError) * 0.3f;
    
    // Cognitive load and planning needs
    float cognitiveLoad = (1.0f - planConfidence_) * 0.2f;
    
    // Combine attention factors
    attentionFocus_ = noveltyAttention + predictionAttention + cognitiveLoad;
    attentionFocus_ = std::clamp(attentionFocus_, 0.1f, 1.0f);
    
    // Update prediction accuracy based on prediction error
    if (std::abs(predictionError) < 0.1f) {
        predictionAccuracy_ = std::min(1.0f, predictionAccuracy_ + 0.05f);
    } else {
        predictionAccuracy_ = std::max(0.0f, predictionAccuracy_ - 0.05f);
    }
}

void AgentBrain::processPredictionErrorForLearning(float predictionError) {
    // Process prediction error for cognitive system learning
    // This integrates with the brain's prediction system
    
    if (!brain_ || !brain_->getPredictionSystem()) {
        return;
    }
    
    // Update prediction system with new prediction error
    brain_->getPredictionSystem()->updatePredictionError(predictionError);
    
    // Affect concept formation stability
    if (conceptFormationEnabled_) {
        float learningRate = std::abs(predictionError) * 0.01f;
        for (float& stability : conceptStability_) {
            stability = std::max(0.0f, stability - learningRate);
        }
    }
}

void AgentBrain::storeExperienceForConceptFormation(const SensoryPercept& percept, MotorCommand action, float reward) {
    // Store experience for later concept formation and learning
    
    Experience exp;
    exp.percept = percept.getAllSignals();
    exp.action = action;
    exp.reward = reward;
    exp.timestamp = developmentalAge_;
    
    experienceBuffer_.push_back(exp);
    
    // Maintain experience buffer size
    if (experienceBuffer_.size() > 1000) {
        experienceBuffer_.erase(experienceBuffer_.begin());
    }
}

void AgentBrain::updateSelfModelWithSensoryInput(const SensoryPercept& percept) {
    // Update self-model representation with new sensory input
    // This helps maintain body awareness and action consequence prediction
    
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < vision.size() && i < bodySchema_.size(); ++i) {
        // Update body schema with new sensory information
        bodySchema_[i] = bodySchema_[i] * 0.9f + vision[i] * 0.1f;
    }
}

void AgentBrain::observeOtherAgent(const AgentBrain& otherAgent) {
    // Observe another agent's behavior for social learning
    // This helps learn social rules and norms
    
    SocialObservation obs;
    obs.observerPercept = previousVision_;
    obs.observedAction = otherAgent.decodeMotorCommand();
    obs.socialContext = std::vector<float>(6, 0.0f);  // Placeholder for social context
    obs.observationTime = developmentalAge_;
    
    socialObservations_.push_back(obs);
    
    // Limit social observations
    if (socialObservations_.size() > 500) {
        socialObservations_.erase(socialObservations_.begin());
    }
}

float AgentBrain::calculateNovelty(const std::vector<float>& vision) const {
    // Calculate novelty based on difference from previous vision
    if (previousVision_.empty() || vision.empty()) {
        return 0.5f;
    }
    
    float totalDifference = 0.0f;
    size_t minSize = std::min(previousVision_.size(), vision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDifference += std::abs(vision[i] - previousVision_[i]);
    }
    
    float avgDifference = totalDifference / static_cast<float>(minSize);
    return std::min(1.0f, avgDifference * 2.0f);
}

float AgentBrain::calculateConceptStability() const {
    // Calculate concept stability based on experience consistency
    float stability = 0.0f;
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < experienceBuffer_.size(); ++i) {
        // Weight recent experiences more heavily
        float weight = 1.0f + (static_cast<float>(i) / static_cast<float>(experienceBuffer_.size()));
        float consistency = 0.0f;
        
        // Check consistency of this experience with others
        for (size_t j = i + 1; j < experienceBuffer_.size() && j - i < 10; ++j) {
            float similarity = calculatePerceptSimilarity(experienceBuffer_[i].percept, 
                                                         experienceBuffer_[j].percept);
            consistency += similarity;
        }
        
        stability += consistency * weight;
        totalWeight += weight;
    }
    
    return totalWeight > 0.0f ? stability / totalWeight : 0.0f;
}

float AgentBrain::calculateSocialLearningQuality() const {
    // Calculate quality of social learning based on observation consistency
    if (socialObservations_.empty()) {
        return 0.0f;
    }
    
    float totalConsistency = 0.0f;
    for (size_t i = 0; i < socialObservations_.size(); ++i) {
        for (size_t j = i + 1; j < socialObservations_.size() && j - i < 5; ++j) {
            // Calculate similarity of observed behaviors
            float actionSimilarity = calculateActionSimilarity(socialObservations_[i].observedAction,
                                                             socialObservations_[j].observedAction);
            float contextSimilarity = calculateContextSimilarity(socialObservations_[i].socialContext,
                                                               socialObservations_[j].socialContext);
            totalConsistency += (actionSimilarity + contextSimilarity) * 0.5f;
        }
    }
    
    return totalConsistency / (socialObservations_.size() * 2.0f);
}

float AgentBrain::calculatePerceptSimilarity(const std::vector<float>& percept1, const std::vector<float>& percept2) const {
    // Calculate similarity between two percept vectors
    if (percept1.empty() || percept2.empty()) {
        return 0.0f;
    }
    
    float totalDifference = 0.0f;
    size_t minSize = std::min(percept1.size(), percept2.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDifference += std::abs(percept1[i] - percept2[i]);
    }
    
    float avgDifference = totalDifference / static_cast<float>(minSize);
    return std::max(0.0f, 1.0f - avgDifference);
}

float AgentBrain::calculateActionSimilarity(MotorCommand action1, MotorCommand action2) const {
    // Calculate similarity between two motor commands
    // Simple mapping: same actions are identical, different actions are dissimilar
    return (action1 == action2) ? 1.0f : 0.0f;
}

float AgentBrain::calculateContextSimilarity(const std::vector<float>& context1, const std::vector<float>& context2) const {
    // Calculate similarity between two context vectors
    if (context1.empty() || context2.empty()) {
        return 0.0f;
    }
    
    float totalDifference = 0.0f;
    size_t minSize = std::min(context1.size(), context2.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDifference += std::abs(context1[i] - context2[i]);
    }
    
    float avgDifference = totalDifference / static_cast<float>(minSize);
    return std::max(0.0f, 1.0f - avgDifference);
}

// Implementation of advanced cognitive system getter methods
float AgentBrain::getPlanConfidence() const {
    return planConfidence_;
}

float AgentBrain::getSelfModelAwareness() const {
    return selfModelAwareness_;
}

float AgentBrain::getSocialLearningInfluence() const {
    return socialLearningInfluence_;
}

AgentBrain::CognitiveMetrics AgentBrain::getCognitiveMetrics() const {
    CognitiveMetrics metrics;
    metrics.planConfidence = planConfidence_;
    metrics.conceptStability = conceptFormationEnabled_ ? calculateConceptStability() : 0.0f;
    metrics.selfModelAwareness = selfModelAwareness_;
    metrics.socialLearningInfluence = socialLearningInfluence_;
    metrics.attentionFocus = attentionFocus_;
    metrics.predictionAccuracy = predictionAccuracy_;
    return metrics;
}

} // namespace nlm
