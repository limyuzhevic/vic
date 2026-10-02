#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Implements the curiosity hypothesis: exploration is driven by prediction error
// and unexpected sensory input
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on prediction error and novelty
    void update(float predictionError, float novelty, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setPredictionErrorWeight(float weight);
    void setNoveltyWeight(float weight);
    void setIntegrationRate(float rate);
    void setDecayRate(float rate);
    
    // Get current parameters
    float getPredictionErrorWeight() const { return predictionErrorWeight_; }
    float getNoveltyWeight() const { return noveltyWeight_; }
    float getIntegrationRate() const { return integrationRate_; }
    float getDecayRate() const { return decayRate_; }
    
    // Reset curiosity to baseline
    void reset();
    
    // Check if curiosity is driving exploration
    bool isDrivingExploration() const;
    
    // Get curiosity statistics
    float getMeanCuriosity() const;
    float getMaxCuriosity() const;
    float getMinCuriosity() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Current parameters (stored in Impl for easy access)
    float predictionErrorWeight_;
    float noveltyWeight_;
    float integrationRate_;
    float decayRate_;
};

} // namespace nlm
