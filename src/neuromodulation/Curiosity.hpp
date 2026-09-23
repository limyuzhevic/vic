#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error

class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Neuromodulator interface
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get curiosity level
    float getExplorationDrive() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
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
