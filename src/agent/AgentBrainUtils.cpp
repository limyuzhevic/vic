#include "AgentBrainUtils.hpp"
#include "AgentBrain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

// MotorCommandSelector implementation
MotorCommandSelector::MotorCommandSelector(std::shared_ptr<const Config> config)
    : config_(config) {}

MotorCommand MotorCommandSelector::selectFromActivity(const std::vector<float>& activities) {
    if (activities.empty()) return MotorCommand::Wait;
    
    float maxActivity = calculateMaxActivity(activities);
    
    // Get threshold from config or use default
    float threshold = 0.5f;
    if (config_) {
        if (auto thresholdOpt = config_->get<float>("motor_command_threshold")) {
            threshold = *thresholdOpt;
        }
    }
    
    if (maxActivity < threshold) {
        return MotorCommand::Wait;
    }
    
    // Find command with maximum activity
    size_t maxIndex = std::distance(activities.begin(),
        std::max_element(activities.begin(), activities.end()));
    
    switch (maxIndex) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

float MotorCommandSelector::calculateWeightedActivity(const std::vector<float>& activities,
                                                     const std::vector<float>& weights) {
    if (activities.empty()) return 0.0f;
    
    float total = 0.0f;
    for (size_t i = 0; i < activities.size(); ++i) {
        float weight = (i < weights.size()) ? weights[i] : 1.0f;
        total += activities[i] * weight;
    }
    
    return total / activities.size();
}

float MotorCommandSelector::calculateMaxActivity(const std::vector<float>& activities) {
    if (activities.empty()) return 0.0f;
    return *std::max_element(activities.begin(), activities.end());
}

MotorCommand MotorCommandSelector::getRandomAction(std::shared_ptr<RandomGenerator> rng) {
    if (!rng) return MotorCommand::Wait;
    
    int choice = rng->uniformInt(0, 7);
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

// NeuromodulationState implementation
NeuromodulationState::NeuromodulationState(std::shared_ptr<const Config> config)
    : config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , plasticityFactor_(1.0f)
    , sensoryNoveltyDecay_(0.99f)
    , rewardModulationEnabled_(true)
    , curiosityEnabled_(true) {
    
    if (config_) {
        if (auto decayOpt = config_->get<float>("sensory_novelty_decay")) {
            sensoryNoveltyDecay_ = *decayOpt;
        }
        if (auto rewardEnabledOpt = config_->get<bool>("reward_modulation_enabled")) {
            rewardModulationEnabled_ = *rewardEnabledOpt;
        }
        if (auto curiosityEnabledOpt = config_->get<bool>("curiosity_enabled")) {
            curiosityEnabled_ = *curiosityEnabledOpt;
        }
    }
}

void NeuromodulationState::update(const RewardPrediction& reward, float timestep) {
    if (!rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward.reward - reward.predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward.reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    clampDopamine();
    
    // Decay novelty
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        float curiosityBase = noveltyLevel_ * 2.0f;
        float predictionInfluence = std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = curiosityBase + predictionInfluence;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update plasticity factor based on dopamine
    updatePlasticityFactor();
}

// NoveltyDetector implementation
NoveltyDetector::NoveltyDetector(std::shared_ptr<const Config> config)
    : config_(config)
    , noveltyThreshold_(0.5f)
    , decayRate_(0.99f) {
    
    if (config_) {
        if (auto thresholdOpt = config_->get<float>("novelty_threshold")) {
            noveltyThreshold_ = *thresholdOpt;
        }
        if (auto decayOpt = config_->get<float>("sensory_novelty_decay")) {
            decayRate_ = *decayOpt;
        }
    }
}

float NoveltyDetector::calculateSpatialNovelty(const std::vector<float>& current,
                                            const std::vector<float>& previous,
                                            float decayRate) {
    if (current.empty()) return 0.0f;
    
    size_t minSize = std::min(current.size(), previous.size());
    float totalDiff = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDiff += std::abs(current[i] - previous[i]);
    }
    
    return totalDiff / std::max(minSize, 1);
}

float NoveltyDetector::calculateTemporalNovelty(float currentLevel,
                                              float previousLevel,
                                              float decayRate) {
    float decayed = currentLevel * decayRate;
    return std::abs(decayed - previousLevel);
}

void NoveltyDetector::update(std::vector<float>& previous,
                           const std::vector<float>& current,
                           float decayRate) {
    previous = current;
}

// DevelopmentManager implementation
DevelopmentManager::DevelopmentManager(std::shared_ptr<const Config> config)
    : config_(config) {}

DevelopmentManager::DevelopmentalStageInfo DevelopmentManager::getStageInfo(double age) const {
    DevelopmentalStageInfo info;
    
    if (age < INITIAL_DURATION) {
        info.plasticityModifier = INITIAL_PLASTICITY;
    } else if (age < INITIAL_DURATION + CRITICAL_PERIOD_DURATION) {
        info.plasticityModifier = CRITICAL_PERIOD_PLASTICITY;
    } else if (age < INITIAL_DURATION + CRITICAL_PERIOD_DURATION + MATURATION_DURATION) {
        info.plasticityModifier = MATURATION_PLASTICITY;
    } else {
        info.plasticityModifier = ADULT_PLASTICITY;
    }
    
    // Calculate rates using the helper methods
    info.synaptogenesisRate = calculateSynaptogenesisRate(info.plasticityModifier);
    info.pruningRate = calculatePruningRate(info.plasticityModifier);
    
    return info;
}

float DevelopmentManager::calculateSynaptogenesisRate(float plasticityModifier) const {
    if (config_) {
        if (auto rateOpt = config_->get<float>("synaptogenesis_rate")) {
            return *rateOpt * plasticityModifier;
        }
    }
    return 0.0001f * plasticityModifier;
}

float DevelopmentManager::calculatePruningRate(float plasticityModifier) const {
    if (config_) {
        if (auto rateOpt = config_->get<float>("pruning_rate")) {
            return *rateOpt * (2.0f - plasticityModifier);
        }
    }
    return 0.00001f * (2.0f - plasticityModifier);
}

// ConfigValidator implementation
const float ConfigValidator::MIN_NEURON_ACTIVITY = 0.0f;
const float ConfigValidator::MAX_NEURON_ACTIVITY = 10.0f;
const float ConfigValidator::MIN_NOVELTY_LEVEL = 0.0f;
const float ConfigValidator::MAX_NOVELTY_LEVEL = 1.0f;
const float ConfigValidator::MIN_DOPAMINE_LEVEL = -1.0f;
const float ConfigValidator::MAX_DOPAMINE_LEVEL = 1.0f;
const float ConfigValidator::MIN_CURIOUSITY_LEVEL = 0.0f;
const float ConfigValidator::MAX_CURIOUSITY_LEVEL = 1.0f;
const float ConfigValidator::MIN_SENSORY_NOVELTY_DECAY = 0.5f;
const float ConfigValidator::MAX_SENSORY_NOVELTY_DECAY = 1.0f;
const double ConfigValidator::MIN_DEVELOPMENTAL_AGE = 0.0;
const double ConfigValidator::MAX_DEVELOPMENTAL_AGE = 10000.0;

bool ConfigValidator::validateAgentBrainConfig(const Config& config) {
    validateValueRanges(config);
    return true;
}

void ConfigValidator::validateValueRanges(const Config& config) {
    // Validate motor command threshold
    if (auto thresholdOpt = config.get<float>("motor_command_threshold")) {
        float value = *thresholdOpt;
        if (value < MIN_NEURON_ACTIVITY || value > MAX_NEURON_ACTIVITY) {
            NLM_LOG_WARNING("Invalid motor_command_threshold: " + std::to_string(value));
        }
    }
    
    // Validate novelty threshold
    if (auto thresholdOpt = config.get<float>("novelty_threshold")) {
        float value = *thresholdOpt;
        if (value < MIN_NOVELTY_LEVEL || value > MAX_NOVELTY_LEVEL) {
            NLM_LOG_WARNING("Invalid novelty_threshold: " + std::to_string(value));
        }
    }
    
    // Validate dopamine level
    if (auto levelOpt = config.get<float>("dopamine_level")) {
        float value = *levelOpt;
        if (value < MIN_DOPAMINE_LEVEL || value > MAX_DOPAMINE_LEVEL) {
            NLM_LOG_WARNING("Invalid dopamine_level: " + std::to_string(value));
        }
    }
    
    // Validate curiosity level
    if (auto levelOpt = config.get<float>("curiosity_level")) {
        float value = *levelOpt;
        if (value < MIN_CURIOUSITY_LEVEL || value > MAX_CURIOUSITY_LEVEL) {
            NLM_LOG_WARNING("Invalid curiosity_level: " + std::to_string(value));
        }
    }
    
    // Validate sensory novelty decay
    if (auto decayOpt = config.get<float>("sensory_novelty_decay")) {
        float value = *decayOpt;
        if (value < MIN_SENSORY_NOVELTY_DECAY || value > MAX_SENSORY_NOVELTY_DECAY) {
            NLM_LOG_WARNING("Invalid sensory_novelty_decay: " + std::to_string(value));
        }
    }
    
    // Validate developmental age (indirect validation)
}

} // namespace nlm
