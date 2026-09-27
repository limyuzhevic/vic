#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

/**
 * @file DevelopmentSystem.hpp
 * @brief Header file for the DevelopmentSystem class implementing neural development
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the DevelopmentSystem class definition, which provides
 * developmental regulation for neural systems. The DevelopmentSystem implements
 * developmental stages, critical periods, and activity-dependent plasticity
 * modulation essential for neural maturation and adaptive behavior.
 */

/**
 * @class DevelopmentSystem
 * @brief System that orchestrates neural development and plasticity modulation
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The DevelopmentSystem implements developmental regulation for neural systems,
 * managing developmental stages, critical periods, and activity-dependent
 * plasticity modulation. It coordinates developmental processes that shape
 * neural connectivity, learning capabilities, and behavioral strategies.
 *
 * Key features:
 * - Multiple developmental stages with distinct characteristics
 * - Critical period regulation for sensitive learning windows
 * - Activity-dependent plasticity modulation
 * - Structural and functional development coordination
 * - Integration with neural system dynamics
 *
 * @note This class manages developmental timing and plasticity modulation
 * that affects learning efficiency, memory formation, and behavioral adaptation
 * throughout the agent's lifecycle.
 *
 * @warning Development system must be properly initialized and integrated
 * with the neural system before starting developmental processes.
 *
 * @see Brain, RandomGenerator, DevelopmentStage
 */
class DevelopmentSystem {
public:
    /**
     * @brief Constructor for the DevelopmentSystem class
     * @post Creates development system with initial state
     * @note Development system starts at initial developmental stage
     */
    DevelopmentSystem();
    
    ~DevelopmentSystem();
    
    // Get current developmental stage
    /**
     * @brief Get current developmental stage
     * @return DevelopmentalStage Current developmental stage
     * @post Returns the current stage of development
     * @note Stage affects plasticity, learning rate, and system capabilities
     */
    DevelopmentalStage getStage() const;
    
    /**
     * @brief Set current developmental stage
     * @param stage New developmental stage
     * @post Developmental stage is updated
     * @note Changing stages may trigger stage-specific plasticity changes
     */
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    /**
     * @brief Advance developmental stage to next stage
     * @post Developmental stage is incremented to next stage
     * @note Automatically called when developmental age reaches thresholds
     */
    void advanceStage();
    
    // Get stage name
    /**
     * @brief Get descriptive name of current developmental stage
     * @return const char* String describing the current stage
     * @post Returns human-readable name of current stage
     * @note Useful for logging and monitoring development progress
     */
    const char* getStageName() const;
    
    // Update development (simple version)
    /**
     * @brief Update development based on simulation step
     * @param brain Reference to brain for system integration
     * @param currentStep Current simulation step number
     * @post Development parameters are updated based on current stage
     * @note Simple version with basic stage-based updates
     */
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    /**
     * @brief Update development with comprehensive parameters
     * @param brain Reference to brain for system integration
     * @param rng Reference to random number generator
     * @param dt Timestep duration
     * @post Development is updated with full temporal and stochastic parameters
     * @note Detailed version supporting complex developmental dynamics
     */
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    /**
     * @brief Get plasticity modifier for current developmental stage
     * @return float Plasticity modifier (typically 0.0 to 2.0)
     * @post Returns factor that modifies plasticity rates for current stage
     * @note Higher values increase plasticity, lower values decrease learning
     */
    float getPlasticityModifier() const;
    
    // Get critical period info
    /**
     * @brief Check if current developmental stage is a critical period
     * @return bool true if current stage is a critical period, false otherwise
     * @post Returns critical period status for current stage
     * @note Critical periods are sensitive learning windows
     */
    bool isCriticalPeriod() const;
    
    /**
     * @brief Get progress through current critical period
     * @return float Progress from 0.0 (start) to 1.0 (end)
     * @post Returns normalized progress through current critical period
     * @note Useful for monitoring and predicting developmental milestones
     */
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    /**
     * @brief Get developmental age in simulation time
     * @return double Developmental age in simulation time units
     * @post Returns current developmental age
     * @note Age determines current developmental stage and capabilities
     */
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    /**
     * @brief Set developmental age
     * @param age Developmental age to set
     * @post Developmental age is updated
     * @warning Setting age outside normal ranges may cause unpredictable behavior
     */
    void setDevelopmentalAge(double age) { age_ = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm