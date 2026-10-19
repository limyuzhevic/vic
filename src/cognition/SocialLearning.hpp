#pragma once

/**
 * SocialLearning - Enables learning from observing other agents
 * NOT language - simple action-effect learning from observation
 *
 * Key mechanisms:
 * - Observes other agent's actions
 * - Learns that other agents cause predictable changes
 * - Can imitate observed actions
 * - Develops simple communication signals
 *
 * This system was developed in Phase 4 and is intended to be integrated
 * into the main brain loop for social interaction and learning.
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <deque>

namespace nlm {

// ObservedAction: Records an observation of another agent's action
struct ObservedAction {
    // The action that was observed
    ActionType observedAction;
    
    // The sensory state of the observer BEFORE the action
    std::vector<float> observerStateBefore;
    
    // The sensory state AFTER the action (result)
    std::vector<float> resultingState;
    
    // How confident the observer is about this observation
    float confidence;
    
    // How many times this observation has been made
    size_t observationCount;
    
    // Estimated reward from this observation
    float estimatedReward;
    
    // Time when this was observed
    SimulationStep observationTime;
    
    ObservedAction()
        : observedAction(ActionType::Wait), confidence(0.0f), observationCount(0), estimatedReward(0.0f), observationTime(0) {}
    
    ObservedAction(ActionType action, const std::vector<float>& before,
                   const std::vector<float>& after, float conf)
        : observedAction(action), observerStateBefore(before), resultingState(after),
          confidence(conf), observationCount(1), estimatedReward(0.0f), observationTime(0) {}
};

// ImitationRule: Represents a learned rule for imitating actions
struct ImitationRule {
    // The action to imitate
    ActionType actionToImitate;
    
    // The conditions under which to imitate (sensory state pattern)
    std::vector<float> imitationCondition;
    
    // How likely the agent is to imitate this action
    float imitationProbability;
    
    // How often this rule has been successful
    float successRate;
    
    // When this rule was last used
    SimulationStep lastUsed;
    
    ImitationRule()
        : actionToImitate(ActionType::Wait), imitationProbability(0.0f), successRate(0.0f), lastUsed(0) {}
    
    ImitationRule(ActionType action, const std::vector<float>& condition, float prob)
        : actionToImitate(action), imitationCondition(condition),
          imitationProbability(prob), successRate(0.0f), lastUsed(0) {}
};

// CommunicationSignal: Simple learned communication signal
struct CommunicationSignal {
    // The neural pattern representing this signal
    std::vector<float> signalPattern;
    
    // What this signal means (associated reward/purpose)
    float signalMeaning;
    
    // How often this signal has been detected
    size_t detectionCount;
    
    // When this signal was last detected
    SimulationStep lastDetection;
    
    CommunicationSignal()
        : signalMeaning(0.0f), detectionCount(0), lastDetection(0) {}
    
    CommunicationSignal(const std::vector<float>& pattern, float meaning)
        : signalPattern(pattern), signalMeaning(meaning), detectionCount(1), lastDetection(0) {}
};

// SocialLearning: Enables learning from observing other agents
class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record observation of another agent's action and effect
    // This is the core learning mechanism for social learning
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState,
                           float reward = 0.0f);

    // Can I imitate this observed action?
    bool canImitate(ActionType observedAction) const;

    // Get the best action to imitate given current state
    ActionType getImitationAction(const std::vector<float>& currentState) const;

    // Learn simple communication signal from another agent
    // Signal is a neural pattern that predicts reward from other agent
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);

    // Detect if another agent is signaling
    bool detectSignal(const std::vector<float>& neuralPattern) const;

    // Get learned signal pattern
    std::vector<float> getSignalPattern() const { return signalPattern_; }

    // Get signal meaning (associated reward)
    float getSignalMeaning() const { return signalMeaning_; }

    // Update social knowledge based on interactions
    void updateSocialKnowledge(float interactionReward);

    // Clear social learning
    void clear();

    // Has learned from others?
    bool hasSocialKnowledge() const { return observationCount_ > 0; }

    // Get observation count
    size_t getObservationCount() const { return observationCount_; }

    // Get number of imitation rules
    size_t getImitationRuleCount() const { return imitationRules_.size(); }

    // Get number of communication signals
    size_t getCommunicationSignalCount() const { return communicationSignals_.size(); }

    // Get average imitation success rate
    float getAverageImitationSuccess() const;

    // Get social learning effectiveness (0-1)
    float getSocialLearningEffectiveness() const;

    // Set parameters
    void setImitationThreshold(float threshold) { imitationThreshold_ = threshold; }
    void setCommunicationConfidence(float confidence) { communicationConfidence_ = confidence; }
    void setMaxObservations(size_t max) { maxObservations_ = max; }

private:
    // Find best matching observed action for a given sensory state
    std::vector<ObservedAction> findMatchingObservations(const std::vector<float>& state) const;

    // Calculate similarity between two sensory states
    float calculateStateSimilarity(const std::vector<float>& state1,
                                   const std::vector<float>& state2) const;

    // Generate new imitation rule from observed actions
    void generateImitationRules(const std::vector<float>& currentState);

    // Update imitation rule based on success/failure
    void updateImitationRule(ActionType action, bool success, float reward);

    // Learn communication signal from observation patterns
    void learnFromObservationPattern(const std::vector<ObservedAction>& observations);

    // Detect signal in neural pattern
    bool isSignalPattern(const std::vector<float>& pattern) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float imitationThreshold_;
    float communicationConfidence_;
    size_t maxObservations_;
    
    // Observed actions indexed by action type
    std::vector<std::vector<ObservedAction>> observedActions_;  // Indexed by ActionType
    
    // Learned imitation rules
    std::vector<ImitationRule> imitationRules_;
    
    // Learned communication signals
    std::vector<CommunicationSignal> communicationSignals_;
    
    // Simple signal (single pattern)
    std::vector<float> signalPattern_;
    float signalMeaning_;
    
    // Social learning statistics
    size_t observationCount_;
    size_t successfulImitations_;
    float socialLearningScore_;
};

} // namespace nlm