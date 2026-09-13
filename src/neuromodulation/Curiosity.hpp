#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error
//
// Implementation:
// - Exponential moving average of novelty and prediction error inputs
// - Decay over time to prevent unbounded growth
// - Returns exploration drive level between 0.0 and 1.0
//
// Configuration:
// - noveltyWeight: Weight for novelty contribution (default: 0.5)
// - predictionErrorWeight: Weight for prediction error contribution (default: 0.5)
// - decayRate: Rate of decay (default: 0.05 per timestep)
class Curiosity {
public:
    /**
     * @brief Construct a new Curiosity object
     */
    Curiosity();
    
    /**
     * @brief Destroy the Curiosity object
     */
    ~Curiosity();
    
    /**
     * @brief Initialize the curiosity system with a brain reference
     * 
     * @param brain Pointer to the brain object (must not be null)
     * @note This should be called after the brain is initialized
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get the current curiosity level
     * 
     * @return float Current curiosity level (0.0 to 1.0)
     */
    float getLevel() const;
    
    /**
     * @brief Update curiosity based on novelty and prediction error
     * 
     * @param novelty Novelty level (0.0 to 1.0)
     * @param predictionError Prediction error (negative if actual < predicted)
     * @param dt Timestep duration
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    /**
     * @brief Get exploration drive (same as level)
     * 
     * @return float Exploration drive level
     */
    float getExplorationDrive() const;
    
    /**
     * @brief Set novelty weight parameter
     * 
     * @param weight New novelty weight (typically 0.0 to 1.0)
     */
    void setNoveltyWeight(float weight);
    
    /**
     * @brief Set prediction error weight parameter
     * 
     * @param weight New prediction error weight (typically 0.0 to 1.0)
     */
    void setPredictionErrorWeight(float weight);
    
    /**
     * @brief Reset curiosity to zero level
     */
    void reset();
    
private:
    /**
     * @brief Pimpl pattern implementation details
     */
    struct Impl;
    
    /**
     * @brief Pointer to implementation
     */
    Impl* pImpl;
};

} // namespace nlm
