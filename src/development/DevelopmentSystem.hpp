#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes
// Models the biological maturation process from initial formation through aging

class DevelopmentSystem {
public:
    /// Constructor - initializes development system in Initial stage
    DevelopmentSystem();
    
    /// Destructor - cleans up development system resources
    ~DevelopmentSystem();
    
    /// Get current developmental stage
    /// @return Current developmental stage (Initial, CriticalPeriod, Maturation, Adult, or Aging)
    DevelopmentalStage getStage() const;
    
    /// Set developmental stage to a specific stage
    /// @param stage The developmental stage to set
    /// @note This resets stage age counters and updates logs
    void setStage(DevelopmentalStage stage);
    
    /// Advance to next developmental stage in sequence
    /// Automatically progresses: Initial -> CriticalPeriod -> Maturation -> Adult -> Aging
    /// @note Resets stage counters and logs the stage transition
    void advanceStage();
    
    /// Get human-readable name of current developmental stage
    /// @return String name of current stage
    const char* getStageName() const;
    
    /// Update development system with basic parameters
    /// @param brain Pointer to the brain being developed
    /// @param currentStep Current simulation step
    /// @note Uses approximate time steps (0.001 per call)
    void update(Brain* brain, SimulationStep currentStep);
    
    /// Update development system with full parameters
    /// @param brain Pointer to the brain being developed
    /// @param rng Random number generator for stochastic development
    /// @param dt Time step duration in simulation seconds
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    /// Get plasticity modifier for current developmental stage
    /// @return Plasticity modifier (1.0 = high plasticity, 0.1 = low plasticity)
    float getPlasticityModifier() const;
    
    /// Check if brain is in critical period
    /// @return True if current stage is CriticalPeriod
    bool isCriticalPeriod() const;
    
    /// Get progress through critical period (0.0 to 1.0)
    /// @return Progress through critical period (1.0 = completed)
    float getCriticalPeriodProgress() const;
    
    /// Get developmental age (in simulation time)
    /// @return Age of brain development in simulation time units
    double getDevelopmentalAge() const { return age_; }
    
    /// Set developmental age (in simulation time)
    /// @param age The developmental age to set
    void setDevelopmentalAge(double age) { age_ = age; }
    
private:
    /// Pimpl implementation detail
    struct Impl;
    
    /// Pointer to implementation (compiler-independent design)
    Impl* pImpl;
    
    /// Total developmental age of the brain (in simulation time)
    double age_;
};

} // namespace nlm
