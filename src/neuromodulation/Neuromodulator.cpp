#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float expectedReward;
    float predictionError;
    float temporalDifference;
    float eligibilityTrace;
    float uncertainty;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             learningRate(0.01f), expectedReward(0.0f), predictionError(0.0f),
             temporalDifference(0.0f), eligibilityTrace(0.0f), uncertainty(0.0f) {}
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
    // Advanced dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity up to a point (inverted U-shape)
    float modulation = pImpl->level;
    if (modulation > 0.7f) {
        // Saturation effect - very high dopamine reduces plasticity
        return 1.0f - 0.5f * (modulation - 0.7f) / 0.3f;
    }
    return 0.5f + 0.5f * modulation;
}

void Dopamine::update(TimestepDuration dt, const Neuromodulator* coModulator, 
                     float predictionError) {
    // Advanced dopamine dynamics with multiple inputs
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Incorporate prediction error (TD learning)
    if (predictionError != 0.0f) {
        pImpl->predictionError = predictionError;
        // Temporal difference learning rule
        pImpl->temporalDifference = predictionError;
        pImpl->eligibilityTrace += pImpl->learningRate * predictionError * dtFloat;
        
        // Modulate level based on prediction error
        pImpl->level = std::clamp(pImpl->level + pImpl->temporalDifference * pImpl->releaseRate * dtFloat, 0.0f, 1.0f);
    }
    
    // Influence from co-modulators
    if (coModulator) {
        float coModLevel = coModulator->getLevel();
        // Norepinephrine enhances dopamine response
        if (std::string(coModulator->getName()) == "NE") {
            pImpl->level = std::min(pImpl->level * (1.0f + 0.5f * coModLevel), 1.0f);
        }
        // Serotonin suppresses dopamine response
        else if (std::string(coModulator->getName()) == "5-HT") {
            pImpl->level = std::max(pImpl->baseline, pImpl->level * (1.0f - 0.3f * coModLevel));
        }
    }
    
    // Incorporate uncertainty
    pImpl->level *= (1.0f - pImpl->uncertainty * 0.1f);
}

void Dopamine::signalReward(float reward, float prediction) {
    // Advanced reward signaling with expectation matching
    float predictionError = reward - prediction;
    signalRewardPredictionError(predictionError);
    
    // Update expected reward with temporal difference learning
    float tdError = predictionError;
    pImpl->expectedReward += pImpl->learningRate * tdError;
    
    // Burst firing for unexpected rewards
    if (std::abs(predictionError) > 0.5f) {
        pImpl->level = std::min(pImpl->level + reward * pImpl->releaseRate * 2.0f, pImpl->peak);
    } else {
        pImpl->level = std::min(pImpl->level + reward * pImpl->releaseRate, pImpl->peak);
    }
    
    pImpl->uncertainty = std::max(0.0f, pImpl->uncertainty - 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Temporal difference learning for reward prediction error
    pImpl->predictionError = error;
    pImpl->temporalDifference = error;
    
    // Update level based on prediction error
    pImpl->level = std::clamp(pImpl->level + error * pImpl->releaseRate, 0.0f, 1.0f);
    
    // Maintain eligibility trace for learning
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + std::abs(error) * pImpl->learningRate);
}

void Dopamine::signalUncertainty(float uncertainty) {
    pImpl->uncertainty = std::clamp(uncertainty, 0.0f, 1.0f);
    
    // High uncertainty reduces dopamine level (prevents over-learning)
    if (uncertainty > 0.5f) {
        pImpl->level *= (1.0f - uncertainty * 0.3f);
    }
}

void Dopamine::setExpectedReward(float reward) {
    pImpl->expectedReward = reward;
}

void Dopamine::updateExpectedReward(float actualReward, float discount) {
    float tdError = actualReward - pImpl->expectedReward;
    pImpl->temporalDifference = tdError;
    pImpl->expectedReward += pImpl->learningRate * tdError;
    
    // Update level based on temporal difference
    pImpl->level = std::clamp(pImpl->level + tdError * pImpl->releaseRate * discount, 0.0f, 1.0f);
}

float Dopamine::getExpectedReward() const {
    return pImpl->expectedReward;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float attentionStrength;
    float workingMemoryCapacity;
    float memoryStability;
    float consolidationRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f),
             learningRate(0.005f), attentionStrength(0.5f), workingMemoryCapacity(1.0f),
             memoryStability(0.5f), consolidationRate(0.01f) {}
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
    // ACh enhances plasticity during attention periods
    return 1.0f + pImpl->attentionStrength * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt, const Neuromodulator* coModulator, 
                          float predictionError) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Update attention modulation
    pImpl->attentionStrength = std::clamp(pImpl->attentionStrength, 0.0f, 1.0f);
    
    // Influence from other neuromodulators
    if (coModulator) {
        float coModLevel = coModulator->getLevel();
        // Dopamine enhances ACh effect
        if (std::string(coModulator->getName()) == "DA") {
            pImpl->attentionStrength *= (1.0f + 0.3f * coModLevel);
        }
    }
}

void Acetylcholine::signalAttention(float attentionStrength) {
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->level + attentionStrength * pImpl->releaseRate, pImpl->peak);
}

void Acetylcholine::signalWorkingMemoryLoad(float load) {
    // Load affects memory capacity
    pImpl->workingMemoryCapacity = 1.0f - std::clamp(load, 0.0f, 1.0f) * 0.8f;
    
    if (load > 0.7f) {
        // High load reduces attention
        pImpl->level *= (1.0f - (load - 0.7f) * 2.0f);
    }
}

void Acetylcholine::signalMemoryUpdate(const std::vector<float>& memoryTraces) {
    if (memoryTraces.empty()) return;
    
    // Calculate memory stability from trace consistency
    float meanTrace = std::accumulate(memoryTraces.begin(), memoryTraces.end(), 0.0f) / memoryTraces.size();
    float variance = 0.0f;
    for (float trace : memoryTraces) {
        variance += (trace - meanTrace) * (trace - meanTrace);
    }
    variance /= memoryTraces.size();
    
    pImpl->memoryStability = 1.0f - std::sqrt(variance);
    
    // Strengthen memory traces with ACh
    if (pImpl->memoryStability > 0.5f) {
        pImpl->level = std::min(pImpl->level + pImpl->memoryStability * pImpl->consolidationRate, pImpl->peak);
    }
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getWorkingMemoryCapacity() const {
    return pImpl->workingMemoryCapacity;
}

float Acetylcholine::getMemoryStability() const {
    return pImpl->memoryStability;
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float vigilanceLevel;
    float stressLevel;
    float cognitiveControl;
    float arousalRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(0.7f),
             learningRate(0.003f), vigilanceLevel(0.5f), stressLevel(0.0f),
             cognitiveControl(0.5f), arousalRate(0.02f) {}
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
    // NE modulates plasticity based on arousal and stress
    float modulation = pImpl->level;
    if (pImpl->stressLevel > 0.5f) {
        // Stress reduces plasticity
        modulation *= (1.0f - pImpl->stressLevel * 0.5f);
    }
    return 0.5f + 0.5f * modulation * pImpl->cognitiveControl;
}

void Norepinephrine::update(TimestepDuration dt, const Neuromodulator* coModulator, 
                          float predictionError) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Update arousal dynamics
    pImpl->level += pImpl->arousalRate * dtFloat;
    
    // Stress affects recovery
    if (pImpl->stressLevel > 0.0f) {
        pImpl->level *= (1.0f - pImpl->stressLevel * 0.1f * dtFloat);
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.05f * dtFloat);
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Stress increases NE level
    if (stressLevel > 0.3f) {
        pImpl->level = std::min(pImpl->level + stressLevel * pImpl->releaseRate * 2.0f, pImpl->peak);
    }
    
    // Stress reduces cognitive control
    pImpl->cognitiveControl = std::max(0.0f, pImpl->cognitiveControl - stressLevel * 0.5f);
}

void Norepinephrine::signalCognitiveLoad(float load) {
    // Load affects vigilance and cognitive control
    pImpl->vigilanceLevel = 1.0f - std::clamp(load, 0.0f, 1.0f) * 0.8f;
    
    if (load > 0.6f) {
        // High load reduces cognitive control
        pImpl->cognitiveControl *= (1.0f - (load - 0.6f) * 1.5f);
    }
}

void Norepinephrine::signalPerformanceFeedback(float performance) {
    // Performance feedback adjusts cognitive control
    if (performance < 0.5f) {
        pImpl->cognitiveControl = std::min(1.0f, pImpl->cognitiveControl + 0.1f);
    } else if (performance > 0.8f) {
        pImpl->cognitiveControl = std::max(0.0f, pImpl->cognitiveControl - 0.05f);
    }
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getCognitiveControl() const {
    return pImpl->cognitiveControl;
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float moodLevel;
    float socialBehaviorStrength;
    float behavioralFlexibility;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(0.6f),
             learningRate(0.002f), moodLevel(0.5f), socialBehaviorStrength(0.5f),
             behavioralFlexibility(0.5f), impulsivity(0.3f) {}
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
    // Serotonin modulates plasticity based on mood and behavioral flexibility
    float modulation = pImpl->level;
    
    // Low serotonin reduces plasticity (associated with depression)
    if (modulation < 0.3f) {
        return 0.2f + 0.3f * modulation;
    }
    
    // High serotonin increases flexibility
    return 0.8f + 0.4f * modulation;
}

void Serotonin::update(TimestepDuration dt, const Neuromodulator* coModulator, 
                      float predictionError) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Social context affects serotonin level
    if (pImpl->socialBehaviorStrength > 0.5f) {
        pImpl->level = std::min(pImpl->level + 0.01f * dtFloat, pImpl->peak);
    }
}

void Serotonin::signalMood(float mood) {
    pImpl->moodLevel = std::clamp(mood, 0.0f, 1.0f);
    
    // Good mood increases serotonin level
    if (mood > 0.7f) {
        pImpl->level = std::min(pImpl->level + mood * pImpl->releaseRate, pImpl->peak);
    }
    
    // Mood affects behavioral flexibility
    pImpl->behavioralFlexibility = 0.3f + 0.7f * mood;
}

void Serotonin::signalSocialContext(float socialFactor) {
    pImpl->socialBehaviorStrength = std::clamp(socialFactor, 0.0f, 1.0f);
    
    if (socialFactor > 0.5f) {
        // Strong social context increases serotonin
        pImpl->level = std::min(pImpl->level + socialFactor * pImpl->releaseRate * 0.5f, pImpl->peak);
    }
}

void Serotonin::signalBehavioralFlexibility(float flexibility) {
    pImpl->behavioralFlexibility = std::clamp(flexibility, 0.0f, 1.0f);
    
    // High flexibility reduces impulsivity
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - flexibility * 0.3f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getSocialBehaviorStrength() const {
    return pImpl->socialBehaviorStrength;
}

float Serotonin::getBehavioralFlexibility() const {
    return pImpl->behavioralFlexibility;
}

} // namespace nlm

