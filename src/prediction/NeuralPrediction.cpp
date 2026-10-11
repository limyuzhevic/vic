#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Learning buffers for temporal patterns
    std::deque<std::pair<std::vector<float>, SimulationStep>> sequenceBuffer;
    
    // Pattern learning weights
    std::vector<float> patternWeights;
    
    // Error tracking
    float predictionError_;
    float predictionConfidence_;
    
    // For generating temporal patterns
    std::mt19937 rng;
    
    Impl() : brain(nullptr), predictionError_(1.0f), predictionConfidence_(0.5f) {
        rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

NeuralPrediction::NeuralPrediction() : pImpl(new Impl) {}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->sequenceBuffer.clear();
    pImpl->patternWeights.clear();
    
    if (brain) {
        // Get number of neurons for pattern size
        size_t numNeurons = brain->getTotalNeuronCount();
        pImpl->patternWeights.resize(numNeurons, 0.0f);
        
        NLM_LOG_INFO("NeuralPrediction initialized with brain");
    }
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep) {
    if (!brain_ || sensoryState.empty()) return;
    
    // Limit buffer size to prevent memory issues
    if (pImpl->sequenceBuffer.size() >= 1000) {
        pImpl->sequenceBuffer.pop_front();
    }
    
    // Record the sensory state with its timestamp
    pImpl->sequenceBuffer.push_back(std::make_pair(sensoryState, currentStep));
    
    // Update pattern weights through learning
    updatePatternWeights(sensoryState, currentStep);
    
    NLM_LOG_INFO("NeuralPrediction recorded sensory state at step " + std::to_string(currentStep));
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    std::vector<float> prediction;
    
    if (pImpl->sequenceBuffer.empty() || !brain_) {
        // Return random prediction if no data available
        prediction.resize(10, 0.5f); // Default sensory size
        for (float& val : prediction) {
            val = std::uniform_real_distribution<float>(0.0f, 1.0f)(pImpl->rng);
        }
        return prediction;
    }
    
    // Get recent sequence history
    size_t historySize = std::min(size_t(10), pImpl->sequenceBuffer.size());
    
    if (historySize < 2) {
        // Use current state if we don't have enough history
        prediction = pImpl->sequenceBuffer.back().first;
        return prediction;
    }
    
    // Use learned patterns to generate prediction
    // This is where the neural prediction logic would apply
    prediction = predictFromLearnedPatterns(currentStep, historySize);
    
    return prediction;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState, 
                                            SimulationStep currentStep) {
    if (pImpl->sequenceBuffer.empty()) return 1.0f;
    
    // Get the most recent predicted state
    const auto& lastRecorded = pImpl->sequenceBuffer.back();
    std::vector<float> predictedState = generatePrediction(currentStep);
    
    // Calculate prediction error
    float error = calculatePredictionError(predictedState, actualState);
    
    pImpl->predictionError_ = error;
    
    // Update confidence based on error
    updatePredictionConfidence(error);
    
    // Learn from the actual observation
    recordSensoryState(actualState, currentStep);
    
    NLM_LOG_INFO("NeuralPrediction updated: error = " + std::to_string(error));
    
    return error;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                             const std::vector<float>& currentState) {
    std::vector<float> prediction;
    
    // Simple action consequence prediction
    // In a real implementation, this would use learned action-effect associations
    prediction = currentState;  // Start with current state
    
    // Modify based on action type
    switch (action) {
        case ActionType::MoveForward:
            // Predict movement effect
            for (float& val : prediction) {
                val += 0.1f; // Movement increases sensory input slightly
                val = std::min(1.0f, val);
            }
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Predict turning effect
            for (float& val : prediction) {
                val += 0.05f;
                val = std::min(1.0f, val);
            }
            break;
            
        case ActionType::Wait:
            // No change
            break;
            
        case ActionType::Attack:
            // Strong effect
            for (float& val : prediction) {
                val += 0.2f;
                val = std::min(1.0f, val);
            }
            break;
    }
    
    return prediction;
}

float NeuralPrediction::getPredictionConfidence() const {
    return pImpl->predictionConfidence_;
}

void NeuralPrediction::updatePatternWeights(const std::vector<float>& sensoryState, SimulationStep currentStep) {
    if (sensoryState.empty()) return;
    
    // Simple Hebbian-like learning for pattern weights
    // This simulates synaptic strengthening through experience
    for (size_t i = 0; i < std::min(sensoryState.size(), pImpl->patternWeights.size()); ++i) {
        pImpl->patternWeights[i] = pImpl->patternWeights[i] * 0.9f + sensoryState[i] * 0.1f;
    }
}

std::vector<float> NeuralPrediction::predictFromLearnedPatterns(SimulationStep currentStep, size_t historySize) {
    std::vector<float> prediction;
    
    // Get recent sequence history
    size_t startIndex = pImpl->sequenceBuffer.size() - historySize;
    
    if (startIndex >= pImpl->sequenceBuffer.size()) {
        return pImpl->sequenceBuffer.back().first;
    }
    
    // Average the recent states for prediction
    // This simulates temporal integration
    std::vector<float> sum(pImpl->patternWeights.size(), 0.0f);
    size_t validCount = 0;
    
    for (size_t i = startIndex; i < pImpl->sequenceBuffer.size(); ++i) {
        const auto& state = pImpl->sequenceBuffer[i].first;
        if (state.size() == sum.size()) {
            for (size_t j = 0; j < state.size(); ++j) {
                sum[j] += state[j];
            }
            validCount++;
        }
    }
    
    if (validCount > 0) {
        prediction.resize(sum.size());
        for (size_t i = 0; i < sum.size(); ++i) {
            prediction[i] = sum[i] / validCount;
            
            // Add stochasticity for exploration
            std::uniform_real_distribution<float> noise(-0.05f, 0.05f);
            prediction[i] += noise(pImpl->rng);
            prediction[i] = std::max(0.0f, std::min(1.0f, prediction[i]));
        }
    } else {
        // Fallback to most recent state
        prediction = pImpl->sequenceBuffer.back().first;
    }
    
    return prediction;
}

float NeuralPrediction::calculatePredictionError(const std::vector<float>& predicted, 
                                                const std::vector<float>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f; // Maximum error
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    return std::sqrt(sumSquaredError / predicted.size());
}

void NeuralPrediction::updatePredictionConfidence(float error) {
    // Confidence decreases with higher prediction error
    if (error < 0.1f) {
        pImpl->predictionConfidence_ = std::min(1.0f, pImpl->predictionConfidence_ + 0.1f);
    } else if (error > 0.5f) {
        pImpl->predictionConfidence_ = std::max(0.1f, pImpl->predictionConfidence_ - 0.1f);
    } else {
        // Moderate error - confidence stays around baseline
        pImpl->predictionConfidence_ = 0.5f;
    }
}

} // namespace nlm
