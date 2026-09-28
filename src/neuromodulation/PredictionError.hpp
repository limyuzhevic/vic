#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// 
// The prediction error system tracks the discrepancy between the brain's
// expectations and actual sensory outcomes, providing a signal for learning
// and curiosity-driven exploration. This is a core component of the
// neuromodulation system that drives adaptive behavior.
// 
// The prediction error follows the temporal difference learning framework:
// error = actual_reward - predicted_reward
// where positive error indicates better-than-expected outcomes
// and negative error indicates worse-than-expected outcomes.
// 
// @note Used by curiosity system to drive exploration
// @note Used by reward modulation to adjust learning rates

// Prediction error signal for curiosity and learning
// Computes difference between predicted and actual values
// 
// The prediction error system tracks the discrepancy between the brain's
// expectations and actual sensory outcomes, providing a signal for learning
// and curiosity-driven exploration. This is a core component of the
// neuromodulation system that drives adaptive behavior.
// 
// The prediction error follows the temporal difference learning framework:
// error = actual_reward - predicted_reward
// where positive error indicates better-than-expected outcomes
// and negative error indicates worse-than-expected outcomes.
// 
// @note Used by curiosity system to drive exploration
// @note Used by reward modulation to adjust learning rates

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Disable copying, enable moving (Rule of Five compliance)
    PredictionError(const PredictionError&) = delete;
    PredictionError& operator=(const PredictionError&) = delete;
    PredictionError(PredictionError&&) noexcept;
    PredictionError& operator=(PredictionError&&) noexcept;
    
    /**
     * @brief Initialize with brain reference (modern API)
     * 
     * @param brain Shared pointer to brain object
     * @note Uses weak_ptr internally to avoid circular references
     */
    void initialize(std::shared_ptr<class Brain> brain);
    
    /**
     * @brief Initialize with brain reference (backward compatibility)
     * 
     * @param brain Raw pointer to brain object
     * @warning This creates a non-owning reference; brain must outlive this PredictionError
     * @note Modern code should use the shared_ptr version for better memory safety
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get error value
     * 
     * @return float Current prediction error (actual - predicted)
     */
    float getError() const;
    
    /**
     * @brief Compute prediction error
     * 
     * @param predicted Expected value (from brain's forward model)
     * @param actual Actual observed value
     * 
     * @note Error is stored and added to history for analysis
     * @note Higher magnitude indicates larger discrepancy
     */
    void computeError(float predicted, float actual);
    
    /**
     * @brief Update prediction value
     * 
     * @param newPrediction New predicted value
     */
    void updatePrediction(float newPrediction);
    
    /**
     * @brief Get prediction error history
     * 
     * @return const std::vector<float>& Vector of recent error values
     * @note History is bounded to prevent memory issues
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear error history
     * 
     * @note Resets error history to empty vector
     */
    void clearHistory();
    
    /**
     * @brief Get absolute error magnitude
     * 
     * @return float Absolute value of prediction error
     * @note Used for neuromodulation scaling
     */
    float getMagnitude() const;
    
private:
    /**
     * @brief Hidden implementation pointer (Pimpl pattern)
     * 
     * @details Prediction error implementation details are hidden to reduce
     *          compilation dependencies and improve encapsulation. All operations
     *          are forwarded to the pImpl object.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
