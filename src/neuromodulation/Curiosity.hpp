#pragma once

#include "../core/Types/Types.hpp"
#include "Brain.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error
// Integrates with plasticity rules to balance exploration vs exploitation

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference for integration with neural networks
    void initialize(class Brain* brain);
    
    // Get curiosity level (exploration motivation)
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    // Directly integrates with plasticity mechanisms
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setExplorationDecayRate(float rate);
    void setPlasticityCoupling(float coupling);
    
    // Reset to initial state
    void reset();
    
    // Get neuromodulatory effect on plasticity
    float getPlasticityModulator() const;
    
    // Spike-based curiosity signaling
    void signalNoveltySpike();
    void signalPredictionErrorSpike();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
