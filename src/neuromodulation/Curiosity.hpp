#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

/**
 * @class Curiosity
 * @brief Curiosity drive: exploration motivation based on novelty and prediction error
 * 
 * The Curiosity class implements the neural curiosity mechanism that drives exploration
 * and learning in the NLM brain. It computes curiosity as a weighted combination of:
 * - Novelty: Prediction error between expected and actual sensory input
 * - Prediction error: Difference between predicted and actual rewards
 * 
 * Curiosity plays a crucial role in the NLM architecture by:
 * - Balancing exploration vs. exploitation
 * - Promoting discovery of novel patterns and behaviors
 * - Driving learning in unfamiliar or changing environments
 * - Supporting adaptive behavior in dynamic contexts
 * 
 * The curiosity system is behaviorally motivated, encouraging agents to
 * explore their environment and gather information when faced with uncertainty
 * or when novelty is detected.
 */

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
     * @brief Initialize the curiosity system with brain reference
     * 
     * Establishes the connection to the parent brain, allowing the curiosity
     * system to access brain state and integrate with other systems.
     * 
     * @param brain Pointer to the parent brain instance
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get the current curiosity level
     * 
     * @return float Current curiosity level (typically 0.0-1.0)
     */
    float getLevel() const;
    
    /**
     * @brief Get the exploration drive
     * 
     * @return float Exploration drive (same as curiosity level)
     */
    float getExplorationDrive() const;
    
    /**
     * @brief Update curiosity based on novelty and prediction error
     * 
     * Computes and updates the curiosity level using a combination of novelty
     * and prediction error signals. The update follows an exponential moving
     * average approach to smooth changes over time.
     * 
     * @param novelty Novelty signal (0.0-1.0, higher = more novel)
     * @param predictionError Prediction error signal (can be positive or negative)
     * @param dt Time duration of the timestep
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    /**
     * @brief Set curiosity novelty weight
     * 
     * Configures how much novelty contributes to overall curiosity. Higher
     * weight means novelty has greater influence on exploration behavior.
     * 
     * @param weight Novelty weight (0.0-1.0)
     */
    void setNoveltyWeight(float weight);
    
    /**
     * @brief Set curiosity prediction error weight
     * 
     * Configures how much prediction error contributes to overall curiosity.
     * Higher weight means prediction error has greater influence on exploration.
     * 
     * @param weight Prediction error weight (0.0-1.0)
     */
    void setPredictionErrorWeight(float weight);
    
    /**
     * @brief Reset curiosity to initial state
     */
    void reset();
    
    /**
     * @brief Get the curiosity decay rate
     * 
     * @return float Curiosity decay rate per timestep
     */
    float getDecayRate() const;
    
    /**
     * @brief Set the curiosity decay rate
     * 
     * @param rate Decay rate per timestep (0.0-1.0)
     */
    void setDecayRate(float rate);
    
    /**
     * @brief Check if curiosity is within normal bounds
     * 
     * @param level Curiosity level to check
     * @return true if curiosity level is within valid bounds
     */
    bool isValidLevel(float level) const;
    
    /**
     * @brief Apply curiosity bonus to action selection
     * 
     * Applies a curiosity-driven bonus to action selection, encouraging
     * exploration of novel or uncertain states.
     * 
     * @param baseAction Base action to potentially modify
     * @param novelty Novelty signal
     * @param predictionError Prediction error signal
     * @param dt Time duration of the timestep
     * @return class ActionType Action type with curiosity bonus applied
     */
    class ActionType applyCuriosityBonus(class ActionType baseAction, float novelty, float predictionError, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

