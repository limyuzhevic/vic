#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    // Forward model state
    std::vector<float> forwardPrediction;
    std::vector<float> sensoryInput;
    
    // Temporal dynamics
    float predictionConfidence;
    float predictionPrecision;
    float learningRate;
    
    // Temporal difference learning components
    float eligibilityTrace;
    float temporalDifference;
    float discountFactor;
    
    // Integration with reward prediction error
    float integratedRPE;
    float rewardPredictionError;
    
    // Plasticity modulation
    float synapticModulation;
    float plasticityFactor;
    
    // Time constants
    float predictionUpdateRate;
    float traceDecayRate;
    float integrationTimeConstant;
    
    // Novelty integration
    float noveltySignal;
    float explorationBonus;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             predictionConfidence(0.5f), predictionPrecision(0.5f), learningRate(0.01f),
             eligibilityTrace(0.0f), temporalDifference(0.0f), discountFactor(0.9f),
             integratedRPE(0.0f), rewardPredictionError(0.0f), synapticModulation(0.0f),
             plasticityFactor(0.5f), predictionUpdateRate(0.1f), traceDecayRate(0.05f),
             integrationTimeConstant(1.0f), noveltySignal(0.0f), explorationBonus(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

// Get forward model prediction error
float PredictionError::getForwardError() const {
    return getMagnitude();
}

// Get temporal difference error
float PredictionError::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

// Get prediction confidence
float PredictionError::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

// Get eligibility trace
float PredictionError::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

// Get integrated reward prediction error
float PredictionError::getIntegratedRPE() const {
    return pImpl->integratedRPE;
}

// Get synaptic modulation factor
float PredictionError::getSynapticModulation() const {
    return pImpl->synapticModulation;
}

// Get plasticity factor
float PredictionError::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

// Get novelty signal
float PredictionError::getNoveltySignal() const {
    return pImpl->noveltySignal;
}

// Get exploration bonus
float PredictionError::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

// Configuration methods
void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void PredictionError::setPredictionConfidence(float confidence) {
    pImpl->predictionConfidence = std::clamp(confidence, 0.0f, 1.0f);
}

void PredictionError::setPredictionPrecision(float precision) {
    pImpl->predictionPrecision = std::clamp(precision, 0.0f, 1.0f);
}

void PredictionError::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.0f, 1.0f);
}

void PredictionError::setPredictionUpdateRate(float rate) {
    pImpl->predictionUpdateRate = rate;
}

void PredictionError::setTraceDecayRate(float rate) {
    pImpl->traceDecayRate = rate;
}

void PredictionError::setIntegrationTimeConstant(float timeConstant) {
    pImpl->integrationTimeConstant = timeConstant;
}

void PredictionError::setNoveltySignal(float signal) {
    pImpl->noveltySignal = signal;
}

void PredictionError::setExplorationBonus(float bonus) {
    pImpl->explorationBonus = bonus;
}

// Forward model prediction
void PredictionError::computeForwardPrediction(const std::vector<float>& sensoryInput) {
    pImpl->sensoryInput = sensoryInput;
    pImpl->forwardPrediction = sensoryInput;  // Simple identity model for now
}

// Update prediction with new sensory input
void PredictionError::updateForwardModel(const std::vector<float>& newSensoryInput) {
    float floatDt = static_cast<float>(1.0);  // Default timestep
    
    // Update prediction based on prediction update rate
    for (size_t i = 0; i < newSensoryInput.size() && i < pImpl->forwardPrediction.size(); ++i) {
        float predictionError = newSensoryInput[i] - pImpl->forwardPrediction[i];
        pImpl->forwardPrediction[i] += predictionError * pImpl->predictionUpdateRate * floatDt;
        
        // Update prediction confidence based on error magnitude
        pImpl->predictionConfidence *= (1.0f - std::abs(predictionError) * 0.1f);
    }
    
    // Store current sensory input
    pImpl->sensoryInput = newSensoryInput;
}

// Compute prediction error with temporal difference learning
void PredictionError::computeTemporalDifference(float currentValue, float nextValue) {
    pImpl->temporalDifference = nextValue - pImpl->discountFactor * currentValue;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = pImpl->traceDecayRate * pImpl->eligibilityTrace + 
                              pImpl->temporalDifference * pImpl->predictionPrecision;
    
    // Compute forward model prediction error
    float forwardError = 0.0f;
    if (!pImpl->sensoryInput.empty() && !pImpl->forwardPrediction.empty()) {
        size_t minSize = std::min(pImpl->sensoryInput.size(), pImpl->forwardPrediction.size());
        for (size_t i = 0; i < minSize; ++i) {
            forwardError += std::abs(pImpl->sensoryInput[i] - pImpl->forwardPrediction[i]);
        }
        forwardError /= static_cast<float>(minSize);
    }
    
    // Store the main prediction error
    pImpl->error = forwardError;
    
    // Integrate with reward prediction error from Dopamine
    pImpl->integratedRPE = pImpl->rewardPredictionError * 0.7f + pImpl->temporalDifference * 0.3f;
    
    // Apply novelty signal for curiosity-driven learning
    pImpl->noveltySignal = std::max(0.0f, pImpl->noveltySignal - pImpl->traceDecayRate * 1.0f);
    pImpl->explorationBonus = pImpl->noveltySignal * 0.5f;
    
    // Update plasticity factor based on prediction error
    float errorMagnitude = std::abs(forwardError);
    pImpl->plasticityFactor = 0.3f + 0.7f * std::min(1.0f, errorMagnitude * 10.0f);
    
    // Modulate synaptic strength based on prediction error
    pImpl->synapticModulation = std::max(0.0f, pImpl->plasticityFactor * errorMagnitude);
    
    // Update history
    pImpl->history.push_back(pImpl->error);
    
    // Maintain history size limit
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

// Update prediction error with reward prediction error input
void PredictionError::integrateRewardPredictionError(float rpe) {
    pImpl->rewardPredictionError = rpe;
    
    // Integrate RPE with temporal difference learning
    float integrationRate = static_cast<float>(1.0) / pImpl->integrationTimeConstant;
    pImpl->integratedRPE += (rpe - pImpl->integratedRPE) * integrationRate;
}

// Clear all state
void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->forwardPrediction.clear();
    pImpl->sensoryInput.clear();
    pImpl->predictionConfidence = 0.5f;
    pImpl->predictionPrecision = 0.5f;
    pImpl->learningRate = 0.01f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->integratedRPE = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->synapticModulation = 0.0f;
    pImpl->plasticityFactor = 0.5f;
    pImpl->noveltySignal = 0.0f;
    pImpl->explorationBonus = 0.0f;
    pImpl->history.clear();
}
