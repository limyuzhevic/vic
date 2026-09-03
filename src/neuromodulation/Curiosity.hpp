#pragma once

#include "../core/Types/Types.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// PLACEHOLDER - Phase 2 will implement real curiosity computation

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive
    float getExplorationDrive() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
