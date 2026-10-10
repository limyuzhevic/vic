#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @file Curiosity.hpp
 * 
 * Curiosity system for NLM
 * 
 * This file provides the curiosity system for NLM, which implements exploration
 * motivation based on novelty and prediction error. Curiosity is essential for:
 * - Exploration of novel environments
 * - Discovery of new information
 * - Learning through exploration
 * - Adaptive behavior
 * - Behavioral flexibility
 * 
 * The curiosity system in NLM:
 * - **Exploration motivation**: Drives exploration of novel stimuli
 * - **Novelty-based**: Responds to novelty and prediction error
 * - **Learning-driven**: Motivates learning through exploration
 * - **Adaptive**: Adapts to environmental challenges
 * - **Flexible**: Enables flexible and adaptive behavior
 * 
 * Key features:
 * - **Exploration drive**: Motivates exploration of novel stimuli
 * - **Novelty-based**: Responds to novelty and prediction error
 * - **Learning-driven**: Motivates learning through exploration
 * - **Adaptive**: Adapts to environmental challenges
 * - **Flexible**: Enables flexible and adaptive behavior
 * 
 * @note Essential for exploration and learning
 * @note Used extensively in motivation and behavior
 * @note Critical for adaptive behavior
 * @see Config.hpp - Configuration options for curiosity
 * @see Brain.hpp - Integration with brain architecture
 * @see Novelty.hpp - For novelty detection
 * @see PredictionError.hpp - For prediction error
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class Curiosity
 * 
 * Curiosity drive: exploration motivation based on novelty and prediction error.
 * 
 * This class implements curiosity as a system that generates exploration motivation
 * based on novelty and prediction error. It is essential for:
 * - Exploration of novel environments
 * - Discovery of new information
 * - Learning through exploration
 * - Adaptive behavior
 * - Behavioral flexibility
 * 
 * Curiosity mechanisms in NLM:
 * - **Exploration motivation**: Drives exploration of novel stimuli
 * - **Novelty-based**: Responds to novelty and prediction error
 * - **Learning-driven**: Motivates learning through exploration
 * - **Adaptive**: Adapts to environmental challenges
 * - **Flexible**: Enables flexible and adaptive behavior
 * 
 * Key characteristics of curiosity in NLM:
 * - **Exploration**: Essential for exploration of novel stimuli
 * - **Motivation**: Critical for motivation and behavior
 * - **Learning**: Essential for learning through exploration
 * - **Adaptation**: Critical for adaptive behavior
 * - **Flexibility**: Essential for behavioral flexibility
 * 
 * @note Essential for exploration and learning
 * @note Used extensively in motivation and behavior
 * @note Critical for adaptive behavior
 * @see Config.hpp - Configuration options for curiosity
 * @see Brain.hpp - Integration with brain architecture
 * @see Novelty.hpp - For novelty detection
 * @see PredictionError.hpp - For prediction error
 * 
 * @author NLM Development Team
 */
class Curiosity {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new curiosity system instance with default parameters.
     * The curiosity system is initialized with default settings for
     * computing curiosity based on novelty and prediction error.
     */
    Curiosity();
    
    /**
     * @brief Destructor
     * 
     * Cleans up curiosity system resources and performs any necessary
     * cleanup operations. The curiosity system is left in a valid but
     * non-operational state after destruction.
     */
    ~Curiosity();
    
    // Disable copying - PImpl idiom
    Curiosity(const Curiosity&) = delete;
    Curiosity& operator=(const Curiosity&) = delete;
    Curiosity(Curiosity&&) noexcept;
    Curiosity& operator=(Curiosity&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the curiosity system with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get curiosity level
     * 
     * Returns the current curiosity level, which reflects the exploration
     * motivation based on novelty and prediction error. This is used by
     * neural systems to modulate their behavior based on curiosity.
     * 
     * @return Current curiosity level
     * 
     * @post Return value fluctuates during neural computation
     * @complexity O(1)
     */
    float getLevel() const;
    
    /**
     * @brief Update curiosity based on novelty and prediction error
     * 
     * Updates the curiosity system based on current novelty and prediction
     * error levels. This computes curiosity as a weighted combination of
     * novelty and prediction error for:
     * - Computing curiosity motivation
     * - Updating exploration drive
     * - Adapting to environmental challenges
     * - Enabling flexible behavior
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    /**
     * @brief Get exploration drive
     * 
     * Returns the exploration drive, which is the same as the curiosity level
     * and represents the motivation to explore novel stimuli. This is used
     * by neural systems to modulate their exploration behavior.
     * 
     * @return Exploration drive
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     */
    float getExplorationDrive() const;
    
    /**
     * @brief Set curiosity parameters
     * 
     * Sets the curiosity parameters that control the weighting of novelty
     * and prediction error in curiosity computation. This is used for:
     * - Configuring curiosity behavior
     * - Experimental control
     * - Tuning curiosity parameters
     * - Testing curiosity dynamics
     */
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    /**
     * @brief Reset curiosity
     * 
     * Resets the curiosity system to its initial state, clearing all internal
     * state and preparing for new curiosity computation. This is used for:
     * - Resetting curiosity system
     * - Managing curiosity resources
     * - Error recovery
     * - Curiosity reset
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
