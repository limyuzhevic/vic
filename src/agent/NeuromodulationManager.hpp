#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include <algorithm>

namespace nlm {

class NeuromodulationManager {
public:
    NeuromodulationManager(std::shared_ptr<Brain> brain);
    
    void processReward(float reward, float predictedReward, bool rewardModulationEnabled);
    float getDopamineLevel() const { return dopamineLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    // Enable/disable reward modulation
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
private:
    std::shared_ptr<Brain> brain_;
    float dopamineLevel_;
    float predictionError_;
    float expectedReward_;
    float plasticityModifier_;
    bool rewardModulationEnabled_;
};

} // namespace nlm
