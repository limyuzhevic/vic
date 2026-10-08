#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced with comprehensive integration capabilities

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Enhanced neuromodulation effects
    
    // Get modulation factor for memory consolidation
    virtual float getMemoryConsolidationFactor() const { return 1.0f; }
    
    // Get modulation factor for attention
    virtual float getAttentionModulationFactor() const { return 1.0f; }
    
    // Get modulation factor for behavioral inhibition
    virtual float getBehavioralInhibitionFactor() const { return 1.0f; }
    
    // Get modulation factor for arousal
    virtual float getArousalModulationFactor() const { return 1.0f; }
    
    // Get modulation factor for mood regulation
    virtual float getMoodModulationFactor() const { return 1.0f; }
    
    // Get modulation factor for signal detection
    virtual float getSignalDetectionFactor() const { return 1.0f; }
    
    // Get modulation factor for exploration vs exploitation
    virtual float getExplorationModulationFactor() const { return 1.0f; }
    
    // Get modulation factor for reward prediction
    virtual float getRewardPredictionFactor() const { return 1.0f; }
    
    // Check if neuromodulator is active
    virtual bool isActive() const { return getLevel() > 0.001f; }
    
    // Get modulation strength (combines all factors)
    virtual float getTotalModulationStrength() const {
        float strength = getPlasticityFactor();
        strength *= getMemoryConsolidationFactor();
        strength *= getAttentionModulationFactor();
        strength *= getBehavioralInhibitionFactor();
        strength *= getArousalModulationFactor();
        strength *= getMoodModulationFactor();
        strength *= getSignalDetectionFactor();
        strength *= getExplorationModulationFactor();
        strength *= getRewardPredictionFactor();
        return strength;
    }
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for enhanced neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
