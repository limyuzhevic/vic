#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// ObservedEffect: Records what we see from another agent's action
// NOT a symbolic observation - an actual neural activity pattern
struct ObservedEffect {
    // State when the other agent took action
    std::vector<float> state;
    
    // Resulting state after the action
    std::vector<float> resultingState;
    
    // Estimated reward (may be approximate)
    float reward;
};

// SocialLearning: Enables learning from observing other agents
// NOT language - simple action-effect learning from observation
//
// Key mechanisms:
// - Observes other agent's actions
// - Learns that other agents cause predictable changes
// - Can imitate observed actions
// - Develops simple communication signals

class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record observation of another agent's action and effect
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);

    // Can I imitate this observed action?
    bool canImitate(ActionType observedAction) const;

    // Get the best action to imitate given current state
    ActionType getImitationAction(const std::vector<float>& currentState);

    // Learn simple communication signal from another agent
    // Signal is a neural pattern that predicts reward from other agent
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);

    // Detect if another agent is signaling
    bool detectSignal(const std::vector<float>& neuralPattern) const;

    // Get learned signal pattern
    std::vector<float> getSignalPattern() const;

    // Get signal meaning (associated reward)
    float getSignalMeaning() const;

    // Update social knowledge based on interactions
    void updateSocialKnowledge(float interactionReward);

    // Clear social learning
    void clear();

    // Has learned from others?
    bool hasSocialKnowledge() const { return observationCount_ > 0; }

    // Get observation count
    size_t getObservationCount() const { return observationCount_; }

private:
    // Compute similarity between two patterns
    float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t observationCount_;
    
    // Observed action effects (other agent's actions)
    struct ObservedEffect {
        std::vector<float> state;
        std::vector<float> resultingState;
        float reward;
    };
    
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
    
    // Communication signal
    std::vector<float> signalPattern_;
    float signalMeaning_;
};

} // namespace nlm
