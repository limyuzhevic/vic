#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    size_t predictionCount;
    std::vector<SensoryInput> predictionBuffer;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionCount(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    try {
        // TODO PHASE 2: Implement real prediction using NLM's neural substrate
        // For now, implement a simple predictive coding baseline
        pImpl->predictionBuffer.push_back(currentState);
        if (pImpl->predictionBuffer.size() > 100) {
            pImpl->predictionBuffer.erase(pImpl->predictionBuffer.begin());
        }
        
        // Simple predictive baseline: return a processed version of current state
        auto prediction = currentState.clone();
        
        // Add small prediction noise based on confidence
        if (pImpl->confidence > 0.5f) {
            auto& data = prediction->getData();
            for (auto& val : data) {
                val += (std::rand() % 100 - 50) / 50.0f * (1.0f - pImpl->confidence);
            }
        }
        
        pImpl->predictionCount++;
        return prediction;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("PredictionSystem::predictNextState failed: ") + e.what());
        return currentState.clone();
    }
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    try {
        // TODO PHASE 2: Implement real prediction error computation
        // Calculate prediction error using multiple metrics
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() == actualData.size() && !predData.empty()) {
            // Calculate mean squared error
            float sumError = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                sumError += diff * diff;
            }
            pImpl->predictionError = sumError / predData.size();
            
            // Update confidence based on error (lower error = higher confidence)
            pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
            
            // Store error in history
            pImpl->errorHistory.push_back(pImpl->predictionError);
            if (pImpl->errorHistory.size() > 10000) {
                pImpl->errorHistory.erase(pImpl->errorHistory.begin());
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("PredictionSystem::updatePredictions failed: ") + e.what());
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

size_t PredictionSystem::getPredictionCount() const {
    return pImpl->predictionCount;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->predictionBuffer.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    try {
        // TODO PHASE 2: Implement real prediction model training
        // For now, implement basic statistics collection
        // In a real implementation, this would update neural network weights
        // or other prediction model parameters
        
        // Collect observation statistics for training
        const auto& data = observation.getData();
        if (!data.empty()) {
            // Calculate basic statistics
            float sum = 0.0f, min = data[0], max = data[0];
            for (float val : data) {
                sum += val;
                min = std::min(min, val);
                max = std::max(max, val);
            }
            float mean = sum / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            
            // Update confidence based on data variability
            // Higher variance typically means more challenging predictions
            pImpl->confidence = std::max(0.1f, 1.0f - std::sqrt(variance) * 0.5f);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("PredictionSystem::train failed: ") + e.what());
    }
}

void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    clearHistory();
    pImpl->predictionCount = 0;
}

} // namespace nlm
