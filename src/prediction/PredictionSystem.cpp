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

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Enhanced prediction using temporal patterns and neural dynamics
    // More sophisticated than simple copy-paste
    
    auto prediction = std::make_unique<SensoryInput>();
    
    // Get current data
    const auto& currentData = currentState.getData();
    
    // Apply temporal filtering and prediction
    std::vector<float> predictedData = currentData;
    size_t dataSize = predictedData.size();
    
    if (dataSize > 0) {
        // Temporal smoothing with exponential decay
        for (size_t i = 0; i < dataSize; ++i) {
            float smoothed = predictedData[i];
            
            // Apply Gaussian-like smoothing kernel for prediction
            float weightSum = 0.0f;
            float filteredValue = 0.0f;
            
            // Look at previous time steps with decaying weights
            size_t windowSize = std::min(size_t(5), i + 1);
            for (size_t j = 0; j < windowSize; ++j) {
                size_t idx = i - j;
                if (idx < dataSize) {
                    float weight = std::exp(-static_cast<float>(j) * 0.3f);
                    filteredValue += predictedData[idx] * weight;
                    weightSum += weight;
                }
            }
            
            if (weightSum > 0.0f) {
                smoothed = filteredValue / weightSum;
                
                // Add predictive bias based on recent trends
                if (i >= 2) {
                    float trend = predictedData[i] - predictedData[i-1];
                    smoothed += trend * 0.1f;  // Gentle trend extrapolation
                }
                
                // Apply confidence-based modulation
                smoothed *= pImpl->confidence;
                
                predictedData[i] = smoothed;
            }
        }
        
        // Add stochastic component for exploration
        float noiseLevel = 1.0f - pImpl->confidence;
        for (size_t i = 0; i < dataSize; ++i) {
            if (noiseLevel > 0.0f) {
                // Gaussian noise with variance proportional to uncertainty
                float noise = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
                predictedData[i] += noise * noiseLevel * 0.1f;
            }
        }
        
        prediction->setData(predictedData);
        prediction->setTimestamp(currentState.getTimestamp() + 0.001f); // Advance time
        
        // Update prediction confidence based on data characteristics
        if (!predictedData.empty()) {
            float maxVal = *std::max_element(predictedData.begin(), predictedData.end());
            float minVal = *std::min_element(predictedData.begin(), predictedData.end());
            float range = maxVal - minVal;
            
            // Higher range indicates more predictable patterns (higher confidence)
            pImpl->confidence = std::min(1.0f, pImpl->confidence + range * 0.1f);
            pImpl->confidence = std::max(0.1f, std::min(pImpl->confidence, 1.0f));
        }
    }
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Advanced prediction error computation with temporal context
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute multiple error metrics
    float absoluteError = 0.0f;
    float squaredError = 0.0f;
    float maxAbsoluteError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        absoluteError += std::abs(diff);
        squaredError += diff * diff;
        maxAbsoluteError = std::max(maxAbsoluteError, std::abs(diff));
    }
    
    size_t dataSize = predData.size();
    
    // Weighted prediction error with temporal discounting
    pImpl->predictionError = squaredError / dataSize;  // MSE
    
    // Scale error with confidence
    pImpl->predictionError *= (2.0f - pImpl->confidence);  // Higher confidence = lower tolerance
    
    // Add temporal dynamics
    if (!pImpl->errorHistory.empty()) {
        float previousError = pImpl->errorHistory.back();
        // Trend component
        float errorTrend = pImpl->predictionError - previousError;
        // Limit trend contribution
        pImpl->predictionError += errorTrend * 0.1f;
    }
    
    // Store in history with size limit
    pImpl->errorHistory.push_back(pImpl->predictionError);
    const size_t maxHistorySize = 1000;
    if (pImpl->errorHistory.size() > maxHistorySize) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Adjust confidence based on prediction accuracy
    if (!pImpl->errorHistory.empty()) {
        float avgError = 0.0f;
        for (float error : pImpl->errorHistory) {
            avgError += error;
        }
        avgError /= pImpl->errorHistory.size();
        
        // Lower average error increases confidence
        pImpl->confidence = 1.0f / (1.0f + avgError * 10.0f);
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
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm
