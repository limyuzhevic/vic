#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Defines the interface for all neuromodulators

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
    virtual void update(TimestepDuration dt, const std::vector<NeuronId>& neurons = {}, SimulationStep currentStep = 0) = 0;
    
    // Initialize with brain reference (optional)
    virtual void initialize(class Brain* brain) {}
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for concrete implementations
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

} // namespace nlm
