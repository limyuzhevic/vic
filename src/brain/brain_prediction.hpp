#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Prediction system integration
class BrainPrediction {
public:
    BrainPrediction() = default;
    ~BrainPrediction() = default;
    
    // Initialize prediction system
    void initialize(std::shared_ptr<class PredictionSystem> predictionSystem);
    
    // Prediction system updates (called from main step)
    void update(std::shared_ptr<PredictionSystem> predictionSystem,
               const class SensoryInput& sensoryInput);
    
    // Get prediction system accessor
    PredictionSystem* getPredictionSystem() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
