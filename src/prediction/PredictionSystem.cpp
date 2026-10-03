#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Neural prediction model
    struct NeuralPattern {
        std::vector<float> features;
        Timestamp timestamp;
        float predictionConfidence;
    };
    
    std::vector<NeuralPattern> patternHistory;
    
    // Temporal prediction model (simple AR process)
    struct TemporalModel {
        std::vector<float> coefficients;
        std::vector<float> recentValues;
        size_t maxHistory;
        
        TemporalModel(size_t historySize = 100) : maxHistory(historySize) {
            coefficients.resize(historySize, 0.1f);
            recentValues.reserve(historySize);
        }
        
        float predictNext(const std::vector<float>& currentPattern) {
            if (currentPattern.empty() || coefficients.empty()) return 0.0f;
            
            size_t minSize = std::min(recentValues.size(), coefficients.size());
            if (minSize == 0) return 0.0f;
            
            // Simple autoregressive prediction
            float prediction = 0.0f;
            for (size_t i = 0; i < minSize && i < currentPattern.size(); ++i) {
                prediction += currentPattern[i] * coefficients[i];
            }
            
            // Normalize prediction
            float maxCoeff = *std::max_element(coefficients.begin(), coefficients.end());
            if (maxCoeff > 0.0f) {
                prediction /= maxCoeff;
            }
            
            return prediction;
        }
        
        void update(const std::vector<float>& newValues) {
            recentValues.push_back(newValues.back());
            if (recentValues.size() > maxHistory) {
                recentValues.erase(recentValues.begin());
            }
        }
    };
    
    TemporalModel temporalModel;
    
    // Prediction error tracking
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Prediction learning parameters
    float learningRate;
    float errorDecayRate;
    float confidenceThreshold;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f),
             learningRate(0.01f), errorDecayRate(0.95f), confidenceThreshold(0.7f) {
        patternHistory.reserve(1000);
        errorHistory.reserve(500);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brainPtr) {
    pImpl->brain = brainPtr;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real neural prediction based on current brain activity
    if (!pImpl->brain) return currentState.clone();
    
    // Extract current neural activity pattern for prediction
    std::vector<float> neuralPattern;
    neuralPattern.reserve(1000);
    
    // Sample neural activity from different brain regions
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto* neuron : population->getNeurons()) {
                if (neuron) {
                    const auto& state = neuron->getState();
                    neuralPattern.push_back(
                        std::abs(state.membranePotential - state.restingPotential) / 20.0f
                    );
                }
            }
        }
    }
    
    if (neuralPattern.empty()) {
        return currentState.clone();
    }
    
    // Update temporal model
    pImpl->temporalModel.update(neuralPattern);
    
    // Make prediction
    float predictedValue = pImpl->temporalModel.predictNext(neuralPattern);
    
    // Create sensory input representing the predicted state
    // Normalize prediction to match sensory input range
    auto predictedInput = std::make_unique<SensoryInput>();
    
    // Create predicted sensory data based on neural pattern
    // This simulates what the sensory input would look like if prediction is correct
    std::vector<float> predictedData;
    predictedData.reserve(neuralPattern.size() * 2);  // 2D representation
    
    // Generate 2D feature representation from neural pattern
    size_t halfSize = neuralPattern.size() / 2;
    for (size_t i = 0; i < halfSize && i < neuralPattern.size(); ++i) {
        predictedData.push_back(neuralPattern[i]);
    }
    
    // Add predicted temporal component
    predictedData.push_back(predictedValue);
    
    // Add some noise based on prediction uncertainty
    RandomGenerator rng(42); // Deterministic for reproducibility
    for (float& value : predictedData) {
        value += rng.uniformReal(-0.1f, 0.1f) * (1.0f - pImpl->confidence);
    }
    
    predictedInput->setData(predictedData);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate mean squared error
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.1f, 1.0f - pImpl->predictionError);
        
        // Decay error history
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
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
    // Learn from actual observation to improve future predictions
    if (!pImpl->brain) return;
    
    // Convert sensory observation to neural pattern
    const auto& obsData = observation.getData();
    std::vector<float> neuralPattern = obsData;
    
    // Update temporal model with new pattern
    pImpl->temporalModel.update(neuralPattern);
    
    // Learn better coefficients for temporal prediction
    // Simple gradient descent on prediction error
    float prediction = pImpl->temporalModel.predictNext(neuralPattern);
    
    // Calculate learning signal based on prediction error
    float learningSignal = std::max(0.0f, 1.0f - pImpl->predictionError) - 0.5f;
    
    // Update temporal model coefficients
    for (size_t i = 0; i < pImpl->temporalModel.coefficients.size() && i < neuralPattern.size(); ++i) {
        // Simple Hebbian-like learning for prediction coefficients
        float delta = learningSignal * neuralPattern[i] * pImpl->learningRate;
        pImpl->temporalModel.coefficients[i] += delta;
        
        // Ensure coefficients stay in valid range
        pImpl->temporalModel.coefficients[i] = 
            std::max(0.0f, std::min(1.0f, pImpl->temporalModel.coefficients[i]));
    }
}

void PredictionSystem::update(Timestamp currentTime) {
    // Update prediction confidence based on neural activity stability
    if (pImpl->brain) {
        pImpl->confidence = getNeuralConfidence();
    }
    
    // Decay prediction error over time if no new errors
    pImpl->predictionError *= pImpl->errorDecayRate;
    
    // Update temporal model statistics
    pImpl->temporalModel.update(std::vector<float>(1, 0.0f)); // Placeholder
}

float PredictionSystem::getNeuralConfidence() const {
    if (!pImpl->brain || pImpl->brain->getRegions().empty()) {
        return 0.5f;
    }
    
    // Calculate confidence based on neural activity patterns
    // More consistent activity = higher confidence
    float totalActivity = 0.0f;
    size_t activeNeuronCount = 0;
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto* neuron : population->getNeurons()) {
                if (neuron) {
                    const auto& state = neuron->getState();
                    float activity = std::abs(state.membranePotential - state.restingPotential);
                    totalActivity += activity;
                    activeNeuronCount++;
                }
            }
        }
    }
    
    if (activeNeuronCount == 0) return 0.5f;
    
    // Normalize activity and convert to confidence
    float avgActivity = totalActivity / activeNeuronCount;
    float normalizedActivity = std::min(1.0f, avgActivity / 10.0f);  // Scale to 0-1
    
    // Confidence is proportional to activity but with diminishing returns
    float confidence = 0.3f + 0.7f * std::pow(normalizedActivity, 2.0f);
    
    return confidence;
}

void PredictionSystem::reset() {
    pImpl->patternHistory.clear();
    pImpl->temporalModel = Impl::TemporalModel();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
}

} // namespace nlm