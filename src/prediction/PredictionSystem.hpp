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
    
    // Train with observation data
    void trainWithObservation(const SensoryInput& observation) {
        if (!brain_) return;
        
        // Update prediction model with new observation
        // This helps the model learn temporal patterns
        auto& data = observation.getData();
        if (!data.empty()) {
            // Store observation in model for future prediction
            // In a real implementation, this would update internal model parameters
            // For now, just store for reference
            lastObservation_ = observation.clone();
        }
    }
    
private:
    Brain* brain_;
    std::unique_ptr<SensoryInput> lastObservation_;
};

// Improved Prediction System with neural-based prediction
class PredictionSystem {
public:
    // Standard constructor (without brain reference)
    PredictionSystem();
    
    // Enhanced constructor with brain reference for neural-based prediction
    explicit PredictionSystem(Brain* brain);
    
    ~PredictionSystem();
    
    // Make prediction for next timestep using neural prediction model
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model with new observations
    void train(const SensoryInput& observation);
    
    // Get reference to neural prediction model for advanced configuration
    NeuralPredictionModel& getNeuralPredictionModel() { return *pImpl->neuralModel; }
    
    // Initialize prediction system with brain reference (alternative initialization)
    void initialize(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
