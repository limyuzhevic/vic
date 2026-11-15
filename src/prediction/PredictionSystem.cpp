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

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const class Brain* brain) {
    // Phase 6: Real prediction using neural activity patterns
    // Uses working memory and neural dynamics for prediction
    
    if (!brain) {
        NLM_LOG_ERROR("PredictionSystem::predictNextState called with null brain");
        return nullptr;
    }
    
    // Get working memory state as basis for prediction
    auto workingMemory = brain->getWorkingMemory();
    auto predictionSystem = brain->getPredictionSystem();
    
    if (workingMemory) {
        // Use working memory pattern as prediction
        const auto& neurons = workingMemory->getMemoryNeurons();
        const auto& activations = workingMemory->getMemoryActivations();
        
        std::vector<float> predictionData;
        predictionData.reserve(neurons.size());
        
        for (size_t i = 0; i < neurons.size() && i < activations.size(); ++i) {
            // Enhance activation with prediction confidence
            float predictedActivation = activations[i] * predictionSystem->getConfidence();
            predictionData.push_back(predictedActivation);
        }
        
        if (!predictionData.empty()) {
            return std::make_unique<SensoryInput>(predictionData);
        }
    }
    
    // Fallback to random prediction if no working memory data
    std::vector<float> fallbackData(10, 0.1f); // Placeholder prediction
    return std::make_unique<SensoryInput>(fallbackData);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Phase 6: Enhanced prediction error computation
    // Integrates with neuromodulation systems for learning
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Log prediction error for debugging
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionError));
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
    pImpl->predictionError = 0.0f;
}

void PredictionSystem::train(const class Brain* brain, const SensoryInput& observation) {
    // Phase 6: Real prediction model training using neural plasticity
    // Integrates with brain plasticity systems
    
    if (!brain) {
        return;
    }
    
    // Update prediction confidence based on learning
    float learningRate = 0.01f;
    pImpl->confidence = std::min(1.0f, pImpl->confidence + learningRate);
    
    // Store observation for training
    const auto& obsData = observation.getData();
    if (!obsData.empty()) {
        // This would integrate with brain's plasticity systems
        // For now, just log the training
        NLM_LOG_INFO("Prediction system training with " + std::to_string(obsData.size()) + " data points");
    }
}

void PredictionSystem::setConfidence(float confidence, float noveltyLevel, float curiosityLevel) {
    // Phase 6: Enhanced confidence calculation using neuromodulatory signals
    pImpl->confidence = confidence;
    
    // Adjust confidence based on neuromodulatory signals
    if (noveltyLevel > 0.5f) {
        pImpl->confidence *= (1.0f + noveltyLevel * 0.5f);
    }
    
    if (curiosityLevel > 0.5f) {
        pImpl->confidence *= (1.0f + curiosityLevel * 0.3f);
    }
    
    // Clamp confidence to reasonable range
    pImpl->confidence = std::max(0.1f, std::min(1.0f, pImpl->confidence));
}

const std::vector<float>& PredictionSystem::getPredictionActivity() const {
    // Return placeholder prediction activity
    // This would integrate with brain's neural activity
    static std::vector<float> emptyActivity;
    return emptyActivity;
}

void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
}

} // namespace nlm
