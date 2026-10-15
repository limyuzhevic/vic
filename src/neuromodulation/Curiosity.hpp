#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error
// Now properly integrates with Neuromodulator base class

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
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Get curiosity parameters
    float getNoveltyWeight() const;
    float getPredictionErrorWeight() const;
    
    // Reset to baseline
    void reset();
    
    // Update based on external inputs (novelty and prediction error)
    void updateFromInputs(float novelty, float predictionError, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
