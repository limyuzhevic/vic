#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class Config;

// PredictionSystem: Forward model and prediction error computation
// Manages sensory prediction and prediction error signals
class PredictionSystem {
public:
    PredictionSystem(std::shared_ptr<Config> config);
    ~PredictionSystem();
    
    // Initialize prediction system
    bool initialize(Brain* brain);
    
    // Prediction computation
    void update(double timestep);
    
    // Prediction error access
    float getPredictionError() const;
    
    // Prediction signal access
    const std::vector<float>& getPredictionSignal() const { return predictionSignal_; }
    const std::vector<float>& getSensoryPrediction() const { return sensoryPrediction_; }
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Prediction buffers
    std::vector<float> predictionSignal_;
    std::vector<float> sensoryPrediction_;
    std::vector<float> predictionErrorHistory_;
};

} // namespace nlm
