#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @file Novelty.hpp
 * 
 * Novelty detection system for NLM
 * 
 * This file provides the novelty detection system for NLM, which computes
 * novelty from comparison with previous observations. Novelty detection is
 * essential for:
 * - Learning new information
 * - Detecting changes and differences
 * - Encouraging exploration and discovery
 * - Updating cognitive models
 * - Adapting to new environments
 * 
 * The novelty detection system in NLM:
 * - **Computes novelty**: Calculates novelty from previous observations
 * - **Supports learning**: Detects new and different information
 * - **Encourages exploration**: Promotes exploration of novel stimuli
 * - **Updates models**: Refines internal models based on novelty
 * - **Adapts**: Adapts to environmental changes
 * 
 * Key features:
 * - **Difference computation**: Compares current with previous observations
 * - **Learning support**: Facilitates learning of new information
 * - **Exploration promotion**: Encourages novel stimulus exploration
 * - **Model updating**: Updates internal cognitive models
 * - **Adaptation**: Enables adaptation to new environments
 * 
 * @note Essential for learning and adaptation
 * @note Used extensively in cognitive processing
 * @note Critical for novelty detection
 * @see Config.hpp - Configuration options for novelty detection
 * @see Brain.hpp - Integration with brain architecture
 * @see Curiosity.hpp - For curiosity detection
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class Novelty
 * 
 * Novelty detection signal computing novelty from previous observations.
 * 
 * This class implements novelty detection by computing differences between
 * current and previous observations. It is essential for:
 * - Learning new information
 * - Detecting changes and differences
 * - Encouraging exploration and discovery
 * - Updating cognitive models
 * - Adapting to new environments
 * 
 * Novelty detection mechanisms in NLM:
 * - **Difference computation**: Compares current with previous observations
 * - **Learning support**: Detects new and different information
 * - **Exploration promotion**: Encourages novel stimulus exploration
 * - **Model updating**: Refines internal models based on novelty
 * - **Adaptation**: Enables adaptation to new environments
 * 
 * Key characteristics of novelty in NLM:
 * - **Difference**: Measures difference from previous observations
 * - **Learning**: Essential for learning new information
 * - **Exploration**: Promotes exploration of novel stimuli
 * - **Adaptation**: Enables adaptation to new environments
 * - **Discovery**: Supports discovery of new patterns
 * 
 * @note Essential for learning and adaptation
 * @note Used extensively in cognitive processing
 * @note Critical for novelty detection
 * @see Config.hpp - Configuration options for novelty detection
 * @see Brain.hpp - Integration with brain architecture
 * @see Curiosity.hpp - For curiosity detection
 * 
 * @author NLM Development Team
 */
class Novelty {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new novelty detection instance with default parameters.
     * The novelty detection is initialized with default settings for
     * computing novelty from observations.
     */
    Novelty();
    
    /**
     * @brief Destructor
     * 
     * Cleans up novelty detection resources and performs any necessary
     * cleanup operations. The novelty detection is left in a valid but
     * non-operational state after destruction.
     */
    ~Novelty();
    
    // Disable copying - PImpl idiom
    Novelty(const Novelty&) = delete;
    Novelty& operator=(const Novelty&) = delete;
    Novelty(Novelty&&) noexcept;
    Novelty& operator=(Novelty&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the novelty detection system with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get novelty level
     * 
     * Returns the current novelty level, which reflects the degree of
     * novelty in the current observation compared to previous observations.
     * This is used by neural systems to modulate their behavior based on
     * novelty information.
     * 
     * @return Current novelty level
     * 
     * @post Return value fluctuates during neural computation
     * @complexity O(1)
     */
    float getLevel() const;
    
    /**
     * @brief Set novelty level
     * 
     * Sets the novelty level, which affects neural processing. This is
     * used for:
     * - Controlling novelty signaling
     * - Implementing novelty detection
     * - Regulating neural plasticity
     * - Modulating learning rates
     */
    void setLevel(float level);
    
    /**
     * @brief Detect novelty from observation
     * 
     * Detects novelty from an observation compared to a previous observation.
     * This computes the degree of difference between two observations to
     * determine how novel the current observation is. This is essential for:
     * - Computing novelty from observations
     * - Learning new information
     * - Detecting changes and differences
     * - Updating cognitive models
     */
    void detectNovelty(const class Observation& observation,
                       const class Observation& previousObservation);
    
    /**
     * @brief Detect novelty from sensory input pattern
     * 
     * Detects novelty from sensory input patterns compared to previous patterns.
     * This is an alternative to observation-based novelty detection that works
     * directly with sensory input patterns for:
     * - Efficient novelty computation
     * - Real-time novelty detection
     * - Pattern-based novelty detection
     * - Continuous novelty monitoring
     */
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    /**
     * @brief Decay novelty over time
     * 
     * Decays novelty over time, reducing the novelty level based on time
     * elapsed since novelty detection. This is essential for:
     * - Implementing temporal decay of novelty
     * - Managing novelty dynamics
     * - Updating novelty levels
     * - Controlling novelty persistence
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Get novelty history
     * 
     * Returns the novelty history, containing novelty levels over time.
     * This is used for:
     * - Analyzing novelty patterns
     * - Monitoring novelty trends
     * - Novelty history analysis
     * - Novelty trend tracking
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear novelty history
     * 
     * Clears the novelty history, removing all recorded novelty levels.
     * This is useful for:
     * - Resetting novelty history
     * - Managing novelty storage
     * - Novelty history reset
     * - Error recovery
     */
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
