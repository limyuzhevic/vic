#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Prediction System Integration
// Handles prediction system integration for sensory prediction and error computation

class BrainPrediction {
public:
    // Initialize prediction system
    void initializePredictionSystem(Brain::Impl& pImpl);
    
    // Update prediction system with current state
    void updatePredictionSystem(Brain::Impl& pImpl);
    
    // Get prediction system statistics
    void logPredictionStats(const Brain::Impl& pImpl) const;
    
private:
    // Helper functions for prediction integration
    void integratePredictionWithMemory(Brain::Impl& pImpl);
    void updatePredictionError(Brain::Impl& pImpl);
};

} // namespace nlm
