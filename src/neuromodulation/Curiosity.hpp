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
    float getExplorationDrive() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Update curiosity based on novelty alone
    void updateNovelty(float novelty);
    
    // Get/set curiosity weights
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset curiosity
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
