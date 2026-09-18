#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             predictionError(0.0f), learningRate(0.01f) {}
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
    // More realistic dopamine-modulated plasticity factor
    // Based on dopamine's effect on synaptic strength and learning
    if (pImpl->level < 0.3f) {
        // Low dopamine reduces plasticity (synaptic weakening)
        return 0.2f + 0.3f * (pImpl->level / 0.3f);
    } else if (pImpl->level < 0.7f) {
        // Moderate dopamine maintains baseline plasticity
        return 0.5f;
    } else {
        // High dopamine enhances plasticity (learning boost)
        return 0.8f + 0.2f * ((pImpl->level - 0.7f) / 0.3f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // More realistic dopamine dynamics with realistic time constants
    float dt_float = static_cast<float>(dt);
    
    // Decay towards baseline with time constant of 1000ms
    float decayFactor = std::exp(-dt_float / 1000.0f);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with temporal dynamics
    // Dopamine burst on reward delivery, with realistic saturation
    float rewardSignal = reward * pImpl->releaseRate;
    
    // Apply with realistic saturation (cannot exceed peak by more than 0.2)
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal);
    
    // Also update prediction error based on reward prediction
    pImpl->predictionError = reward;
    
    NLM_LOG_INFO(std::string("Dopamine burst: reward=") + std::to_string(reward) + 
                 std::string(", level=") + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Realistic reward prediction error signaling (TD learning)
    // Positive error (better than expected) -> dopamine increase
    // Negative error (worse than expected) -> dopamine decrease
    
    // Modulate by prediction error magnitude
    float errorSignal = error * pImpl->learningRate;
    
    // Apply error signal with bounded response
    pImpl->predictionError = error;
    pImpl->level = std::max(0.0f, pImpl->level + errorSignal * 2.0f);  // 2x sensitivity
    
    NLM_LOG_INFO(std::string("Dopamine RPE signal: error=") + std::to_string(error) + 
                 std::string(", level=") + std::to_string(pImpl->level));
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidationRate;
    float cholinergicTone;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f),
             attentionGain(1.0f), memoryConsolidationRate(0.1f), cholinergicTone(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for attention-related processes
    // Higher ACh improves signal-to-noise ratio and facilitates learning
    return 0.7f + 0.3f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Realistic acetylcholine dynamics with slower decay
    float dt_float = static_cast<float>(dt);
    
    // Decay towards baseline (longer time constant)
    float decayFactor = std::exp(-dt_float / 2000.0f);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Modulate with cholinergic tone (tonic baseline)
    pImpl->level = pImpl->level * pImpl->cholinergicTone;
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Signal attention modulation - increases ACh level
    // Attention enhances synaptic efficacy and learning in attended stimuli
    float attentionSignal = attentionStrength * pImpl->releaseRate * 1.5f;  // 1.5x gain for attention
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal);
    pImpl->attentionGain = std::min(2.0f, pImpl->attentionGain * (1.0f + attentionStrength * 0.5f));
    
    NLM_LOG_INFO(std::string("Acetylcholine attention signal: strength=") + std::to_string(attentionStrength) + 
                 std::string(", level=") + std::to_string(pImpl->level));
}

void Acetylcholine::signalMemoryConsolidation(float consolidationStrength) {
    // Signal memory consolidation processes
    // ACh facilitates consolidation of salient memories
    float consolidationSignal = consolidationStrength * pImpl->memoryConsolidationRate;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationSignal);
    
    NLM_LOG_INFO(std::string("Acetylcholine memory consolidation: strength=") + std::to_string(consolidationStrength) + 
                 std::string(", level=") + std::to_string(pImpl->level));
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceFactor;
    float locusCoeruleusGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.2f),
             arousalLevel(0.5f), vigilanceFactor(1.0f), locusCoeruleusGain(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and attention-related plasticity
    // Both increases and decreases NE can enhance plasticity (inverted U)
    float deviation = std::abs(pImpl->level - 0.5f);
    return 1.0f - 0.3f * deviation;  // Peak at 0.5
}

void Norepinephrine::update(TimestepDuration dt) {
    // Realistic norepinephrine dynamics with burst-pause coding
    float dt_float = static_cast<float>(dt);
    
    // Decay with realistic time constant
    float decayFactor = std::exp(-dt_float / 1500.0f);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Vigilance modulation
    pImpl->level = pImpl->level * pImpl->vigilanceFactor;
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Signal arousal changes - NE responds to novel stimuli, stress, attention
    pImpl->arousalLevel = arousalLevel;
    
    float arousalSignal = arousalLevel * pImpl->releaseRate * (1.0f + pImpl->level * 0.3f);
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal);
    pImpl->locusCoeruleusGain = std::min(3.0f, pImpl->locusCoeruleusGain * (1.0f + arousalLevel * 0.5f));
    
    NLM_LOG_INFO(std::string("Norepinephrine arousal: level=") + std::to_string(arousalLevel) + 
                 std::string(", NE=") + std::to_string(pImpl->level));
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityGain;
    float socialBehaviorModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), releaseRate(0.7f),
             moodLevel(0.5f), impulsivityGain(1.0f), socialBehaviorModulation(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin has complex effects on plasticity (inverted U, mood-dependent)
    // High serotonin reduces impulsivity and enhances certain types of learning
    if (pImpl->level > 0.7f) {
        // High serotonin: reduces impulsivity, stabilizes plasticity
        return 0.6f + 0.1f * (pImpl->level - 0.7f);
    } else {
        // Low to moderate serotonin
        return 0.4f + 0.3f * pImpl->level;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Realistic serotonin dynamics with longer time constant
    float dt_float = static_cast<float>(dt);
    
    // Decay with slower kinetics
    float decayFactor = std::exp(-dt_float / 3000.0f);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Social behavior modulation
    pImpl->level = pImpl->level * pImpl->socialBehaviorModulation;
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Serotonin::signalMood(float moodLevel) {
    // Signal mood changes - serotonin modulates mood and affect
    pImpl->moodLevel = moodLevel;
    
    float moodSignal = moodLevel * pImpl->releaseRate * 1.2f;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal);
    pImpl->impulsivityGain = std::max(0.5f, pImpl->impulsivityGain * (1.0f - moodLevel * 0.3f));
    
    NLM_LOG_INFO(std::string("Serotonin mood signal: level=") + std::to_string(moodLevel) + 
                 std::string(", 5-HT=") + std::to_string(pImpl->level));
}

void Serotonin::signalSocialBehavior(float socialStrength) {
    // Signal social behavior modulation
    // Serotonin enhances social bonding and reduces aggression
    float socialSignal = socialStrength * pImpl->socialBehaviorModulation * 1.3f;
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialSignal);
    pImpl->socialBehaviorModulation = std::min(2.0f, pImpl->socialBehaviorModulation * (1.0f + socialStrength * 0.4f));
    
    NLM_LOG_INFO(std::string("Serotonin social signal: strength=") + std::to_string(socialStrength) + 
                 std::string(", 5-HT=") + std::to_string(pImpl->level));
}

} // namespace nlm
