#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural integration
    Brain* brain;
    float lastUpdateTime;
    float predictionGain;  // How much prediction affects other systems
    
    // Current sensory state tracking
    std::unique_ptr<SensoryInput> lastPredictedState;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr), lastUpdateTime(0.0f), predictionGain(1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    clearHistory();
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // For now, create a copy and add some variation to simulate prediction
    auto predicted = currentState.clone();
    auto& data = predicted->getData();
    
    // Add small random variation to simulate prediction uncertainty
    float noise = 0.01f;  // 1% noise level
    for (float& value : data) {
        if (pImpl->brain && pImpl->brain->getRandomGenerator()) {
            value += ((pImpl->brain->getRandomGenerator()->uniformReal(0.0f, 1.0f) * 2.0f - 1.0f) * noise);
        } else {
            value += ((static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * noise);
        }
    }
    
    pImpl->lastPredictedState = predicted->clone();
    pImpl->confidence = std::max(0.1f, 1.0f - std::sqrt(pImpl->predictionError));
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // TODO PHASE 2: Implement real prediction error computation
    // PLACEHOLDER: Calculate simple error
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Update confidence based on error
        pImpl->confidence = std::max(0.1f, 1.0f - std::sqrt(pImpl->predictionError));
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
    // TODO PHASE 2: Train prediction model
    // For now, just store observation for future predictions
}

void PredictionSystem::update(float timestep) {
    pImpl->lastUpdateTime += timestep;
    
    // Apply prediction effects to other systems if brain is available
    if (pImpl->brain && pImpl->predictionGain > 0.0f) {
        float error = getPredictionError();
        float conf = getConfidence();
        
        // 1. Update neuromodulation (prediction error affects dopamine and curiosity)
        updateNeuromodulation(error, conf);
        
        // 2. Influence working memory (prediction error guides what to remember)
        updateWorkingMemory(error, conf);
        
        // 3. Guide attention (confidence directs attention)
        updateAttention(error, conf);
        
        // 4. Modulate plasticity (prediction error affects learning rate)
        updatePlasticity(error, conf);
    }
}

void PredictionSystem::updateNeuromodulation(float error, float confidence) {
    // Update dopamine with prediction error
    if (auto* dopamine = pImpl->brain->getDopamine()) {
        float predictionErrorSignal = error * 10.0f;  // Scale error
        predictionErrorSignal = std::clamp(predictionErrorSignal, -1.0f, 1.0f);
        
        // Dopamine acts as reward prediction error
        dopamine->setLevel(predictionErrorSignal);
        
        // Also update the prediction error neuromodulator if available
        if (auto* predError = pImpl->brain->getPredictionErrorSignal()) {
            predError->computeError(getPredictionError(), 0.0f);  // Error relative to zero prediction
        }
    }
    
    // Update curiosity based on prediction error and confidence
    if (auto* curiosity = pImpl->brain->getCuriosity()) {
        // High error + low confidence = high curiosity
        float curiosityLevel = error * (1.0f - confidence) * 5.0f;
        curiosityLevel = std::clamp(curiosityLevel, 0.0f, 1.0f);
        curiosity->update(curiosityLevel, error, 0.0f);
    }
}

void PredictionSystem::updateWorkingMemory(float error, float confidence) {
    if (auto* workingMemory = pImpl->brain->getWorkingMemory()) {
        // Store prediction error in working memory
        workingMemory->store({error, confidence}, 0.5f);
        
        // If prediction is good (low error, high confidence), strengthen memory
        if (error < 0.1f && confidence > 0.8f) {
            workingMemory->strengthenMemory(1.5f);
        }
        
        // High prediction error triggers memory search
        if (error > 0.5f) {
            workingMemory->runCompetition();
        }
    }
}

void PredictionSystem::updateAttention(float error, float confidence) {
    if (auto* attention = pImpl->brain->getAttention()) {
        // Apply attention bias based on prediction state
        // High confidence directs attention to predicted regions
        if (confidence > 0.8f) {
            // Attend to the most likely next state
            // In a real implementation, this would focus on predicted sensory regions
            // For now, just adjust inhibition strength based on confidence
            float inhibition = std::clamp(1.0f - confidence, 0.2f, 1.0f);
            attention->setInhibitionStrength(inhibition);
        }
        
        // High prediction error increases saliency and broadens attention
        if (error > 0.5f) {
            attention->applyTopDownBias(999, 1.0f);  // High bias for all neurons
        }
    }
}

void PredictionSystem::updatePlasticity(float error, float confidence) {
    // Modulate plasticity based on prediction state
    if (auto* stdp = pImpl->brain->getSTDP()) {
        // High prediction error increases plasticity
        float plasticityMod = 1.0f + error * 2.0f;  // Up to 3x normal plasticity
        
        // Low confidence also increases plasticity (need to learn)
        if (confidence < 0.5f) {
            plasticityMod *= 1.5f;
        }
        
        plasticityMod = std::clamp(plasticityMod, 0.5f, 3.0f);
        
        // Apply to STDP parameters
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        
        stdp->setLTPWeight(baseLTP * plasticityMod);
        stdp->setLTDWeight(baseLTD * plasticityMod);
    }
    
    // Hebbian learning can be modulated too
    if (auto* hebbian = pImpl->brain->getHebbian()) {
        // Similar modulation
        float plasticityMod = 1.0f + error * 2.0f;
        if (confidence < 0.5f) {
            plasticityMod *= 1.5f;
        }
        plasticityMod = std::clamp(plasticityMod, 0.5f, 3.0f);
        hebbian->setLearningRate(0.01f * plasticityMod);
    }
}

} // namespace nlm
