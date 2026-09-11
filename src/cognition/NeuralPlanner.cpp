# Improved NeuralPlanner implementation with better action selection
struct NeuralPlanner::Impl {
    Brain* brain;
    
    // Track preferred actions based on experience
    std::array<float, 10> actionPreferences;  // 10 action types
    
    // Track recent plan success rates
    std::deque<bool> recentPlanSuccess;
    
    // Track current planning state
    bool isPlanning;
    ActionType currentPlan;
    
    Impl() : brain(nullptr), isPlanning(false) {
        actionPreferences.fill(0.0f);
        recentPlanSuccess.clear();
    }
};

NeuralPlanner::NeuralPlanner()
    : pImpl(new Impl)
    , brain_(nullptr)
    , planningDepth_(3)
    , planningConfidence_(0.5f)
    , currentGoal_()
{
    actionQuality_.resize(10, 0.0f);  // 10 action types
    recentPlanSuccess_.clear();
}

NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->isPlanning = false;
    NLM_LOG_INFO("NeuralPlanner initialized with action preferences tracking");
}

ActionType NeuralPlanner::planAction(const std::vector<float>& currentState,
                                    float targetReward) {
    pImpl->isPlanning = true;
    
    // Generate possible action sequences
    auto sequences = generateActionSequences(planningDepth_);
    
    PlanningCandidate best;
    best.expectedReward = -1000.0f;
    
    // Evaluate each sequence with neural-aware evaluation
    for (const auto& sequence : sequences) {
        PlanningCandidate candidate = evaluateSequence(sequence, currentState);
        
        // Apply action preferences (Hebbian-like learning)
        float preferenceBonus = 0.0f;
        for (ActionType action : sequence) {
            size_t actionIdx = static_cast<size_t>(action);
            if (actionIdx < pImpl->actionPreferences.size()) {
                preferenceBonus += pImpl->actionPreferences[actionIdx];
            }
            size_t qIdx = static_cast<size_t>(action);
            if (qIdx < actionQuality_.size()) {
                preferenceBonus += actionQuality_[qIdx] * 0.5f;
            }
        }
        
        candidate.expectedReward += preferenceBonus;
        
        if (candidate.expectedReward > best.expectedReward) {
            best = candidate;
        }
    }
    
    // Update confidence based on consistency
    if (!sequences.empty()) {
        planningConfidence_ = best.confidence;
    }
    
    // Record success/failure based on reward achievement
    if (best.expectedReward >= targetReward) {
        recentPlanSuccess_.push_back(true);
    } else {
        recentPlanSuccess_.push_back(false);
    }
    
    if (recentPlanSuccess_.size() > 10) {
        recentPlanSuccess_.pop_front();
    }
    
    // Store current plan for monitoring
    if (!best.actions.empty()) {
        currentGoal_ = currentState;  // Store current state as goal
        pImpl->currentPlan = best.actions[0];
        
        // Update action preferences for successful actions
        size_t actionIdx = static_cast<size_t>(pImpl->currentPlan);
        if (actionIdx < pImpl->actionPreferences.size()) {
            pImpl->actionPreferences[actionIdx] = 
                std::min(1.0f, pImpl->actionPreferences[actionIdx] + 0.1f);
        }
    }
    
    // Return first action of best sequence
    if (!best.actions.empty()) {
        pImpl->isPlanning = false;
        return best.actions[0];
    }
    
    pImpl->isPlanning = false;
    // Default: wait if no good option
    return ActionType::Wait;
}

PlanningCandidate NeuralPlanner::evaluateSequence(const std::vector<ActionType>& actions,
                                                 const std::vector<float>& startState) {
    PlanningCandidate candidate;
    candidate.actions = actions;
    candidate.depth = actions.size();
    
    std::vector<float> currentState = startState;
    float totalReward = 0.0f;
    float confidence = 1.0f;
    
    for (size_t i = 0; i < actions.size(); ++i) {
        ActionType action = actions[i];
        
        // Evaluate this action using neural state
        float actionValue = evaluateAction(action, currentState);
        
        // Predict next state using SelfModel if available
        std::vector<float> predictedNext = predictNextState(action, currentState);
        
        if (!predictedNext.empty()) {
            candidate.predictedStates.push_back(predictedNext);
            currentState = predictedNext;
        }
        
        // Get action quality from experience with neural modulation
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < actionQuality_.size()) {
            // Combine learned quality with current evaluation
            actionValue = actionQuality_[actionIdx] * 0.6f + actionValue * 0.4f;
        }
        
        // Apply development stage modulation
        if (brain_) {
            DevelopmentalStage stage = brain_->getDevelopmentalStage();
            float devMod = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial: devMod = 1.2f; break;
                case DevelopmentalStage::CriticalPeriod: devMod = 1.0f; break;
                case DevelopmentalStage::Maturation: devMod = 0.8f; break;
                case DevelopmentalStage::Adult: devMod = 0.6f; break;
            }
            actionValue *= devMod;
        }
        
        totalReward += actionValue;
        
        // Reduce confidence if uncertain
        confidence *= getSelfModelConfidence(action);
    }
    
    candidate.expectedReward = totalReward;
    candidate.confidence = confidence;
    
    return candidate;
}

std::vector<std::vector<ActionType>> NeuralPlanner::generateActionSequences(size_t depth) {
    std::vector<std::vector<ActionType>> result;
    
    // Base actions with neural-based weights
    std::vector<ActionType> baseActions = {
        ActionType::MoveForward, ActionType::MoveBackward,
        ActionType::TurnLeft, ActionType::TurnRight,
        ActionType::Interact, ActionType::Wait
    };
    
    // Filter actions based on current neural preferences
    std::vector<ActionType> availableActions;
    for (ActionType action : baseActions) {
        size_t actionIdx = static_cast<size_t>(action);
        if (actionIdx < pImpl->actionPreferences.size() && 
            pImpl->actionPreferences[actionIdx] > 0.1f) {
            availableActions.push_back(action);
        }
    }
    
    // If no preferences set, use all actions
    if (availableActions.empty()) {
        availableActions = baseActions;
    }
    
    // Generate sequences with intelligent pruning
    std::function<void(std::vector<ActionType>&, size_t)> generate = 
        [&](std::vector<ActionType>& current, size_t remaining) {
            if (remaining == 0) {
                result.push_back(current);
                return;
            }
            
            // Prune unlikely sequences based on action preferences
            for (ActionType action : availableActions) {
                size_t actionIdx = static_cast<size_t>(action);
                
                // Skip if action preference is very low
                if (actionIdx < pImpl->actionPreferences.size() && 
                    pImpl->actionPreferences[actionIdx] < 0.05f) {
                    continue;
                }
                
                current.push_back(action);
                generate(current, remaining - 1);
                current.pop_back();
            }
        };
    
    std::vector<ActionType> current;
    generate(current, depth);
    
    // Limit number of sequences for performance
    if (result.size() > 50) {
        result.resize(50);
    }
    
    return result;
}

float NeuralPlanner::evaluateAction(ActionType action, const std::vector<float>& state) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionQuality_.size()) return 0.0f;
    
    // Base value from neural experience
    float baseValue = actionQuality_[actionIdx];
    
    // Bonus for approaching goal (if goal is defined)
    float goalBonus = 0.0f;
    if (!currentGoal_.empty() && state.size() == currentGoal_.size()) {
        float stateGoalSim = 0.0f, stateNorm = 0.0f, goalNorm = 0.0f;
        for (size_t i = 0; i < state.size(); ++i) {
            stateGoalSim += state[i] * currentGoal_[i];
            stateNorm += state[i] * state[i];
            goalNorm += currentGoal_[i] * currentGoal_[i];
        }
        if (stateNorm > 0.0001f && goalNorm > 0.0001f) {
            goalBonus = stateGoalSim / (std::sqrt(stateNorm) * std::sqrt(goalNorm));
        }
    }
    
    // Apply development modulation
    float devMod = 1.0f;
    if (brain_) {
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        switch (stage) {
            case DevelopmentalStage::Initial: devMod = 1.2f; break;
            case DevelopmentalStage::CriticalPeriod: devMod = 1.0f; break;
            case DevelopmentalStage::Maturation: devMod = 0.9f; break;
            case DevelopmentalStage::Adult: devMod = 0.8f; break;
        }
    }
    
    return (baseValue * 0.7f + goalBonus * 0.3f) * devMod;
}

std::vector<float> NeuralPlanner::predictNextState(ActionType action,
                                                  const std::vector<float>& currentState) {
    // Use more sophisticated prediction with neural mechanisms
    std::vector<float> nextState = currentState;
    
    // Apply expected effect based on action with neural dynamics
    // This is a simplified model - real implementation would use experience
    
    switch (action) {
        case ActionType::MoveForward:
            // Shift visual field forward with neural decay
            if (nextState.size() > 10) {
                for (size_t i = 10; i < nextState.size(); ++i) {
                    nextState[i-10] = nextState[i] * 0.95f;
                }
                // Add some noise for exploration
                for (size_t i = 0; i < 10; ++i) {
                    nextState[i] += (brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f)) * 
                                    (1.0f - pImpl->actionPreferences[static_cast<size_t>(ActionType::MoveForward)]);
                }
            }
            break;
        case ActionType::MoveBackward:
            if (nextState.size() > 10) {
                for (size_t i = nextState.size() - 1; i >= 10; --i) {
                    nextState[i] = nextState[i-10] * 0.95f;
                }
            }
            break;
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Rotate pattern more realistically
            float rotation = (action == ActionType::TurnLeft) ? -0.1f : 0.1f;
            // Apply rotation with neural gain modulation
            for (auto& v : nextState) {
                v += rotation * (1.0f + brain_->getRandomGenerator()->uniformReal(-0.2f, 0.2f));
                v = std::clamp(v, 0.0f, 1.0f);
            }
            break;
        case ActionType::Interact:
            // Interaction changes state more dramatically
            for (auto& v : nextState) {
                v = v > 0.5f ? 0.9f : 0.1f;
            }
            break;
        case ActionType::Wait:
        default:
            // Apply neural decay and some exploration
            for (auto& v : nextState) {
                v *= 0.99f;
                v += brain_->getRandomGenerator()->uniformReal(-0.01f, 0.01f);
                v = std::clamp(v, 0.0f, 1.0f);
            }
            break;
    }
    
    return nextState;
}

void NeuralPlanner::updatePlanQuality(const std::vector<ActionType>& plannedActions,
                                     const std::vector<ActionType>& actualActions,
                                     float actualReward) {
    // Update action quality based on how well plan worked with neural modulation
    size_t minLen = std::min(plannedActions.size(), actualActions.size());
    
    for (size_t i = 0; i < minLen; ++i) {
        size_t actionIdx = static_cast<size_t>(actualActions[i]);
        if (actionIdx < actionQuality_.size()) {
            // If action matched plan, positive update; else negative
            float delta = (actualActions[i] == plannedActions[i]) ? 0.1f : -0.05f;
            delta *= actualReward;
            
            // Apply development modulation
            if (brain_) {
                DevelopmentalStage stage = brain_->getDevelopmentalStage();
                switch (stage) {
                    case DevelopmentalStage::Initial: delta *= 1.5f; break;
                    case DevelopmentalStage::CriticalPeriod: break;
                    case DevelopmentalStage::Maturation: delta *= 0.8f; break;
                    case DevelopmentalStage::Adult: delta *= 0.6f; break;
                }
            }
            
            actionQuality_[actionIdx] = std::clamp(
                actionQuality_[actionIdx] + delta, -1.0f, 1.0f);
        }
    }
}

void NeuralPlanner::clearCache() {
    // Clear any planning cache and reset preferences
    pImpl->actionPreferences.fill(0.0f);
    pImpl->isPlanning = false;
    currentGoal_.clear();
}

bool NeuralPlanner::wasRecentPlanSuccessful() const {
    if (recentPlanSuccess_.empty()) return true;  // No data
    
    size_t successCount = 0;
    for (bool success : recentPlanSuccess_) {
        if (success) ++successCount;
    }
    
    return successCount > recentPlanSuccess_.size() / 2;
}

// SelfModel Implementation
struct SelfModel::Impl {
    Brain* brain;
    
    // Track action effects with neural representation
    std::array<std::vector<ActionEffect>, 10> actionEffects;  // 10 action types
    
    Impl() : brain(nullptr) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
{
    actionEffects_.resize(10);  // 10 action types
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized with neural representation");
}

void SelfModel::recordSelfAction(ActionType action,
                                 const std::vector<float>& beforeState,
                                 const std::vector<float>& afterState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.observationCount = 1;
    
    // Compute confidence as consistency with previous observations
    float consistency = 0.5f;
    for (const auto& prev : actionEffects_[actionIdx]) {
        if (computeSimilarity(beforeState, prev.beforeState) > 0.8f) {
            consistency = std::max(consistency, 
                computeSimilarity(afterState, prev.afterState));
            effect.observationCount += prev.observationCount;
        }
    }
    effect.confidence = consistency;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Keep memory bounded
    if (actionEffects_[actionIdx].size() > 100) {
        actionEffects_[actionIdx].erase(actionEffects_[actionIdx].begin());
    }
    
    // Update capability level
    float totalConfidence = 0.0f;
    size_t count = 0;
    for (const auto& effects : actionEffects_) {
        for (const auto& e : effects) {
            totalConfidence += e.confidence;
            ++count;
        }
    }
    if (count > 0) {
        capabilityLevel_ = totalConfidence / count;
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return currentState;
    
    return findMatchingEffect(action, currentState);
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return 0.0f;
    
    float totalConf = 0.0f;
    for (const auto& e : effects) {
        totalConf += e.confidence;
    }
    return totalConf / effects.size();
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                            const std::vector<float>& afterState,
                                            ActionType action) const {
    // If we have a good prediction for this action, it's likely self-generated
    auto predicted = findMatchingEffect(action, beforeState);
    
    if (predicted.empty()) return 0.0f;
    
    float similarity = computeSimilarity(predicted, afterState);
    return similarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    ActionType best = ActionType::Wait;
    float bestValue = -1000.0f;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        if (actionEffects_[i].empty()) continue;
        
        // Check how well this action would work in current state
        auto predicted = findMatchingEffect(static_cast<ActionType>(i), state);
        if (!predicted.empty()) {
            // Value = how much the state changes toward reward
            float value = computeSimilarity(predicted, state);
            if (value > bestValue) {
                bestValue = value;
                best = static_cast<ActionType>(i);
            }
        }
    }
    
    return best;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    // Find and update matching effect
    for (auto& effect : actionEffects_[actionIdx]) {
        if (computeSimilarity(effect.beforeState, predicted) > 0.5f) {
            // Update with prediction error
            float error = 1.0f - computeSimilarity(predicted, actual);
            
            // Adjust confidence based on error
            effect.confidence *= (1.0f - error * 0.1f);
            effect.confidence = std::max(0.1f, effect.confidence);
        }
    }
}

float SelfModel::getBodyAwareness() const {
    return capabilityLevel_;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    capabilityLevel_ = 0.5f;
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                 const std::vector<float>& beforeState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return {};
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return {};
    
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& effect : effects) {
        float sim = computeSimilarity(beforeState, effect.beforeState);
        if (sim > bestSim) {
            bestSim = sim;
            bestPrediction = effect.afterState;
        }
    }
    
    return bestPrediction;
}

float SelfModel::computeSimilarity(const std::vector<float>& a,
                                   const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

// SocialLearning Implementation
struct SocialLearning::Impl {
    Brain* brain;
    
    // Track observed actions and their effects
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions;
    
    // Learn communication signals
    std::vector<float> signalPattern;
    float signalMeaning;
    
    Impl() : brain(nullptr), signalMeaning(0.0f) {}
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.0f)
{
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized with neural representation");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                       const std::vector<float>& observerState,
                                       const std::vector<float>& resultingState) {
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Observer doesn't know reward
    
    pImpl->observedActions.push_back({observedAction, effect});
    ++observationCount_;
    
    // Keep memory bounded
    if (pImpl->observedActions.size() > 500) {
        pImpl->observedActions.erase(pImpl->observedActions.begin());
    }
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've seen this action before and know its effect
    for (const auto& pair : pImpl->observedActions) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    ActionType best = ActionType::Wait;
    float bestSim = 0.0f;
    
    for (const auto& pair : pImpl->observedActions) {
        float sim = computeSimilarity(currentState, pair.second.state);
        if (sim > bestSim) {
            bestSim = sim;
            best = pair.first;
        }
    }
    
    return best;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                            float signalReward) {
    pImpl->signalPattern = signalPattern;
    pImpl->signalMeaning = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (pImpl->signalPattern.empty()) return false;
    
    float similarity = computeSimilarity(neuralPattern, pImpl->signalPattern);
    return similarity > 0.7f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return pImpl->signalPattern;
}

float SocialLearning::getSignalMeaning() const {
    return pImpl->signalMeaning;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update value of observed actions based on whether interaction was beneficial
    for (auto& pair : pImpl->observedActions) {
        // Adjust reward estimate based on interaction outcome
        pair.second.reward = pair.second.reward * 0.9f + interactionReward * 0.1f;
    }
}

void SocialLearning::clear() {
    pImpl->observedActions.clear();
    pImpl->signalPattern.clear();
    pImpl->signalMeaning = 0.0f;
    observationCount_ = 0;
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
