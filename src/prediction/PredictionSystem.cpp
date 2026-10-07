#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <cstdlib>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction state
    std::vector<std::vector<float>> learnedPatterns;
    std::vector<std::vector<float>> associatedPatterns;
    std::vector<float> patternWeights;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize pattern sizes with reasonable defaults
    currentSensorySize = 50;  // Default total sensory size
    visionPatternSize = 16;   // Default vision size (16x16)
    touchPatternSize = 8;     // Default touch sensors
    internalPatternSize = 4;  // Default internal signals
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // REAL IMPLEMENTATION: Use brain's neural substrate for prediction
    // 
    // Step 1: Extract neural activity patterns from relevant brain regions
    std::vector<float> currentPattern;
    extractNeuralPatternFromBrain(currentPattern);
    
    // Step 2: Convert current sensory state to neural pattern representation
    std::vector<float> currentSensoryPattern = convertSensoryToNeural(currentState);
    
    // Step 3: Find matching learned sequences in prediction system
    std::vector<float> predictedPattern = findBestMatchingSequence(currentSensoryPattern);
    
    // Step 4: Apply neural prediction dynamics (temporal associations)
    if (!predictedPattern.empty() && predictedPattern != currentSensoryPattern) {
        // There is a learned sequence prediction
        pImpl->predictionError = computePredictionError(currentSensoryPattern, predictedPattern);
        pImpl->confidence = 1.0f - pImpl->predictionError;
        
        // Generate predicted sensory input from neural pattern
        return createSensoryFromNeuralPattern(predictedPattern);
    }
    
    // Step 5: Generate baseline prediction using current neural activity
    // For now, use enhanced current state with noise for exploration
    std::vector<float> enhancedPattern = enhanceCurrentPattern(currentSensoryPattern);
    return createSensoryFromNeuralPattern(enhancedPattern);
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
    // PLACEHOLDER: Training not yet implemented
    
    // For now, track observation statistics for future prediction
    const auto& obsData = observation.getData();
    
    if (!obsData.empty()) {
        // Store observation statistics for learning patterns
        // In a real implementation, this would train a neural predictor
        // or statistical model on the observation sequences
        
        // Simple feature extraction - mean and variance of observation
        float sum = 0.0f;
        float sumSq = 0.0f;
        for (float val : obsData) {
            sum += val;
            sumSq += val * val;
        }
        
        float mean = sum / obsData.size();
        float variance = sumSq / obsData.size() - mean * mean;
        
        // Store feature statistics for prediction
        pImpl->errorHistory.push_back(mean);
        pImpl->errorHistory.push_back(variance);
        
        // Update confidence based on data variability
        // Higher variance means more uncertainty
        pImpl->confidence = std::max(0.1f, 1.0f - std::min(0.9f, variance / 10.0f));
    }
}

// Helper methods implementation

void PredictionSystem::extractNeuralPatternFromBrain(std::vector<float>& pattern) {
    // Extract neural activity patterns from sensory and prediction regions
    // This mimics how NeuralPrediction.cpp extracts patterns
    
    pattern.clear();
    
    // If we have a brain reference, extract activity patterns
    // For now, we'll simulate this with simple feature extraction
    // In a real implementation, this would read from brain regions
    
    // Default pattern based on current state
    pattern = std::vector<float>(currentSensorySize, 0.0f);
    
    // Add temporal sequence information
    if (!pImpl->errorHistory.empty()) {
        // Use recent error history as part of pattern
        for (size_t i = 0; i < std::min(static_cast<size_t>(3), pImpl->errorHistory.size()); ++i) {
            pattern[i] = pImpl->errorHistory[pImpl->errorHistory.size() - 1 - i];
        }
    }
}

void PredictionSystem::convertSensoryToNeural(const SensoryInput& sensory, std::vector<float>& pattern) {
    // Convert sensory input to neural pattern representation
    // This is similar to NeuralPrediction::computeSimilarity but in reverse
    
    const auto& data = sensory.getData();
    
    // Copy sensory data as neural pattern
    pattern = data;
    
    // Apply neural encoding transformations
    // In real implementation, this would involve:
    // - Synaptic gating
    // - Neuromodulatory influences
    // - Adaptation variable effects
    
    // For now, add some biological realism
    for (float& val : pattern) {
        // Apply tanh-like non-linearity for biological realism
        val = std::tanh(val);
    }
}

std::vector<float> PredictionSystem::findBestMatchingSequence(const std::vector<float>& pattern) {
    // Find the best matching learned sequence
    // Inspired by NeuralPrediction::findMatchingPatternNeuron
    
    if (pImpl->learnedPatterns.empty()) {
        return pattern;  // No learned patterns, return current
    }
    
    float bestSimilarity = 0.0f;
    size_t bestMatchIndex = 0;
    
    for (size_t i = 0; i < pImpl->learnedPatterns.size(); ++i) {
        float similarity = computePatternSimilarity(pattern, pImpl->learnedPatterns[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatchIndex = i;
        }
    }
    
    if (bestSimilarity > 0.6f) {  // Good enough match
        pImpl->confidence = bestSimilarity;
        return pImpl->associatedPatterns[bestMatchIndex];
    }
    
    return pattern;  // No good match, use current
}

float PredictionSystem::computePatternSimilarity(const std::vector<float>& a, 
                                                const std::vector<float>& b) const {
    // Cosine similarity metric for pattern matching
    // Adapted from NeuralPrediction::computeSimilarity
    
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) {
        return 0.0f;
    }
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

float PredictionSystem::computePredictionError(const std::vector<float>& actual,
                                               const std::vector<float>& predicted) const {
    // Compute prediction error similar to NeuralPrediction::updateWithObservation
    
    if (actual.size() != predicted.size() || actual.empty()) {
        return 0.0f;
    }
    
    // Compute mean squared error
    float sumError = 0.0f;
    for (size_t i = 0; i < actual.size(); ++i) {
        float diff = actual[i] - predicted[i];
        sumError += diff * diff;
    }
    
    return sumError / actual.size();
}

std::unique_ptr<SensoryInput> PredictionSystem::createSensoryFromNeuralPattern(const std::vector<float>& pattern) {
    // Create a SensoryInput from neural pattern
    // Similar to NeuralPrediction's generatePrediction but returns SensoryInput
    
    auto sensory = std::make_unique<SensoryInput>();
    
    // Split pattern into component streams
    const size_t visionSize = visionPatternSize;  // Should be defined somewhere
    const size_t touchSize = touchPatternSize;
    const size_t internalSize = internalPatternSize;
    
    size_t patternIndex = 0;
    
    if (patternIndex + visionSize <= pattern.size()) {
        std::vector<float> visionPattern(pattern.begin() + patternIndex, 
                                        pattern.begin() + patternIndex + visionSize);
        sensory->setVision(visionPattern);
        patternIndex += visionSize;
    }
    
    if (patternIndex + touchSize <= pattern.size()) {
        std::vector<float> touchPattern(pattern.begin() + patternIndex,
                                        pattern.begin() + patternIndex + touchSize);
        sensory->setTouch(touchPattern);
        patternIndex += touchSize;
    }
    
    if (patternIndex + internalSize <= pattern.size()) {
        std::vector<float> internalPattern(pattern.begin() + patternIndex,
                                           pattern.begin() + patternIndex + internalSize);
        sensory->setInternal(internalPattern);
        patternIndex += internalSize;
    }
    
    // Set timestamp for the prediction
    double currentTime = getCurrentSimulationTime();
    sensory->setTimestamp(currentTime);
    
    return sensory;
}

std::vector<float> PredictionSystem::enhanceCurrentPattern(const std::vector<float>& pattern) {
    // Enhance current pattern with noise for exploration
    // Similar to the enhanced current state in NeuralPrediction
    
    std::vector<float> enhanced = pattern;
    
    // Add small amount of noise for exploration
    const float explorationNoise = 0.05f;
    for (float& val : enhanced) {
        val += ((static_cast<float>(rand()) / RAND_MAX) - 0.5f) * explorationNoise;
    }
    
    // Apply non-linear transformation for biological realism
    for (float& val : enhanced) {
        val = std::tanh(val * 1.2f);
    }
    
    return enhanced;
}

void PredictionSystem::updateLearning(const SensoryInput& actualState, const SensoryInput& predictedState) {
    // Update the neural prediction system with actual observation
    // This is called from the PredictionSystem::updatePredictions method
    
    // Convert sensory states to neural patterns
    std::vector<float> actualPattern, predictedPattern;
    convertSensoryToNeural(actualState, actualPattern);
    convertSensoryToNeural(predictedState, predictedPattern);
    
    // Compute prediction error
    float error = computePredictionError(actualPattern, predictedPattern);
    pImpl->predictionError = error;
    pImpl->confidence = 1.0f - error;
    
    // Store in error history
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Learn the association if error is small (good prediction)
    if (error < 0.05f) {  // Low error means good prediction
        learnSequenceAssociation(actualPattern, predictedPattern, 1.0f - error);
    }
}

void PredictionSystem::learnSequenceAssociation(const std::vector<float>& fromPattern,
                                                const std::vector<float>& toPattern,
                                                float strength) {
    // Learn sequence association between patterns
    // Similar to NeuralPrediction::learnTemporalSequence
    
    // Find or create pattern neuron for 'from' pattern
    size_t fromIndex = findOrCreatePatternNeuron(fromPattern);
    size_t toIndex = findOrCreatePatternNeuron(toPattern);
    
    if (fromIndex != SIZE_MAX && toIndex != SIZE_MAX) {
        // Store the association
        if (pImpl->patternWeights.size() <= fromIndex) {
            pImpl->patternWeights.resize(fromIndex + 1, 0.0f);
        }
        if (pImpl->associatedPatterns.size() <= fromIndex) {
            pImpl->associatedPatterns.resize(fromIndex + 1);
        }
        
        pImpl->patternWeights[fromIndex] = strength;
        pImpl->associatedPatterns[fromIndex] = toPattern;
    }
}

size_t PredictionSystem::findOrCreatePatternNeuron(const std::vector<float>& pattern) {
    // Find or create a pattern neuron for the given pattern
    // Similar to NeuralPrediction::findMatchingPatternNeuron
    
    // Check for existing match
    for (size_t i = 0; i < pImpl->learnedPatterns.size(); ++i) {
        float similarity = computePatternSimilarity(pattern, pImpl->learnedPatterns[i]);
        if (similarity > 0.7f) {  // Good match threshold
            return i;
        }
    }
    
    // No match found, create new pattern neuron
    pImpl->learnedPatterns.push_back(pattern);
    return pImpl->learnedPatterns.size() - 1;
}

float PredictionSystem::getCurrentSimulationTime() const {
    // Get current simulation time
    // In real implementation, this would come from SimulationClock
    // For now, return a static value that increments
    static double time = 0.0;
    return static_cast<float>(time);
}

void PredictionSystem::setCurrentSensorySize(size_t size) {
    // Set the expected size of current sensory data
    currentSensorySize = size;
}

void PredictionSystem::setVisionPatternSize(size_t size) {
    visionPatternSize = size;
}

void PredictionSystem::setTouchPatternSize(size_t size) {
    touchPatternSize = size;
}

void PredictionSystem::setInternalPatternSize(size_t size) {
    internalPatternSize = size;
}

float PredictionSystem::getPredictionError(const SensoryInput& actual, const SensoryInput& predicted) const {
    // Compute prediction error between actual and predicted sensory inputs
    return computePredictionError(actual.getData(), predicted.getData());
}

} // namespace nlm
