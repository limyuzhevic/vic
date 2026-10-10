#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

class Brain;

// Curiosity: exploration drive based on novelty and prediction error
// Combines computational curiosity with enhanced exploration behavior
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Get curiosity level (alias for getLevel())
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get exploration threshold
    float getExplorationThreshold() const;
    
    // Get plasticity enhancement factor
    float getPlasticityEnhancement() const;
    
    // Get attention modulation factor
    float getAttentionModulation() const;
    
    // Get action selection bias
    float getActionSelectionBias() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Configure curiosity system parameters
    void configure(float noveltyWeight, float predictionErrorWeight, float decayRate);
    
    // Apply enhanced exploration effects
    void applyExplorationEffects();
    
    // Enhance plasticity during exploration
    void enhancePlasticityDuringExploration();
    
    // Set exploration threshold
    void setExplorationThreshold(float threshold);
    
    // Set plasticity enhancement factor
    void setPlasticityEnhancement(float enhancement);
    
    // Set attention modulation factor
    void setAttentionModulation(float modulation);
    
    // Set action selection bias
    void setActionSelectionBias(float bias);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm