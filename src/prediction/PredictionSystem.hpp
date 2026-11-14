#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// PredictionErrorType enumeration for detailed error tracking
enum class PredictionErrorType {
    IntensityError,
    SpatialError,
    TemporalError,
    TotalError,
    Count
};

// Prediction system: predicts future sensory states and consequences
// REAL IMPLEMENTATION: Neural predictive coding with temporal and action-consequence prediction

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Make prediction for next timestep
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getConfidence() const;
    
    // Get detailed error components
    float getPredictionErrorComponent(PredictionErrorType type) const;
    std::vector<float> getPredictionErrorComponents() const;
    
    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model
    void train(const SensoryInput& observation);
    
    // Get action prediction bias
    std::vector<float> getPredictionBias() const;
    
    // Integrate prediction system with brain operations
    void integrateWithBrain(class Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
