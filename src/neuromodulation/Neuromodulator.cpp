#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

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

Dopamine::~Dopamine() {
    delete pImpl;
}

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

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;                     // Current ACh concentration
    float baseline;                  // Baseline level
    float peak;                      // Peak level
    float decayRate;                 // Rate of decay
    float releaseRate;               // Rate of release
    float attentionStrength;         // Effect on attention
    float memoryConsolidationRate;   // Effect on memory consolidation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f),
             attentionStrength(1.0f), memoryConsolidationRate(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

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
    // ACh increases plasticity for attention-grabbing stimuli
    // Higher ACh -> more selective attention -> focused plasticity
    float attentionFactor = 0.5f + 0.5f * pImpl->level;
    float memoryFactor = 0.3f + 0.7f * pImpl->memoryConsolidationRate;
    return attentionFactor * memoryFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays over time and naturally fluctuates
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float strength) {
    // Stimuli grab attention -> increase ACh
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryEvent(float importance) {
    // Important events boost memory consolidation via ACh
    pImpl->level = std::min(pImpl->peak, pImpl->level + importance * pImpl->memoryConsolidationRate * 0.5f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;                     // Current NE concentration
    float baseline;                  // Baseline level
    float peak;                      // Peak level
    float decayRate;                 // Rate of decay
    float releaseRate;               // Rate of release
    float arousalStrength;          // Effect on arousal/vigilance
    float learningRate;              // Effect on learning speed
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             arousalStrength(1.0f), learningRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

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
    // NE affects arousal and vigilance
    // Moderate NE improves signal-to-noise ratio
    float arousalFactor = 0.8f + 0.4f * std::sin(pImpl->level * M_PI);
    float learningFactor = 1.0f + 0.5f * pImpl->level;
    return arousalFactor * learningFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE decays but responds to novel/stimulating inputs
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalNovelStimulus(float intensity) {
    // Novel or intense stimuli increase NE
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Norepinephrine::signalAlert(float urgency) {
    // Urgent alerts trigger NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + urgency * pImpl->learningRate * 0.8f);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;                     // Current 5-HT concentration
    float baseline;                  // Baseline level
    float peak;                      // Peak level
    float decayRate;                 // Rate of decay
    float releaseRate;               // Rate of release
    float moodFactor;                // Effect on mood/impulsivity
    float socialFactor;              // Effect on social behavior
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.03f), releaseRate(0.6f),
             moodFactor(1.0f), socialFactor(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

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
    // Serotonin affects mood and impulsivity
    // Higher serotonin = more stable mood = more consistent learning
    float moodFactor = 1.0f - 0.3f * pImpl->level;  // Higher 5-HT = less impulsivity
    float socialFactor = 0.7f + 0.6f * pImpl->level;
    return moodFactor * socialFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin has slower dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward(float intensity) {
    // Positive reward increases serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Serotonin::signalSocialEvent(float value) {
    // Social interactions boost serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + value * pImpl->socialFactor * 0.5f);
}

} // namespace nlm
