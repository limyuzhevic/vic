// Brain development module - handles developmental system and maturation
// 
// This module provides functions for managing developmental processes that are part of Phase 6.
// It handles developmental stages, maturation effects, and age-dependent changes in neural
// plasticity and cognitive capabilities.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainDevelopment
 * @brief Manages developmental processes and maturation
 * 
 * The BrainDevelopment class provides functions for managing developmental processes that
 * are part of Phase 6: Final Integration. It handles developmental stages, maturation effects,
 * and age-dependent changes in neural plasticity and cognitive capabilities. Development
 * is essential for the brain's growth and adaptation over time.
 * 
 * @details This class implements the developmental systems required for the integrated
 * brain architecture:
 * - Developmental stages: Progress through Initial, CriticalPeriod, Maturation, Adult, Aging
 * - Plasticity modulation: Modulate learning rates based on developmental stage
 * - Maturation effects: Enhance cognitive capabilities and efficiency
 * - Aging effects: Decline in plasticity and cognitive function
 * 
 * @note Development is a fundamental aspect of the brain's lifecycle, enabling growth,
 * learning, and adaptation from initial states to mature capabilities.
 */
class BrainDevelopment {
public:
    /**
     * @brief Update development system effects
     * 
     * This method updates the development system with current simulation progress,
     * applying developmental stage effects and modulating plasticity based on age.
     * Development affects learning rates, connectivity, and cognitive capabilities.
     * 
     * @param brain Pointer to Brain instance
     * @param currentStep Current simulation step
     * @param timestep Time step size (seconds)
     * 
     * @pre brain must not be null, development system must be initialized
     * @post Developmental effects applied based on current stage
     */
    static void updateDevelopment(Brain* brain, SimulationStep currentStep, TimestepDuration timestep);
    
    /**
     * @brief Apply developmental stage effects on plasticity
     * 
     * This method applies the effects of the current developmental stage on neural
     * plasticity, modulating learning rates and structural changes based on whether
     * the brain is in Initial, CriticalPeriod, Maturation, Adult, or Aging stage.
     * 
     * @param brain Pointer to Brain instance
     * @param developmentalStage Current developmental stage
     * 
     * @pre brain must not be null
     * @post Plasticity modulated by developmental stage
     */
    static void applyDevelopmentalStageEffects(Brain* brain, DevelopmentalStage developmentalStage);
    
    /**
     * @brief Get current developmental stage
     * 
     * This method returns the current developmental stage of the brain, which
     * determines learning rates, plasticity, and cognitive capabilities.
     * 
     * @param brain Pointer to Brain instance
     * @return Current developmental stage
     * 
     * *pre brain must not be null
     * @post Developmental stage returned
     */
    static DevelopmentalStage getDevelopmentalStage(Brain* brain);
    
    /**
     * @brief Set developmental stage
     * 
     * This method sets the developmental stage of the brain, allowing for manual
     * control over development or simulation of different life stages.
     * 
     * @param brain Pointer to Brain instance
     * @param stage Developmental stage to set
     * 
     * @pre brain must not be null
     * @post Developmental stage set
     */
    static void setDevelopmentalStage(Brain* brain, DevelopmentalStage stage);
    
    /**
     * @brief Check if development is complete
     * 
     * This method checks whether development is complete, typically indicating
     * that the brain has reached the Adult or Aging stage. Development completion
     * affects learning dynamics and plasticity.
     * 
     * @param brain Pointer to Brain instance
     * @return True if development is complete (Adult or Aging stage)
     * 
     * @pre brain must not be null
     * @post Development completion status returned
     */
    static bool isDevelopmentComplete(Brain* brain);
    
    /**
     * @brief Get plasticity modulation factor based on developmental stage
     * 
     * This method returns the plasticity modulation factor based on the current
     * developmental stage. Plasticity is highest during Initial stage and decreases
     * through CriticalPeriod, Maturation, Adult, to Aging stages.
     * 
     * @param brain Pointer to Brain instance
     * @return Plasticity modulation factor (typically 0-2, where 1.0 is baseline)
     * 
     * @pre brain must not be null
     * @post Plasticity modulation factor returned
     */
    static float getPlasticityModulation(Brain* brain);
};

} // namespace nlm