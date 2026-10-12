#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    std::vector<float> currentPrediction;
    std::vector<float> targetPrediction;
    std::vector<float> predictionWeights;
    
    // Temporal components for prediction
    std::deque<float> temporalTrace;
    float predictionDecay;
    
    // Integration with brain systems
    Brain* brain;
    
    // Memory integration
    std::vector<float> episodicPattern;
    float episodicConfidence;
    
    Impl() : predictionError(0.0f), confidence(0.5f), predictionDecay(0.99f),
             episodicConfidence(0.0f) {
        temporalTrace.reserve(100);
        predictionWeights.reserve(50);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    pImpl->brain = nullptr;
    pImpl->currentPrediction.reserve(50);
    pImpl->targetPrediction.reserve(50);
    
    // Initialize prediction weights based on neural connectivity patterns
    for (int i = 0; i < 50; ++i) {
        pImpl->predictionWeights.push_back(0.1f * (0.5f + 0.5f * std::sin(static_cast<float>(i) * 0.1f)));
    }
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem: Brain not initialized, returning empty prediction");
        return std::make_unique<SensoryInput>();
    }
    
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return std::make_unique<SensoryInput>();
    }
    
    // Neural prediction using brain-based forward model
    // Project current state through learned prediction weights
    pImpl->currentPrediction = currentData;
    pImpl->targetPrediction.resize(pImpl->currentPrediction.size());
    
    // Apply neural prediction mechanism
    // 1. Extract current sensory pattern
    // 2. Apply prediction weights (simulating forward model)
    // 3. Use episodic memory patterns for contextual prediction
    // 4. Apply neuromodulation to adjust prediction confidence
    
    for (size_t i = 0; i < pImpl->currentPrediction.size(); ++i) {
        // Base prediction from current state
        float prediction = pImpl->currentPrediction[i];
        
        // Apply prediction weight modulation
        if (i < pImpl->predictionWeights.size()) {
            prediction *= pImpl->predictionWeights[i];
        }
        
        // Apply episodic memory influence if available
        if (!pImpl->episodicPattern.empty() && i < pImpl->episodicPattern.size()) {
            prediction = prediction * 0.7f + pImpl->episodicPattern[i] * 0.3f * pImpl->episodicConfidence;
        }
        
        // Apply prediction decay for temporal consistency
        if (!pImpl->temporalTrace.empty()) {
            float avgTrace = 0.0f;
            for (float trace : pImpl->temporalTrace) {
                avgTrace += trace;
            }
            avgTrace /= pImpl->temporalTrace.size();
            prediction = prediction * 0.9f + avgTrace * 0.1f;
        }
        
        // Apply neuromodulation effects (primarily dopamine)
        if (auto* dopamine = pImpl->brain->getDopamine()) {
            float dopamineLevel = dopamine->getLevel();
            // Higher dopamine increases prediction exploration
            prediction += (prediction - 0.5f) * dopamineLevel * 0.1f;
        }
        
        // Apply curiosity modulation
        if (auto* curiosity = pImpl->brain->getCuriosity()) {
            float curiosityLevel = curiosity->getLevel();
            // Curiosity increases prediction uncertainty
            prediction *= (1.0f + curiosityLevel * 0.1f);
        }
        
        pImpl->targetPrediction[i] = prediction;
    }
    
    // Update temporal trace
    pImpl->temporalTrace.push_back(pImpl->currentPrediction[0]);  // Use first element as representative
    if (pImpl->temporalTrace.size() > 100) {
        pImpl->temporalTrace.pop_front();
    }
    
    // Create prediction sensory input
    auto predictedInput = std::make_unique<SensoryInput>(pImpl->targetPrediction);
    
    // Calculate prediction confidence based on similarity to history
    float predictionVariance = 0.0f;
    for (size_t i = 0; i < pImpl->currentPrediction.size(); ++i) {
        float diff = pImpl->targetPrediction[i] - pImpl->currentPrediction[i];
        predictionVariance += diff * diff;
    }
    pImpl->confidence = std::max(0.1f, 1.0f - predictionVariance / pImpl->currentPrediction.size());
    
    NLM_LOG_DEBUG("PredictionSystem: Generated prediction with confidence " + std::to_string(pImpl->confidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update predictions based on actual observation using neural learning mechanisms
    if (!pImpl->brain) {
        // Fallback to original simple error calculation
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
            
            // Keep history bounded
            if (pImpl->errorHistory.size() > 1000) {
                pImpl->errorHistory.erase(pImpl->errorHistory.begin());
            }
        }
        return;
    }
    
    // Neural prediction error computation with neuromodulation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumSquaredError = 0.0f;
        float predictionAccuracy = 0.0f;
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumSquaredError += diff * diff;
            
            // Compute prediction accuracy with temporal decay
            float temporalWeight = 1.0f;
            if (!pImpl->temporalTrace.empty()) {
                // More recent predictions have higher weight
                temporalWeight = pImpl->temporalTrace.back();
            }
            predictionAccuracy += (1.0f - std::abs(diff)) * temporalWeight;
        }
        
        pImpl->predictionError = sumSquaredError / predData.size();
        pImpl->confidence = predictionAccuracy / predData.size();
        
        // Update error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Keep history bounded
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Update prediction weights based on error (Hebbian-like learning)
        for (size_t i = 0; i < std::min(pImpl->predictionWeights.size(), predData.size()); ++i) {
            // Stronger error -> larger weight adjustment
            float errorGradient = -2.0f * (predData[i] - actualData[i]);
            float weightChange = errorGradient * 0.01f * pImpl->confidence;
            
            // Apply neuromodulation to weight changes
            if (auto* dopamine = pImpl->brain->getDopamine()) {
                float dopamineLevel = dopamine->getLevel();
                weightChange *= (1.0f + dopamineLevel * 0.5f);
            }
            
            pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i] + weightChange, 0.01f, 2.0f);
        }
        
        // Update episodic pattern based on prediction error
        if (pImpl->predictionError < 0.1f) {
            // Low error -> reinforce current pattern
            pImpl->episodicPattern = predData;
            pImpl->episodicConfidence = std::min(1.0f, pImpl->episodicConfidence + 0.1f);
        } else {
            // High error -> weaken current pattern
            pImpl->episodicConfidence *= 0.8f;
            pImpl->episodicPattern = actualData;  // Learn from actual outcome
        }
        
        // Apply curiosity-based exploration learning
        if (auto* curiosity = pImpl->brain->getCuriosity()) {
            float curiosityLevel = curiosity->getLevel();
            if (curiosityLevel > 0.5f && pImpl->predictionError > 0.5f) {
                // Explore high-error predictions
                pImpl->confidence *= (1.0f + curiosityLevel * 0.2f);
            }
        }
        
        NLM_LOG_DEBUG("PredictionSystem: Updated predictions, error=" + std::to_string(pImpl->predictionError) + 
                     ", confidence=" + std::to_string(pImpl->confidence));
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
    pImpl->temporalTrace.clear();
    pImpl->currentPrediction.clear();
    pImpl->targetPrediction.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction system using neural learning mechanisms
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem: Brain not initialized, skipping training");
        return;
    }
    
    const auto& obsData = observation.getData();
    if (obsData.empty()) {
        return;
    }
    
    // Store observation in episodic pattern for future predictions
    pImpl->episodicPattern = obsData;
    
    // Update prediction weights based on observation (Hebbian learning)
    for (size_t i = 0; i < std::min(pImpl->predictionWeights.size(), obsData.size()); ++i) {
        float obsValue = obsData[i];
        
        // Increase weight for predictive features
        pImpl->predictionWeights[i] += obsValue * 0.001f;
        
        // Apply synaptic-like saturation
        pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i], 0.01f, 2.0f);
    }
    
    // Update confidence based on observation consistency
    float obsConsistency = 0.5f;
    if (!pImpl->temporalTrace.empty()) {
        float avgTrace = 0.0f;
        for (float trace : pImpl->temporalTrace) {
            avgTrace += trace;
        }
        avgTrace /= pImpl->temporalTrace.size();
        obsConsistency = 1.0f - std::abs(obsData[0] - avgTrace);
    }
    
    pImpl->episodicConfidence = std::min(1.0f, pImpl->episodicConfidence + obsConsistency * 0.1f);
    
    NLM_LOG_INFO("PredictionSystem: Trained with observation, confidence=" + std::to_string(pImpl->episodicConfidence));
}

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with neural integration");
}

} // namespace nlm
