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

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float signalToNoiseRatio;
    float workingMemoryBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , signalToNoiseRatio(1.0f)
        , workingMemoryBoost(1.0f) {}
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
    // ACh affects signal-to-noise ratio and working memory
    pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 3.0f;
    pImpl->workingMemoryBoost = 1.0f + pImpl->level * 2.0f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related processes
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // ACh can enhance attention through the attention system
    if (pImpl->brain && pImpl->brain->getAttention()) {
        // ACh increases attentional focus and signal-to-noise ratio
        pImpl->brain->getAttention()->setExcitationStrength(1.0f + pImpl->level * 2.0f);
        pImpl->brain->getAttention()->setInhibitionStrength(0.5f / (1.0f + pImpl->level));
    }
}

// ========== NOREPINEPHRINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float noveltySensitivity;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(3.0f)
        , arousalLevel(1.0f)
        , noveltySensitivity(1.0f) {}
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
    // NE affects arousal and excitability
    pImpl->arousalLevel = 1.0f + pImpl->level * 2.5f;
    pImpl->noveltySensitivity = 1.0f + pImpl->level * 1.5f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for arousal-related processes and signal detection
    return 1.0f + pImpl->level * 0.8f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // NE affects neural excitability and arousal
    if (pImpl->brain) {
        // Increase neural excitability - NE neurons enhance firing rates
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, pImpl->level * 0.5f);
        
        // NE enhances novelty detection and salience
        if (pImpl->brain->getNovelty()) {
            pImpl->brain->getNovelty()->setLevel(pImpl->level * 0.7f);
        }
        
        // NE modulates prediction error signaling for salient events
        if (pImpl->brain->getPredictionErrorSignal() && pImpl->level > 0.5f) {
            // Strong NE response enhances prediction error signaling
            pImpl->brain->getPredictionErrorSignal()->setLevel(pImpl->level * 0.5f);
        }
    }
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRatePositive;
    float learningRateNegative;
    float moodLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.03f)
        , peak(1.0f)
        , decayRate(0.04f)
        , releaseRate(2.5f)
        , learningRatePositive(1.0f)
        , learningRateNegative(1.0f)
        , moodLevel(1.0f) {}
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
    // 5-HT affects learning rates and mood
    pImpl->learningRatePositive = 1.0f + pImpl->level * 0.8f;
    pImpl->learningRateNegative = 1.0f + pImpl->level * 0.5f;
    pImpl->moodLevel = 1.0f + pImpl->level * 0.5f;
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity based on learning requirements
    // Higher 5-HT enhances both positive and negative learning plasticity
    return 1.0f + pImpl->level * 0.6f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // 5-HT affects learning and mood
    if (pImpl->brain) {
        // 5-HT modulates learning rates
        if (pImpl->brain->getSTDP()) {
            pImpl->brain->getSTDP()->setLTPWeight(0.01f * pImpl->learningRatePositive);
            pImpl->brain->getSTDP()->setLTDWeight(0.012f * pImpl->learningRateNegative);
        }
        
        if (pImpl->brain->getHebbian()) {
            pImpl->brain->getHebbian()->setLearningRate(0.5f * pImpl->learningRatePositive);
        }
        
        // 5-HT influences social behavior and decision making
        // It affects reward prediction error processing
        if (pImpl->brain->getPredictionErrorSignal()) {
            // 5-HT modulates prediction error sensitivity
            float errorMod = 1.0f - pImpl->level * 0.3f;  // Higher 5-HT reduces error sensitivity
            pImpl->brain->getPredictionErrorSignal()->setLevel(pImpl->level * errorMod);
        }
        
        // 5-HT affects impulsivity and decision making
        // It can modulate attentional selection and decision thresholds
        if (pImpl->brain->getAttention()) {
            // 5-HT influences attentional control
            float attentionMod = 1.0f - pImpl->level * 0.2f;  // Moderate 5-HT improves focus
            pImpl->brain->getAttention()->setInhibitionStrength(0.5f * attentionMod);
        }
    }
}

} // namespace nlm
