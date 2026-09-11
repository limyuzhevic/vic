#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Connection to NeuralPrediction for actual prediction
    std::shared_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use NeuralPrediction for actual prediction
    if (pImpl->neuralPrediction) {
        // Get prediction from NeuralPrediction
        auto predictedState = pImpl->neuralPrediction->generatePrediction(0);
        
        // Create SensoryInput from prediction
        auto result = std::make_unique<SensoryInput>();
        
        // For now, simple mapping - in reality would need proper sensory encoding
        if (!predictedState.empty()) {
            result->setData(predictedState);
        }
        
        return result;
    }
    
    // Fallback: return current state (placeholder)
    NLM_LOG_WARNING("PredictionSystem: No neural prediction available, returning current state");
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 6: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
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
        
        // Update confidence based on prediction accuracy
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError * 10.0f);
        
        NLM_LOG_DEBUG("PredictionSystem: Error=" + std::to_string(pImpl->predictionError) + 
                     " Confidence=" + std::to_string(pImpl->confidence));
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
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Initialize NeuralPrediction with brain reference
    // This will be done by Brain::initialize() when creating the prediction system
    
    // TODO PHASE 6: Actually train the prediction model with real data
    // For now, just log the training attempt
    NLM_LOG_DEBUG("PredictionSystem: Training with observation of size " + 
                 std::to_string(observation.getData().size()));
}

// Initialize NeuralPrediction with brain reference when available
void PredictionSystem::initializeWithBrain(Brain* brain) {
    if (brain && !pImpl->neuralPrediction) {
        pImpl->neuralPrediction = std::make_shared<NeuralPrediction>();
        pImpl->neuralPrediction->initialize(brain);
        
        // Configure NeuralPrediction based on config
        auto cfg = brain->getConfig();
        if (cfg) {
            size_t seqMemSize = cfg->getOr<size_t>("sequence_memory_size", 100);
            pImpl->neuralPrediction->setSequenceMemorySize(seqMemSize);
            
            size_t predHorizon = cfg->getOr<size_t>("prediction_horizon", 1);
            pImpl->neuralPrediction->setPredictionHorizon(predHorizon);
            
            pImpl->neuralPrediction->enableTemporalPrediction(true);
            pImpl->neuralPrediction->enableActionConsequencePrediction(true);
        }
        
        NLM_LOG_INFO("PredictionSystem: NeuralPrediction initialized successfully");
    }
}

bool PredictionSystem::isInitialized() const {
    return pImpl->neuralPrediction != nullptr;
}

std::vector<float> PredictionSystem::getPredictionErrorHistory() const {
    return pImpl->errorHistory;
}

float PredictionSystem::getAverageError() const {
    if (pImpl->errorHistory.empty()) return 0.0f;
    
    float sum = std::accumulate(pImpl->errorHistory.begin(), pImpl->errorHistory.end(), 0.0f);
    return sum / pImpl->errorHistory.size();
}

float PredictionSystem::getPredictionHorizon() const {
    if (pImpl->neuralPrediction) {
        return pImpl->neuralPrediction->getPredictionHorizon();
    }
    return 1;  // Default
}

} // namespace nlm
