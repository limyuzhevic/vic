#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @file Curiosity.hpp
 * @brief Header file for the Curiosity class implementing exploration motivation
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the Curiosity class definition, which provides
 * exploration motivation based on novelty and prediction error. The Curiosity
 * system computes exploration drive as a weighted combination of novelty
 * and prediction error, encouraging agents to explore novel states and
 * resolve prediction errors.
 */

/**
 * @class Curiosity
 * @brief System for exploration motivation based on novelty and prediction error
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The Curiosity class implements exploration motivation by computing a curiosity
 * drive as a weighted combination of novelty detection and prediction error.
 * This system encourages agents to explore novel states and resolve prediction
 * errors, driving continuous learning and adaptive behavior.
 *
 * Key features:
 * - Weighted combination of novelty and prediction error
 * - Dynamic exploration drive computation
 * - Parameter tuning for exploration-exploitation balance
 * - Integration with brain and neuromodulation systems
 * - Exploration-exploitation trade-off management
 *
 * @note This class is a core component of the neural framework's ability to
 * maintain exploratory behavior and continuous learning in dynamic environments.
 *
 * @warning Curiosity system must be properly initialized and configured
 * before use. Parameter weights significantly affect exploration behavior.
 *
 * @see Brain, Novelty, PredictionError, NeuralWorkingMemory
 */
class Curiosity {
public:
    /**
     * @brief Constructor for the Curiosity class
     * @post Creates curiosity system with initial state
     * @note Curiosity system is ready for exploration computation after construction
     */
    Curiosity();
    
    ~Curiosity();
    
    // Initialize with brain reference
    /**
     * @brief Initialize curiosity system with brain reference
     * @param brain Pointer to brain for system integration
     * @post Curiosity system is initialized and ready for operation
     * @note Required for proper system integration and neuromodulation
     */
    void initialize(class Brain* brain);
    
    // Get curiosity level
    /**
     * @brief Get current curiosity level
     * @return float Current curiosity level (typically 0.0 to 1.0)
     * @post Returns current exploration drive level
     * @note Higher values indicate stronger motivation to explore
     */
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    /**
     * @brief Update curiosity drive based on novelty and prediction error
     * @param novelty Novelty signal (typically 0.0 to 1.0)
     * @param predictionError Prediction error signal (typically 0.0 to 1.0)
     * @param dt Timestep duration for temporal integration
     * @post Curiosity level is updated based on input signals
     * @note Computation: curiosity = novelty_weight * novelty + error_weight * error
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    /**
     * @brief Get exploration drive (alias for getLevel())
     * @return float Current exploration drive level
     * @post Returns same value as getLevel()
     * @note Provided for backward compatibility and clarity
     */
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    /**
     * @brief Set weight for novelty contribution to curiosity
     * @param weight Novelty weight (typically 0.0 to 1.0)
     * @post Novelty weight is updated
     * @note Higher weights increase influence of novelty on exploration
     */
    void setNoveltyWeight(float weight);
    
    /**
     * @brief Set weight for prediction error contribution to curiosity
     * @param weight Prediction error weight (typically 0.0 to 1.0)
     * @post Prediction error weight is updated
     * @note Higher weights increase influence of prediction error on exploration
     */
    void setPredictionErrorWeight(float weight);
    
    // Reset
    /**
     * @brief Reset curiosity system to initial state
     * @post Curiosity level and internal state are reset
     * @note Clears accumulated curiosity and resets weights
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm