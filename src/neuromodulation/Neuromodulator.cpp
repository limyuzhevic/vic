#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Phasic component - transient responses to rewards and prediction errors
    struct {
        float level;              // Current phasic level
        float target;             // Target phasic level based on RPE
        float tau;                // Time constant for phasic dynamics
        float maxResponse;        // Maximum phasic response
        float baseline;           // Baseline phasic activity
    } phasic;
    
    // Tonic component - baseline dopamine level
    struct {
        float level;              // Current tonic level
        float target;             // Target tonic level (arousal/cognition)
        float tau;                // Time constant for tonic dynamics
        float baseline;           // Baseline tonic activity
        float modulationFactor;   // Modulation by arousal and cognition
    } tonic;
    
    // Reuptake and receptor dynamics
    struct {
        float reuptakeRate;        // Rate of dopamine reuptake by transporters
        float diffusionRate;       // Rate of extracellular diffusion
        float d1ReceptorDensity;  // D1 receptor density
        float d2ReceptorDensity;  // D2 receptor density
        float d1Activation;        // D1 receptor activation level
        float d2Activation;        // D2 receptor activation level
        std::vector<float> history; // History of extracellular dopamine levels
        size_t historySize;        // Size of history buffer
    } receptor;
    
    // Spatial and temporal filtering
    struct {
        float temporalFilter;     // Temporal filter coefficient
        float spatialFilter;      // Spatial filter for population effects
        float adaptationRate;     // Rate of adaptation to sustained stimuli
    } filtering;
    
    // Reward prediction error computation
    struct {
        float lastReward;          // Last received reward
        float predictedReward;     // Predicted reward
        float predictionError;     // Current prediction error
        float learningRate;        // Learning rate for prediction updates
        float eligibilityTrace;    // Eligibility trace for credit assignment
    } rpe;
    
    Impl() {
        // Initialize phasic component
        phasic.level = 0.0f;
        phasic.target = 0.0f;
        phasic.tau = 0.05f;       // Fast dynamics
        phasic.maxResponse = 2.0f;
        phasic.baseline = 0.01f;
        
        // Initialize tonic component
        tonic.level = 0.1f;
        tonic.target = 0.1f;
        tonic.tau = 1.0f;         // Slower dynamics
        tonic.baseline = 0.05f;
        tonic.modulationFactor = 1.0f;
        
        // Initialize receptor dynamics
        receptor.reuptakeRate = 0.1f;
        receptor.diffusionRate = 0.05f;
        receptor.d1ReceptorDensity = 1.0f;
        receptor.d2ReceptorDensity = 1.0f;
        receptor.d1Activation = 0.0f;
        receptor.d2Activation = 0.0f;
        receptor.historySize = 100;
        receptor.history.reserve(receptor.historySize);
        
        // Initialize filtering
        filtering.temporalFilter = 0.9f;
        filtering.spatialFilter = 0.5f;
        filtering.adaptationRate = 0.01f;
        
        // Initialize RPE computation
        rpe.lastReward = 0.0f;
        rpe.predictedReward = 0.0f;
        rpe.predictionError = 0.0f;
        rpe.learningRate = 0.01f;
        rpe.eligibilityTrace = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine phasic and tonic components
    float phasicComponent = pImpl->phasic.level;
    float tonicComponent = pImpl->tonic.level * (1.0f - phasicComponent); // Tonic is reduced when phasic is high
    return phasicComponent + tonicComponent;
}

void Dopamine::setLevel(float level) {
    pImpl->tonic.level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine-modulated plasticity factor
    // D1 receptors enhance LTP, D2 receptors enhance LTD
    float d1Effect = pImpl->receptor.d1Activation * 1.5f;  // D1 enhances LTP
    float d2Effect = pImpl->receptor.d2Activation * 0.5f;  // D2 enhances LTD
    
    // Net plasticity factor: D1 promotes strengthening, D2 promotes weakening
    float plasticity = 0.5f + (d1Effect - d2Effect);
    
    // Clamp to reasonable range
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Update phasic component (fast dynamics)
    if (std::abs(pImpl->phasic.target - pImpl->phasic.level) > 0.001f) {
        // Exponential approach to target
        float delta = (pImpl->phasic.target - pImpl->phasic.level) * pImpl->phasic.tau * dtFloat;
        pImpl->phasic.level += delta;
    } else {
        // Decay towards baseline
        pImpl->phasic.level = std::max(pImpl->phasic.baseline, 
                                      pImpl->phasic.level - pImpl->phasic.tau * dtFloat * 0.5f);
    }
    
    // Update tonic component (slow dynamics)
    if (std::abs(pImpl->tonic.target - pImpl->tonic.level) > 0.001f) {
        float delta = (pImpl->tonic.target - pImpl->tonic.level) * pImpl->tonic.tau * dtFloat;
        pImpl->tonic.level += delta;
    }
    
    // Update receptor activation based on dopamine level
    updateReceptorActivation();
    
    // Update spatial/temporal filtering
    updateFiltering(dtFloat);
    
    // Update eligibility trace
    pImpl->rpe.eligibilityTrace *= std::pow(pImpl->rpe.eligibilityDecay, dtFloat);
}

void Dopamine::signalReward(float reward) {
    pImpl->rpe.lastReward = reward;
    
    // Compute prediction error
    float error = reward - pImpl->rpe.predictedReward;
    pImpl->rpe.predictionError = error;
    
    // Update prediction for next time step
    pImpl->rpe.predictedReward += pImpl->rpe.learningRate * error;
    
    // Generate phasic response based on prediction error
    // Phasic dopamine encodes reward prediction errors
    float phasicResponse = error * pImpl->phasic.maxResponse;
    
    // Apply temporal filtering and adaptation
    phasicResponse *= pImpl->filtering.temporalFilter;
    
    // Update phasic target with processed response
    pImpl->phasic.target = std::max(pImpl->phasic.baseline, phasicResponse);
    
    // Update tonic target (sustained arousal from reward)
    float tonicModulation = std::min(1.0f, std::abs(error) * 2.0f) * pImpl->tonic.modulationFactor;
    pImpl->tonic.target = pImpl->tonic.baseline + tonicModulation * 0.1f;
    
    // Update eligibility trace for credit assignment
    pImpl->rpe.eligibilityTrace = std::min(1.0f, pImpl->rpe.eligibilityTrace + 0.1f);
    
    NLM_LOG_DEBUG("Dopamine: Reward=" + std::to_string(reward) + 
                 ", PredictionError=" + std::to_string(error) + 
                 ", PhasicLevel=" + std::to_string(pImpl->phasic.level) +
                 ", TonicLevel=" + std::to_string(pImpl->tonic.level));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rpe.predictionError = error;
    
    // Generate phasic response for prediction error
    float phasicResponse = error * pImpl->phasic.maxResponse * 1.5f; // Stronger for prediction errors
    
    // Apply temporal filtering
    phasicResponse *= pImpl->filtering.temporalFilter;
    
    // Update phasic target
    pImpl->phasic.target = std::max(pImpl->phasic.baseline, phasicResponse);
    
    // Update eligibility trace
    pImpl->rpe.eligibilityTrace = std::min(1.0f, pImpl->rpe.eligibilityTrace + 0.05f * std::abs(error));
    
    NLM_LOG_DEBUG("Dopamine: PredictionError=" + std::to_string(error) + 
                 ", PhasicLevel=" + std::to_string(pImpl->phasic.level));
}

void Dopamine::updateReceptorActivation() {
    float totalDA = getLevel();
    
    // D1 receptor activation: increases with dopamine level
    pImpl->receptor.d1Activation = std::min(1.0f, totalDA * pImpl->receptor.d1ReceptorDensity);
    
    // D2 receptor activation: complex relationship (inverted U-shape)
    float d2Response = std::exp(-std::pow(totalDA * pImpl->receptor.d2ReceptorDensity - 1.0f, 2.0f) * 5.0f);
    pImpl->receptor.d2Activation = d2Response;
    
    // Update history
    pImpl->receptor.history.push_back(totalDA);
    if (pImpl->receptor.history.size() > pImpl->receptor.historySize) {
        pImpl->receptor.history.erase(pImpl->receptor.history.begin());
    }
}

void Dopamine::updateFiltering(float dt) {
    // Apply temporal filtering
    if (pImpl->receptor.history.size() >= 2) {
        float recent = pImpl->receptor.history.back();
        float older = pImpl->receptor.history.front();
        float filtered = recent * (1.0f - pImpl->filtering.temporalFilter) + 
                        older * pImpl->filtering.temporalFilter;
        
        // Update phasic level with filtered value
        pImpl->phasic.level = filtered * pImpl->filtering.spatialFilter;
    }
    
    // Adaptation to sustained stimuli
    pImpl->phasic.target *= (1.0f - pImpl->filtering.adaptationRate * dt);
}

} // namespace nlm
