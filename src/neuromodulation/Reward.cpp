#include "Reward.hpp"
#include "../core/Config/Config.hpp"
#include "../environment/Observation.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/Observation.hpp"
#include "../brain/Brain.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation parameters
    float goalReward;
    float resourceReward;
    float hazardPenalty;
    float stepPenalty;
    float explorationReward;
    float goalProgressWeight;
    float hazardWeight;
    float resourceWeight;
    float timeDecayRate;
    float predictionErrorWeight;
    float nearMissThreshold;
    float sparseVsDenseBalance;
    
    // State tracking
    float lastStepReward;
    float episodeTotalReward;
    float episodeStartTime;
    float episodeMaxProgress;
    bool goalAchieved;
    bool hazardTriggered;
    
    // Config and environment references
    std::shared_ptr<Config> config;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f)
           , goalReward(10.0f), resourceReward(1.0f), hazardPenalty(-5.0f)
           , stepPenalty(-0.01f), explorationReward(0.1f)
           , goalProgressWeight(0.5f), hazardWeight(0.8f), resourceWeight(0.3f)
           , timeDecayRate(0.01f), predictionErrorWeight(0.5f), nearMissThreshold(0.2f)
           , sparseVsDenseBalance(0.5f)
           , lastStepReward(0.0f), episodeTotalReward(0.0f), episodeStartTime(0.0f)
           , episodeMaxProgress(0.0f), goalAchieved(false), hazardTriggered(false)
           , config(nullptr) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->lastStepReward = delta;
    pImpl->episodeTotalReward += delta;
    pImpl->history.push_back(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->lastStepReward = 0.0f;
    pImpl->episodeTotalReward = 0.0f;
    pImpl->episodeMaxProgress = 0.0f;
    pImpl->goalAchieved = false;
    pImpl->hazardTriggered = false;
}

float Reward::computeReward(const Observation& observation) const {
    if (!pImpl->config) {
        return 0.0f; // No configuration available
    }
    
    float totalReward = 0.0f;
    
    // Extract different components from observation
    float visualReward = computeVisualReward(observation);
    float sensoryReward = computeSensoryReward(observation);
    float internalReward = computeInternalReward(observation);
    float temporalReward = computeTemporalReward();
    
    // Apply reward shaping based on configuration
    float goalProgress = getGoalProgress(observation);
    float hazardRisk = assessHazardRisk(observation);
    float resourceAvailability = getResourceAvailability(observation);
    float explorationValue = computeExplorationReward();
    
    // Calculate weighted rewards
    totalReward += visualReward * pImpl->config->getOr("reward_weight_visual", 1.0f);
    totalReward += sensoryReward * pImpl->config->getOr("reward_weight_sensory", 1.0f);
    totalReward += internalReward * pImpl->config->getOr("reward_weight_internal", 1.0f);
    totalReward += temporalReward * pImpl->config->getOr("reward_weight_temporal", 0.5f);
    
    // Sparse vs dense reward mixing
    if (pImpl->config->getOr("reward_sparse", true)) {
        // Sparse reward system
        if (goalProgress > pImpl->config->getOr("reward_goal_threshold", 0.9f)) {
            totalReward += pImpl->goalReward * pImpl->sparseVsDenseBalance;
            pImpl->goalAchieved = true;
        }
        if (resourceAvailability > 0.0f) {
            totalReward += pImpl->resourceReward * pImpl->sparseVsDenseBalance;
        }
    } else {
        // Dense reward system
        totalReward += goalProgress * pImpl->goalProgressWeight * pImpl->sparseVsDenseBalance;
        totalReward += hazardRisk * pImpl->hazardWeight * pImpl->sparseVsDenseBalance;
        totalReward += resourceAvailability * pImpl->resourceWeight * pImpl->sparseVsDenseBalance;
    }
    
    // Apply penalties
    if (hazardRisk > pImpl->config->getOr("reward_hazard_threshold", 0.7f)) {
        totalReward += pImpl->hazardPenalty;
        pImpl->hazardTriggered = true;
    }
    
    // Near-miss reward tolerance
    if (goalProgress > nearMissThreshold && goalProgress < pImpl->config->getOr("reward_goal_threshold", 0.9f)) {
        totalReward += pImpl->config->getOr("reward_near_miss_bonus", 0.5f);
    }
    
    // Step penalty for excessive time
    totalReward += pImpl->stepPenalty;
    
    // Exploration reward
    totalReward += explorationValue;
    
    // Store for history
    pImpl->currentValue = totalReward;
    pImpl->accumulatedReward += totalReward;
    pImpl->history.push_back(totalReward);
    
    // Limit history size
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    return totalReward;
}

float Reward::computeVisualReward(const Observation& observation) const {
    float visualReward = 0.0f;
    
    // For world Observation
    const auto& worldObs = dynamic_cast<const nlm::world::Observation*>(&observation);
    if (worldObs) {
        const auto& data = worldObs->getData();
        
        // Find goal-related features (assuming goal is bright pixel)
        size_t goalCount = std::count_if(data.begin(), data.end(), 
            [](float val) { return val > 0.9f; });
        
        // Find hazard-related features (assuming hazard is red component)
        size_t hazardCount = 0;
        if (data.size() >= 3) {
            hazardCount = std::count_if(data.begin(), data.begin() + data.size()/3,
                [](float val) { return val > 0.8f; });
        }
        
        // Find resource-related features
        size_t resourceCount = std::count_if(data.begin(), data.end(),
            [](float val) { return val > 0.5f && val <= 0.8f; });
        
        // Calculate rewards
        visualReward = (goalCount * 2.0f) + (resourceCount * 0.5f) - (hazardCount * 1.0f);
        
        // Update max progress
        if (goalCount > 0) {
            pImpl->episodeMaxProgress = std::max(pImpl->episodeMaxProgress, 
                static_cast<float>(goalCount) / data.size());
        }
        
        return visualReward;
    }
    
    return visualReward;
}

float Reward::computeSensoryReward(const Observation& observation) const {
    float sensoryReward = 0.0f;
    
    // Try to access sensory percept
    const auto* sensoryInput = observation.getSensoryInput();
    if (sensoryInput) {
        const auto& data = sensoryInput->getData();
        
        // Touch sensor rewards (penalize collisions)
        size_t touchCount = std::count_if(data.begin(), data.end(),
            [](float val) { return val > 0.9f; });
        sensoryReward -= touchCount * 0.5f;
        
        // Vision rewards (object detection)
        if (std::strcmp(sensoryInput->getType(), "Vision") == 0) {
            sensoryReward += calculateVisionFeatures(data);
        }
        
        // Audio rewards (sound source localization)
        if (std::strcmp(sensoryInput->getType(), "Audio") == 0) {
            sensoryReward += calculateAudioFeatures(data);
        }
    }
    
    return sensoryReward;
}

float Reward::computeInternalReward(const Observation& observation) const {
    float internalReward = 0.0f;
    
    // Internal state rewards based on proprioception and homeostatic signals
    const auto* sensoryInput = observation.getSensoryInput();
    if (sensoryInput && std::strcmp(sensoryInput->getType(), "InternalSignals") == 0) {
        const auto& data = sensoryInput->getData();
        
        // Homeostatic balance rewards
        if (!data.empty()) {
            float homeostasis = data[0];
            if (std::abs(homeostasis - 0.5f) < 0.1f) {
                internalReward += 0.1f; // Good homeostasis
            }
        }
        
        // Metabolic efficiency (lower cost for same output)
        if (data.size() > 1) {
            float energy = data[0];
            float output = data[1];
            internalReward += output / (energy + 0.01f);
        }
    }
    
    return internalReward;
}

float Reward::computeTemporalReward() const {
    float temporalReward = 0.0f;
    
    // Reward decay over time
    temporalReward -= pImpl->timeDecayRate;
    
    // Maximum episode length penalty
    temporalReward -= 0.001f;
    
    return temporalReward;
}

float Reward::getGoalProgress(const Observation& observation) const {
    float goalProgress = 0.0f;
    
    // For world Observation
    const auto& worldObs = dynamic_cast<const nlm::world::Observation*>(&observation);
    if (worldObs) {
        const auto& data = worldObs->getData();
        
        // Calculate proximity to goal based on color/similarity to goal
        size_t goalProximity = std::count_if(data.begin(), data.end(),
            [](float val) { return val > 0.7f && val <= 0.9f; });
        
        if (!data.empty()) {
            goalProgress = static_cast<float>(goalProximity) / data.size();
        }
    }
    
    return goalProgress;
}

float Reward::assessHazardRisk(const Observation& observation) const {
    float hazardRisk = 0.0f;
    
    // For world Observation
    const auto& worldObs = dynamic_cast<const nlm::world::Observation*>(&observation);
    if (worldObs) {
        const auto& data = worldObs->getData();
        
        // Count hazard indicators (assuming red channel > 0.8)
        size_t hazardCount = 0;
        if (data.size() >= 3) {
            // Check red channel (every 3rd pixel)
            for (size_t i = 0; i < data.size(); i += 3) {
                if (i + 2 < data.size() && data[i + 2] > 0.8f) {
                    hazardCount++;
                }
            }
        }
        
        hazardRisk = static_cast<float>(hazardCount) / 
            (data.size() / 3.0f + 0.01f);
    }
    
    return hazardRisk;
}

float Reward::getResourceAvailability(const Observation& observation) const {
    float resourceAvailability = 0.0f;
    
    // For world Observation
    const auto& worldObs = dynamic_cast<const nlm::world::Observation*>(&observation);
    if (worldObs) {
        const auto& data = worldObs->getData();
        
        // Count resource indicators (assuming green channel medium value)
        size_t resourceCount = 0;
        if (data.size() >= 3) {
            // Check green channel (every 3rd pixel starting from 1)
            for (size_t i = 1; i < data.size(); i += 3) {
                if (i < data.size() && data[i] > 0.4f && data[i] <= 0.7f) {
                    resourceCount++;
                }
            }
        }
        
        resourceAvailability = static_cast<float>(resourceCount) / 
            (data.size() / 3.0f + 0.01f);
    }
    
    return resourceAvailability;
}

float Reward::computeExplorationReward() const {
    float explorationReward = 0.0f;
    
    // Simple exploration bonus for visiting new areas
    // In a full implementation, this would track visited states
    explorationReward = pImpl->explorationReward * 
        std::sqrt(pImpl->config->getOr("reward_exploration_rate", 0.1f));
    
    return explorationReward;
}

float Reward::calculateVisionFeatures(const std::vector<float>& data) const {
    float visionReward = 0.0f;
    
    // Calculate simple vision features from grayscale image
    if (!data.empty()) {
        float brightness = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        
        // Moderate brightness is good for visibility
        if (brightness > 0.3f && brightness < 0.7f) {
            visionReward += 0.5f;
        }
        
        // High contrast indicates interesting features
        float mean = brightness;
        float variance = 0.0f;
        for (float val : data) {
            variance += (val - mean) * (val - mean);
        }
        variance /= data.size();
        visionReward += std::sqrt(variance) * 0.2f;
    }
    
    return visionReward;
}

float Reward::calculateAudioFeatures(const std::vector<float>& data) const {
    float audioReward = 0.0f;
    
    // Calculate simple audio features
    if (!data.empty()) {
        float energy = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        
        // Good signal-to-noise ratio
        audioReward += energy * 0.3f;
        
        // Presence of structured sound (moderate entropy)
        // In a full implementation, would use more sophisticated analysis
        audioReward += 0.1f;
    }
    
    return audioReward;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
