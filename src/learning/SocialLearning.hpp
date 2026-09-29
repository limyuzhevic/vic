#ifndef SOCIAL_LEARNING_HPP
#define SOCIAL_LEARNING_HPP

#include <vector>
#include <string>

namespace nlm {

class Brain;

// Action types for social learning
enum class ActionType {
    Wait,
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact
};

class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();
    
    void initialize(Brain* brain);
    void observeAgentAction(ActionType observedAction,
                            const std::vector<float>& observerState,
                            const std::vector<float>& resultingState);
    void imitate(const std::string& agentID,
                 const std::vector<float>& currentState,
                 float predictedReward);
    void updatePeerInfluence(const std::string& agentID,
                           const std::vector<std::pair<std::string, float>>& peerActions);
    void updateCollectiveWisdom(const std::vector<std::string>& agentIDs,
                               const std::vector<float>& currentState);
    void update(TimestepDuration dt);
    
    // Communication and signal processing
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                  float signalReward);
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    std::vector<float> getSignalPattern() const;
    float getSignalMeaning() const;
    
    // Knowledge and reward updates
    void updateSocialKnowledge(float interactionReward);
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Social observation data
    struct ObservedAction {
        ActionType action;
        std::vector<float> state;
        std::vector<float> resultingState;
        float reward;
    };
    
    std::vector<ObservedAction> observedActions_;
    int observationCount_;
    
    // Communication signal data
    std::vector<float> signalPattern_;
    float signalMeaning_;
    
    // Imitation success tracking
    std::unordered_map<std::string, int> imitationSuccesses_;
    
    Brain* brain_;
};

} // namespace nlm

#endif // SOCIAL_LEARNING_HPP
