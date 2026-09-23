#include "PredictionSystem.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionPatterns;
    std::unordered_map<size_t, std::vector<float>> actionPredictions;
    float accumulatedReward;
    
    Impl() : predictionError(0.0f), confidence(0.5f), accumulatedReward(0.0f) {}
};

PredictionSystem::PredictionSystem() 
    : pImpl(new Impl), brain_(nullptr), predictionHorizon_(1.0f), learningRate_(0.01f), confidenceThreshold_(0.5f) {
    std::random_device rd;
    rng_.seed(rd());
    // Initialize prediction weights for different sensory dimensions
    predictionWeights_.resize(256, 1.0f);  // Default to 256 sensory dimensions
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("PredictionSystem initialized with neural substrate");
    
    // Initialize with some default prediction patterns
    predictionPatterns_.push_back({0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
    predictionPatterns_.push_back({0.9f, 0.8f, 0.7f, 0.6f, 0.5f});
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!brain_ || !currentState.getData().empty()) {
        return currentState.clone();
    }
    
    // Use neural activity patterns to generate predictions
    auto predictedState = std::make_unique<SensoryInput>();
    const auto& currentData = currentState.getData();
    
    // Create prediction based on current sensory patterns
    // This simulates predictive coding using the brain's neural substrate
    std::vector<float> predictedData(currentData.size());
    
    for (size_t i = 0; i < currentData.size() && i < predictionWeights_.size(); ++i) {
        // Apply learned prediction weights and neural modulation
        float prediction = currentData[i] * predictionWeights_[i % predictionWeights_.size()];
        
        // Add neural noise and modulation
        std::normal_distribution<float> noise(0.0f, 0.1f);
        prediction += noise(rng_);
        
        // Apply action-conditioned predictions if brain has motor neurons
        for (const auto& [action, predictions] : pImpl->actionPredictions) {
            if (!predictions.empty() && i < predictions.size()) {
                prediction += predictions[i] * 0.2f;  // Action contribution
            }
        }
        
        predictedData[i] = std::max(0.0f, std::min(prediction, 1.0f));
    }
    
    predictedState->setData(predictedData);
    
    // Calculate prediction confidence based on error patterns
    float error = 0.0f;
    for (size_t i = 0; i < predictedData.size() && i < currentData.size(); ++i) {
        error += std::abs(predictedData[i] - currentData[i]);
    }
    pImpl->predictionError = error / std::max(1.0f, static_cast<float>(predictedData.size()));
    
    // Update confidence based on prediction accuracy
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError * 2.0f);
    
    if (pImpl->predictionError > 0.5f) {
        NLM_LOG_WARNING("Prediction error is high: " + std::to_string(pImpl->predictionError));
    }
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        float maxError = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float absDiff = std::abs(diff);
            sumError += absDiff;
            maxError = std::max(maxError, absDiff);
        }
        
        // Combine different error metrics
        pImpl->predictionError = sumError / predData.size();
        float patternError = maxError;
        
        // Update prediction patterns based on error
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Adjust confidence based on error magnitude
        pImpl->confidence = std::max(0.0f, 1.0f - patternError * 3.0f);
        
        // Learn from prediction error
        train(actual);
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
    if (!brain_ || observation.getData().empty()) return;
    
    const auto& data = observation.getData();
    
    // Update prediction weights based on observation
    for (size_t i = 0; i < std::min(data.size(), predictionWeights_.size()); ++i) {
        float gradient = data[i] - predictionWeights_[i];
        predictionWeights_[i] += learningRate_ * gradient;
        
        // Keep weights in reasonable range
        predictionWeights_[i] = std::max(0.0f, std::min(predictionWeights_[i], 2.0f));
    }
    
    // Update prediction patterns
    std::vector<float> pattern(data.begin(), data.end());
    if (pattern.size() > 5) {
        // Add scaled pattern variations
        std::vector<float> scaledPattern = pattern;
        for (float& val : scaledPattern) {
            val *= 0.5f;
        }
        predictionPatterns_.push_back(scaledPattern);
        
        if (predictionPatterns_.size() > 10) {
            predictionPatterns_.erase(predictionPatterns_.begin());
        }
    }
    
    // Accumulate reward for future predictions
    float observationEnergy = std::accumulate(data.begin(), data.end(), 0.0f);
    pImpl->accumulatedReward += observationEnergy * learningRate_;
    
    NLM_LOG_DEBUG("Prediction model trained with error: " + std::to_string(pImpl->predictionError));
}

void PredictionSystem::update(double timestep) {
    // Update internal prediction model state
    // This integrates with brain's learning dynamics
    
    // Gradually adjust confidence based on accumulated reward
    if (pImpl->accumulatedReward > 0.0f) {
        pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.001f * timestep);
    } else {
        pImpl->confidence = std::max(0.1f, pImpl->confidence - 0.001f * timestep);
    }
    
    // Update prediction weights based on recent errors
    if (!pImpl->errorHistory.empty()) {
        float recentError = pImpl->errorHistory.back();
        if (recentError > confidenceThreshold_) {
            // Adjust weights if prediction error is too high
            for (float& weight : predictionWeights_) {
                weight *= (1.0f - learningRate_);
            }
        }
    }
}

std::vector<std::pair<float, std::unique_ptr<SensoryInput>>> PredictionSystem::getTopPredictions(int k) const {
    std::vector<std::pair<float, std::unique_ptr<SensoryInput>>> predictions;
    
    // Generate predictions from stored patterns
    for (const auto& pattern : predictionPatterns_) {
        auto predState = std::make_unique<SensoryInput>();
        predState->setData(pattern);
        
        // Calculate pattern similarity score
        float score = 0.0f;
        for (float val : pattern) {
            score += val;
        }
        
        predictions.emplace_back(score, std::move(predState));
    }
    
    // Sort by score (highest first)
    std::sort(predictions.begin(), predictions.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top k
    if (k > static_cast<int>(predictions.size())) {
        k = static_cast<int>(predictions.size());
    }
    
    std::vector<std::pair<float, std::unique_ptr<SensoryInput>>> topK;
    topK.reserve(k);
    for (int i = 0; i < k; ++i) {
        topK.push_back(std::move(predictions[i]));
    }
    
    return topK;
}

} // namespace nlm
