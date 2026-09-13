#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Prediction error tracking
    float getPredictionError() const;
    float getPredictionErrorBaseline() const;
    float getEligibilityTrace() const;
    void updatePrediction(float predictedReward);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

inline Dopamine::Dopamine() : pImpl(new Impl) {
    pImpl->predictionErrorBaseline = 0.0f;
    pImpl->currentStep = 0;
}

inline Dopamine::~Dopamine() = default;

inline const char* Dopamine::getName() const {
    return "DA";
}

inline float Dopamine::getLevel() const {
    return pImpl->level;
}

inline void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

inline float Dopamine::getPlasticityFactor() const {
    float modFactor = 0.5f + 0.5f * pImpl->predictionError;
    return std::clamp(modFactor, 0.1f, 2.0f);
}

inline void Dopamine::update(TimestepDuration dt) {
    TimestepDuration currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->eligibility *= pImpl->traceDecay;
    pImpl->predictionErrorBaseline *= pImpl->traceDecay;
    pImpl->predictionErrorBaseline = std::max(0.0f, pImpl->predictionErrorBaseline);
}

inline void Dopamine::signalReward(float reward) {
    float error = reward - pImpl->expectedReward;
    
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * reward;
    
    pImpl->predictionErrorBaseline = 0.9f * pImpl->predictionErrorBaseline + 0.1f * std::abs(error);
    
    float normalizedError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    pImpl->predictionError = normalizedError;
    
    pImpl->eligibility += reward * 0.1f;
    
    Timestamp currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    if (std::abs(error) > pImpl->burstThreshold && 
        (currentTime - pImpl->lastBurstTime) > 1.0f) {
        
        float burst = std::min(pImpl->peakLevel, 0.5f + std::abs(error) * 2.0f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + burst * pImpl->releaseRate);
        pImpl->lastBurstTime = currentTime;
        
        NLM_LOG_DEBUG(std::string("Dopamine burst: ") + std::to_string(burst) + 
                     std::string(" (reward prediction error: ") + std::to_string(error) + std::string(")"));
    } else {
        float gradual = std::min(0.2f, std::abs(error) * 0.5f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + gradual * pImpl->releaseRate);
    }
}

inline void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    pImpl->eligibility += error * 0.1f;
    
    float errorMagnitude = std::abs(error);
    
    if (error > 0.0f) {
        float increase = std::min(0.5f, errorMagnitude * 0.3f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + increase * pImpl->releaseRate);
    } else {
        float decrease = std::min(0.3f, errorMagnitude * 0.2f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease * pImpl->releaseRate);
    }
    
    pImpl->rewardHistory = error;
}

inline float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

inline float Dopamine::getPredictionErrorBaseline() const {
    return pImpl->predictionErrorBaseline;
}

inline float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibility;
}

inline void Dopamine::updatePrediction(float predictedReward) {
    pImpl->expectedReward = predictedReward;
}

inline void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->predictionErrorBaseline = 0.0f;
    pImpl->eligibility = 0.0f;
    pImpl->rewardHistory = 0.0f;
}

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention control
    void enhanceAttention(float strength);
    void strengthenMemoryTrace(float trace);
    
    // Alpha rhythms and thalamocortical integration
    void modulateAlphaRhythms(float frequency, float coherence);
    void integrateWithThalamocorticalLoop(float corticalInput);
    
    // Memory consolidation during sleep/REST
    void consolidateMemories(float consolidationStrength);
    void supportWorkingMemory(float maintenanceEnergy);
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

inline Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    pImpl->level = 0.5f;
}

inline Acetylcholine::~Acetylcholine() = default;

inline const char* Acetylcholine::getName() const {
    return "ACh";
}

inline float Acetylcholine::getLevel() const {
    return pImpl->level;
}

inline void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

inline float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + pImpl->attentionLevel * 0.5f;
}

inline void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionLevel *= pImpl->traceDecay;
}

inline void Acetylcholine::reset() {
    pImpl->level = 0.5f;
    pImpl->attentionLevel = 0.0f;
    pImpl->memoryTraceStrength = 0.0f;
}

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Vigilance and alertness
    void increaseVigilance(float alertnessLevel);
    void enhanceSensoryProcessing(float enhancement);
    
    // Locus coeruleus - norepinephrine release patterns
    void triggerLocusCoeruleusBurst(float burstIntensity);
    void sustain tonicRelease(float tonicLevel);
    void modulateWithACh(float achLevel);
    
    // Stress response and coping
    void initiateStressResponse(float stressor);
    void supportAdaptiveBehavior(float stressLevel);
    void restoreHomeostasis();
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

inline Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    pImpl->level = 0.3f;
}

inline Norepinephrine::~Norepinephrine() = default;

inline const char* Norepinephrine::getName() const {
    return "NE";
}

inline float Norepinephrine::getLevel() const {
    return pImpl->level;
}

inline void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

inline float Norepinephrine::getPlasticityFactor() const {
    return 1.0f + pImpl->arousalLevel * 0.3f;
}

inline void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel *= pImpl->traceDecay;
}

inline void Norepinephrine::reset() {
    pImpl->level = 0.3f;
    pImpl->arousalLevel = 0.0f;
    pImpl->vigilanceLevel = 0.0f;
}

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void modulateMood(float moodState);
    void reduceAnxiety(float anxietyLevel);
    void enhanceSocialBehavior(float socialCohesion);
    
    // Dorsal raphe nucleus - serotonin release patterns
    void triggerDRNPhasicRelease(float rewardSignal);
    void maintainTonicSerotonin(float baselineLevel);
    void modulateWithNE(float neLevel);
    
    // Impulse control and decision making
    void enhanceImpulseControl(float controlStrength);
    void supportDecisionMaking(float uncertainty);
    void reduceCompulsiveBehavior(float compulsivity);
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

inline Serotonin::Serotonin() : pImpl(new Impl) {
    pImpl->level = 0.4f;
}

inline Serotonin::~Serotonin() = default;

inline const char* Serotonin::getName() const {
    return "5-HT";
}

inline float Serotonin::getLevel() const {
    return pImpl->level;
}

inline void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

inline float Serotonin::getPlasticityFactor() const {
    return 1.0f + pImpl->moodLevel * 0.2f;
}

inline void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->moodLevel *= pImpl->traceDecay;
}

inline void Serotonin::reset() {
    pImpl->level = 0.4f;
    pImpl->moodLevel = 0.0f;
    pImpl->socialCohesion = 0.0f;
}

} // namespace nlm
