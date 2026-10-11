// Improved prediction system implementation with actual neural prediction
// instead of simple state copying

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Neural Prediction Model
// Uses the brain's neural substrate to make predictions
class NeuralPredictionModel {
public:
    NeuralPredictionModel(Brain* brain) : brain_(brain) {
        // Create prediction neurons that mirror sensory processing
        initializePredictionNeurons();
    }
    
    void initializePredictionNeurons() {
        if (!brain_) return;
        
        // Create prediction population if it doesn't exist
        for (auto& region : brain_->getRegions()) {
            // Check if we already have prediction neurons
            bool hasPredictionNeurons = false;
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Prediction) {
                    hasPredictionNeurons = true;
                    break;
                }
            }
            
            if (!hasPredictionNeurons) {
                // Add prediction neurons
                region->addPopulation(20, NeuronType::Prediction);
            }
        }
    }
    
    // Make prediction based on current neural activity
    SensoryInput predictNextState(const SensoryInput& currentState) {
        SensoryInput prediction;
        
        // Get prediction neurons from the first region
        if (brain_ && brain_->getRegionCount() > 0) {
            auto* region = brain_->getRegion(RegionId(1));
            if (region) {
                // Find prediction neurons
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Prediction) {
                        // Generate prediction based on their activity
                        std::vector<float> predictedData;
                        for (auto* neuron : pop->getNeurons()) {
                            // Use membrane potential as prediction signal
                            float pred = std::max(0.0f, 
                                (neuron->getMembranePotential() + 10.0f) / 20.0f);
                            predictedData.push_back(pred);
                        }
                        prediction.setData(predictedData);
                        break;
                    }
                }
            }
        }
        
        // If no prediction neurons, use enhanced version of current state
        if (prediction.getData().empty()) {
            prediction = currentState.clone();
            
            // Apply predictive dynamics
            auto& data = prediction.getData();
            for (size_t i = 0; i < data.size(); ++i) {
                // Add predictive bias based on history
                float historyBias = 0.0f;
                if (i > 0 && i < data.size() - 1) {
                    historyBias = (data[i] - data[i-1]) * 0.5f + (data[i+1] - data[i]) * 0.5f;
                }
                
                // Apply temporal continuity and trend prediction
                data[i] += historyBias * 0.1f + 
                          (std::rand() % 100 - 50) / 500.0f; // Add some stochasticity
                
                // Clamp to valid range
                data[i] = std::max(0.0f, std::min(1.0f, data[i]));
            }
        }
        
        return prediction;
    }
    
    // Update prediction model based on prediction error
    void updatePredictionModel(const SensoryInput& predicted,
                              const SensoryInput& actual,
                              float error) {
        if (!brain_ || error < 0.001f) return;
        
        // Modulate prediction neurons based on prediction error
        float learningRate = error * 0.1f; // Adaptive learning rate based on error
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Prediction) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Update neuron weights based on prediction error
                        float currentWeight = neuron->getMembranePotential();
                        float targetWeight = (currentWeight + error * 0.5f) / 2.0f;
                        
                        // Apply Hebbian-like learning
                        float delta = (targetWeight - currentWeight) * learningRate;
                        neuron->injectCurrent(delta * 10.0f); // Strong update for predictions
                    }
                }
            }
        }
    }
    
private:
    Brain* brain_;
};

// Improved Prediction System with neural-based prediction
class PredictionSystem {
public:
    PredictionSystem(Brain* brain) : brain_(brain), predictionModel_(brain) {}
    
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState) {
        // Use neural prediction model
        SensoryInput prediction = predictionModel_.predictNextState(currentState);
        
        // Apply prediction smoothing and refinement
        refinePrediction(prediction);
        
        // Return as unique pointer
        auto result = std::make_unique<SensoryInput>(prediction);
        return result;
    }
    
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
        // Calculate prediction error
        float error = calculatePredictionError(predicted, actual);
        
        // Update prediction error history
        errorHistory_.push_back(error);
        if (errorHistory_.size() > errorHistory_.max_size()) {
            errorHistory_.pop_front();
        }
        
        // Update prediction model based on error
        predictionModel_.updatePredictionModel(predicted, actual, error);
        
        // Update prediction confidence based on error history
        updateConfidence();
    }
    
    float getPredictionError() const {
        if (errorHistory_.empty()) return 0.0f;
        
        float sumError = 0.0f;
        for (float error : errorHistory_) {
            sumError += error;
        }
        
        return sumError / errorHistory_.size();
    }
    
    float getConfidence() const {
        return confidence_;
    }
    
    const std::deque<float>& getErrorHistory() const {
        return errorHistory_;
    }
    
    void clearHistory() {
        errorHistory_.clear();
        confidence_ = 0.5f;
    }
    
    void train(const SensoryInput& observation) {
        // Use observation to improve prediction model
        // This is where we learn temporal patterns
        lastTrainingObservation_ = observation.clone();
        
        // Train prediction model with new observation
        predictionModel_.trainWithObservation(observation);
    }
    
    // Get the current prediction model for external use
    NeuralPredictionModel& getPredictionModel() {
        return predictionModel_;
    }
    
private:
    void refinePrediction(SensoryInput& prediction) {
        auto& data = prediction.getData();
        if (data.empty()) return;
        
        // Apply temporal smoothing
        std::vector<float> smoothed = data;
        if (data.size() > 2) {
            for (size_t i = 1; i < data.size() - 1; ++i) {
                smoothed[i] = (data[i-1] * 0.3f + data[i] * 0.4f + data[i+1] * 0.3f);
            }
        }
        
        // Apply predictive constraints based on brain state
        float avgActivity = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        
        // Ensure prediction is consistent with overall brain activity level
        for (float& val : smoothed) {
            // Scale prediction based on current activity level
            val = val * avgActivity + (1.0f - avgActivity) * 0.5f;
        }
        
        // Apply bounded changes to prevent wild predictions
        for (size_t i = 0; i < data.size(); ++i) {
            float change = smoothed[i] - data[i];
            // Limit the rate of change
            change = std::max(-0.2f, std::min(0.2f, change));
            data[i] += change;
        }
        
        prediction.setData(data);
    }
    
    float calculatePredictionError(const SensoryInput& predicted, const SensoryInput& actual) {
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() != actualData.size() || predData.empty()) {
            return 1.0f; // Large error if shapes don't match
        }
        
        float sumSquaredError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
        }
        
        return std::sqrt(sumSquaredError / predData.size());
    }
    
    void updateConfidence() {
        float avgError = getPredictionError();
        
        // Confidence is inversely related to error
        if (avgError < 0.1f) {
            confidence_ = std::min(1.0f, confidence_ + 0.05f);
        } else if (avgError > 0.5f) {
            confidence_ = std::max(0.1f, confidence_ - 0.1f);
        } else {
            // Moderate error - confidence stays around baseline
            confidence_ = 0.5f;
        }
    }
    
    Brain* brain_;
    NeuralPredictionModel predictionModel_;
    std::deque<float> errorHistory_;
    float confidence_;
    std::unique_ptr<SensoryInput> lastTrainingObservation_;
};

} // namespace nlm
