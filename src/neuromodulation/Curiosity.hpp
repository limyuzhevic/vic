#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @class Curiosity
 * @brief Curiosity-driven exploration motivation system for the agent
 * 
 * The Curiosity system implements the agent's drive to explore novel stimuli and
 * discover new information. It computes an exploration motivation signal based on
 * the combination of novelty detection and prediction error, guiding the agent
 * toward informative experiences that maximize learning.
 * 
 * Curiosity serves several critical functions:
 * - Balances exploration vs. exploitation in reinforcement learning
 * - Promotes discovery of new patterns and environmental regularities
 * - Enhances adaptive behavior in dynamic and changing environments
 * - Supports lifelong learning by preventing premature convergence
 * 
 * The system computes curiosity as a weighted combination of:
 * - Novelty: how unexpected the current sensory input is
 * - Prediction error: difference between expected and actual outcomes
 * 
 * @note This class is part of Phase 1 implementation. Phase 2 will implement
 *       more sophisticated curiosity computations based on information gain
 *       and intrinsic motivation theories.
 * 
 * @see Novelty
 * @see PredictionError
 * @see Dopamine
 * @see Brain
 * @see AgentBrain
 */
class Curiosity {
public:
    /**
     * @brief Constructor: Creates a new Curiosity system instance
     * 
     * Initializes the curiosity system with default parameters and
     * prepares it for integration with the agent's brain and sensory systems.
     * The curiosity level starts at zero and will be updated based on
     * environmental inputs.
     * 
     * @post Curiosity system is initialized with default parameters
     * @note Default novelty and prediction error weights are both 0.5, providing
     *       balanced emphasis on both sources of exploration motivation.
     * @see initialize(class Brain*)
     * @see update(float, float, TimestepDuration)
     */
    Curiosity();
    
    /**
     * @brief Destructor: Cleans up the Curiosity system resources
     * 
     * Performs final cleanup of the curiosity system's internal state
     * and any allocated resources.
     * 
     * @post All curiosity system resources are released
     * @note Virtual destructor ensures proper cleanup through base class pointers.
     */
    ~Curiosity();
    
    /**
     * @brief Initialize the curiosity system with brain reference
     * 
     * Establishes the connection between the curiosity system and the agent's
     * neural brain. This allows the curiosity system to access neural activity
     * patterns and brain state information needed for adaptive curiosity computation.
     * 
     * @param brain Pointer to the Brain object that provides neural context
     *              and state information for curiosity computations
     * @post Brain reference is stored internally, curiosity system is ready
     *       to receive updates from sensory and reward systems
     * @note This method must be called before any curiosity updates or level queries.
     *       The brain pointer should remain valid for the lifetime of this object.
     * @see Curiosity()
     * @see update(float, float, TimestepDuration)
     * @see getLevel()
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get the current curiosity level indicating exploration motivation
     * 
     * Returns the current curiosity level, which represents the agent's drive
     * to explore novel stimuli and take exploratory actions. The level is
     * typically in the range [0.0, 1.0], where higher values indicate stronger
     * exploration motivation.
     * 
     * @return float Current curiosity level (0.0 = no exploration, 1.0 = maximum exploration)
     * @post None
     * @note Curiosity level is computed from novelty and prediction error inputs
     *       during the update() method. The level influences action selection and
     *       learning prioritization.
     * @see update(float, float, TimestepDuration)
     * @see getExplorationDrive()
     * @see enableCuriosity(bool)
     */
    float getLevel() const;
    
    /**
     * @brief Update the curiosity system based on novelty and prediction error
     * 
     * Computes the current curiosity level by combining novelty detection and
     * prediction error signals. This update is called whenever the agent
     * processes new sensory input or receives reward feedback.
     * 
     * The computation follows:
     * 
     *   curiosity = (novelty_weight × novelty) + (prediction_error_weight × error)
     *   where weights sum to 1.0 (typically both 0.5 for balanced emphasis)
     * 
     * @param novelty Current sensory novelty level (0.0-1.0)
     * @param predictionError Current prediction error signal (can be positive or negative)
     * @param dt Time step duration for the update (in seconds)
     * @post Curiosity level is updated based on novelty and prediction error
     * @note This method should be called after processing sensory input or receiving rewards.
     *       The time step parameter allows for decay of curiosity over time if needed.
     * @see getLevel()
     * @see setNoveltyWeight(float)
     * @see setPredictionErrorWeight(float)
     * @see processSensoryInput(const SensoryPercept&)
     */
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    /**
     * @brief Get the exploration drive, equivalent to curiosity level
     * 
     * This method provides an alternative way to access the curiosity level,
     * returning the same value as getLevel(). It is provided for semantic clarity
     * in contexts where "exploration drive" is a more appropriate term than
     * "curiosity level".
     * 
     * @return float Current exploration drive (0.0-1.0 range)
     * @post None
     * @note This is functionally identical to getLevel() but provides semantic
     *       clarity in documentation and code.
     * @see getLevel()
     * @see selectWithCuriosity(MotorCommand)
     */
    float getExplorationDrive() const;
    
    /**
     * @brief Set the weight for novelty contribution to curiosity computation
     * 
     * Adjusts the relative importance of novelty detection in the curiosity
     * computation. Higher weights give more emphasis to novel stimuli as
     * exploration motivators.
     * 
     * @param weight Novelty weight in range [0.0, 1.0]
     * @post Novelty weight is updated; curiosity computation uses new weight
     * @note The prediction error weight is adjusted automatically to maintain
     *       a total weight of 1.0 (weight + prediction_error_weight = 1.0).
     *       Changing these weights can tune the agent's exploration strategy.
     * @see setPredictionErrorWeight(float)
     * @see getLevel()
     * @see update(float, float, TimestepDuration)
     */
    void setNoveltyWeight(float weight);
    
    /**
     * @brief Set the weight for prediction error contribution to curiosity computation
     * 
     * Adjusts the relative importance of prediction error signals in the curiosity
     * computation. Higher weights give more emphasis to prediction errors as
     * exploration motivators.
     * 
     * @param weight Prediction error weight in range [0.0, 1.0]
     * @post Prediction error weight is updated; curiosity computation uses new weight
     * @note The novelty weight is adjusted automatically to maintain
     *       a total weight of 1.0 (prediction_error_weight + novelty_weight = 1.0).
     *       This allows fine-tuning of the agent's exploration strategy.
     * @see setNoveltyWeight(float)
     * @see getLevel()
     * @see update(float, float, TimestepDuration)
     */
    void setPredictionErrorWeight(float weight);
    
    /**
     * @brief Reset the curiosity system to initial state
     * 
     * Resets all internal state variables, curiosity level, and historical
     * data to their initial values. This prepares the curiosity system for
     * a new episode or testing scenario.
     * 
     * @post All state variables reset to initial values, curiosity level is zero
     * @note This method is useful for starting fresh learning episodes or
     *       for testing purposes. Configuration parameters (weights) are
     *       preserved across resets.
     * @see Curiosity()
     * @see update(float, float, TimestepDuration)
     * @see getLevel()
     */
    void reset();
    
private:
    /**
     * @brief Implementation details for the Curiosity system
     * 
     * Pimpl idiom implementation details for the Curiosity class. This structure
     * contains all the internal implementation details including:
     * - Neural brain reference for adaptive curiosity computation
     * - Historical data storage for novelty calculation
     * - Current state variables and parameters
     * - Internal computation buffers
     * 
     * @note Using the Pimpl idiom keeps the header file clean and allows
     *       for easier implementation changes without affecting the interface.
     * @see Curiosity()
     * @see ~Curiosity()
     * @see initialize(class Brain*)
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation details structure
     * 
     * Holds a pointer to the actual implementation of the Curiosity system.
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
