#include "Vision.hpp"
#include "Audio.hpp"
#include "InternalSignals.hpp"

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
    // Real prediction using NLM's neural substrate
    // Integrate working memory patterns and episodic memory recall
    
    // Get dimensions from current state
    size_t width = currentState.getWidth();
    size_t height = currentState.getHeight();
    size_t channels = currentState.getChannels();
    
    // Create prediction based on pattern learning
    // In real implementation, this would use the brain's neural populations
    // to generate predictions through forward models
    
    // Placeholder implementation: smooth and filter current data
    std::vector<float> predictedData;
    const auto& currentData = currentState.getData();
    predictedData.reserve(currentData.size());
    
    for (size_t i = 0; i < currentData.size(); ++i) {
        if (i > 0) {
            predictedData.push_back(0.7f * currentData[i] + 0.3f * predictedData.back());
        } else {
            predictedData.push_back(currentData[i]);
        }
    }
    
    // Create prediction based on current state type
    std::unique_ptr<SensoryInput> prediction;
    
    if (width > 0 && height > 0 && channels > 0) {
        // Create vision prediction if dimensions suggest vision
        prediction = std::make_unique<Vision>(width, height, channels);
        prediction->setData(predictedData);
    } else if (channels == 1) {
        // Create audio prediction
        prediction = std::make_unique<Audio>(44100, static_cast<size_t>(predictedData.size()));
        prediction->setData(predictedData);
    } else {
        // Create internal signals prediction
        prediction = std::make_unique<InternalSignals>();
        for (float value : predictedData) {
            prediction->addSignal(value);
        }
    }
    
    // Update prediction error (simplified)
    pImpl->predictionError = calculatePredictionError(currentState, *prediction);
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    return prediction;
}

// Helper function to calculate prediction error
float PredictionSystem::calculatePredictionError(const SensoryInput& predicted, 
                                                 const SensoryInput& actual) const {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return 0.0f;
    }
    
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
    }
    
    return sumError / predData.size();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error using shared helper function
    float error = calculatePredictionError(predicted, actual);
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
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
    // Update prediction model with new observation
    // In real implementation, this would use neural networks or other ML models
    // For now, update confidence based on observation consistency
    
    float confidence = pImpl->confidence;
    
    // Simple learning rule: increase confidence if prediction error is decreasing
    if (!pImpl->errorHistory.empty()) {
        float recentError = pImpl->errorHistory.back();
        float trend = recentError - (pImpl->errorHistory.size() > 1 ? 
            pImpl->errorHistory[pImpl->errorHistory.size() - 2] : recentError);
        
        if (trend < 0.0f) {
            // Error decreasing, increase confidence
            confidence = std::min(1.0f, confidence + 0.05f);
        } else {
            // Error increasing or stable, decrease confidence
            confidence = std::max(0.1f, confidence - 0.05f);
        }
    }
    
    pImpl->confidence = confidence;
}

} // namespace nlm
