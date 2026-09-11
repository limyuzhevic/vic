#pragma once

// BrainPredictionIntegration.hpp - Prediction system integration
// Contains prediction system integration functionality

#include "BrainCore.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class BrainCore;

// Brain Prediction Integration Implementation
class BrainPredictionIntegration {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain prediction integration with reference to core brain
    explicit BrainPredictionIntegration(BrainCore* core);
    
    ~BrainPredictionIntegration();
    
    // Disable copying, enable moving
    BrainPredictionIntegration(const BrainPredictionIntegration&) = delete;
    BrainPredictionIntegration& operator=(const BrainPredictionIntegration&) = delete;
    BrainPredictionIntegration(BrainPredictionIntegration&&) noexcept;
    BrainPredictionIntegration& operator=(BrainPredictionIntegration&&) noexcept;
    
    // Initialize prediction system
    bool initialize();
    
    // Update prediction system
    void update(SimulationStep currentStep, Timestamp currentTime);
    
    // Process prediction error from sensory input
    void processPredictionError(const std::vector<float>& currentActivity,
                                const std::vector<float>& previousActivity);
    
    // Get prediction system
    PredictionSystem* getPredictionSystem();
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
    // Clear prediction state
    void clear();
    
    // Get statistics
    size_t getPredictionErrorsProcessed() const;
    
private:
    // Internal helper methods
    void updatePredictionSystem(SimulationStep currentStep, Timestamp currentTime);
    void calculatePredictionError(const std::vector<float>& currentActivity,
                                  const std::vector<float>& previousActivity);
    void storeCurrentSensoryActivity(const std::vector<float>& activity);
    void updatePredictionConfidence(float stability);
    
    // Utility methods
    void calculatePredictionStats() const;
    
    struct Impl {
        BrainCore* brainCore;
        
        // Prediction system reference
        PredictionSystem* predictionSystem;
        
        // Prediction state
        std::vector<float> currentSensoryActivity;
        std::vector<float> previousSensoryActivity;
        
        // Statistics
        size_t predictionErrorsProcessed;
        float predictionConfidence;
        
        Impl(BrainCore* core);
    };
};

} // namespace nlm

