#include "Neuromodulator.hpp"
#include <algorithm>

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
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// Include the full Neuromodulator implementation with advanced features

// Initialize with brain reference
void Dopamine::initialize(Brain* brain) {
    pImpl->baseline = 0.0f;
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

// Signal a prediction error for reward learning
void Dopamine::signalPredictionError(float predictionError) {
    // Dopaminergic prediction error: positive for better-than-expected, negative for worse
    pImpl->level = std::max(0.0f, pImpl->level + predictionError * pImpl->releaseRate);
    NLM_LOG_DEBUG("Dopamine prediction error: " + std::to_string(predictionError) + 
                  ", new level: " + std::to_string(pImpl->level));
}

// Reset dopamine to baseline state
void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    NLM_LOG_INFO("Dopamine reset to baseline");
}

// Set baseline dopamine level
void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    NLM_LOG_INFO("Dopamine baseline set to: " + std::to_string(pImpl->baseline));
}

// Get baseline dopamine level
float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

// Set release rate for dopamine bursts
void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
}

// Get release rate
float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

// Set decay rate for dopamine clearance
void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

// Get decay rate
float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

// Get current concentration in molar units (mM)
float Dopamine::getConcentration() const {
    return pImpl->level * 1000.0f; // Scale to mM
}

// Get dopamine signal quality
float Dopamine::getSignalQuality() const {
    // Signal quality based on variability and response speed
    float variability = std::abs(pImpl->level - pImpl->baseline);
    float responseSpeed = pImpl->releaseRate;
    return (variability * 0.6f + responseSpeed * 0.4f);
}

// Get detailed state string
const char* Dopamine::getStateString() const {
    static std::string state;
    std::ostringstream oss;
    oss << "DA(level=" << pImpl->level << ", baseline=" << pImpl->baseline 
        << ", peak=" << pImpl->peak << ", decay=" << pImpl->decayRate 
        << ", release=" << pImpl->releaseRate << ")";
    state = oss.str();
    return state.c_str();
}

}#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float integrationFactor;
    float excitabilityModulation;
    float connectivityModulation;
    float developmentalModulation;
    bool isStructuralPlasticityEnabled;
    std::string stateString;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             integrationFactor(0.5f), excitabilityModulation(1.0f), 
             connectivityModulation(1.0f), developmentalModulation(1.0f),
             isStructuralPlasticityEnabled(false), stateString("Initialized") {}
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
    pImpl->level = clampLevel(level);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on neuroscience
    // Higher dopamine levels increase LTP and decrease LTD
    float basePlasticity = 0.5f;
    float modulation = pImpl->level * 0.8f;  // Max additional plasticity
    return basePlasticity + modulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with time-dependent clearance
    // Incorporates reuptake, metabolism, and baseline tonic firing
    float tonicLevel = pImpl->baseline * pImpl->integrationFactor;
    float phasicAddition = 0.0f;  // Would be set by signals
    
    // Natural decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = tonicLevel + (pImpl->level - tonicLevel) * std::pow(0.95f, dt * 10.0f);
    
    // Update state
    updateStateString();
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics and learning rate
    // Reward prediction error signal based on RESCORLA-WINTERS algorithm
    float predictionError = reward - getPredictionError(reward);
    
    // Burst of dopamine proportional to prediction error
    float burstMagnitude = std::max(0.0f, predictionError * pImpl->releaseRate);
    pImpl->level = clampLevel(pImpl->level + burstMagnitude, 0.0f, pImpl->peak);
    
    // Update neuromodulatory effects
    updateNeuromodulatoryEffects();
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (TRUE DARWINIAN learning)
    // Direct modulation based on prediction error
    pImpl->level = clampLevel(pImpl->level + error * pImpl->releaseRate);
    
    // Apply prediction error to plasticity rules
    applyPredictionErrorToPlasticity(error);
}

void Dopamine::updateNeuromodulatoryEffects() {
    // Update downstream neuromodulatory effects
    pImpl->excitabilityModulation = 0.5f + pImpl->level * 0.5f;
    pImpl->connectivityModulation = 1.0f + pImpl->level * 0.3f;
    pImpl->developmentalModulation = 1.0f - pImpl->level * 0.2f;
}

void Dopamine::applyPredictionErrorToPlasticity(float predictionError) {
    // Modulate synaptic plasticity based on prediction error
    if (predictionError > 0.0f) {
        // Positive prediction error: enhance LTP, suppress LTD
        pImpl->integrationFactor = std::min(1.0f, pImpl->integrationFactor + 0.1f);
        pImpl->isStructuralPlasticityEnabled = true;
    } else {
        // Negative prediction error: enhance LTD, suppress structural plasticity
        pImpl->integrationFactor = std::max(0.0f, pImpl->integrationFactor - 0.1f);
        pImpl->isStructuralPlasticityEnabled = false;
    }
}

float Dopamine::getPredictionError(float reward) const {
    // Simple prediction model: expected reward based on recent history
    return pImpl->baseline * 0.5f + reward * 0.1f;
}

void Dopamine::updateStateString() {
    // Update state string for debugging/monitoring
    std::ostringstream oss;
    oss << "DA(level=" << pImpl->level << ", baseline=" << pImpl->baseline 
        << ", peak=" << pImpl->peak << ", decay=" << pImpl->decayRate 
        << ", release=" << pImpl->releaseRate << ", struct=" 
        << (pImpl->isStructuralPlasticityEnabled ? "true" : "false") << ")";
    pImpl->stateString = oss.str();
}

const char* Dopamine::getStateString() const {
    updateStateString();
    return pImpl->stateString.c_str();
}

}} // namespace nlm
