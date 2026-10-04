#include "PredictionSystem.hpp"
#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    // Core predictive coding state
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction engine
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Forward model weights (synaptic connections for prediction)
    std::vector<float> forwardModelWeights;
    std::vector<NeuronId> predictionNeurons;
    
    // Hebbian learning for predictive connections
    float learningRate;
    float predictionHorizon;
    
    // Integration with brain systems
    Brain* brain;
    bool predictionActive;
    
    Impl() 
        : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), predictionHorizon(1.0f)
        , brain(nullptr), predictionActive(false) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
    pImpl->forwardModelWeights.clear();
    pImpl->predictionNeurons.clear();
    pImpl->predictionActive = true;
    NLM_LOG_INFO("PredictionSystem initialized with neural predictive coding");
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralPrediction->initialize(brain);
    
    // Create prediction pathway neurons in sensory regions
    if (brain) {
        // Find sensory region and add prediction neurons
        for (const auto& region : brain->getRegions()) {
            for (NeuronType type : {NeuronType::Sensory, NeuronType::Internal}) {
                for (const auto* population : region->getPopulations()) {
                    if (population->getType() == type) {
                        // Create prediction neurons for each population
                        for (size_t i = 0; i < 10; ++i) {
                            NeuronId predNeuron(pImpl->predictionNeurons.size() + 20000);
                            pImpl->predictionNeurons.push_back(predNeuron);
                        }
                    }
                }
            }
        }
        
        // Initialize forward model weights (synaptic connections)
        pImpl->forwardModelWeights.resize(pImpl->predictionNeurons.size(), 0.5f);
    }
    
    NLM_LOG_INFO("PredictionSystem neural substrate initialized with " + 
                 std::to_string(pImpl->predictionNeurons.size()) + " prediction neurons");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain || !pImpl->predictionActive) {
        return currentState.clone();
    }
    
    // Convert sensory input to vector for neural prediction
    const auto& sensoryData = currentState.getData();
    
    // Use neural prediction engine to generate future state
    auto predictedData = pImpl->neuralPrediction->generatePrediction(pImpl->brain->getConfig()->getOr<size_t>("simulation_step", 0));
    
    if (predictedData.empty()) {
        // Fall back to current state with prediction
        NLM_LOG_DEBUG("Neural prediction empty, using current state as prediction");
        return currentState.clone();
    }
    
    // Apply forward model transformation using learned weights
    std::vector<float> finalPrediction = applyForwardModel(sensoryData, predictedData);
    
    // Create predicted sensory input
    std::unique_ptr<SensoryInput> result;
    
    if (currentState.getType() == "Vision") {
        result = std::make_unique<Vision>(sqrt(finalPrediction.size()), sqrt(finalPrediction.size()), 1);
        result->setData(finalPrediction);
    } else if (currentState.getType() == "Audio") {
        auto audio = std::make_unique<Audio>(44100, finalPrediction.size());
        audio->setData(finalPrediction);
        result = std::move(audio);
    } else {
        // For other types, create InternalSignals
        auto internal = std::make_unique<InternalSignals>();
        for (float val : finalPrediction) {
            internal->addSignal(val);
        }
        result = std::move(internal);
    }
    
    if (!result) {
        result = currentState.clone();
    }
    
    // Store prediction for error computation
    pImpl->neuralPrediction->recordSensoryState(sensoryData, pImpl->brain->getTotalSpikeCount());
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain) return;
    
    // Convert both to vectors for error computation
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.size() != actualData.size() || predictedData.empty()) {
        return;
    }
    
    // Compute prediction error using neural similarity measure
    float similarity = computeNeuralSimilarity(predictedData, actualData);
    pImpl->predictionError = 1.0f - similarity;  // Convert similarity to error (0=perfect, 1=total error)
    
    // Store error in neural prediction engine
    pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->brain->getTotalSpikeCount());
    
    // Update error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Apply error-driven learning (PEMD - Predictive Encoding Minimization Principle)
    applyPredictiveErrorMinimization(predictedData, actualData);
    
    // Update forward model weights
    updateForwardModelWeights(predictedData, actualData, similarity);
    
    // Provide prediction error signal to neuromodulation (dopamine)
    providePredictionErrorToNeuromodulation();
}

float PredictionSystem::computeNeuralSimilarity(const std::vector<float>& a, 
                                              const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

void PredictionSystem::applyPredictiveErrorMinimization(const std::vector<float>& predicted,
                                                       const std::vector<float>& actual) {
    if (!pImpl->brain) return;
    
    // Create prediction error signal for neuromodulation
    float error = pImpl->predictionError;
    
    if (error > 0.1f) {  // Significant prediction error
        // Apply Hebbian learning to strengthen correct predictions
        for (size_t i = 0; i < pImpl->predictionNeurons.size(); ++i) {
            if (pImpl->forwardModelWeights[i] > 0.3f) {
                // Strengthen connections that predicted well
                float weightAdjustment = pImpl->learningRate * (1.0f - error) * 0.1f;
                pImpl->forwardModelWeights[i] += weightAdjustment;
                
                // Ensure weight stays in valid range
                if (pImpl->forwardModelWeights[i] > 1.0f) {
                    pImpl->forwardModelWeights[i] = 1.0f;
                }
            }
        }
        
        // Weaken incorrect predictions
        for (size_t i = 0; i < pImpl->predictionNeurons.size(); ++i) {
            if (pImpl->forwardModelWeights[i] < 0.7f) {
                float weightAdjustment = -pImpl->learningRate * error * 0.1f;
                pImpl->forwardModelWeights[i] += weightAdjustment;
                
                if (pImpl->forwardModelWeights[i] < 0.0f) {
                    pImpl->forwardModelWeights[i] = 0.0f;
                }
            }
        }
        
        // Update neural prediction synapses through plasticity
        pImpl->neuralPrediction->modulatePredictionSynapses(error, 1.0f - error);
    }
}

void PredictionSystem::updateForwardModelWeights(const std::vector<float>& predicted,
                                                const std::vector<float>& actual,
                                                float similarity) {
    if (similarity > 0.8f) {
        // Successful prediction - strengthen forward model weights
        for (size_t i = 0; i < pImpl->predictionNeurons.size() && i < pImpl->forwardModelWeights.size(); ++i) {
            pImpl->forwardModelWeights[i] *= (1.0f + pImpl->learningRate * 0.05f);
            if (pImpl->forwardModelWeights[i] > 1.0f) pImpl->forwardModelWeights[i] = 1.0f;
        }
    } else if (similarity < 0.3f) {
        // Poor prediction - weaken forward model weights
        for (size_t i = 0; i < pImpl->predictionNeurons.size() && i < pImpl->forwardModelWeights.size(); ++i) {
            pImpl->forwardModelWeights[i] *= (1.0f - pImpl->learningRate * 0.05f);
            if (pImpl->forwardModelWeights[i] < 0.0f) pImpl->forwardModelWeights[i] = 0.0f;
        }
    }
}

std::vector<float> PredictionSystem::applyForwardModel(const std::vector<float>& sensoryState,
                                                       const std::vector<float>& neuralPrediction) const {
    std::vector<float> result = neuralPrediction;
    
    // Apply forward model transformation using learned weights
    for (size_t i = 0; i < std::min(result.size(), pImpl->forwardModelWeights.size()); ++i) {
        result[i] *= pImpl->forwardModelWeights[i];
    }
    
    // Add bias term (synaptic baseline)
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] += 0.1f;  // Bias term
    }
    
    return result;
}

void PredictionSystem::providePredictionErrorToNeuromodulation() {
    if (!pImpl->brain) return;
    
    // Get prediction error signal from neuromodulation system
    auto* predErrorSignal = pImpl->brain->getPredictionErrorSignal();
    if (predErrorSignal) {
        // Get actual vs predicted error
        const auto& errorHistory = pImpl->neuralPrediction->getErrorHistory();
        if (!errorHistory.empty()) {
            float currentError = errorHistory.back();
            
            // Compute prediction vs actual mismatch using brain's internal state
            // This would normally involve comparing predicted sensory activity with actual
            // For now, use the prediction error from neural prediction engine
            predErrorSignal->computeError(pImpl->predictionError, currentError);
        }
    }
    
    // Also update dopamine level based on prediction error
    auto* dopamine = pImpl->brain->getDopamine();
    if (dopamine) {
        // Higher prediction error = higher dopamine (prediction error signaling)
        float dopamineLevel = std::min(1.0f, pImpl->predictionError * 2.0f);
        // In real implementation, dopamine would be updated through neuromodulation
        // For now, just record the error for later processing
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
    if (!pImpl) return;
    // Train neural prediction engine
    pImpl->neuralPrediction->recordSensoryState(observation.getData(), pImpl->brain ? pImpl->brain->getTotalSpikeCount() : 0);
}

// Helper method implementations
std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    if (!pImpl) return std::vector<NeuronId>();
    // Get prediction neurons from neural prediction engine
    return pImpl->neuralPrediction ? pImpl->neuralPrediction->getPredictionNeurons() : std::vector<NeuronId>();
}

std::vector<NeuronId> PredictionSystem::getSequenceNeurons() const {
    if (!pImpl) return std::vector<NeuronId>();
    // Get sequence neurons from neural prediction engine  
    return pImpl->neuralPrediction ? pImpl->neuralPrediction->getSequenceNeurons() : std::vector<NeuronId>();
}
