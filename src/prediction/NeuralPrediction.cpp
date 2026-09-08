#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// Forward declaration
class Brain;

struct NeuralPrediction::Impl {
    Brain* brain;
    size_t sequenceMemorySize;
    size_t predictionHorizon;
    bool temporalPredictionEnabled;
    bool actionConsequenceEnabled;
    
    // State tracking
    std::deque<std::vector<float>> recentSensoryStates;
    std::deque<SimulationStep> stateTimestamps;
    
    // Neural components
    std::vector<NeuronId> predictionNeurons;
    std::vector<NeuronId> sequenceNeurons;
    
    // Error and confidence
    float predictionError;
    float predictionConfidence;
    std::deque<float> errorHistory;
    
    // Action-consequence tracking
    std::deque<std::pair<ActionType, std::vector<float>>> recentActions;
    
    // For simplicity, we'll use a placeholder neural representation
    // In a real implementation, this would involve actual neuron groups
    Impl() : brain(nullptr), sequenceMemorySize(100), predictionHorizon(5),
             temporalPredictionEnabled(true), actionConsequenceEnabled(true),
             predictionError(0.0f), predictionConfidence(0.5f) {}
};

NeuralPrediction::NeuralPrediction() : pImpl(new Impl) {}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralPrediction initialized with brain reference");
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep) {
    // Add to recent states
    pImpl->recentSensoryStates.push_back(sensoryState);
    pImpl->stateTimestamps.push_back(currentStep);
    
    // Maintain sequence memory size
    while (pImpl->recentSensoryStates.size() > pImpl->sequenceMemorySize) {
        pImpl->recentSensoryStates.pop_front();
        pImpl->stateTimestamps.pop_front();
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    std::vector<float> prediction;
    
    if (!pImpl->temporalPredictionEnabled || pImpl->recentSensoryStates.size() < 2) {
        // Return empty prediction if not enough history
        return prediction;
    }
    
    // Simple prediction: use the last known state as base
    // In a real implementation, this would use learned temporal sequences
    if (!pImpl->recentSensoryStates.empty()) {
        prediction = pImpl->recentSensoryStates.back();
    }
    
    // Add small noise to simulate neural variability
    for (float& val : prediction) {
        val += (pImpl->brain->getRandomGenerator()->uniform() - 0.5f) * 0.1f;
    }
    
    // Store the prediction for later comparison
    recordSensoryState(prediction, currentStep);
    
    return prediction;
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState, 
                                             SimulationStep currentStep) {
    std::vector<float> predictedState;
    if (!pImpl->recentSensoryStates.empty()) {
        // Get the most recent prediction
        predictedState = pImpl->recentSensoryStates.back();
    }
    
    // Compute prediction error
    float error = computePredictionError(predictedState, actualState);
    
    // Update prediction error
    pImpl->predictionError = error;
    pImpl->predictionConfidence = std::max(0.0f, 1.0f - error);
    pImpl->errorHistory.push_back(error);
    
    // Keep error history size manageable
    while (pImpl->errorHistory.size() > pImpl->sequenceMemorySize) {
        pImpl->errorHistory.pop_front();
    }
    
    // Use prediction error to modulate neural activity
    modulatePredictionNeurons(error);
    
    return error;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                             const std::vector<float>& currentState) {
    std::vector<float> predictedConsequence;
    
    if (pImpl->actionConsequenceEnabled) {
        // Simple placeholder: predict similar outcome with some noise
        predictedConsequence = currentState;
        
        // Add action-specific bias
        if (action == ActionType::MoveLeft) {
            if (!predictedConsequence.empty()) {
                predictedConsequence[0] += 0.5f;
            }
        } else if (action == ActionType::MoveRight) {
            if (!predictedConsequence.empty()) {
                predictedConsequence[0] -= 0.5f;
            }
        }
        
        // Clamp values
        for (float& val : predictedConsequence) {
            val = std::max(0.0f, std::min(val, 1.0f));
        }
    }
    
    return predictedConsequence;
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                     size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    if (!pImpl->temporalPredictionEnabled || pImpl->recentSensoryStates.empty()) {
        return predictions;
    }
    
    // Simple forward prediction: repeat last state with small changes
    std::vector<float> current = pImpl->recentSensoryStates.back();
    predictions.push_back(current);
    
    for (size_t step = 0; step < numSteps; ++step) {
        std::vector<float> predictedStep = current;
        
        // Apply simple dynamics
        for (float& val : predictedStep) {
            val += (pImpl->brain->getRandomGenerator()->uniform() - 0.5f) * 0.05f;
            val = std::max(0.0f, std::min(val, 1.0f));
        }
        
        predictions.push_back(predictedStep);
        current = predictedStep;
    }
    
    return predictions;
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    if (!pImpl->recentSensoryStates.empty()) {
        pImpl->recentActions.push_back(std::make_pair(action, pImpl->recentSensoryStates.back()));
        
        // Keep only recent actions
        while (pImpl->recentActions.size() > 10) {
            pImpl->recentActions.pop_front();
        }
    }
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& nextState,
                                            SimulationStep currentStep) {
    // Create sequence association
    NeuronId from = findMatchingPatternNeuron(currentState);
    NeuronId to = findMatchingPatternNeuron(nextState);
    
    if (from != INVALID_NEURON_ID && to != INVALID_NEURON_ID) {
        createSequenceAssociation(from, to, 1.0f);
    }
}

NeuronId NeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                    float similarityThreshold) {
    // Simple placeholder: create a new neuron for this pattern
    // In a real implementation, this would search existing neurons
    NeuronId newId(1); // Placeholder ID
    pImpl->predictionNeurons.push_back(newId);
    return newId;
}

void NeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    // Placeholder for creating synaptic connections
    // In a real implementation, this would connect neurons in the brain's regions
    pImpl->sequenceNeurons.push_back(from);
    pImpl->sequenceNeurons.push_back(to);
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    float magnitude = std::sqrt(normA * normB);
    if (magnitude == 0.0f) return 0.0f;
    
    return dotProduct / magnitude;
}

void NeuralPrediction::modulatePredictionNeurons(float error) {
    // Simple modulation: stronger error leads to more neural activity
    // In a real implementation, this would modulate existing prediction neurons
    if (pImpl->brain && !pImpl->predictionNeurons.empty()) {
        // Inject current proportional to error into prediction neurons
        float current = error * 10.0f;  // Scale factor
        for (NeuronId id : pImpl->predictionNeurons) {
            pImpl->brain->injectCurrent(id, current);
        }
    }
}

float NeuralPrediction::computePredictionError(const std::vector<float>& predicted,
                                             const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f; // Maximum error if dimensions don't match
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    return std::sqrt(sumSquaredError / predicted.size());
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    return pImpl->predictionNeurons;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    return pImpl->sequenceNeurons;
}

} // namespace nlm