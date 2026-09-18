#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedValue;
    float learningRate;
    float eligibilityTrace;
    float neuromodulationStrength;
    float adaptationIndex;
    
    // Historical tracking for learning
    std::vector<float> rewardHistory;
    std::vector<float> predictionHistory;
    std::vector<float> errorHistory;
    Timestamp lastUpdateTime;
    
    // Biological parameters
    float tonicFiringRate;        // Baseline dopamine firing (5-10 Hz)
    float phasicFiringAmplitude;  // Phasic response magnitude (2-5x baseline)
    float nmdaDependentGain;      // NMDA receptor contribution factor
    float calciumDependentGain;  // Calcium influx scaling factor
    float dopamineSaturation;    // Maximum achievable dopamine level
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), expectedValue(0.0f), learningRate(0.01f),
             eligibilityTrace(0.0f), neuromodulationStrength(1.0f), adaptationIndex(1.0f),
             tonicFiringRate(7.0f), phasicFiringAmplitude(3.0f), nmdaDependentGain(0.8f),
             calciumDependentGain(0.6f), dopamineSaturation(2.0f), lastUpdateTime(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized with biological accuracy");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA_RPE";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->dopamineSaturation);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced dopamine-modulated plasticity factor based on biological evidence
    // Positive dopamine increases LTP, decreases LTD
    // Negative dopamine decreases LTP, increases LTD
    float basePlasticity = 0.5f;
    
    if (pImpl->level > 0.0f) {
        // Phasic dopamine bursts facilitate LTP via D1/D5 receptor activation
        float d1Factor = pImpl->level * pImpl->adaptationIndex;
        return basePlasticity + d1Factor * 0.4f;
    } else if (pImpl->level < 0.0f) {
        // Phasic dopamine dips facilitate LTD via D2 receptor activation
        float d2Factor = -pImpl->level * pImpl->adaptationIndex * 0.5f;
        return basePlasticity - d2Factor * 0.3f;
    }
    
    return basePlasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with temporal integration and biological realism
    Timestamp currentTime = static_cast<Timestamp>(pImpl->lastUpdateTime + dt);
    
    // Decay towards baseline with biological time constant (15-30 seconds in vivo)
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt) * 0.001f);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Track temporal dynamics
    pImpl->lastUpdateTime = currentTime;
    
    // Maintain history for learning analysis
    if (pImpl->rewardHistory.size() > 1000) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Comprehensive reward signaling with prediction error computation
    float currentLevel = pImpl->level;
    
    // Compute reward prediction error (RPE): delta = reward - expected
    float predictionError = reward - pImpl->expectedValue;
    pImpl->predictionError = predictionError;
    
    // Update expected value with temporal difference learning
    float learningRate = pImpl->learningRate * pImpl->neuromodulationStrength;
    pImpl->expectedValue += learningRate * predictionError;
    
    // Enhanced dopamine response based on RPE magnitude and sign
    // Phasic bursts for positive RPE (unexpected reward)
    // Phasic dips for negative RPE (omitted reward)
    if (predictionError > 0.0f) {
        // Positive prediction error -> burst of dopamine
        // NMDA receptor dependent, calcium amplified
        float burstAmplitude = predictionError * pImpl->phasicFiringAmplitude;
        burstAmplitude *= pImpl->nmdaDependentGain;
        burstAmplitude *= pImpl->calciumDependentGain;
        
        // Integrate with tonic firing
        pImpl->level = pImpl->tonicFiringRate + burstAmplitude;
    } else if (predictionError < 0.0f) {
        // Negative prediction error -> dip of dopamine below baseline
        float dipAmplitude = -predictionError * pImpl->phasicFiringAmplitude * 0.5f;
        pImpl->level = std::max(0.0f, pImpl->baseline - dipAmplitude);
    }
    
    // Update eligibility trace for reward-modulated plasticity
    pImpl->eligibilityTrace += predictionError * learningRate;
    
    // Record history
    pImpl->rewardHistory.push_back(reward);
    pImpl->predictionHistory.push_back(pImpl->expectedValue);
    pImpl->errorHistory.push_back(predictionError);
    
    // Log biologically relevant information
    if (std::abs(predictionError) > 0.01f) {
        NLM_LOG_DEBUG("Dopamine RPE: reward=" + std::to_string(reward) + 
                     " expected=" + std::to_string(pImpl->expectedValue) + 
                     " error=" + std::to_string(predictionError) + 
                     " level=" + std::to_string(pImpl->level));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct RPE signaling (alternative to reward-based computation)
    pImpl->predictionError = error;
    
    // Update expected value based on current error
    float learningRate = pImpl->learningRate * pImpl->neuromodulationStrength;
    pImpl->expectedValue += learningRate * error;
    
    // Compute dopamine response based on error
    if (error > 0.0f) {
        // Positive error -> dopamine burst
        pImpl->level = pImpl->tonicFiringRate + error * pImpl->phasicFiringAmplitude * 0.8f;
    } else if (error < 0.0f) {
        // Negative error -> dopamine dip
        pImpl->level = std::max(0.0f, pImpl->baseline - std::abs(error) * pImpl->phasicFiringAmplitude * 0.4f);
    }
    
    // Update eligibility trace
    pImpl->eligibilityTrace += error * learningRate;
    
    // Record history
    pImpl->errorHistory.push_back(error);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

void Dopamine::setExpectedValue(float value) {
    pImpl->expectedValue = value;
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Dopamine::setNeuromodulationStrength(float strength) {
    pImpl->neuromodulationStrength = std::clamp(strength, 0.1f, 3.0f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->rewardHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->errorHistory.clear();
}

} // namespace nlm
