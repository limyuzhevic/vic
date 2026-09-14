#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <algorithm>

namespace nlm {

// Neuromodulator type enumeration
enum class NeuromodulatorType {
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    Acetylcholine,
    Norepinephrine,
    Serotonin
};

// Base abstract class for neuromodulators
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
    
    // Initialize with brain reference
    virtual void initialize(class Brain* brain) = 0;
    
    // Get type identifier
    virtual NeuromodulatorType getType() const = 0;
    
    // Load configuration parameters
    virtual void loadConfig(const std::string& key, float value) = 0;
    
protected:
    Neuromodulator() = default;
    Neuromodulator(const Neuromodulator&) = default;
    Neuromodulator& operator=(const Neuromodulator&) = default;
};

// Common base for all neuromodulators
class NeuromodulatorBase : public Neuromodulator {
protected:
    NeuromodulatorBase() = default;
    ~NeuromodulatorBase() override = default;
    
    // Common state
    float currentLevel = 0.0f;
    TimestepDuration lastUpdateTime = 0;
    float strength = 1.0f;
    
    // Common helper methods
    float computeDecay(float level, float decayRate, TimestepDuration dt) const {
        return std::max(0.0f, level - decayRate * static_cast<float>(dt));
    }
    
    float computeScale(float value, float min, float max) const {
        return std::clamp((value - min) / (max - min), 0.0f, 1.0f);
    }
    
    void applySmoothing(float& current, float target, float smoothingFactor) {
        current += (target - current) * smoothingFactor;
    }
};

} // namespace nlm
