#include "Serotonin.hpp"
#include "brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodSignal;
    float impulsivityLevel;
    float socialBehavior;
    float plasticityInfluence;
    float rewardPredictionError;
    float rewardHistory;
    
    // Random number generator for behavioral variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(2.0f)
        , moodSignal(0.0f)
        , impulsivityLevel(0.0f)
        , socialBehavior(0.5f)
        , plasticityInfluence(1.0f)
        , rewardPredictionError(0.0f)
        , rewardHistory(0.0f)
        , rng(std::random_device{}())
        , dist(0.0f, 1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    return pImpl->plasticityInfluence;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Mood modulation affects plasticity
    pImpl->plasticityInfluence = 0.6f + 0.6f * pImpl->level;
    
    // Apply mood-based modulation to impulsivity
    if (pImpl->level > 0.7f) {
        pImpl->impulsivityLevel = std::min(1.0f, 
                                           pImpl->impulsivityLevel + 0.1f * static_cast<float>(dt));
    } else {
        pImpl->impulsivityLevel = std::max(0.0f, 
                                           pImpl->impulsivityLevel - 0.05f * static_cast<float>(dt));
    }
}

void Serotonin::signalReward(float reward, bool predictionError) {
    pImpl->rewardHistory = reward;
    
    if (predictionError) {
        pImpl->rewardPredictionError = -reward;  // Negative prediction error (worse than expected)
        pImpl->level = std::max(pImpl->baseline, 
                                pImpl->level + std::abs(reward) * pImpl->releaseRate * 0.7f);
    } else {
        pImpl->rewardPredictionError = reward;  // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, 
                                pImpl->level + reward * pImpl->releaseRate);
    }
    
    // Enhance plasticity after reward prediction error
    pImpl->plasticityInfluence = std::min(2.0f, 
                                           pImpl->plasticityInfluence + 0.3f * reward);
}

void Serotonin::updateSocialBehavior(float interactionQuality) {
    if (interactionQuality > 0.5f) {
        pImpl->socialBehavior = std::min(1.0f, 
                                          pImpl->socialBehavior + 0.1f);
    } else {
        pImpl->socialBehavior = std::max(0.0f, 
                                          pImpl->socialBehavior - 0.05f);
    }
}

void Serotonin::increaseMood(float delta) {
    if (delta > 0.0f) {
        pImpl->moodSignal = std::min(pImpl->peak, 
                                     pImpl->moodSignal + delta * pImpl->releaseRate * 0.8f);
    } else {
        pImpl->moodSignal = std::max(0.0f, 
                                     pImpl->moodSignal + delta * pImpl->releaseRate * 0.5f);
    }
}

void Serotonin::introduceBehavioralVariability(float strength) {
    // Add noise to behavior when serotonin is high (behavioral flexibility)
    if (pImpl->level > 0.6f) {
        float noise = (pImpl->dist(pImpl->rng) - 0.5f) * strength * 0.2f;
        pImpl->level = std::clamp(pImpl->level + noise, 0.0f, 1.0f);
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodSignal = 0.0f;
    pImpl->impulsivityLevel = 0.0f;
    pImpl->socialBehavior = 0.5f;
    pImpl->plasticityInfluence = 1.0f;
    pImpl->rewardPredictionError = 0.0f;
}

} // namespace nlm
