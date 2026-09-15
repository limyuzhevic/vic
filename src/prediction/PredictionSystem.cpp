#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../sensory/SensoryInput.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    // Integration with brain
    Brain* brainContext;
    NeuralWorkingMemory* memoryContext;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brainContext(nullptr), memoryContext(nullptr) {}
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

void PredictionSystem::setInputProvider(SensoryInput* input) {
    // Store input provider for integration
    pImpl->inputProvider = input;
}

void PredictionSystem::setMemoryProvider(void* memory) {
    // Store memory provider for integration
    pImpl->memoryProvider = memory;
}

void PredictionSystem::integrateWithBrain(Brain* brain) {
    // Connect prediction system with brain's sensory and memory systems
    pImpl->brainContext = brain;
    
    // Get working memory from brain for integration
    if (brain && brain->getWorkingMemory()) {
        pImpl->memoryContext = brain->getWorkingMemory();
    }
    
    // Enable prediction system updates during brain simulation
    if (pImpl->brainContext) {
        pImpl->brainContext->getPredictionSystem()->setInputProvider(this);
    }
}

} // namespace nlm

