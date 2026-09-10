#include "NeuralPrediction.hpp"
#include "ActionConsequencePredictor.hpp"
#include "PredictionErrorSignal.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Integrated prediction components
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    std::unique_ptr<PredictionErrorSignal> predictionErrorSignal;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize integrated prediction subsystems
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->actionConsequencePredictor = std::make_unique<ActionConsequencePredictor>();
    pImpl->predictionErrorSignal = std::make_unique<PredictionErrorSignal>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Generate prediction using neural prediction subsystem
    const auto& sensoryData = currentState.getData();
    auto predicted = std::make_unique<SensoryInput>();
    
    // Use neural prediction for more sophisticated predictions
    if (pImpl->neuralPrediction) {
        std::vector<float> prediction = pImpl->neuralPrediction->generatePrediction(0); // Using step 0 as placeholder
        predicted->setData(prediction);
    } else {
        // Fallback: return copy of current state
        predicted = currentState.clone();
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error
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
    }
    
    // Update prediction error signal for neuromodulation
    if (pImpl->predictionErrorSignal) {
        pImpl->predictionErrorSignal->computeError(predData, actualData);
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

void PredictionSystem::initialize(Brain* brain) {
    if (brain && pImpl->neuralPrediction) {
        pImpl->neuralPrediction->initialize(brain);
    }
    if (brain && pImpl->actionConsequencePredictor) {
        pImpl->actionConsequencePredictor->initialize(brain);
    }
    NLM_LOG_INFO("PredictionSystem initialized");
}

// Accessor methods for integrated systems
NeuralPrediction* PredictionSystem::getNeuralPrediction() {
    return pImpl->neuralPrediction.get();
}

ActionConsequencePredictor* PredictionSystem::getActionConsequencePredictor() {
    return pImpl->actionConsequencePredictor.get();
}

PredictionErrorSignal* PredictionSystem::getPredictionErrorSignal() {
    return pImpl->predictionErrorSignal.get();
}

Curiosity* PredictionSystem::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* PredictionSystem::getNovelty() {
    return pImpl->novelty.get();
}

} // namespace nlm
