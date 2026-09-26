#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    bool initialized;
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    Impl() : initialized(false), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
    pImpl->initialized = true;
}

bool PredictionSystem::isInitialized() const {
    return pImpl->initialized;
}

NeuralPrediction* PredictionSystem::getNeuralPrediction() const {
    return pImpl->neuralPrediction.get();
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->initialized) {
        return currentState.clone();
    }
    
    // Use NeuralPrediction to generate predictions from neural dynamics
    const auto& sensoryData = currentState.getData();
    auto* brain = pImpl->brain;
    
    // Get current sensory neurons' activity
    std::vector<float> predictedState;
    
    // For now, implement a simple neural prediction mechanism
    // In a real implementation, this would use NeuralPrediction
    predictedState = sensoryData;
    
    return std::make_unique<SensoryInput>(predictedState);
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->initialized) {
        return;
    }
    
    // Calculate prediction error using neural prediction system
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->brain->getCurrentStep());
        float predictionError = sumError / predData.size();
        
        static_cast<PredictionSystem*>(this)->clearHistory();
        static_cast<PredictionSystem*>(this)->getNeuralPrediction()->getErrorHistory().push_back(predictionError);
    }
}

float PredictionSystem::getPredictionError() const {
    if (!pImpl->initialized || !pImpl->neuralPrediction) {
        return 0.0f;
    }
    return pImpl->neuralPrediction->getPredictionError();
}

float PredictionSystem::getConfidence() const {
    if (!pImpl->initialized || !pImpl->neuralPrediction) {
        return 0.5f;
    }
    return pImpl->neuralPrediction->getPredictionConfidence();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    static std::vector<float> emptyHistory;
    if (!pImpl->initialized || !pImpl->neuralPrediction) {
        return emptyHistory;
    }
    const auto& errorHistory = pImpl->neuralPrediction->getErrorHistory();
    return std::vector<float>(errorHistory.begin(), errorHistory.end());
}

void PredictionSystem::clearHistory() {
    if (pImpl->initialized && pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (pImpl->initialized && pImpl->neuralPrediction) {
        pImpl->neuralPrediction->train(observation);
    }
}

} // namespace nlm
