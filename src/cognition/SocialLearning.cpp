// SocialLearning Implementation
struct SocialLearning::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
{
    signalPattern_.resize(10);  // 10-dimensional signal
    signalMeaning_ = 0.0f;
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                       const std::vector<float>& observerState,
                                       const std::vector<float>& resultingState) {
    ++observationCount_;
    
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Will be updated later
    
    observedActions_.push_back(std::make_pair(observedAction, effect));
    
    // Update signal pattern based on observed action
    // Signal is a neural pattern that predicts reward from other agent
    for (size_t i = 0; i < signalPattern_.size(); ++i) {
        signalPattern_[i] = 0.5f;  // Default neutral
    }
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    for (const auto& pair : observedActions_) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    // Find the most similar observed action
    float bestSimilarity = -1.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& pair : observedActions_) {
        float similarity = 0.0f;
        for (size_t i = 0; i < currentState.size() && i < pair.second.state.size(); ++i) {
            similarity += std::abs(currentState[i] - pair.second.state[i]);
        }
        similarity = 1.0f - similarity / std::max(static_cast<float>(currentState.size()), 1.0f);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestAction = pair.first;
        }
    }
    
    return bestAction;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                             float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    // Compute similarity between received pattern and learned signal
    float similarity = 0.0f;
    for (size_t i = 0; i < neuralPattern.size() && i < signalPattern_.size(); ++i) {
        similarity += std::abs(neuralPattern[i] - signalPattern_[i]);
    }
    similarity = 1.0f - similarity / std::max(static_cast<float>(neuralPattern.size()), 1.0f);
    
    return similarity > 0.5f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update the meaning of signals based on interaction outcomes
    signalMeaning_ = (signalMeaning_ + interactionReward) * 0.5f;
    
    // Update observed effect rewards
    for (auto& pair : observedActions_) {
        pair.second.reward = (pair.second.reward + interactionReward) * 0.5f;
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    observationCount_ = 0;
    signalPattern_.assign(signalPattern_.size(), 0.0f);
    signalMeaning_ = 0.0f;
}

} // namespace nlm