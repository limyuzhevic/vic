#pragma once

/**
 * @file PredictionError.hpp
 * @brief Prediction error computation system for learning and adaptation
 * 
 * This header defines the PredictionError class for the brain's prediction error system.
 * The prediction error system computes the difference between predicted and actual values,
 * providing a fundamental learning signal for reinforcement learning and neural plasticity.
 * It serves as the core mechanism for temporal difference learning and value-based learning.
 * 
 * Prediction Error Theory:
 * - Prediction errors drive learning and adaptation
 * - TD learning relies on accurate prediction error computation
 * - Prediction errors signal unexpected or surprising events
 * - Error minimization leads to better environmental models
 * - Prediction errors modulate neural plasticity and learning rates
 * 
 * Prediction Error Functions:
 * - Error computation: Calculate difference between predicted and actual values
 * - Error magnitude: Compute absolute error for neuromodulation
 * - Error history: Maintain temporal sequence of prediction errors
 * - Error integration: Accumulate errors over time for stable learning
 * - Error application: Apply errors to update value estimates and policies
 * 
 * Integration Points:
 * - Connected to neuromodulation systems (dopamine, curiosity)
 * - Used by curiosity system for exploration motivation
 * - Applied by reward system for reward computation
 * - Affects attention and memory systems
 * - Influences planning and decision-making processes
 * - Drives plasticity in STDP and Hebbian systems
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class PredictionError;
class Brain;
class Neuromodulator;

/**
 * @brief Prediction error computation system for learning and adaptation
 * 
 * Implements the brain's prediction error system for computing differences
 * between predicted and actual values. The prediction error system provides
 * the fundamental learning signal for reinforcement learning, temporal
 * difference learning, and neural plasticity. It serves as the core
 * mechanism for value-based learning and environmental adaptation.
 * 
 * Prediction Error System Architecture:
 * 1. Prediction: Generate predictions about environmental states
 * 2. Actualization: Observe actual environmental states
 * 3. Comparison: Compute difference between predicted and actual
 * 4. Integration: Integrate prediction errors over time
 * 5. Application: Apply prediction errors to update value estimates
 * 
 * Prediction Error Processing Pipeline:
 * 1. Prediction -> Actual Observation -> Error Computation
 * 2. Error Integration -> Error Magnitude -> Neuromodulation
 * 3. Neuromodulation -> Plasticity -> Learning
 * 4. Learning -> Updated Predictions -> Next Cycle
 * 
 * Prediction Error System Properties:
 * - Accuracy: Precise computation of prediction differences
 * - Sensitivity: Strong response to unexpected events
 * - Integration: Accumulation over time for stable learning
 * - Modulation: Response depends on context and state
 * - Distribution: Prediction errors follow specific statistical patterns
 * 
 * Prediction Error Dynamics:
 * - Temporal difference: Error computed over time intervals
 * - Credit assignment: Error attributed to appropriate causes
 * - Variance reduction: Error signals reduce prediction variance
 * - Exploration: Errors drive exploration of uncertain states
 * - Exploitation: Errors reinforce successful predictions
 * 
 * Thread Safety: Prediction error system is thread-safe for concurrent access
 * from different brain components. Error computation and integration are
 * protected with mutexes.
 * 
 * Memory Management: Prediction error system uses pImpl pattern with pointer
 * storage for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note Prediction error system is fundamental to learning and adaptation.
 *       It works in conjunction with other systems for comprehensive
 *       reinforcement learning.
 * @see Neuromodulator for neuromodulatory error processing
 * @see Curiosity for error-driven exploration
 * @see Reward for error-based reward computation
 */
class PredictionError {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a prediction error system with initial state. The prediction
     * error implementation uses the pImpl pattern for efficient memory usage
     * and to reduce compilation dependencies.
     */
    PredictionError();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of PredictionError's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~PredictionError();
    
    /**
     * @brief Initialize prediction error system with brain reference
     * 
     * Initializes the prediction error system with reference to the brain,
     * enabling access to brain's state, predictions, and other systems for
     * context-aware error computation.
     * 
     * @param brain Pointer to the brain that contains this prediction error system
     * 
     * @note Thread-safe with mutex protection.
     *       This method should be called after brain construction but
     *       before prediction error system is used for learning.
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get current prediction error value
     * 
     * Returns the current prediction error computed by the prediction error system.
     * The prediction error represents the difference between predicted and actual
     * values, typically ranging from -1.0 (under-prediction) to +1.0 (over-prediction).
     * 
     * @return Current prediction error value (-1.0 to +1.0 typical range)
     * 
     * @note Prediction error is dynamically computed based on prediction accuracy
     *       and environmental feedback. It represents the surprise or prediction error
     *       signal for learning.
     */
    float getError() const;
    
    /**
     * @brief Compute prediction error from predicted and actual values
     * 
     * Computes the prediction error as the difference between predicted and actual
     * values. This is the fundamental method for generating prediction error signals.
     * 
     * Prediction Error Computation:
     * 1. Validate input values (actual range, prediction range)
     * 2. Calculate difference: error = actual - predicted
     * 3. Normalize error to typical range (-1.0 to +1.0)
     * 4. Store error for history and integration
     * 5. Return normalized error
     * 
     * @param predicted Predicted value based on current model
     * @param actual Actual observed value from environment
     * @return Computed prediction error (typically -1.0 to +1.0)
     * 
     * @note Thread-safe with mutex protection.
     *       Prediction error computation is fundamental to learning
     *       and drives exploration and adaptation behavior.
     * 
     * @see getError()
     * @see getMagnitude()
     */
    void computeError(float predicted, float actual);
    
    /**
     * @brief Update prediction with new estimate
     * 
     * Updates the internal prediction based on the computed prediction error.
     * This method implements the learning aspect of prediction error, updating
     * the prediction model based on prediction error signals.
     * 
     * Prediction Update Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Get current prediction error
     * 3. Apply learning rate to error signal
     * 4. Update prediction: new_prediction = old_prediction + (error * learning_rate)
     * 5. Store updated prediction
     * 6. Return new prediction
     * 
     * @param newPrediction New prediction value based on learning
     * @return Updated prediction value
     * 
     * @note Thread-safe with mutex protection.
     *       This method implements the learning aspect of prediction error,
     *       updating the prediction model based on prediction error signals.
     */
    float updatePrediction(float newPrediction);
    
    /**
     * @brief Get prediction error history
     * 
     * Returns the complete history of prediction errors computed by the
     * prediction error system. This provides temporal context for error
     * processing and is used for pattern discovery and learning analysis.
     * 
     * @return Vector of past prediction errors (most recent at end)
     * 
     * @note Prediction error history is updated with each computeError() operation.
     *       This provides temporal context for error processing and
     *       pattern discovery in the prediction error system.
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear prediction error history
     * 
     * Removes all past prediction errors from the history. This is used
     * to reset the temporal memory of the prediction error system while
     * preserving the current error value.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the temporal memory but preserves
     *       the current state of prediction error.
     */
    void clearHistory();
    
    /**
     * @brief Get error magnitude for neuromodulation
     * 
     * Returns the absolute value of the current prediction error,
     * representing the strength or salience of the prediction error signal.
     * This is used by neuromodulatory systems to determine the appropriateness
     * of neuromodulator release.
     * 
     * @return Error magnitude (absolute value of current error)
     * 
     * @note Neuromodulators like dopamine respond more strongly to
     *       prediction error magnitude than to error sign. This provides
     *       a measure of prediction error salience independent of error valence.
     */
    float getMagnitude() const;
    
    /**
     * @brief Set prediction error system parameters
     * 
     * Configures internal parameters for prediction error processing, such as
     * learning rate, error normalization, or integration parameters.
     * 
     * @param learningRate Learning rate for prediction updates (0.0-1.0)
     * @param errorNormalizationFactor Factor to normalize error values
     * @param integrationRate Rate at which errors are integrated over time
     * 
     * @note Thread-safe with mutex protection.
     *       These parameters affect prediction error processing dynamics and
     *       should be set before prediction error system is used for learning.
     */
    void setParameters(float learningRate, float errorNormalizationFactor,
                       float integrationRate);
    
    /**
     * @brief Get current prediction error parameters
     * 
     * @return Vector containing current prediction error parameters
     *         [learningRate, errorNormalizationFactor, integrationRate]
     * 
     * @note Parameters control prediction error processing and learning dynamics.
     */
    std::vector<float> getParameters() const;
    
    /**
     * @brief Get current developmental stage influence on prediction
     * 
     * Returns how the current developmental stage affects prediction error processing.
     * Different developmental stages have different prediction sensitivities
     * and processing characteristics.
     * 
     * @return Developmental stage multiplier for prediction error processing
     * 
     * @note Developmental stage influences prediction sensitivity, error processing,
     *       and learning mechanisms.
     *       Younger stages may have different prediction strategies.
     * 
     * @see DevelopmentSystem
     */
    float getDevelopmentalStageMultiplier() const;
    
    /**
     * @brief Reset prediction error system
     * 
     * Resets the prediction error system to initial state. This clears error history
     * and resets prediction error to baseline levels.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the prediction error system but preserves
     *       configuration parameters.
     */
    void reset();
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     * 
     * Hidden implementation for PredictionError system.
     * Stores prediction error state including current error, prediction history,
     * error history, and parameters.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm