#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @file PredictionSystem.hpp
 * 
 * Prediction system implementation for NLM
 * 
 * This file provides the prediction system for NLM, implementing predictive
 * coding and forward models that estimate future sensory states and consequences.
 * Prediction is essential for:
 * - Anticipating future sensory input
 * - Error-driven learning and adaptation
 * - Motor planning and action selection
 * - Cognitive modeling and reasoning
 * - Predictive processing in neural systems
 * 
 * The prediction system in NLM:
 * - **Forward models**: Predict future sensory states
 * - **Predictive coding**: Minimize prediction errors
 * - **Learning**: Update prediction models based on errors
 * - **Planning**: Use predictions for action planning
 * - **Adaptation**: Adjust to environmental changes
 * 
 * Key features:
 * - **Prediction models**: Forward models of sensory processing
 * - **Error computation**: Prediction error signals
 * - **Learning mechanisms**: Model updating based on errors
 * - **Confidence estimation**: Uncertainty quantification
 * - **Temporal processing**: Time-based prediction
 * 
 * @note Essential for predictive processing and learning
 * @note Used extensively in cognitive and motor systems
 * @note Critical for forward models and predictive coding
 * @see Config.hpp - Configuration options for prediction systems
 * @see Brain.hpp - Integration with brain architecture
 * @see NeuralPrediction.hpp - For neural prediction implementation
 * @see SensoryInput.hpp - For sensory input and observations
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class PredictionSystem
 * 
 * Prediction system implementing forward models and predictive coding.
 * 
 * This class implements prediction systems that predict future sensory states
 * and consequences using forward models and predictive coding. It is essential for:
 * - Anticipating future sensory input
 * - Error-driven learning and adaptation
 * - Motor planning and action selection
 * - Cognitive modeling and reasoning
 * - Predictive processing in neural systems
 * 
 * Prediction system mechanisms in NLM:
 * - **Forward models**: Predict future sensory states
 * - **Predictive coding**: Minimize prediction errors
 * - **Learning**: Update prediction models based on errors
 * - **Planning**: Use predictions for action planning
 * - **Adaptation**: Adjust to environmental changes
 * 
 * Key characteristics of prediction systems in NLM:
 * - **Forward models**: Predict future sensory states
 * - **Prediction error**: Compute differences between predicted and actual
 * - **Learning**: Update models based on prediction errors
 * - **Confidence**: Quantify prediction uncertainty
 * - **Temporal**: Handle temporal predictions and sequences
 * 
 * @note Essential for predictive processing and learning
 * @note Used extensively in cognitive and motor systems
 * @note Critical for forward models and predictive coding
 * @see Config.hpp - Configuration options for prediction systems
 * @see Brain.hpp - Integration with brain architecture
 * @see NeuralPrediction.hpp - For neural prediction implementation
 * @see SensoryInput.hpp - For sensory input and observations
 * 
 * @author NLM Development Team
 */
class PredictionSystem {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new prediction system instance with default parameters.
     * The prediction system is initialized with default settings for
     * predictive coding and forward modeling.
     */
    PredictionSystem();
    
    /**
     * @brief Destructor
     * 
     * Cleans up prediction system resources and performs any necessary
     * cleanup operations. The prediction system is left in a valid but
     * non-operational state after destruction.
     */
    ~PredictionSystem();
    
    // Disable copying - PImpl idiom
    PredictionSystem(const PredictionSystem&) = delete;
    PredictionSystem& operator=(const PredictionSystem&) = delete;
    PredictionSystem(PredictionSystem&&) noexcept;
    PredictionSystem& operator=(PredictionSystem&&) noexcept;
    
    /**
     * @brief Make prediction for next timestep
     * 
     * Generates a prediction for the next sensory state based on the current
     * state and learned forward model. This is essential for:
     * - Anticipating future sensory input
     * - Planning and action selection
     * - Error-driven learning
     * - Predictive processing
     */
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    /**
     * @brief Update predictions based on actual observation
     * 
     * Updates prediction models based on the difference between predicted
     * and actual sensory input. This implements predictive coding and
     * learning through error minimization. This is essential for:
     * - Learning from prediction errors
     * - Updating forward models
     * - Improving prediction accuracy
     * - Adaptive behavior
     */
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    /**
     * @brief Get prediction error
     * 
     * Returns the current prediction error, which represents the difference
     * between predicted and actual sensory input. This is used by neural
     * systems to modulate their behavior based on prediction errors.
     */
    float getPredictionError() const;
    
    /**
     * @brief Get prediction confidence
     * 
     * Returns the current prediction confidence, which represents the
     * certainty of the prediction. Higher confidence indicates more
     * certain predictions, while lower confidence indicates uncertainty.
     */
    float getConfidence() const;
    
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
    const std::vector<float>& getErrorHistory() const;
    
    /**
     * @brief Clear prediction history
     * 
     * Clears the prediction history, removing all recorded prediction errors.
     * This is useful for:
     * - Resetting prediction system
     * - Managing prediction resources
     * - Error history management
     * - Error recovery
     */
    void clearHistory();
    
    /**
     * @brief Train prediction model
     * 
     * Trains the prediction model based on an observed sensory input.
     * This updates the forward model to better predict future sensory states
     * and is essential for:
     * - Learning prediction models
     * - Improving prediction accuracy
     * - Adaptation to new inputs
     * - Learning from experience
     */
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
