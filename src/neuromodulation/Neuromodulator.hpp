#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

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
    
    // Apply neuromodulatory effect to neuron
    virtual void applyToNeuron(class Neuron* neuron, TimestepDuration dt) = 0;
    
    // Apply neuromodulatory effect to synapse
    virtual void applyToSynapse(class Synapse* synapse, TimestepDuration dt) = 0;
    
    // Reset neuromodulator state
    virtual void reset() = 0;
    
    // Get baseline level
    virtual float getBaseline() const = 0;
    
    // Set baseline level
    virtual void setBaseline(float baseline) = 0;
    
    // Check if neuromodulator is active (level > baseline)
    virtual bool isActive() const = 0;
    
protected:
    Neuromodulator() = default;
};

} // namespace nlm