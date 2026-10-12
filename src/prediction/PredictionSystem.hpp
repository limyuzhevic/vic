#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <deque>
#include <cmath>
#include <algorithm>

namespace nlm {

// PredictionError: Quantifies difference between predicted and actual states
struct PredictionError {
    float magnitude;                    // Overall error magnitude (0-1)
    float uncertainty;                   // Uncertainty in prediction (0-1)
    std::vector<float> errorComponents;  // Error broken down by sensory dimensions
    Timestamp timestamp;                 // When error occurred
    
    PredictionError() : magnitude(0.0f), uncertainty(0.5f), timestamp(-1.0f) {}
};

// PredictionState: Neural representation of predicted future state
struct PredictionState {
    std::vector<float> stateVector;         // Encoded sensory state
    std::vector<float> uncertaintyVector;   // Uncertainty per dimension
    std::vector<NeuronId> sourceNeurons;    // Which neurons generated this prediction
    float confidence;                        // Overall confidence (0-1)
    float predictionHorizon;                 // How many steps ahead (seconds)
    
    PredictionState() : confidence(0.5f), predictionHorizon(0.0f) {}
};

// PredictionSystem: Core prediction engine using spiking neural substrate
// Implements Bayesian inference, uncertainty quantification, and hierarchical prediction
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference and configuration
    void initialize(Brain* brain, size_t predictionNeurons = 100);
    
    // Make real prediction for next timestep using neural dynamics
    // Returns predicted sensory state with uncertainty bounds
    std::pair<std::unique_ptr<SensoryInput>, PredictionError> predictNextState(
        const SensoryInput& currentState, Timestamp currentTime);
    
    // Update predictions based on actual observation with Bayesian learning
    void updatePredictions(const PredictionState& predicted, 
                         const std::vector<float>& actualObservation,
                         Timestamp currentTime);
    
    // Bayesian prediction combining multiple hypotheses
    PredictionState bayesianPrediction(
        const std::vector<float>& currentState,
        const std::vector<std::vector<float>>& hypotheses,
        const std::vector<float>& hypothesisWeights);
    
    // Get current prediction error
    float getPredictionError() const;
    
    // Get overall prediction confidence (0-1)
    float getConfidence() const;
    
    // Get prediction error history for analysis
    const std::deque<PredictionError>& getErrorHistory() const { return errorHistory_; }
    
    // Clear prediction history and reset model
    void clearHistory();
    
    // Train prediction model using real sensory sequences
    void train(const std::vector<SensoryInput>& observations);
    
    // Set neuromodulation for confidence-based exploration
    void enableNeuromodulation(bool enable) { neuromodulationEnabled_ = enable; }
    
    // Configure prediction horizon and learning rates
    void setPredictionHorizon(float horizon) { predictionHorizon_ = horizon; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setUncertaintyThreshold(float threshold) { uncertaintyThreshold_ = threshold; }
    
    // Get neurons involved in prediction
    std::vector<NeuronId> getPredictionNeurons() const;
    
    // Get current active prediction neurons
    std::vector<NeuronId> getActivePredictionNeurons() const;
    
    // Enable/disable hierarchical prediction
    void enableHierarchicalPrediction(bool enable) { hierarchicalPredictionEnabled_ = enable; }
    
    // Get prediction statistics
    struct Statistics {
        float meanPredictionError;
        float meanConfidence;
        size_t totalPredictions;
        size_t successfulPredictions;  // Error < threshold
    };
    
    Statistics getStatistics() const;
    
    // Real-time prediction update based on neural activity
    void updateFromNeuralActivity(const std::vector<float>& neuralPattern,
                                 Timestamp currentTime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration parameters
    size_t predictionNeurons_;
    float predictionHorizon_;
    float learningRate_;
    float uncertaintyThreshold_;
    bool neuromodulationEnabled_;
    bool hierarchicalPredictionEnabled_;
    
    // Error history for learning and analysis
    std::deque<PredictionError> errorHistory_;
    
    // Internal state management
    Brain* brain_;
    Timestamp lastPredictionTime_;
    
    // Helper methods
    void createPredictionNeurons();
    PredictionError computeBayesianError(const PredictionState& predicted,
                                        const std::vector<float>& actual);
    void updatePredictionWeights(const PredictionError& error);
    float computePredictionConfidence(const PredictionState& prediction) const;
};

} // namespace nlm
