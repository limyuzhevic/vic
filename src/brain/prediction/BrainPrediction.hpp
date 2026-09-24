#pragma once

#include "BrainPrediction.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {
    class Brain;
    
    // Prediction system for sensory prediction and error computation
    class Prediction {
    public:
        Prediction();
        ~Prediction();
        
        // Initialize prediction system
        void initializePrediction(Brain* brain);
        
        // Update prediction based on sensory input
        void updatePrediction(const class SensoryInput& input);
        
        // Get prediction error signal
        PredictionError* getPredictionErrorSignal();
        
        // Compute prediction error
        void computePredictionError(const class SensoryInput& input);
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}
