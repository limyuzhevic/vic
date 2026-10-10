#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    float currentPredictionBias;
    float predictionConfidenceDecay;
    
    // Memory-based prediction
    std::vector<float> recentPattern;
    std::vector<float> predictedPattern;
    
    Impl() : predictionError(0.0f), confidence(0.5f), currentPredictionBias(0.0f), 
             predictionConfidenceDecay(0.99f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

// ENHANCEMENT: Implement neural prediction using working memory patterns
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // ENHANCED: Use working memory patterns and neural activity for prediction
    
    const auto& currentData = currentState.getData();
    
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    // Calculate predicted state based on patterns in working memory
    // This simulates the brain's ability to predict sensory consequences
    
    // Initialize predicted state with current state as baseline
    std::vector<float> predictedData(currentData.size(), 0.0f);
    
    // If we have working memory patterns, use them for prediction
    // This represents how the brain uses stored patterns to predict future states
    if (!pImpl->recentPattern.empty()) {
        // Apply pattern-based prediction with some noise for variability
        float patternStrength = std::min(1.0f, static_cast<float>(pImpl->recentPattern.size()) / 100.0f);
        
        for (size_t i = 0; i < currentData.size(); ++i) {
            // Base prediction on current state with pattern modulation
            float basePrediction = currentData[i];
            
            // Add pattern influence (simulate memory-driven prediction)
            if (i < pImpl->recentPattern.size()) {
                basePrediction += pImpl->recentPattern[i] * patternStrength * 0.1f;
            }
            
            // Add prediction bias (dopamine modulation)
            basePrediction += pImpl->currentPredictionBias * 0.05f;
            
            // Add some stochasticity for exploration
            float stochasticity = 0.1f * (1.0f - pImpl->confidence);
            float noise = (pImpl->confidence > 0.5f) ? 
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * stochasticity * 2.0f :
                0.0f;
            
            predictedData[i] = basePrediction + noise;
        }
    } else {
        // Fallback: simple prediction based on current state
        for (size_t i = 0; i < currentData.size(); ++i) {
            predictedData[i] = currentData[i];
        }
    }
    
    // Clamp to valid sensory range
    for (float& value : predictedData) {
        value = std::max(0.0f, std::min(1.0f, value));
    }
    
    // Create new SensoryInput with predicted data
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    // Update prediction confidence based on pattern strength
    pImpl->confidence = patternStrength * 0.7f + 0.3f;
    
    NLM_LOG_DEBUG("PredictionSystem: Predicted next state from pattern of size " + 
                 std::to_string(pImpl->recentPattern.size()) + 
                 " with confidence " + std::to_string(pImpl->confidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // ENHANCED: Compute prediction error and update neural confidence
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error as mean squared error
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float squaredError = diff * diff;
            sumError += squaredError;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = sumError / predData.size();
        
        // Store error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep error history manageable
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update prediction bias based on error
        // Positive error (underprediction) increases bias
        // Negative error (overprediction) decreases bias
        pImpl->currentPredictionBias = maxError * 0.1f;  // Scale bias by max error
        
        // Decay confidence over time based on recent errors
        float recentErrorAvg = 0.0f;
        if (!pImpl->errorHistory.empty()) {
            size_t recentCount = std::min<size_t>(100, pImpl->errorHistory.size());
            float sumRecent = std::accumulate(pImpl->errorHistory.end() - recentCount, 
                                             pImpl->errorHistory.end(), 0.0f);
            recentErrorAvg = sumRecent / recentCount;
        }
        
        // Confidence decays based on prediction error
        float errorFactor = std::min(1.0f, recentErrorAvg * 10.0f);
        pImpl->confidence *= (1.0f - errorFactor * 0.05f);
        pImpl->confidence = std::max(0.1f, std::min(0.99f, pImpl->confidence));
        
        // Update recent pattern from actual observation
        pImpl->recentPattern = actualData;
        
        NLM_LOG_DEBUG("PredictionSystem: Error=" + std::to_string(pImpl->predictionError) + 
                     " Confidence=" + std::to_string(pImpl->confidence) +
                     " Bias=" + std::to_string(pImpl->currentPredictionBias));
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

// ENHANCEMENT: Implement experience-based prediction training
void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
    // ENHANCED: Learn patterns from sensory observations for better prediction
    
    const auto& obsData = observation.getData();
    if (obsData.empty()) return;
    
    // Store observation as recent pattern for prediction
    pImpl->recentPattern = obsData;
    
    // Calculate predicted pattern (current state with minor variations)
    pImpl->predictedPattern.clear();
    pImpl->predictedPattern.reserve(obsData.size());
    
    for (float value : obsData) {
        // Add small variation to simulate temporal dynamics
        float variation = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.02f;
        pImpl->predictedPattern.push_back(value + variation);
    }
    
    // Clamp predicted values
    for (float& value : pImpl->predictedPattern) {
        value = std::max(0.0f, std::min(1.0f, value));
    }
    
    // Improve confidence through training
    pImpl->confidence = std::min(0.9f, pImpl->confidence + 0.05f);
    
    NLM_LOG_DEBUG("PredictionSystem: Trained with observation, confidence=" + 
                 std::to_string(pImpl->confidence));
}

// ENHANCEMENT: Set predicted pattern
void PredictionSystem::setPredictedPattern(const std::vector<float>& pattern) {
    pImpl->predictedPattern = pattern;
}

// ENHANCEMENT: Get predicted pattern
const std::vector<float>& PredictionSystem::getPredictedPattern() const {
    return pImpl->predictedPattern;
}

} // namespace nlm
