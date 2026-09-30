#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

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
    // Real reward prediction error signaling
    // Dopamine carries RPE signals: positive for better-than-expected, negative for worse
    if (error > 0.0f) {
        // Better than expected - increase dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.0f);
    } else {
        // Worse than expected - decrease dopamine
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryGain;
    
    // Neural mechanisms for ACh
    float cholinergicExcitation;
    float corticalInhibition;
    
    Impl() : level(0.05f), baseline(0.02f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f),
             attentionGain(2.0f), memoryGain(1.5f), cholinergicExcitation(0.0f), corticalInhibition(0.0f) {}
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
    // ACh modulates plasticity: enhances LTP, suppresses LTD
    // Higher ACh increases learning rate
    return 0.5f + 0.5f * pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics: bursts during attention, tonic baseline
    // Decay towards baseline, modulated by learning events
    float targetLevel = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = targetLevel;
    
    // Update neural effects
    pImpl->cholinergicExcitation = pImpl->level * pImpl->attentionGain;
    pImpl->corticalInhibition = 1.0f - std::min(1.0f, pImpl->level * 0.5f);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Real ACh response to attentional demands
    // ACh bursts when attention is required or novel stimuli appear
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate * 3.0f);
}

void Acetylcholine::signalMemoryEncoding(float memoryStrength) {
    // ACh enhances memory encoding and consolidation
    // Supports working memory formation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->releaseRate * 0.8f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalGain;
    float vigilanceGain;
    
    // NE effects on arousal and vigilance
    float locusCoeruleusActivation;
    float noradrenergicExcitation;
    
    Impl() : level(0.05f), baseline(0.02f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalGain(1.5f), vigilanceGain(1.0f), locusCoeruleusActivation(0.0f), noradrenergicExcitation(0.0f) {}
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
    // NE modulates plasticity: enhances signal detection, suppresses noise
    // Higher NE improves signal-to-noise ratio for learning
    return 0.5f + 0.5f * pImpl->level * pImpl->vigilanceGain;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE dynamics: phasic responses to novel events, tonic baseline
    // PEARL (phasic excitatory arousal and locus coeruleus) responses
    float targetLevel = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = targetLevel;
    
    // Update neural effects
    pImpl->noradrenergicExcitation = pImpl->level * pImpl->arousalGain;
    pImpl->locusCoeruleusActivation = pImpl->level * 0.8f;
}

void Norepinephrine::signalArousal(float arousalStrength) {
    // Real NE response to arousing events
    // NE spikes during surprise, novelty, or unexpected events
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalStrength * pImpl->releaseRate * 2.5f);
}

void Norepinephrine::signalVigilance(float vigilanceStrength) {
    // NE maintains vigilance and alertness
    // Supports sustained attention and focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceStrength * pImpl->releaseRate * 1.2f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodGain;
    float impulsivityGain;
    
    // Serotonin effects on mood and behavior
    float rapheNucleusActivation;
    float serotonergicInhibition;
    
    Impl() : level(0.1f), baseline(0.05f), peak(1.0f), decayRate(0.2f), releaseRate(1.0f),
             moodGain(1.0f), impulsivityGain(0.8f), rapheNucleusActivation(0.0f), serotonergicInhibition(0.0f) {}
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
    // Serotonin modulates plasticity: context-dependent learning
    // Supports behavioral flexibility and emotional learning
    return 0.5f + 0.5f * pImpl->level * pImpl->moodGain;
}

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT dynamics: complex modulatory effects
    // Involved in mood regulation, sleep, appetite, learning
    float targetLevel = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = targetLevel;
    
    // Update neural effects
    pImpl->serotonergicInhibition = pImpl->level * 0.7f;
    pImpl->rapheNucleusActivation = pImpl->level * 0.6f;
}

void Serotonin::signalMoodChange(float moodStrength) {
    // Real 5-HT response to mood-relevant stimuli
    // Supports affective learning and behavioral adaptation
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodStrength * pImpl->releaseRate * 1.8f);
}

void Serotonin::signalBehavioralAdaptation(float adaptationStrength) {
    // Serotonin supports behavioral flexibility and adaptation
    // Helps adjust behavior based on social and environmental context
    pImpl->level = std::min(pImpl->peak, pImpl->level + adaptationStrength * pImpl->releaseRate * 1.5f);
}

} // namespace nlm