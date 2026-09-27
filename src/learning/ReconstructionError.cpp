#include "ReconstructionError.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ReconstructionError::Impl {
    class Brain* brain;
    float learningRate;
    float errorThreshold;
    size_t bufferSize;
    
    // Current target and reconstruction
    std::vector<float> target;
    std::vector<float> reconstruction;
    
    // Error statistics
    float currentError;
    float mse;
    float maxError;
    
    // History tracking
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> reconstructionHistory;
    
    // Learning state
    bool enabled;
    size_t convergedSteps;
    size_t totalSteps;
    size_t stableSteps;
    
    // Reconstruction buffers
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    
    Impl() : brain(nullptr), learningRate(0.01f), errorThreshold(0.001f),
             bufferSize(1000), currentError(0.0f), mse(0.0f), maxError(0.0f),
             enabled(true), convergedSteps(0), totalSteps(0), stableSteps(0) {}
};

ReconstructionError::ReconstructionError() : pImpl(std::make_unique<Impl>()) {}

ReconstructionError::~ReconstructionError() = default;

void ReconstructionError::initialize(class Brain* brain, float learningRate, 
                                    size_t bufferSize) {
    pImpl->brain = brain;
    pImpl->learningRate = learningRate;
    pImpl->errorThreshold = 0.001f;
    pImpl->bufferSize = bufferSize;
    
    pImpl->errorHistory.reserve(bufferSize);
    pImpl->reconstructionHistory.reserve(bufferSize);
    
    NLM_LOG_INFO("Reconstruction error system initialized");
}

void ReconstructionError::setTarget(const std::vector<float>& target) {
    pImpl->target = target;
    
    // Initialize reconstruction with zeros
    pImpl->reconstruction.resize(target.size(), 0.0f);
    
    // Add to history
    if (pImpl->reconstructionHistory.size() < pImpl->bufferSize) {
        pImpl->reconstructionHistory.push_back(pImpl->reconstruction);
    }
}

float ReconstructionError::getError() const {
    return pImpl->currentError;
}

std::vector<float> ReconstructionError::getReconstruction() const {
    return pImpl->reconstruction;
}

void ReconstructionError::computeError(const std::vector<float>& currentState) {
    if (!pImpl->enabled || pImpl->target.empty()) {
        pImpl->currentError = 0.0f;
        return;
    }
    
    // Store current state as reconstruction
    pImpl->reconstruction = currentState;
    
    // Compute reconstruction error (Mean Squared Error)
    float sum = 0.0f;
    float maxDiff = 0.0f;
    
    size_t compareSize = std::min(pImpl->target.size(), currentState.size());
    
    for (size_t i = 0; i < compareSize; ++i) {
        float diff = pImpl->target[i] - currentState[i];
        sum += diff * diff;
        maxDiff = std::max(maxDiff, std::abs(diff));
    }
    
    // Average over number of elements
    size_t numElements = compareSize > 0 ? compareSize : 1;
    pImpl->mse = sum / static_cast<float>(numElements);
    pImpl->maxError = maxDiff;
    
    // Compute L1 error for additional metric
    pImpl->currentError = pImpl->mse; // Using MSE as primary error metric
    
    // Add to error history
    if (pImpl->errorHistory.size() < pImpl->bufferSize) {
        pImpl->errorHistory.push_back(pImpl->currentError);
    }
    
    // Add to reconstruction history
    if (pImpl->reconstructionHistory.size() < pImpl->bufferSize) {
        pImpl->reconstructionHistory.push_back(pImpl->reconstruction);
    }
    
    // Update step count
    pImpl->totalSteps++;
    
    // Check for convergence
    if (pImpl->currentError < pImpl->errorThreshold) {
        pImpl->convergedSteps++;
        pImpl->stableSteps++;
    } else {
        pImpl->stableSteps = 0;
    }
}

void ReconstructionError::applyLearning(class RandomGenerator& rng) {
    if (!pImpl->enabled || pImpl->target.empty() || !pImpl->brain) {
        return;
    }
    
    // Scale learning rate based on error
    float adaptiveLR = pImpl->learningRate * (1.0f + pImpl->currentError);
    
    // Apply reconstruction-based learning to brain systems
    // This can affect multiple brain components:
    
    // 1. Working memory: Strengthen connections for reconstruction targets
    if (pImpl->brain->getWorkingMemory()) {
        for (size_t i = 0; i < std::min(pImpl->target.size(), pImpl->reconstruction.size()); ++i) {
            float targetValue = pImpl->target[i];
            float currentValue = pImpl->reconstruction[i];
            
            // Only learn if there's a difference
            if (std::abs(targetValue - currentValue) > 0.01f) {
                // Add to working memory traces
                float traceStrength = std::abs(targetValue - currentValue) * adaptiveLR;
                pImpl->brain->getWorkingMemory()->storeToNeuron(
                    static_cast<NeuronId>(i % 1000),  // Use neuron ID as index
                    traceStrength
                );
            }
        }
    }
    
    // 2. Episodic memory: Store reconstruction as new episode
    if (pImpl->brain->getEpisodicMemory()) {
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->totalSteps;
        episode.reward = 0.0f;  // Reconstruction doesn't directly provide reward
        
        // Store reconstruction as episode content
        for (size_t i = 0; i < std::min(pImpl->target.size(), pImpl->reconstruction.size()); ++i) {
            episode.activeNeurons.push_back(static_cast<NeuronId>(i % 1000));
            episode.neuronActivations.push_back(pImpl->reconstruction[i]);
        }
        
        pImpl->brain->getEpisodicMemory()->storeEpisode(episode);
    }
    
    // 3. Prediction system: Use reconstruction as prediction error
    if (pImpl->brain->getPredictionSystem()) {
        // Use average reconstruction error as prediction error signal
        float predictionError = pImpl->currentError;
        
        // Apply to prediction system if available
        // (This would require additional prediction system API)
    }
    
    // 4. Neuromodulation: Increase acetylcholine for successful reconstruction
    if (pImpl->brain->getDopamine()) {
        // Successful reconstruction (low error) increases dopamine
        if (pImpl->currentError < pImpl->errorThreshold) {
            pImpl->brain->getDopamine()->signalReward(0.5f * (1.0f - pImpl->currentError));
        }
    }
}

bool ReconstructionError::hasConverged(float threshold) const {
    if (pImpl->stableSteps >= 10) {  // Require 10 consecutive stable steps
        return true;
    }
    return pImpl->currentError < threshold && pImpl->stableSteps > 0;
}

float ReconstructionError::getMeanSquaredError() const {
    return pImpl->mse;
}

float ReconstructionError::getMaxError() const {
    return pImpl->maxError;
}

size_t ReconstructionError::getBufferSize() const {
    return pImpl->bufferSize;
}

const std::vector<float>& ReconstructionError::getErrorHistory() const {
    return pImpl->errorHistory;
}

const std::vector<std::vector<float>>& ReconstructionError::getReconstructionHistory() const {
    return pImpl->reconstructionHistory;
}

void ReconstructionError::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->reconstructionHistory.clear();
    pImpl->convergedSteps = 0;
    pImpl->totalSteps = 0;
    pImpl->stableSteps = 0;
}

void ReconstructionError::enable(bool enable) {
    pImpl->enabled = enable;
}

bool ReconstructionError::isEnabled() const {
    return pImpl->enabled;
}

void ReconstructionError::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void ReconstructionError::setErrorThreshold(float threshold) {
    pImpl->errorThreshold = threshold;
}

void ReconstructionError::setBufferSize(size_t size) {
    pImpl->bufferSize = size;
    pImpl->errorHistory.reserve(size);
    pImpl->reconstructionHistory.reserve(size);
}

float ReconstructionError::getLearningProgress() const {
    if (pImpl->errorHistory.empty()) {
        return 0.0f;
    }
    
    // Compute average improvement over time
    size_t historySize = pImpl->errorHistory.size();
    if (historySize < 2) {
        return 0.0f;
    }
    
    float firstError = pImpl->errorHistory.front();
    float lastError = pImpl->errorHistory.back();
    
    return std::max(0.0f, 1.0f - (lastError / (firstError + 0.001f)));
}

size_t ReconstructionError::getConvergedSteps() const {
    return pImpl->convergedSteps;
}

} // namespace nlm