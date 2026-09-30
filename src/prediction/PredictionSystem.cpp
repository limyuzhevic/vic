#include "PredictionSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Integration points
    Brain* brain;
    NeuralPrediction* neuralPrediction;
    PredictionError* predictionErrorSignal;
    NeuralPlanner* planner;
    ConceptFormation* conceptFormation;
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    
    // Configuration
    float predictionUpdateInterval;
    float confidenceUpdateRate;
    float errorHistorySize;
    bool enablePredictiveAttention;
    bool enablePredictivePlanning;
    bool enableTemporalLearning;
    
    Impl() : predictionError(0.0f), confidence(0.5f),
             predictionUpdateInterval(1.0f), confidenceUpdateRate(0.1f),
             errorHistorySize(1000.0f), enablePredictiveAttention(true),
             enablePredictivePlanning(true), enableTemporalLearning(true) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Get references to integrated systems
    pImpl->neuralPrediction = brain->getNeuralPrediction();
    pImpl->predictionErrorSignal = brain->getPredictionErrorSignal();
    pImpl->planner = brain->getPlanner();
    pImpl->conceptFormation = brain->getConceptFormation();
    pImpl->workingMemory = brain->getWorkingMemory();
    pImpl->episodicMemory = brain->getEpisodicMemory();
    
    NLM_LOG_INFO("PredictionSystem initialized with all integrated modules");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->neuralPrediction) {
        NLM_LOG_ERROR("NeuralPrediction system not available");
        return currentState.clone();
    }
    
    // Convert sensory input to neural pattern representation
    const auto& currentData = currentState.getData();
    
    // Use NeuralPrediction to generate prediction based on learned temporal sequences
    std::vector<float> predictedPattern;
    if (pImpl->enableTemporalLearning && !currentData.empty()) {
        // Get current step from brain simulation time
        SimulationStep currentStep = pImpl->brain->getCurrentStep();
        
        // Generate prediction based on learned sequences
        predictedPattern = pImpl->neuralPrediction->generatePrediction(currentStep);
        
        // If neural prediction returns empty, use current pattern with noise
        if (predictedPattern.empty()) {
            predictedPattern = currentData;
            // Add some variation to simulate prediction uncertainty
            for (float& value : predictedPattern) {
                value += ((rand() % 100) / 100.0f - 0.5f) * 0.2f;
            }
        }
    } else {
        predictedPattern = currentData;
    }
    
    // Convert back to SensoryInput
    auto predictedInput = std::make_unique<SensoryInput>(SensoryInputType::Internal, predictedPattern);
    
    NLM_LOG_DEBUG("Generated prediction based on temporal sequences: " + 
                 std::to_string(predictedPattern.size()) + " values");
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error using the sophisticated PredictionError signal system
    if (pImpl->predictionErrorSignal) {
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() == actualData.size() && !predData.empty()) {
            // Compute prediction error components
            pImpl->predictionErrorSignal->computeError(predData, actualData);
            float error = pImpl->predictionErrorSignal->getError();
            
            // Update neural prediction with the error
            if (pImpl->neuralPrediction) {
                SimulationStep currentStep = pImpl->brain->getCurrentStep();
                pImpl->neuralPrediction->updateWithObservation(actualData, currentStep);
            }
            
            // Update confidence based on error magnitude
            updateConfidence(error);
            
            // Store error history
            pImpl->errorHistory.push_back(error);
            if (pImpl->errorHistory.size() > pImpl->errorHistorySize) {
                pImpl->errorHistory.erase(pImpl->errorHistory.begin());
            }
            
            // Trigger neuromodulation based on prediction error
            triggerPredictionErrorModulation(error);
            
            // Update episodic memory with prediction failure
            if (pImpl->episodicMemory && error > pImpl->predictionErrorSignal->getSurpriseThreshold()) {
                storePredictionFailure(predData, actualData, error);
            }
            
            NLM_LOG_INFO("Prediction updated: error = " + std::to_string(error) + 
                        ", confidence = " + std::to_string(pImpl->confidence));
        }
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
    if (pImpl->predictionErrorSignal) {
        pImpl->predictionErrorSignal->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (pImpl->neuralPrediction && pImpl->enableTemporalLearning) {
        const auto& observationData = observation.getData();
        
        if (!observationData.empty()) {
            SimulationStep currentStep = pImpl->brain->getCurrentStep();
            
            // Record sensory state for future prediction
            pImpl->neuralPrediction->recordSensoryState(observationData, currentStep);
            
            NLM_LOG_DEBUG("Training prediction system with observation: " + 
                         std::to_string(observationData.size()) + " values");
        }
    }
}

void PredictionSystem::updateConfidence(float error) {
    // Update prediction confidence based on prediction error
    // High error reduces confidence, low error increases confidence
    float confidenceChange = pImpl->confidenceUpdateRate * (1.0f - std::abs(error));
    
    if (error > 0.5f) {
        // Large error reduces confidence significantly
        pImpl->confidence *= (1.0f - confidenceChange * 2.0f);
    } else if (error > 0.1f) {
        // Medium error reduces confidence moderately
        pImpl->confidence *= (1.0f - confidenceChange);
    } else {
        // Small error may increase confidence
        pImpl->confidence *= (1.0f + confidenceChange * 0.5f);
    }
    
    // Clamp confidence to reasonable range
    pImpl->confidence = std::clamp(pImpl->confidence, 0.1f, 1.0f);
    
    // Update neural prediction confidence
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->setPredictionConfidence(pImpl->confidence);
    }
}

void PredictionSystem::triggerPredictionErrorModulation(float error) {
    if (pImpl->predictionErrorSignal && pImpl->brain->getDopamine()) {
        // Prediction error affects dopamine (reward prediction error signal)
        // Large prediction error = potential learning opportunity
        float modulationStrength = error * 2.0f;  // Amplify for neuromodulation
        
        if (modulationStrength > 0.3f) {
            // Significant prediction error triggers enhanced dopamine release
            // This signals important learning opportunity
            float currentDopamine = pImpl->brain->getDopamine()->getLevel();
            float enhancedDopamine = std::min(1.0f, currentDopamine + modulationStrength * 0.5f);
            pImpl->brain->getDopamine()->setLevel(enhancedDopamine);
            
            NLM_LOG_INFO("Prediction error triggered enhanced dopaminergic learning: " + 
                        std::to_string(modulationStrength));
        }
        
        // Update prediction error signal history
        pImpl->predictionErrorSignal->recordError(error, pImpl->brain->getCurrentStep());
    }
}

void PredictionSystem::storePredictionFailure(const std::vector<float>& predicted,
                                             const std::vector<float>& actual,
                                             float error) {
    if (pImpl->episodicMemory) {
        // Store prediction failure as an important episodic memory
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->brain->getCurrentStep();
        episode.reward = pImpl->brain->getDopamine() ? pImpl->brain->getDopamine()->getLevel() : 0.0f;
        
        // Store pattern information
        episode.activeNeurons.clear();
        episode.neuronActivations.clear();
        
        // Store prediction and actual values as memory content
        episode.predictedPattern = predicted;
        episode.actualPattern = actual;
        
        pImpl->episodicMemory->storeEpisode(episode);
        
        NLM_LOG_DEBUG("Stored prediction failure as episodic memory (error: " + std::to_string(error) + ")");
    }
}

void PredictionSystem::configure(float updateInterval, float confidenceRate, 
                                float historySize, bool predictiveAttention,
                                bool predictivePlanning, bool temporalLearning) {
    pImpl->predictionUpdateInterval = updateInterval;
    pImpl->confidenceUpdateRate = confidenceRate;
    pImpl->errorHistorySize = historySize;
    pImpl->enablePredictiveAttention = predictiveAttention;
    pImpl->enablePredictivePlanning = predictivePlanning;
    pImpl->enableTemporalLearning = temporalLearning;
    
    NLM_LOG_INFO("PredictionSystem configured: updateInterval=" + std::to_string(updateInterval) +
                ", predictiveAttention=" + std::to_string(predictiveAttention) +
                ", predictivePlanning=" + std::to_string(predictivePlanning));
}

} // namespace nlm