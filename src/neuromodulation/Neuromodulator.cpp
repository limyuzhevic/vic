#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float tonicLevel;
    float phasicLevel;
    float predictionError;
    float expectedValue;
    float rewardPredictionError;
    
    // Dynamics parameters
    float decayRate;
    float releaseRate;
    float reuptakeRate;
    float synthesisRate;
    float refractoriness;
    
    // Time integration
    float timeSinceLastSpike;
    float integrationWindow;
    float lastTime;
    
    // Eligibility traces for learning
    std::vector<float> eligibilityTrace;
    float traceDecay;
    float traceDecayRate;
    
    // Plasticity modulation
    float stdpLTPWeight;
    float stdpLTDWeight;
    float learningRateModifier;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.1f), tonicLevel(0.1f), phasicLevel(0.0f), 
             predictionError(0.0f), expectedValue(0.0f), rewardPredictionError(0.0f),
             decayRate(0.05f), releaseRate(1.0f), reuptakeRate(0.3f), 
             synthesisRate(0.01f), refractoriness(0.0f), timeSinceLastSpike(0.0f),
             integrationWindow(1.0f), lastTime(0.0f), traceDecay(0.9f),
             traceDecayRate(0.98f), stdpLTPWeight(0.01f), stdpLTDWeight(0.012f),
             learningRateModifier(1.0f)
    {
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        eligibilityTrace.resize(1000, 0.0f);
    }
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
    // Implement biologically plausible dopamine-modulated plasticity factor
    // Phasic dopamine (reward prediction error) strongly modulates plasticity
    // Tonic dopamine (baseline) sets general plasticity level
    
    // Phasic component scales with reward prediction error
    float phasicContribution = std::abs(pImpl->rewardPredictionError) * 2.0f;
    
    // Tonic component provides baseline
    float tonicContribution = pImpl->tonicLevel;
    
    // Total dopamine level (phasic + tonic, normalized)
    float totalDA = std::min(pImpl->phasicLevel + tonicContribution, 1.0f);
    
    // Plasticity is highest at moderate dopamine levels (inverted U-curve)
    // This matches empirical findings about optimal learning
    float optimal = 0.5f;
    float deviation = std::abs(totalDA - optimal);
    float plasticity = 1.0f - deviation * deviation * 2.0f;
    
    // Ensure plasticity stays within bounds
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Update time since last spike
    pImpl->timeSinceLastSpike += timeStep;
    pImpl->lastTime += timeStep;
    
    // Natural decay (reuptake and metabolism)
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->reuptakeRate * timeStep);
    
    // Decay of phasic component
    pImpl->phasicLevel *= (1.0f - pImpl->decayRate * timeStep);
    
    // Decay eligibility traces
    for (auto& trace : pImpl->eligibilityTrace) {
        trace *= pImpl->traceDecayRate;
    }
    
    // Update prediction error signal
    pImpl->predictionError = pImpl->rewardPredictionError;
}

void Dopamine::signalReward(float reward) {
    // Implement reward prediction error signaling
    // Phasic dopamine bursts for unexpected rewards
    
    float expected = pImpl->expectedValue;
    
    // Compute reward prediction error (delta rule)
    pImpl->rewardPredictionError = reward - expected;
    
    // Update expected value with temporal averaging
    pImpl->expectedValue = 0.9f * pImpl->expectedValue + 0.1f * reward;
    
    // Generate phasic dopamine response
    // Stronger response for positive prediction errors
    if (pImpl->rewardPredictionError > 0.0f) {
        // Phasic burst for reward prediction error
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + pImpl->releaseRate * pImpl->rewardPredictionError, 1.0f);
        
        // Spike probability increases with prediction error magnitude
        float spikeProbability = std::min(pImpl->rewardPredictionError * 2.0f, 1.0f);
        if (pImpl->uniformDist(pImpl->rng) < spikeProbability) {
            // Actual spike event
            pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->rewardPredictionError * 2.0f);
            
            // Update eligibility trace for this spike
            if (!pImpl->eligibilityTrace.empty()) {
                pImpl->eligibilityTrace.back() = 1.0f;
            }
        }
    } else {
        // Phasic dip for negative prediction errors
        pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - pImpl->decayRate * pImpl->rewardPredictionError);
        
        // Decrease dopamine level on prediction error
        pImpl->level = std::max(pImpl->baseline, pImpl->level + pImpl->rewardPredictionError * 0.5f);
    }
    
    NLM_LOG_DEBUG("Dopamine reward signal: reward=%f, expected=%f, RPE=%f, phasic=%f", 
                  reward, expected, pImpl->rewardPredictionError, pImpl->phasicLevel);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling
    // Used when prediction error is computed elsewhere
    
    pImpl->rewardPredictionError = error;
    
    // Generate appropriate dopamine response
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + pImpl->releaseRate * error, 1.0f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * 1.5f);
        
        // Learning signal
        if (pImpl->uniformDist(pImpl->rng) < std::min(error * 2.0f, 1.0f)) {
            if (!pImpl->eligibilityTrace.empty()) {
                pImpl->eligibilityTrace.back() = 1.0f;
            }
        }
    } else {
        // Negative prediction error (worse than expected)
        pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel + pImpl->decayRate * error);
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * 0.5f);
    }
    
    // Update expected value (running average)
    pImpl->expectedValue += 0.1f * error;
    pImpl->expectedValue = std::clamp(pImpl->expectedValue, 0.0f, 1.0f);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

const std::vector<float>& Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::updateEligibilityTrace(float traceValue) {
    if (!pImpl->eligibilityTrace.empty()) {
        pImpl->eligibilityTrace.back() = traceValue;
    }
}

void Dopamine::decayEligibilityTrace(float decayFactor) {
    float decay = std::clamp(decayFactor, 0.0f, 1.0f);
    for (auto& trace : pImpl->eligibilityTrace) {
        trace *= decay;
    }
}

float Dopamine::getPhasicLevel() const {
    return pImpl->phasicLevel;
}

float Dopamine::getTonicLevel() const {
    return pImpl->tonicLevel;
}

float Dopamine::getRefractoriness() const {
    return pImpl->refractoriness;
}

void Dopamine::setRefractoriness(float refractoriness) {
    pImpl->refractoriness = std::clamp(refractoriness, 0.0f, 1.0f);
}

void Dopamine::setTraceDecayRate(float decayRate) {
    pImpl->traceDecayRate = std::clamp(decayRate, 0.5f, 0.999f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
    
    // Clear eligibility traces
    for (auto& trace : pImpl->eligibilityTrace) {
        trace = 0.0f;
    }
    
    NLM_LOG_INFO("Dopamine system reset");
}

} // namespace nlm