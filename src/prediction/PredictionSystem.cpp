#include "PredictionSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction components
    std::vector<float> currentPredictions;
    std::vector<float> confidenceWeights;
    std::vector<float> predictionHistory;
    
    // Self-model integration
    std::vector<float> selfModelPredictions;
    std::vector<float> selfModelConfidence;
    
    // Working memory integration
    std::vector<float> workingMemoryPredictions;
    std::vector<float> workingMemoryActivation;
    
    // Forward model learning
    std::vector<std::vector<float>> forwardModel;
    std::vector<std::vector<float>> predictionErrors;
    
    // Neuromodulation integration
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorSignal;
    
    // Learning parameters
    float learningRate;
    float predictionGain;
    float confidenceScaling;
    
    // State tracking
    SimulationStep currentStep;
    size_t predictionHorizon;
    size_t memoryWindowSize;
    
    // Biological parameters
    float synapticPlasticity;
    float neuralAdaptationRate;
    
    // Brain integration
    Brain* brain_;
    
    Impl() : dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f), 
             predictionErrorSignal(0.0f), learningRate(0.01f), predictionGain(1.0f),
             confidenceScaling(1.0f), synapticPlasticity(0.5f), neuralAdaptationRate(0.05f),
             currentStep(0), predictionHorizon(10), memoryWindowSize(50), brain_(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    // Integrate multiple prediction sources
    pImpl->currentPredictions.clear();
    pImpl->selfModelPredictions.clear();
    pImpl->workingMemoryPredictions.clear();
    
    // Get current sensory state as vector
    pImpl->currentPredictions = currentData;
    
    // Integrate with SelfModel if available for more accurate predictions
    if (pImpl->brain_) {
        // Use SelfModel's learned action-effect mappings for better predictions
        // SelfModel predicts action consequences based on recorded experiences
        // This provides biologically realistic forward predictions
        // (In a full implementation, this would call SelfModel::predictActionConsequence)
        
        // Blend SelfModel predictions with sensory predictions
        float selfModelWeight = pImpl->selfModelConfidence.empty() ? 0.5f : std::min(1.0f, pImpl->selfModelConfidence[0] + 0.5f);
        
        if (!pImpl->selfModelPredictions.empty() && selfModelWeight > 0.0f) {
            size_t blendSize = std::min(pImpl->currentPredictions.size(), pImpl->selfModelPredictions.size());
            for (size_t i = 0; i < blendSize; ++i) {
                pImpl->currentPredictions[i] = pImpl->currentPredictions[i] * (1.0f - selfModelWeight) +
                                              pImpl->selfModelPredictions[i] * selfModelWeight;
            }
        }
    }
    
    // Integrate with working memory for context-aware predictions
    integrateWithWorkingMemory(pImpl->currentPredictions);
    
    // Apply neuromodulation scaling
    applyNeuromodulationScaling(pImpl->currentPredictions);
    
    // Generate predicted state based on forward model
    std::vector<float> predictedState = generateForwardPrediction(pImpl->currentPredictions);
    
    // Add biologically realistic noise for exploration
    addPredictionNoise(predictedState);
    
    // Create SensoryInput from predicted state
    std::unique_ptr<SensoryInput> predictedInput = std::make_unique<InternalSignals>();
    auto& predictedData = const_cast<std::vector<float>&>(static_cast<const InternalSignals&>(*predictedInput).getData());
    predictedData = predictedState;
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error using biologically plausible metrics
    float predictionError = computePredictionError(predData, actualData);
    
    // Update prediction error signal for neuromodulation
    pImpl->predictionErrorSignal = predictionError;
    
    // Store error in history for learning
    pImpl->predictionHistory.push_back(predictionError);
    if (pImpl->predictionHistory.size() > pImpl->memoryWindowSize) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Update forward model based on prediction error (Hebbian-like learning)
    updateForwardModel(predData, actualData, predictionError);
    
    // Update prediction confidence based on error consistency
    updatePredictionConfidence(predictionError);
    
    // Apply synaptic plasticity based on prediction error
    applySynapticPlasticity(predictionError);
    
    // Update prediction system state
    pImpl->currentStep++;
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionErrorSignal;
}

float PredictionSystem::getConfidence() const {
    if (pImpl->confidenceWeights.empty()) return 0.5f;
    
    float totalConfidence = 0.0f;
    for (float weight : pImpl->confidenceWeights) {
        totalConfidence += weight;
    }
    
    return totalConfidence / pImpl->confidenceWeights.size();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->predictionHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->predictionHistory.clear();
    pImpl->predictionErrors.clear();
    pImpl->forwardModel.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Learn forward model from observation (Hebbian-like learning)
    learnFromObservation(data);
    
    // Update prediction parameters
    adaptLearningParameters();
}

// Helper methods for enhanced prediction system

void PredictionSystem::integrateWithWorkingMemory(std::vector<float>& predictions) {
    if (!pImpl->brain_ || !pImpl->brain_->getWorkingMemory()) {
        return;
    }
    
    auto wmState = pImpl->brain_->getWorkingMemory()->retrieve();
    if (wmState.empty()) {
        return;
    }
    
    // Use working memory to weight predictions
    float wmActivity = pImpl->brain_->getWorkingMemory()->getMemoryActivity();
    if (wmActivity > 0.1f) {
        // Blend predictions with working memory content
        size_t blendSize = std::min(predictions.size(), wmState.size());
        for (size_t i = 0; i < blendSize; ++i) {
            float wmWeight = wmState[i] * wmActivity;
            predictions[i] = predictions[i] * (1.0f - wmWeight) + wmState[i] * wmWeight;
        }
    }
}

void PredictionSystem::applyNeuromodulationScaling(std::vector<float>& predictions) {
    // Apply dopamine scaling based on prediction confidence
    float dopamineFactor = 1.0f + pImpl->dopamineLevel * 0.5f;
    
    // Apply curiosity enhancement
    float curiosityFactor = 1.0f + pImpl->curiosityLevel * 0.3f;
    
    // Apply novelty enhancement for exploration
    float noveltyFactor = 1.0f + pImpl->noveltyLevel * 0.2f;
    
    // Combine scaling factors
    float totalScale = dopamineFactor * curiosityFactor * noveltyFactor;
    
    // Apply scaling to predictions
    for (auto& pred : predictions) {
        pred *= totalScale;
        pred = std::clamp(pred, 0.0f, 1.0f);
    }
}

std::vector<float> PredictionSystem::generateForwardPrediction(const std::vector<float>& state) {
    // Generate prediction using learned forward model
    std::vector<float> prediction = state;
    
    // If we have a learned forward model, use it
    if (!pImpl->forwardModel.empty() && pImpl->forwardModel.size() > 1) {
        // Use the most recent forward model mapping
        size_t currentSize = std::min(state.size(), pImpl->forwardModel[0].size());
        for (size_t i = 0; i < currentSize; ++i) {
            prediction[i] = pImpl->forwardModel[0][i] * pImpl->predictionGain;
        }
    } else {
        // Fallback: generate biologically plausible prediction
        // Simulate expected changes based on current state
        for (auto& pred : prediction) {
            // Add expected drift towards equilibrium
            pred = pred * 0.9f + 0.05f;
            
            // Add context-dependent modulation
            if (pred > 0.7f) {
                pred *= 0.95f; // Expect decay from high values
            } else if (pred < 0.3f) {
                pred *= 1.05f; // Expect growth from low values
            }
        }
    }
    
    return prediction;
}

void PredictionSystem::addPredictionNoise(std::vector<float>& predictions) {
    // Add biologically realistic noise for exploration
    static std::mt19937 rng(std::random_device{}());
    std::normal_distribution<float> noiseDist(0.0f, 0.02f);
    
    for (auto& pred : predictions) {
        // Scale noise based on confidence and neuromodulation
        float noiseScale = 0.05f * (1.0f - getConfidence());
        if (pImpl->dopamineLevel > 0.5f) {
            noiseScale *= 1.5f; // Higher dopamine increases exploration noise
        }
        
        pred += noiseDist(rng) * noiseScale;
        pred = std::clamp(pred, 0.0f, 1.0f);
    }
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;
    }
    
    // Calculate multiple error components (biologically plausible)
    float intensityError = 0.0f;
    float temporalError = 0.0f;
    float spatialError = 0.0f;
    
    size_t splitPoint = predicted.size() / 3;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float error = predicted[i] - actual[i];
        float squaredError = error * error;
        
        // Split error into components based on position
        if (i < splitPoint) {
            intensityError += squaredError;
        } else if (i < splitPoint * 2) {
            spatialError += squaredError;
        } else {
            temporalError += squaredError;
        }
    }
    
    // Combine errors with biologically inspired weighting
    float totalError = intensityError * 0.4f + spatialError * 0.3f + temporalError * 0.3f;
    totalError = std::sqrt(totalError) / std::sqrt(static_cast<float>(predicted.size()));
    
    return totalError;
}

void PredictionSystem::updateForwardModel(const std::vector<float>& predicted, const std::vector<float>& actual, float error) {
    // Learn forward model using prediction error (error-driven learning)
    
    // Shift existing forward model
    if (pImpl->forwardModel.size() >= pImpl->predictionHorizon) {
        pImpl->forwardModel.erase(pImpl->forwardModel.begin());
    }
    
    // Create new forward model entry
    std::vector<float> modelEntry;
    
    // Store actual state as learned consequence
    modelEntry = actual;
    
    // Apply error-based learning (predictive coding)
    float learningFactor = pImpl->learningRate * (1.0f - error);
    
    for (size_t i = 0; i < modelEntry.size() && i < predicted.size(); ++i) {
        // Hebbian-like update: strengthen connections that reduce error
        float delta = (actual[i] - predicted[i]) * learningFactor;
        modelEntry[i] += delta;
        modelEntry[i] = std::clamp(modelEntry[i], 0.0f, 1.0f);
    }
    
    pImpl->forwardModel.push_back(modelEntry);
}

void PredictionSystem::updatePredictionConfidence(float error) {
    // Update prediction confidence based on error and neuromodulation
    
    float confidenceChange = 0.0f;
    
    if (error < 0.1f) {
        // Low error increases confidence
        confidenceChange = pImpl->neuralAdaptationRate * 0.5f;
    } else if (error > 0.5f) {
        // High error decreases confidence
        confidenceChange = -pImpl->neuralAdaptationRate * 0.8f;
    }
    
    // Apply neuromodulation effects on confidence
    confidenceChange *= (1.0f + pImpl->dopamineLevel * 0.3f);
    confidenceChange *= (1.0f + pImpl->curiosityLevel * 0.2f);
    
    // Update confidence weights
    for (auto& weight : pImpl->confidenceWeights) {
        weight += confidenceChange;
        weight = std::clamp(weight, 0.1f, 1.0f);
    }
    
    // Update self-model confidence if available
    for (auto& selfConf : pImpl->selfModelConfidence) {
        selfConf += confidenceChange * 0.5f;
        selfConf = std::clamp(selfConf, 0.1f, 1.0f);
    }
}

void PredictionSystem::applySynapticPlasticity(float error) {
    // Apply synaptic plasticity based on prediction error
    
    // Scale plasticity with neuromodulators
    float plasticityScale = 1.0f;
    plasticityScale *= (1.0f + pImpl->dopamineLevel * 0.5f); // Dopamine enhances plasticity
    plasticityScale *= (1.0f - std::min(1.0f, error * 2.0f)); // High error reduces plasticity
    
    pImpl->synapticPlasticity = std::clamp(pImpl->synapticPlasticity * plasticityScale, 0.1f, 1.0f);
}

void PredictionSystem::learnFromObservation(const std::vector<float>& observation) {
    // Learn from sensory observation using Hebbian-like mechanisms
    
    // Create neural representation of observation
    std::vector<float> neuralRep = observation;
    
    // Apply homeostatic normalization
    float repNorm = 0.0f;
    for (float val : neuralRep) {
        repNorm += val * val;
    }
    repNorm = std::sqrt(repNorm);
    
    if (repNorm > 0.01f) {
        for (auto& val : neuralRep) {
            val /= repNorm;
        }
    }
    
    // Store in forward model
    if (pImpl->forwardModel.size() < pImpl->predictionHorizon) {
        pImpl->forwardModel.push_back(neuralRep);
    }
}

void PredictionSystem::adaptLearningParameters() {
    // Adapt learning parameters based on prediction performance
    
    if (pImpl->predictionHistory.size() > 10) {
        // Calculate recent prediction error
        float recentErrorSum = 0.0f;
        size_t recentCount = std::min(static_cast<size_t>(10), pImpl->predictionHistory.size());
        
        for (size_t i = 0; i < recentCount; ++i) {
            recentErrorSum += pImpl->predictionHistory[pImpl->predictionHistory.size() - 1 - i];
        }
        
        float avgError = recentErrorSum / recentCount;
        
        // Adapt learning rate based on error
        if (avgError < 0.1f) {
            // Good predictions - increase learning rate
            pImpl->learningRate = std::min(0.1f, pImpl->learningRate * 1.1f);
        } else if (avgError > 0.5f) {
            // Poor predictions - decrease learning rate
            pImpl->learningRate = std::max(0.001f, pImpl->learningRate * 0.9f);
        }
    }
}

void PredictionSystem::integrateWithWorkingMemory(std::vector<float>& predictions) {
    if (!pImpl->brain_ || !pImpl->brain_->getWorkingMemory()) {
        return;
    }
    
    auto wmState = pImpl->brain_->getWorkingMemory()->retrieve();
    if (wmState.empty()) {
        return;
    }
    
    // Use working memory to weight predictions
    float wmActivity = pImpl->brain_->getWorkingMemory()->getMemoryActivity();
    if (wmActivity > 0.1f) {
        // Blend predictions with working memory content
        size_t blendSize = std::min(predictions.size(), wmState.size());
        for (size_t i = 0; i < blendSize; ++i) {
            float wmWeight = wmState[i] * wmActivity;
            predictions[i] = predictions[i] * (1.0f - wmWeight) + wmState[i] * wmWeight;
        }
    }
}

void PredictionSystem::applyNeuromodulationScaling(std::vector<float>& predictions) {
    // Apply dopamine scaling based on prediction confidence
    float dopamineFactor = 1.0f + pImpl->dopamineLevel * 0.5f;
    
    // Apply curiosity enhancement
    float curiosityFactor = 1.0f + pImpl->curiosityLevel * 0.3f;
    
    // Apply novelty enhancement for exploration
    float noveltyFactor = 1.0f + pImpl->noveltyLevel * 0.2f;
    
    // Combine scaling factors
    float totalScale = dopamineFactor * curiosityFactor * noveltyFactor;
    
    // Apply scaling to predictions
    for (auto& pred : predictions) {
        pred *= totalScale;
        pred = std::clamp(pred, 0.0f, 1.0f);
    }
}

std::vector<float> PredictionSystem::generateForwardPrediction(const std::vector<float>& state) {
    // Generate prediction using learned forward model
    std::vector<float> prediction = state;
    
    // If we have a learned forward model, use it
    if (!pImpl->forwardModel.empty() && pImpl->forwardModel.size() > 1) {
        // Use the most recent forward model mapping
        size_t currentSize = std::min(state.size(), pImpl->forwardModel[0].size());
        for (size_t i = 0; i < currentSize; ++i) {
            prediction[i] = pImpl->forwardModel[0][i] * pImpl->predictionGain;
        }
    } else {
        // Fallback: generate biologically plausible prediction
        // Simulate expected changes based on current state
        for (auto& pred : prediction) {
            // Add expected drift towards equilibrium
            pred = pred * 0.9f + 0.05f;
            
            // Add context-dependent modulation
            if (pred > 0.7f) {
                pred *= 0.95f; // Expect decay from high values
            } else if (pred < 0.3f) {
                pred *= 1.05f; // Expect growth from low values
            }
        }
    }
    
    return prediction;
}

void PredictionSystem::addPredictionNoise(std::vector<float>& predictions) {
    // Add biologically realistic noise for exploration
    static std::mt19937 rng(std::random_device{}());
    std::normal_distribution<float> noiseDist(0.0f, 0.02f);
    
    for (auto& pred : predictions) {
        // Scale noise based on confidence and neuromodulation
        float noiseScale = 0.05f * (1.0f - getConfidence());
        if (pImpl->dopamineLevel > 0.5f) {
            noiseScale *= 1.5f; // Higher dopamine increases exploration noise
        }
        
        pred += noiseDist(rng) * noiseScale;
        pred = std::clamp(pred, 0.0f, 1.0f);
    }
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;
    }
    
    // Calculate multiple error components (biologically plausible)
    float intensityError = 0.0f;
    float temporalError = 0.0f;
    float spatialError = 0.0f;
    
    size_t splitPoint = predicted.size() / 3;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float error = predicted[i] - actual[i];
        float squaredError = error * error;
        
        // Split error into components based on position
        if (i < splitPoint) {
            intensityError += squaredError;
        } else if (i < splitPoint * 2) {
            spatialError += squaredError;
        } else {
            temporalError += squaredError;
        }
    }
    
    // Combine errors with biologically inspired weighting
    float totalError = intensityError * 0.4f + spatialError * 0.3f + temporalError * 0.3f;
    totalError = std::sqrt(totalError) / std::sqrt(static_cast<float>(predicted.size()));
    
    return totalError;
}

void PredictionSystem::updateForwardModel(const std::vector<float>& predicted, const std::vector<float>& actual, float error) {
    // Learn forward model using prediction error (error-driven learning)
    
    // Shift existing forward model
    if (pImpl->forwardModel.size() >= pImpl->predictionHorizon) {
        pImpl->forwardModel.erase(pImpl->forwardModel.begin());
    }
    
    // Create new forward model entry
    std::vector<float> modelEntry;
    
    // Store actual state as learned consequence
    modelEntry = actual;
    
    // Apply error-based learning (predictive coding)
    float learningFactor = pImpl->learningRate * (1.0f - error);
    
    for (size_t i = 0; i < modelEntry.size() && i < predicted.size(); ++i) {
        // Hebbian-like update: strengthen connections that reduce error
        float delta = (actual[i] - predicted[i]) * learningFactor;
        modelEntry[i] += delta;
        modelEntry[i] = std::clamp(modelEntry[i], 0.0f, 1.0f);
    }
    
    pImpl->forwardModel.push_back(modelEntry);
}

void PredictionSystem::updatePredictionConfidence(float error) {
    // Update prediction confidence based on error and neuromodulation
    
    float confidenceChange = 0.0f;
    
    if (error < 0.1f) {
        // Low error increases confidence
        confidenceChange = pImpl->neuralAdaptationRate * 0.5f;
    } else if (error > 0.5f) {
        // High error decreases confidence
        confidenceChange = -pImpl->neuralAdaptationRate * 0.8f;
    }
    
    // Apply neuromodulation effects on confidence
    confidenceChange *= (1.0f + pImpl->dopamineLevel * 0.3f);
    confidenceChange *= (1.0f + pImpl->curiosityLevel * 0.2f);
    
    // Update confidence weights
    for (auto& weight : pImpl->confidenceWeights) {
        weight += confidenceChange;
        weight = std::clamp(weight, 0.1f, 1.0f);
    }
    
    // Update self-model confidence if available
    for (auto& selfConf : pImpl->selfModelConfidence) {
        selfConf += confidenceChange * 0.5f;
        selfConf = std::clamp(selfConf, 0.1f, 1.0f);
    }
}

void PredictionSystem::applySynapticPlasticity(float error) {
    // Apply synaptic plasticity based on prediction error
    
    // Scale plasticity with neuromodulators
    float plasticityScale = 1.0f;
    plasticityScale *= (1.0f + pImpl->dopamineLevel * 0.5f); // Dopamine enhances plasticity
    plasticityScale *= (1.0f - std::min(1.0f, error * 2.0f)); // High error reduces plasticity
    
    pImpl->synapticPlasticity = std::clamp(pImpl->synapticPlasticity * plasticityScale, 0.1f, 1.0f);
}

void PredictionSystem::learnFromObservation(const std::vector<float>& observation) {
    // Learn from sensory observation using Hebbian-like mechanisms
    
    // Create neural representation of observation
    std::vector<float> neuralRep = observation;
    
    // Apply homeostatic normalization
    float repNorm = 0.0f;
    for (float val : neuralRep) {
        repNorm += val * val;
    }
    repNorm = std::sqrt(repNorm);
    
    if (repNorm > 0.01f) {
        for (auto& val : neuralRep) {
            val /= repNorm;
        }
    }
    
    // Store in forward model
    if (pImpl->forwardModel.size() < pImpl->predictionHorizon) {
        pImpl->forwardModel.push_back(neuralRep);
    }
}

void PredictionSystem::adaptLearningParameters() {
    // Adapt learning parameters based on prediction performance
    
    if (pImpl->predictionHistory.size() > 10) {
        // Calculate recent prediction error
        float recentErrorSum = 0.0f;
        size_t recentCount = std::min(static_cast<size_t>(10), pImpl->predictionHistory.size());
        
        for (size_t i = 0; i < recentCount; ++i) {
            recentErrorSum += pImpl->predictionHistory[pImpl->predictionHistory.size() - 1 - i];
        }
        
        float avgError = recentErrorSum / recentCount;
        
        // Adapt learning rate based on error
        if (avgError < 0.1f) {
            // Good predictions - increase learning rate
            pImpl->learningRate = std::min(0.1f, pImpl->learningRate * 1.1f);
        } else if (avgError > 0.5f) {
            // Poor predictions - decrease learning rate
            pImpl->learningRate = std::max(0.001f, pImpl->learningRate * 0.9f);
        }
    }
}

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    
    if (brain_) {
        // Initialize neuromodulation references
        if (brain_->getDopamine()) {
            pImpl->dopamineLevel = brain_->getDopamine()->getLevel();
        }
        if (brain_->getCuriosity()) {
            pImpl->curiosityLevel = brain_->getCuriosity()->getLevel();
        }
        if (brain_->getNovelty()) {
            pImpl->noveltyLevel = brain_->getNovelty()->getLevel();
        }
        
        // Initialize SelfModel predictions
        if (brain_->getSelfModel()) {
            pImpl->selfModelPredictions.resize(10, 0.5f);
            pImpl->selfModelConfidence.resize(10, 0.5f);
        }
    }
    
    NLM_LOG_INFO("PredictionSystem initialized with enhanced predictive coding");
}
