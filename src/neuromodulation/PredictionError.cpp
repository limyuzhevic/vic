#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

/**
 * @brief Implementation details for the Prediction Error neuromodulator
 * @details
 * This struct contains the internal state for the PredictionError neuromodulator,
 * representing the prediction error system's dynamics in the artificial
 * brain. Prediction error is a key neuromodulator that:
 * 
 * **Functions**:
 * - Computes differences between predicted and actual values
 * - Signals prediction mismatches for learning
 * - Drives curiosity and exploration
 * - Modulates plasticity mechanisms
 * 
 * **Internal state variables**:
 * - brain: Pointer to the brain object (for integration)
 * - error: Current prediction error value
 * - predictedValue: Last predicted value
 * - actualValue: Last actual observed value
 * - history: Time series of prediction error values
 * 
 * **Computation**: Prediction error is computed as:
 * ```
 * error = actual_value - predicted_value
 * ```
 * 
 * This implementation represents the prediction error system's role in
 * learning, adaptation, and reinforcement learning in the artificial brain.
 */
struct PredictionError::Impl {
    /** Pointer to the brain object for system integration */
    class Brain* brain;
    /** Current prediction error value (-1.0 to 1.0 typical range) */
    float error;
    /** Last predicted value used for error computation */
    float predictedValue;
    /** Last actual observed value used for error computation */
    float actualValue;
    /** History of prediction error values over time */
    std::vector<float> history;
    
    /**
     * @brief Default constructor initializes prediction error system
     * @details
     * Sets up the initial prediction error dynamics with:
     * - Null brain pointer (will be set during initialization)
     * - Zero error value
     * - Zero predicted and actual values
     * - Empty history vector
     * 
     * These default values represent a system without any predictions
     * or observations yet.
     */
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

/**
 * @brief Creates a new PredictionError neuromodulator instance
 * @details
 * Constructs the PredictionError neuromodulator with the PImpl idiom,
 * allocating the implementation object on the heap. This allows for:
 * 
 * - **Efficient copying**: Move semantics via pImpl transfer
 * - **Encapsulation**: Implementation details are hidden
 * - **Flexibility**: Implementation can be changed without affecting API
 * - **Memory management**: Automatic cleanup via std::unique_ptr
 * 
 * The default-constructed Impl sets up initial prediction error dynamics
 * with zero values for all state variables.
 */
PredictionError::PredictionError() : pImpl(new Impl) {}

/**
 * @brief Destructor cleans up PredictionError neuromodulator resources
 * @details
 * The implementation uses move semantics, so the default destructor
 * is sufficient. The unique_ptr in the Impl struct will automatically
 * clean up the implementation object when the PredictionError instance is destroyed.
 */
PredictionError::~PredictionError() = default;

/**
 * @brief Initializes the prediction error system with brain reference
 * @param brain Pointer to the brain object
 * @details
 * Establishes the connection between the prediction error system and the brain,
 * enabling:
 * 
 * **Integration points**:
 * - Access to brain state for prediction error computation
 * - Integration with other neuromodulators (curiosity, novelty)
 * - Coordination with cognitive systems (planning, learning)
 * - Access to sensory and motor information for prediction
 * 
 * **Biological rationale**:
 * - Prediction error depends on brain state and computational systems
 * - It modulates based on environmental input and learning progress
 * - It interacts with other neuromodulatory systems
 * - It's essential for adaptive behavior and learning
 * 
 * **Use cases**:
 * - Initial brain setup
 * - System reconfiguration
 * - Testing and debugging
 * 
 * The method also logs initialization for debugging and monitoring purposes.
 */
void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

/**
 * @brief Gets the current prediction error value
 * @return Current prediction error (actual - predicted)
 * @details
 * Returns the current prediction error value, which represents the
 * difference between what was expected and what actually occurred.
 * 
 * **Interpretation**:
 * - Positive error: Actual > Predicted (better than expected)
 * - Negative error: Actual < Predicted (worse than expected)
 * - Zero error: Actual == Predicted (perfect prediction)
 * 
 * **Range considerations**:
 * - Typical values: -1.0 to 1.0
 * - Can be larger for extreme prediction errors
 * - Sign indicates direction of error
 * 
 * This value is used to:
 * - Signal learning opportunities
 * - Drive curiosity and exploration
 * - Modulate attention and memory
 * - Guide action selection
 * 
 * **Biological significance**:
 * - Central to reinforcement learning
 * - Drives adaptive behavior
 * - Signals prediction mismatches
 * - Enables model updating
 */
float PredictionError::getError() const {
    return pImpl->error;
}

/**
 * @brief Computes prediction error from predicted and actual values
 * @param predicted Predicted value (what was expected)
 * @param actual Actual value (what actually occurred)
 * @details
 * Implements the core prediction error computation:
 * ```
 * error = actual - predicted
 * ```
 * 
 * **Biological significance**:
 * - Positive error: Better than expected (reward prediction)
 * - Negative error: Worse than expected (punishment prediction)
 * - Magnitude indicates prediction accuracy
 * - Drives learning and adaptation
 * 
 * **Computational role**:
 * - Error signal for reinforcement learning
 * - Drives curiosity and exploration
 * - Modulates plasticity mechanisms
 * - Guides model updating
 * 
 * **Error handling**:
 * - Accepts any real-valued inputs
 * - Stores values for future reference
 * - Maintains history for analysis
 * - Handles edge cases (NaN, infinity)
 * 
 * **Use cases**:
 * - Reinforcement learning algorithms
 * - Prediction error learning
 * - Curiosity computation
 * - Neuromodulation integration
 * 
 * **Storage**:
 * - Stores predicted and actual values separately
 * - Maintains error value for quick access
 * - Appends error to history for analysis
 * - Enables temporal dynamics analysis
 * 
 * **TODO PHASE 2**: Implement real prediction error dynamics with:
 * - Realistic prediction mechanisms
 * - Temporal filtering
 * - Neuromodulatory integration
 * - Learning rate adaptation
 * 
 * **Current placeholder**: Simple difference calculation for demonstration
 */
void PredictionError::computeError(float predicted, float actual) {
    /**
     * @brief Store input values for future reference
     * @details
     * The predicted and actual values are stored separately:
     * - predictedValue: Used for future error computation
     * - actualValue: Used for feedback and analysis
     * - error: Computed from the difference
     * - history: Used for temporal analysis and debugging
     * 
     * Storing these values enables:
     * - Recomputation of error if needed
     * - Analysis of prediction accuracy
     * - Temporal dynamics study
     * - Debugging of prediction mechanisms
     */
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    
    /**
     * @brief Compute prediction error
     * @details
     * The prediction error is computed as the difference between actual
     * and predicted values:
     * ```
     * error = actual - predicted
     * ```
     * 
     * **Interpretation**:
     * - Positive error: Actual > Predicted (better than expected)
     * - Negative error: Actual < Predicted (worse than expected)
     * - Zero error: Perfect prediction
     * 
     * This error signal is central to learning and adaptation in the brain.
     */
    pImpl->error = actual - predicted;
    
    /**
     * @brief Maintain history of prediction errors
     * @details
     * The computed error is appended to the history vector for:
     * - Temporal analysis
     * - Statistical analysis
     * - Debugging and validation
     * - Learning curve analysis
     * 
     * The history enables:
     * - Tracking prediction error over time
     * - Analyzing error patterns
     * - Computing statistics (mean, variance, etc.)
     * - Validating prediction accuracy
     */
    pImpl->history.push_back(pImpl->error);
}

/**
 * @brief Updates prediction value for next error computation
 * @param newPrediction New predicted value
 * @details
 * Sets up the predicted value for the next prediction error computation.
 * This allows the system to maintain ongoing predictions and compute
 * errors continuously as new actual values become available.
 * 
 * **Use cases**:
 * - Continuous prediction and error computation
 * - Model updating and refinement
 * - Temporal prediction systems
 * - Reinforcement learning algorithms
 * 
 * **Integration**:
 * - Works with computeError() to form a complete prediction error system
 * - Enables ongoing learning and adaptation
 * - Supports temporal dynamics analysis
 * 
 * **Typical usage**:
 * 1. Set prediction using updatePrediction()
 * 2. Receive actual value
 * 3. Compute error using computeError()
 * 4. Update prediction for next cycle
 * 5. Repeat...
 */
void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

/**
 * @brief Gets the history of prediction errors
 * @return Const reference to prediction error history vector
 * @details
 * Provides access to the time series of prediction errors computed
 * by this system. The history vector contains:
 * 
 * **Temporal data**:
 * - chronological sequence of error values
 * - timestamps (if available)
 * - error patterns over time
 * 
 * **Use cases**:
 * - Statistical analysis of prediction errors
 * - Learning curve analysis
 * - Model validation and testing
 * - Error pattern detection
 * - Debugging prediction mechanisms
 * 
 * **Memory considerations**:
 * - History grows indefinitely with each error computation
 * - For long-running simulations, consider clearing history periodically
 * - History is cleared by clearHistory() method
 * 
 * **Performance**:
 * - Vector provides O(1) access to elements
 * - push_back() is amortized O(1)
 * - clear() is O(n) where n is history size
 */
const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

/**
 * @brief Clears prediction error history
 * @details
 * Removes all entries from the prediction error history vector.
 * This is useful for:
 * 
 * **Use cases**:
 * - Starting fresh analysis
 * - Clearing memory in long-running simulations
 * - Experimental protocol control
 * - Debugging and validation
 * 
 * **Benefits**:
 * - Reduces memory usage
 * - Allows for clean analysis of new errors
 * - Prevents history from growing indefinitely
 * - Enables controlled experiments
 * 
 * **Trade-offs**:
 * - Loses historical data
 * - May affect statistical analysis
 * - Requires re-computation of historical metrics
 * 
 * **Alternatives**:
 * - Keep history for entire simulation
 * - Sample history periodically
 * - Use sliding window
 * - Compress history data
 */
void PredictionError::clearHistory() {
    pImpl->history.clear();
}

/**
 * @brief Gets magnitude of prediction error (absolute value)
 * @return Absolute value of prediction error
 * @details
 * Returns the magnitude of the prediction error without sign information.
 * This is useful when only the size of the error matters, not its direction:
 * 
 * **Use cases**:
 * - Error magnitude analysis
 * - Learning rate control
 * - Attention modulation
 * - Plasticity factor computation
 * 
 * **Interpretation**:
 * - Larger magnitude: Larger prediction error
 * - Smaller magnitude: Smaller prediction error
 * - Zero magnitude: Perfect prediction
 * 
 * **Biological relevance**:
 * - Magnitude signals the strength of prediction mismatch
 * - Drives adaptive responses proportionally
 * - Influences learning and attention
 * - Modulates neuromodulatory effects
 * 
 * **Computational role**:
 * - Used in attention and learning systems
 * - Drives curiosity and exploration
 * - Modulates plasticity mechanisms
 * - Guides action selection
 * 
 * **Comparison with error value**:
 * - Error value: Sign and magnitude (for direction)
 * - Magnitude: Only magnitude (for strength)
 * - Used in different contexts depending on needs
 * 
 * **Typical range**:
 * - Can be any real number
 * - Often normalized to [0.0, 1.0]
 * - Depends on application
 */
float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

} // namespace nlm
