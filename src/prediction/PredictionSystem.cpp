#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    // Enhanced prediction storage for reward-modulated plasticity
    std::vector<float> predictionErrorHistory;
    std::vector<float> predictedSensoryStates;
    std::vector<float> predictionConfidence;
    std::vector<float> predictionUncertainty;
    std::vector<float> predictionTiming;  // Timesteps when predictions were made
    std::vector<float> updateFrequency;  // How often predictions were updated
    std::vector<float> stabilityMetrics; // Prediction system stability over time
    
    // Learning and training
    std::vector<float> trainingErrorHistory;
    std::vector<size_t> successfulPredictions;  // Indices of successful predictions
    std::vector<float> predictionPatterns; // Pattern templates for future prediction
    
    // Integration with other systems
    std::vector<NeuronId> predictionErrorNeurons;  // Neurons encoding prediction error
    std::vector<float> attentionModulation; // How predictions influence attention
    std::vector<size_t> conceptFormationTriggers; // When predictions trigger concept learning
    
    // Current state
    float currentPredictionError;
    float currentConfidence;
    float currentUncertainty;
    float lastUpdateTime;
    size_t predictionCount;
    
    Impl() : currentPredictionError(0.0f), currentConfidence(0.5f), 
             currentUncertainty(0.3f), lastUpdateTime(0.0f), predictionCount(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase 2: Implement real prediction using NLM's neural substrate
    // For now, enhance with pattern-based prediction and learning
    pImpl->predictionCount++;
    
    // Store current state as predicted state for learning
    const auto& currentData = currentState.getData();
    pImpl->predictedSensoryStates.insert(pImpl->predictedSensoryStates.end(), 
                                         currentData.begin(), currentData.end());
    pImpl->predictionTiming.push_back(pImpl->predictionCount);
    
    // Create enhanced prediction - add temporal dynamics
    auto predicted = std::make_unique<SensoryInput>(currentData.size());
    
    // Apply temporal dynamics to prediction (moving average for next timestep)
    float temporalWeight = std::min(1.0f, pImpl->predictionCount * 0.01f); // Gradually increases
    float baseConfidence = 0.5f + temporalWeight * 0.5f; // Confidence improves with time
    
    for (size_t i = 0; i < currentData.size(); ++i) {
        // Add some temporal dynamics to prediction
        float variation = (pImpl->predictionCount > 0) ? 
            (currentData[i] * 0.1f) : 0.0f; // Predict slight change
        
        float predictedValue = currentData[i] + variation;
        predicted->setValue(i, predictedValue);
        
        // Store confidence and uncertainty
        pImpl->predictionConfidence.push_back(baseConfidence);
        pImpl->predictionUncertainty.push_back(1.0f - baseConfidence); // Higher confidence = lower uncertainty
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Phase 2: Implement real prediction error computation with enhanced storage
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float error = std::abs(diff);
            sumError += error * error;
            maxError = std::max(maxError, error);
        }
        
        pImpl->predictionError = sumError / predData.size();
        pImpl->currentPredictionError = pImpl->predictionError;
        pImpl->predictionErrorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error
        float confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        pImpl->currentConfidence = confidence;
        pImpl->predictionConfidence.push_back(confidence);
        
        // Calculate uncertainty (inverse of confidence)
        float uncertainty = 1.0f - confidence;
        pImpl->currentUncertainty = uncertainty;
        pImpl->predictionUncertainty.push_back(uncertainty);
        
        // Track successful predictions (small error)
        if (pImpl->predictionError < 0.1f) {
            pImpl->successfulPredictions.push_back(pImpl->predictionErrorHistory.size() - 1);
        }
        
        // Calculate stability metric (moving average of recent errors)
        if (pImpl->predictionErrorHistory.size() > 10) {
            float recentSum = 0.0f;
            size_t recentCount = std::min(pImpl->predictionErrorHistory.size(), size_t(10));
            for (size_t i = pImpl->predictionErrorHistory.size() - recentCount; 
                 i < pImpl->predictionErrorHistory.size(); ++i) {
                recentSum += pImpl->predictionErrorHistory[i];
            }
            float recentAverage = recentSum / recentCount;
            pImpl->stabilityMetrics.push_back(1.0f - recentAverage); // Higher stability when error is low
        }
        
        // Calculate update frequency
        if (pImpl->predictionTiming.size() > 1) {
            float timeSinceLastUpdate = pImpl->predictionTiming.back() - pImpl->predictionTiming[pImpl->predictionTiming.size() - 2];
            pImpl->updateFrequency.push_back(1.0f / std::max(1.0f, timeSinceLastUpdate));
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->currentPredictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->currentConfidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->predictionErrorHistory.clear();
    pImpl->predictedSensoryStates.clear();
    pImpl->predictionConfidence.clear();
    pImpl->predictionUncertainty.clear();
    pImpl->predictionTiming.clear();
    pImpl->updateFrequency.clear();
    pImpl->stabilityMetrics.clear();
    pImpl->trainingErrorHistory.clear();
    pImpl->successfulPredictions.clear();
    pImpl->predictionPatterns.clear();
    pImpl->predictionErrorNeurons.clear();
    pImpl->attentionModulation.clear();
    pImpl->conceptFormationTriggers.clear();
    
    pImpl->currentPredictionError = 0.0f;
    pImpl->currentConfidence = 0.5f;
    pImpl->currentUncertainty = 0.3f;
    pImpl->lastUpdateTime = 0.0f;
    pImpl->predictionCount = 0;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Phase 2: Train prediction model on sensory sequences
    const auto& data = observation.getData();
    
    if (data.empty()) return;
    
    // Store training observation for pattern learning
    std::vector<float> scaledData = data;
    
    // Normalize data for learning
    float maxVal = *std::max_element(data.begin(), data.end());
    float minVal = *std::min_element(data.begin(), data.end());
    float range = maxVal - minVal;
    
    if (range > 0.0f) {
        for (size_t i = 0; i < data.size(); ++i) {
            scaledData[i] = (data[i] - minVal) / range;
        }
    }
    
    pImpl->predictionPatterns.insert(pImpl->predictionPatterns.end(),
                                     scaledData.begin(), scaledData.end());
    
    // Calculate temporal patterns in the observation
    std::vector<float> temporalPatterns;
    if (data.size() > 1) {
        // Calculate first-order differences (temporal dynamics)
        for (size_t i = 1; i < data.size(); ++i) {
            temporalPatterns.push_back(data[i] - data[i-1]);
        }
    }
    
    // Store temporal patterns for sequence prediction
    if (!temporalPatterns.empty()) {
        pImpl->predictionPatterns.insert(pImpl->predictionPatterns.end(),
                                         temporalPatterns.begin(), temporalPatterns.end());
    }
    
    // Calculate training error for learning
    if (!scaledData.empty()) {
        float patternVariation = 0.0f;
        for (size_t i = 1; i < scaledData.size(); ++i) {
            patternVariation += std::abs(scaledData[i] - scaledData[i-1]);
        }
        patternVariation /= scaledData.size();
        
        pImpl->trainingErrorHistory.push_back(patternVariation);
        
        // Use prediction error to update learning parameters
        if (!pImpl->predictionErrorHistory.empty()) {
            float learningRate = 0.01f * pImpl->currentConfidence; // Higher confidence = faster learning
            
            // Calculate error between predicted and actual patterns
            float predictionError = pImpl->currentPredictionError;
            if (!pImpl->predictionErrorHistory.empty()) {
                predictionError = pImpl->predictionErrorHistory.back();
            }
            
            float errorUpdate = learningRate * predictionError;
            
            // Update internal state based on learning
            pImpl->currentConfidence -= errorUpdate;
            pImpl->currentConfidence = std::clamp(pImpl->currentConfidence, 0.1f, 1.0f);
            
            // Update uncertainty based on learning progress
            float uncertaintyUpdate = errorUpdate * 0.5f;
            pImpl->currentUncertainty += uncertaintyUpdate;
            pImpl->currentUncertainty = std::clamp(pImpl->currentUncertainty, 0.1f, 1.0f);
        }
        
        // Store learning metrics
        if (pImpl->trainingErrorHistory.size() % 10 == 0) {
            // Every 10 training samples, calculate stability
            float recentSum = 0.0f;
            size_t recentCount = std::min(pImpl->trainingErrorHistory.size(), size_t(10));
            for (size_t i = pImpl->trainingErrorHistory.size() - recentCount;
                 i < pImpl->trainingErrorHistory.size(); ++i) {
                recentSum += pImpl->trainingErrorHistory[i];
            }
            float recentAverage = recentSum / recentCount;
            pImpl->stabilityMetrics.push_back(1.0f - recentAverage); // Higher stability when error is low
        }
    }
}

} // namespace nlm
