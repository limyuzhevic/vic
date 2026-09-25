#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 6 implementation - complete neuromodulation integration

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Phase 6: Implemented real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Rest-related functions
    virtual void signalRest(float energySaved) {}
    virtual void signalWakeUp() {}
    virtual void signalMetabolicDemand(float demand) {}
    virtual bool shouldTriggerRest() const { return false; }
    virtual bool shouldEndRest() const { return false; }
    virtual float getRestDrive() const { return 0.0f; }
    
    // Additional acetylcholine-specific accessors
    virtual void setAttentionGain(float gain) {}
    virtual float getAttentionGain() const { return 1.0f; }
    virtual void setMemoryConsolidationBoost(float boost) {}
    virtual float getMemoryConsolidationBoost() const { return 1.0f; }
    
    // Additional norepinephrine-specific accessors
    virtual void setArousalLevel(float arousal) {}
    virtual float getArousalLevel() const { return 0.5f; }
    virtual void setVigilanceBias(float bias) {}
    virtual float getVigilanceBias() const { return 1.0f; }
    
    // Additional serotonin-specific accessors
    virtual void setMoodLevel(float mood) {}
    virtual float getMoodLevel() const { return 0.5f; }
    virtual void setImpulsivityLevel(float impulsivity) {}
    virtual float getImpulsivityLevel() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Adenosine: Sleep pressure and fatigue signaling
// Implements the homeostatic sleep drive
class Adenosine;

// Dopamine: Reward and reinforcement learning signal
// Phase 6: Complete reward prediction error system
class Dopamine;

// Acetylcholine: Attention and memory consolidation
// Phase 6: Complete attention and memory integration
class Acetylcholine;

// Norepinephrine: Arousal and vigilance
// Phase 6: Complete arousal and vigilance system
class Norepinephrine;

// Serotonin: Mood, impulsivity, and social behavior
// Phase 6: Complete social and emotional regulation
class Serotonin;

} // namespace nlm
