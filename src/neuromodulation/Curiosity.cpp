#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../world/Observation.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

// Curiosity implementation
struct Curiosity::Impl {
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float explorationTemperature;
    float epsilon;
    
    ExplorationPolicy policy;
    
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> explorationHistory;
    
    std::unique_ptr<RandomGenerator> rng;
    
    Impl()
        : level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(CuriosityConstants::EPSILON_DECAY)
        , explorationTemperature(CuriosityConstants::EXPLORATION_TEMPERATURE)
        , epsilon(CuriosityConstants::INITIAL_EPSILON)
        , policy(ExplorationPolicy::EpsilonGreedy)
        , rng(nullptr) {
        rng = std::make_unique<RandomGenerator>(42);
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    return pImpl->level;
}

void Curiosity::update(TimestepDuration dt) {
    // Decay curiosity level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update exploration statistics
    epsilon = std::max(0.01f, epsilon * pImpl->decayRate);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->noveltyWeight = 0.5f;
    pImpl->predictionErrorWeight = 0.5f;
    pImpl->epsilon = CuriosityConstants::INITIAL_EPSILON;
    pImpl->policy = ExplorationPolicy::EpsilonGreedy;
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
    pImpl->explorationHistory.clear();
}

bool Curiosity::isActive() const {
    return pImpl->level > 0.1f;
}

void Curiosity::initialize(Brain* brain) {
    // Store brain reference if needed for curiosity integration
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Combine novelty and prediction error to update curiosity level
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * std::abs(predictionError);
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Store history
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    if (pImpl->noveltyHistory.size() > NoveltyConstants::PATTERN_HISTORY_SIZE) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    }
    if (pImpl->predictionErrorHistory.size() > NoveltyConstants::PATTERN_HISTORY_SIZE) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setExplorationTemperature(float temp) {
    pImpl->explorationTemperature = std::max(0.1f, temp);
}

void Curiosity::setEpsilon(float epsilon) {
    pImpl->epsilon = std::clamp(epsilon, 0.0f, 1.0f);
}

float Curiosity::selectAction(const std::vector<float>& actionValues, 
                            const std::string& policy) {
    if (actionValues.empty()) return 0.0f;
    
    std::string policyStr = policy;
    
    if (policyStr.empty() || policyStr == "epsilon_greedy") {
        // Epsilon-greedy exploration
        if (pImpl->rng->uniformReal() < pImpl->epsilon) {
            // Explore: choose random action
            size_t idx = pImpl->rng->uniformInt(0, actionValues.size() - 1);
            return actionValues[idx];
        } else {
            // Exploit: choose best action
            return *std::max_element(actionValues.begin(), actionValues.end());
        }
    } else if (policyStr == "ucb") {
        // Upper Confidence Bound exploration
        float ucbValue = 0.0f;
        for (size_t i = 0; i < actionValues.size(); ++i) {
            float ucb = actionValues[i] + 
                       CuriosityConstants::UCB_EXPLORATION_CONSTANT * 
                       std::sqrt(std::log(pImpl->explorationHistory.size() + 1) / (i + 1));
            if (i == 0 || ucb > ucbValue) {
                ucbValue = ucb;
            }
        }
        return ucbValue;
    } else if (policyStr == "thompson_sampling") {
        // Thompson sampling: sample from beta distribution
        // For simplicity, sample from Dirichlet with alpha=1+history
        std::vector<float> samples(actionValues.size());
        for (size_t i = 0; i < actionValues.size(); ++i) {
            float alpha = 1.0f + static_cast<float>(pImpl->explorationHistory.size());
            samples[i] = pImpl->rng->beta(alpha, alpha);
        }
        // Choose action with highest sample
        return *std::max_element(samples.begin(), samples.end());
    } else if (policyStr == "boltzmann") {
        // Boltzmann (softmax) exploration
        float maxValue = *std::max_element(actionValues.begin(), actionValues.end());
        std::vector<float> expValues;
        float sum = 0.0f;
        
        for (float value : actionValues) {
            float expValue = std::exp((value - maxValue) / pImpl->explorationTemperature);
            expValues.push_back(expValue);
            sum += expValue;
        }
        
        // Normalize
        for (float& expValue : expValues) {
            expValue /= sum;
        }
        
        // Sample according to distribution
        float rand = pImpl->rng->uniformReal();
        float cumulative = 0.0f;
        for (float expValue : expValues) {
            cumulative += expValue;
            if (rand <= cumulative) {
                return expValue; // Return the probability, not the index
            }
        }
        
        // Fallback to max
        return maxValue;
    }
    
    // Default: epsilon-greedy
    return selectAction(actionValues, "epsilon_greedy");
}

void Curiosity::setExplorationPolicy(ExplorationPolicy policy) {
    pImpl->policy = policy;
}

Curiosity::ExplorationPolicy Curiosity::getExplorationPolicy() const {
    return pImpl->policy;
}

float Curiosity::getEpsilon() const {
    return pImpl->epsilon;
}

float Curiosity::getExplorationTemperature() const {
    return pImpl->explorationTemperature;
}

} // namespace nlm
