#include "Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class NeuralPrediction;
class ActionConsequencePredictor;
class PredictionErrorSignal;

// This header includes all Phase 6 prediction systems in one place:
// - NeuralPrediction: Temporal sequence prediction using spiking substrate
// - ActionConsequencePredictor: Action-to-consequence mapping
// - PredictionErrorSignal: Prediction error computation and neuromodulation

// Use this for clean integration with the Brain class
class Phase6PredictionSystems {
public:
    // Initialize all prediction systems with brain reference
    void initialize(Brain* brain) {
        neuralPrediction = std::make_unique<NeuralPrediction>();
        actionConsequencePredictor = std::make_unique<ActionConsequencePredictor>();
        predictionErrorSignal = std::make_unique<PredictionErrorSignal>();
        
        neuralPrediction->initialize(brain);
        // Note: ActionConsequencePredictor and PredictionErrorSignal 
        // are initialized within NeuralPrediction.cpp
    }
    
    // Get access to individual systems
    NeuralPrediction* getNeuralPrediction() { return neuralPrediction.get(); }
    ActionConsequencePredictor* getActionConsequencePredictor() { 
        return neuralPrediction ? neuralPrediction->getActionConsequencePredictor() : nullptr;
    }
    PredictionErrorSignal* getPredictionErrorSignal() { 
        return neuralPrediction ? neuralPrediction->getPredictionErrorSignal() : nullptr;
    }
    
    // Update prediction systems with current sensory input
    void updatePredictions(const std::vector<float>& sensoryState, SimulationStep step) {
        if (neuralPrediction) {
            neuralPrediction->recordSensoryState(sensoryState, step);
        }
    }
    
    // Generate predictions for next action
    std::vector<float> generatePredictions(SimulationStep step) {
        return neuralPrediction ? neuralPrediction->generatePrediction(step) : std::vector<float>();
    }
    
    // Record action consequences
    void recordActionConsequence(ActionType action, 
                                 const std::vector<float>& beforeState,
                                 const std::vector<float>& afterState,
                                 float reward,
                                 SimulationStep step) {
        if (neuralPrediction) {
            neuralPrediction->recordAction(action, step);
            if (auto* actionPredictor = neuralPrediction->getActionConsequencePredictor()) {
                actionPredictor->recordExperience(action, beforeState, afterState, reward);
            }
        }
    }
    
    // Get prediction error for neuromodulation
    float getPredictionError() const {
        return neuralPrediction ? neuralPrediction->getPredictionError() : 0.0f;
    }
    
    // Get prediction confidence
    float getPredictionConfidence() const {
        return neuralPrediction ? neuralPrediction->getPredictionConfidence() : 0.0f;
    }
    
    // Check if prediction is surprising (for curiosity)
    bool isPredictionSurprising() const {
        return neuralPrediction ? neuralPrediction->isSurprising() : false;
    }
    
    // Reset prediction history
    void clearPredictionHistory() {
        if (neuralPrediction) {
            neuralPrediction->clearHistory();
        }
    }
    
    // Configure prediction parameters
    void configurePrediction(size_t sequenceMemorySize, size_t predictionHorizon) {
        if (neuralPrediction) {
            neuralPrediction->setSequenceMemorySize(sequenceMemorySize);
            neuralPrediction->setPredictionHorizon(predictionHorizon);
        }
    }
    
private:
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    // ActionConsequencePredictor and PredictionErrorSignal are 
    // contained within NeuralPrediction implementation
};

} // namespace nlm
