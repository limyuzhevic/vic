#include "../memory/NeuralEpisodicMemory.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    NeuralEpisodicMemory* episodicMemory;  // Reference to episodic memory for pattern completion
    
    Impl() : predictionError(0.0f), confidence(0.5f), episodicMemory(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // TODO PHASE 2: Implement real prediction using NLM's neural substrate
    // PLACEHOLDER: Just return a copy of current state
    return currentState.clone();
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
}

// Connect episodic memory for pattern completion and prediction
void PredictionSystem::setEpisodicMemory(NeuralEpisodicMemory* episodicMemory) {
    pImpl->episodicMemory = episodicMemory;
}

// Get episodes similar to current pattern for prediction
std::vector<const EpisodicMemoryItem*> PredictionSystem::getSimilarEpisodes(const std::vector<float>& pattern, size_t maxResults) const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->retrieveSimilar(pattern, maxResults);
    }
    return {};
}

// Make prediction based on episodic memory patterns
std::unique_ptr<SensoryInput> PredictionSystem::predictFromEpisodicMemory(const std::vector<float>& currentPattern) const {
    if (!pImpl->episodicMemory) {
        return nullptr;
    }
    
    // Get similar episodes from episodic memory
    std::vector<const EpisodicMemoryItem*> similarEpisodes = 
        pImpl->episodicMemory->retrieveSimilar(currentPattern, 3);
    
    if (similarEpisodes.empty()) {
        // If no similar episodes, return a null prediction (will fallback to default)
        return nullptr;
    }
    
    // Calculate average pattern from similar episodes for prediction
    std::vector<float> predictedPattern;
    size_t patternSize = similarEpisodes[0]->sensoryState.size();
    
    if (patternSize == 0) return nullptr;
    
    predictedPattern.resize(patternSize, 0.0f);
    
    for (const auto* episode : similarEpisodes) {
        for (size_t i = 0; i < patternSize && i < episode->sensoryState.size(); ++i) {
            predictedPattern[i] += episode->sensoryState[i];
        }
    }
    
    // Average the patterns
    float count = static_cast<float>(similarEpisodes.size());
    for (size_t i = 0; i < predictedPattern.size(); ++i) {
        predictedPattern[i] /= count;
    }
    
    // Create a sensory input from the predicted pattern
    // (In real implementation, this would create proper SensoryInput objects)
    auto sensoryInput = std::make_unique<SensoryInput>();
    sensoryInput->addData(predictedPattern);
    
    return sensoryInput;
}

} // namespace nlm
