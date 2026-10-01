#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declaration
class Brain;

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error
// Implements curiosity as a homeostatic system that increases exploration

class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    // Initialize with brain reference
    void initialize(Brain* brain) override;
    
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
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
