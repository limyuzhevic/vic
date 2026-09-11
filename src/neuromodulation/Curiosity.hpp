#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error
// Phase 2 COMPLETE - Enhanced implementation

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();
    
    // Additional methods for enhanced functionality
    void setDecayRate(float rate);
    void setMaxLevel(float maxLevel);
    float getDecayRate() const;
    float getMaxLevel() const;
    
    // Get exploration statistics
    float getAverageExploration() const;
    float getPeakExploration() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
