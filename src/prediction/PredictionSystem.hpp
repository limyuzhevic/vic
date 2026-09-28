#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PLACEHOLDER - Phase 2 will implement real predictive coding

class PredictionSystem {
public:
    /**
     * @brief Construct a new PredictionSystem object
     * Initializes prediction system with default parameters
     */
    PredictionSystem();
    
    /**
     * @brief Destroy the PredictionSystem object
     */
    ~PredictionSystem();
    
    /**
     * @brief Make prediction for next timestep using neural dynamics
     * Uses working memory patterns, episodic memory, and recurrent connections
     * for predictive coding.
     * @param currentState Current sensory state to predict from
     * @return std::unique_ptr<SensoryInput> Predicted sensory state for next timestep
     * @note This is a placeholder implementation. Real implementation will use
     * neural predictive coding with temporal sequences.
     */
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    /**
     * @brief Update predictions based on actual observation
     * Integrates with neuromodulation (dopamine, curiosity) for learning
     * @param predicted The predicted sensory state
     * @param actual The actual observed sensory state
     * @note Updates prediction error and confidence based on prediction accuracy
     */
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    /**
     * @brief Get prediction error
     * @return float Current prediction error (RMS difference between prediction and reality)
     * Error values range from 0.0 (perfect prediction) to 1.0+ (poor prediction)
     */
    float getPredictionError() const;
    
    /**
     * @brief Get prediction confidence
     * @return float Prediction confidence as value between 0.0 and 1.0
     * 0.0 = no confidence, 1.0 = complete confidence in prediction
     */
    float getConfidence() const;
    
    /**
     * @brief Get prediction error history
     * @return const std::vector<float>& Historical prediction error values
     * Most recent errors are at the end of the vector
     */
    const std::vector<float>& getErrorHistory() const;
    
    /**
     * @brief Clear prediction history
     * Resets error history, predicted patterns, and predicted sequences
     */
    void clearHistory();
    
    /**
     * @brief Train prediction model using neural plasticity rules
     * Updates recurrent weights and associative memories based on observations
     * @param observation Current sensory observation to learn from
     * @note This is a placeholder implementation. Real implementation will use
     * STDP and reward-modulated plasticity for learning.
     */
    void train(const SensoryInput& observation);
    
    /**
     * @brief Initialize prediction system with brain reference
     * @param brain Pointer to the brain object that contains neural populations
     * The prediction system will use brain for accessing neurons and memory systems
     */
    void initialize(class Brain* brain);
    
    /**
     * @brief Get current sensory state from brain
     * Extracts current sensory neuron activities and combines them into a state vector
     * @return std::vector<float> Vector representing current sensory state (normalized 0.0-1.0)
     */
    std::vector<float> getCurrentSensoryState() const;
    
    /**
     * @brief Get neural activity pattern for prediction
     * Combines working memory patterns with sensory activity for prediction
     * @return std::vector<float> Neural activity pattern (normalized 0.0-1.0)
     * Can be used for making predictions or comparing with expected patterns
     */
    std::vector<float> getNeuralActivityPattern() const;
    
    /**
     * @brief Update prediction based on neuromodulatory signals
     * Adjusts prediction parameters based on dopamine (precision) and curiosity (exploration)
     * @param dopamineLevel Neuromodulatory dopamine level (0.0 to 1.0+)
     * @param curiosityLevel Neuromodulatory curiosity level (0.0 to 1.0)
     * @param dt Time step duration in simulation time
     * @note Higher dopamine increases confidence in predictions, higher curiosity
     * encourages exploration and prediction updates
     */
    void updateWithNeuromodulation(float dopamineLevel, float curiosityLevel, TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
