#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <memory>

namespace nlm {

struct Dopamine::Impl {
    float level;           // Current dopamine concentration (0-1)
    float baseline;        // Baseline dopamine level
    float peak;            // Peak dopamine level during reward
    float decayRate;       // Rate of dopamine decay
    float releaseRate;     // Rate of dopamine release on reward
    float predictionError; // Reward prediction error (RPE)
    float plasticityFactor; // Modulation factor for plasticity (0-1)
    float adaptationRate;  // Rate of adaptation to prediction error
    float timeConstant;    // Time constant for dopamine dynamics
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , predictionError(0.0f)
        , plasticityFactor(1.0f)
        , adaptationRate(0.3f)
        , timeConstant(10.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl()) {}

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
    // Enhanced plasticity factor with biological realism
    // Baseline plasticity + dopamine-dependent modulation
    float basePlasticity = 0.5f;
    float dopamineModulation = pImpl->level * pImpl->releaseRate * 0.5f;
    float predictionModulation = std::abs(pImpl->predictionError) * 0.5f;
    
    // Combined factors with saturation
    float plasticity = basePlasticity + dopamineModulation + predictionModulation;
    pImpl->plasticityFactor = std::clamp(plasticity, 0.0f, 1.0f);
    
    // Update level slightly to reflect plasticity modulation
    pImpl->level = std::min(pImpl->level * 1.02f, 1.0f);
    
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics with biological realism
    // Decay toward baseline with time constant
    float timeConstantMs = pImpl->timeConstant;
    float decayFactor = std::exp(-static_cast<float>(dt) * timeConstantMs / 1000.0f);
    
    // Apply decay
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Update plasticity factor based on current level
    getPlasticityFactor(); // This also updates the level for consistency
}

void Dopamine::signalReward(float reward) {
    // Reward signaling with biological dynamics
    float rewardScaled = std::clamp(reward, 0.0f, 1.0f);
    
    // Strong burst response to unexpected rewards
    if (rewardScaled > 0.5f) {
        pImpl->level = pImpl->peak;
    } else {
        pImpl->level = std::max(0.0f, pImpl->level + rewardScaled * pImpl->releaseRate * 0.8f);
    }
    
    // Update prediction error based on reward
    pImpl->predictionError = rewardScaled;
    
    // Reset baseline slightly on positive reward
    pImpl->baseline = std::max(0.1f, pImpl->baseline * 0.95f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    float errorScaled = std::clamp(error, -1.0f, 1.0f);
    
    // Strong response to prediction errors
    pImpl->predictionError = errorScaled;
    
    // Positive prediction error (better than expected): increase dopamine
    if (errorScaled > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(errorScaled) * pImpl->releaseRate * 1.0f);
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + std::abs(errorScaled) * 0.1f);
    }
    // Negative prediction error (worse than expected): decrease dopamine
    else {
        pImpl->level = std::max(0.0f, pImpl->level - std::abs(errorScaled) * pImpl->decayRate * 1.5f);
        pImpl->baseline = std::max(0.0f, pImpl->baseline - std::abs(errorScaled) * 0.1f);
    }
    
    // Update plasticity factor to reflect prediction error
    getPlasticityFactor();
}

void Dopamine::setParameters(float baseline, float peak, float decayRate, 
                             float releaseRate, float timeConstant) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    pImpl->peak = std::clamp(peak, 0.0f, 2.0f);
    pImpl->decayRate = std::clamp(decayRate, 0.01f, 1.0f);
    pImpl->releaseRate = std::clamp(releaseRate, 0.01f, 5.0f);
    pImpl->timeConstant = std::clamp(timeConstant, 1.0f, 100.0f);
}

// Acetylcholine implementation with enhanced dynamics
struct Acetylcholine::Impl {
    float level;           // ACh concentration (0-1)
    float baseline;        // Baseline level
    float peak;            // Peak during attention
    float decayRate;       // Decay rate
    float attentionBoost;  // Attention modulation strength
    float learningBoost;   // Learning rate enhancement
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , attentionBoost(1.5f)
        , learningBoost(1.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl()) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity during attention/learning
    float attentionModulation = pImpl->level * pImpl->attentionBoost;
    float learningModulation = pImpl->level * pImpl->learningBoost;
    
    // Stronger plasticity during high attention
    float plasticity = 1.0f + attentionModulation + learningModulation;
    return std::clamp(plasticity, 0.0f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay toward baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Signal attention/arousal
    float attentionScaled = std::clamp(attention, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionScaled * pImpl->attentionBoost);
}

// Norepinephrine implementation with enhanced dynamics
struct Norepinephrine::Impl {
    float level;           // NE concentration (0-1)
    float baseline;        // Baseline level
    float peak;            // Peak during arousal
    float decayRate;       // Decay rate
    float arousalBoost;   // Arousal modulation strength
    float vigilanceBoost; // Vigilance enhancement
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , arousalBoost(1.3f)
        , vigilanceBoost(1.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl()) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity during high arousal/vigilance
    float arousalModulation = pImpl->level * pImpl->arousalBoost;
    float vigilanceModulation = pImpl->level * pImpl->vigilanceBoost;
    
    // Combined arousal and vigilance effects
    float plasticity = 1.0f + arousalModulation + vigilanceModulation;
    return std::clamp(plasticity, 0.0f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay toward baseline with some spontaneous fluctuations
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Add small spontaneous fluctuations
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 0.01f);
    pImpl->level += dis(gen);
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal arousal/vigilance
    float arousalScaled = std::clamp(arousal, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalScaled * pImpl->arousalBoost);
}

// Serotonin implementation with enhanced dynamics
struct Serotonin::Impl {
    float level;           // 5-HT concentration (0-1)
    float baseline;        // Baseline level
    float peak;            // Peak during positive states
    float decayRate;       // Decay rate
    float moodBoost;       // Mood modulation strength
    float impulsivityMod;  // Impulsivity modulation
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , moodBoost(1.2f)
        , impulsivityMod(0.8f) {}
};

Serotonin::Serotonin() : pImpl(new Impl()) {}

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
    // Serotonin modulates plasticity based on mood/state
    float moodModulation = pImpl->level * pImpl->moodBoost;
    float impulsivityModulation = pImpl->level * pImpl->impulsivityMod;
    
    // Lower impulsivity with higher serotonin, enhancing stable learning
    float plasticity = 1.0f + moodModulation - impulsivityModulation;
    return std::clamp(plasticity, 0.0f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay toward baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Signal mood/state
    float moodScaled = std::clamp(mood, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodScaled * pImpl->moodBoost);
}

} // namespace nlm