// Implementation for SocialLearning - Neural social learning from observation
// NOT language - simple action-effect learning from observation

#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct SocialLearning::Impl {
    Brain* brain;
    
    // Random number generator for stochastic learning
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
    std::normal_distribution<float> normalDist;
    
    // Observation similarity database
    std::vector<std::vector<float>> observationSimilarities;
    
    // State compression for efficient storage
    std::vector<float> compressedState;
    
    // Imitation rule generation parameters
    std::vector<std::vector<ActionType>> possibleActionSequences;
    
    Impl() : brain(nullptr) {
        // Seed random generator
        std::random_device rd;
        rng.seed(rd());
        randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
        observationSimilarities.resize(10);  // 10 action types
        
        // Generate some possible action sequences for imitation rules
        possibleActionSequences.reserve(20);
        std::vector<ActionType> seq1 = {ActionType::MoveForward, ActionType::TurnLeft};
        std::vector<ActionType> seq2 = {ActionType::MoveForward, ActionType::TurnRight};
        std::vector<ActionType> seq3 = {ActionType::Interact, ActionType::Wait};
        possibleActionSequences.push_back(seq1);
        possibleActionSequences.push_back(seq2);
        possibleActionSequences.push_back(seq3);
    }
};

SocialLearning::SocialLearning() : pImpl(std::make_unique<Impl>()) {
    brain_ = nullptr;
    imitationThreshold_ = 0.7f;
    communicationConfidence_ = 0.5f;
    maxObservations_ = 100;
    observationCount_ = 0;
    successfulImitations_ = 0;
    socialLearningScore_ = 0.0f;
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized with brain reference");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                        const std::vector<float>& observerState,
                                        const std::vector<float>& resultingState,
                                        float reward) {
    if (!brain_) {
        return;
    }
    
    // Check if we already have enough observations for this action
    if (observedActions_[static_cast<size_t>(observedAction)].size() >= maxObservations_) {
        // Remove oldest observation (simple FIFO)
        observedActions_[static_cast<size_t>(observedAction)].erase(
            observedActions_[static_cast<size_t>(observedAction)].begin());
    }
    
    // Calculate similarity with existing observations
    float bestSimilarity = 0.0f;
    size_t bestMatchIdx = 0;
    
    auto& observations = observedActions_[static_cast<size_t>(observedAction)];
    for (size_t i = 0; i < observations.size(); ++i) {
        float similarity = calculateStateSimilarity(observerState, observations[i].observerStateBefore);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatchIdx = i;
        }
    }
    
    // Create new observed action
    ObservedAction newObservation(observedAction, observerState, resultingState, bestSimilarity);
    newObservation.estimatedReward = reward;
    newObservation.observationTime = observationCount_;
    
    // If we have a similar observation, merge or update confidence
    if (bestSimilarity > imitationThreshold_) {
        // Update existing observation
        newObservation = observations[bestMatchIdx];
        newObservation.observationCount++;
        newObservation.estimatedReward = (newObservation.estimatedReward * (newObservation.observationCount - 1) + reward) /
                                         newObservation.observationCount;
        newObservation.confidence = (newObservation.confidence + 1.0f) / 2.0f;  // Increase confidence
        observations[bestMatchIdx] = newObservation;
        
        NLM_LOG_DEBUG(std::string("Updated social learning for action ") + std::to_string(static_cast<int>(observedAction)) +
                     std::string(" with similarity ") + std::to_string(bestSimilarity));
    } else {
        // Add new observation
        observations.push_back(newObservation);
        observationCount_++;
        NLM_LOG_DEBUG(std::string("Recorded new social learning observation for action ") + std::to_string(static_cast<int>(observedAction)));
    }
    
    // Generate new imitation rules
    generateImitationRules(observerState);
    
    // Learn from observation patterns
    learnFromObservationPattern(observations);
    
    // Update social knowledge
    updateSocialKnowledge(reward);
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    const auto& observations = observedActions_[static_cast<size_t>(observedAction)];
    
    if (observations.empty()) {
        return false;
    }
    
    // Check if we have at least one high-confidence observation
    for (const auto& observation : observations) {
        if (observation.confidence > imitationThreshold_ &&
            observation.observationCount >= 2) {  // Have seen it at least twice
            return true;
        }
    }
    
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) const {
    ActionType bestImitation = ActionType::Wait;
    float bestProbability = 0.0f;
    
    // Find best matching observation across all actions
    std::vector<ObservedAction> matchingObservations;
    
    for (size_t i = 0; i < observedActions_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        const auto& observations = observedActions_[i];
        
        for (const auto& observation : observations) {
            float similarity = calculateStateSimilarity(currentState, observation.observerStateBefore);
            if (similarity > imitationThreshold_) {
                matchingObservations.push_back(observation);
            }
        }
    }
    
    if (matchingObservations.empty()) {
        return ActionType::Wait;
    }
    
    // Find the most likely action to imitate
    for (const auto& observation : matchingObservations) {
        float probability = observation.confidence * (1.0f + observation.estimatedReward);
        
        if (probability > bestProbability) {
            bestProbability = probability;
            bestImitation = observation.observedAction;
        }
    }
    
    // Update imitation rule
    updateImitationRule(bestImitation, true, 1.0f);
    
    NLM_LOG_DEBUG(std::string("Selected imitation action ") + std::to_string(static_cast<int>(bestImitation)) +
                 std::string(" with probability ") + std::to_string(bestProbability));
    
    return bestImitation;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                              float signalReward) {
    // Check if we already have a similar signal
    bool foundSimilar = false;
    for (auto& signal : communicationSignals_) {
        float similarity = calculateStateSimilarity(signalPattern, signal.signalPattern);
        if (similarity > communicationConfidence_) {
            // Update existing signal
            signal.signalMeaning = (signal.signalMeaning + signalReward) / 2.0f;
            signal.detectionCount++;
            signal.lastDetection = observationCount_;
            foundSimilar = true;
            break;
        }
    }
    
    if (!foundSimilar) {
        // Add new signal
        CommunicationSignal newSignal(signalPattern, signalReward);
        communicationSignals_.push_back(newSignal);
        NLM_LOG_DEBUG(std::string("Learned new communication signal with reward ") + std::to_string(signalReward));
    }
    
    // Update simple signal
    signalPattern_ = signalPattern;
    signalMeaning_ = (signalMeaning_ + signalReward) / 2.0f;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    // Check simple signal first
    if (signalPattern_.empty()) {
        return false;
    }
    
    float simpleSimilarity = calculateStateSimilarity(neuralPattern, signalPattern_);
    if (simpleSimilarity > communicationConfidence_) {
        return true;
    }
    
    // Check complex signals
    for (const auto& signal : communicationSignals_) {
        float similarity = calculateStateSimilarity(neuralPattern, signal.signalPattern);
        if (similarity > communicationConfidence_) {
            return true;
        }
    }
    
    return false;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update social learning effectiveness based on rewards
    float rewardEffect = std::min(1.0f, interactionReward * 2.0f);
    
    socialLearningScore_ = (socialLearningScore_ * 0.9f) + (rewardEffect * 0.1f);
    
    // Track successful imitations (simplified)
    if (interactionReward > 0.5f) {
        successfulImitations_++;
    }
    
    NLM_LOG_DEBUG(std::string("Social learning score updated: ") + std::to_string(socialLearningScore_));
}

void SocialLearning::clear() {
    for (auto& observations : observedActions_) {
        observations.clear();
    }
    
    imitationRules_.clear();
    communicationSignals_.clear();
    
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
    
    observationCount_ = 0;
    successfulImitations_ = 0;
    socialLearningScore_ = 0.0f;
    
    NLM_LOG_INFO("SocialLearning cleared");
}

float SocialLearning::getAverageImitationSuccess() const {
    if (successfulImitations_ == 0 || observationCount_ == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(successfulImitations_) / observationCount_;
}

float SocialLearning::getSocialLearningEffectiveness() const {
    return socialLearningScore_;
}

void SocialLearning::generateImitationRules(const std::vector<float>& currentState) {
    // Generate simple imitation rules from recent observations
    for (size_t i = 0; i < observedActions_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        auto& observations = observedActions_[i];
        
        if (observations.size() < 2) {
            continue;  // Need at least 2 observations to generate rules
        }
        
        // Calculate average state for this action
        std::vector<float> avgState(currentState.size(), 0.0f);
        for (const auto& obs : observations) {
            for (size_t j = 0; j < currentState.size(); ++j) {
                avgState[j] += obs.observerStateBefore[j];
            }
        }
        
        for (size_t j = 0; j < currentState.size(); ++j) {
            avgState[j] /= observations.size();
        }
        
        // Create imitation rule
        ImitationRule rule(action, avgState, 0.5f);
        rule.successRate = getAverageImitationSuccess();
        
        // Check if we already have a similar rule
        bool found = false;
        for (const auto& existingRule : imitationRules_) {
            float similarity = calculateStateSimilarity(avgState, existingRule.imitationCondition);
            if (similarity > 0.9f) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            imitationRules_.push_back(rule);
            NLM_LOG_DEBUG(std::string("Generated imitation rule for action ") + std::to_string(static_cast<int>(action)));
        }
    }
}

void SocialLearning::updateImitationRule(ActionType action, bool success, float reward) {
    // Find existing rule for this action
    bool found = false;
    for (auto& rule : imitationRules_) {
        if (rule.actionToImitate == action) {
            found = true;
            rule.successRate = (rule.successRate * 0.8f) + (success ? 1.0f : 0.0f) * 0.2f;
            rule.imitationProbability = std::min(1.0f, rule.imitationProbability + 0.1f * reward);
            rule.lastUsed = observationCount_;
            break;
        }
    }
    
    if (!found && success) {
        // Create new rule
        ImitationRule newRule(action, std::vector<float>(), 0.5f * reward);
        newRule.successRate = success ? 1.0f : 0.0f;
        imitationRules_.push_back(newRule);
    }
}

void SocialLearning::learnFromObservationPattern(const std::vector<ObservedAction>& observations) {
    if (observations.size() < 3) {
        return;  // Need enough observations to learn patterns
    }
    
    // Simple pattern learning from multiple observations
    std::vector<float> patternSum(observations[0].observerStateBefore.size(), 0.0f);
    
    for (const auto& obs : observations) {
        for (size_t i = 0; i < patternSum.size(); ++i) {
            patternSum[i] += obs.observerStateBefore[i];
        }
    }
    
    for (size_t i = 0; i < patternSum.size(); ++i) {
        patternSum[i] /= observations.size();
    }
    
    // Calculate average resulting state
    std::vector<float> resultSum(observations[0].resultingState.size(), 0.0f);
    for (const auto& obs : observations) {
        for (size_t i = 0; i < resultSum.size(); ++i) {
            resultSum[i] += obs.resultingState[i];
        }
    }
    
    for (size_t i = 0; i < resultSum.size(); ++i) {
        resultSum[i] /= observations.size();
    }
    
    // Learn communication signal if we have enough consistent observations
    float consistency = 0.0f;
    for (size_t i = 0; i < observations.size(); ++i) {
        float stateSimilarity = calculateStateSimilarity(patternSum, observations[i].observerStateBefore);
        float resultSimilarity = calculateStateSimilarity(resultSum, observations[i].resultingState);
        consistency += (stateSimilarity + resultSimilarity) / 2.0f;
    }
    consistency /= observations.size();
    
    if (consistency > 0.7f) {
        // Learn this as a communication signal
        learnCommunicationSignal(patternSum, consistency);
    }
}

bool SocialLearning::isSignalPattern(const std::vector<float>& pattern) const {
    // Check against simple signal
    if (signalPattern_.empty()) {
        return false;
    }
    
    float similarity = calculateStateSimilarity(pattern, signalPattern_);
    return similarity > communicationConfidence_;
}

std::vector<ObservedAction> SocialLearning::findMatchingObservations(const std::vector<float>& state) const {
    std::vector<ObservedAction> matches;
    
    for (size_t i = 0; i < observedActions_.size(); ++i) {
        const auto& observations = observedActions_[i];
        
        for (const auto& observation : observations) {
            float similarity = calculateStateSimilarity(state, observation.observerStateBefore);
            if (similarity > imitationThreshold_) {
                matches.push_back(observation);
            }
        }
    }
    
    return matches;
}

float SocialLearning::calculateStateSimilarity(const std::vector<float>& state1,
                                               const std::vector<float>& state2) const {
    if (state1.size() != state2.size()) {
        return 0.0f;
    }
    
    float sumSquaredDiff = 0.0f;
    for (size_t i = 0; i < state1.size(); ++i) {
        float diff = state1[i] - state2[i];
        sumSquaredDiff += diff * diff;
    }
    
    float similarity = 1.0f / (1.0f + sqrt(sumSquaredDiff));
    return similarity;
}

} // namespace nlm