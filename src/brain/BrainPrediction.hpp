// Brain prediction module - handles prediction and error computation systems
// 
// This module provides functions for managing the prediction system and prediction error
// computation that are part of Phase 6. It handles sensory prediction, prediction error
// tracking, and predictive coding mechanisms.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainPrediction
 * @brief Manages prediction and error computation systems
 * 
 * The BrainPrediction class provides functions for managing the prediction system and
 * prediction error computation that are part of Phase 6: Final Integration. It handles
 * sensory prediction, prediction error tracking, and predictive coding mechanisms that
 * enable the brain to anticipate sensory consequences and learn from prediction errors.
 * 
 * @details This class implements the prediction systems required for the integrated
 * brain architecture:
 * - Prediction system: Computes predicted sensory consequences of actions
 * - Prediction error: Computes discrepancy between predicted and actual sensory input
 * - Predictive coding: Updates internal models based on prediction errors
 * 
 * @note Prediction systems are essential for goal-directed behavior, enabling the
 * brain to plan actions and anticipate their consequences.
 */
class BrainPrediction {
public:
    /**
     * @brief Update prediction system with current sensory state
     * 
     * This method updates the prediction system with the current sensory state,
     * computing predicted sensory consequences and prediction errors. The prediction
     * system enables the brain to anticipate sensory consequences of neural activity
     * and current state.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null, prediction system must be initialized
     * @post Prediction system updated with current sensory state
     */
    static void updatePredictionSystem(Brain* brain);
    
    /**
     * @brief Get prediction error signal
     * 
     * This method returns the current prediction error signal, which represents the
     * discrepancy between predicted and actual sensory input. Prediction error is
     * a fundamental learning signal in the brain, driving updates to internal models.
     * 
     * @param brain Pointer to Brain instance
     * @return Prediction error level (higher = larger prediction error)
     * 
     * @pre brain must not be null
     * @post Prediction error level returned
     */
    static float getPredictionError(Brain* brain);
    
    /**
     * @brief Reset prediction system
     * 
     * This method resets the prediction system to its initial state, clearing
     * prediction histories and error signals.
     * 
     * @param brain Pointer to Brain instance
     * 
     * @pre brain must not be null
     * @post Prediction system reset
     */
    static void reset(Brain* brain);
    
    /**
     * @brief Enable or disable prediction system
     * 
     * This method enables or disables the prediction system, allowing for
     * temporary suspension of predictive coding during specific processing modes.
     * 
     * @param brain Pointer to Brain instance
     * @param enable Enable flag (true = enable, false = disable)
     * 
     * @pre brain must not be null
     * @post Prediction system enabled or disabled
     */
    static void enable(Brain* brain, bool enable);
    
    /**
     * @brief Check if prediction system is enabled
     * 
     * This method checks whether the prediction system is currently enabled.
     * 
     * @param brain Pointer to Brain instance
     * @return True if prediction system is enabled, false otherwise
     * 
     * @pre brain must not be null
     * @post Enable status returned
     */
    static bool isEnabled(Brain* brain);
};

} // namespace nlm