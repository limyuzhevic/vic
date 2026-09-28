#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const { return "5-HT"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(float dt);
    
    // Signal processing
    void signalActionSelection(float actionValue);
    void signalPatience(float waitingTime);
    void signalSocialLearning(float socialReward);
    void signalExplorationExploitation(float explorationBias);
    void signalRewardPredictionError(float error);
    void modulateActionSelection(float& actionValue, float baseValue) const;
    void modulateExploration(float& explorationRate, float baseRate) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm