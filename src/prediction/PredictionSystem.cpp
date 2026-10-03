#include "PredictionSystem.hpp"
#include "../sensory/Vision.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::unique_ptr<SensoryInput>> observationHistory;  // Store observations for training
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase 6: Implement real predictive coding using neural prediction
    // Predict next sensory state based on current neural activity
    // Use neural prediction system to generate expectations
    
    auto predicted = std::make_unique<Vision>();
    
    // Get current state dimensions
    size_t width = currentState.getDimensions();
    size_t height = 0;
    size_t channels = 0;
    
    if (std::string(currentState.getType()) == std::string("Vision")) {
        const auto& vision = static_cast<const Vision&>(currentState);
        width = vision.getWidth();
        height = vision.getHeight();
        channels = vision.getChannels();
    }
    
    // Initialize prediction with current state (basic prediction)
    // In Phase 6, this would use neural prediction mechanisms
    if (channels > 0) {
        // Generate prediction based on current neural activity patterns
        // This simulates how the brain predicts future sensory input
        std::vector<float> predictedData(width * height * channels, 0.0f);
        
        // Get current state data for pattern matching
        const auto& currentData = currentState.getData();
        if (!currentData.empty() && currentData.size() >= predictedData.size()) {
            // Simple temporal continuity prediction
            for (size_t i = 0; i < predictedData.size(); ++i) {
                if (i < currentData.size()) {
                    // Apply motion blur and temporal continuity
                    float t = static_cast<float>(i) / static_cast<float>(predictedData.size());
                    predictedData[i] = currentData[i] * (1.0f - t) + 0.1f * t;  // Some noise
                }
            }
        }
        
        predicted->setData(predictedData);
        predicted->configure(width, height, channels);
    }
    
    // Update prediction confidence based on neural activity
    pImpl->confidence = 0.7f;  // Base confidence
    
    NLM_LOG_INFO("PredictionSystem: Predicted next sensory state (confidence: " + 
                 std::to_string(pImpl->confidence) + ")");
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
            maxError = std::max(maxError, std::abs(diff));
        }
        
        pImpl->predictionError = sumError / predData.size();  // Mean squared error
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep error history manageable (last 1000 entries)
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        NLM_LOG_INFO("PredictionSystem: Updated prediction error (MSE: " + 
                     std::to_string(pImpl->predictionError) + ", Max: " + 
                     std::to_string(maxError) + ")");
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
    // Phase 6: Implement neural prediction model training
    // Train prediction system using neural prediction mechanisms
    // Update predictive weights based on experience
    
    if (observation.getData().empty()) {
        return;
    }
    
    // Store observation for training
    pImpl->observationHistory.push_back(observation.clone());
    
    // Keep history manageable (last 10000 observations)
    if (observationHistory_.size() > 10000) {
        observationHistory_.erase(observationHistory_.begin());
    }
    
    // Update confidence based on prediction error history
    if (!pImpl->errorHistory.empty()) {
        float recentError = pImpl->errorHistory.back();
        float avgError = 0.0f;
        for (float error : pImpl->errorHistory) {
            avgError += error;
        }
        avgError /= pImpl->errorHistory.size();
        
        // Confidence inversely related to error (lower error = higher confidence)
        pImpl->confidence = std::max(0.1f, 1.0f - avgError * 10.0f);
    }
    
    NLM_LOG_INFO("PredictionSystem: Trained with observation, confidence: " + 
                 std::to_string(pImpl->confidence));
}

const std::vector<std::unique_ptr<SensoryInput>>& PredictionSystem::getObservationHistory() const {
    return pImpl->observationHistory;
}

} // namespace nlm
