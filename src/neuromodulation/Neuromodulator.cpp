// Neuromodulator.cpp - Enhanced implementation

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
    float timeConstant;
    float eligibilityTraceDecay;
    
    // Dynamic state
    float phasicBurst;
    float tonicModulation;
    float predictionErrorBuffer;
    float rewardBuffer;
    
    // Temporal dynamics
    float lastUpdateTime;
    float spikeHistory[10];  // For burst detection
    int spikeCount;
    
    Impl() 
        : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , timeConstant(100.0f), eligibilityTraceDecay(0.95f)
        , phasicBurst(0.0f), tonicModulation(baseline), predictionErrorBuffer(0.0f)
        , rewardBuffer(0.0f), lastUpdateTime(0.0f), spikeCount(0) {
        for (int i = 0; i < 10; ++i) spikeHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine phasic and tonic components
    float phasic = pImpl->phasicBurst * 0.7f;
    float tonic = pImpl->tonicModulation * 0.3f;
    return phasic + tonic;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // More sophisticated plasticity modulation
    float currentDA = getLevel();
    
    // Phasic DA increases plasticity (learning)
    float phasicEffect = currentDA * 1.5f;
    
    // Tonic DA modulates baseline plasticity
    float tonicEffect = (currentDA - pImpl->baseline) * 0.5f;
    
    // Complex interaction for optimal learning
    float factor = 0.3f + phasicEffect * 0.7f - tonicEffect * 0.2f;
    
    // Ensure reasonable range
    return std::clamp(factor, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float currentTime = static_cast<float>(dt) / 1000.0f;  // Convert ms to seconds
    float dtSeconds = currentTime - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = currentTime;
    
    if (dtSeconds <= 0.0f) return;
    
    // Decay phasic burst
    pImpl->phasicBurst = std::max(0.0f, pImpl->phasicBurst - pImpl->decayRate * dtSeconds);
    
    // Slow adaptation of tonic modulation
    pImpl->tonicModulation = std::max(pImpl->baseline, 
                                     pImpl->tonicModulation - 
                                     (pImpl->tonicModulation - pImpl->baseline) * 0.01f * dtSeconds);
    
    // Update prediction error buffer
    pImpl->predictionErrorBuffer *= (1.0f - pImpl->eligibilityTraceDecay * dtSeconds);
    
    // Update reward buffer
    pImpl->rewardBuffer *= (1.0f - pImpl->eligibilityTraceDecay * dtSeconds);
    
    // Shift spike history
    for (int i = 9; i > 0; --i) {
        pImpl->spikeHistory[i] = pImpl->spikeHistory[i-1];
    }
    pImpl->spikeHistory[0] = 0.0f;
}

void Dopamine::signalReward(float reward) {
    // Encode reward as positive prediction error
    pImpl->rewardBuffer = std::max(pImpl->rewardBuffer, reward * pImpl->releaseRate);
    
    // Phasic response: burst proportional to reward magnitude
    pImpl->phasicBurst = std::min(pImpl->peak, pImpl->phasicBurst + reward * pImpl->releaseRate * 2.0f);
    
    // Update tonic level gradually
    pImpl->tonicModulation = std::min(pImpl->peak, 
                                     pImpl->tonicModulation + reward * 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Encode prediction error
    pImpl->predictionErrorBuffer = error;
    
    // Phasic response: immediate and proportional
    if (error > 0.0f) {
        // Positive prediction error (better than expected): phasic burst
        pImpl->phasicBurst = std::min(pImpl->peak, 
                                     pImpl->phasicBurst + error * pImpl->releaseRate * 1.5f);
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected): dip then recovery
        pImpl->phasicBurst = std::max(0.0f, pImpl->phasicBurst + error * pImpl->releaseRate * 0.5f);
    }
    
    // Update tonic level based on average prediction error
    pImpl->tonicModulation = std::max(pImpl->baseline,
                                     pImpl->tonicModulation + error * 0.05f);
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention-related state
    float attentionWeight;
    float memoryConsolidation;
    float arousalState;
    
    Impl()
        : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), releaseRate(0.5f)
        , attentionWeight(baseline), memoryConsolidation(0.0f), arousalState(baseline) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    // Integrate attention and arousal components
    float attention = pImpl->attentionWeight * 0.7f;
    float arousal = pImpl->arousalState * 0.3f;
    return attention + arousal;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity and attention
    float attention = pImpl->attentionWeight;
    float memoryConsolidation = pImpl->memoryConsolidation;
    
    // Attention enhances plasticity for attended stimuli
    float attentionEffect = attention * 0.8f;
    
    // Memory consolidation modulates learning rate
    float consolidationEffect = memoryConsolidation * 0.5f;
    
    return 1.0f + attentionEffect - consolidationEffect * 0.2f;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtSeconds = static_cast<float>(dt) / 1000.0f;
    
    // Decay attention weight
    pImpl->attentionWeight = std::max(pImpl->baseline, 
                                     pImpl->attentionWeight - pImpl->decayRate * dtSeconds);
    
    // Decay memory consolidation
    pImpl->memoryConsolidation *= (1.0f - pImpl->decayRate * dtSeconds);
    
    // Arousal decays slowly
    pImpl->arousalState = std::max(pImpl->baseline,
                                  pImpl->arousalState - pImpl->decayRate * 0.5f * dtSeconds);
}

// Norepinephrine implementation  
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Vigilance and arousal state
    float vigilanceLevel;
    float stressLevel;
    float focusMultiplier;
    
    Impl()
        : level(0.0f), baseline(0.005f), peak(1.0f), decayRate(0.02f), releaseRate(0.3f)
        , vigilanceLevel(baseline), stressLevel(0.0f), focusMultiplier(1.0f) {}
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
    // NE affects arousal, vigilance, and exploration
    float vigilance = pImpl->vigilanceLevel;
    float stress = pImpl->stressLevel;
    
    // High vigilance increases plasticity (enhanced learning)
    float vigilanceEffect = vigilance * 1.5f;
    
    // Stress can either enhance or impair learning
    float stressEffect = stress * 0.5f;
    
    // Focus multiplier affects learning efficiency
    float focusEffect = pImpl->focusMultiplier * 0.3f;
    
    return 1.0f + vigilanceEffect - stressEffect + focusEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtSeconds = static_cast<float>(dt) / 1000.0f;
    
    // Vigilance builds with stimulation, decays otherwise
    pImpl->vigilanceLevel = std::max(pImpl->baseline,
                                    pImpl->vigilanceLevel - pImpl->decayRate * dtSeconds);
    
    // Stress accumulates and decays
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->decayRate * 0.5f * dtSeconds);
    
    // Focus decays
    pImpl->focusMultiplier = std::max(1.0f,
                                     pImpl->focusMultiplier - pImpl->decayRate * dtSeconds);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and behavioral state
    float moodLevel;
    float impulsivity;
    float socialBehavior;
    float temperature;
    
    Impl()
        : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.08f), releaseRate(0.4f)
        , moodLevel(baseline), impulsivity(0.0f), socialBehavior(0.0f), temperature(baseline) {}
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
    // Serotonin affects mood, impulsivity, and learning
    float mood = pImpl->moodLevel;
    float impulsivity = pImpl->impulsivity;
    float social = pImpl->socialBehavior;
    
    // Positive mood enhances certain types of learning
    float moodEffect = (mood - baseline) * 0.7f;
    
    // Impulsivity affects risk-taking in exploration
    float impulsivityEffect = impulsivity * 0.5f;
    
    // Social behavior modulates learning from social observation
    float socialEffect = social * 0.8f;
    
    // Temperature (arousal) affects plasticity
    float tempEffect = (pImpl->temperature - baseline) * 0.3f;
    
    float factor = 1.0f + moodEffect + impulsivityEffect - socialEffect * 0.5f + tempEffect;
    
    return std::clamp(factor, 0.5f, 2.5f);
}

void Serotonin::update(TimestepDuration dt) {
    float dtSeconds = static_cast<float>(dt) / 1000.0f;
    
    // Mood adapts based on experience
    pImpl->moodLevel = std::max(baseline,
                               pImpl->moodLevel - pImpl->decayRate * dtSeconds);
    
    // Impulsivity and social behavior decay
    pImpl->impulsivity *= (1.0f - pImpl->decayRate * dtSeconds);
    pImpl->socialBehavior *= (1.0f - pImpl->decayRate * dtSeconds);
    
    // Temperature adapts
    pImpl->temperature = std::max(baseline,
                                 pImpl->temperature - pImpl->decayRate * dtSeconds);
}

} // namespace nlm
