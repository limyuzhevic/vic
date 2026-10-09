#include "CustomReward.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

CustomRewardImpl::CustomRewardImpl(const RewardConfig& config)
    : config_(config)
    , stats_() {
    
    // Initialize reward function weights
    float totalWeight = 0.0f;
    if (config_.enableIntrinsicMotivation) totalWeight += config_.intrinsicWeight;
    if (config_.enableExtrinsicMotivation) totalWeight += config_.extrinsicWeight;
    
    if (totalWeight > 0.0f) {
        // Normalize weights
        if (config_.enableIntrinsicMotivation) {
            config_.intrinsicWeight /= totalWeight;
        }
        if (config_.enableExtrinsicMotivation) {
            config_.extrinsicWeight /= totalWeight;
        }
    }
    
    NLM_LOG_INFO("CustomReward initialized with type=" + config_.functionType +
                 ", weight=" + std::to_string(config_.weight));
}

float CustomRewardImpl::compute(const std::vector<float>& state) {
    if (state.empty()) {
        return 0.0f;
    }
    
    float reward = 0.0f;
    
    // Compute based on function type
    if (config_.functionType == "simple") {
        reward = computeSimpleReward(state);
    } else if (config_.functionType == "complex") {
        reward = computeComplexReward(state);
    } else if (config_.functionType == "intrinsic") {
        reward = computeIntrinsicallyMotivatedReward(state);
    } else if (config_.functionType == "extrinsic") {
        reward = computeExtrinsicallyMotivatedReward(state);
    } else {
        // Default to simple reward
        reward = computeSimpleReward(state);
    }
    
    // Apply temporal abstraction
    if (config_.useTemporalAbstraction) {
        reward = computeTemporalAbstractionReward(state) * reward;
    }
    
    // Apply exploration bonus
    if (config_.explorationBonus != "none") {
        float explorationBonus = computeExplorationBonusReward(state);
        reward += explorationBonus;
    }
    
    // Scale by weight
    reward *= config_.weight;
    
    // Update statistics
    stats_.currentReward = reward;
    stats_.averageReward = stats_.averageReward * 0.9f + reward * 0.1f;
    stats_.maxReward = std::max(stats_.maxReward, reward);
    stats_.minReward = std::min(stats_.minReward, reward);
    
    float diff = reward - stats_.averageReward;
    stats_.stdDeviation = std::sqrt(stats_.stdDeviation * stats_.stdDeviation + diff * diff);
    stats_.stdDeviation = std::sqrt(stats_.stdDeviation / (stats_.stdDeviation + 1.0f));
    
    stats_.temporalVariation = std::abs(reward - stats_.averageReward);
    
    // Update exploration bonus statistics
    stats_.explorationBonus = computeExplorationBonusReward(state);
    
    // Update intrinsic motivation statistics
    if (config_.enableIntrinsicMotivation) {
        stats_.intrinsicMotivation = computeIntrinsicallyMotivatedReward(state);
    }
    
    // Update extrinsic motivation statistics
    if (config_.enableExtrinsicMotivation) {
        stats_.extrinsicMotivation = computeExtrinsicallyMotivatedReward(state);
    }
    
    NLM_LOG_DEBUG("CustomReward computed: " + std::to_string(reward) +
                 " (exploration bonus=" + std::to_string(stats_.explorationBonus) + ")");
    
    return reward;
}

std::vector<float> CustomRewardImpl::computeGradient(const std::vector<float>& state) {
    if (state.empty() || !config_.enableGradient) {
        return std::vector<float>(state.size(), 0.0f);
    }
    
    std::vector<float> gradient(state.size(), 0.0f);
    
    // Compute gradient based on function type
    if (config_.functionType == "simple") {
        gradient = computeSimpleRewardGradient(state);
    } else if (config_.functionType == "complex") {
        gradient = computeComplexRewardGradient(state);
    } else {
        // For complex functions, compute numerical gradient
        for (size_t i = 0; i < state.size(); ++i) {
            std::vector<float> statePlus = state;
            std::vector<float> stateMinus = state;
            
            statePlus[i] += 0.001f;
            stateMinus[i] -= 0.001f;
            
            float rewardPlus = compute(statePlus);
            float rewardMinus = compute(stateMinus);
            
            gradient[i] = (rewardPlus - rewardMinus) / 0.002f;
        }
    }
    
    // Scale by weight
    for (float& g : gradient) {
        g *= config_.weight;
    }
    
    return gradient;
}

float CustomRewardImpl::computeSimpleReward(const std::vector<float>& state) const {
    if (config_.inputFeatures.empty()) {
        // Default: sum of state values
        return std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
    }
    
    // Use specified features
    float sum = 0.0f;
    for (size_t idx : config_.featureIndices) {
        if (idx < state.size()) {
            sum += state[idx];
        }
    }
    
    return sum / config_.featureIndices.size();
}

float CustomRewardImpl::computeComplexReward(const std::vector<float>& state) const {
    // Combine multiple reward components
    float reward = 0.0f;
    
    // State-based reward
    if (!config_.inputFeatures.empty()) {
        float stateReward = 0.0f;
        for (size_t idx : config_.featureIndices) {
            if (idx < state.size()) {
                stateReward += state[idx];
            }
        }
        reward += stateReward * 0.5f;
    }
    
    // Diversity reward (encourage exploration)
    if (config_.inputFeatures.size() > 1) {
        float diversity = 0.0f;
        for (size_t i = 0; i < state.size(); ++i) {
            for (size_t j = i + 1; j < state.size(); ++j) {
                diversity += std::abs(state[i] - state[j]);
            }
        }
        reward += diversity * 0.2f;
    }
    
    // Peak reward (encourage high values)
    if (!state.empty()) {
        float maxValue = *std::max_element(state.begin(), state.end());
        reward += maxValue * 0.3f;
    }
    
    return reward;
}

float CustomRewardImpl::computeIntrinsicallyMotivatedReward(const std::vector<float>& state) const {
    if (config_.intrinsicFunction.empty()) {
        // Default: novelty reward
        return computeExplorationBonusReward(state);
    } else if (config_.intrinsicFunction == "novelty") {
        return computeExplorationBonusReward(state);
    } else if (config_.intrinsicFunction == "surprise") {
        // Surprise based on prediction error
        float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
        float variance = 0.0f;
        for (float val : state) {
            variance += (val - mean) * (val - mean);
        }
        variance /= state.size();
        return std::sqrt(variance);
    } else if (config_.intrinsicFunction == "complexity") {
        // Complexity based on state statistics
        float range = *std::max_element(state.begin(), state.end()) -
                      *std::min_element(state.begin(), state.end());
        float entropy = 0.0f;
        for (float val : state) {
            float p = val / (*std::max_element(state.begin(), state.end()) + 0.0001f);
            if (p > 0.0f && p < 1.0f) {
                entropy -= p * std::log(p) + (1.0f - p) * std::log(1.0f - p);
            }
        }
        return (range + entropy) / 2.0f;
    } else {
        return 0.0f;
    }
}

float CustomRewardImpl::computeExtrinsicallyMotivatedReward(const std::vector<float>& state) const {
    float reward = 0.0f;
    
    for (const auto& extrinsic : config_.extrinsicWeights) {
        // Find feature index
        auto it = std::find(config_.inputFeatures.begin(), config_.inputFeatures.end(), extrinsic.first);
        if (it != config_.inputFeatures.end()) {
            size_t idx = std::distance(config_.inputFeatures.begin(), it);
            if (idx < state.size()) {
                reward += state[idx] * extrinsic.second;
            }
        }
    }
    
    return reward;
}

float CustomRewardImpl::computeTemporalAbstractionReward(const std::vector<float>& state) const {
    // Apply temporal discount to future rewards
    return std::exp(-config_.temporalDiscount * static_cast<float>(stats_.temporalVariation));
}

float CustomRewardImpl::computeExplorationBonusReward(const std::vector<float>& state) const {
    if (config_.explorationBonus == "entropy") {
        // Entropy-based exploration bonus
        float entropy = 0.0f;
        for (float val : state) {
            float p = val / (*std::max_element(state.begin(), state.end()) + 0.0001f);
            if (p > 0.0f && p < 1.0f) {
                entropy -= p * std::log(p) + (1.0f - p) * std::log(1.0f - p);
            }
        }
        return entropy * 0.1f;
    } else if (config_.explorationBonus == "variance") {
        // Variance-based exploration bonus
        float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
        float variance = 0.0f;
        for (float val : state) {
            variance += (val - mean) * (val - mean);
        }
        variance /= state.size();
        return variance * 0.1f;
    } else if (config_.explorationBonus == "distance") {
        // Distance from mean-based exploration bonus
        float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
        float distance = 0.0f;
        for (float val : state) {
            distance += std::abs(val - mean);
        }
        return distance * 0.1f;
    } else {
        return 0.0f;
    }
}

std::vector<float> CustomRewardImpl::computeSimpleRewardGradient(const std::vector<float>& state) const {
    std::vector<float> gradient(state.size(), 1.0f / state.size());
    return gradient;
}

std::vector<float> CustomRewardImpl::computeComplexRewardGradient(const std::vector<float>& state) const {
    std::vector<float> gradient(state.size(), 0.0f);
    
    if (!config_.inputFeatures.empty()) {
        float weightPerFeature = 0.5f / config_.inputFeatures.size();
        for (size_t idx : config_.featureIndices) {
            if (idx < gradient.size()) {
                gradient[idx] += weightPerFeature;
            }
        }
    }
    
    if (config_.inputFeatures.size() > 1 && state.size() > 1) {
        float diversityWeight = 0.2f;
        for (size_t i = 0; i < state.size(); ++i) {
            for (size_t j = i + 1; j < state.size(); ++j) {
                gradient[i] += diversityWeight * 0.5f;
                gradient[j] -= diversityWeight * 0.5f;
            }
        }
    }
    
    if (!state.empty()) {
        float maxValue = *std::max_element(state.begin(), state.end());
        float peakWeight = 0.3f;
        for (size_t i = 0; i < state.size(); ++i) {
            if (state[i] == maxValue) {
                gradient[i] += peakWeight;
            }
        }
    }
    
    return gradient;
}

CustomRewardImpl::RewardStats CustomRewardImpl::getStats() const {
    return stats_;
}

void CustomRewardImpl::resetStats() {
    stats_.currentReward = 0.0f;
    stats_.averageReward = 0.0f;
    stats_.maxReward = std::numeric_limits<float>::lowest();
    stats_.minReward = std::numeric_limits<float>::max();
    stats_.stdDeviation = 0.0f;
    stats_.temporalVariation = 0.0f;
    stats_.explorationBonus = 0.0f;
    stats_.intrinsicMotivation = 0.0f;
    stats_.extrinsicMotivation = 0.0f;
}

} // namespace nlm
