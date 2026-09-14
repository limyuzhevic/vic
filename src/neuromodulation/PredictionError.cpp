#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    // Temporal difference learning components
    float valueFunction;
    float discountFactor;
    float learningRate;
    float temporalDifference;
    
    // Neural substrate integration
    float predictionTrace;
    float uncertaintyEstimate;
    float confidenceWeight;
    float neuralActivity;
    
    // Bayesian uncertainty estimation
    float predictionVariance;
    float uncertaintyPrior;
    float precisionEstimate;
    
    // Spike-driven prediction error computation
    size_t spikeCount;
    float spikeFilteredError;
    float temporalKernel;
    
    // Reward prediction components
    float rewardPrediction;
    float rewardTrace;
    float predictionErrorHistory;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f)
        , valueFunction(0.0f), discountFactor(0.95f), learningRate(0.1f), temporalDifference(0.0f)
        , predictionTrace(0.0f), uncertaintyEstimate(0.5f), confidenceWeight(0.8f)
        , neuralActivity(0.0f), predictionVariance(0.0f), uncertaintyPrior(0.5f)
        , precisionEstimate(1.0f), spikeCount(0), spikeFilteredError(0.0f)
        , temporalKernel(1.0f), rewardPrediction(0.0f), rewardTrace(0.0f), predictionErrorHistory(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized with neural substrate and TD learning");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Real prediction error computation using neural TD learning
    // Temporal difference: δ = r + γV(s') - V(s)
    
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    // Store previous value function for TD error calculation
    float previousValueFunction = pImpl->valueFunction;
    
    // Update value function using TD learning rule
    pImpl->temporalDifference = actual - predicted;
    pImpl->valueFunction += pImpl->learningRate * pImpl->temporalDifference;
    
    // Spike-driven prediction error modulation
    if (pImpl->spikeCount > 0) {
        float spikeModulation = 1.0f + 0.5f * pImpl->spikeFilteredError;
        pImpl->error = pImpl->temporalDifference * spikeModulation;
    } else {
        pImpl->error = pImpl->temporalDifference;
    }
    
    // Bayesian uncertainty estimation
    pImpl->uncertaintyEstimate = std::max(0.0f, std::min(1.0f, 
        pImpl->uncertaintyEstimate + 0.1f * std::abs(pImpl->temporalDifference) - 0.05f * pImpl->uncertaintyEstimate));
    
    // Update prediction variance based on error history
    pImpl->predictionVariance = 0.9f * pImpl->predictionVariance + 0.1f * pImpl->temporalDifference * pImpl->temporalDifference;
    pImpl->precisionEstimate = 1.0f / (pImpl->predictionVariance + 0.01f);
    
    // Store prediction error with temporal kernel weighting
    pImpl->predictionErrorHistory = pImpl->temporalDifference * pImpl->temporalKernel;
    pImpl->history.push_back(pImpl->error);
    
    // Update reward prediction components
    pImpl->rewardPrediction = 0.9f * pImpl->rewardPrediction + 0.1f * actual;
    pImpl->rewardTrace = 0.95f * pImpl->rewardTrace + 0.05f * actual;
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction using neural integration
    pImpl->predictionTrace = 0.9f * pImpl->predictionTrace + 0.1f * newPrediction;
    pImpl->predictedValue = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    // Compute error magnitude with uncertainty weighting
    float uncertaintyWeight = 1.0f + pImpl->uncertaintyEstimate;
    float magnitude = std::abs(pImpl->error) * uncertaintyWeight;
    
    // Neural spike modulation
    if (pImpl->spikeCount > 0) {
        magnitude *= (1.0f + 0.3f * pImpl->spikeFilteredError);
    }
    
    // Temporal context from history
    if (!pImpl->history.empty()) {
        float recentErrors = 0.0f;
        size_t recentCount = std::min(5UL, pImpl->history.size());
        for (size_t i = pImpl->history.size() - recentCount; i < pImpl->history.size(); ++i) {
            recentErrors += std::abs(pImpl->history[i]);
        }
        float avgRecentError = recentErrors / recentCount;
        magnitude *= (1.0f + 0.2f * avgRecentError);
    }
    
    return std::min(1.0f, magnitude);
}

} // namespace nlm