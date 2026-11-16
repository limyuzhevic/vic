#include "Reward.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Touch.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../sensory/Audio.hpp"
#include "../motor/Action.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward parameters
    float resourceValue;
    float hazardPenalty;
    float noveltyMultiplier;
    float explorationFactor;
    float survivalThreshold;
    float efficiencyWeight;
    float developmentalRewardRate;
    
    // Reward counters
    float resourceRewards;
    float hazardRewards;
    float noveltyRewards;
    float explorationRewards;
    float survivalRewards;
    float efficiencyRewards;
    float developmentRewards;
    
    Impl() : 
        currentValue(0.0f), 
        accumulatedReward(0.0f),
        resourceValue(10.0f),
        hazardPenalty(5.0f),
        noveltyMultiplier(0.5f),
        explorationFactor(0.1f),
        survivalThreshold(0.5f),
        efficiencyWeight(0.5f),
        developmentalRewardRate(0.1f),
        resourceRewards(0.0f),
        hazardRewards(0.0f),
        noveltyRewards(0.0f),
        explorationRewards(0.0f),
        survivalRewards(0.0f),
        efficiencyRewards(0.0f),
        developmentRewards(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta, Reward::RewardType type) {
    pImpl->currentValue += delta;
    pImpl->accumulatedReward += delta;
    pImpl->history.push_back(delta);
    
    // Update type-specific counters
    switch (type) {
        case Reward::RewardType::ResourceFound:
            pImpl->resourceRewards += delta;
            break;
        case Reward::RewardType::HazardAvoided:
        case Reward::RewardType::HazardDamage:
            pImpl->hazardRewards += delta;
            break;
        case Reward::RewardType::Novelty:
            pImpl->noveltyRewards += delta;
            break;
        case Reward::RewardType::Exploration:
            pImpl->explorationRewards += delta;
            break;
        case Reward::RewardType::Survival:
            pImpl->survivalRewards += delta;
            break;
        case Reward::RewardType::EnergyEfficiency:
            pImpl->efficiencyRewards += delta;
            break;
        case Reward::RewardType::Development:
            pImpl->developmentRewards += delta;
            break;
        default:
            break;
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    
    // Reset counters
    pImpl->resourceRewards = 0.0f;
    pImpl->hazardRewards = 0.0f;
    pImpl->noveltyRewards = 0.0f;
    pImpl->explorationRewards = 0.0f;
    pImpl->survivalRewards = 0.0f;
    pImpl->efficiencyRewards = 0.0f;
    pImpl->developmentRewards = 0.0f;
}

float Reward::computeReward(const Observation& observation,
                           float dopamineLevel,
                           float curiosityLevel,
                           float noveltyLevel,
                           float health,
                           float energy,
                           bool isMoving,
                           bool hasInteracted,
                           float visionCohesion,
                           float forwardProgress,
                           float explorationRate) {
    float totalReward = 0.0f;
    
    // Get sensory input
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    // Compute different reward components
    float resourceReward = computeResourceReward(*sensoryInput);
    float hazardReward = computeHazardReward(*sensoryInput);
    float noveltyReward = computeNoveltyReward(*sensoryInput);
    float survivalReward = 0.0f;
    float efficiencyReward = computeEfficiencyReward(1.0f - energy, 1.0f); // Energy efficiency
    float developmentReward = computeDevelopmentReward(health);
    
    // Apply neuromodulation effects
    float neuromodulationFactor = 1.0f;
    if (dopamineLevel > 0.0f) {
        neuromodulationFactor *= (1.0f + dopamineLevel * 0.5f);
    }
    if (curiosityLevel > 0.0f) {
        neuromodulationFactor *= (1.0f + curiosityLevel * 0.3f);
    }
    
    // Calculate base reward
    totalReward += resourceReward;
    totalReward += hazardReward;
    totalReward += noveltyReward;
    totalReward += survivalReward;
    totalReward += efficiencyReward;
    totalReward += developmentReward;
    
    // Apply exploration bonus
    if (explorationRate > 0.0f) {
        totalReward += explorationRate * pImpl->explorationFactor;
    }
    
    // Apply neuromodulation
    totalReward *= neuromodulationFactor;
    
    // Store reward history
    pImpl->history.push_back(totalReward);
    pImpl->currentValue = totalReward;
    
    return totalReward;
}

float Reward::computeResourceReward(const SensoryInput& sensoryInput) const {
    float resourceReward = 0.0f;
    
    // Process each sensory modality for resource detection
    if (sensoryInput.getType() == "Vision" || sensoryInput.getType() == "Audio") {
        const auto& visionData = sensoryInput.getVision();
        float maxBrightness = 0.0f;
        for (float val : visionData) {
            maxBrightness = std::max(maxBrightness, val);
        }
        
        // Bright regions likely contain resources
        if (maxBrightness > 0.7f) {
            resourceReward += pImpl->resourceValue * (maxBrightness - 0.7f) * 2.0f;
        }
    }
    
    // Touch sensors indicate physical interaction with objects
    if (sensoryInput.getType() == "Touch") {
        const auto& touchData = sensoryInput.getTouch();
        float touchStrength = 0.0f;
        for (float val : touchData) {
            touchStrength = std::max(touchStrength, val);
        }
        
        // Strong touch indicates resource contact
        if (touchStrength > 0.5f) {
            resourceReward += pImpl->resourceValue * touchStrength * 0.5f;
        }
    }
    
    return resourceReward;
}

float Reward::computeHazardReward(const SensoryInput& sensoryInput) const {
    float hazardReward = 0.0f;
    
    // Bright dark/contrast areas may indicate hazards
    if (sensoryInput.getType() == "Vision") {
        const auto& visionData = sensoryInput.getVision();
        float avgDarkness = 0.0f;
        size_t count = 0;
        for (float val : visionData) {
            if (val < 0.3f) {  // Dark areas
                avgDarkness += (0.3f - val);
                count++;
            }
        }
        
        if (count > 0) {
            avgDarkness /= static_cast<float>(count);
            // Dark areas with high contrast may be hazards
            if (avgDarkness > 0.2f) {
                hazardReward -= pImpl->hazardPenalty * avgDarkness;
            }
        }
    }
    
    return hazardReward;
}

float Reward::computeNoveltyReward(const SensoryInput& sensoryInput) const {
    float noveltyReward = 0.0f;
    
    // High variance in sensory input indicates novelty
    if (sensoryInput.getType() == "Vision") {
        const auto& visionData = sensoryInput.getVision();
        if (visionData.size() > 1) {
            float mean = 0.0f;
            for (float val : visionData) {
                mean += val;
            }
            mean /= static_cast<float>(visionData.size());
            
            float variance = 0.0f;
            for (float val : visionData) {
                float diff = val - mean;
                variance += diff * diff;
            }
            variance /= static_cast<float>(visionData.size());
            
            noveltyReward += std::sqrt(variance) * pImpl->noveltyMultiplier;
        }
    }
    
    return noveltyReward;
}

float Reward::computeDevelopmentReward(float developmentalAge) const {
    // Reward developmental progress
    float baseReward = pImpl->developmentalRewardRate;
    float ageFactor = std::min(1.0f, developmentalAge / 300.0f); // Normalize to 300 steps
    return baseReward * ageFactor;
}

float Reward::computeEfficiencyReward(float energyUsage, float time) const {
    if (time <= 0.0f) return 0.0f;
    
    float efficiency = (1.0f - energyUsage) / time;
    return efficiency * pImpl->efficiencyWeight;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

float Reward::getAverageReward() const {
    if (pImpl->history.empty()) return 0.0f;
    float sum = 0.0f;
    for (float reward : pImpl->history) {
        sum += reward;
    }
    return sum / static_cast<float>(pImpl->history.size());
}

float Reward::getMaxReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::max_element(pImpl->history.begin(), pImpl->history.end());
}

float Reward::getMinReward() const {
    if (pImpl->history.empty()) return 0.0f;
    return *std::min_element(pImpl->history.begin(), pImpl->history.end());
}

size_t Reward::getHistorySize() const {
    return pImpl->history.size();
}

void Reward::setResourceValue(float value) {
    pImpl->resourceValue = value;
}

void Reward::setHazardPenalty(float penalty) {
    pImpl->hazardPenalty = penalty;
}

void Reward::setNoveltyMultiplier(float multiplier) {
    pImpl->noveltyMultiplier = multiplier;
}

void Reward::setExplorationFactor(float factor) {
    pImpl->explorationFactor = factor;
}

void Reward::setSurvivalThreshold(float threshold) {
    pImpl->survivalThreshold = threshold;
}

void Reward::setEfficiencyWeight(float weight) {
    pImpl->efficiencyWeight = weight;
}

void Reward::setDevelopmentalRewardRate(float rate) {
    pImpl->developmentalRewardRate = rate;
}

float Reward::getResourceValue() const {
    return pImpl->resourceValue;
}

float Reward::getHazardPenalty() const {
    return pImpl->hazardPenalty;
}

float Reward::getNoveltyMultiplier() const {
    return pImpl->noveltyMultiplier;
}

float Reward::getExplorationFactor() const {
    return pImpl->explorationFactor;
}

float Reward::getSurvivalThreshold() const {
    return pImpl->survivalThreshold;
}

float Reward::getEfficiencyWeight() const {
    return pImpl->efficiencyWeight;
}

float Reward::getDevelopmentalRewardRate() const {
    return pImpl->developmentalRewardRate;
}

} // namespace nlm
