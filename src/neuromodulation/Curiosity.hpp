#pragma once

#include "../core/Types/Types.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include <memory>

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computes weighted combination of novelty and prediction error with biological realism

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference and neuromodulation systems
    void initialize(class Brain* brain, std::shared_ptr<Novelty> noveltySystem,
                    std::shared_ptr<PredictionError> predictionErrorSystem);
    
    // Get curiosity level
    float getLevel() const;
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Update curiosity based on novelty and prediction error
    // Implements biologically realistic curiosity mechanisms
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Set curiosity parameters with biological ranges
    void setNoveltyWeight(float weight);              // Weight for novelty contribution (0-1)
    void setPredictionErrorWeight(float weight);      // Weight for prediction error contribution (0-1)
    void setExplorationThreshold(float threshold);    // Threshold for exploration activation
    void setAttentionWindow(float window);            // Time window for novelty detection
    void setLearningRate(float rate);                 // Learning rate for curiosity update
    void setBaselineCuriosity(float baseline);        // Baseline curiosity level
    
    // Get curiosity components
    float getNoveltyComponent() const;                // Novelty-based curiosity
    float getPredictionErrorComponent() const;        // Prediction error-based curiosity
    float getSurpriseComponent() const;               // Surprise-based curiosity
    
    // Reset and initialize
    void reset();
    void initializeWithDefaults();
    
    // Get curiosity state for analysis
    bool isExplorationActive() const;                // Whether curiosity-driven exploration is active
    float getTimeSinceLastExploration() const;        // Time since last exploration event
    float getCuriosityStrength() const;               // Overall curiosity strength
    
    // Get novelty system integration
    bool hasNoveltySystem() const;                    // Whether novelty system is integrated
    bool hasPredictionErrorSystem() const;            // Whether prediction error system is integrated
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
