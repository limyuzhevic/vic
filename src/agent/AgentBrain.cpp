// ========== ADVANCED AGENT CAPABILITIES IMPLEMENTATION ==========

// Enhanced sensory processing
std::vector<float> AgentBrain::extractSensoryFeatures(const SensoryPercept& percept) const {
    std::vector<float> features;
    
    // Vision features: statistics, edges, patterns
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float sum = std::accumulate(vision.begin(), vision.end(), 0.0f);
        float mean = sum / vision.size();
        float variance = 0.0f;
        for (float val : vision) {
            variance += (val - mean) * (val - mean);
        }
        variance /= vision.size();
        float stddev = std::sqrt(variance);
        
        features.push_back(mean);
        features.push_back(stddev);
        features.push_back(*std::max_element(vision.begin(), vision.end()));  // maximum
        features.push_back(*std::min_element(vision.begin(), vision.end()));  // minimum
        
        // Add edge information (simple approximation)
        int edgeCount = 0;
        for (size_t i = 1; i < vision.size() - 1; ++i) {
            if (std::abs(vision[i] - vision[i-1]) > 0.1f || std::abs(vision[i] - vision[i+1]) > 0.1f) {
                edgeCount++;
            }
        }
        features.push_back(static_cast<float>(edgeCount) / vision.size());
    }
    
    // Touch features: collision statistics
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        float touchMean = std::accumulate(touch.begin(), touch.end(), 0.0f) / touch.size();
        features.push_back(touchMean);
        features.push_back(*std::max_element(touch.begin(), touch.end()));  // strongest impact
    }
    
    // Internal signals features
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        float internMean = std::accumulate(intern.begin(), intern.end(), 0.0f) / intern.size();
        features.push_back(internMean);
    }
    
    // Proprioception features
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty()) {
        float proprioMean = std::accumulate(proprio.begin(), proprio.end(), 0.0f) / proprio.size();
        features.push_back(proprioMean);
    }
    
    // Novelty and curiosity features
    features.push_back(noveltyLevel_);
    features.push_back(curiosityLevel_);
    features.push_back(predictionError_);
    
    return features;
}

std::vector<float> AgentBrain::processSensoryHistory(const std::vector<SensoryPercept>& history) const {
    std::vector<float> features;
    
    if (history.empty()) return features;
    
    // Temporal statistics
    size_t historySize = history.size();
    
    // Vision temporal statistics
    std::vector<float> visionMeans;
    std::vector<float> visionChanges;
    
    for (size_t i = 1; i < historySize; ++i) {
        const auto& currentVision = history[i].getVision();
        const auto& prevVision = history[i-1].getVision();
        
        if (!currentVision.empty() && !prevVision.empty()) {
            float currentMean = std::accumulate(currentVision.begin(), currentVision.end(), 0.0f) / currentVision.size();
            float prevMean = std::accumulate(prevVision.begin(), prevVision.end(), 0.0f) / prevVision.size();
            
            visionMeans.push_back(currentMean);
            visionChanges.push_back(currentMean - prevMean);
        }
    }
    
    // Temporal derivatives and trends
    if (!visionChanges.empty()) {
        float totalChange = std::accumulate(visionChanges.begin(), visionChanges.end(), 0.0f);
        float avgChange = totalChange / visionChanges.size();
        
        features.push_back(avgChange);  // Average change rate
        
        float changeVariance = 0.0f;
        for (float change : visionChanges) {
            changeVariance += (change - avgChange) * (change - avgChange);
        }
        changeVariance /= visionChanges.size();
        features.push_back(std::sqrt(changeVariance));  // Change volatility
    }
    
    // Novelty history
    std::vector<float> noveltyHistory;
    for (const auto& percept : history) {
        // Simple novelty calculation (would need proper novelty detector)
        noveltyHistory.push_back(percept.getInternal().empty() ? 0.0f : 
                              percept.getInternal()[0]);
    }
    
    if (!noveltyHistory.empty()) {
        float avgNovelty = std::accumulate(noveltyHistory.begin(), noveltyHistory.end(), 0.0f) / noveltyHistory.size();
        features.push_back(avgNovelty);
    }
    
    return features;
}

float AgentBrain::assessSensoryQuality(const SensoryPercept& percept) const {
    float quality = 0.0f;
    
    // Vision quality (based on variance and signal strength)
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float mean = std::accumulate(vision.begin(), vision.end(), 0.0f) / vision.size();
        float variance = 0.0f;
        for (float val : vision) {
            variance += (val - mean) * (val - mean);
        }
        variance /= vision.size();
        
        quality += std::sqrt(variance);  // Higher variance = more informative
        quality += std::max(0.0f, mean);  // Higher mean = stronger signal
    }
    
    // Touch quality
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        float touchStrength = *std::max_element(touch.begin(), touch.end());
        quality += touchStrength * 2.0f;
    }
    
    // Internal signals quality
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        float internStrength = std::sqrt(std::accumulate(intern.begin(), intern.end(), 0.0f,
                                                       [](float sum, float val) { return sum + val * val; }) / intern.size());
        quality += internStrength;
    }
    
    return std::min(1.0f, quality);
}

// Multi-modal sensory fusion
std::vector<float> AgentBrain::fuseSensoryModality(const std::vector<float>& visionFeatures,
                                                  const std::vector<float>& touchFeatures,
                                                  const std::vector<float>& internalFeatures,
                                                  const std::vector<float>& proprioceptionFeatures) const {
    std::vector<float> fused;
    
    // Simple weighted fusion based on modality reliability
    // Vision typically has highest weight, touch medium, internal low
    float visionWeight = 0.5f;
    float touchWeight = 0.3f;
    float internalWeight = 0.15f;
    float proprioceptionWeight = 0.05f;
    
    // Check if features are present before weighting
    float totalWeight = 0.0f;
    if (!visionFeatures.empty()) { totalWeight += visionWeight; }
    if (!touchFeatures.empty()) { totalWeight += touchWeight; }
    if (!internalFeatures.empty()) { totalWeight += internalWeight; }
    if (!proprioceptionFeatures.empty()) { totalWeight += proprioceptionWeight; }
    
    if (totalWeight < 0.01f) return fused;
    
    // Normalize weights
    visionWeight /= totalWeight;
    touchWeight /= totalWeight;
    internalWeight /= totalWeight;
    proprioceptionWeight /= totalWeight;
    
    // Fuse corresponding features
    size_t maxFeatures = std::max({visionFeatures.size(), touchFeatures.size(),
                                  internalFeatures.size(), proprioceptionFeatures.size()});
    
    for (size_t i = 0; i < maxFeatures; ++i) {
        float fusedValue = 0.0f;
        float weightSum = 0.0f;
        
        if (i < visionFeatures.size()) {
            fusedValue += visionFeatures[i] * visionWeight;
            weightSum += visionWeight;
        }
        if (i < touchFeatures.size()) {
            fusedValue += touchFeatures[i] * touchWeight;
            weightSum += touchWeight;
        }
        if (i < internalFeatures.size()) {
            fusedValue += internalFeatures[i] * internalWeight;
            weightSum += internalWeight;
        }
        if (i < proprioceptionFeatures.size()) {
            fusedValue += proprioceptionFeatures[i] * proprioceptionWeight;
            weightSum += proprioceptionWeight;
        }
        
        if (weightSum > 0.0f) {
            fused.push_back(fusedValue / weightSum);
        }
    }
    
    return fused;
}

// Policy-based action selection
MotorCommand AgentBrain::selectAction(const std::vector<float>& state, bool useExploration) {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand selectedAction;
    
    if (actionSelectionMethod_ == "reinforcement") {
        selectedAction = applyReinforcementPolicy(0.0f, 0.0f, state);
    } else if (actionSelectionMethod_ == "rule_based") {
        selectedAction = applyRuleBasedPolicy(state);
    } else {
        selectedAction = decodeFromMotorNeurons();
    }
    
    // Apply exploration/exploitation balance
    if (useExploration) {
        selectedAction = balanceExplorationExploitation(state, explorationRate_);
    }
    
    // Track action statistics
    totalActionsTaken_++;
    if (selectedAction != MotorCommand::Wait) {  // Consider wait as unsuccessful
        successfulActions_++;
    }
    updatePerformanceCounters(successfulActions_ > 0);
    
    return selectedAction;
}

MotorCommand AgentBrain::applyReinforcementPolicy(float reward, float predictedReward, const std::vector<float>& state) {
    // Simple reinforcement learning: prefer actions with higher estimated value
    float bestValue = -1000.0f;
    MotorCommand bestAction = MotorCommand::Wait;
    
    // Check each action's estimated value
    for (int i = 0; i < 6; ++i) {
        MotorCommand cmd;
        switch (i) {
            case 0: cmd = MotorCommand::MoveForward; break;
            case 1: cmd = MotorCommand::MoveBackward; break;
            case 2: cmd = MotorCommand::TurnLeft; break;
            case 3: cmd = MotorCommand::TurnRight; break;
            case 4: cmd = MotorCommand::LookLeft; break;
            case 5: cmd = MotorCommand::LookRight; break;
            default: continue;
        }
        
        float value = getActionValue(cmd, state);
        if (value > bestValue) {
            bestValue = value;
            bestAction = cmd;
        }
    }
    
    // Update action value with reward
    if (bestAction != MotorCommand::Wait) {
        float updatedValue = bestValue + learningRate_ * (reward - bestValue);
        actionValues_[bestAction].push_back(updatedValue);
        // Keep only recent values
        if (actionValues_[bestAction].size() > 100) {
            actionValues_[bestAction].erase(actionValues_[bestAction].begin());
        }
    }
    
    return bestAction;
}

MotorCommand AgentBrain::applyRuleBasedPolicy(const std::vector<float>& state) const {
    // Simple rule-based system based on state features
    if (state.size() < 8) return decodeFromMotorNeurons();
    
    // Rule 1: If goal is set, move towards it
    if (goalState_.size() == state.size()) {
        float goalProgress = getGoalProgress(state);
        if (goalProgress > 0.8f) return MotorCommand::Wait;  // Goal nearly reached
        if (goalProgress > 0.5f) return MotorCommand::Wait;  // Goal in sight
    }
    
    // Rule 2: Avoid high novelty (change)
    if (state[7] > 0.7f) {  // novelty feature
        return MotorCommand::Wait;  // Stay still when novelty is high
    }
    
    // Rule 3: Approach positive internal signals
    if (state[3] > 0.5f) {  // internal feature
        return MotorCommand::MoveForward;  // Move towards positive internal state
    }
    
    // Default to motor neuron activity
    return decodeFromMotorNeurons();
}

// Behavior planning and sequencing
std::vector<MotorCommand> AgentBrain::planBehaviorSequence(const std::vector<float>& goalState) const {
    std::vector<MotorCommand> sequence;
    
    if (goalState.empty()) return sequence;
    
    // Simple behavior planning based on goal
    // This would be more sophisticated in a real implementation
    
    // Check if goal requires movement
    bool requiresMovement = false;
    for (size_t i = 0; i < std::min(goalState.size(), state_.size()); ++i) {
        if (std::abs(goalState[i] - state_[i]) > 0.1f) {
            requiresMovement = true;
            break;
        }
    }
    
    if (requiresMovement) {
        // Plan a simple sequence: approach -> achieve -> maintain
        sequence.push_back(MotorCommand::MoveForward);
        sequence.push_back(MotorCommand::MoveForward);
        sequence.push_back(MotorCommand::Wait);
    } else {
        // Goal already reached
        sequence.push_back(MotorCommand::Wait);
    }
    
    return sequence;
}

MotorCommand AgentBrain::getNextActionInSequence(const std::vector<MotorCommand>& sequence, size_t stepIndex) const {
    if (stepIndex >= sequence.size()) {
        return MotorCommand::Wait;  // No more actions in sequence
    }
    return sequence[stepIndex];
}

bool AgentBrain::shouldContinueSequence(const std::vector<MotorCommand>& sequence, size_t currentStep) const {
    // Continue sequence unless we've reached the end or the action was invalid
    return currentStep < sequence.size();
}

// Advanced curiosity and exploration
float AgentBrain::computeCuriosityDrive(const SensoryPercept& currentPercept) const {
    float novelty = 0.0f;
    const auto& vision = currentPercept.getVision();
    
    if (!vision.empty() && !previousVision_.empty()) {
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            novelty += std::abs(vision[i] - previousVision_[i]);
        }
        novelty /= std::max<size_t>(vision.size(), 1);
    }
    
    // Multiplicative combination of sources
    float curiosity = novelty * 2.0f + std::abs(predictionError_) * 0.5f;
    return std::clamp(curiosity, 0.0f, 1.0f);
}

MotorCommand AgentBrain::applyCuriosityDrivenExploration(const std::vector<float>& state) {
    // Compute curiosity based on current state and history
    float curiosity = computeCuriosityDrive(SensoryPercept());  // Would need current percept
    
    // High curiosity = more exploration
    if (curiosity > 0.5f) {
        float exploreChance = curiosity * 0.4f;  // Up to 40% random action
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random exploration action
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    // Fall back to normal action selection
    return decodeFromMotorNeurons();
}

MotorCommand AgentBrain::balanceExplorationExploitation(const std::vector<float>& state, float explorationRate) {
    // Simple epsilon-greedy exploration
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    if (r < explorationRate) {
        // Explore: choose random action
        int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            default: return MotorCommand::Wait;
        }
    } else {
        // Exploit: use learned policy
        return applyReinforcementPolicy(0.0f, 0.0f, state);
    }
}

// Sensorimotor integration
std::vector<float> AgentBrain::predictSensoryOutcome(const MotorCommand& action) const {
    std::vector<float> prediction;
    
    // Simple forward model: estimate sensory consequences of actions
    // This would be learned in a real implementation
    
    // Default prediction based on action type
    switch (action) {
        case MotorCommand::MoveForward:
            prediction = {0.1f, 0.0f, 0.0f, 0.0f};  // Slightly positive internal
            break;
        case MotorCommand::TurnLeft:
            prediction = {-0.1f, 0.0f, 0.0f, 0.0f};  // Negative internal (orientation change)
            break;
        case MotorCommand::MoveBackward:
            prediction = {-0.1f, 0.0f, 0.0f, 0.0f};  // Negative internal
            break;
        case MotorCommand::TurnRight:
            prediction = {0.1f, 0.0f, 0.0f, 0.0f};   // Positive internal
            break;
        default:
            prediction = {0.0f, 0.0f, 0.0f, 0.0f};   // No change
            break;
    }
    
    return prediction;
}

float AgentBrain::computeSensoryPredictionError(const SensoryPercept& actual, const SensoryPercept& predicted) const {
    float error = 0.0f;
    
    // Compare vision features
    const auto& actualVision = actual.getVision();
    const auto& predictedVision = predicted.getVision();
    
    if (!actualVision.empty() && !predictedVision.empty()) {
        for (size_t i = 0; i < actualVision.size() && i < predictedVision.size(); ++i) {
            error += std::abs(actualVision[i] - predictedVision[i]);
        }
        error /= std::max<size_t>(actualVision.size(), 1);
    }
    
    return error;
}

void AgentBrain::updateSensorimotorMapping(const MotorCommand& action, const SensoryPercept& outcome) {
    // Update forward model based on actual outcomes
    // In a real implementation, this would be a proper learning mechanism
    
    float error = computeSensoryPredictionError(SensoryPercept(), outcome);  // Would need predicted percept
    
    // Simple weight update for sensorimotor mapping
    // This would involve neural adaptation in a real system
    if (error > 0.01f) {
        NLM_LOG_INFO("Sensorimotor prediction error: " + std::to_string(error));
        // Adapt mapping based on error
    }
}

// Goal-based behavior
void AgentBrain::setGoal(const std::vector<float>& goalState) {
    goalState_ = goalState;
    NLM_LOG_INFO("Agent goal set with " + std::to_string(goalState_.size()) + " target values");
}

void AgentBrain::clearGoal() {
    goalState_.clear();
}

float AgentBrain::getGoalProgress(const std::vector<float>& currentState) const {
    if (goalState_.empty() || goalState_.size() != currentState.size()) {
        return 0.0f;
    }
    
    float totalProgress = 0.0f;
    for (size_t i = 0; i < goalState_.size(); ++i) {
        float current = currentState[i];
        float goal = goalState_[i];
        
        // Progress towards goal: 0 when far, 1 when close
        float progress = 1.0f - std::min(1.0f, std::abs(current - goal) * 5.0f);
        totalProgress += progress;
    }
    
    return totalProgress / goalState_.size();
}

// Social learning capabilities
void AgentBrain::learnFromObservation(const std::vector<float>& observerState, const MotorCommand& observedAction) {
    if (!socialLearningEnabled_) return;
    
    // Update social action weights
    socialActionWeights_[observedAction].push_back(1.0f);  // Positive weight for observed action
    
    // Decay less common social actions
    for (auto& pair : socialActionWeights_) {
        if (pair.first != observedAction) {
            for (float& weight : pair.second) {
                weight *= 0.95f;  // Decay
            }
        }
    }
}

std::vector<MotorCommand> AgentBrain::getSociallyInformedActions(const std::vector<float>& socialContext) const {
    std::vector<MotorCommand> actions;
    
    if (socialActionWeights_.empty()) {
        return actions;
    }
    
    // Choose actions with highest social weights
    struct ActionWeight {
        MotorCommand action;
        float weight;
    };
    
    std::vector<ActionWeight> weightedActions;
    for (const auto& pair : socialActionWeights_) {
        if (!pair.second.empty()) {
            float avgWeight = std::accumulate(pair.second.begin(), pair.second.end(), 0.0f) / pair.second.size();
            weightedActions.push_back({pair.first, avgWeight});
        }
    }
    
    // Sort by weight (descending)
    std::sort(weightedActions.begin(), weightedActions.end(),
              [](const ActionWeight& a, const ActionWeight& b) {
                  return a.weight > b.weight;
              });
    
    // Select top 2 actions for social learning
    for (size_t i = 0; i < std::min(size_t(2), weightedActions.size()); ++i) {
        actions.push_back(weightedActions[i].action);
    }
    
    return actions;
}

// Advanced agent state and monitoring
std::vector<float> AgentBrain::getCurrentAgentState() const {
    std::vector<float> state;
    
    // Combine various state components
    state.reserve(20);  // Reserve space for all state features
    
    // Current state (if stored)
    for (float val : state_) {
        state.push_back(val);
    }
    
    // Neuromodulation state
    state.push_back(dopamineLevel_);
    state.push_back(curiosityLevel_);
    state.push_back(noveltyLevel_);
    state.push_back(predictionError_);
    
    // Development state
    state.push_back(developmentalAge_);
    state.push_back(plasticityModifier_);
    
    // Goal state
    for (float val : goalState_) {
        state.push_back(val);
    }
    
    // Performance metrics
    state.push_back(actionSuccessRate_);
    state.push_back(explorationRatio_);
    
    return state;
}

void AgentBrain::setAgentState(const std::vector<float>& state) {
    state_ = state;
    NLM_LOG_INFO("Agent state set with " + std::to_string(state_.size()) + " values");
}

float AgentBrain::getActionValue(const MotorCommand& action, const std::vector<float>& state) const {
    // Get stored value for this action
    auto it = actionValues_.find(action);
    if (it != actionValues_.end() && !it->second.empty()) {
        // Return average of recent values
        float sum = std::accumulate(it->second.begin(), it->second.end(), 0.0f);
        return sum / it->second.size();
    }
    return 0.0f;  // Default value
}

void AgentBrain::updateActionValues(const MotorCommand& action, float value, const std::vector<float>& state) {
    actionValues_[action].push_back(value);
    
    // Keep only recent values
    if (actionValues_[action].size() > 100) {
        actionValues_[action].erase(actionValues_[action].begin());
    }
}

// Performance monitoring
void AgentBrain::updatePerformanceCounters(bool success) {
    if (!performanceMonitoringEnabled_) return;
    
    successfulActions_ += success ? 1 : 0;
    totalActionsTaken_++;
    
    if (totalActionsTaken_ > 0) {
        actionSuccessRate_ = static_cast<float>(successfulActions_) / totalActionsTaken_;
    }
    
    // Update exploration ratio
    // This would be calculated from exploration vs exploitation actions
    explorationRatio_ = curiosityLevel_;  // Simple approximation
    
    // Log performance metrics periodically
    if (totalActionsTaken_ % 100 == 0) {
        NLM_LOG_INFO("Agent performance: " + std::to_string(actionSuccessRate_ * 100.0f) + "% success rate, " +
                     std::to_string(explorationRatio_ * 100.0f) + "% exploration");
    }
}

// Configuration groups
void AgentBrain::addSensorGroup(const std::string& groupName) {
    sensorGroups_.insert(groupName);
}

void AgentBrain::removeSensorGroup(const std::string& groupName) {
    sensorGroups_.erase(groupName);
}

bool AgentBrain::hasSensorGroup(const std::string& groupName) const {
    return sensorGroups_.find(groupName) != sensorGroups_.end();
}

// Episodic memory for agent behavior
void AgentBrain::storeEpisode(const std::vector<float>& state, const MotorCommand& action, 
                             float reward, const std::vector<float>& nextState) {
    episodeMemory_.emplace_back(state, action, reward, nextState);
    
    // Keep only recent episodes
    if (episodeMemory_.size() > 100) {
        episodeMemory_.erase(episodeMemory_.begin());
    }
}

std::vector<std::tuple<std::vector<float>, MotorCommand, float, std::vector<float>>> AgentBrain::getRecentEpisodes(size_t count) const {
    if (count > episodeMemory_.size()) {
        count = episodeMemory_.size();
    }
    
    std::vector<std::tuple<std::vector<float>, MotorCommand, float, std::vector<float>>> result;
    result.reserve(count);
    
    // Copy most recent episodes
    auto start = episodeMemory_.end() - count;
    for (auto it = start; it != episodeMemory_.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

// Helper methods
std::vector<float> AgentBrain::encodeSensoryPercept(const SensoryPercept& percept) const {
    std::vector<float> encoded;
    
    // Simple encoding: flatten all sensory data
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Vision (256 values)
    encoded.reserve(256 + 8 + 4 + 6);
    for (float val : vision) encoded.push_back(val);
    
    // Touch (8 values)
    for (float val : touch) encoded.push_back(val);
    
    // Internal (4 values)
    for (float val : intern) encoded.push_back(val);
    
    // Proprioception (6 values)
    for (float val : proprio) encoded.push_back(val);
    
    return encoded;
}

MotorCommand AgentBrain::decodeMotorFromActivity(const std::vector<float>& motorActivity) const {
    if (motorActivity.size() < 6) return MotorCommand::Wait;
    
    // Find maximum activity
    float maxActivity = 0.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (int i = 0; i < 6; ++i) {
        float activity = motorActivity[i];
        if (activity > maxActivity) {
            maxActivity = activity;
            switch (i) {
                case 0: bestCommand = MotorCommand::MoveForward; break;
                case 1: bestCommand = MotorCommand::MoveBackward; break;
                case 2: bestCommand = MotorCommand::TurnLeft; break;
                case 3: bestCommand = MotorCommand::TurnRight; break;
                case 4: bestCommand = MotorCommand::LookLeft; break;
                case 5: bestCommand = MotorCommand::LookRight; break;
            }
        }
    }
    
    // Only act if there's meaningful activity
    if (maxActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return bestCommand;
}

float AgentBrain::calculateExplorationBonus(const std::vector<float>& state) const {
    // Calculate bonus for exploring novel states
    float noveltyBonus = 0.0f;
    
    if (!state.empty()) {
        // Simple novelty calculation based on state deviation
        float stateMean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
        float stateVariance = 0.0f;
        for (float val : state) {
            stateVariance += (val - stateMean) * (val - stateMean);
        }
        stateVariance /= state.size();
        
        noveltyBonus = std::sqrt(stateVariance) * curiosityLevel_;
    }
    
    return noveltyBonus;
}

// Health and robustness
bool AgentBrain::isHealthy() const {
    // Check various health metrics
    bool healthy = true;
    
    // Check for excessive inactivity
    if (totalActionsTaken_ > 100 && actionSuccessRate_ < 0.1f) {
        healthy = false;  // Too many failed actions
    }
    
    // Check for excessive exploration
    if (explorationRatio_ > 0.9f) {
        healthy = false;  // Always exploring, never exploiting
    }
    
    // Check for critical curiosity
    if (curiosityLevel_ > 0.95f) {
        healthy = false;  // Too curious, might be distracted
    }
    
    return healthy;
}

std::string AgentBrain::getHealthStatus() const {
    std::stringstream status;
    
    status << "Agent Health Status:" << std::endl;
    status << "  Actions taken: " << totalActionsTaken_ << std::endl;
    status << "  Success rate: " << (actionSuccessRate_ * 100.0f) << "%" << std::endl;
    status << "  Exploration ratio: " << (explorationRatio_ * 100.0f) << "%" << std::endl;
    status << "  Curiosity level: " << (curiosityLevel_ * 100.0f) << "%" << std::endl;
    status << "  Neuromodulation: dopamine=" << (dopamineLevel_ * 100.0f) << "%, " 
              << "novelty=" << (noveltyLevel_ * 100.0f) << "%" << std::endl;
    status << "  Healthy: " << (isHealthy() ? "Yes" : "No") << std::endl;
    
    return status.str();
}

void AgentBrain::resetPerformanceCounters() {
    totalActionsTaken_ = 0;
    successfulActions_ = 0;
    actionSuccessRate_ = 0.0f;
    explorationRatio_ = 0.0f;
}

} // namespace nlm