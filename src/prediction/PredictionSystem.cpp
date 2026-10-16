#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../brain/NeuralWorkingMemory.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction engine
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Integration with brain systems
    Brain* brain;
    NeuralWorkingMemory* workingMemory;
    Neuromodulator* neuromodulator;
    StructuralPlasticity* structuralPlasticity;
    
    // Neural substrate for prediction
    std::vector<NeuronId> predictionNeurons;
    
    // Prediction state
    std::vector<float> currentNeuralPattern;
    std::vector<float> lastPrediction;
    
    Impl() 
        : predictionError(0.0f), confidence(0.5f)
        , brain(nullptr)
        , workingMemory(nullptr)
        , neuromodulator(nullptr)
        , structuralPlasticity(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    if (!brain) {
        return;
    }
    
    pImpl->brain = brain;
    pImpl->workingMemory = brain->getWorkingMemory();
    pImpl->neuromodulator = brain->getDopamine(); // Using dopamine as neuromodulator
    pImpl->structuralPlasticity = brain->getStructuralPlasticity();
    
    // Initialize neural prediction engine
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->neuralPrediction->initialize(brain);
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Get sensory data from current state
    const auto& sensoryData = currentState.getData();
    
    // Check if we have neural substrate available
    if (!pImpl->workingMemory || sensoryData.empty()) {
        // No neural integration - return simple copy as fallback
        return currentState.clone();
    }
    
    // Store current state in working memory
    pImpl->currentNeuralPattern = sensoryData;
    pImpl->workingMemory->store(sensoryData, 1.0f);
    
    // Use neural prediction to generate next state
    // NeuralPrediction generates predictions based on learned sequences
    // This is the key difference from simple copying - we use neural patterns
    if (pImpl->neuralPrediction) {
        auto prediction = pImpl->neuralPrediction->generatePrediction(pImpl->workingMemory->getCurrentStep());
        
        if (!prediction.empty()) {
            // Convert neural pattern to SensoryInput
            auto result = std::make_unique<SensoryInput>();
            result->setData(prediction);
            pImpl->lastPrediction = prediction;
            pImpl->confidence = pImpl->neuralPrediction->getPredictionConfidence();
            
            // Store the prediction in working memory for prediction error computation
            pImpl->workingMemory->store(prediction, pImpl->confidence);
            
            NLM_LOG_INFO("Prediction generated using neural substrate with confidence: " + 
                        std::to_string(pImpl->confidence));
            
            return result;
        }
    }
    
    // Fallback: copy current state if neural prediction not available
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error using neural error signals
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.empty() || actualData.empty() || predData.size() != actualData.size()) {
        pImpl->predictionError = 1.0f; // Maximum error
        pImpl->errorHistory.push_back(pImpl->predictionError);
        return;
    }
    
    float sumError = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
        count++;
    }
    
    if (count > 0) {
        pImpl->predictionError = std::sqrt(sumError / count);
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Neural prediction error computation - use error to modulate confidence
        // Lower error = higher confidence
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
        
        // Use NeuralPrediction for advanced error computation if available
        if (pImpl->neuralPrediction) {
            pImpl->predictionError = pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->workingMemory->getCurrentStep());
        }
        
        // Signal neuromodulation systems
        if (pImpl->neuromodulator && pImpl->predictionError > 0.0f) {
            // Create prediction error signal
            NeuromodulationSignal signal;
            signal.type = NeuromodulationType::PredictionError;
            signal.strength = pImpl->predictionError * 2.0f; // Scale error to 0-2 range
            signal.duration = 1;
            // Note: NeuromodulationRepresentation may not be defined, omit representation
            
            pImpl->neuromodulator->signal(pImpl->predictionError, signal);
        }
        
        // Apply plasticity based on prediction error
        if (pImpl->structuralPlasticity && pImpl->predictionError > 0.3f) {
            // High prediction error triggers structural changes
            float plasticityFactor = (pImpl->predictionError - 0.3f) / 0.7f; // Normalize to 0-1
            pImpl->structuralPlasticity->updateWithError(plasticityFactor, pImpl->predictionError);
        }
        
        NLM_LOG_INFO("Prediction error computed: " + std::to_string(pImpl->predictionError) +
                    " (confidence: " + std::to_string(pImpl->confidence) + ")");
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train using neural plasticity mechanisms
    if (!pImpl->workingMemory || observation.getData().empty()) {
        return;
    }
    
    const auto& data = observation.getData();
    
    // Use working memory to maintain observation pattern
    pImpl->workingMemory->store(data, 1.0f);
    
    // Update neural prediction model with new experience
    if (pImpl->neuralPrediction) {
        // Feed current observation to neural prediction system
        // This updates learned sequences and associations
        SimulationStep currentStep = pImpl->workingMemory->getCurrentStep();
        pImpl->neuralPrediction->recordSensoryState(data, currentStep);
        
        // Use neuromodulator signals to guide learning
        if (pImpl->neuromodulator) {
            NeuromodulationSignal curiositySignal;
            curiositySignal.type = NeuromodulationType::Curiosity;
            curiositySignal.strength = 0.1f; // Mild curiosity for exploration
            curiositySignal.duration = 1;
            
            pImpl->neuromodulator->signal(0.1f, curiositySignal);
        }
    }
    
    // Apply synaptic plasticity to prediction neurons
    if (pImpl->structuralPlasticity && !pImpl->predictionNeurons.empty()) {
        for (const auto& neuronId : pImpl->predictionNeurons) {
            // Apply Hebbian-like learning based on prediction accuracy
            if (pImpl->predictionError < 0.3f) {
                // Successful prediction - strengthen connections
                pImpl->structuralPlasticity->updateWithLearning(neuronId, 0.5f, pImpl->predictionError);
            } else {
                // Failed prediction - weaken incorrect connections
                pImpl->structuralPlasticity->updateWithLearning(neuronId, -0.2f, pImpl->predictionError);
            }
        }
    }
    
    NLM_LOG_INFO("Prediction system trained with observation, error: " + 
                std::to_string(pImpl->predictionError));
}

} // namespace nlm
