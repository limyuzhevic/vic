#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct AdvancedDopamineImpl {
    // Dopamine dynamics parameters
    float level;                    // Current dopamine level
    float baseline;                 // Tonic baseline level
    float phasicPeak;               // Peak of phasic response
    float phasicWidth;              // Width of phasic response
    float phasicRiseTime;           // Time to reach peak
    float phasicDecayTime;          // Time to decay to baseline
    float decayRate;                // Tonic decay rate
    float releaseRate;              // Phasic release rate
    
    // Plasticity modulation parameters
    float plasticityGain;           // Overall gain on plasticity
    float calciumCoupling;          // Coupling to calcium dynamics
    float eligibilityTrace;         // Eligibility trace for R-STDP
    float neuromodulationTimeConstant; // Time constant for neuromodulation
    
    // State tracking
    float phasicActiveTime;         // Time since phasic burst started
    float lastPhasicEvent;          // Time of last phasic event
    float tonicLevel;               // Current tonic level
    
    // History for computation
    std::vector<float> levelHistory;    // Historical dopamine levels
    std::vector<float> plasticityFactors; // Historical plasticity factors
    
    // Biological parameters
    static constexpr float MIN_LEVEL = 0.0f;
    static constexpr float MAX_LEVEL = 2.0f;           // Phasic responses can exceed baseline
    static constexpr float DEFAULT_BASELINE = 0.1f;
    static constexpr float DEFAULT_PEAK = 1.5f;
    static constexpr float DEFAULT_WIDTH = 100.0f;     // ms
    static constexpr float DEFAULT_RISE = 20.0f;       // ms
    static constexpr float DEFAULT_DECAY = 200.0f;     // ms
    static constexpr float DEFAULT_DECAY_RATE = 0.05f; // /ms
    static constexpr float DEFAULT_RELEASE_RATE = 2.0f; // /ms
    static constexpr float DEFAULT_GAIN = 1.0f;
    static constexpr float DEFAULT_CALCIUM_COUPLING = 0.8f;
    static constexpr float DEFAULT_TRACE_DECAY = 0.01f; // /ms
    
    AdvancedDopamineImpl() {
        // Initialize with biologically realistic values
        baseline = DEFAULT_BASELINE;
        phasicPeak = DEFAULT_PEAK;
        phasicWidth = DEFAULT_WIDTH;
        phasicRiseTime = DEFAULT_RISE;
        phasicDecayTime = DEFAULT_DECAY;
        decayRate = DEFAULT_DECAY_RATE;
        releaseRate = DEFAULT_RELEASE_RATE;
        plasticityGain = DEFAULT_GAIN;
        calciumCoupling = DEFAULT_CALCIUM_COUPLING;
        neuromodulationTimeConstant = phasicWidth * 2.0f;
        
        level = baseline;
        tonicLevel = baseline;
        phasicActiveTime = 0.0f;
        lastPhasicEvent = -1000.0f;
        eligibilityTrace = 0.0f;
        
        // Initialize history buffers
        levelHistory.reserve(1000);
        plasticityFactors.reserve(1000);
        levelHistory.push_back(level);
    }
};

Dopamine::Dopamine() : pImpl(new AdvancedDopamineImpl()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, AdvancedDopamineImpl::MIN_LEVEL, AdvancedDopamineImpl::MAX_LEVEL);
}

float Dopamine::getPlasticityFactor() const {
    // Advanced dopamine-modulated plasticity factor
    // Implements biologically realistic dopamine effects on synaptic plasticity
    
    float normalizedLevel = std::clamp(pImpl->level / AdvancedDopamineImpl::DEFAULT_PEAK, 0.0f, 1.0f);
    
    // Multiplicative modulation of plasticity
    float plasticityFactor = 0.5f + pImpl->plasticityGain * normalizedLevel;
    
    // Calcium-dependent modulation
    plasticityFactor *= (1.0f + pImpl->calciumCoupling * std::min(1.0f, pImpl->level));
    
    // Eligibility trace integration
    plasticityFactor *= (1.0f + pImpl->eligibilityTrace * 0.5f);
    
    // Clamp to reasonable bounds
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float timestep = static_cast<float>(dt) * 1000.0f; // Convert to ms
    
    // Update eligibility trace decay
    pImpl->eligibilityTrace *= std::exp(-pImpl->neurotransmissionTimeConstant * timestep);
    
    // Compute tonic dopamine dynamics (slow baseline changes)
    float targetTonic = baseline + computeTonicTarget();
    float tonicChange = (targetTonic - pImpl->tonicLevel) * pImpl->decayRate * timestep;
    pImpl->tonicLevel += tonicChange;
    
    // Update phasic dopamine if active
    float newLevel = pImpl->tonicLevel;
    
    if (pImpl->phasicActiveTime > 0.0f) {
        pImpl->phasicActiveTime += timestep;
        
        // Calculate phasic component using Gaussian function
        float timeSinceEvent = pImpl->phasicActiveTime - pImpl->lastPhasicEvent;
        float phasicComponent = pImpl->phasicPeak * std::exp(-(timeSinceEvent * timeSinceEvent) / 
                                                           (2.0f * pImpl->phasicWidth * pImpl->phasicWidth));
        
        newLevel += phasicComponent;
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(newLevel, AdvancedDopamineImpl::MIN_LEVEL, AdvancedDopamineImpl::MAX_LEVEL);
    
    // Record level history for analysis
    if (pImpl->levelHistory.size() >= 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
    pImpl->levelHistory.push_back(pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to reward (Rangarajan et al. 2015)
    float currentTime = pImpl->lastPhasicEvent > -1000.0f ? pImpl->phasicActiveTime : 0.0f;
    
    // Compute phasic response magnitude and timing
    float rewardMagnitude = std::clamp(reward, 0.0f, 10.0f); // Cap at 10x baseline
    float responseMagnitude = pImpl->phasicPeak * (1.0f + rewardMagnitude * 0.1f);
    
    // Update phasic state
    pImpl->phasicActiveTime = 0.0f;
    pImpl->lastPhasicEvent = currentTime;
    
    // Set level to peak response
    setLevel(responseMagnitude);
    
    // Update eligibility trace for reward-modulated learning
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + rewardMagnitude * 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signal (Schultz et al. 1997)
    // Positive error (better than expected) → Phasic increase
    // Negative error (worse than expected) → Phasic decrease
    
    float errorMagnitude = std::abs(error);
    float currentTime = pImpl->lastPhasicEvent > -1000.0f ? pImpl->phasicActiveTime : 0.0f;
    
    // Compute prediction error response
    float responseMagnitude = error > 0.0f ? 
        pImpl->phasicPeak * (1.0f + errorMagnitude * 0.15f) : 
        -pImpl->phasicPeak * errorMagnitude * 0.5f; // Can go below baseline
    
    // Update phasic state
    pImpl->phasicActiveTime = 0.0f;
    pImpl->lastPhasicEvent = currentTime;
    
    // Set level to prediction error response
    setLevel(responseMagnitude);
    
    // Update eligibility trace for prediction error learning
    float traceChange = error * pImpl->neurotransmissionTimeConstant;
    pImpl->eligibilityTrace = std::max(-1.0f, std::min(1.0f, pImpl->eligibilityTrace + traceChange * 0.01f));
}

float Dopamine::computeTonicTarget() {
    // Compute tonic dopamine target based on behavioral state
    // Implemented as a simple homeostatic control system
    
    // In real implementation, this would integrate various signals
    // including arousal, motivation, stress levels, etc.
    
    return pImpl->baseline;
}

} // namespace nlm
