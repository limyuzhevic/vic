#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include "../sensory/InternalSignals.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Predictive coding parameters
    float predictionError_{0.0f};
    float confidence_{0.5f};
    float predictionRate_{0.1f};
    float learningRate_{0.01f};
    
    // Historical data
    std::vector<float> errorHistory_;
    std::vector<std::unique_ptr<SensoryInput>> predictionHistory_;
    
    // Model parameters
    std::vector<float> weights_;
    float weightDecay_{0.999f};
    
    // Random generator for exploration
    std::mt19937 rng_{std::random_device{}()};
    std::uniform_real_distribution<float> distribution_{0.0f, 1.0f};
};

PredictionSystem::PredictionSystem() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("PredictionSystem initialized");
    // Initialize with reasonable size
    pImpl->errorHistory_.reserve(1000);
    pImpl->predictionHistory_.reserve(100);
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    NLM_LOG_DEBUG("Generating prediction for next state");
    
    // Create prediction based on current state
    auto predicted = std::make_unique<SensoryInput>();
    
    // Copy current sensory attributes as baseline prediction
    predicted->setType(currentState.getType());
    predicted->setTimestamp(currentState.getTimestamp() + 1.0f);
    
    // Add some noise/uncertainty for realistic predictions
    float uncertainty = 1.0f - pImpl->confidence_;
    
    // For Vision input
    if (dynamic_cast<const Vision*>(&currentState)) {
        const auto& vision = dynamic_cast<const Vision&>(currentState);
        predicted->setData(vision.getData());
        
        // Add noise to simulate prediction uncertainty
        auto& data = predicted->getData();
        for (auto& value : data) {
            if (distribution_(pImpl->rng_) < uncertainty) {
                value += (distribution_(pImpl->rng_) - 0.5f) * 0.1f; // 10% noise
            }
        }
    }
    // For Audio input
    else if (dynamic_cast<const Audio*>(&currentState)) {
        const auto& audio = dynamic_cast<const Audio&>(currentState);
        predicted->setData(audio.getData());
        
        // Predict slight pitch/duration changes
        auto& data = predicted->getData();
        for (size_t i = 0; i < data.size(); ++i) {
            if (distribution_(pImpl->rng_) < uncertainty) {
                data[i] *= (1.0f + (distribution_(pImpl->rng_) - 0.5f) * 0.05f);
            }
        }
    }
    // For Internal signals
    else if (dynamic_cast<const InternalSignals*>(&currentState)) {
        const auto& internal = dynamic_cast<const InternalSignals&>(currentState);
        // Internal signals evolve based on noise and prediction error
        auto& signals = predicted->getInternal();
        signals = internal.getInternal();
        
        for (float& signal : signals) {
            signal += (distribution_(pImpl->rng_) - 0.5f) * 0.1f * pImpl->predictionError_;
        }
    }
    
    // Store prediction in history
    pImpl->predictionHistory_.push_back(std::make_unique<SensoryInput>(*predicted));
    if (pImpl->predictionHistory_.size() > 100) {
        pImpl->predictionHistory_.erase(pImpl->predictionHistory_.begin());
    }
    
    // Update confidence based on prediction error
    float newConfidence = pImpl->confidence_ * 0.95f + 0.05f * (1.0f - std::abs(pImpl->predictionError_));
    pImpl->confidence_ = std::max(0.1f, std::min(1.0f, newConfidence));
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error as difference between predicted and actual
    float error = 0.0f;
    
    if (predicted->getType() == actual.getType()) {
        const auto& predData = predicted->getData();
        const auto& actualData = actual.getData();
        
        size_t minSize = std::min(predData.size(), actualData.size());
        for (size_t i = 0; i < minSize; ++i) {
            float diff = predData[i] - actualData[i];
            error += diff * diff;
        }
        error = std::sqrt(error / static_cast<float>(minSize));
        
        // Add to history
        pImpl->errorHistory_.push_back(error);
        if (pImpl->errorHistory_.size() > 1000) {
            pImpl->errorHistory_.erase(pImpl->errorHistory_.begin());
        }
        
        // Update prediction error
        pImpl->predictionError_ = error;
        
        // Learn from error
        pImpl->learnFromError(error);
        
        NLM_LOG_DEBUG("Prediction error: " + std::to_string(error) + ", Confidence: " + std::to_string(pImpl->confidence_));
    }
}

void PredictionSystem::Impl::learnFromError(float error) {
    // Simple Hebbian-like learning: larger errors strengthen connections
    if (!weights_.empty()) {
        for (size_t i = 0; i < weights_.size(); ++i) {
            // Weight adjustment based on prediction error
            float adjustment = learningRate_ * error * (distribution_(rng_) - 0.5f);
            weights_[i] += adjustment;
            
            // Apply weight decay for stability
            weights_[i] *= weightDecay_;
            
            // Clamp weights to reasonable range
            weights_[i] = std::max(-1.0f, std::min(1.0f, weights_[i]));
        }
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Simple predictive training: use observation to update internal model
    float prediction = 0.0f;
    
    if (dynamic_cast<const Vision*>(&observation)) {
        const auto& vision = dynamic_cast<const Vision&>(observation);
        const auto& data = vision.getData();
        
        // Compute prediction as average with some noise
        for (float value : data) {
            prediction += value;
        }
        prediction /= static_cast<float>(data.size());
        
        // Add exploration noise
        prediction += (distribution_(pImpl->rng_) - 0.5f) * 0.1f;
        
        // Update weights with observation
        pImpl->weights_.assign(data.begin(), data.end());
        
    } else if (dynamic_cast<const Audio*>(&observation)) {
        const auto& audio = dynamic_cast<const Audio&>(observation);
        const auto& data = audio.getData();
        
        for (float value : data) {
            prediction += value;
        }
        prediction /= static_cast<float>(data.size());
        
        pImpl->weights_.assign(data.begin(), data.end());
    }
    
    // Store training data
    pImpl->predictionHistory_.push_back(std::make_unique<SensoryInput>(observation));
    if (pImpl->predictionHistory_.size() > 100) {
        pImpl->predictionHistory_.erase(pImpl->predictionHistory_.begin());
    }
    
    NLM_LOG_DEBUG("Training completed, prediction: " + std::to_string(prediction));
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError_;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence_;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory_;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory_.clear();
    pImpl->predictionHistory_.clear();
    pImpl->predictionError_ = 0.0f;
    pImpl->confidence_ = 0.5f;
    NLM_LOG_INFO("Prediction system history cleared");
}

} // namespace nlm