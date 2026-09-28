#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @class Curiosity
 * @brief Curiosity drive system - exploration motivation based on novelty and prediction error
 * 
 * The curiosity system motivates the agent to explore novel or surprising aspects of the environment.
 * It computes a drive signal based on the weighted combination of novelty detection and 
 * prediction error signals, encouraging exploration when regular sensory patterns become
 * predictable.
 * 
 * @details
 * - Novelty: Measure of how unexpected or new the current sensory input is
 * - Prediction error: Difference between expected and actual sensory input
 * - Drive: Higher levels lead to increased exploration behavior
 * - Exploration: Exploration vs. exploitation trade-off in reinforcement learning
 * 
 * @note Phase 1 implementation uses simplified dynamics. Phase 2 will implement
 *       more biologically plausible models of curiosity based on information gain
 *       and intrinsic motivation theories.
 */
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    /**
     * @brief Initialize curiosity system with brain reference
     * 
     * @param brain Pointer to the brain instance for accessing neural systems
     * 
     * @note This method should be called after brain creation to establish the
     *       connection between curiosity drive and brain operations.
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get current curiosity level
     * 
     * @return Curiosity level between 0.0 (no curiosity) and 1.0 (maximum curiosity)
     * 
     * @note The level is updated based on novelty and prediction error signals
     *       during the update() method calls.
     */
    float getLevel() const;
    
    /**
     * @brief Update curiosity level based on environmental inputs
     * 
     * @param novelty Novelty signal (0.0 = familiar, 1.0 = highly novel)
     * @param predictionError Prediction error signal (0.0 = perfect prediction, 
     *                        higher = larger prediction error)
     * @param dt Timestep duration in seconds
     * 
     * @details The curiosity level is computed as a weighted sum of novelty and
     *         prediction error signals, with smooth temporal dynamics to prevent
     *         rapid fluctuations that could lead to unstable exploration behavior.
     * 
     * @note Phase 2 improvements may include:
     *       - Different time constants for different input types
     *       - Adaptive weighting based on learning progress
     *       - Integration with other neuromodulatory systems
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    /**
     * @brief Get exploration drive (alias for getLevel())
     * 
     * @return Exploration drive signal (same as curiosity level)
     * 
     * @note This method provides a more intuitive name for the same underlying
     *       signal when used in action selection contexts.
     */
    float getExplorationDrive() const;
    
    /**
     * @brief Set novelty weight in curiosity computation
     * 
     * @param weight Weight for novelty signal in [0.0, 1.0]
     * 
     * @note Should sum with prediction error weight to 1.0 for proper normalization
     */
    void setNoveltyWeight(float weight);
    
    /**
     * @brief Set prediction error weight in curiosity computation
     * 
     * @param weight Weight for prediction error signal in [0.0, 1.0]
     * 
     * @note Should sum with novelty weight to 1.0 for proper normalization
     */
    void setPredictionErrorWeight(float weight);
    
    /**
     * @brief Reset curiosity system to initial state
     * 
     * @note This method is typically called when starting a new episode or
     *       when curiosity should be reset to baseline levels.
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
