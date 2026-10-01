#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             curiosityLevel(0.0f), noveltyLevel(0.0f), predictionErrorLevel(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

float Dopamine::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

void Dopamine::setCuriosityLevel(float level) {
    pImpl->curiosityLevel = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

void Dopamine::setNoveltyLevel(float level) {
    pImpl->noveltyLevel = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPredictionErrorLevel() const {
    return pImpl->predictionErrorLevel;
}

void Dopamine::setPredictionErrorLevel(float level) {
    pImpl->predictionErrorLevel = std::clamp(level, 0.0f, 1.0f);
}

// Acetylcholine implementations
struct Acetylcholine::Impl {
    float level;
    float attentionStrength;
    bool attentionEnabled;
    float memoryConsolidation;
    
    Impl() : level(0.0f), attentionStrength(0.0f), attentionEnabled(false), memoryConsolidation(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity, especially for attention-related learning
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay of ACh level
    pImpl->level = std::max(0.0f, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(bool enable) {
    pImpl->attentionEnabled = enable;
    pImpl->attentionStrength = enable ? 1.0f : 0.0f;
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryPattern) {
    // Simple memory consolidation: strengthen association between pattern and reward
    float consolidation = std::min(1.0f, pImpl->level * 2.0f);
    pImpl->memoryConsolidation = consolidation;
}

float Acetylcholine::getMemoryConsolidationLevel() const {
    return pImpl->memoryConsolidation;
}

// Norepinephrine implementations
struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilanceLevel;
    bool vigilant;
    
    Impl() : level(0.0f), arousalLevel(0.0f), vigilanceLevel(0.0f), vigilant(false) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance effects on plasticity
    return 0.7f + 0.3f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE level decays with time
    pImpl->level = std::max(0.0f, pImpl->level - 0.03f * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float intensity) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + intensity);
}

void Norepinephrine::decreaseArousal(float intensity) {
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - intensity);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

bool Norepinephrine::isVigilant() const {
    return pImpl->vigilant;
}

void Norepinephrine::setVigilanceLevel(float level) {
    pImpl->vigilant = (level > 0.5f);
    pImpl->vigilanceLevel = level;
}

// Serotonin implementations
struct Serotonin::Impl {
    float level;
    float moodLevel;
    float impulsivityLevel;
    bool socialBehaviorEnabled;
    float socialStrength;
    
    Impl() : level(0.0f), moodLevel(0.5f), impulsivityLevel(0.5f), 
             socialBehaviorEnabled(false), socialStrength(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin generally stabilizes plasticity, reducing excessive learning
    return 0.6f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics
    pImpl->level = std::max(0.0f, pImpl->level - 0.04f * static_cast<float>(dt));
}

void Serotonin::regulateMood(float moodLevel) {
    pImpl->moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

void Serotonin::reduceImpulsivity(float reduction) {
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - reduction);
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

void Serotonin::enhanceSocialBehavior(bool enable) {
    pImpl->socialBehaviorEnabled = enable;
    pImpl->socialStrength = enable ? 1.0f : 0.0f;
}

float Serotonin::getSocialBehaviorStrength() const {
    return pImpl->socialStrength;
}

// Curiosity implementations
struct Curiosity::Impl {
    Brain* brain;
    float level;
    float explorationDrive;
    float noveltyWeight;
    float predictionErrorWeight;
    float baseline;
    
    Impl() : brain(nullptr), level(0.0f), explorationDrive(0.0f), noveltyWeight(0.5f), 
             predictionErrorWeight(0.5f), baseline(0.3f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}
Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void Curiosity::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::updateWithNoveltyAndError(float novelty, float predictionError, TimestepDuration dt) {
    // Compute curiosity as weighted combination
    float curiosity = pImpl->noveltyWeight * novelty + pImpl->predictionErrorWeight * predictionError;
    setLevel(std::min(1.0f, curiosity));
    
    pImpl->explorationDrive = getLevel() * 2.0f - 1.0f; // Map to -1 to +1
    
    // Update baseline based on sustained curiosity
    if (getLevel() > 0.5f) {
        pImpl->baseline = std::min(1.0f, pImpl->baseline + 0.01f * static_cast<float>(dt));
    }
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    setLevel(0.0f);
    pImpl->explorationDrive = 0.0f;
}

// Novelty implementations
struct Novelty::Impl {
    Brain* brain;
    float level;
    std::vector<float> history;
    float baseline;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.2f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}
Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    return 0.3f + 0.7f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.size() != previousPattern.size() || currentPattern.empty()) {
        return;
    }
    
    // Compute novelty as average difference from previous
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentPattern.size(); ++i) {
        totalDiff += std::abs(currentPattern[i] - previousPattern[i]);
    }
    
    float novelty = totalDiff / currentPattern.size();
    setLevel(std::min(1.0f, novelty * 2.0f));
    
    pImpl->history.push_back(getLevel());
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

// PredictionError implementations
struct PredictionError::Impl {
    Brain* brain;
    float error;
    float magnitude;
    std::vector<float> history;
    float baseline;
    float lastError;
    float surpriseThreshold;
    
    Impl() : brain(nullptr), error(0.0f), magnitude(0.0f), baseline(0.1f), 
             lastError(0.0f), surpriseThreshold(0.3f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}
PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getLevel() const {
    return pImpl->error;
}

void PredictionError::setLevel(float level) {
    pImpl->error = std::clamp(level, 0.0f, 1.0f);
    pImpl->magnitude = pImpl->error;
}

float PredictionError::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->error;
}

void PredictionError::update(TimestepDuration dt) {
    // Decay prediction error over time
    pImpl->error = std::max(pImpl->baseline, pImpl->error - 0.2f * static_cast<float>(dt));
}

void PredictionError::computeError(float predicted, float actual) {
    float diff = std::abs(predicted - actual);
    setLevel(std::min(1.0f, diff * 2.0f));
    
    // Compute magnitude
    pImpl->magnitude = diff;
    pImpl->lastError = pImpl->error;
    
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

bool PredictionError::isSurprising() const {
    return pImpl->lastError > pImpl->surpriseThreshold;
}

void PredictionError::setSurpriseThreshold(float t) {
    pImpl->surpriseThreshold = std::clamp(t, 0.0f, 1.0f);
}

float PredictionError::getSurpriseThreshold() const {
    return pImpl->surpriseThreshold;
}

} // namespace nlm