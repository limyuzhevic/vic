#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @class PredictionError
 * @brief Prediction error signal system for curiosity and learning
 * 
 * The PredictionError class implements the agent's prediction error processing
 * system, managing the difference between predicted and actual values. This signal
 * is fundamental to reinforcement learning, curiosity-driven exploration, and
 * adaptive behavior. Prediction errors indicate how well the agent's expectations
 * match reality and drive learning and plasticity in neural circuits.
 * 
 * Key functions of the prediction error system:
 * - Computes the difference between predicted and actual values
 * - Provides learning signals for value-based and policy-based learning
 * - Drives curiosity and exploration through prediction error magnitude
 * - Facilitates credit assignment in reinforcement learning
 * - Updates the agent's internal models and expectations
 * 
 * The prediction error system is essential for:
 * - Model-based and model-free reinforcement learning
 * - Curiosity-driven exploration and discovery
 * - Reward prediction error calculations
 * - Adaptive behavior in changing environments
 * - Credit assignment in complex task sequences
 * 
 * @note This class is part of Phase 1 implementation. Phase 2 will implement
 *       more sophisticated prediction error computations based on temporal
 *       difference learning, eligibility traces, and Bayesian inference.
 * 
 * @see Neuromodulator
 * @see Dopamine
 * @see Curiosity
 * @see Reward
 * @see Brain
 * @see AgentBrain
 * @see applyRewardModulation()
 * @see getPredictionError()
 */
class PredictionError {
public:
    /**
     * @brief Constructor: Creates a new PredictionError system instance
     * 
     * Initializes the prediction error system with zero error and
     * prepares it for processing prediction discrepancies. The system
     * starts with an empty prediction history and zero current error.
     * 
     * @post Prediction error system initialized with zero error and empty history
     * @note Default construction sets up an empty prediction error system ready for
     *       processing prediction discrepancies. Errors can be computed via the
     *       computeError() method or updated via updatePrediction().
     * @see PredictionError()
     * @see computeError(float, float)
     * @see updatePrediction(float)
     */
    PredictionError();
    
    /**
     * @brief Destructor: Cleans up the PredictionError system resources
     * 
     * Performs final cleanup of the prediction error system's internal state
     * and any allocated resources.
     * 
     * @post All prediction error system resources are released
     * @note Virtual destructor ensures proper cleanup through base class pointers.
     */
    ~PredictionError();
    
    /**
     * @brief Initialize the prediction error system with brain reference
     * 
     * Establishes the connection between the prediction error system and the agent's
     * neural brain. This allows the prediction error system to access neural activity
     * patterns and brain state information needed for adaptive prediction error computation.
     * 
     * @param brain Pointer to the Brain object that provides neural context
     *              and state information for prediction error computations
     * @post Brain reference is stored internally, prediction error system is ready
     *       to receive updates from reward and sensory systems
     * @note This method must be called before any prediction error computations or queries.
     *       The brain pointer should remain valid for the lifetime of this object.
     * @see PredictionError()
     * @see computeError(float, float)
     * @see getError()
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get the current prediction error value
     * 
     * Returns the current prediction error, which represents the difference
     * between the agent's predicted value and the actual received value. This
     * is the core learning signal that drives reinforcement learning and
     * adaptive behavior.
     * 
     * @return float Current prediction error (positive for better-than-expected outcomes)
     * @post None
     * @note Prediction error is computed as actual - predicted. Positive errors
     *       indicate better-than-expected outcomes (learning signal to increase behavior),
     *       negative errors indicate worse-than-expected outcomes (learning signal to decrease behavior).
     *       The magnitude of the error influences the strength of learning.
     * @see computeError(float, float)
     * @see updatePrediction(float)
     * @see getMagnitude()
     */
    float getError() const;
    
    /**
     * @brief Compute prediction error from predicted and actual values
     * 
     * Computes the current prediction error by comparing the agent's predicted
     * value with the actual received value. This is the fundamental operation
     * for generating learning signals in reinforcement learning systems.
     * 
     * The computation follows:
     * 
     *   prediction_error = actual_value - predicted_value
     *   where positive errors indicate better-than-expected outcomes
     *   and negative errors indicate worse-than-expected outcomes
     * 
     * @param predicted Predicted value based on the agent's current model
     * @param actual Actual value received from the environment
     * @post Prediction error is computed and stored internally
     * @note This method should be called whenever new rewards or outcomes are
     *       received. The computed prediction error drives learning and plastic changes
     *       in neural circuits and behavior selection.
     * @see getError()
     * @see updatePrediction(float)
     * @see getMagnitude()
     * @see applyRewardModulation(float, float)
     */
    void computeError(float predicted, float actual);
    
    /**
     * @brief Update the agent's internal prediction
     * 
     * Updates the agent's internal model or prediction based on new information
     * or learning. This method allows the agent to adjust its expectations based
     * on new experiences or learning signals.
     * 
     * @param newPrediction New predicted value to adopt
     * @post Internal prediction is updated to the new value
     * @note This method is used when the agent learns new information about
     *       the value of states or actions. The new prediction replaces the
     *       old prediction in the agent's internal model.
     * @see computeError(float, float)
     * @see getError()
     * @see getMagnitude()
     */
    void updatePrediction(float newPrediction);
    
    /**
     * @brief Get the absolute magnitude of the prediction error
     * 
     * Returns the absolute magnitude of the prediction error, which represents
     * the strength of the learning signal regardless of sign. This is useful
     * for determining the urgency or importance of learning.
     * 
     * @return float Absolute magnitude of prediction error (always non-negative)
     * @post None
     * @note The magnitude indicates how unexpected the current outcome is.
     *       Higher magnitudes suggest greater prediction error and stronger
     *       learning signals. This is often used to modulate learning rates
     *       and attention.
     * @see getError()
     * @see computeError(float, float)
     */
    float getMagnitude() const;
    
    /**
     * @brief Get the complete prediction error history
     * 
     * Returns a reference to the internal prediction error history vector containing
     * all prediction errors that have been computed over time. This provides
     * access to the temporal sequence of prediction errors for analysis and learning.
     * 
     * @return const std::vector<float>& Reference to the prediction error history vector
     * @post None
     * @note The history vector contains all prediction error values computed since
     *       the last clearHistory(). It can be used for statistical analysis, pattern
     *       detection, and learning curve analysis.
     * @see clearHistory()
     * @see computeError(float, float)
     * @see updatePrediction(float)
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear the prediction error history vector
     * 
     * Clears all historical prediction error data from the internal history vector.
     * This removes all previously recorded prediction errors while preserving
     * the current prediction error value and prediction.
     * 
     * @post Prediction error history vector is emptied, all historical data removed
     * @note This method is useful for memory management or when historical
     *       data is no longer needed. The current prediction error and prediction
     *       values remain unchanged.
     * @see getHistory()
     * @see reset()
     */
    void clearHistory();
    
    /**
     * @brief Reset the prediction error system to initial state
     * 
     * Resets all internal state variables, prediction error, and historical
     * data to their initial values. This prepares the prediction error system
     * for a new episode or testing scenario.
     * 
     * @post All state variables reset to initial values, error and prediction zeroed
     * @note This method is useful for starting fresh learning episodes or
     *       for testing purposes. Configuration parameters are preserved
     *       across resets.
     * @see PredictionError()
     * @see computeError(float, float)
     * @see getError()
     */
    void reset();
    
private:
    /**
     * @brief Implementation details for the PredictionError system
     * 
     * Pimpl idiom implementation details for the PredictionError class. This structure
     * contains all the internal implementation details including:
     * - Current prediction error value storage
     * - Predicted value storage
     * - Historical prediction error data storage
     * - Internal computation buffers and temporary variables
     * - Neural brain reference for adaptive prediction error computation
     * 
     * @note Using the Pimpl idiom keeps the header file clean and allows
     *       for easier implementation changes without affecting the interface.
     * @see PredictionError()
     * @see ~PredictionError()
     * @see initialize(class Brain*)
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation details structure
     * 
     * Holds a pointer to the actual implementation of the PredictionError system.
     * This is part of the Pimpl idiom pattern used to separate interface from
     * implementation and reduce compilation dependencies.
     * 
     * @note The pointer is managed by the implementation to ensure proper
     *       lifecycle management. Users should not access this directly.
     * @see Impl
     */
    Impl* pImpl;
};

} // namespace nlm
