#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

// Forward declarations for the concrete classes
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float maxLevel;
    float minLevel;
    std::vector<float> history;
    float averageLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             maxLevel(0.0f), minLevel(0.0f), averageLevel(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateLevelStats(pImpl->level);
}

float Dopamine::getPlasticityFactor() const {
    // Biological dopamine plasticity: inverted U-shaped relationship
    float normalizedDA = pImpl->level / pImpl->peak;
    return 0.8f * std::exp(-2.0f * (normalizedDA - 0.5f) * (normalizedDA - 0.5f));
}

void Dopamine::update(TimestepDuration dt) {
    // Biological dopamine dynamics: exponential decay to baseline
    float decayAmount = (pImpl->level - pImpl->baseline) * pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    updateLevelStats(pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error (RPE) signal: burst of dopamine
    float rpe = reward - getLevel(); // Simplified RPE
    
    // Apply RPE signal with saturation
    float burstLevel = std::min(pImpl->peak, pImpl->level + rpe * pImpl->releaseRate);
    setLevel(burstLevel);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct prediction error signal (temporal difference learning)
    float newLevel = std::max(0.0f, std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate));
    setLevel(newLevel);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        setLevel(baseline);
    }
    updateLevelStats(pImpl->level);
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

float Dopamine::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Dopamine::getMinLevel() const {
    return pImpl->minLevel;
}

float Dopamine::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Dopamine::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    pImpl->history.push_back(level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

// Acetylcholine implementation

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float attentionStrength;
    float memoryPriority;
    float maxLevel;
    float minLevel;
    std::vector<float> history;
    float averageLevel;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), decayRate(0.05f), releaseRate(1.0f),
             attentionStrength(0.0f), memoryPriority(0.0f), maxLevel(0.0f), 
             minLevel(0.0f), averageLevel(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateLevelStats(pImpl->level);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity during attention periods
    return 0.5f + 0.5f * pImpl->level * (1.0f + pImpl->memoryPriority);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: exponential decay, modulated by attention and memory
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    updateLevelStats(pImpl->level);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Cholinergic burst signal for attention
    pImpl->attentionStrength = attentionStrength;
    float burstLevel = std::min(1.0f, pImpl->level + attentionStrength * pImpl->releaseRate);
    setLevel(burstLevel);
}

void Acetylcholine::signalMemoryPriority(float priority) {
    // Enhance ACh for memory priority
    pImpl->memoryPriority = priority;
    float memoryLevel = std::min(1.0f, pImpl->level + priority * 0.5f);
    setLevel(memoryLevel);
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        setLevel(baseline);
    }
    updateLevelStats(pImpl->level);
}

void Acetylcholine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Acetylcholine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

float Acetylcholine::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Acetylcholine::getMinLevel() const {
    return pImpl->minLevel;
}

float Acetylcholine::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Acetylcholine::getHistory() const {
    return pImpl->history;
}

void Acetylcholine::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Acetylcholine::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    pImpl->history.push_back(level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressLevel;
    float learningRate;
    float maxLevel;
    float minLevel;
    std::vector<float> history;
    float averageLevel;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), decayRate(0.03f), releaseRate(1.0f),
             arousalLevel(0.0f), stressLevel(0.0f), learningRate(0.0f),
             maxLevel(0.0f), minLevel(0.0f), averageLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateLevelStats(pImpl->level);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and stress
    float arousalEffect = 0.5f * pImpl->arousalLevel;
    float stressEffect = 0.3f * pImpl->stressLevel;
    float learningEffect = 0.2f * pImpl->learningRate;
    return 0.5f + arousalEffect + stressEffect + learningEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: complex regulation by arousal, stress, and learning
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Modulate based on current state
    pImpl->level *= (1.0f + pImpl->arousalLevel * 0.1f);
    pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level));
    
    updateLevelStats(pImpl->level);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousalLevel = arousalLevel;
    float arousalSignal = std::min(1.0f, arousalLevel * 2.0f);
    setLevel(arousalSignal);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressLevel = stressLevel;
    float stressResponse = std::min(1.0f, stressLevel * 1.5f);
    setLevel(stressResponse);
}

void Norepinephrine::signalLearning(float learningRate) {
    pImpl->learningRate = learningRate;
    float learningSignal = std::min(1.0f, learningRate * 1.2f);
    setLevel(learningSignal);
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        setLevel(baseline);
    }
    updateLevelStats(pImpl->level);
}

void Norepinephrine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Norepinephrine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

float Norepinephrine::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Norepinephrine::getMinLevel() const {
    return pImpl->minLevel;
}

float Norepinephrine::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Norepinephrine::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    pImpl->history.push_back(level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

// Serotonin implementation

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float socialBehavior;
    float impulsivityLevel;
    float maxLevel;
    float minLevel;
    std::vector<float> history;
    float averageLevel;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), decayRate(0.08f), releaseRate(1.0f),
             moodLevel(0.0f), socialBehavior(0.0f), impulsivityLevel(0.0f),
             maxLevel(0.0f), minLevel(0.0f), averageLevel(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateLevelStats(pImpl->level);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin has complex effects on plasticity: inverted U-shaped relationship
    float moodEffect = 0.5f * pImpl->moodLevel;
    float socialEffect = 0.3f * pImpl->socialBehavior;
    float impulsivityEffect = 0.2f * pImpl->impulsivityLevel;
    
    float normalizedLevel = pImpl->level / 1.0f;
    float baselinePlasticity = 0.5f + moodEffect + socialEffect - impulsivityEffect;
    
    // Inverted U-shaped relationship (optimal at 0.5)
    float plasticity = baselinePlasticity * std::exp(-2.0f * (normalizedLevel - 0.5f) * (normalizedLevel - 0.5f));
    return std::max(0.1f, std::min(2.0f, plasticity));
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: complex regulation by mood, social behavior, and impulsivity
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    updateLevelStats(pImpl->level);
}

void Serotonin::signalMood(float moodLevel) {
    pImpl->moodLevel = moodLevel;
    float moodResponse = std::min(1.0f, moodLevel * 1.5f);
    setLevel(moodResponse);
}

void Serotonin::signalSocialBehavior(float socialBehavior) {
    pImpl->socialBehavior = socialBehavior;
    float socialResponse = std::min(1.0f, socialBehavior * 1.2f);
    setLevel(socialResponse);
}

void Serotonin::signalImpulsivity(float impulsivityLevel) {
    pImpl->impulsivityLevel = impulsivityLevel;
    float impulsivityResponse = std::min(1.0f, impulsivityLevel * 1.8f);
    setLevel(impulsivityResponse);
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        setLevel(baseline);
    }
    updateLevelStats(pImpl->level);
}

void Serotonin::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Serotonin::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

float Serotonin::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Serotonin::getMinLevel() const {
    return pImpl->minLevel;
}

float Serotonin::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Serotonin::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    pImpl->history.push_back(level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

// Curiosity implementation (extended)

void Curiosity::update(TimestepDuration dt) {
    // Curiously call the main update method with zero values
    update(0.0f, 0.0f, dt);
}

void Curiosity::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        pImpl->level = baseline;
    }
    updateLevelStats(pImpl->level);
}

void Curiosity::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Curiosity::setReleaseRate(float rate) {
    float oldNoveltyWeight = pImpl->noveltyWeight;
    float oldPEWeight = pImpl->predictionErrorWeight;
    pImpl->noveltyWeight = rate * oldNoveltyWeight;
    pImpl->predictionErrorWeight = rate * oldPEWeight;
}

float Curiosity::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Curiosity::getMinLevel() const {
    return pImpl->minLevel;
}

float Curiosity::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Curiosity::getHistory() const {
    return pImpl->history;
}

void Curiosity::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Curiosity::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    pImpl->history.push_back(level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

// Novelty implementation (extended)

void Novelty::update(TimestepDuration dt) {
    // Decay novelty towards baseline
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    updateLevelStats(pImpl->level);
}

void Novelty::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    if (pImpl->level < baseline) {
        setLevel(baseline);
    }
    updateLevelStats(pImpl->level);
}

void Novelty::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Novelty::setReleaseRate(float rate) {
    pImpl->decayRate = rate; // Novelty uses release rate as decay rate
}

float Novelty::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Novelty::getMinLevel() const {
    return pImpl->minLevel;
}

float Novelty::getAverageLevel() const {
    return pImpl->averageLevel;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->maxLevel = pImpl->baseline;
    pImpl->minLevel = pImpl->baseline;
    pImpl->averageLevel = pImpl->baseline;
}

void Novelty::updateLevelStats(float level) {
    pImpl->maxLevel = std::max(pImpl->maxLevel, level);
    pImpl->minLevel = std::min(pImpl->minLevel, level);
    
    pImpl->history.push_back(level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (!pImpl->history.empty()) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->averageLevel = sum / static_cast<float>(pImpl->history.size());
    } else {
        pImpl->averageLevel = level;
    }
}

} // namespace nlm
