#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

/**
 * @file DevelopmentSystem.hpp
 * 
 * Neural development system for NLM
 * 
 * This file provides the neural development system for NLM, implementing:
 * - Developmental stage progression
 * - Plasticity modulation across stages
 * - Critical period management
 * - Structural development changes
 * - Age-based adaptive behavior
 * 
 * The development system in NLM:
 * - **DevelopmentalStage**: Represents stages of neural development
 * - **DevelopmentSystem**: Orchestrates neural development progression
 * - **Plasticity management**: Controls synaptic plasticity across stages
 * - **Critical periods**: Manages sensitive learning windows
 * - **Structural changes**: Implements developmental structural changes
 * 
 * Key features:
 * - **Stage progression**: Advances through developmental stages
 * - **Plasticity modulation**: Controls learning ability across stages
 * - **Critical periods**: Manages sensitive developmental windows
 * - **Age tracking**: Tracks developmental age and progression
 * - **Adaptive behavior**: Adapts behavior based on developmental stage
 * 
 * @note Critical for neural development and adaptation
 * @note Used extensively in brain development and plasticity
 * @note Essential for developmental learning
 * @see Config.hpp - Configuration options for development
 * @see Brain.hpp - Integration with brain architecture
 * @see DevelopmentSystem.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

// Forward declarations
class RandomGenerator;

/**
 * @class DevelopmentSystem
 * 
 * Neural implementation of developmental progression and plasticity management.
 * 
 * This class implements the neural development system, orchestrating
 * developmental stage progression, plasticity modulation, and structural
 * development changes. It is essential for:
 * - **Developmental stages**: Managing progression through development stages
 * - **Plasticity modulation**: Controlling synaptic plasticity across stages
 * - **Critical periods**: Managing sensitive learning windows
 * - **Structural changes**: Implementing developmental structural changes
 * - **Age-based adaptation**: Adapting behavior based on developmental age
 * 
 * Development system characteristics in NLM:
 * - **Stage progression**: Advances through developmental stages sequentially
 * - **Plasticity control**: Modulates learning ability based on developmental stage
 * - **Critical period management**: Manages sensitive developmental windows
 * - **Age tracking**: Tracks developmental age and progression
 * - **Adaptive behavior**: Adapts behavior and learning based on stage
 * 
 * Key components:
 * - **DevelopmentalStage**: Enumerates developmental stages
 * - **Stage management**: Tracks and progresses through developmental stages
 * - **Plasticity control**: Manages synaptic plasticity across stages
 * - **Critical periods**: Handles sensitive learning windows
 * - **Structural changes**: Implements developmental structural modifications
 * 
 * @note Implements developmental stages and plasticity modulation
 * @see RandomGenerator - For random number generation
 * @see Brain.hpp - For brain integration
 * @see Types.hpp - For type definitions
 * @see DevelopmentSystem.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class DevelopmentSystem {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new development system instance with default parameters.
     * The development system is initialized with default developmental
     * stage and age.
     */
    DevelopmentSystem();
    
    /**
     * @brief Destructor
     * 
     * Cleans up development system resources and performs any necessary
     * cleanup operations. The development system is left in a valid but
     * non-operational state after destruction.
     */
    ~DevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage. This is used for:
     * - Stage tracking
     * - Stage-specific behavior
     * - Plasticity management
     * - Development management
     */
    DevelopmentalStage getStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * Sets the current developmental stage. This is used for:
     * - Stage setting
     * - Stage management
     * - Development control
     * - Development state
     */
    void setStage(DevelopmentalStage stage);
    
    /**
     * @brief Advance developmental stage
     * 
     * Advances the developmental stage to the next stage. This is used for:
     * - Stage progression
     * - Development advancement
     * - Development movement
     * - Development update
     */
    void advanceStage();
    
    /**
     * @brief Get stage name
     * 
     * Returns the name of the current developmental stage. This is used for:
     * - Stage naming
     * - Stage identification
     * - Stage description
     * - Stage documentation
     */
    const char* getStageName() const;
    
    /**
     * @brief Update development (simple version)
     * 
     * Updates the development system with brain reference and current step.
     * This is used for:
     * - Development update
     * - Stage management
     * - Development tracking
     * - Development processing
     */
    void update(Brain* brain, SimulationStep currentStep);
    
    /**
     * @brief Update development with full parameters
     * 
     * Updates the development system with brain reference, random generator,
     * and timestep duration. This is used for:
     * - Development update
     * - Stage management
     * - Development tracking
     * - Development processing
     */
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    /**
     * @brief Get plasticity modifier for current stage
     * 
     * Returns the plasticity modifier for the current developmental stage.
     * This is used for:
     * - Plasticity assessment
     * - Plasticity management
     * - Plasticity control
     * - Plasticity adjustment
     */
    float getPlasticityModifier() const;
    
    /**
     * @brief Get critical period info
     * 
     * Determines whether the current stage is a critical period.
     * This is used for:
     * - Critical period assessment
     * - Critical period management
     * - Critical period tracking
     * - Critical period control
     */
    bool isCriticalPeriod() const;
    
    /**
     * @brief Get critical period progress
     * 
     * Returns the critical period progress for the current stage.
     * This is used for:
     * - Critical period assessment
     * - Critical period management
     * - Critical period tracking
     * - Critical period control
     */
    float getCriticalPeriodProgress() const;
    
    /**
     * @brief Get developmental age
     * 
     * Returns the developmental age (in simulation time). This is used for:
     * - Age tracking
     * - Development tracking
     * - Development management
     * - Development processing
     */
    double getDevelopmentalAge() const { return age_; }
    
    /**
     * @brief Set developmental age
     * 
     * Sets the developmental age (in simulation time). This is used for:
     * - Age setting
     * - Age management
     * - Development control
     * - Development state
     */
    void setDevelopmentalAge(double age) { age_ = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm
