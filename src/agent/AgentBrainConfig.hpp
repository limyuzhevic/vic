#pragma once

#include <cstddef>
#include <limits>

namespace nlm {

/**
 * @brief Configuration constants for AgentBrain and related components.
 * 
 * This header file centralizes all magic numbers and configuration parameters
 * used throughout the AgentBrain class and its dependencies. Using this
 * configuration header improves code maintainability, allows for easy
 * tuning of behavioral parameters, and provides a single point of change
 * for all configuration-related settings.
 * 
 * @note All constants are in logical units (e.g., fractions, rates, dimensions)
 * @warning Changing these constants may affect the behavior of the neural
 *          simulation. Use with care and test thoroughly.
 * @author NLM Development Team
 * @date Phase 6: Final Integration
 */

struct AgentBrainConfig {
    // ========================================================================
    // Motor and Sensory Neuron Grouping Configuration
    // ========================================================================
    
    /**
     * @brief Size of motor neuron groups (Forward, Backward, Left, Right, Interact, Wait)
     * 
     * Defines how many neurons are distributed to each motor action group.
     * Lower values may reduce computational complexity but could limit fine-grained control.
     */
    static constexpr size_t MOTOR_GROUP_SIZE = 6;
    
    /**
     * @brief Distribution group sizes for sensory neurons
     * 
     * These values determine how sensory neurons are distributed among different
     * sensory modalities. Adjust based on the expected input sizes from sensors.
     */
    static constexpr size_t SENSOR_GROUP_SIZE_VISION = 4;
    static constexpr size_t SENSOR_GROUP_SIZE_TOUCH = 8;
    static constexpr size_t SENSOR_GROUP_SIZE_INTERNAL = 4;
    static constexpr size_t SENSOR_GROUP_SIZE_PROPRIO = 6;
    
    // ========================================================================
    // Sensory Input Configuration
    // ========================================================================
    
    /**
     * @brief Vision sensor configuration
     * 
     * The vision system uses a fixed 16x16 grayscale grid for processing,
     * which provides a balanced view resolution for the agent's navigation
     * and interaction capabilities.
     */
    static constexpr size_t VISION_WIDTH = 16;
    static constexpr size_t VISION_HEIGHT = 16;
    static constexpr size_t VISION_TOTAL = VISION_WIDTH * VISION_HEIGHT;  ///< Total vision input size
    
    /**
     * @brief Touch sensor configuration
     * 
     * Binary collision sensors covering the agent's body surface. The value of
     * 8 represents a reasonable balance between coverage and computational cost.
     */
    static constexpr size_t TOUCH_TOTAL = 8;
    
    /**
     * @brief Internal homeostatic signals configuration
     * 
     * Represents internal physiological states like hunger, fatigue, etc.
     * 4 signals provide sufficient dimensionality for basic homeostatic regulation.
     */
    static constexpr size_t INTERNAL_TOTAL = 4;
    
    /**
     * @brief Proprioception sensor configuration
     * 
     * Body position and velocity signals. 6 degrees of freedom for a simple
     * agent embodiment (position x,y,z and rotation euler angles).
     */
    static constexpr size_t PROPRIOCEPTION_TOTAL = 6;
    
    // ========================================================================
    // Motor Command Configuration
    // ========================================================================
    
    /**
     * @brief Total number of distinct motor commands
     * 
     * Includes basic movement commands, looking commands, interaction,
     * and waiting. This value is used for random exploration generation.
     */
    static constexpr size_t MOTOR_COMMANDS_TOTAL = 8;  // MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait
    
    // ========================================================================
    // Neuromodulation Thresholds
    // ========================================================================
    
    /**
     * @brief Motor activity threshold for action selection
     * 
     * The minimum neural activity level required to trigger a motor command.
     * Values below this threshold result in the agent staying idle.
     */
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    /**
     * @brief Low curiosity threshold for exploration behavior
     * 
     * When curiosity falls below this level, the agent reduces exploratory
     * behavior and focuses on goal-directed actions.
     */
    static constexpr float CURIOSITY_LOW_THRESHOLD = 0.3f;
    
    /**
     * @brief High curiosity threshold for exploration behavior
     * 
     * When curiosity exceeds this level, the agent engages in significant
     * exploration behavior, occasionally selecting random actions.
     */
    static constexpr float CURIOSITY_HIGH_THRESHOLD = 0.5f;
    
    /**
     * @brief Maximum exploration chance when curiosity is high
     * 
     * Determines the proportion of random actions taken during exploration.
     * A value of 0.3 means up to 30% of actions may be random at maximum curiosity.
     */
    static constexpr float CURIOSITY_EXPLORATION_RATE = 0.3f;
    
    // ========================================================================
    // Novelty and Curiosity Configuration
    // ========================================================================
    
    /**
     * @brief Novelty signal decay rate per timestep
     * 
     * Controls how quickly novelty signals decay over time. Values close to 1.0
     * result in slow decay, while values closer to 0.0 result in rapid decay.
     * 0.99 provides a balance between novelty retention and adaptation.
     */
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    
    /**
     * @brief Weight of novelty in curiosity calculation
     * 
     * Determines how much sensory novelty contributes to overall curiosity.
     * Higher values make curiosity more sensitive to novel sensory input.
     */
    static constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;
    
    /**
     * @brief Weight of prediction error in curiosity calculation
     * 
     * Determines how much prediction error contributes to overall curiosity.
     * Higher values make curiosity more sensitive to learning opportunities.
     */
    static constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
    
    // ========================================================================
    // Reward Modulation Configuration
    // ========================================================================
    
    /**
     * @brief Exponential decay factor for expected reward
     * 
     * Used in the moving average calculation for expected reward. Values closer
     * to 1.0 give more weight to past expectations, values closer to 0.0 give
     * more weight to recent rewards.
     */
    static constexpr float REWARD_EXPONENTIAL_DECAY = 0.95f;
    
    /**
     * @brief Exponential increment factor for expected reward
     * 
     * Complementary to the decay factor, used in the moving average calculation.
     * Sum of REWARD_EXPONENTIAL_DECAY and REWARD_EXPONENTIAL_INCREMENT should be 1.0.
     */
    static constexpr float REWARD_EXPONENTIAL_INCREMENT = 0.05f;
    
    /**
     * @brief Minimum valid dopamine level
     * 
     * Dopamine signals are clamped between these bounds to prevent numerical
     * instability and ensure biologically plausible ranges.
     */
    static constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    
    /**
     * @brief Maximum valid dopamine level
     * 
     * Dopamine signals are clamped between these bounds to prevent numerical
     * instability and ensure biologically plausible ranges.
     */
    static constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    
    // ========================================================================
    // Plasticity Configuration
    // ========================================================================
    
    /**
     * @brief Minimum allowed plasticity factor
     * 
     * Prevents plasticity from dropping to zero or negative values, which would
     * eliminate learning capability. 0.1 represents approximately 10% of maximum plasticity.
     */
    static constexpr float PLASTICITY_MIN = 0.1f;
    
    /**
     * @brief Maximum allowed plasticity factor
     * 
     * Represents the theoretical maximum plasticity rate. Used as a scaling factor
     * in various plasticity calculations.
     */
    static constexpr float PLASTICITY_MAX = 2.0f;
    
    /**
     * @brief Base plasticity factor
     * 
     * The baseline plasticity level used in calculations. Represents normal
     * learning capacity before modulation by neuromodulators.
     */
    static constexpr float PLASTICITY_BASE = 0.5f;
    
    /**
     * @brief Dopamine modulation coefficient for plasticity
     * 
     * Determines how strongly dopamine affects plasticity. Positive values
     * increase plasticity with positive dopamine (reward prediction), negative
     * values decrease plasticity with negative dopamine (punishment prediction).
     */
    static constexpr float PLASTICITY_DOPAMINE_COEFF = 0.5f;
    
    /**
     * @brief Base learning rate for Long-Term Potentiation (LTP)
     * 
     * The baseline rate at which synaptic strengthening occurs following
     * pre-post spike pairing. 0.01 represents approximately 1% weight change.
     */
    static constexpr float LTP_BASE = 0.01f;
    
    /**
     * @brief Base learning rate for Long-Term Depression (LTD)
     * 
     * The baseline rate at which synaptic weakening occurs following
     * post-pre spike pairing. 0.012 represents approximately 1.2% weight change.
     */
    static constexpr float LTD_BASE = 0.012f;
    
    // ========================================================================
    // Development Configuration
    // ========================================================================
    
    /**
     * @brief Duration of initial developmental stage (in simulation time)
     * 
     * Represents the period of high plasticity where rapid learning and development
     * occur. 60.0 simulation units (~1 minute real-time at standard speed).
     */
    static constexpr double DEVELOPMENTAL_INITIAL = 60.0;
    
    /**
     * @brief Duration of critical period developmental stage
     * 
     * Period of enhanced sensitivity to environmental input. 300.0 simulation
     * units (~5 minutes real-time) allows for significant early development.
     */
    static constexpr double DEVELOPMENTAL_CRITICAL = 300.0;
    
    /**
     * @brief Duration of maturation developmental stage
     * 
     * Period of continued development and refinement. 900.0 simulation
     * units (~15 minutes real-time) provides time for more complex learning.
     */
    static constexpr double DEVELOPMENTAL_MATURATION = 900.0;
    
    // ========================================================================
    // Structural Plasticity Configuration
    // ========================================================================
    
    /**
     * @brief Base rate of synaptogenesis during development
     * 
     * Controls how quickly new synapses form. Higher rates in early development
     * allow for rapid initial connectivity establishment.
     */
    static constexpr float SYNAPTOGENESIS_BASE = 0.0001f;
    
    /**
     * @brief Base rate of synaptic pruning during development
     * 
     * Controls how quickly unused synapses are eliminated. Pruning increases
     * with decreasing plasticity (as the system matures).
     */
    static constexpr float PRUNING_BASE = 0.00001f;
    
    // ========================================================================
    // Sensory Input Scaling Configuration
    // ========================================================================
    
    /**
     * @brief Scale factor for vision input current injection
     * 
     * Converts vision intensity values (0.0-1.0) to injected current. Higher
     * values increase sensitivity to visual input.
     */
    static constexpr float VISION_SCALE_FACTOR = 5.0f;
    
    /**
     * @brief Scale factor for touch input current injection
     * 
     * Converts touch signals (0.0-1.0) to injected current for collision
     * detection. Higher values increase sensitivity to touch.
     */
    static constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    
    /**
     * @brief Center value for internal signal scaling
     * 
     * Used to center internal homeostatic signals around zero before scaling.
     * 1.0 represents the midpoint of typical internal signal ranges.
     */
    static constexpr float INTERNAL_SCALE_CENTER = 1.0f;
    
    /**
     * @brief Scale factor for internal signal current injection
     * 
     * Converts internal signals to injected current after centering. Controls
     * how strongly internal states influence neural activity.
     */
    static constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    
    /**
     * @brief Center value for proprioception signal scaling
     * 
     * Used to center proprioception signals around zero before scaling.
     * 1.0 represents the midpoint of typical proprioception signal ranges.
     */
    static constexpr float PROPRIOCEPTION_SCALE_CENTER = 1.0f;
    
    /**
     * @brief Scale factor for proprioception signal current injection
     * 
     * Converts proprioception signals to injected current after centering.
     * Controls how strongly body position influences neural activity.
     */
    static constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    
    // ========================================================================
    // Numerical Limits and Constants
    // ========================================================================
    
    /**
     * @brief Minimum valid value for division operations
     * 
     * Used to prevent division by zero in normalization calculations. Should
     * be a small positive value that represents the smallest meaningful difference.
     */
    static constexpr size_t MIN_DIVISION_VALUE = 1;
    
    /**
     * @brief Maximum valid value for eligibility traces
     * 
     * Used to determine when eligibility traces become negligible. Values below
     * this threshold are considered effectively zero.
     */
    static constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    
    /**
     * @brief Minimum meaningful motor activity
     * 
     * Used in MotorCommand selection to determine when an action is strong enough
     * to be executed. Values below this threshold result in the agent waiting.
     */
    static constexpr float MIN_MOTOR_ACTIVITY = 0.5f;
    
    /**
     * @brief Minimum exploration chance
     * 
     * Ensures exploration never drops to exactly zero, maintaining some level of
     * behavioral variability even at low curiosity levels.
     */
    static constexpr float MIN_EXPLORATION_CHANCE = 0.05f;
    
    /**
     * @brief Maximum plasticity reduction factor
     * 
     * Represents the maximum reduction in plasticity during development,
     * going from full plasticity (1.0) to adult stability (0.2).
     */
    static constexpr float PLASTICITY_REDUCTION_FACTOR = 0.8f;
    
    /**
     * @brief Maximum structural plasticity rate reduction
     * 
     * Represents how much structural plasticity rates decrease from initial
     * development to adult stability.
     */
    static constexpr float STRUCTURAL_PLASTICITY_REDUCTION = 0.9f;
};

} // namespace nlm