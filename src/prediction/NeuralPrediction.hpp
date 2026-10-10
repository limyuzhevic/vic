#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <functional>

namespace nlm {

/**
 * @file NeuralPrediction.hpp
 * 
 * Neural prediction implementations for NLM
 * 
 * This file provides neural implementations of prediction systems in NLM.
 * These implementations use actual neural dynamics and mechanisms to perform:
 * - Temporal sequence learning and prediction
 * - Action-consequence association and prediction
 * - Prediction error computation and signaling
 * - Neural pattern completion and similarity matching
 * - Predictive coding and forward modeling
 * 
 * The neural prediction system in NLM:
 * - **NeuralPrediction**: Implements temporal and action-consequence prediction
 * - **ActionConsequencePredictor**: Learns action-consequence associations
 * - **PredictionErrorSignal**: Computes and signals prediction error
 * - **Integration**: Connects prediction with neural plasticity
 * - **Learning**: Updates predictions based on prediction error
 * 
 * Key features:
 * - **Temporal prediction**: Predicts future sensory states
 * - **Action prediction**: Predicts consequences of actions
 * - **Pattern completion**: Retrieves similar neural patterns
 * - **Prediction error**: Computes and signals prediction errors
 * - **Neural learning**: Uses neural plasticity for learning
 * 
 * @note These are neural implementations of prediction systems
 * @see Config.hpp - Configuration options for prediction
 * @see Brain.hpp - Integration with brain architecture
 * @see PredictionSystem.hpp - High-level prediction interface
 * @see Novelty.hpp - For novelty detection
 * @see PredictionError.hpp - For prediction error
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class NeuralPrediction
 * 
 * Neural implementation of temporal sequence prediction.
 * 
 * This class implements neural temporal sequence prediction using actual
 * neural dynamics. Instead of using symbolic representations, it stores
 * temporal sequences as neural activity patterns and uses neural dynamics
 * to predict future states. This provides:
 * - **Temporal learning**: Learns temporal sequences from sensory observations
 * - **Prediction**: Predicts future sensory states based on learned sequences
 * - **Pattern completion**: Completes patterns based on neural similarity
 * - **Adaptation**: Adapts predictions based on prediction errors
 * - **Multi-step prediction**: Predicts multiple steps into the future
 * 
 * NeuralPrediction mechanisms:
 * - **Temporal sequence learning**: Learns sequences from sensory observations
 * - **Pattern matching**: Matches sensory patterns for prediction
 * - **Prediction generation**: Generates predictions based on learned sequences
 * - **Prediction error computation**: Computes prediction errors
 * - **Adaptation**: Updates predictions based on errors
 * 
 * Key characteristics of neural prediction in NLM:
 * - **Neural representation**: Sequences stored as neural patterns
 * - **Temporal dynamics**: Learns temporal dependencies
 * - **Pattern completion**: Retrieves related patterns
 * - **Prediction error**: Computes prediction errors for learning
 * - **Adaptation**: Updates predictions based on feedback
 * 
 * @note This is the neural implementation of temporal prediction
 * @see PredictionSystem.hpp - High-level prediction interface
 * @see ActionConsequencePredictor - For action-consequence prediction
 * @see PredictionErrorSignal - For prediction error signaling
 * @see Config.hpp - Configuration parameters
 * 
 * @author NLM Development Team
 */
class NeuralPrediction {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new neural prediction instance with default parameters.
     * The neural prediction is initialized with default settings for
     * temporal sequence learning and prediction.
     */
    NeuralPrediction();
    
    /**
     * @brief Destructor
     * 
     * Cleans up neural prediction resources and performs any necessary
     * cleanup operations. The neural prediction is left in a valid but
     * non-operational state after destruction.
     */
    ~NeuralPrediction();
    
    // Disable copying - PImpl idiom
    NeuralPrediction(const NeuralPrediction&) = delete;
    NeuralPrediction& operator=(const NeuralPrediction&) = delete;
    NeuralPrediction(NeuralPrediction&&) noexcept;
    NeuralPrediction& operator=(NeuralPrediction&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the neural prediction system with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record current sensory state for future prediction
     * 
     * Records the current sensory state for use in learning and predicting
     * future temporal sequences. This is essential for:
     * - Building temporal sequence models
     * - Learning temporal dependencies
     * - Predicting future states
     * - Temporal pattern recognition
     */
    void recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep);
    
    /**
     * @brief Generate prediction for next timestep
     * 
     * Generates a prediction for the next timestep based on learned temporal
     * sequences. This uses neural pattern completion to retrieve the most
     * similar stored sequence. This is essential for:
     * - Predicting future sensory states
     * - Temporal sequence completion
     * - Pattern-based prediction
     * - Neural prediction
     */
    std::vector<float> generatePrediction(SimulationStep currentStep);
    
    /**
     * @brief Update predictions based on actual observed state
     * 
     * Updates predictions based on the difference between predicted and actual
     * sensory states. This computes prediction error and modulates learning
     * based on prediction accuracy. This is essential for:
     * - Learning from prediction errors
     * - Updating prediction models
     * - Improving prediction accuracy
     * - Adaptive behavior
     */
    float updateWithObservation(const std::vector<float>& actualState,
                               SimulationStep currentStep);
    
    /**
     * @brief Predict consequences of a potential action
     * 
     * Predicts the consequences of taking a specific action in a given state.
     * This uses learned action-consequence associations to predict outcomes
     * and supports:
     * - Action planning
     * - Goal-directed behavior
     * - Reinforcement learning
     * - Action selection
     * 
     * @param action Action to predict consequences for
     * @param currentState Current sensory state
     * @return Predicted sensory state after action
     */
    std::vector<float> predictActionConsequence(ActionType action,
                                              const std::vector<float>& currentState);
    
    /**
     * @brief Get current prediction error
     * 
     * Returns the current prediction error, which represents the difference
     * between predicted and actual sensory states. This is used by neural
     * systems to modulate their behavior based on prediction errors.
     */
    float getPredictionError() const;
    
    /**
     * @brief Get prediction confidence
     * 
     * Returns the current prediction confidence, which represents the
     * certainty of the prediction based on consistency of learned patterns.
     * Higher confidence indicates more certain predictions.
     */
    float getPredictionConfidence() const;
    
    /**
     * @brief Multi-step prediction
     * 
     * Generates predictions for multiple steps into the future. This is used
     * for:
     * - Planning for the future
     * - Long-term prediction
     * - Sequential planning
     * - Multi-step action planning
     */
    std::vector<std::vector<float>> predictMultipleSteps(SimulationStep currentStep,
                                                        size_t numSteps);
    
    /**
     * @brief Record action that was taken
     * 
     * Records that a specific action was taken at a specific step for
     * action-consequence learning. This is essential for:
     * - Building action-consequence associations
     * - Learning action effects
     * - Action-sequence learning
     * - Action consequence learning
     */
    void recordAction(ActionType action, SimulationStep step);
    
    /**
     * @brief Get prediction history
     * 
     * Returns the prediction history, containing prediction errors over time.
     * This is used for:
     * - Analyzing prediction performance
     * - Monitoring prediction accuracy
     * - Learning rate adaptation
     * - Prediction analysis
     */
    const std::deque<float>& getErrorHistory() const;
    
    /**
     * @brief Clear prediction history
     * 
     * Clears the prediction history, removing all recorded prediction errors.
     * This is used for:
     * - Resetting prediction system
     * - Managing prediction resources
     * - Error history management
     * - Error recovery
     */
    void clearHistory();
    
    /**
     * @brief Enable/disable temporal prediction
     * 
     * Enables or disables temporal prediction functionality. This is used
     * for:
     * - Controlling prediction behavior
     * - Experimental control
     * - Tuning prediction parameters
     * - Testing prediction dynamics
     */
    void enableTemporalPrediction(bool enable) { temporalPredictionEnabled_ = enable; }
    
    /**
     * @brief Enable/disable action-consequence prediction
     * 
     * Enables or disables action-consequence prediction functionality.
     * This is used for:
     * - Controlling action-consequence behavior
     * - Experimental control
     * - Tuning action-consequence parameters
     * - Testing action-consequence dynamics
     */
    void enableActionConsequencePrediction(bool enable) { actionConsequenceEnabled_ = enable; }
    
    /**
     * @brief Set sequence memory size
     * 
     * Sets the size of the sequence memory, which determines how many
     * temporal sequences can be stored. This is used for:
     * - Configuring memory limits
     * - Experimental control
     * - Tuning memory parameters
     * - Testing memory dynamics
     */
    void setSequenceMemorySize(size_t size) { sequenceMemorySize_ = size; }
    
    /**
     * @brief Set prediction horizon
     * 
     * Sets the prediction horizon, which determines how many steps into
     * the future predictions can be generated. This is used for:
     * - Configuring prediction capabilities
     * - Experimental control
     * - Tuning prediction parameters
     * - Testing prediction dynamics
     */
    void setPredictionHorizon(size_t steps) { predictionHorizon_ = steps; }
    
    /**
     * @brief Get neurons involved in prediction
     * 
     * Returns neurons involved in prediction, which are used for neural
     * prediction and pattern completion.
     */
    std::vector<NeuronId> getPredictionNeurons() const;
    
    /**
     * @brief Get neurons involved in sequence tracking
     * 
     * Returns neurons involved in sequence tracking, which are used for
     * temporal sequence learning and prediction.
     */
    std::vector<NeuronId> getSequenceNeurons() const;
    
private:
    /**
     * @brief Learn temporal sequence from sensory observations
     * 
     * Learns temporal sequences from sensory observations. This is used for:
     * - Building temporal sequence models
     * - Learning temporal dependencies
     * - Temporal pattern recognition
     * - Sequence learning
     */
    void learnTemporalSequence(const std::vector<float>& currentState,
                               const std::vector<float>& nextState,
                               SimulationStep currentStep);
    
    /**
     * @brief Find or create neurons that respond to specific sensory pattern
     * 
     * Finds or creates neurons that respond to specific sensory patterns
     * for pattern matching and prediction. This is used for:
     * - Pattern storage
     * - Pattern matching
     * - Neural pattern recognition
     * - Neural pattern completion
     */
    NeuronId findMatchingPatternNeuron(const std::vector<float>& pattern,
                                      float similarityThreshold = 0.8f);
    
    /**
     * @brief Create association between pattern neuron and predicted pattern
     * 
     * Creates an association between pattern neurons for sequence prediction.
     * This is used for:
     * - Building sequence associations
     * - Pattern completion
     * - Neural connectivity
     * - Prediction learning
     */
    void createSequenceAssociation(NeuronId from, NeuronId to, float strength);
    
    /**
     * @brief Compute neural representation similarity
     * 
     * Computes the similarity between two neural patterns. This is used for:
     * - Pattern matching
     * - Pattern comparison
     * - Association formation
     * - Pattern similarity
     */
    float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;
    
    /**
     * @brief Strengthen synapses for successful predictions, weaken for errors
     * 
     * Modulates prediction-related synapses based on prediction accuracy.
     * This is used for:
     * - Learning from predictions
     * - Updating prediction models
     * - Improving prediction accuracy
     * - Adaptive learning
     */
    void modulatePredictionSynapses(float error, float reward);
    
    // Structure
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration
    size_t sequenceMemorySize_;
    size_t predictionHorizon_;
    bool temporalPredictionEnabled_;
    bool actionConsequenceEnabled_;
    
    // State
    Brain* brain_;
    float predictionError_;
    float predictionConfidence_;
    std::deque<float> errorHistory_;
    
    // Internal state
    std::vector<std::vector<float>> recentSensoryStates_;
    std::deque<SimulationStep> stateTimestamps_;
    std::vector<NeuronId> predictionNeurons_;
    std::vector<NeuronId> sequenceNeurons_;
    
    // Action-consequence tracking
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions_;
};

/**
 * @class ActionConsequencePredictor
 * 
 * Neural implementation of action-consequence prediction.
 * 
 * This class implements neural action-consequence prediction using actual
 * neural dynamics. Instead of using symbolic representations, it stores
 * action-consequence associations as neural patterns and uses neural dynamics
 * to predict consequences of actions. This provides:
 * - **Action learning**: Learns action-consequence mappings
 * - **Prediction**: Predicts consequences of actions
 * - **Learning**: Learns action outcomes from experience
 * - **Planning**: Plans actions based on expected outcomes
 * - **Adaptation**: Adapts action predictions based on feedback
 * 
 * ActionConsequencePredictor mechanisms:
 * - **Association learning**: Learns action-consequence associations
 * - **Prediction**: Predicts consequences of actions
 * - **Learning**: Learns action outcomes from experience
 * - **Adaptation**: Adapts action predictions based on feedback
 * - **Quality**: Evaluates action quality based on expected rewards
 * 
 * Key characteristics of action-consequence prediction in NLM:
 * - **Neural representation**: Associations stored as neural patterns
 * - **Action learning**: Learns action-consequence mappings
 * - **Outcome prediction**: Predicts action consequences
 * - **Learning evaluation**: Evaluates action quality
 * - **Adaptation**: Adapts action predictions based on feedback
 * 
 * @note This is the neural implementation of action-consequence prediction
 * @see NeuralPrediction - For temporal prediction
 * @see PredictionErrorSignal - For prediction error signaling
 * @see Config.hpp - Configuration parameters
 * 
 * @author NLM Development Team
 */
class ActionConsequencePredictor {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new action-consequence predictor instance with default parameters.
     * The action-consequence predictor is initialized with default settings for
     * learning action-consequence associations.
     */
    ActionConsequencePredictor();
    
    /**
     * @brief Destructor
     * 
     * Cleans up action-consequence predictor resources and performs any necessary
     * cleanup operations. The action-consequence predictor is left in a valid but
     * non-operational state after destruction.
     */
    ~ActionConsequencePredictor();
    
    // Disable copying - PImpl idiom
    ActionConsequencePredictor(const ActionConsequencePredictor&) = delete;
    ActionConsequencePredictor& operator=(const ActionConsequencePredictor&) = delete;
    ActionConsequencePredictor(ActionConsequencePredictor&&) noexcept;
    ActionConsequencePredictor& operator=(ActionConsequencePredictor&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the action-consequence predictor with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Record that taking an action in a state led to a specific consequence
     * 
     * Records that taking a specific action in a given state led to a specific
     * consequence and reward. This is essential for:
     * - Learning action-consequence associations
     * - Building action quality estimates
     * - Action consequence learning
     * - Reinforcement learning
     */
    void recordExperience(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward);
    
    /**
     * @brief Predict consequence of taking an action
     * 
     * Predicts the consequence of taking a specific action in a given state.
     * This is essential for:
     * - Action planning
     * - Prediction-based decision making
     * - Action selection
     * - Reinforcement learning
     */
    std::vector<float> predictConsequence(ActionType action,
                                         const std::vector<float>& currentState);
    
    /**
     * @brief Get prediction confidence for an action
     * 
     * Returns the prediction confidence for a specific action in a given state.
     * This is essential for:
     * - Action planning
     * - Decision making
     * - Action selection
     * - Risk assessment
     */
    float getConsequenceConfidence(ActionType action,
                                  const std::vector<float>& currentState) const;
    
    /**
     * @brief Update based on actual observed consequence
     * 
     * Updates action predictions based on actual observed consequences.
     * This is essential for:
     * - Learning from action outcomes
     * - Updating action predictions
     * - Improving action quality
     * - Action learning
     */
    void updatePrediction(ActionType action,
                         const std::vector<float>& predicted,
                         const std::vector<float>& actual,
                         float reward);
    
    /**
     * @brief Get learned action quality
     * 
     * Returns the learned action quality (average expected reward) for a
     * specific action. This is used for:
     * - Action evaluation
     * - Action selection
     * - Reinforcement learning
     * - Action planning
     */
    float getActionQuality(ActionType action) const;
    
    /**
     * @brief Clear learned associations
     * 
     * Clears all learned action-consequence associations. This is used for:
     * - Resetting action-consequence predictor
     * - Managing association resources
     * - Error recovery
     * - Association management
     */
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class PredictionErrorSignal
 * 
 * Computes and broadcasts prediction error as a neuromodulatory signal.
 * 
 * This class computes prediction errors between predicted and actual sensory
 * states and broadcasts them as neuromodulatory signals to enhance learning.
 * It is essential for:
 * - Computing prediction errors
 * - Signaling prediction errors to neural systems
 * - Enhancing learning based on prediction errors
 * - Modulating neural plasticity
 * - Error-driven learning
 * 
 * PredictionErrorSignal mechanisms:
 * - **Error computation**: Computes differences between predicted and actual
 * - **Signaling**: Broadcasts errors as neuromodulatory signals
 * - **Learning enhancement**: Enhances learning based on prediction errors
 * - **Modulation**: Modulates neural plasticity
 * - **Error-driven learning**: Drives learning from prediction errors
 * 
 * Key characteristics of prediction error signaling in NLM:
 * - **Error computation**: Computes differences between predicted and actual
 * - **Signal strength**: Provides neuromodulatory signal strength
 * - **Learning enhancement**: Enhances learning based on errors
 * - **Error components**: Computes error components (intensity, spatial, temporal)
 * - **Surprise**: Detects surprising events based on error threshold
 * 
 * @note This is the neural implementation of prediction error signaling
 * @see NeuralPrediction - For prediction generation
 * @see Config.hpp - Configuration parameters
 * 
 * @author NLM Development Team
 */
class PredictionErrorSignal {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new prediction error signal instance with default parameters.
     * The prediction error signal is initialized with default settings for
     * computing and signaling prediction errors.
     */
    PredictionErrorSignal();
    
    /**
     * @brief Destructor
     * 
     * Cleans up prediction error signal resources and performs any necessary
     * cleanup operations. The prediction error signal is left in a valid but
     * non-operational state after destruction.
     */
    ~PredictionErrorSignal();
    
    // Disable copying - PImpl idiom
    PredictionErrorSignal(const PredictionErrorSignal&) = delete;
    PredictionErrorSignal& operator=(const PredictionErrorSignal&) = delete;
    PredictionErrorSignal(PredictionErrorSignal&&) noexcept;
    PredictionErrorSignal& operator=(PredictionErrorSignal&&) noexcept;
    
    /**
     * @brief Compute error between predicted and actual
     * 
     * Computes the prediction error between predicted and actual sensory states.
     * This is used for:
     * - Error computation
     * - Learning modulation
     * - Error signaling
     * - Error-driven learning
     */
    float computeError(const std::vector<float>& predicted,
                      const std::vector<float>& actual);
    
    /**
     * @brief Get error components
     * 
     * Returns the error components, which represent different aspects of the
     * prediction error (intensity, spatial, temporal). This is used for:
     * - Error analysis
     * - Error interpretation
     * - Error signaling
     * - Error-driven learning
     */
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were
        float temporalError;     // When things happened
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }
    
    /**
     * @brief Get neuromodulation signal strength
     * 
     * Returns the neuromodulation signal strength, which represents the
     * strength of the prediction error signal for learning. Higher values
     * indicate stronger error signals and enhanced learning.
     */
    float getModulationSignal() const;
    
    /**
     * @brief Update history
     * 
     * Updates error history with the current error. This is used for:
     * - Error history management
     * - Error tracking
     * - Error analysis
     * - Error monitoring
     */
    void recordError(float error, SimulationStep step);
    
    /**
     * @brief Get error history
     * 
     * Returns the error history, containing prediction errors over time.
     * This is used for:
     * - Error history management
     * - Error analysis
     * - Error tracking
     * - Error monitoring
     */
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }
    
    /**
     * @brief Is this a surprising event
     * 
     * Determines whether the current error represents a surprising event
     * based on a threshold. This is used for:
     * - Surprise detection
     * - Error interpretation
     * - Surprise signaling
     * - Surprise-driven learning
     */
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }
    
    /**
     * @brief Set surprise threshold
     * 
     * Sets the surprise threshold for detecting surprising events.
     */
    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    
    /**
     * @brief Get surprise threshold
     * 
     * Returns the surprise threshold for detecting surprising events.
     */
    float getSurpriseThreshold() const { return surpriseThreshold_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float lastError_;
    float surpriseThreshold_;
    ErrorComponents errorComponents_;
    std::deque<float> errorHistory_;
};

} // namespace nlm
