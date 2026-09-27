#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction state
    std::vector<float> currentPredictionPattern;
    std::vector<float> neuralPredictionBuffer;
    size_t predictionBufferSize;
    
    // Learning and adaptation
    float learningRate;
    float predictionHorizon;
    
    Impl() : predictionError(0.0f), confidence(0.5f)
        , predictionBufferSize(10)
        , learningRate(0.01f)
        , predictionHorizon(1.0f) {
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

// Enhanced prediction using neural working memory
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Check if working memory is available
    Brain* brain = findBrainContext();
    if (!brain) {
        // Fallback to placeholder
        return predictFromSimpleModel(currentState);
    }
    
    NeuralWorkingMemory* workingMemory = brain->getWorkingMemory();
    if (!workingMemory) {
        // No working memory, return simple prediction
        return predictFromSimpleModel(currentState);
    }
    
    // Get current working memory patterns
    std::vector<float> workingMemoryPattern = workingMemory->retrieve();
    
    if (!workingMemoryPattern.empty()) {
        // Neural prediction based on working memory patterns
        std::vector<float> predictedPattern = predictFromWorkingMemory(workingMemoryPattern, brain);
        return createSensoryInputFromPattern(predictedPattern);
    }
    
    // Fallback to placeholder
    return predictFromSimpleModel(currentState);
}

// Enhanced prediction error computation using neural mechanisms
void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Compute prediction error using multiple mechanisms
        float simpleError = computeSimplePredictionError(predData, actualData);
        float neuralError = computeNeuralPredictionError(predData, actualData);
        
        // Blend simple and neural error computation
        pImpl->predictionError = 0.5f * simpleError + 0.5f * neuralError;
        
        // Update prediction confidence based on error magnitude
        updatePredictionConfidence(pImpl->predictionError);
        
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Limit history size
        if (pImpl->errorHistory.size() > 100) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
    }
}

// Train prediction model using experience
void PredictionSystem::train(const SensoryInput& observation) {
    // Check if working memory is available
    Brain* brain = findBrainContext();
    if (!brain) return;
    
    NeuralWorkingMemory* workingMemory = brain->getWorkingMemory();
    if (!workingMemory) return;
    
    // Get current working memory for training
    std::vector<float> currentPattern = workingMemory->retrieve();
    
    if (!currentPattern.empty()) {
        // Update neural prediction model
        updateNeuralPredictionModel(currentPattern);
        
        // Get concepts for pattern completion
        ConceptFormation* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            size_t conceptId = conceptFormation->findConceptForPattern(currentPattern);
            if (conceptId > 0) {
                // Update prediction using concept knowledge
                std::vector<float> conceptPrototype = conceptFormation->getConceptPrototype(conceptId);
                updatePredictionFromConcept(conceptPrototype);
            }
        }
    }
    
    NLM_LOG_DEBUG("PredictionSystem: Trained prediction model");
}

// Get prediction error
float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

// Get prediction confidence
float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

// Get prediction error history
const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

// Clear prediction history
void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->neuralPredictionBuffer.clear();
}

// Update prediction confidence based on error
void PredictionSystem::updatePredictionConfidence(float error) {
    // Confidence decreases with larger prediction errors
    pImpl->confidence = std::max(0.1f, 1.0f - error * 2.0f);
}

// Compute prediction error using neural mechanisms
float PredictionSystem::computeNeuralPredictionError(const std::vector<float>& predicted, 
                                                   const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) return 1.0f;
    
    // Use working memory for pattern matching if available
    Brain* brain = findBrainContext();
    if (brain && brain->getWorkingMemory()) {
        // Get working memory patterns for context
        std::vector<float> wmPattern = brain->getWorkingMemory()->retrieve();
        if (!wmPattern.empty()) {
            // Compute similarity to working memory patterns
            float wmSimilarity = computePatternSimilarity(predicted, wmPattern);
            return 1.0f - wmSimilarity;  // Error inversely related to similarity
        }
    }
    
    // Fallback to simple Euclidean distance
    float sumSquared = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquared += diff * diff;
    }
    
    return std::sqrt(sumSquared / predicted.size());
}

// Compute pattern similarity using cosine similarity
float PredictionSystem::computePatternSimilarity(const std::vector<float>& a, 
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

// Find brain context (for integration with Brain class)
Brain* PredictionSystem::findBrainContext() {
    // This method should be implemented in the header file
    // For now, returning nullptr - integration will be done via Brain.cpp
    return nullptr;
}

// Create SensoryInput from pattern
std::unique_ptr<SensoryInput> PredictionSystem::createSensoryInputFromPattern(const std::vector<float>& pattern) {
    // Create a simple SensoryInput from the pattern
    // This is a simplified implementation
    std::unique_ptr<SensoryInput> input = std::make_unique<SensoryInput>();
    
    // Convert pattern to sensory input data
    // In a full implementation, this would properly encode the pattern
    // For now, just use the pattern as-is
    input->setData(pattern);
    
    return input;
}

// Predict from simple model
std::unique_ptr<SensoryInput> PredictionSystem::predictFromSimpleModel(const SensoryInput& currentState) {
    // Placeholder implementation
    return currentState.clone();
}

// Update neural prediction model using working memory patterns
void PredictionSystem::updateNeuralPredictionModel(const std::vector<float>& workingMemoryPattern) {
    // Add to prediction buffer
    pImpl->neuralPredictionBuffer.push_back(workingMemoryPattern);
    
    if (pImpl->neuralPredictionBuffer.size() > pImpl->predictionBufferSize) {
        pImpl->neuralPredictionBuffer.erase(pImpl->neuralPredictionBuffer.begin());
    }
    
    // Update current prediction
    if (!pImpl->neuralPredictionBuffer.empty()) {
        // Average the last few patterns
        pImpl->currentPredictionPattern.clear();
        size_t bufferSize = std::min(pImpl->neuralPredictionBuffer.size(), pImpl->predictionBufferSize);
        
        for (size_t i = pImpl->neuralPredictionBuffer.size() - bufferSize; 
             i < pImpl->neuralPredictionBuffer.size(); ++i) {
            const auto& pattern = pImpl->neuralPredictionBuffer[i];
            pImpl->currentPredictionPattern.insert(pImpl->currentPredictionPattern.end(), 
                                                  pattern.begin(), pattern.end());
        }
        
        // Average if multiple patterns
        if (bufferSize > 1) {
            for (size_t i = 0; i < pImpl->currentPredictionPattern.size(); ++i) {
                pImpl->currentPredictionPattern[i] /= bufferSize;
            }
        }
    }
}

// Update prediction from concept prototype
void PredictionSystem::updatePredictionFromConcept(const std::vector<float>& conceptPrototype) {
    // Blend current prediction with concept prototype
    if (pImpl->currentPredictionPattern.empty()) {
        pImpl->currentPredictionPattern = conceptPrototype;
        return;
    }
    
    if (conceptPrototype.size() == pImpl->currentPredictionPattern.size() && !conceptPrototype.empty()) {
        // Weighted average: 70% concept, 30% previous prediction
        for (size_t i = 0; i < conceptPrototype.size(); ++i) {
            pImpl->currentPredictionPattern[i] = 
                0.7f * conceptPrototype[i] + 0.3f * pImpl->currentPredictionPattern[i];
        }
    }
}

// Predict from working memory
std::vector<float> PredictionSystem::predictFromWorkingMemory(const std::vector<float>& workingMemoryPattern, Brain* brain) {
    std::vector<float> predictedPattern = workingMemoryPattern;
    
    if (!brain) return predictedPattern;
    
    // Apply concept formation if available
    ConceptFormation* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        size_t matchingConcept = conceptFormation->findConceptForPattern(workingMemoryPattern);
        if (matchingConcept > 0) {
            std::vector<float> prototype = conceptFormation->getConceptPrototype(matchingConcept);
            if (!prototype.empty()) {
                // Use concept prototype for prediction
                predictedPattern = prototype;
            }
        }
    }
    
    // Apply temporal dynamics if available
    TemporalRelation* temporalRelation = brain->getTemporalRelation();
    if (temporalRelation) {
        std::vector<float> temporalPrediction = temporalRelation->predictNext(workingMemoryPattern);
        if (!temporalPrediction.empty() && temporalPrediction.size() == predictedPattern.size()) {
            // Blend working memory with temporal prediction
            for (size_t i = 0; i < predictedPattern.size(); ++i) {
                predictedPattern[i] = 0.6f * predictedPattern[i] + 0.4f * temporalPrediction[i];
            }
        }
    }
    
    return predictedPattern;
}

// Compute simple prediction error
float PredictionSystem::computeSimplePredictionError(const std::vector<float>& predicted, 
                                                   const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) return 1.0f;
    
    float sumSquared = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquared += diff * diff;
    }
    
    return std::sqrt(sumSquared / predicted.size());
}

} // namespace nlm
