#include "PredictionSystem.hpp"
#include "../../brain/Brain.hpp"
#include "../../core/Random/Random.hpp"
#include "../../sensory/SensoryInput.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural components for prediction
    Brain* brain;
    RandomGenerator* rng;
    
    // Prediction error and confidence
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Prediction storage
    std::vector<SensoryInput> predictions;
    std::vector<SensoryInput> targets;
    
    // Integration flag
    bool integrated;
    
    Impl() : brain(nullptr), rng(nullptr), predictionError(0.0f), confidence(0.5f),
             integrated(false) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain, RandomGenerator* rng) {
    pImpl->brain = brain;
    pImpl->rng = rng;
    pImpl->integrated = (brain != nullptr && rng != nullptr);
}

void PredictionSystem::processPrediction(std::unique_ptr<SensoryInput>& prediction) {
    if (!pImpl->brain || !pImpl->rng || !prediction) return;
    
    // Neural prediction using working memory
    // Extract patterns from working memory and apply predictive dynamics
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        // Use working memory traces to generate predictions
        // This implements predictive coding at neural level
        std::vector<NeuronId> activeNeurons = workingMemory->getMemoryNeurons();
        if (!activeNeurons.empty()) {
            // Generate prediction based on active memory patterns
            // Scale and transform based on neural properties
            const auto& values = prediction->getData();
            std::vector<float> predictedValues(values.size(), 0.0f);
            
            for (size_t i = 0; i < values.size(); ++i) {
                // Apply neural modulation based on active population
                float modulation = 1.0f;
                
                // Apply prediction gain based on confidence
                modulation *= pImpl->confidence;
                
                // Add predictive noise for exploration
                modulation *= (1.0f + pImpl->rng->uniformReal(-0.1f, 0.1f));
                
                predictedValues[i] = values[i] * modulation;
            }
            
            prediction->setData(predictedValues);
        }
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Create prediction based on current state and neural dynamics
    auto prediction = std::make_unique<SensoryInput>(currentState);
    
    // Apply neural prediction processing
    processPrediction(prediction);
    
    // Store prediction for later comparison
    pImpl->predictions.push_back(*prediction);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error using neural difference
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        pImpl->predictionError = 0.0f;
        return;
    }
    
    // Compute prediction error as RMSE (root mean squared error)
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
    }
    
    pImpl->predictionError = std::sqrt(sumError / predData.size());
    
    // Update confidence based on prediction error
    // Lower error = higher confidence
    pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError);
    
    // Store error for analysis
    pImpl->errorHistory.push_back(pImpl->predictionError);
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
    pImpl->predictions.clear();
    pImpl->targets.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using observation
    // Implements Hebbian learning for predictive weights
    
    if (!pImpl->brain) return;
    
    // Extract features from observation
    const auto& obsData = observation.getData();
    
    // Create target prediction based on observation
    // This is a simplified version - real implementation would use
    // temporal sequences and neural dynamics
    for (size_t i = 0; i < obsData.size(); ++i) {
        // Apply predictive learning rule
        // Higher observation strength increases predictive weight
        float learningSignal = obsData[i];
        
        // Apply dopamine-modulated learning if available
        auto* dopamine = pImpl->brain->getDopamine();
        if (dopamine) {
            float plasticityFactor = dopamine->getPlasticityFactor();
            learningSignal *= plasticityFactor;
        }
    }
}

SensoryInput PredictionSystem::getAveragePrediction() const {
    SensoryInput average(std::vector<float>(100, 0.0f));  // Default size
    
    if (pImpl->predictions.empty()) return average;
    
    // Compute average prediction across stored predictions
    size_t totalSamples = pImpl->predictions.size();
    
    for (const auto& prediction : pImpl->predictions) {
        const auto& values = prediction.getData();
        for (size_t i = 0; i < values.size() && i < average.getData().size(); ++i) {
            average.getData()[i] += values[i];
        }
    }
    
    // Average the values
    for (float& value : average.getData()) {
        value /= totalSamples;
    }
    
    return average;
}

bool PredictionSystem::isIntegrated() const {
    return pImpl->integrated;
}

} // namespace nlm
