#pragma once

#include <vector>
#include <cmath>

namespace nlm {

class AgentStateManager {
public:
    AgentStateManager();
    ~AgentStateManager();
    
    // Neuromodulation state
    void setDopamineLevel(float level) { dopamineLevel_ = level; }
    float getDopamineLevel() const { return dopamineLevel_; }
    
    void setNoveltyLevel(float level) { noveltyLevel_ = level; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    void setCuriosityLevel(float level) { curiosityLevel_ = level; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    void setPredictionError(float error) { predictionError_ = error; }
    float getPredictionError() const { return predictionError_; }
    
    void setExpectedReward(float reward) { expectedReward_ = reward; }
    float getExpectedReward() const { return expectedReward_; }
    
    // Development state
    void setDevelopmentalAge(double age) { developmentalAge_ = age; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    void setPlasticityModifier(float modifier) { plasticityModifier_ = modifier; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Configuration flags
    void setRewardModulationEnabled(bool enabled) { rewardModulationEnabled_ = enabled; }
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    void setStructuralPlasticityEnabled(bool enabled) { structuralPlasticityEnabled_ = enabled; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    void setDevelopmentEnabled(bool enabled) { developmentEnabled_ = enabled; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Previous sensory state for novelty detection
    void setPreviousVision(const std::vector<float>& vision) { previousVision_ = vision; }
    const std::vector<float>& getPreviousVision() const { return previousVision_; }
    
    // Reset all state
    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Clear previous vision
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
    
    // Configuration
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    float getSensoryNoveltyDecay() const { return sensoryNoveltyDecay_; }

private:
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm