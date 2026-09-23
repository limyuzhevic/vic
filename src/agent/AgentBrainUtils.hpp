#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Config;

// Motor command selection algorithms
class MotorCommandSelector {
public:
    struct MotorGroup {
        std::string name;
        float activity;
        MotorCommand command;
    };
    
    MotorCommandSelector(std::shared_ptr<const Config> config = nullptr);
    void setThreshold(float threshold) { threshold_ = threshold; }
    MotorCommand selectFromActivity(const std::vector<float>& activities);
    float calculateWeightedActivity(const std::vector<float>& activities, 
                                  const std::vector<float>& weights = {});
    float calculateMaxActivity(const std::vector<float>& activities);
    MotorCommand getRandomAction(std::shared_ptr<class RandomGenerator> rng);
    
private:
    std::shared_ptr<const Config> config_;
    float threshold_;
};

// Neuromodulation state management
class NeuromodulationState {
public:
    struct RewardPrediction {
        float reward;
        float predictedReward;
        float predictionError;
        float expectedReward;
    };
    
    NeuromodulationState(std::shared_ptr<const Config> config = nullptr);
    void update(const RewardPrediction& reward, float timestep);
    void clampDopamine() { dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f); }
    void updatePlasticityFactor() { 
        plasticityFactor_ = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor_ = std::clamp(plasticityFactor_, 0.1f, 2.0f);
    }
    
    float getDopamineLevel() const { return dopamineLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getPlasticityFactor() const { return plasticityFactor_; }
    void setRewardModulationEnabled(bool enabled) { rewardModulationEnabled_ = enabled; }
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    void setCuriosityLevel(float level) { curiosityLevel_ = level; }
    
    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
    }
    
private:
    std::shared_ptr<const Config> config_;
    
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float plasticityFactor_;
    float sensoryNoveltyDecay_;
    bool rewardModulationEnabled_;
    bool curiosityEnabled_;
};

// Novelty detection algorithms
class NoveltyDetector {
public:
    NoveltyDetector(std::shared_ptr<const Config> config = nullptr);
    float calculateSpatialNovelty(const std::vector<float>& current,
                                const std::vector<float>& previous,
                                float decayRate = 0.99f);
    float calculateTemporalNovelty(float currentLevel, float previousLevel,
                                  float decayRate);
    void update(std::vector<float>& previous, const std::vector<float>& current,
               float decayRate = 0.99f);
    
private:
    std::shared_ptr<const Config> config_;
    float noveltyThreshold_;
    float decayRate_;
};

// Developmental stage management
class DevelopmentManager {
public:
    struct DevelopmentalStageInfo {
        float plasticityModifier;
        float synaptogenesisRate;
        float pruningRate;
    };
    
    DevelopmentManager(std::shared_ptr<const Config> config = nullptr);
    DevelopmentalStageInfo getStageInfo(double age) const;
    float calculateSynaptogenesisRate(float plasticityModifier) const;
    float calculatePruningRate(float plasticityModifier) const;
    
private:
    std::shared_ptr<const Config> config_;
    
    static constexpr double INITIAL_DURATION = 60.0;
    static constexpr double CRITICAL_PERIOD_DURATION = 240.0;
    static constexpr double MATURATION_DURATION = 600.0;
    
    static constexpr float INITIAL_PLASTICITY = 1.0f;
    static constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;
    static constexpr float MATURATION_PLASTICITY = 0.5f;
    static constexpr float ADULT_PLASTICITY = 0.2f;
};

// Configuration validation utilities
class ConfigValidator {
public:
    static bool validateAgentBrainConfig(const Config& config);
    static void validateValueRanges(const Config& config);
    
private:
    static const float MIN_NEURON_ACTIVITY;
    static const float MAX_NEURON_ACTIVITY;
    static const float MIN_NOVELTY_LEVEL;
    static const float MAX_NOVELTY_LEVEL;
    static const float MIN_DOPAMINE_LEVEL;
    static const float MAX_DOPAMINE_LEVEL;
    static const float MIN_CURIOUSITY_LEVEL;
    static const float MAX_CURIOUSTY_LEVEL;
    static const float MIN_SENSORY_NOVELTY_DECAY;
    static const float MAX_SENSORY_NOVELTY_DECAY;
    static const double MIN_DEVELOPMENTAL_AGE;
    static const double MAX_DEVELOPMENTAL_AGE;
};

} // namespace nlm
