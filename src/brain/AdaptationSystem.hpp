#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class AdaptationSystem {
public:
    AdaptationSystem();
    
    // Spike-frequency adaptation management
    void updateAdaptationVariable(float& adaptationVariable, float decayRate = 0.05f) const;
    
    // Apply adaptation to membrane potential
    void applyAdaptationToPotential(MembranePotential& V, float adaptationVariable, float strength = 0.01f) const;
    
    // Reset adaptation variable
    void resetAdaptationVariable(float& adaptationVariable) const;
    
    // Increment adaptation variable after spike
    void incrementAdaptation(float& adaptationVariable, float increment = 1.0f) const;
    
    // Check if adaptation is active
    bool isAdaptationActive(float adaptationVariable) const { return adaptationVariable > 0.0f; }
    
    // Get adaptation contribution factor
    static float getAdaptationStrength(float adaptationVariable) { return adaptationVariable * 0.01f; }
};

} // namespace nlm