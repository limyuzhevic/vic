#include "PredictionSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction model components
    std::vector<float> predictionWeights;
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    
    // Prediction configuration
    float learningRate;
    float confidenceThreshold;
    float errorDecayRate;
    
    // State tracking
    float currentPredictionError;
    float currentConfidence;
    float predictionAccuracy;
    float predictionPrecision;
    float predictionRecall;
    
    // Integration
    Brain* brain;
    bool enabled;
    
    // Temporal dynamics
    std::vector<float> temporalWeights;
    size_t memoryWindow;
    size_t featureHistoryLength;
    
    Impl() 
        : learningRate(0.01f), confidenceThreshold(0.5f), errorDecayRate(0.995f)
        , currentPredictionError(0.0f), currentConfidence(0.5f)
        , predictionAccuracy(0.0f), predictionPrecision(0.0f), predictionRecall(0.0f)
        , brain(nullptr), enabled(true)
        , memoryWindow(100), featureHistoryLength(50) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl()) {}

PredictionSystem::~PredictionSystem() = default;

bool PredictionSystem::initialize(const class Config& config) {
    NLM_LOG_INFO("Initializing Prediction System (Phase 2.5: Advanced Predictive Coding)...");
    
    // Get configuration parameters
    size_t featureSize = 64;  // Default feature size
    if (config.has("prediction_feature_size")) {
        auto val = config.get<size_t>("prediction_feature_size");
        if (val) featureSize = *val;
    }
    
    // Initialize prediction weights with Xavier initialization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distribution(0.0f, std::sqrt(2.0f / featureSize));
    
    pImpl->predictionWeights.resize(featureSize * featureSize);
    pImpl->temporalWeights.resize(featureSize);
    
    for (size_t i = 0; i < featureSize * featureSize; ++i) {
        pImpl->predictionWeights[i] = distribution(gen) * 0.1f;
    }
    
    for (size_t i = 0; i < featureSize; ++i) {
        pImpl->temporalWeights[i] = distribution(gen) * 0.1f;
    }
    
    // Configure prediction parameters
    if (config.has("prediction_learning_rate")) {
        auto val = config.get<float>("prediction_learning_rate");
        if (val) pImpl->learningRate = *val;
    }
    
    if (config.has("prediction_confidence_threshold")) {
        auto val = config.get<float>("prediction_confidence_threshold");
        if (val) pImpl->confidenceThreshold = *val;
    }
    
    if (config.has("prediction_error_decay")) {
        auto val = config.get<float>("prediction_error_decay");
        if (val) pImpl->errorDecayRate = *val;
    }
    
    // Configure temporal dynamics
    if (config.has("prediction_memory_window")) {
        auto val = config.get<size_t>("prediction_memory_window");
        if (val) pImpl->memoryWindow = *val;
    }
    
    if (config.has("prediction_history_length")) {
        auto val = config.get<size_t>("prediction_history_length");
        if (val) pImpl->featureHistoryLength = *val;
    }
    
    NLM_LOG_INFO("Prediction System initialized successfully");
    NLM_LOG_INFO("  Features: " + std::to_string(featureSize));
    NLM_LOG_INFO("  Learning rate: " + std::to_string(pImpl->learningRate));
    NLM_LOG_INFO("  Confidence threshold: " + std::to_string(pImpl->confidenceThreshold));
    NLM_LOG_INFO("  Memory window: " + std::to_string(pImpl->memoryWindow));
    
    return true;
}

bool PredictionSystem::initialize(const class Config& config) {
    return initialize(config);
}

Brain* PredictionSystem::getBrain() {
    return pImpl->brain;
}

const Brain* PredictionSystem::getBrain() const {
    return pImpl->brain;
}

void PredictionSystem::enable(bool enabled) {
    pImpl->enabled = enabled;
    if (enabled) {
        NLM_LOG_INFO("Prediction System enabled");
    } else {
        NLM_LOG_INFO("Prediction System disabled");
    }
}

bool PredictionSystem::isEnabled() const {
    return pImpl->enabled;
}

float PredictionSystem::getAccuracy() const {
    return pImpl->predictionAccuracy;
}

float PredictionSystem::getPrecision() const {
    return pImpl->predictionPrecision;
}

float PredictionSystem::getRecall() const {
    return pImpl->predictionRecall;
}

bool PredictionSystem::exportModel(const std::string& filepath) const {
    NLM_LOG_INFO("Exporting prediction model to: " + filepath);
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + filepath);
        return false;
    }
    
    try {
        // Write model parameters
        size_t featureSize = std::sqrt(pImpl->predictionWeights.size());
        
        file.write(reinterpret_cast<const char*>(&featureSize), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(pImpl->predictionWeights.data()), 
                  pImpl->predictionWeights.size() * sizeof(float));
        file.write(reinterpret_cast<const char*>(&pImpl->learningRate), sizeof(float));
        file.write(reinterpret_cast<const char*>(&pImpl->confidenceThreshold), sizeof(float));
        file.write(reinterpret_cast<const char*>(&pImpl->errorDecayRate), sizeof(float));
        
        file.write(reinterpret_cast<const char*>(&pImpl->predictionAccuracy), sizeof(float));
        file.write(reinterpret_cast<const char*>(&pImpl->predictionPrecision), sizeof(float));
        file.write(reinterpret_cast<const char*>(&pImpl->predictionRecall), sizeof(float));
        
        NLM_LOG_INFO("Prediction model exported successfully (" + 
                    std::to_string(pImpl->predictionWeights.size() * sizeof(float) + 40) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting model: ") + e.what());
        return false;
    }
}

bool PredictionSystem::importModel(const std::string& filepath) {
    NLM_LOG_INFO("Importing prediction model from: " + filepath);
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for import: " + filepath);
        return false;
    }
    
    try {
        // Read model parameters
        size_t featureSize;
        if (!file.read(reinterpret_cast<char*>(&featureSize), sizeof(size_t))) {
            NLM_LOG_ERROR("Failed to read feature size from model file");
            return false;
        }
        
        pImpl->predictionWeights.resize(featureSize * featureSize);
        
        if (!file.read(reinterpret_cast<char*>(pImpl->predictionWeights.data()), 
                      pImpl->predictionWeights.size() * sizeof(float))) {
            NLM_LOG_ERROR("Failed to read prediction weights from model file");
            return false;
        }
        
        if (!file.read(reinterpret_cast<char*>(&pImpl->learningRate), sizeof(float))) {
            NLM_LOG_ERROR("Failed to read learning rate from model file");
            return false;
        }
        
        if (!file.read(reinterpret_cast<char*>(&pImpl->confidenceThreshold), sizeof(float))) {
            NLM_LOG_ERROR("Failed to read confidence threshold from model file");
            return false;
        }
        
        if (!file.read(reinterpret_cast<char*>(&pImpl->errorDecayRate), sizeof(float))) {
            NLM_LOG_ERROR("Failed to read error decay rate from model file");
            return false;
        }
        
        if (!file.read(reinterpret_cast<char*>(&pImpl->predictionAccuracy), sizeof(float))) {
            NLM_LOG_ERROR("Failed to read accuracy from model file");
            return false;
        }
        
        if (!file.read(reinterpret_cast<char*>(&pImpl->predictionPrecision), sizeof(float))) {
            NLM_LOG_ERROR("Failed to read precision from model file");
            return false;
        }
        
        if (!file.read(reinterpret_cast<char*>(&pImpl->predictionRecall), sizeof(float))) {
            NLM_LOG_ERROR("Failed to read recall from model file");
            return false;
        }
        
        NLM_LOG_INFO("Prediction model imported successfully");
        NLM_LOG_INFO("  Feature size: " + std::to_string(featureSize));
        NLM_LOG_INFO("  Learning rate: " + std::to_string(pImpl->learningRate));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception importing model: ") + e.what());
        return false;
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->enabled) {
        return currentState.clone();
    }
    
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Prediction system not integrated with brain - returning copy of current state");
        return currentState.clone();
    }
    
    // Extract sensory features
    const auto& currentData = currentState.getData();
    size_t featureSize = std::sqrt(pImpl->predictionWeights.size());
    
    if (currentData.size() < featureSize) {
        NLM_LOG_WARNING("Current sensory data size smaller than expected prediction features");
        return currentState.clone();
    }
    
    // Compute predicted features using weighted sum
    std::vector<float> predictedFeatures(featureSize, 0.0f);
    
    for (size_t i = 0; i < featureSize; ++i) {
        for (size_t j = 0; j < featureSize; ++j) {
            size_t idx = i * featureSize + j;
            if (idx < pImpl->predictionWeights.size()) {
                predictedFeatures[i] += pImpl->predictionWeights[idx] * currentData[j];
            }
        }
        
        // Add temporal dynamics
        if (!pImpl->temporalWeights.empty()) {
            predictedFeatures[i] += pImpl->temporalWeights[i];
        }
    }
    
    // Apply sigmoid activation for prediction output
    for (size_t i = 0; i < featureSize; ++i) {
        predictedFeatures[i] = 1.0f / (1.0f + std::exp(-predictedFeatures[i]));
    }
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedFeatures);
    
    // Update confidence based on prediction stability
    float maxFeature = *std::max_element(predictedFeatures.begin(), predictedFeatures.end());
    float minFeature = *std::min_element(predictedFeatures.begin(), predictedFeatures.end());
    pImpl->currentConfidence = maxFeature - minFeature;  // Confidence as feature range
    
    // Update prediction error
    float predictionError = 0.0f;
    for (size_t i = 0; i < featureSize; ++i) {
        float diff = predictedFeatures[i] - currentData[i % currentData.size()];
        predictionError += diff * diff;
    }
    pImpl->currentPredictionError = predictionError / featureSize;
    pImpl->errorHistory.push_back(pImpl->currentPredictionError);
    
    // Maintain error history size
    if (pImpl->errorHistory.size() > pImpl->featureHistoryLength) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    NLM_LOG_DEBUG("Prediction computed - error: " + std::to_string(pImpl->currentPredictionError) + 
                 ", confidence: " + std::to_string(pImpl->currentConfidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->enabled) return;
    
    // Calculate prediction error
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.size() != actualData.size() || predictedData.empty()) {
        return;
    }
    
    float error = 0.0f;
    float correctPredictions = 0;
    float totalPredictions = 0;
    
    for (size_t i = 0; i < predictedData.size(); ++i) {
        float diff = predictedData[i] - actualData[i];
        error += diff * diff;
        
        // Simple accuracy calculation (threshold at 0.5)
        float predictedVal = predictedData[i] > 0.5f ? 1.0f : 0.0f;
        float actualVal = actualData[i] > 0.5f ? 1.0f : 0.0f;
        
        if (predictedVal == actualVal) {
            correctPredictions++;
        }
        totalPredictions++;
    }
    
    pImpl->currentPredictionError = error / predictedData.size();
    pImpl->errorHistory.push_back(pImpl->currentPredictionError);
    
    // Update accuracy metrics
    if (totalPredictions > 0) {
        pImpl->predictionAccuracy = correctPredictions / totalPredictions;
        
        // Precision and recall (simplified - assuming predicted positive = 1, actual positive = 1)
        pImpl->predictionPrecision = pImpl->predictionAccuracy;  // Simplified
        pImpl->predictionRecall = pImpl->predictionAccuracy;    // Simplified
    }
    
    // Decay old errors
    for (size_t i = 0; i < pImpl->errorHistory.size(); ++i) {
        pImpl->errorHistory[i] *= pImpl->errorDecayRate;
    }
    
    // Update confidence based on error
    pImpl->currentConfidence = 1.0f - std::min(1.0f, pImpl->currentPredictionError);
    pImpl->confidenceHistory.push_back(pImpl->currentConfidence);
    
    // Maintain history sizes
    if (pImpl->errorHistory.size() > pImpl->featureHistoryLength) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    if (pImpl->confidenceHistory.size() > pImpl->featureHistoryLength) {
        pImpl->confidenceHistory.erase(pImpl->confidenceHistory.begin());
    }
    
    NLM_LOG_DEBUG("Predictions updated - error: " + std::to_string(pImpl->currentPredictionError) + 
                 ", accuracy: " + std::to_string(pImpl->predictionAccuracy));
}

float PredictionSystem::getPredictionError() const {
    return pImpl->currentPredictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->currentConfidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->confidenceHistory.clear();
    pImpl->currentPredictionError = 0.0f;
    pImpl->currentConfidence = 0.5f;
    NLM_LOG_INFO("Prediction system history cleared");
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->enabled || pImpl->brain == nullptr) return;
    
    // Extract features from observation
    const auto& data = observation.getData();
    size_t featureSize = std::sqrt(pImpl->predictionWeights.size());
    
    if (data.size() < featureSize) return;
    
    // Simple Hebbian learning for prediction weights
    for (size_t i = 0; i < featureSize; ++i) {
        for (size_t j = 0; j < featureSize; ++j) {
            size_t idx = i * featureSize + j;
            if (idx < pImpl->predictionWeights.size()) {
                // Hebbian update: weight += learning_rate * input * target
                float input = data[j];
                float target = (i < data.size()) ? data[i] : 0.5f;
                pImpl->predictionWeights[idx] += pImpl->learningRate * input * (target - predictedFeatures[i]);
            }
        }
    }
    
    NLM_LOG_DEBUG("Prediction system trained with new observation");
}

} // namespace nlm
