#pragma once

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class SelfModel {
public:
    SelfModel();
    ~SelfModel();
    
    void initialize(Brain* brain);
    
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    std::vector<float> predictActionConsequence(ActionType action,
                                               const std::vector<float>& currentState) const;
    
    float getSelfModelConfidence(ActionType action) const;
    
    float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                      const std::vector<float>& afterState,
                                      ActionType action) const;
    
    ActionType getPreferredAction(const std::vector<float>& state) const;
    
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);
    
    float getBodyAwareness() const;
    
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    float capabilityLevel_;
    
    std::vector<std::vector<ActionEffect>> actionEffects_;
    
    static float computeSimilarity(const std::vector<float>& a,
                                  const std::vector<float>& b);
};

struct ActionEffect {
    std::vector<float> beforeState;
    std::vector<float> afterState;
    float confidence;
    size_t observationCount;
    
    ActionEffect() : confidence(0.0f), observationCount(0) {}
};

} // namespace nlm
