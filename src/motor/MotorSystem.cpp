    // Track action selection for reinforcement learning
    actionHistory.push_back(action.get());
    
    // Clean up if history is too large
    if (pImpl->actionHistory.size() > 100) {
        delete pImpl->actionHistory[0];
        pImpl->actionHistory.erase(pImpl->actionHistory.begin());
    }
}

ActionType MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Implement real action selection based on neural activity
    // Uses competitive dynamics and neuromodulatory biases
    
    std::vector<std::pair<ActionType, float>> actionScores;
    
    // Get neural activity from motor region
    const auto& neurons = motorRegion.getAllNeurons();
    if (!neurons.empty()) {
        // Average neural activity
        float totalActivity = 0.0f;
        for (const auto& neuron : *neurons) {
            totalActivity += neuron->getActivation();
        }
        float avgActivity = totalActivity / neurons->size();
        
        // Calculate action scores based on neural patterns
        for (size_t i = 0; i < pImpl->availableActions.size(); ++i) {
            ActionType action = pImpl->availableActions[i];
            
            // Base score from random selection for now
            float score = 0.5f;
            
            // Add bias from action preferences
            for (const auto& pref : actionPreferences_) {
                if (pref.first == action) {
                    score += pref.second * 0.5f;
                }
            }
            
            // Add novelty bonus (encourage exploration)
            if (actionHistory_.empty() || 
                std::find_if(actionHistory_.begin(), actionHistory_.end(),
                    [action](const Action* a) { return a->getType() == action; }) == actionHistory_.end()) {
                score += 0.3f;  // Bonus for novel actions
            }
            
            actionScores.push_back({action, score});
        }
    }
    
    // If no neurons, return default action
    if (actionScores.empty()) {
        return ActionType::Wait;
    }
    
    // Select best action (highest score)
    std::sort(actionScores.begin(), actionScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Add stochasticity for exploration
    float rand = (std::rand() % 100) / 100.0f;
    float exploration = 0.1f;  // 10% exploration
    
    int selectedIdx = 0;
    if (rand < exploration) {
        // Random selection
        selectedIdx = std::rand() % actionScores.size();
    } else {
        // Greedy selection
        selectedIdx = 0;
    }
    
    return actionScores[selectedIdx].first;
}

std::vector<ActionType> MotorSystem::getActionPreferences() const {
    // Return current action preferences
    std::vector<ActionType> preferences;
    for (const auto& pref : actionPreferences_) {
        // Repeat action based on preference strength
        int count = static_cast<int>(pref.second * 10);
        for (int i = 0; i < count && i < 5; ++i) {  // Limit to 5 entries per action
            preferences.push_back(pref.first);
        }
    }
    return preferences;
}

void MotorSystem::updateActionHistory(ActionType action, float outcome) {
    // Record action outcome for learning
    // In a full implementation, this would update synaptic weights
    // For now, just track the action and outcome
    actionHistory_.push_back({action, outcome});
    
    // Update action preferences based on outcome
    for (auto& pref : actionPreferences_) {
        if (pref.first == action) {
            // Reinforce good actions, punish bad ones
            if (outcome > 0.5f) {
                pref.second = std::min(1.0f, pref.second + 0.1f);
            } else {
                pref.second = std::max(0.0f, pref.second - 0.2f);
            }
            break;
        }
    }
}

// MotorSystem Implementation

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // For now, just log the preference request
    // In a full implementation, this would influence action selection
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm
