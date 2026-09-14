#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    NeuralPredictionModel neuralModel;
    
    // Prediction system parameters
    float predictionGain;
    float predictionDecay;
    
    // Current state tracking
    std::vector<float> currentNeuralState;
    std::vector<float> predictedNeuralState;
    
    // Prediction error from neural dynamics
    float neuralPredictionError;
    
    // Reward prediction error from neuromodulation
    float rewardPredictionError;
    
    // Integration state
    SimulationStep predictionStep;
    
    Impl() : brain(nullptr), predictionGain(0.5f), predictionDecay(0.99f),
             neuralPredictionError(0.0f), rewardPredictionError(0.0f), predictionStep(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->neuralModel.initialize(brain);
    
    NLM_LOG_INFO("PredictionSystem initialized with neural substrate");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    // Convert sensory input to neural activity pattern
    const auto& sensoryData = currentState.getData();
    
    // Store current state in working memory
    if (pImpl->brain->getWorkingMemory()) {
        pImpl->brain->getWorkingMemory()->store(sensoryData, 1.0f);
    }
    
    // Update neural prediction model with current state
    std::vector<float> currentNeural(sensoryData.begin(), sensoryData.end());
    pImpl->currentNeuralState = currentNeural;
    
    // Use neural prediction model to predict next state
    std::vector<float> predictedNeural = pImpl->neuralModel.predictNextState(currentNeural);
    pImpl->predictedNeuralState = predictedNeural;
    
    // Create predicted sensory input (as Vision for now)
    auto predictedInput = std::make_unique<Vision>(predictedNeural.size(), 1, 1);
    predictedInput->setData(predictedNeural);
    
    // Update step counter
    pImpl->predictionStep++;
    
    NLM_LOG_DEBUG("PredictionSystem: Predicted next state using neural model");
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                         const SensoryInput& actual) {
    if (!pImpl->brain) return;
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Convert to neural patterns for error computation
    std::vector<float> predNeural(predData.begin(), predData.end());
    std::vector<float> actualNeural(actualData.begin(), actualData.end());
    
    // Compute neural prediction error
    float error = pImpl->neuralModel.computeNeuralPredictionError(predNeural, actualNeural);
    
    // Get reward prediction error from neuromodulation
    auto* predictionError = pImpl->brain->getPredictionErrorSignal();
    if (predictionError) {
        error += predictionError->getError();
    }
    
    // Apply prediction gain
    error *= pImpl->predictionGain;
    
    // Update neural prediction model with actual observation
    pImpl->neuralModel.train(predNeural, actualNeural);
    
    // Store error in history
    pImpl->neuralPredictionError = error;
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    
    // Update working memory with actual observation
    if (pImpl->brain->getWorkingMemory()) {
        pImpl->brain->getWorkingMemory()->store(actualData, 1.0f - error);
    }
    
    // Update episodic memory if error is high (unexpected event)
    if (error > 0.3f) {
        if (pImpl->brain->getEpisodicMemory()) {
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->predictionStep;
            episode.sensoryState = predNeural;
            episode.resultingSensoryState = actualNeural;
            episode.action = ActionType::Wait;  // Could be enhanced
            episode.reward = 1.0f - error;
            
            pImpl->brain->getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Updated predictions, error = " + std::to_string(error));
}

float PredictionSystem::getPredictionError() const {
    return pImpl->neuralPredictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->neuralModel.getPredictionConfidence();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    // Update neural prediction model with observation
    const auto& data = observation.getData();
    std::vector<float> observationNeural(data.begin(), data.end());
    
    // Get current working memory state
    std::vector<float> currentMemory = pImpl->brain->getWorkingMemory()->retrieve();
    
    // Train prediction model
    pImpl->neuralModel.train(currentMemory, observationNeural);
    
    // Update prediction with reward
    auto* dopamine = pImpl->brain->getDopamine();
    if (dopamine) {
        pImpl->neuralModel.updateWithReward(dopamine->getLevel());
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Trained prediction model");
}

void PredictionSystem::updateWithReward(float reward) {
    pImpl->neuralModel.updateWithReward(reward);
}

const std::vector<float>& PredictionSystem::getNeuralPrediction() const {
    return pImpl->predictedNeuralState;
}

void PredictionSystem::setPredictionGain(float gain) {
    pImpl->predictionGain = gain;
}

void PredictionSystem::setPredictionDecay(float decay) {
    pImpl->predictionDecay = decay;
}

} // namespace nlm
