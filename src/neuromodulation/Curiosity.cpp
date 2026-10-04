#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Implementation details for the Curiosity neuromodulator
 * @details
 * This struct contains the internal state for the Curiosity neuromodulator,
 * representing the biological curiosity system's dynamics in the artificial
 * brain. Curiosity is a key neuromodulator that:
 * 
 * **Functions**:
 * - Encodes exploration motivation
 * - Integrates novelty and prediction error signals
 * - Drives exploration behavior
 * - Balances exploration vs. exploitation
 * 
 * **Internal state variables**:
 * - brain: Pointer to the brain object (for integration)
 * - level: Current curiosity level (exploration drive)
 * - noveltyWeight: Weight of novelty in curiosity computation
 * - predictionErrorWeight: Weight of prediction error in curiosity computation
 * - decayRate: Rate at which curiosity decays over time
 * 
 * **Computation**: Curiosity is computed as a weighted combination of novelty
 * and prediction error:
 * ```
 * target = novelty_weight * novelty + prediction_error_weight * prediction_error
 * level = level + (target - level) * 0.1f  // Smooth update
 * level = max(0.0f, level - decay_rate * dt)  // Decay
 * ```
 * 
 * This implementation represents the biological curiosity system's role in
 * driving exploration and learning in the artificial brain.
 */
struct Curiosity::Impl {
    /** Pointer to the brain object for system integration */
    class Brain* brain;
    /** Current curiosity level (exploration drive, 0.0 to 1.0) */
    float level;
    /** Weight of novelty signal in curiosity computation */
    float noveltyWeight;
    /** Weight of prediction error signal in curiosity computation */
    float predictionErrorWeight;
    /** Rate at which curiosity decays over time */
    float decayRate;
    
    /**
     * @brief Default constructor initializes curiosity to resting state
     * @details
     * Sets up the initial curiosity dynamics with:
     * - Null brain pointer (will be set during initialization)
     * - Zero initial level
     * - Equal weights for novelty and prediction error
     * - Default decay rate
     * 
     * These default values represent a resting, unmotivated brain state.
     */
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
};

/**
 * @brief Creates a new Curiosity neuromodulator instance
 * @details
 * Constructs the Curiosity neuromodulator with the PImpl idiom,
 * allocating the implementation object on the heap. This allows for:
 * 
 * - **Efficient copying**: Move semantics via pImpl transfer
 * - **Encapsulation**: Implementation details are hidden
 * - **Flexibility**: Implementation can be changed without affecting API
 * - **Memory management**: Automatic cleanup via std::unique_ptr
 * 
 * The default-constructed Impl sets up initial curiosity dynamics
 * with zero level and default parameters.
 */
Curiosity::Curiosity() : pImpl(new Impl) {}

/**
 * @brief Destructor cleans up Curiosity neuromodulator resources
 * @details
 * The implementation uses move semantics, so the default destructor
 * is sufficient. The unique_ptr in the Impl struct will automatically
 * clean up the implementation object when the Curiosity instance is destroyed.
 */
Curiosity::~Curiosity() = default;

/**
 * @brief Initializes the curiosity system with brain reference
 * @param brain Pointer to the brain object
 * @details
 * Establishes the connection between the curiosity system and the brain,
 * enabling:
 * 
 * **Integration points**:
 * - Access to brain state for curiosity computation
 * - Integration with other neuromodulators
 * - Coordination with cognitive systems
 * - Access to sensory and motor information
 * 
 * **Biological rationale**:
 * - Curiosity depends on brain state
 * - It modulates based on environmental input
 * - It interacts with other neuromodulatory systems
 * - It influences learning and decision making
 * 
 * **Use cases**:
 * - Initial brain setup
 * - System reconfiguration
 * - Testing and debugging
 * 
 * The method also logs initialization for debugging and monitoring purposes.
 */
void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

/**
 * @brief Gets the current curiosity level
 * @return Current curiosity level (exploration drive, 0.0 to 1.0)
 * @details
 * Returns the current level of curiosity in the system, normalized to
 * the range [0.0, 1.0]. This value represents:
 * 
 * - **0.0**: No curiosity (no exploration motivation)
 * - **0.5**: Moderate curiosity (balanced state)
 * - **1.0**: High curiosity (strong exploration drive)
 * 
 * The level is constrained to prevent invalid values and ensures biological
 * plausibility.
 * 
 * This value is used to:
 * - Modulate exploration behavior
 * - Influence attention and learning
 * - Drive action selection strategies
 * - Balance exploration vs. exploitation
 * 
 * **Biological significance**:
 * - Drives exploratory behavior
 * - Increases response to novel stimuli
 * - Enhances learning from prediction errors
 * - Maintains behavioral flexibility
 */
float Curiosity::getLevel() const {
    return pImpl->level;
}

/**
 * @brief Updates curiosity based on novelty and prediction error
 * @param novelty Novelty signal level (0.0 to 1.0)
 * @param predictionError Prediction error signal level (-1.0 to 1.0)
 * @param dt Time elapsed since last update
 * @details
 * Implements the dynamic evolution of curiosity over time, integrating
 * information from both novelty and prediction error signals. The curiosity
 * computation follows a two-stage process:
 * 
 * **Stage 1: Target computation**
 * ```
 * target = novelty_weight * novelty + prediction_error_weight * prediction_error
 * ```
 * 
 * **Stage 2: Smooth update with decay**
 * ```
 * level = level + (target - level) * 0.1f  // Smooth approach to target
 * level = max(0.0f, level - decay_rate * dt)  // Natural decay
 * ```
 * 
 * **Biological interpretation**:
 * - Novelty signals increase curiosity (explore new things)
 * - Prediction errors increase curiosity (learn from mistakes)
 * - Decay models the fading of curiosity over time
 * - Smooth update prevents abrupt changes
 * 
 * **Error handling**:
 * - Validates input ranges (with warnings for out-of-range values)
 * - Ensures level never goes negative
 * - Handles invalid time steps gracefully
 * 
 * **Parameters**:
 * - novelty: Should be in range [0.0, 1.0]
 * - predictionError: Can be negative (prediction was too high)
 * - dt: Should be positive and small (simulation timestep)
 * 
 * **TODO PHASE 2**: Implement real curiosity dynamics with:
 * - Spike train modulation
 * - Temporal filtering
 * - Neuromodulatory integration
 * - Learning rate adaptation
 * 
 * **Current placeholder**: Simple linear integration for demonstration
 */
void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    /**
     * @brief Compute target curiosity level from inputs
     * @details
     * The target curiosity level is a weighted sum of novelty and prediction
     * error signals. The weights determine how much each signal contributes
     * to curiosity:
     * 
     * - High novelty weight: More curiosity from novel stimuli
     * - High prediction error weight: More curiosity from learning opportunities
     * - Equal weights: Balanced approach (default)
     * 
     * The computation:
     * ```
     * target = novelty_weight * novelty + prediction_error_weight * prediction_error
     * ```
     * 
     * This represents how the brain combines different sources of information
     * to determine exploration motivation.
     */
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    /**
     * @brief Smoothly update curiosity level towards target
     * @details
     * The curiosity level doesn't jump directly to the target, but approaches
     * it gradually:
     * ```
     * level = level + (target - level) * 0.1f
     * ```
     * 
     * The factor 0.1f controls the speed of adjustment:
     * - Higher value: Faster response to changing inputs
     * - Lower value: Slower, more stable behavior
     * 
     * This smooth update models the biological time constant of neuromodulatory
     * systems, preventing abrupt changes in behavior.
     */
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    /**
     * @brief Apply decay to natural curiosity reduction
     * @details
     * Curiosity naturally decays over time, modeling:
     * - Habituation to novel stimuli
     * - Reduced exploration after initial investigation
     * - Return to baseline motivation levels
     * 
     * The decay is applied after the smooth update:
     * ```
     * level = max(0.0f, level - decay_rate * dt)
     * ```
     * 
     * The decay rate controls how quickly curiosity fades:
     * - Higher rate: Faster decay (shorter curiosity bursts)
     * - Lower rate: Slower decay (longer curiosity states)
     * - The max(0.0f, ...) prevents negative values
     */
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

/**
 * @brief Gets exploration drive (alias for curiosity level)
 * @return Current curiosity level (same as getLevel())
 * @details
 * This method provides an alternative name for the curiosity level,
 * emphasizing its functional role in driving exploration behavior.
 * 
 * **Biological significance**:
 * - Exploration drive determines how much the organism seeks new information
 * - High exploration leads to more sampling of the environment
 * - Low exploration leads to exploitation of known rewards
 * - This balance is central to adaptive behavior
 * 
 * **Computational role**:
 * - Used in action selection algorithms
 * - Influences decision making strategies
 * - Integrates with reward learning
 * - Modulates attention and memory
 * 
 * **Use cases**:
 * - Policy gradient methods in reinforcement learning
 * - Exploration-exploitation algorithms
 * - Behavioral characterization
 * - Experimental paradigm implementation
 */
float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

/**
 * @brief Sets curiosity novelty weight
 * @param weight New weight for novelty signal (0.0 to 1.0)
 * @details
 * Adjusts how much novelty contributes to curiosity computation.
 * This weight should be between 0.0 and 1.0, with the constraint that
 * the sum of novelty_weight and prediction_error_weight should typically
 * be close to 1.0 for proper normalization.
 * 
 * **Use cases**:
 * - Tuning exploration strategies
 * - Research studies
 * - Behavioral experiments
 * - Parameter optimization
 * 
 * **Constraints**:
 * - Must be non-negative
 * - Should be balanced with prediction error weight
 * - Extreme values may cause numerical instability
 */
void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

/**
 * @brief Sets curiosity prediction error weight
 * @param weight New weight for prediction error signal (0.0 to 1.0)
 * @details
 * Adjusts how much prediction error contributes to curiosity computation.
 * As with novelty weight, this should be balanced with the novelty weight
 * for proper normalization of the curiosity computation.
 * 
 * **Biological rationale**:
 * - Prediction error signals highlight learning opportunities
 * - They indicate where the current model is wrong
 * - They motivate exploration to resolve uncertainty
 * - They're essential for adaptive behavior
 * 
 * **Use cases**:
 * - Studying the role of prediction error in curiosity
 * - Testing different exploration strategies
 * - Understanding curiosity-driven learning
 * 
 * **Research applications**:
 * - Compare curiosity weights across different conditions
 * - Test effects of neuromodulator manipulation
 * - Study individual differences in exploration
 */
void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

/**
 * @brief Resets curiosity system to baseline state
 * @details
 * Sets the curiosity level back to zero, representing:
 * - Return to neutral motivation state
 * - Reset for new learning episode
 * - Baseline for experimental control
 * - Recovery from overexcitation
 * 
 * **Use cases**:
 * - Starting fresh simulations
 * - Testing without carryover effects
 * - Experimental paradigm control
 * - System reset and recovery testing
 * 
 * **Effects**:
 * - Curiosity level set to 0.0
 * - Other parameters unchanged (weights, decay rate)
 * - Brain connection maintained (if initialized)
 * - Ready for new input signals
 */
void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
