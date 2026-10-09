#include "DynamicSTDP.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

DynamicSTDPImpl::DynamicSTDPImpl(const DynamicSTDPConfig& config)
    : config_(config)
    , lastUpdateTime_(0.0f)
    , accumulatedActivity_(0.0f) {
    
    // Initialize activity history
    activityHistory_.reserve(config_.activityHistorySize);
    
    NLM_LOG_INFO("DynamicSTDP initialized with LTP=" + std::to_string(config_.baseLTP) +
                 ", LTD=" + std::to_string(config_.baseLTD));
}

void DynamicSTDPImpl::update(Synapse* synapse,
                            const std::vector<float>& preSpikeTimes,
                            const std::vector<float>& postSpikeTimes,
                            float dt,
                            float neuromodulatorLevel) {
    if (!synapse) return;
    
    float currentTime = lastUpdateTime_ + dt;
    lastUpdateTime_ = currentTime;
    
    // Update activity history
    accumulatedActivity_ += preSpikeTimes.size() + postSpikeTimes.size();
    if (activityHistory_.size() < config_.activityHistorySize) {
        activityHistory_.push_back(accumulatedActivity_);
    } else {
        activityHistory_.erase(activityHistory_.begin());
        activityHistory_.push_back(accumulatedActivity_);
    }
    
    // Calculate time-varying modulation factor
    float timeModulation = getTimeVaryingModulation(currentTime);
    
    // Calculate activity-dependent factor
    float activityFactor = getActivityDependentFactor(activityHistory_);
    
    // Calculate neuromodulator modulation
    float neuromodulatorModulation = neuromodulatorLevel * config_.neuromodulatorSensitivity;
    
    // Calculate effective LTP and LTD rates
    float effectiveLTP = config_.baseLTP * timeModulation * (1.0f + neuromodulatorModulation);
    float effectiveLTD = config_.baseLTD * timeModulation * (1.0f + neuromodulatorModulation);
    
    // Apply STDP for each spike pair
    float totalWeightChange = 0.0f;
    
    for (float preTime : preSpikeTimes) {
        for (float postTime : postSpikeTimes) {
            float deltaT = postTime - preTime;
            
            if (std::abs(deltaT) > 0.0001f) { // Avoid numerical issues
                float weightChange;
                
                if (deltaT > 0) { // Post after pre (LTP)
                    weightChange = effectiveLTP * activityFactor;
                    if (config_.enableMetaplasticity) {
                        // Metaplastic modification based on recent activity
                        float metaplasticFactor = 1.0f + config_.metaplasticRate * getRecentActivityLevel();
                        weightChange *= metaplasticFactor;
                    }
                } else { // Pre after post (LTD)
                    weightChange = -effectiveLTD * activityFactor;
                    if (config_.enableMetaplasticity) {
                        // Metaplastic modification based on recent activity
                        float metaplasticFactor = 1.0f + config_.metaplasticRate * getRecentActivityLevel();
                        weightChange *= metaplasticFactor;
                    }
                }
                
                totalWeightChange += weightChange;
            }
        }
    }
    
    // Apply weight change to synapse
    if (totalWeightChange != 0.0f) {
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + totalWeightChange;
        
        // Constrain weight to reasonable bounds
        float minWeight = config_.enableDepression ? -1.0f : 0.0f;
        float maxWeight = config_.enableDepression ? 1.0f : 2.0f;
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        
        synapse->setWeight(newWeight);
        
        NLM_LOG_DEBUG("DynamicSTDP: Synapse " + std::to_string(synapse->getId()) +
                     " weight changed from " + std::to_string(currentWeight) +
                     " to " + std::to_string(newWeight) +
                     " (delta=" + std::to_string(totalWeightChange) + ")");
    }
}

float DynamicSTDPImpl::getTimeVaryingModulation(float time) const {
    if (config_.timeFunction.empty()) {
        // Default exponential decay
        return std::exp(-time / 100.0f);
    }
    
    if (config_.timeFunction == "exponential") {
        return std::exp(-time / 100.0f);
    } else if (config_.timeFunction == "gaussian") {
        return std::exp(-time * time / 200.0f);
    } else if (config_.timeFunction == "linear") {
        return std::max(0.0f, 1.0f - time / 1000.0f);
    } else if (config_.timeFunction == "sigmoid") {
        return 1.0f / (1.0f + std::exp(-(time - 500.0f) / 100.0f));
    } else {
        // Default
        return std::exp(-time / 100.0f);
    }
}

float DynamicSTDPImpl::getActivityDependentFactor(const std::vector<float>& recentActivity) const {
    if (recentActivity.empty()) {
        return 1.0f;
    }
    
    // Calculate average recent activity
    float avgActivity = 0.0f;
    for (float activity : recentActivity) {
        avgActivity += activity;
    }
    avgActivity /= recentActivity.size();
    
    // Normalize to [0, 2] range
    float normalizedActivity = std::min(2.0f, avgActivity / 10.0f);
    
    return normalizedActivity;
}

float DynamicSTDPImpl::getRecentActivityLevel() const {
    if (activityHistory_.empty()) {
        return 0.0f;
    }
    
    return activityHistory_.back();
}

} // namespace nlm
