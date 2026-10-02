#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    // Current prediction error
    float error;
    
    // Previous and current values for temporal difference computation
    float previousValue;
    float currentValue;
    
    // Predicted and actual observation values
    float predictedValue;
    float actualValue;
    
    // History of prediction errors
    std::vector<float> history;
    
    // Error statistics
    float minError;
    float maxError;
    float sumError;
    size_t countError;
    
    // Context-specific prediction errors
    std::unordered_map<uint64_t, float> contextErrors;  // hash -> error
    
    // Temporal difference learning parameters
    float learningRate;           // α - learning rate (0.01 - 0.1 typical)
    float discountFactor;         // γ - discount factor (0.9 - 0.99 typical)
    float errorThreshold;         // Threshold for significant prediction errors
    float confidence;             // Confidence in predictions
    
    // Integration parameters
    float eligibilityTrace;       // For credit assignment over time
    float timeConstant;           // Time constant for eligibility decay
    
    Impl() : error(0.0f), previousValue(0.0f), currentValue(0.0f),
             predictedValue(0.0f), actualValue(0.0f), learningRate(0.01f),
             discountFactor(0.95f), errorThreshold(0.1f), confidence(1.0f),
             eligibilityTrace(0.0f), timeConstant(100.0f),
             minError(std::numeric_limits<float>::max()),
             maxError(std::numeric_limits<float>::lowest()),
             sumError(0.0f), countError(0) {}
};

PredictionError::PredictionError() : pImpl(new Impl), recentError_(0.0f) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    // Initialize prediction error system with brain reference
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeTemporalDifferenceError(float reward, float currentValue, float nextValue, float discountFactor) {
    // Temporal difference (TD) learning error: δ = r + γ * V(s') - V(s)
    // This is the fundamental learning signal in reinforcement learning
    
    float tdError = reward + discountFactor * nextValue - currentValue;
    
    // Store previous value for integration
    pImpl->previousValue = pImpl->currentValue;
    pImpl->currentValue = currentValue;
    
    // Update error with eligibility trace
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - pImpl->timeConstant);
    pImpl->error = tdError * pImpl->eligibilityTrace;
    
    // Integrate TD error into value estimates (online learning)
    pImpl->currentValue += pImpl->learningRate * tdError;
    
    // Update statistics
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (pImpl->countError > 0) {
        pImpl->minError = std::min(pImpl->minError, pImpl->error);
        pImpl->maxError = std::max(pImpl->maxError, pImpl->error);
        pImpl->sumError += pImpl->error;
    } else {
        pImpl->minError = pImpl->error;
        pImpl->maxError = pImpl->error;
    }
    pImpl->countError++;
    
    pImpl->recentError_ = pImpl->error;
}

void PredictionError::computeObservationError(const Observation& predicted, const Observation& actual) {
    // Compute prediction error between predicted and actual observations
    // Based on similarity metrics between observation vectors
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.empty() || actualData.empty()) {
        pImpl->error = 0.0f;
        return;
    }
    
    // Compute various error metrics
    float mse = 0.0f;  // Mean squared error
    float mae = 0.0f;  // Mean absolute error
    
    size_t compareLen = std::min(predData.size(), actualData.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = predData[i] - actualData[i];
        mse += diff * diff;
        mae += std::abs(diff);
    }
    
    // Normalize errors
    mse /= compareLen;
    mae /= compareLen;
    
    // Convert MSE to error metric (Gaussian likelihood)
    float observationError = std::exp(-mse / 0.1f);  // Error based on MSE
    
    // Blend different error metrics
    pImpl->error = 0.7f * observationError + 0.3f * (1.0f - mae);
    
    // Update predicted vs actual values
    pImpl->predictedValue = (predData[0] + actualData[0]) / 2.0f;
    pImpl->actualValue = actualData[0];
    
    // Update statistics and history
    pImpl->history.push_back(pImpl->error);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->recentError_ = pImpl->error;
}

void PredictionError::updatePrediction(float newPrediction, float confidence) {
    // Update prediction based on new information
    // Uses weighted averaging based on confidence
    
    pImpl->confidence = std::clamp(confidence, 0.0f, 1.0f);
    
    // Update prediction with exponential moving average
    float alpha = pImpl->learningRate * pImpl->confidence;
    pImpl->predictedValue = (1.0f - alpha) * pImpl->predictedValue + alpha * newPrediction;
    
    // Update eligibility trace for credit assignment
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + alpha);
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->contextErrors.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

float PredictionError::getMeanError() const {
    if (pImpl->countError == 0) return 0.0f;
    return pImpl->sumError / pImpl->countError;
}

float PredictionError::getMaxError() const {
    return pImpl->maxError;
}

float PredictionError::getMinError() const {
    return pImpl->minError;
}

bool PredictionError::isSignificant() const {
    return std::abs(pImpl->error) > pImpl->errorThreshold;
}

float PredictionError::getContextError(const NeuralPattern& context) const {
    // Get prediction error for specific neural context
    // This allows different predictions for different brain states
    
    // Create hash from neural pattern for lookup
    uint64_t hash = createNeuralPatternHash(context);
    
    auto it = pImpl->contextErrors.find(hash);
    if (it != pImpl->contextErrors.end()) {
        return it->second;
    }
    
    return 0.0f; // Default error if no context-specific error
}

void PredictionError::updateContextError(const NeuralPattern& context, float error) {
    // Update prediction error for specific neural context
    uint64_t hash = createNeuralPatternHash(context);
    pImpl->contextErrors[hash] = error;
}

// Helper function to create hash from neural pattern
uint64_t PredictionError::createNeuralPatternHash(const NeuralPattern& pattern) const {
    // Simple hash function for neural patterns
    const auto& neurons = pattern.getNeurons();
    uint64_t hash = 0;
    
    for (size_t i = 0; i < neurons.size() && i < 4; ++i) {
        hash = (hash << 8) | (static_cast<uint64_t>(neurons[i].id) & 0xFF);
    }
    
    return hash;
}

} // namespace nlm
