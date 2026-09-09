#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computes weighted combination of novelty and prediction error with adaptation

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level (0.0 to 1.0)
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset to initial state
    void reset();
    
    // Get prediction error from brain
    float getPredictionError() const;
    
    // Get novelty from brain
    float getNovelty() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
