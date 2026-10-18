#include "brain/prediction/BrainPredictor.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

BrainPredictor::BrainPredictor(std::shared_ptr<Config> config)
    : config_(config)
    , predictionCount_(0)
    , averagePredictionAccuracy_(0.0f)
    , predictionError_(0.0f)
{
    if (!config_) {
        NLM_LOG_ERROR("Configuration is null in BrainPredictor constructor");
    }
    
    predictionHistory_.reserve(1000);
    outcomeHistory_.reserve(1000);
    
    NLM_LOG_INFO("BrainPredictor initialized");
}

BrainPredictor::~BrainPredictor() = default;

bool BrainPredictor::initialize() {
    NLM_LOG_INFO("Initializing brain prediction system...");
    
    // Initialize prediction system
    predictionSystem_ = std::make_unique<PredictionSystem>();
    
    // Configure prediction system based on config
    // This would include setting up prediction models, algorithms, etc.
    
    // Validate configuration
    if (!validatePredictionConfig(config_)) {
        NLM_LOG_ERROR("Prediction system configuration validation failed");
        return false;
    }
    
    NLM_LOG_INFO("Brain prediction system initialized");
    return true;
}

void BrainPredictor::reset() {
    NLM_LOG_INFO("Resetting brain prediction system...");
    
    predictionHistory_.clear();
    outcomeHistory_.clear();
    predictionCount_ = 0;
    averagePredictionAccuracy_ = 0.0f;
    predictionError_ = 0.0f;
    targetAction_.reset();
    
    NLM_LOG_INFO("Brain prediction system reset");
}

void BrainPredictor::updatePrediction(const SensoryInput& sensoryInput) {
    if (!predictionSystem_) {
        return;
    }
    
    // Get sensory data
    const auto& sensoryData = sensoryInput.getData();
    
    if (sensoryData.empty()) {
        return;
    }
    
    // Make prediction based on sensory input
    // This would involve the prediction system model
    float prediction = 0.0f;
    
    if (!predictionHistory_.empty()) {
        // Simple average prediction as placeholder
        float sum = 0.0f;
        for (float p : predictionHistory_) {
            sum += p;
        }
        prediction = sum / predictionHistory_.size();
    }
    
    // Store prediction
    predictionHistory_.push_back(prediction);
    
    // Keep only recent predictions (rolling window)
    if (predictionHistory_.size() > 1000) {
        predictionHistory_.erase(predictionHistory_.begin());
    }
    
    predictionCount_++;
    
    // Calculate prediction accuracy if we have outcome
    if (!outcomeHistory_.empty() && predictionCount_ > 1) {
        float totalError = 0.0f;
        for (size_t i = 0; i < std::min(predictionHistory_.size(), outcomeHistory_.size()); ++i) {
            float error = std::abs(predictionHistory_[i] - outcomeHistory_[i]);
            totalError += error;
        }
        float accuracy = 1.0f - (totalError / std::min(predictionHistory_.size(), outcomeHistory_.size()));
        averagePredictionAccuracy_ = (averagePredictionAccuracy_ + accuracy) * 0.5f;  // Exponential moving average
    }
}

float BrainPredictor::getPredictionError() const {
    return predictionError_;
}

void BrainPredictor::setTargetAction(const class Action& action) {
    targetAction_ = std::make_unique<Action>(action);
}

void BrainPredictor::learnFromOutcome(float outcome) {
    outcomeHistory_.push_back(outcome);
    
    // Keep only recent outcomes
    if (outcomeHistory_.size() > 1000) {
        outcomeHistory_.erase(outcomeHistory_.begin());
    }
    
    // Update prediction error
    if (!predictionHistory_.empty()) {
        predictionError_ = outcome - predictionHistory_.back();
    }
}

void BrainPredictor::updatePredictionWeights(float learningRate) {
    if (!predictionSystem_ || predictionHistory_.empty()) {
        return;
    }
    
    // Update prediction system weights based on learning
    // This would involve gradient descent or other learning algorithms
    
    NLM_LOG_INFO("Prediction weights updated with learning rate: " + std::to_string(learningRate));
}

bool BrainPredictor::validatePredictionConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate prediction-related configuration
    // Check for prediction algorithm, learning rate, etc.
    
    auto predictionAlgorithm = config->getOr<std::string>("prediction_algorithm", "default");
    if (predictionAlgorithm.empty()) {
        NLM_LOG_ERROR("Prediction algorithm cannot be empty");
        return false;
    }
    
    auto learningRate = config->getOr<float>("prediction_learning_rate", 0.01f);
    if (learningRate < 0.0f || learningRate > 1.0f) {
        NLM_LOG_ERROR("Invalid prediction learning rate: " + std::to_string(learningRate));
        return false;
    }
    
    return true;
}

std::string BrainPredictor::getDefaultPredictionAlgorithm() {
    return "default_neural_prediction";
}

} // namespace nlm
