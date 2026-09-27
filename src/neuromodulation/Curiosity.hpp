#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Real curiosity system: biologically-inspired exploration algorithm

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
    
    // Set curiosity parameters with biological tuning
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setBaselineCuriosity(float baseline);
    void setLearningRate(float rate);
    
    // Reset curiosity state
    void reset();
    
    // Get curiosity statistics
    float getNoveltyContribution() const;
    float getPredictionErrorContribution() const;
    float getExplorationBonus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
