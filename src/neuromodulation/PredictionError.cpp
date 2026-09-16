#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <cmath>
#include <limits>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    std::vector<float> filteredHistory;
    float confidenceInterval;
    float statisticalMean;
    float statisticalVariance;
    size_t maxHistorySize;
    float minValue;
    float maxValue;
    bool isInitialized;
    uint32_t invalidInputCount;
    float temporalDecay;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
              confidenceInterval(0.95f), statisticalMean(0.0f), statisticalVariance(0.0f),
              maxHistorySize(1000), minValue(-std::numeric_limits<float>::max()),
              maxValue(std::numeric_limits<float>::max()), isInitialized(false),
              invalidInputCount(0), temporalDecay(0.99f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionError system constructed");
}

PredictionError::~PredictionError() {
    if (pImpl) {
        clearHistory();
        pImpl.reset();
        NLM_LOG_INFO("PredictionError system destroyed");
    }
}

void PredictionError::validateInput(float value, const char* name) const {
    if (std::isnan(value)) {
        NLM_LOG_WARNING(std::string("Invalid NaN input for ") + name + std::string(": "));
        pImpl->invalidInputCount++;
        pImpl->error = 0.0f;
    } else if (std::isinf(value)) {
        NLM_LOG_WARNING(std::string("Invalid infinity input for ") + name + std::string(": "));
        pImpl->invalidInputCount++;
        pImpl->error = (value > 0.0f) ? std::numeric_limits<float>::max() : -std::numeric_limits<float>::max();
    } else if (value < pImpl->minValue || value > pImpl->maxValue) {
        NLM_LOG_WARNING(std::string("Input out of bounds for ") + name + std::string(": "));
        pImpl->invalidInputCount++;
        pImpl->error = value > 0.0f ? std::numeric_limits<float>::max() : -std::numeric_limits<float>::max();
    }
}

void PredictionError::validateState() const {
    if (!pImpl->isInitialized) {
        NLM_LOG_ERROR("PredictionError state not initialized - brain reference is null");
        pImpl->error = 0.0f;
        return;
    }
    
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Brain pointer is null in PredictionError");
        pImpl->invalidInputCount++;
    }
}

void PredictionError::updateFilteredHistory() {
    // Temporal filtering of error history with exponential decay
    if (pImpl->history.empty()) {
        pImpl->filteredHistory.clear();
        return;
    }
    
    pImpl->filteredHistory.clear();
    pImpl->filteredHistory.reserve(pImpl->history.size());
    
    float totalWeight = 0.0f;
    for (size_t i = 0; i < pImpl->history.size(); ++i) {
        float weight = std::pow(pImpl->temporalDecay, static_cast<float>(pImpl->history.size() - 1 - i));
        totalWeight += weight;
        pImpl->filteredHistory.push_back(pImpl->history[i] * weight);
    }
    
    if (totalWeight > 0.0f) {
        float scale = 1.0f / totalWeight;
        for (auto& value : pImpl->filteredHistory) {
            value *= scale;
        }
    }
}

void PredictionError::updateStatistics() {
    if (pImpl->history.empty()) {
        pImpl->statisticalMean = 0.0f;
        pImpl->statisticalVariance = 0.0f;
        return;
    }
    
    // Calculate mean
    float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    pImpl->statisticalMean = sum / static_cast<float>(pImpl->history.size());
    
    // Calculate variance
    float sumSq = 0.0f;
    for (float value : pImpl->history) {
        float diff = value - pImpl->statisticalMean;
        sumSq += diff * diff;
    }
    pImpl->statisticalVariance = (sumSq / static_cast<float>(pImpl->history.size()));
}

float PredictionError::calculateConfidenceInterval() const {
    if (pImpl->history.size() < 2) {
        return 0.0f;
    }
    
    float stddev = std::sqrt(pImpl->statisticalVariance);
    float zScore = 1.96f; // 95% confidence interval
    return zScore * stddev / std::sqrt(static_cast<float>(pImpl->history.size()));
}

void PredictionError::adaptError(float& error) {
    // Apply adaptation mechanisms for error scaling based on magnitude
    if (std::abs(error) > 1.0f) {
        // Scale large errors to maintain system stability
        error = error * 0.1f;
        NLM_LOG_DEBUG(std::string("Adapted large error: ") + std::to_string(error));
    }
    
    // Apply temporal filtering
    if (!pImpl->filteredHistory.empty()) {
        float filteredMean = std::accumulate(pImpl->filteredHistory.begin(), pImpl->filteredHistory.end(), 0.0f) / 
                           static_cast<float>(pImpl->filteredHistory.size());
        error += (error - filteredMean) * 0.01f; // Small correction
    }
}

void PredictionError::validateHistoryBounds() {
    if (pImpl->history.size() > pImpl->maxHistorySize) {
        // Remove oldest entries to maintain bounds
        size_t excess = pImpl->history.size() - pImpl->maxHistorySize;
        pImpl->history.erase(pImpl->history.begin(), pImpl->history.begin() + excess);
        NLM_LOG_INFO(std::string("PredictionError history trimmed, removed ") + std::to_string(excess) + std::string(" entries"));
    }
}

void PredictionError::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionError with null brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    pImpl->isInitialized = true;
    
    // Load configuration parameters
    try {
        if (brain->getConfig()) {
            auto maxHistory = brain->getConfig()->getOr<size_t>("prediction.max_history_size", 1000);
            pImpl->maxHistorySize = maxHistory;
            
            auto minValue = brain->getConfig()->getOr<float>("prediction.min_value", -1000.0f);
            pImpl->minValue = minValue;
            
            auto maxValue = brain->getConfig()->getOr<float>("prediction.max_value", 1000.0f);
            pImpl->maxValue = maxValue;
            
            auto decayRate = brain->getConfig()->getOr<float>("prediction.temporal_decay", 0.99f);
            pImpl->temporalDecay = decayRate;
        }
    } catch (...) {
        NLM_LOG_WARNING("Failed to load PredictionError configuration, using defaults");
    }
    
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    if (!pImpl) return 0.0f;
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    if (!pImpl) {
        NLM_LOG_ERROR("Cannot compute error - PredictionError not properly initialized");
        return;
    }
    
    validateState();
    
    // Validate inputs
    validateInput(predicted, "predicted value");
    validateInput(actual, "actual value");
    
    // Apply bounds checking before computation
    float clampedPredicted = std::clamp(predicted, pImpl->minValue, pImpl->maxValue);
    float clampedActual = std::clamp(actual, pImpl->minValue, pImpl->maxValue);
    
    // Store original values
    pImpl->predictedValue = clampedPredicted;
    pImpl->actualValue = clampedActual;
    
    // Compute error with overflow protection
    float rawError = clampedActual - clampedPredicted;
    
    // Apply error adaptation
    adaptError(rawError);
    
    pImpl->error = rawError;
    
    // Validate bounds after computation
    if (std::isnan(pImpl->error)) {
        NLM_LOG_ERROR("Computed prediction error is NaN - applying fallback");
        pImpl->error = 0.0f;
    } else if (std::isinf(pImpl->error)) {
        NLM_LOG_ERROR("Computed prediction error is infinite - applying fallback");
        pImpl->error = (pImpl->error > 0.0f) ? std::numeric_limits<float>::max() : -std::numeric_limits<float>::max();
    }
    
    // Update history with bounds checking
    if (pImpl->error >= pImpl->minValue && pImpl->error <= pImpl->maxValue) {
        validateHistoryBounds();
        pImpl->history.push_back(pImpl->error);
    } else {
        NLM_LOG_WARNING(std::string("Error out of bounds, not recorded: ") + std::to_string(pImpl->error));
    }
    
    // Update statistics
    updateStatistics();
    updateFilteredHistory();
    
    // Integration with neuromodulation systems
    if (pImpl->brain && pImpl->invalidInputCount < 10) {
        try {
            Dopamine* dopamine = pImpl->brain->getDopamine();
            if (dopamine) {
                // Signal reward prediction error to dopamine system
                dopamine->signalRewardPredictionError(pImpl->error);
                
                // Update neuromodulation state based on error magnitude
                float level = getMagnitude() * 0.01f; // Scale error to neuromodulator level
                dopamine->setLevel(level);
            }
        } catch (...) {
            NLM_LOG_DEBUG("Failed to integrate with neuromodulation systems");
        }
    }
    
    // Recovery mechanism for corrupted state
    if (pImpl->invalidInputCount > 10) {
        NLM_LOG_WARNING(std::string("High invalid input count ") + std::to_string(pImpl->invalidInputCount) + std::string(" - attempting recovery"));
        
        // Reset history to recover
        clearHistory();
        pImpl->statisticalMean = 0.0f;
        pImpl->statisticalVariance = 0.0f;
        pImpl->invalidInputCount = 0;
        
        NLM_LOG_INFO("PredictionError state recovered");
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    if (!pImpl) return;
    
    validateInput(newPrediction, "new prediction");
    
    pImpl->predictedValue = std::clamp(newPrediction, pImpl->minValue, pImpl->maxValue);
    
    // Recompute error with updated prediction
    computeError(pImpl->predictedValue, pImpl->actualValue);
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

const std::vector<float>& PredictionError::getFilteredHistory() const {
    return pImpl->filteredHistory;
}

float PredictionError::getPredictionValue() const {
    return pImpl->predictedValue;
}

float PredictionError::getActualValue() const {
    return pImpl->actualValue;
}

float PredictionError::getConfidenceInterval() const {
    return calculateConfidenceInterval();
}

float PredictionError::getStatisticalMean() const {
    return pImpl->statisticalMean;
}

float PredictionError::getStatisticalVariance() const {
    return pImpl->statisticalVariance;
}

float PredictionError::getMagnitude() const {
    if (!pImpl) return 0.0f;
    return std::abs(pImpl->error);
}

float PredictionError::getStdDev() const {
    if (!pImpl) return 0.0f;
    return std::sqrt(pImpl->statisticalVariance);
}

void PredictionError::clearHistory() {
    if (!pImpl) return;
    
    pImpl->history.clear();
    pImpl->filteredHistory.clear();
    pImpl->statisticalMean = 0.0f;
    pImpl->statisticalVariance = 0.0f;
    
    NLM_LOG_DEBUG("PredictionError history cleared");
}

void PredictionError::reset() {
    if (!pImpl) return;
    
    clearHistory();
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->invalidInputCount = 0;
    
    NLM_LOG_INFO("PredictionError system reset");
}

bool PredictionError::isHealthy() const {
    if (!pImpl) return false;
    
    return pImpl->isInitialized && 
           pImpl->brain != nullptr && 
           pImpl->invalidInputCount < 100;
}

void PredictionError::logDiagnostics() const {
    if (!pImpl) return;
    
    NLM_LOG_INFO(std::string("PredictionError diagnostics: error=") + std::to_string(pImpl->error) +
                std::string(", history_size=") + std::to_string(pImpl->history.size()) +
                std::string(", invalid_count=") + std::to_string(pImpl->invalidInputCount) +
                std::string(", healthy=") + std::string(isHealthy() ? "true" : "false"));
}

} // namespace nlm
