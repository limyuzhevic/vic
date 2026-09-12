// Brain neuromodulation module - handles neuromodulation systems (dopamine, curiosity, novelty)
// 
// This module provides functions for managing neuromodulation systems that are part of Phase 6.
// It handles dopamine (reward and reinforcement), curiosity (exploration motivation), and
// novelty detection systems that regulate learning, attention, and behavior.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainNeuromodulation
 * @brief Manages neuromodulation systems that regulate learning and behavior
 * 
 * The BrainNeuromodulation class provides functions for managing neuromodulation systems
 * that are part of Phase 6: Final Integration. It handles dopamine (reward and reinforcement),
 * curiosity (exploration motivation), and novelty detection systems that regulate learning,
 * attention, and behavior. Neuromodulation is essential for adaptive behavior and learning.
 * 
 * @details This class implements the neuromodulation systems required for the integrated
 * brain architecture:
 * - Dopamine system: Processes rewards and reinforcement signals
 * - Curiosity system: Drives exploration and discovery behavior
 * - Novelty detection: Identifies new and surprising information
 * - Prediction error: Computes discrepancy between predicted and actual outcomes
 * 
 * @note Neuromodulation systems provide essential learning signals that modulate
 * plasticity, attention, and motivation based on environmental feedback.
 */
class BrainNeuromodulation {
public:
    /**
     * @brief Update neuromodulation systems
     * 
     * This method updates all neuromodulation systems (dopamine, curiosity, novelty,
     * prediction error) based on current neural activity and environmental input.
     * Neuromodulation levels influence plasticity, attention, and behavior.
     * 
     * @param brain Pointer to Brain instance
     * @param timestep Time step size (seconds)
     * 
     * @pre brain must not be null
     * @post All neuromodulation systems updated
     */
    static void updateNeuromodulation(Brain* brain, TimestepDuration timestep);
    
    /**
     * @brief Apply neuromodulation effects on neural excitability
     * 
     * This method applies neuromodulatory signals to neurons, modulating their
     * excitability and firing properties. Neuromodulators like dopamine can
     * enhance or suppress neural activity based on behavioral relevance.
     * 
     * @param brain Pointer to Brain instance
     * @param neuromodulator Neuromodulator signal to apply
     * 
     * @pre brain must not be null
     * @post Neuromodulation effects applied to neural excitability
     */
    static void applyNeuromodulation(Brain* brain, const Neuromodulator& neuromodulator);
    
    /**
     * @brief Get dopamine level (reward signal)
     * 
     * This method returns the current dopamine level, which represents the reward
     * prediction error signal. Dopamine modulates neural excitability and plasticity
     * based on reward prediction.
     * 
     * @param brain Pointer to Brain instance
     * @return Dopamine level (0-1, where 1.0 is maximum reward signal)
     * 
     * @pre brain must not be null, dopamine system must be initialized
     * @post Dopamine level returned
     */
    static float getDopamineLevel(Brain* brain);
    
    /**
     * @brief Get curiosity level (exploration drive)
     * 
     * This method returns the current curiosity level, which represents the drive
     * for exploration and discovery. Curiosity motivates the brain to seek novel
     * and information-rich experiences.
     * 
     * @param brain Pointer to Brain instance
     * @return Curiosity level (0-1, where 1.0 is maximum exploration drive)
     * 
     * @pre brain must not be null, curiosity system must be initialized
     * @post Curiosity level returned
     */
    static float getCuriosityLevel(Brain* brain);
    
    /**
     * @brief Get novelty level (novelty detection)
     * 
     * This method returns the current novelty level, which represents the detection
     * of novel or surprising information. Novelty detection drives attention and
     * learning toward new experiences.
     * 
     * @param brain Pointer to Brain instance
     * @return Novelty level (0-1, where 1.0 indicates novel information)
     * 
     * @pre brain must not be null, novelty system must be initialized
     * @post Novelty level returned
     */
    static float getNoveltyLevel(Brain* brain);
    
    /**
     * @brief Get prediction error level
     * 
     * This method returns the current prediction error level, which represents the
     * discrepancy between predicted and actual sensory input. Prediction error is
     * a fundamental learning signal that drives updates to internal models.
     * 
     * @param brain Pointer to Brain instance
     * @return Prediction error level (higher = larger prediction error)
     * 
     * @pre brain must not be null, prediction error system must be initialized
     * @post Prediction error level returned
     */
    static float getPredictionErrorLevel(Brain* brain);
    
    /**
     * @brief Get neuromodulation factor for plasticity
     * 
     * This method returns the current neuromodulation factor, which scales plasticity
     * based on current neuromodulator levels. Neuromodulation factors are essential
     * for adaptive learning and behavioral flexibility.
     * 
     * @param brain Pointer to Brain instance
     * @return Neuromodulation factor (typically 0-2, where 1.0 is baseline)
     * 
     * @pre brain must not be null, neuromodulation system must be initialized
     * @post Neuromodulation factor returned
     */
    static float getPlasticityFactor(Brain* brain);
};

} // namespace nlm