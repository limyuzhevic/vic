#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// Using neural mechanisms for predictive coding

class NeuralPrediction;

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update prediction system with current brain state
    void update(TimestepDuration dt);
    
    // Make prediction for next timestep based on neural state
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
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get current prediction from neural predictor
    std::vector<float> getCurrentPrediction() const;
    
    // Get prediction error signal for neuromodulation
    float getPredictionErrorSignal() const;
    
    // Record sensory state for prediction learning
    void recordSensoryState(const std::vector<float>& sensoryState);
    
    // Generate prediction for action consequences
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);
    
    // Update prediction with actual observation
    float updateWithObservation(const std::vector<float>& actualState);
    
    // Get neural prediction component
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<NeuralPrediction> neuralPrediction_;
};

} // namespace nlm
