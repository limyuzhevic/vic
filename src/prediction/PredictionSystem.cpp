#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionSystem initialized (Phase 6 Enhanced)");
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // ===== PHASE 6 ENHANCED PREDICTION =====
    
    // Use neural patterns for prediction
    // In real implementation, this would use brain neural networks
    std::vector<float> predictedData = currentState.getData();
    
    // Apply neuromodulation to prediction
    float neuromodulation = 1.0f + getPredictionError() * 0.5f;
    
    // Add prediction error to future state
    if (getPredictionError() > 0.1f) {
        // High prediction error -> anticipate change
        for (size_t i = 0; i < predictedData.size(); ++i) {
            predictedData[i] += (getPredictionError() * neuromodulation * 0.1f);
            predictedData[i] = std::clamp(predictedData[i], 0.0f, 1.0f);
        }
    }
    
    // Create enhanced prediction input
    class EnhancedSensoryInput : public SensoryInput {
        std::vector<float> data;
        float confidence;
    public:
        EnhancedSensoryInput(const std::vector<float>& d, float c) : data(d), confidence(c) {}
        const std::vector<float>& getData() const override { return data; }
        float getConfidence() const { return confidence; }
        std::unique_ptr<SensoryInput> clone() const override {
            return std::make_unique<EnhancedSensoryInput>(data, confidence);
        }
    };
    
    float confidence = 1.0f / (1.0f + std::abs(getPredictionError()) * 10.0f);
    
    return std::make_unique<EnhancedSensoryInput>(predictedData, confidence);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // ===== PHASE 6 ENHANCED PREDICTION ERROR COMPUTATION =====
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Multi-factor prediction error
        float temporalError = 0.0f;
        float magnitudeError = 0.0f;
        float patternError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            temporalError += diff * diff;
            magnitudeError += std::abs(diff);
            
            // Pattern consistency error
            if (i > 0 && i < predData.size() - 1) {
                float predChange = predData[i] - predData[i-1];
                float actualChange = actualData[i] - actualData[i-1];
                patternError += std::abs(predChange - actualChange) * 0.5f;
            }
        }
        
        // Combine error factors with weights
        float temporalWeight = 0.5f;
        float magnitudeWeight = 0.3f;
        float patternWeight = 0.2f;
        
        float totalError = (temporalError / predData.size()) * temporalWeight +
                          (magnitudeError / predData.size()) * magnitudeWeight +
                          (patternError / predData.size()) * patternWeight;
        
        pImpl->predictionError = totalError;
        
        // Enhanced error history with confidence weighting
        float errorConfidence = 1.0f / (1.0f + totalError * 10.0f);
        pImpl->predictionError *= errorConfidence;  // Weight by confidence
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit history size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction accuracy
        if (!pImpl->errorHistory.empty()) {
            float avgError = 0.0f;
            for (float err : pImpl->errorHistory) {
                avgError += err;
            }
            avgError /= pImpl->errorHistory.size();
            
            pImpl->confidence = std::clamp(1.0f - avgError * 2.0f, 0.1f, 1.0f);
        }
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

void PredictionSystem::train(const SensoryInput& observation) {
    // ===== PHASE 6 ENHANCED PREDICTION TRAINING =====
    
    // Enhanced prediction training using neural patterns
    // In real implementation, this would train neural networks in the brain
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Update prediction model with new observation
    // This is a placeholder for neural network training
    float observationEntropy = 0.0f;
    for (float val : data) {
        observationEntropy -= val * std::log(std::max(val, 0.001f));
    }
    
    // Adjust confidence based on observation entropy
    pImpl->confidence *= (1.0f - observationEntropy / data.size());
    pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
    
    // Store for future prediction
    NLM_LOG_DEBUG("PredictionSystem trained with observation (entropy: " + 
                 std::to_string(observationEntropy) + ", confidence: " + 
                 std::to_string(pImpl->confidence) + ")");
    
    // Update prediction error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
}

// Add helper methods for prediction system
float PredictionSystem::getPredictionStability() const {
    if (pImpl->errorHistory.size() < 10) return 1.0f;
    
    float sum = 0.0f, sumSq = 0.0f;
    for (size_t i = std::max<size_t>(0, pImpl->errorHistory.size() - 100); i < pImpl->errorHistory.size(); ++i) {
        sum += pImpl->errorHistory[i];
        sumSq += pImpl->errorHistory[i] * pImpl->errorHistory[i];
    }
    
    float mean = sum / pImpl->errorHistory.size();
    float stddev = std::sqrt(sumSq / pImpl->errorHistory.size() - mean * mean);
    
    // Lower standard deviation = higher stability
    return std::max(0.0f, 1.0f - stddev / 0.5f);
}

float PredictionSystem::getTemporalPredictionHorizon() const {
    // Estimate how far ahead predictions are reliable
    float errorGrowth = 0.0f;
    if (pImpl->errorHistory.size() >= 10) {
        for (size_t i = 1; i < pImpl->errorHistory.size(); ++i) {
            errorGrowth += pImpl->errorHistory[i] - pImpl->errorHistory[i-1];
        }
        errorGrowth /= pImpl->errorHistory.size() - 1;
    }
    
    // High growth rate = short prediction horizon
    return std::max(1.0f, 100.0f * (1.0f - errorGrowth));
}
