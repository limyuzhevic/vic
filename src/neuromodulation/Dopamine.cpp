#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Biological parameters inspired by VTA dopamine neurons
    float baselineLevel;     // Baseline tonic firing (~2 nM in biological systems)
    float peakLevel;         // Peak phasic firing (~50 nM in biological systems)
    float currentLevel;      // Current dopamine concentration
    float prediction;        // Current reward prediction
    float rewardBuffer;      // Accumulated reward signal
    float eligibilityTrace;  // Temporal difference learning trace
    float learningRate;      // Adaptive learning rate
    float decayRate;         // Reuptake and clearance rate
    float synthesisRate;     // Activity-dependent synthesis rate
    float saturation;        // Firing rate saturation
    
    // History tracking
    std::vector<float> levelHistory;
    std::vector<float> predictionHistory;
    std::vector<float> rewardHistory;
    
    // Random number generator for biological variability
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : 
        baselineLevel(2.0f),     // ~2 nM baseline
        peakLevel(50.0f),         // ~50 nM peak (burst firing)
        currentLevel(baselineLevel),
        prediction(0.0f),
        rewardBuffer(0.0f),
        eligibilityTrace(0.0f),
        learningRate(0.1f),
        decayRate(0.5f),         // 50% decay per timestep
        synthesisRate(2.0f),
        saturation(100.0f),
        noiseDist(0.0f, 0.1f) {
        rng.seed(std::random_device{}());
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Dopamine system initialized with VTA-inspired parameters");
    NLM_LOG_INFO("Baseline: " + std::to_string(pImpl->baselineLevel) + " nM, Peak: " + std::to_string(pImpl->peakLevel) + " nM");
}

Dopamine::~Dopamine() {
    NLM_LOG_INFO("Dopamine system destroyed");
}

const char* Dopamine::getName() const {
    return "Dopamine (DA)";
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->currentLevel = std::clamp(level, 0.0f, pImpl->saturation);
    pImpl->levelHistory.push_back(pImpl->currentLevel);
    if (pImpl->levelHistory.size() > 100) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
}

float Dopamine::getPlasticityFactor() const {
    // Compute plasticity modulation based on prediction error and magnitude
    float errorMagnitude = std::abs(pImpl->prediction - pImpl->rewardBuffer);
    float errorSignal = (pImpl->rewardBuffer - pImpl->prediction) * pImpl->learningRate;
    
    // Nonlinear transformation of error into plasticity factor
    float plasticityFactor = 0.1f + 2.0f * std::tanh(std::abs(errorSignal)) * errorMagnitude / pImpl->peakLevel;
    
    // Apply biological constraints
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Phasic component: burst firing for prediction errors
    float predictionError = pImpl->rewardBuffer - pImpl->prediction;
    float phasicComponent = predictionError * pImpl->learningRate * pImpl->synthesisRate * timeStep;
    
    // Tonic component: baseline firing with adaptive synthesis
    float tonicSynthesis = pImpl->synthesisRate * (1.0f + pImpl->prediction) * timeStep;
    
    // Biological dynamics: combine components with saturation
    float totalChange = (phasicComponent + tonicSynthesis) * timeStep;
    float biologicalNoise = pImpl->noiseDist(pImpl->rng);
    
    // Apply dynamics with saturation and reuptake
    pImpl->currentLevel += totalChange + biologicalNoise;
    pImpl->currentLevel = std::max(pImpl->baselineLevel, std::min(pImpl->currentLevel, pImpl->saturation));
    
    // Reuptake and clearance (exponential decay)
    pImpl->currentLevel *= std::pow(1.0f - pImpl->decayRate * timeStep, 1.0f);
    
    // Update eligibility trace for temporal difference learning
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - pImpl->decayRate * timeStep);
    if (predictionError != 0.0f) {
        pImpl->eligibilityTrace += predictionError * pImpl->learningRate * timeStep;
    }
    
    // Update history
    pImpl->levelHistory.push_back(pImpl->currentLevel);
    pImpl->predictionHistory.push_back(pImpl->prediction);
    pImpl->rewardHistory.push_back(pImpl->rewardBuffer);
    if (pImpl->levelHistory.size() > 100) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Log occasionally
    if (static_cast<int>(pImpl->currentLevel * 10) % 100 == 0) {
        NLM_LOG_DEBUG("Dopamine level: " + std::to_string(pImpl->currentLevel) + 
                     " nM (prediction: " + std::to_string(pImpl->prediction) + ", error: " + 
                     std::to_string(predictionError) + ")");
    }
}

void Dopamine::signalReward(float reward) {
    // Phasic burst response: sudden dopamine release for unexpected rewards
    float rewardPrediction = reward;
    
    // Calculate prediction error
    float predictionError = reward - pImpl->prediction;
    
    // Apply burst firing model (biological burst response)
    float burstStrength = std::min(1.0f, std::abs(predictionError) * 2.0f);
    float phasicBurst = burstStrength * pImpl->peakLevel * (1.0f + predictionError);
    
    // Apply to current level with saturation
    setLevel(getLevel() + phasicBurst);
    
    // Update reward buffer and eligibility trace
    pImpl->rewardBuffer = reward;
    pImpl->eligibilityTrace = std::max(pImpl->eligibilityTrace, std::abs(predictionError) * pImpl->learningRate);
    
    // Log reward signaling
    NLM_LOG_INFO("Reward signaled: " + std::to_string(reward) + 
                 " nM, prediction error: " + std::to_string(predictionError) + 
                 ", burst strength: " + std::to_string(burstStrength));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling (temporal difference learning)
    pImpl->prediction = 0.0f;  // Reset prediction
    
    // Error signal drives learning
    float errorSignal = error * pImpl->learningRate;
    
    // Apply error with biological dynamics
    setLevel(getLevel() + errorSignal * pImpl->peakLevel);
    
    // Update eligibility trace with error
    pImpl->eligibilityTrace = std::max(pImpl->eligibilityTrace, std::abs(error) * pImpl->learningRate);
    
    // Log prediction error signaling
    NLM_LOG_INFO("Reward prediction error signaled: " + std::to_string(error) + 
                 ", eligibility trace: " + std::to_string(pImpl->eligibilityTrace));
}

// Additional utility methods for debugging and analysis
float Dopamine::getPrediction() const {
    return pImpl->prediction;
}

void Dopamine::setPrediction(float prediction) {
    pImpl->prediction = prediction;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

const std::vector<float>& Dopamine::getLevelHistory() const {
    return pImpl->levelHistory;
}

const std::vector<float>& Dopamine::getPredictionHistory() const {
    return pImpl->predictionHistory;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.01f, 0.5f);
}

} // namespace nlm
