#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             attentionBoost(2.0f), memoryBoost(1.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances learning-related plasticity
    // Modulates NMDA receptors, strengthens memory consolidation
    return 1.0f + pImpl->attentionBoost * pImpl->level + (pImpl->memoryBoost - 1.0f) * pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // ACh signals when attention is focused
    // Increases level proportionally to attention
    setLevel(std::min(pImpl->level + attention * pImpl->releaseRate, pImpl->peak));
}

void Acetylcholine::signalMemoryLoad(float memoryLoad) {
    // ACh increases during memory encoding
    // Temporarily boosts level for memory consolidation
    if (memoryLoad > 0.0f) {
        setLevel(pImpl->level + memoryLoad * 0.3f);
    }
}

void Acetylcholine::signalThetaOscillation(float thetaPower) {
    // ACh modulated by theta rhythm (8-12 Hz)
    // Coordinates prefrontal-hippocampal interactions
    // Simple model: level modulated by theta power
    setLevel(std::min(pImpl->level + thetaPower * 0.1f, 1.0f));
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    float vigilanceBoost;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f),
             arousalBoost(1.5f), vigilanceBoost(1.2f), stressResponse(0.8f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances alertness-related plasticity
    // Boosts encoding of salient/novel information
    return 1.0f + pImpl->arousalBoost * pImpl->level + pImpl->vigilanceBoost * pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stress) {
    // NE increases with stress (threat/arousal)
    // Also modulates locus coerulean firing
    if (stress > 0.0f) {
        setLevel(std::min(pImpl->level + stress * pImpl->stressResponse * pImpl->releaseRate, pImpl->peak));
    }
}

void Norepinephrine::signalLocusCoeruleActivation(float activation) {
    // Direct LC firing affects NE release
    // Phasic responses to novel events
    setLevel(pImpl->level + activation * 0.5f);
}

void Norepinephrine::signalArousal(float arousal) {
    // Tonic arousal signals
    if (arousal > pImpl->level) {
        setLevel(arousal);
    }
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float rewardBoost;
    float socialBoost;
    float inhibitionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.12f), releaseRate(1.0f),
             rewardBoost(1.3f), socialBoost(1.2f), inhibitionLevel(0.9f) {}
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
    // 5-HT modulates mood-related plasticity
    // Reduces impulsivity, enhances social learning
    float moodFactor = 1.0f + pImpl->rewardBoost * pImpl->level * 0.3f;
    float socialFactor = 1.0f + pImpl->socialBoost * pImpl->level * 0.4f;
    return moodFactor * socialFactor * pImpl->inhibitionLevel;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSatiety(float satiety) {
    // 5-HT signals satiety and reward
    // Reduces approach behavior towards goals
    if (satiety > 0.0f) {
        setLevel(std::min(pImpl->level + satiety * 0.4f, pImpl->peak));
    }
}

void Serotonin::signalSocialReward(float reward) {
    // Social reward signals through serotonin
    // Enhances social learning and cooperation
    if (reward > 0.0f) {
        setLevel(pImpl->level + reward * 0.3f);
    }
}

void Serotonin::signalMood(float mood) {
    // Direct mood modulation
    if (mood > pImpl->level) {
        setLevel(mood);
    }
}

} // namespace nlm
