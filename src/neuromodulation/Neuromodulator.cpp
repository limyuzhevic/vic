#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Phasic and tonic components
    float tonicLevel;      // Baseline tonic firing
    float phasicLevel;     // Phasic burst component
    float baseline;        // Target baseline
    float peak;           // Maximum achievable level
    
    // Dynamics parameters
    float decayRate;       // Decay toward baseline
    float riseRate;        // Rise rate on positive prediction error
    float adaptationRate;  // Rate of adaptation to new baseline
    float historyRate;     // Rate of prediction error history integration
    float volatility;      // Parameter for prediction error scaling
    
    // State tracking
    float predictionError; // Current reward prediction error
    float rewardHistory;   // Recent reward history
    float predictionHistory; // Recent prediction history
    float memoryTrace;     // Memory trace for credit assignment
    float eligibilityTrace; // Eligibility trace for learning
    
    // Biological parameters (based on striatal dopamine models)
    float tonicFiring;     // Basal tonic firing rate (Hz)
    float peakFiring;      // Maximum phasic firing (Hz)
    float responseTime;    // Response time constant (s)
    float integrationTime; // Integration time constant (s)
    
    Impl() : tonicLevel(0.2f), phasicLevel(0.0f), baseline(0.2f), peak(1.0f),
             decayRate(0.05f), riseRate(2.0f), adaptationRate(0.01f), 
             historyRate(0.1f), volatility(0.5f),
             predictionError(0.0f), rewardHistory(0.0f), 
             predictionHistory(0.0f), memoryTrace(0.0f), eligibilityTrace(0.0f),
             tonicFiring(20.0f), peakFiring(200.0f), responseTime(0.1f),
             integrationTime(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine tonic and phasic components biologically realistically
    return pImpl->tonicLevel + 0.7f * pImpl->phasicLevel;
}

void Dopamine::setLevel(float level) {
    // Set both tonic and phasic components based on total level
    pImpl->tonicLevel = level * 0.3f;
    pImpl->phasicLevel = level * 0.7f;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on DL-PCR2 model
    // Dopamine facilitates LTP via D1 receptor, inhibits LTD via D2 receptor
    float daLevel = getLevel();
    
    // D1-mediated potentiation (linear increase with DA)
    float d1Effect = 1.0f + 2.0f * daLevel;  // Up to 3x potentiation
    
    // D2-mediated depression (increases with lower DA)
    float d2Effect = 1.0f - 1.5f * daLevel;  // Down to 0.5x depression
    
    // Net plasticity factor balances both receptor effects
    float netPlasticity = 0.6f * d1Effect + 0.4f * d2Effect;
    
    // Clamp to reasonable biological range
    return std::clamp(netPlasticity, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float deltaT = static_cast<float>(dt) / 1000.0f; // Convert to seconds
    
    // Update phasic component with biological dynamics
    if (pImpl->phasicLevel > 0.0f) {
        // Phasic component decays exponentially
        float phasicDecay = 1.0f - std::exp(-deltaT / responseTime);
        pImpl->phasicLevel *= (1.0f - phasicDecay);
    }
    
    // Update tonic component with adaptation
    pImpl->baseline += (pImpl->tonicLevel - pImpl->baseline) * 
                      adaptationRate * deltaT * 100.0f; // Scale for continuous time
    
    // Integrate prediction error into tonic component
    if (std::abs(pImpl->predictionError) > 0.01f) {
        pImpl->baseline += pImpl->predictionError * historyRate * deltaT * 100.0f;
        pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 1.0f);
    }
    
    // Update tonic level with biological noise
    float noise = (rand() / (RAND_MAX + 1.0f) - 0.5f) * 0.02f;
    pImpl->tonicLevel += noise;
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Biological reward signaling with response latency
    float responseDelay = responseTime;
    float peakResponse = std::clamp(reward * 2.0f, 0.0f, 1.0f);
    
    // Phasic burst proportional to reward magnitude and prediction error
    float predictionError = reward - pImpl->predictionHistory;
    
    // Update reward history with temporal discounting
    pImpl->rewardHistory = 0.9f * pImpl->rewardHistory + 0.1f * reward;
    
    // Generate phasic response
    pImpl->phasicLevel = peakResponse * std::min(1.0f, reward / volatility);
    
    // Update memory trace for delayed reward effects
    pImpl->memoryTrace += reward * (1.0f - responseDelay);
    
    // Set prediction error for next update
    pImpl->predictionError = predictionError;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling following Schultz model
    // Phasic increase for positive prediction error, decrease for negative
    
    // Scale error by volatility parameter
    float scaledError = error * volatility;
    
    // Phasic response with asymmetric scaling (more sensitive to unexpected rewards)
    if (error > 0.0f) {
        // Positive prediction error: stronger response
        pImpl->phasicLevel += scaledError * riseRate;
    } else {
        // Negative prediction error: weaker response
        pImpl->phasicLevel += scaledError * (riseRate * 0.5f);
    }
    
    // Update prediction error state
    pImpl->predictionError = error;
    
    // Update prediction history
    pImpl->predictionHistory = 0.9f * pImpl->predictionHistory + 0.1f * error;
    
    // Update eligibility trace for credit assignment
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - decayRate * static_cast<float>(1.0f)) +
                             std::abs(error) * historyRate;
}

} // namespace nlm
