// PredictionError implementation
#include "PredictionError.hpp"
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    float level;
    float baseline;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(class Brain* brain) {
    // Store brain reference if needed
}

void PredictionError::update(float predictionError, SimulationStep currentStep) {
    setLevel(predictionError);
    recordError(predictionError, currentStep);
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float PredictionError::computeError(const std::vector<float>& predicted,
                                   const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        totalDiff += std::abs(predicted[i] - actual[i]);
    }
    
    return totalDiff / predicted.size();
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::recordError(float error, SimulationStep step) {
    pImpl->history.push_back(error);
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float PredictionError::getModulationSignal() const {
    return std::min(1.0f, pImpl->level * 2.0f);
}

void PredictionError::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
}

} // namespace nlm
