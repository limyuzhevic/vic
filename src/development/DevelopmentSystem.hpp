#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

class DevelopmentSystem {
public:
    /**
     * @brief Constructor - Creates development system with default parameters
     * 
     * @details Initializes development with typical biological parameters:
     * - Starts at Initial stage
     * - Sets up critical period timing
     * - Configures plasticity reduction rates
     * - Initializes developmental age to zero
     */
    DevelopmentSystem();
    
    /**
     * @brief Destructor - Cleans up development resources
     */
    ~DevelopmentSystem();
    
    /**
     * @brief Copy constructor - Deleted (DevelopmentSystem is not copyable)
     */
    DevelopmentSystem(const DevelopmentSystem&) = delete;
    
    /**
     * @brief Copy assignment operator - Deleted (DevelopmentSystem is not copyable)
     */
    DevelopmentSystem& operator=(const DevelopmentSystem&) = delete;
    
    /**
     * @brief Move constructor - Enabled for efficient transfer
     */
    DevelopmentSystem(DevelopmentSystem&&) noexcept;
    
    /**
     * @brief Move assignment operator - Enabled for efficient transfer
     */
    DevelopmentSystem& operator=(DevelopmentSystem&&) noexcept;
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current stage (Initial, CriticalPeriod, Maturation, or Adult)
     * 
     * @details The developmental stage determines:
     * - Active plasticity mechanisms
     * - Synaptogenesis vs. pruning dominance
     * - Learning rates and stability requirements
     * - Behavioral flexibility vs. habit strength
     */
    DevelopmentalStage getStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Target developmental stage to set
     * 
     * @details Manual stage setting is useful for:
     * - Testing specific developmental phases
     * - Implementing custom developmental trajectories
     * - Studying stage-specific effects
     * 
     * @warning Changing stages unexpectedly may break neural stability.
     *          Typically stages progress forward only.
     */
    void setStage(DevelopmentalStage stage);
    
    /**
     * @brief Advance developmental stage
     * 
     * @details Advances to the next developmental stage in sequence:
     * Initial → CriticalPeriod → Maturation → Adult
     * 
     * @note This is the normal developmental trajectory. Each stage has
     * different plasticity rates and dominant mechanisms.
     */
    void advanceStage();
    
    /**
     * @brief Get current developmental stage name
     * 
     * @return const char* Human-readable name of current stage
     * 
     * @details Useful for logging and debugging developmental progress.
     * Examples: "Initial", "CriticalPeriod", "Maturation", "Adult"
     */
    const char* getStageName() const;
    
    /**
     * @brief Update development with basic parameters
     * 
     * @param brain Pointer to the brain being developed
     * @param currentStep Current simulation step
     * 
     * @details Simple development update that applies stage-appropriate
     * plasticity modulation and basic structural changes using default timing.
     */
    void update(Brain* brain, SimulationStep currentStep);
    
    /**
     * @brief Update development with full temporal precision
     * 
     * @param brain Pointer to the brain being developed
     * @param rng Random number generator for stochastic developmental events
     * @param dt Simulation timestep duration (ms)
     * 
     * @details Comprehensive development update that applies all stage-specific
     * processes with precise temporal control. This includes:
     * - Plasticity rate modulation
     * - Probabilistic synaptogenesis
     * - Activity-dependent pruning
     * - Critical period regulation
     */
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    /**
     * @brief Get plasticity modifier for current developmental stage
     * 
     * @return float Plasticity rate multiplier (0.0 to 1.0)
     * 
     * @details Returns the factor by which plasticity mechanisms are scaled
     * according to the current developmental stage. This directly controls:
     * - STDP learning rates
     * - Hebbian learning strength
     * - Synaptogenesis probability
     * - Pruning intensity
     */
    float getPlasticityModifier() const;
    
    /**
     * @brief Check if brain is in critical period
     * 
     * @return bool True if currently in CriticalPeriod developmental stage
     * 
     * @details Critical period is a sensitive developmental window where:
     * - Plasticity is heightened but regulated
     * - Activity-dependent refinement occurs
     * - Rapid behavioral adaptation is possible
     * - Later learning is more stable
     */
    bool isCriticalPeriod() const;
    
    /**
     * @brief Get critical period progression (0.0 to 1.0)
     * 
     * @return float Progress through critical period (0.0 = start, 1.0 = end)
     * 
     * @details Indicates how far through the critical period the brain is.
     * Used to:
     * - Time sensitive learning interventions
     * - Schedule behavioral challenges
     * - Monitor developmental progress
     */
    float getCriticalPeriodProgress() const;
    
    /**
     * @brief Get developmental age in simulation time
     * 
     * @return double Developmental age in milliseconds or equivalent time units
     * 
     * @details Returns the current age of the brain, which determines:
     * - Which developmental stage is active
     * - Plasticity rate decay
     * - Critical period timing
     * - Maturation progress
     */
    double getDevelopmentalAge() const { return age_; }
    
    /**
     * @brief Set developmental age directly
     * 
     * @param age New developmental age (ms or equivalent time units)
     * 
     * @details Manually sets developmental age, useful for:
     * - Testing age-specific effects
     * - Implementing custom developmental trajectories
     * - Stress-testing stage transitions
     */
    void setDevelopmentalAge(double age) { age_ = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
