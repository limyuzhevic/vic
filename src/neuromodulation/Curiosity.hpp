#pragma once

/**
 * @file Curiosity.hpp
 * @brief Curiosity system for exploration motivation and learning
 * 
 * This header defines the Curiosity class for the brain's exploration motivation system.
 * The curiosity system drives exploration behavior by computing novelty and prediction
 * error signals, motivating agents to seek informative experiences and learn about
 * their environment. It provides the foundation for intrinsic motivation and
 * curiosity-driven learning.
 * 
 * Curiosity Theory:
 * - Intrinsic motivation: Curiosity drives exploration independent of external rewards
 * - Information seeking: Curiosity maximizes learning from novel experiences
 * - Exploration-exploitation: Curiosity balances known rewards vs. new information
 * - Prediction error: Curiosity leverages prediction errors for learning
 * - Novelty detection: Curiosity identifies informative and unexpected experiences
 * 
 * Curiosity Functions:
 * - Novelty detection: Identifies unexpected or information-rich events
 * - Prediction error computation: Leverages prediction errors for learning
 * - Exploration drive: Motivates exploration based on information value
 * - Reward modulation: Integrates with reward system for balanced learning
 * - Attention allocation: Directs attention toward novel and informative stimuli
 * 
 * Integration Points:
 * - Connected to prediction error system for error computation
 * - Modulates reward system for exploration rewards
 * - Influences attention system for directed exploration
 * - Affects planning system for exploratory action selection
 * - Coordinates with neuromodulation systems (dopamine, curiosity)
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
class Curiosity;
class Brain;
class Neuromodulator;
class PredictionError;

/**
 * @brief Curiosity drive system for exploration motivation
 * 
 * Implements the brain's curiosity system for exploration motivation and
 * intrinsic learning. The curiosity system computes exploration drive based
 * on novelty and prediction error signals, motivating agents to seek
 * informative experiences and learn about their environment. It provides
 * the foundation for curiosity-driven learning and intrinsic motivation.
 * 
 * Curiosity System Architecture:
 * 1. Novelty Detection: Identifies unexpected or information-rich events
 * 2. Prediction Error Integration: Combines prediction errors with novelty
 * 3. Exploration Drive: Computes motivation for exploration
 * 4. Reward Modulation: Provides exploration-based rewards
 * 5. Attention Guidance: Directs attention toward novel stimuli
 * 
 * Curiosity Processing Pipeline:
 * 1. Sensory Input -> Novelty Detection -> Prediction Error Integration
 * 2. Integration -> Exploration Drive Computation -> Reward Modulation
 * 3. Reward Modulation -> Exploration Behavior -> Learning
 * 
 * Curiosity System Properties:
 * - Novelty sensitivity: Responds to unexpected or informative events
 * - Information value: Maximizes learning from novel experiences
 * - Exploration balance: Balances exploration vs. exploitation
 * - Temporal discounting: Handles time-based exploration costs
 * - Context sensitivity: Adapts exploration to environmental context
 * 
 * Exploration Dynamics:
 * - Information gain: Maximizes new knowledge acquisition
 * - Uncertainty reduction: Reduces uncertainty about environment
 * - Skill development: Explores to develop new capabilities
 * - Environmental mapping: Explores to understand environment structure
 * - Social learning: Explores to learn from other agents
 * 
 * Thread Safety: Curiosity system is thread-safe for concurrent access
 * from different brain components. Novelty detection and exploration
 * drive computation are protected with mutexes.
 * 
 * Memory Management: Curiosity system uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note Curiosity system is essential for exploration motivation and
 *       intrinsic learning. Works in conjunction with extrinsic reward
 *       systems for balanced learning.
 * @see Neuromodulator for neuromodulatory curiosity processing
 * @see PredictionError for prediction error computation
 * @see Reward for reward-based motivation
 */
class Curiosity {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a curiosity system with initial state. The curiosity implementation
     * uses the pImpl pattern for efficient memory usage and to reduce
     * compilation dependencies.
     */
    Curiosity();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of Curiosity's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~Curiosity();
    
    /**
     * @brief Initialize curiosity system with brain reference
     * 
     * Initializes the curiosity system with reference to the brain,
     * enabling access to brain's state, goals, and other systems for
     * context-aware curiosity computation.
     * 
     * @param brain Pointer to the brain that contains this curiosity system
     * 
     * @note Thread-safe with mutex protection.
     *       This method should be called after brain construction but
     *       before curiosity system is used for exploration.
     * 
     * @see initialize()
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get current curiosity level
     * 
     * Returns the current exploration drive computed by the curiosity system.
     * The curiosity level represents the agent's motivation to explore,
     * typically ranging from 0.0 (no exploration) to 1.0 (maximum exploration).
     * 
     * @return Current curiosity level (0.0 to 1.0 typical range)
     * 
     * @note Curiosity level is dynamically computed based on novelty,
     *       prediction errors, and environmental context.
     *       It represents the agent's intrinsic motivation for exploration.
     */
    float getLevel() const;
    
    /**
     * @brief Update curiosity based on novelty and prediction error
     * 
     * Updates the curiosity system's exploration drive based on computed
     * novelty and prediction error signals. This is the primary method for
     * dynamic curiosity computation.
     * 
     * Curiosity Update Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Calculate novelty based on current state and history
     * 3. Integrate prediction error with novelty signal
     * 4. Apply temporal discounting to both signals
     * 5. Compute exploration drive as weighted combination
     * 6. Apply bounds and normalization
     * 
     * @param novelty Novelty of current experience (0.0-1.0)
     * @param predictionError Prediction error from prediction system
     * @param dt Time step duration
     * 
     * @note Thread-safe with mutex protection.
     *       Novelty and prediction error are combined to compute
     *       exploration motivation. Both signals contribute to
     *       intrinsic motivation for exploration.
     * 
     * @see getLevel()
     * @see initialize()
     * @see Novelty
     * @see PredictionError
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    /**
     * @brief Get exploration drive (same as level)
     * 
     * Returns the current exploration drive computed by the curiosity system.
     * This is an alias for getLevel() provided for API convenience.
     * 
     * @return Current exploration drive (0.0 to 1.0 typical range)
     * 
     * @see getLevel()
     */
    float getExplorationDrive() const;
    
    /**
     * @brief Set curiosity parameters
     * 
     * Configures internal parameters for curiosity computation, such as
     * novelty sensitivity, prediction error weight, or exploration costs.
     * 
     * @param noveltyWeight Weight for novelty signal (0.0-1.0)
     * @param predictionErrorWeight Weight for prediction error signal (0.0-1.0)
     * @param explorationCost Cost of exploration behavior
     * @param noveltyThreshold Threshold for novelty detection
     * 
     * @note Thread-safe with mutex protection.
     *       These parameters control curiosity computation dynamics and
     *       should be set before curiosity system is used for exploration.
     */
    void setParameters(float noveltyWeight, float predictionErrorWeight, 
                       float explorationCost, float noveltyThreshold);
    
    /**
     * @brief Get current curiosity parameters
     * 
     * @return Vector containing current curiosity parameters
     *         [noveltyWeight, predictionErrorWeight, explorationCost, noveltyThreshold]
     * 
     * @note Parameters control curiosity computation and exploration dynamics.
     */
    std::vector<float> getParameters() const;
    
    /**
     * @brief Set novelty weight
     * 
     * Sets the weight for novelty in curiosity computation.
     * 
     * @param weight Novelty weight (0.0-1.0)
     */
    void setNoveltyWeight(float weight);
    
    /**
     * @brief Set prediction error weight
     * 
     * Sets the weight for prediction error in curiosity computation.
     * 
     * @param weight Prediction error weight (0.0-1.0)
     */
    void setPredictionErrorWeight(float weight);
    
    /**
     * @brief Get current developmental stage multiplier
     * 
     * Returns how the current developmental stage affects curiosity processing.
     * Different developmental stages have different curiosity sensitivities
     * and exploration patterns.
     * 
     * @return Developmental stage multiplier for curiosity processing
     * 
     * @note Developmental stage influences curiosity sensitivity, exploration
     *       patterns, and information-seeking behavior.
     *       Younger stages may have higher exploration drive.
     * 
     * @see DevelopmentSystem
     */
    float getDevelopmentalStageMultiplier() const;
    
    /**
     * @brief Reset curiosity system
     * 
     * Resets the curiosity system to initial state. This clears novelty history
     * and resets exploration drive to baseline levels.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the curiosity system but preserves
     *       configuration parameters.
     */
    void reset();
    
    /**
     * @brief Compute novelty of current experience
     * 
     * Computes the novelty of the current experience based on brain's
     * current state and stored experiences. Novelty represents how
     * unexpected or information-rich the current experience is.
     * 
     * @param brain Pointer to brain containing state and experiences
     * @return Novelty value (0.0-1.0, higher = more novel)
     * 
     * @note Novelty computation typically involves comparing current state
     *       with stored experiences or predicted state. Novelty is a key
     *       driver of curiosity and exploration motivation.
     */
    static float computeNovelty(class Brain* brain);
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     * 
     * Hidden implementation for Curiosity system.
     * Stores curiosity state including exploration drive, novelty history,
     * prediction error history, and parameters.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm