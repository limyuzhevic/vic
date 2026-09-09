#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Get exploration drive
    float getExplorationDrive() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setDecayRate(float rate);
    void setTimeConstant(float tc);
    void setIntegrationFactor(float factor);
    void setLearningRate(float lr);
    
    // Compute learning progress from prediction errors
    float computeLearningProgress(float predictionError) const;
    
    // Compute information gain for exploration
    float computeInformationGain(float novelty, float predictionError) const;
    
    // Get exploration value
    float getExplorationValue() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
