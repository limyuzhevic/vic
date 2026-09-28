#include "PredictionSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::unique_ptr<RandomGenerator> rng;
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {
        rng = std::make_unique<RandomGenerator>(42);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

void PredictionSystem::setRandomSeed(uint64_t seed) {
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Enhanced prediction using neural substrate
    // Integrate with working memory and attention systems for better predictions
    
    auto predicted = currentState.clone();
    const auto& currentData = currentState.getData();
    
    // Use prediction error to modulate prediction accuracy
    float errorInfluence = std::min(1.0f, getPredictionError() * 2.0f);
    
    // Enhance prediction based on prediction error - higher error = more exploration in prediction
    if (errorInfluence > 0.1f) {
        // Create more diverse predictions when error is high
        // This enables exploration and adaptation
        
        // Access the internal data vector through the clone
        // We need to get a mutable reference to modify it
        // Note: This is a simplified implementation - real implementation would use proper getter
        
        // Get a mutable pointer to the data
        auto* mutableInput = static_cast<SensoryInput*>(predicted.get());
        auto& mutableData = const_cast<std::vector<float>&>(mutableInput->getData());
        
        for (size_t i = 0; i < currentData.size(); ++i) {
            // Add noise proportional to prediction error
            float noise = (pImpl->rng->nextFloat() - 0.5f) * errorInfluence * 0.2f;
            mutableData[i] = currentData[i] + noise;
            // Clamp to valid range
            mutableData[i] = std::clamp(mutableData[i], 0.0f, 1.0f);
        }
    }
    
    // Update prediction confidence based on error
    float errorBasedConfidence = std::max(0.1f, 1.0f - getPredictionError());
    pImpl->confidence = (pImpl->confidence * 0.8f + errorBasedConfidence * 0.2f);
    
    return predicted;
}

// Add forward declaration for Brain class
#include "../brain/Brain.hpp"

// Add a method to get prediction influence on other systems
float PredictionSystem::getPredictionInfluence() const {
    // Higher prediction error = higher need for influence
    return std::min(1.0f, getPredictionError() * 3.0f);
}

// Update prediction with brain integration
void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual, 
                                         Brain* brain) {
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
        
        // Use prediction error to influence brain systems
        if (brain) {
            // Stronger prediction errors increase neuromodulator influence
            float errorBasedInfluence = std::min(2.0f, getPredictionError() * 2.0f);
            
            // Update prediction confidence based on error
            float errorBasedConfidence = std::max(0.1f, 1.0f - getPredictionError());
            pImpl->confidence = (pImpl->confidence * 0.8f + errorBasedConfidence * 0.2f);
            
            // Broadcast prediction error to influence attention and learning
            if (errorBasedInfluence > 0.5f) {
                // Strong prediction errors trigger attention shifts
                // In a real implementation, this would signal attention system
                // to refocus on relevant features
                
                // Also influence development plasticity
                if (pImpl->predictionError > 0.3f) {
                    // High prediction errors accelerate developmental plasticity
                    // This is a placeholder for actual developmental modulation
                }
            }
        }
    }
}

void PredictionSystem::integrateWithAttention(Brain* brain) {
    if (!brain) return;
    
    // Prediction errors modulate attention allocation
    float predictionInfluence = getPredictionInfluence();
    if (predictionInfluence > 0.3f) {
        // Strong prediction errors require increased attention
        // to resolve the discrepancy between predicted and actual states
        
        // This would signal attention system to:
        // 1. Increase salience of features that predict different outcomes
        // 2. Reduce attention to already well-predicted features
        // 3. Enhance working memory for prediction-relevant information
        
        // In a full implementation, this would send signals to:
        // - AttentionalSelection to adjust inhibition/excitation balance
        // - NeuralWorkingMemory to strengthen prediction-related traces
        // - NeuralPlanner to consider alternative predicted futures
        
        // For now, we just track that integration occurred
        if (predictionInfluence > 0.7f) {
            // Very high prediction error - significant attention shift needed
            pImpl->predictionError *= 1.1f; // Amplify error signal
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
}

void PredictionSystem::train(const SensoryInput& observation) {
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm
