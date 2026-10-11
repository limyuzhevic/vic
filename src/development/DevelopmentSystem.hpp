#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes
// Simulates the biological process of brain development from embryonic stage to adulthood
// Controls when different plasticity mechanisms are active and at what strength
// Integrates with structural plasticity to drive physical brain changes over time
// 
// Development is divided into distinct stages with different characteristics:
// - Initial: Primitive neural circuits, high plasticity
// - CriticalPeriod: Peak learning ability, experience-dependent wiring
// - Maturation: Refinement of connections, pruning of unused pathways
// - Adult: Stable architecture with limited plasticity for maintenance
// - Aging: Age-related changes in connectivity and function
// 
// DevelopmentSystem implementation placeholder - Phase 2 will implement real dynamics

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    // Returns: Current developmental stage
    // Used to determine which plasticity rules are active and at what strength
    DevelopmentalStage getStage() const;
    
    // Set developmental stage
    // Parameters:
    //   stage - New developmental stage to set
    // Note: Typically set automatically through advanceStage(), but can be
    // manually controlled for specific experiments or scenarios
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    // Automatically progresses through stages in the correct order:
    // Initial -> CriticalPeriod -> Maturation -> Adult -> Aging
    // Called regularly by the main simulation loop
    // Resets stage-specific parameters when advancing
    void advanceStage();
    
    // Get stage name
    // Returns: Human-readable name of current stage
    // Useful for logging and debugging developmental processes
    const char* getStageName() const;
    
    // Update development (simple version)
    // Parameters:
    //   brain - Pointer to the brain being developed
    //   currentStep - Current simulation step (used for age calculation)
    // Simple update that calculates developmental age from simulation steps
    // Note: Legacy interface, use update(Brain*, RandomGenerator&, TimestepDuration) for Phase 2
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    // Parameters:
    //   brain - Pointer to the brain being developed
    //   rng - Random number generator (for stochastic developmental events)
    //   dt - Timestep duration (for continuous time tracking)
    // Comprehensive development update including stage transitions and plasticity modulation
    // Note: This is the primary interface for development system updates
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    // Returns: Multiplicative factor (0.0 to 2.0) that scales all plasticity rates
    // Critical periods have high modifiers (>1.0), adult stages have low modifiers (<1.0)
    // Used by plasticity rules to modulate learning strength based on developmental stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    // Returns: true if current stage is the critical period for sensory learning
    // Critical periods are windows of heightened plasticity for specific functions
    // Lasts for finite duration, after which plasticity decreases significantly
    bool isCriticalPeriod() const;
    
    // Get critical period progress
    // Returns: Fraction (0.0 to 1.0) of critical period that has elapsed
    // Useful for tracking when critical periods end and for designing experiments
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    // Returns: Age of the system in simulation time units (typically seconds)
    // Used to determine stage transitions and calculate age-dependent effects
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    // Parameters:
    //   age - New developmental age in simulation time units
    // Note: Setting age directly bypasses stage progression logic
    // Used for testing specific developmental stages or experimental conditions
    void setDevelopmentalAge(double age) { age_ = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;  // Current developmental age in simulation time units
};

} // namespace nlm
