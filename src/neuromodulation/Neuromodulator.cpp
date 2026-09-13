#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ==================== DOPAMINE IMPLEMENTATION ====================

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedReward;
    float predictionErrorBaseline;
    float eligibility;
    float traceDecay;
    float burstThreshold;
    Timestamp lastBurstTime;
    TimestepDuration currentStep;
    
    Impl() 
        : level(0.0f), baseline(0.005f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionError(0.0f), expectedReward(0.0f), predictionErrorBaseline(0.0f)
        , eligibility(0.0f), traceDecay(0.95f), burstThreshold(0.5f)
        , lastBurstTime(-10.0f), currentStep(0) {}
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
    // Enhanced dopamine-modulated plasticity factor
    // Positive prediction error increases plasticity, negative decreases it
    float modFactor = 0.5f + 0.5f * pImpl->predictionError;
    return std::clamp(modFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    TimestepDuration currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay eligibility trace
    pImpl->eligibility *= pImpl->traceDecay;
    
    // Decay prediction error baseline
    pImpl->predictionErrorBaseline *= pImpl->traceDecay;
    pImpl->predictionErrorBaseline = std::max(0.0f, pImpl->predictionErrorBaseline);
    
    pImpl->currentStep++;
}

void Dopamine::signalReward(float reward) {
    // Calculate reward prediction error
    float error = reward - pImpl->expectedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * reward;
    
    // Update prediction error baseline
    pImpl->predictionErrorBaseline = 0.9f * pImpl->predictionErrorBaseline + 0.1f * std::abs(error);
    
    // Compute prediction error (normalized by baseline)
    float normalizedError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    // Update prediction error
    pImpl->predictionError = normalizedError;
    
    // Update eligibility trace for reward-modulated plasticity
    pImpl->eligibility += reward * 0.1f;
    
    // Burst response to unexpected reward
    TimestepDuration currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    if (std::abs(error) > pImpl->burstThreshold && 
        (currentTime - pImpl->lastBurstTime) > 1.0f) {
        
        // Strong burst of dopamine for unexpected reward
        float burst = std::min(pImpl->peak, 0.5f + std::abs(error) * 2.0f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst * pImpl->releaseRate);
        pImpl->lastBurstTime = currentTime;
    } else {
        // Gradual dopamine update for expected reward
        float gradual = std::min(0.2f, std::abs(error) * 0.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + gradual * pImpl->releaseRate);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct signal of reward prediction error
    pImpl->predictionError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    // Update eligibility trace based on prediction error
    pImpl->eligibility += error * 0.1f;
    
    // Modulate dopamine level based on prediction error
    float errorMagnitude = std::abs(error);
    
    if (error > 0.0f) {
        // Positive error (better than expected): increase dopamine
        float increase = std::min(0.5f, errorMagnitude * 0.3f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + increase * pImpl->releaseRate);
    } else {
        // Negative error (worse than expected): decrease dopamine
        float decrease = std::min(0.3f, errorMagnitude * 0.2f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease * pImpl->releaseRate);
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getPredictionErrorBaseline() const {
    return pImpl->predictionErrorBaseline;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibility;
}

void Dopamine::updatePrediction(float predictedReward) {
    pImpl->expectedReward = predictedReward;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->predictionErrorBaseline = 0.0f;
    pImpl->eligibility = 0.0f;
    pImpl->lastBurstTime = -10.0f;
}

// ==================== ACETYLCHOLINE IMPLEMENTATION ====================

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionLevel;
    float memoryTraceStrength;
    float traceDecay;
    float enhancementRate;
    
    Impl()
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f)
        , attentionLevel(0.0f), memoryTraceStrength(0.0f)
        , traceDecay(0.95f), enhancementRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + pImpl->attentionLevel * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionLevel *= pImpl->traceDecay;
    pImpl->memoryTraceStrength *= pImpl->traceDecay;
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + strength);
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->enhancementRate);
}

void Acetylcholine::strengthenMemoryTrace(float trace) {
    pImpl->memoryTraceStrength = std::min(1.0f, pImpl->memoryTraceStrength + trace);
    pImpl->level = std::min(pImpl->peak, pImpl->level + trace * pImpl->enhancementRate * 0.5f);
}

void Acetylcholine::modulateAlphaRhythms(float frequency, float coherence) {
    // Integrate with thalamocortical loop
    pImpl->level = std::min(pImpl->peak, pImpl->level + coherence * 0.3f);
}

void Acetylcholine::integrateWithThalamocorticalLoop(float corticalInput) {
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + corticalInput * 0.2f);
}

void Acetylcholine::consolidateMemories(float consolidationStrength) {
    pImpl->memoryTraceStrength = std::min(1.0f, pImpl->memoryTraceStrength + consolidationStrength * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * pImpl->enhancementRate * 0.3f);
}

void Acetylcholine::supportWorkingMemory(float maintenanceEnergy) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + maintenanceEnergy * pImpl->enhancementRate * 0.2f);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionLevel = 0.0f;
    pImpl->memoryTraceStrength = 0.0f;
}

// ==================== NOREPINEPHRASE IMPLEMENTATION ====================

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    float traceDecay;
    float burstIntensity;
    float tonicLevel;
    
    Impl()
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f)
        , arousalLevel(0.0f), vigilanceLevel(0.0f)
        , traceDecay(0.95f), burstIntensity(1.0f), tonicLevel(0.5f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    return 1.0f + pImpl->arousalLevel * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel *= pImpl->traceDecay;
    pImpl->vigilanceLevel *= pImpl->traceDecay;
}

void Norepinephrine::increaseVigilance(float alertnessLevel) {
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + alertnessLevel);
    pImpl->level = std::min(pImpl->peak, pImpl->level + alertnessLevel * pImpl->burstIntensity);
}

void Norepinephrine::enhanceSensoryProcessing(float enhancement) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + enhancement * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement * pImpl->burstIntensity * 0.5f);
}

void Norepinephrine::triggerLocusCoeruleusBurst(float burstIntensity) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstIntensity * pImpl->burstIntensity);
}

void Norepinephrine::sustainTonicRelease(float tonicLevel) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + (tonicLevel - pImpl->level) * 0.1f);
}

void Norepinephrine::modulateWithACh(float achLevel) {
    // NE modulates ACh and vice versa
    float influence = 0.3f * achLevel;
    pImpl->level = std::min(pImpl->peak, pImpl->level + influence);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + influence * 0.5f);
}

void Norepinephrine::initiateStressResponse(float stressor) {
    float stressResponse = std::min(1.0f, stressor * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse * pImpl->burstIntensity);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + stressResponse);
}

void Norepinephrine::supportAdaptiveBehavior(float stressLevel) {
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + stressLevel * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->tonicLevel * 0.1f);
}

void Norepinephrine::restoreHomeostasis() {
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 0.5f;
    pImpl->arousalLevel *= 0.7f;
    pImpl->vigilanceLevel *= 0.7f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.0f;
    pImpl->vigilanceLevel = 0.0f;
}

// ==================== SEROTONIN IMPLEMENTATION ====================

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float socialCohesion;
    float traceDecay;
    float rewardModulation;
    float impulseControl;
    
    Impl()
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f)
        , moodLevel(0.0f), socialCohesion(0.0f)
        , traceDecay(0.95f), rewardModulation(0.5f), impulseControl(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f + pImpl->moodLevel * 0.2f;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->moodLevel *= pImpl->traceDecay;
    pImpl->socialCohesion *= pImpl->traceDecay;
}

void Serotonin::modulateMood(float moodState) {
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + moodState);
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodState * pImpl->rewardModulation);
}

void Serotonin::reduceAnxiety(float anxietyLevel) {
    float anxietyReduction = std::min(1.0f, anxietyLevel * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + anxietyReduction * pImpl->rewardModulation * 0.5f);
}

void Serotonin::enhanceSocialBehavior(float socialCohesion) {
    pImpl->socialCohesion = std::min(1.0f, pImpl->socialCohesion + socialCohesion);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * pImpl->rewardModulation * 0.3f);
}

void Serotonin::triggerDRNPhasicRelease(float rewardSignal) {
    float release = std::min(1.0f, rewardSignal * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + release * pImpl->rewardModulation);
}

void Serotonin::maintainTonicSerotonin(float baselineLevel) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + (baselineLevel - pImpl->level) * 0.05f);
}

void Serotonin::modulateWithNE(float neLevel) {
    // Serotonin modulates NE activity
    float influence = 0.2f * neLevel;
    pImpl->level = std::min(pImpl->peak, pImpl->level + influence);
}

void Serotonin::enhanceImpulseControl(float controlStrength) {
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + controlStrength * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + controlStrength * pImpl->rewardModulation * 0.2f);
}

void Serotonin::supportDecisionMaking(float uncertainty) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + uncertainty * pImpl->rewardModulation * 0.1f);
}

void Serotonin::reduceCompulsiveBehavior(float compulsivity) {
    float reduction = std::min(1.0f, compulsivity * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + reduction * pImpl->rewardModulation * 0.2f);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodLevel = 0.0f;
    pImpl->socialCohesion = 0.0f;
    pImpl->impulseControl = 1.0f;
}

} // namespace nlm
