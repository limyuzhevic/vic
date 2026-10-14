#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

/**
 * @class Curiosity
 * @brief Neuromodulator for exploration motivation based on novelty and prediction error
 * 
 * Curiosity represents the drive to explore and learn, computed as a weighted
 * combination of detected novelty and prediction error signals. Higher curiosity
 * levels increase exploratory behavior and plasticity.
 * 
 * @details Uses a BaseNeuromodulator-style implementation with common functionality
 * for level management and decay dynamics.
 */
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const override;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
