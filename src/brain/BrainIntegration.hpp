#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "BrainCore.hpp"
#include <string>

namespace nlm {

class Brain;

// Forward declarations for integration systems
class Neuromodulator;
class STDP;
class Hebbian;
class StructuralPlasticity;
class DevelopmentSystem;

// BrainIntegration implementation - Phase 6: Integrated Artificial Brain
// Contains cross-system integration
struct BrainIntegration {
    // ========== NEUROMODULATION ==========
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(Brain* parent, const Neuromodulator& signal);
    
    // ========== PLASTICITY ==========
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity(Brain* parent);
    
    // ========== DEVELOPMENT ==========
    
    // Apply developmental changes (called automatically in step)
    void develop(Brain* parent);
    
    // Helper methods for integration
    void applyNeuromodulationToPlasticity(Brain* parent, const Neuromodulator& signal);
    void updatePlasticityRules(Brain* parent);
    void applyDevelopmentEffects(Brain* parent);
    void updateDevelopmentState(Brain* parent);
    void updatePlasticityRates(Brain* parent);
};

} // namespace nlm