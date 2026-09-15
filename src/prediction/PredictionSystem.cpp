#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "SensoryInput.hpp"
#include "Vision.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "SensoryInput.hpp"
#include "Vision.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::unique_ptr<NeuralPrediction> neuralPredictor;
    class Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {
        neuralPredictor = std::make_unique<NeuralPrediction>();
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(class Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPredictor->initialize(brain);
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain || !pImpl->neuralPredictor) {
        return currentState.clone();
    }
    
    const auto& currentData = currentState.getData();
    
    // Get current step from config for prediction
    size_t currentStep = pImpl->brain->getConfig()->getOr<size_t>("simulation_step", 0);
    
    // Generate prediction using neural predictor
    std::vector<float> predictedData = pImpl->neuralPredictor->generatePrediction(currentStep);
    
    if (predictedData.empty()) {
        // Fall back to current state if prediction fails
        return currentState.clone();
    }
    
    auto predictedInput = std::make_unique<Vision>(currentState.getDimensions() / 3, currentState.getDimensions() / 3, 1);
    predictedInput->setData(predictedData);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain || !pImpl->neuralPredictor) return;
    
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.empty() || actualData.empty()) return;
    
    size_t currentStep = pImpl->brain->getConfig()->getOr<size_t>("simulation_step", 0);
    
    pImpl->predictionError = pImpl->neuralPredictor->updateWithObservation(actualData, currentStep);
    
    if (!predictedData.empty() && !actualData.empty()) {
        float similarity = computeSimilarity(predictedData, actualData);
        pImpl->confidence = similarity;
    }
    
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    if (auto predictionError = pImpl->brain->getPredictionErrorSignal()) {
        predictionError->computeError(pImpl->predictionError, pImpl->predictionError);
        predictionError->recordError(pImpl->predictionError, currentStep);
    }
    
    if (auto curiosity = pImpl->brain->getCuriosity()) {
        float modulation = pImpl->predictionError * 0.5f;
        curiosity->update(modulation);
    }
    
    if (auto dopamine = pImpl->brain->getDopamine()) {
        float rewardPredictionError = 1.0f - pImpl->confidence;
        dopamine->signalRewardPredictionError(rewardPredictionError);
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
    if (pImpl->neuralPredictor) {
        pImpl->neuralPredictor->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain || !pImpl->neuralPredictor) return;
    
    const auto& observationData = observation.getData();
    
    if (!observationData.empty()) {
        size_t currentStep = pImpl->brain->getConfig()->getOr<size_t>("simulation_step", 0);
        pImpl->neuralPredictor->recordSensoryState(observationData, currentStep);
    }
}

static float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.empty() || b.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
