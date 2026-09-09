#pragma once

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class SocialLearning {
public:
    SocialLearning();
    ~SocialLearning();
    
    void initialize(Brain* brain);
    
    void observeAgentAction(ActionType observedAction,
                          const std::vector<float>& observerState,
                          const std::vector<float>& resultingState);
    
    bool canImitate(ActionType observedAction) const;
    
    ActionType getImitationAction(const std::vector<float>& currentState) const;
    
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                float signalReward);
    
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    
    std::vector<float> getSignalPattern() const;
    
    float getSignalMeaning() const;
    
    void updateSocialKnowledge(float interactionReward);
    
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float signalMeaning_;
    
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
    std::vector<float> signalPattern_;
    
    size_t observationCount_;
    
    static float computeSimilarity(const std::vector<float>& a,
                                  const std::vector<float>& b);
};

struct ObservedEffect {
    std::vector<float> state;
    std::vector<float> resultingState;
    float reward;
    
    ObservedEffect() : reward(0.0f) {}
};

} // namespace nlm
