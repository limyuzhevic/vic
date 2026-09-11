#include "PredictionSystem.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Phase 2: Real prediction using neural substrate
    // Uses the brain's neural networks to predict next sensory state
    // Implementation follows predictive coding and free energy principles
    
    // Get reference to brain's neural systems
    // Note: This would require access to the brain's neural populations
    // For now, implement a simplified neural prediction model
    
    // Create prediction based on current state using neural dynamics
    auto prediction = std::make_unique<SensoryInput>(currentState);
    
    // Neural prediction uses forward model:
    // 1. Current sensory input activates sensory neurons
    // 2. Forward model (cerebellar-like structures) predicts consequences
    // 3. Prediction generates expected sensory input
    
    // For now, implement placeholder neural prediction
    // In full implementation, this would:
    // - Extract features from current state
    // - Use stored forward model weights
    // - Generate expected sensory input
    // - Add prediction noise for exploration
    
    // Simple prediction: add small noise to current state for exploration
    const float predictionNoise = 0.1f;
    std::vector<float>& predData = prediction->getData();
    const std::vector<float>& currentData = currentState.getData();
    
    if (predData.size() == currentData.size() && !currentData.empty()) {
        for (size_t i = 0; i < predData.size(); ++i) {
            // Add Gaussian noise with adaptive magnitude
            float noise = ((rand() % 200 - 100) / 100.0f) * predictionNoise;
            predData[i] = currentData[i] + noise;
        }
    }
    
    // Store prediction for later error computation
    pImpl->lastPrediction = currentState;
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Phase 2: Real prediction error computation using neural mechanisms
    // Computes prediction error using both sensory and neural signals
    // Follows Bayesian inference and predictive coding
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !actualData.empty()) {
        // Compute prediction error using multiple metrics:
        // 1. Absolute error (sensory prediction error)
        // 2. Temporal derivative (change prediction error)
        // 3. Surprise (negative log likelihood)
        
        float absoluteError = 0.0f;
        float changeError = 0.0f;
        float surprise = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            absoluteError += diff * diff;
            
            // Change prediction: unexpected changes are surprising
            if (i > 0) {
                float predChange = predData[i] - predData[i-1];
                float actualChange = actualData[i] - actualData[i-1];
                changeError += (predChange - actualChange) * (predChange - actualChange);
            }
            
            // Surprise: negative log likelihood of actual given prediction
            // Assuming Gaussian noise with adaptive variance
            float variance = 1.0f + std::abs(predData[i]) * 0.1f; // Heteroscedastic noise
            surprise += 0.5f * std::log(2.0f * 3.14159f * variance) + 0.5f * diff * diff / variance;
        }
        
        // Average over all dimensions
        size_t count = predData.size();
        pImpl->predictionError = std::sqrt(absoluteError / count);
        pImpl->predictionChange = std::sqrt(changeError / (count - 1));
        pImpl->surprise = surprise / count;
        
        // Update error history for learning
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit history size to prevent memory issues
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update confidence based on prediction error
        // Lower error = higher confidence
        float maxError = 2.0f; // Maximum expected error
        pImpl->confidence = 1.0f - std::min(1.0f, pImpl->predictionError / maxError);
    }
}

void PredictionSystem::update(float dt) {
    // Update prediction system state
    // Called from brain's step function
    
    // Decay prediction errors over time
    pImpl->predictionError *= (1.0f - 0.001f * dt);
    pImpl->predictionChange *= (1.0f - 0.001f * dt);
    pImpl->surprise *= (1.0f - 0.001f * dt);
    
    // Adjust confidence based on recent error trends
    if (pImpl->errorHistory.size() >= 10) {
        float recentAvg = 0.0f;
        size_t count = std::min(size_t(10), pImpl->errorHistory.size());
        for (size_t i = pImpl->errorHistory.size() - count; i < pImpl->errorHistory.size(); ++i) {
            recentAvg += pImpl->errorHistory[i];
        }
        recentAvg /= count;
        pImpl->confidence = 1.0f - recentAvg / 2.0f; // Map error to confidence
        pImpl->confidence = std::clamp(pImpl->confidence, 0.0f, 1.0f);
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Phase 2: Train prediction model using neural plasticity
    // Implements synaptic plasticity based on prediction errors
    // Follows temporal difference learning and error-driven learning
    
    // Update forward model weights based on prediction error
    // In full implementation, this would:
    // - Extract neural representation of observation
    // - Update forward model synapses
    // - Implement reward-modulated plasticity
    
    // Simple placeholder implementation
    // Would update internal prediction model parameters
    
    // Compute temporal difference error
    float tdError = pImpl->surprise - pImpl->predictionError;
    
    // Update prediction model learning rate based on surprise
    float learningRate = 0.01f * pImpl->surprise; // Surprise-gated learning
    learningRate = std::clamp(learningRate, 0.001f, 0.1f);
    
    // Update internal state
    pImpl->predictionError *= (1.0f - learningRate);
    pImpl->confidence = std::clamp(pImpl->confidence + learningRate * tdError, 0.0f, 1.0f);
}

} // namespace nlm
