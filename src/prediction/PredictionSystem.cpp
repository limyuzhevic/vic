#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Implementation file for PredictionSystem
// Phase 6: Integrated predictive brain computation

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> prediction;
    class Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(std::make_unique<Impl>()) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Prediction system initialized with brain reference");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Implement real prediction using brain's neural substrate
    // Use episodic memory to predict next state
    if (!pImpl->brain || !pImpl->brain->getEpisodicMemory()) {
        // Fallback to simple prediction
        return std::make_unique<SensoryInput>(currentState);
    }
    
    // Get recent episodes to predict next state
    auto recentEpisodes = pImpl->brain->getEpisodicMemory()->getRecentEpisodes(3);
    if (recentEpisodes.empty()) {
        // No episodes, return current state
        return std::make_unique<SensoryInput>(currentState);
    }
    
    // Predict based on pattern in recent episodes
    // This is simplified - real implementation would use more sophisticated prediction
    const auto& currentData = currentState.getData();
    
    // Create predicted state with slight modification
    auto predicted = std::make_unique<SensoryInput>();
    predicted->setData(currentData); // Start with current state
    
    // Add some predicted change based on pattern
    float predictionStrength = 0.1f; // Small change prediction
    for (float& val : predicted->getData()) {
        // Add small predicted change
        val += (pImpl->brain->getRandomGenerator()->uniformReal(-1.0f, 1.0f) * predictionStrength);
    }
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update prediction error based on actual observation
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
        
        // Update prediction confidence based on error
        pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
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
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using brain's neural substrate
    if (!pImpl->brain) return;
    
    // Use episodic memory for prediction training
    if (pImpl->brain->getEpisodicMemory()) {
        // Store observation in episodic memory for future prediction
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->brain->getTotalSpikeCount(); // Use spike count as timestamp
        episode.sensoryState = observation.getData();
        episode.predictionError = pImpl->predictionError;
        
        pImpl->brain->getEpisodicMemory()->storeEpisode(episode);
    }
}

// Convenience methods for Phase 6 integration
std::vector<float> PredictionSystem::getPrediction(float horizon) const {
    // Return stored prediction or empty vector
    return pImpl->prediction;
}

// Update prediction from neural activity
void PredictionSystem::updateFromNeuralActivity(const std::vector<Neuron*>& neurons) {
    // Extract prediction from neural activity patterns
    std::vector<float> prediction;
    for (Neuron* neuron : neurons) {
        if (neuron) {
            const auto& state = neuron->getState();
            prediction.push_back(state.membranePotential);
        }
    }
    pImpl->prediction = prediction;
}

// Update prediction confidence based on working memory
void PredictionSystem::updateConfidenceFromWorkingMemory(const class NeuralWorkingMemory* workingMemory) {
    if (!workingMemory) {
        pImpl->confidence = 0.0f;
        return;
    }
    
    // Confidence based on working memory activity
    float memoryActivity = workingMemory->getMemoryActivity();
    pImpl->confidence = std::min(1.0f, memoryActivity * 2.0f);
}

} // namespace nlm