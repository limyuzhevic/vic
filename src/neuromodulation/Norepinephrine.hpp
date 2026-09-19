#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm
{

// Norepinephrine: Arousal and vigilance
// Modulates neuron excitability and firing patterns
// Important for learning under uncertainty

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // NE modulates neuron excitability and firing patterns
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Signal stress or uncertainty event
    void signalStressEvent(float uncertainty, float arousalLevel);
    
    // Modulate neuron excitability
    void modulateExcitability(class Neuron* neuron, float currentLevel);
    
    // Enhance encoding and retrieval of memories under uncertainty
    void enhanceMemoryUnderUncertainty(const std::vector<float>& pattern, float uncertainty);
    
    // Boost vigilance and attention during uncertain states
    void boostVigilance(float vigilanceLevel);
    
    // Reset state
    void reset();
    
    // Get arousal level (derived from NE)
    float getArousalLevel() const;
    
    // Get uncertainty modulation factor
    float getUncertaintyModulationFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm