#include "PredictionError.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

struct PredictionError::Impl {
    float currentLevel;
    float baseline;
    float maxLevel;
    float learningRate;
    float memoryTraceDecay;
    std::vector<float> errorHistory;
    size_t historySize;
    float weightedPredictionError;
    SimulationStep lastPredictionTime;
    
    Impl() : currentLevel(0.0f), baseline(0.02f), maxLevel(0.5f),
             learningRate(0.15f), memoryTraceDecay(0.95f),
             historySize(50), weightedPredictionError(0.0f),
             lastPredictionTime(0) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getLevel() const {
    return currentLevel;
}

void PredictionError::update(float predictionError) {
    // Real prediction error system with temporal dynamics
    
    // Update error history
    pImpl->errorHistory.push_back(predictionError);
    pImpl->lastPredictionTime = predictionError;
    
    if (pImpl->errorHistory.size() > pImpl->historySize) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Compute weighted prediction error (recent errors weighted more)
    pImpl->weightedPredictionError = 0.0f;
    float weightSum = 0.0f;
    
    for (size_t i = 0; i < pImpl->errorHistory.size(); ++i) {
        float weight = std::pow(pImpl->memoryTraceDecay, static_cast<float>(i));
        pImpl->weightedPredictionError += pImpl->errorHistory[i] * weight;
        weightSum += weight;
    }
    
    if (weightSum > 0.0f) {
        pImpl->weightedPredictionError /= weightSum;
    }
    
    // Update current level with learning dynamics
    float learningDelta = pImpl->weightedPredictionError - pImpl->currentLevel;
    pImpl->currentLevel += learningDelta * pImpl->learningRate;
    
    // Apply bounds
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, pImpl->baseline, pImpl->maxLevel);
}

float PredictionError::getWeightedError() const {
    return pImpl->weightedPredictionError;
}

float PredictionError::getAverageError() const {
    if (pImpl->errorHistory.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (float error : pImpl->errorHistory) {
        sum += std::abs(error);
    }
    return sum / pImpl->errorHistory.size();
}

void PredictionError::reset() {
    pImpl->currentLevel = pImpl->baseline;
    pImpl->errorHistory.clear();
    pImpl->weightedPredictionError = 0.0f;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.01f, 0.5f);
}

void PredictionError::setMaxLevel(float max) {
    pImpl->maxLevel = std::max(0.01f, max);
}

void PredictionError::setMemoryTraceDecay(float decay) {
    pImpl->memoryTraceDecay = std::clamp(decay, 0.1f, 1.0f);
}

void PredictionError::initializeRandom(RandomGenerator& rng) {
    // Initialize with biologically plausible prediction error parameters
    pImpl->baseline = rng.uniformReal(0.01f, 0.05f);
    pImpl->maxLevel = rng.uniformReal(0.3f, 0.8f);
    pImpl->learningRate = rng.uniformReal(0.1f, 0.3f);
    pImpl->memoryTraceDecay = rng.uniformReal(0.8f, 0.99f);
    
    // Reset to baseline
    pImpl->currentLevel = pImpl->baseline;
    pImpl->errorHistory.clear();
    pImpl->weightedPredictionError = 0.0f;
}

} // namespace nlm
