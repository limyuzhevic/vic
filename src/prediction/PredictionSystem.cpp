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
    // PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, use prediction error history to make simple predictions
    if (!pImpl->errorHistory.empty()) {
        // Create a prediction based on recent patterns
        float avgError = 0.0f;
        for (float error : pImpl->errorHistory) {
            avgError += error;
        }
        avgError /= pImpl->errorHistory.size();
        
        // Use current state as base prediction, modify based on error
        class PredictedSensoryInput : public SensoryInput {
        public:
            PredictedSensoryInput(const std::vector<float>& data, float error)
                : baseError(error) {
                data_ = data;
                // Add small noise based on prediction error
                for (size_t i = 0; i < data.size(); ++i) {
                    data_[i] = data_[i] + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * error * 0.1f;
                }
            }
            const char* getType() const override { return "PredictedSensory"; }
            const std::vector<float>& getData() const override { return data_; }
            size_t getDimensions() const override { return data_.size(); }
            std::unique_ptr<SensoryInput> clone() const override {
                return std::make_unique<PredictedSensoryInput>(data_, baseError);
            }
        private:
            std::vector<float> data_;
            float baseError;
        };
        
        return std::make_unique<PredictedSensoryInput>(currentState.getData(), avgError);
    }
    
    // Return current state as default prediction
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
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
