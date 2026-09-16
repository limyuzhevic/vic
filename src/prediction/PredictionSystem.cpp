#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // Pattern learning
    std::vector<std::vector<float>> patternHistory;
    std::vector<SimulationStep> patternTimestamps;
    
    // Sequence memory
    std::vector<std::vector<float>> sequenceMemory;
    size_t currentSequencePosition;
    size_t minSequenceLength;
    
    // Temporal prediction
    std::vector<float> predictedPattern;
    float predictionError;
    float confidence;
    float predictionLevel;
    float predictionHorizon;
    float predictionAccuracy;
    float predictionConfidence;
    
    // Prediction state
    size_t predictionStep;
    size_t sequencePosition;
    
    // Parameter tuning
    size_t patternWindowSize;
    size_t maxHistorySize;
    
    // Error accumulation
    std::vector<float> errorAccumulator;
    
    Impl() : brain(nullptr)
        , currentSequencePosition(0)
        , minSequenceLength(3)
        , predictionError(0.0f)
        , confidence(0.5f)
        , predictionLevel(0.5f)
        , predictionHorizon(0.1f)
        , predictionAccuracy(0.8f)
        , predictionConfidence(0.9f)
        , predictionStep(0)
        , sequencePosition(0)
        , patternWindowSize(10)
        , maxHistorySize(1000) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Compute prediction based on current state and history
    computePrediction(currentState);
    
    // Create predicted sensory input based on pattern
    auto predictedInput = std::make_unique<SensoryInput>();
    
    // Convert pattern to vision data if applicable
    if (!pImpl->predictedPattern.empty()) {
        // Simple implementation: create vision from pattern
        size_t visionWidth = 8;
        size_t visionHeight = 8;
        
        std::vector<float> visionData;
        for (float pattern : pImpl->predictedPattern) {
            // Scale pattern to [0, 1] and add noise
            float value = (pattern + 1.0f) * 0.5f;  // Normalize from [-1,1] to [0,1]
            
            // Add some randomness
            std::mt19937 rng(42);  // Deterministic for reproducibility
            std::normal_distribution<float> noise(0.0f, 0.1f);
            value += noise(rng);
            
            // Clamp to valid range
            value = std::max(0.0f, std::min(1.0f, value));
            
            visionData.push_back(value);
        }
        
        // Create vision input
        auto vision = std::make_unique<Vision>(visionWidth, visionHeight);
        vision->setData(visionData);
        
        // Copy to predicted input
        predictedInput->setVision(*vision);
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Calculate prediction error
    float error = calculatePredictionError(predicted, actual);
    
    // Update error history
    pImpl->errorAccumulator.push_back(error);
    if (pImpl->errorAccumulator.size() > 100) {
        pImpl->errorAccumulator.erase(pImpl->errorAccumulator.begin());
    }
    
    // Update prediction accuracy
    float avgError = 0.0f;
    for (float e : pImpl->errorAccumulator) {
        avgError += std::abs(e);
    }
    if (!pImpl->errorAccumulator.empty()) {
        avgError /= pImpl->errorAccumulator.size();
    }
    
    pImpl->predictionAccuracy = std::max(0.0f, 1.0f - avgError);
    
    // Update confidence based on accuracy
    updateConfidence(error);
    
    // Store prediction for learning
    storePredictionInMemory(pImpl->predictedPattern);
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorAccumulator;
}

void PredictionSystem::clearHistory() {
    pImpl->errorAccumulator.clear();
    pImpl->patternHistory.clear();
    pImpl->sequenceMemory.clear();
    pImpl->predictedPattern.clear();
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->predictionLevel = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Convert observation to pattern
    std::vector<float> pattern;
    
    // Extract vision data if available
    auto vision = observation.getVision();
    if (vision) {
        size_t width = vision->getWidth();
        size_t height = vision->getHeight();
        size_t channels = vision->getChannels();
        
        const std::vector<float>& data = vision->getData();
        pattern.reserve(width * height * channels);
        
        for (size_t i = 0; i < std::min(data.size(), width * height * channels); ++i) {
            // Normalize to [-1, 1] range
            float normalized = (data[i] * 2.0f) - 1.0f;
            pattern.push_back(normalized);
        }
    }
    
    // Store pattern in history
    pImpl->patternHistory.push_back(pattern);
    pImpl->patternTimestamps.push_back(pImpl->predictionStep);
    
    // Limit history size
    if (pImpl->patternHistory.size() > pImpl->maxHistorySize) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
        pImpl->patternTimestamps.erase(pImpl->patternTimestamps.begin());
    }
    
    // Update sequence memory
    storePatternInMemory(pattern);
    
    // Learn from sequence
    if (pImpl->patternHistory.size() >= static_cast<size_t>(pImpl->minSequenceLength)) {
        std::vector<SensoryInput> sequence;
        size_t start = pImpl->patternHistory.size() - pImpl->minSequenceLength;
        
        for (size_t i = start; i < pImpl->patternHistory.size(); ++i) {
            auto input = std::make_unique<SensoryInput>();
            // In a real implementation, would properly convert pattern back to SensoryInput
            sequence.push_back(*input);
        }
        
        learnSequence(sequence);
    }
}

void PredictionSystem::update(TimestepDuration dt) {
    // Update prediction level based on confidence
    if (pImpl->confidence > 0.8f) {
        pImpl->predictionLevel = std::min(1.0f, pImpl->predictionLevel + 0.1f * dt);
    } else {
        pImpl->predictionLevel = std::max(0.0f, pImpl->predictionLevel - 0.05f * dt);
    }
    
    // Update temporal prediction
    updateSequencePrediction();
}

void PredictionSystem::storePattern(const std::vector<float>& pattern, SimulationStep step) {
    pImpl->predictedPattern = pattern;
    pImpl->predictionStep = step;
    
    // Apply to working memory if available
    applyPredictionToMemory();
}

std::vector<std::pair<std::vector<float>, SimulationStep>> PredictionSystem::getPatternHistory(size_t count) const {
    std::vector<std::pair<std::vector<float>, SimulationStep>> result;
    
    if (pImpl->patternHistory.empty()) return result;
    
    size_t start = std::max(0LL, static_cast<long long>(pImpl->patternHistory.size() - count));
    
    for (size_t i = start; i < pImpl->patternHistory.size(); ++i) {
        result.push_back(std::make_pair(pImpl->patternHistory[i], pImpl->patternTimestamps[i]));
    }
    
    return result;
}

void PredictionSystem::learnSequence(const std::vector<SensoryInput>& sequence) {
    // Simple sequence learning: store the temporal order
    // In a more sophisticated implementation, this would learn statistical
    // relationships and temporal dependencies
    
    std::vector<float> pattern;
    for (const auto& input : sequence) {
        auto vision = input.getVision();
        if (vision) {
            const std::vector<float>& data = vision->getData();
            pattern.insert(pattern.end(), data.begin(), data.end());
        }
    }
    
    pImpl->sequenceMemory.push_back(pattern);
    
    // Limit sequence memory size
    if (pImpl->sequenceMemory.size() > 100) {
        pImpl->sequenceMemory.erase(pImpl->sequenceMemory.begin());
    }
}

void PredictionSystem::computePrediction(const SensoryInput& currentState) {
    // Clear previous prediction
    pImpl->predictedPattern.clear();
    
    // Compute based on history
    if (pImpl->patternHistory.size() >= 2) {
        // Get the last pattern
        auto& lastPattern = pImpl->patternHistory.back();
        
        // Simple prediction: continue last pattern trend
        pImpl->predictedPattern = lastPattern;
        
        // Add some temporal dynamics
        for (float& value : pImpl->predictedPattern) {
            // Apply simple dynamics: decay towards zero with some noise
            value *= 0.9f;
            
            // Add some temporal variation
            std::mt19937 rng(42 + pImpl->predictionStep);
            std::normal_distribution<float> noise(0.0f, 0.05f);
            value += noise(rng);
        }
    } else {
        // No history, return zero pattern
        pImpl->predictedPattern = std::vector<float>(8, 0.0f);
    }
}

void PredictionSystem::updateSequencePrediction() {
    // Update sequence position
    pImpl->sequencePosition++;
    
    // If sequence is long enough, make a prediction
    if (pImpl->sequenceMemory.size() >= static_cast<size_t>(pImpl->minSequenceLength)) {
        // Get the sequence context
        size_t contextStart = std::max(0LL, static_cast<long long>(pImpl->sequenceMemory.size() - pImpl->patternWindowSize));
        
        // Compute prediction based on context
        std::vector<float> prediction;
        for (size_t i = contextStart; i < pImpl->sequenceMemory.size(); ++i) {
            auto& pattern = pImpl->sequenceMemory[i];
            for (size_t j = 0; j < std::min(pattern.size(), 8U); ++j) {
                prediction.push_back(pattern[j]);
            }
        }
        
        if (!prediction.empty()) {
            pImpl->predictedPattern = prediction;
        }
    }
}

float PredictionSystem::calculatePredictionError(const SensoryInput& predicted, const SensoryInput& actual) {
    // Simple error calculation: compare vision data
    auto predVision = predicted.getVision();
    auto actVision = actual.getVision();
    
    if (!predVision || !actVision) {
        return 0.0f;
    }
    
    const std::vector<float>& predData = predVision->getData();
    const std::vector<float>& actData = actVision->getData();
    
    if (predData.size() != actData.size() || predData.empty()) {
        return 0.0f;
    }
    
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actData[i];
        sumError += diff * diff;
    }
    
    return std::sqrt(sumError / predData.size());
}

void PredictionSystem::updateConfidence(float error) {
    // Confidence increases with accuracy
    float baseConfidence = 0.5f;
    
    // Reduce confidence based on error
    float confidenceReduction = error * 2.0f;
    confidence_ = std::max(0.0f, baseConfidence - confidenceReduction);
    
    // Boost confidence based on recent accuracy
    if (!pImpl->errorAccumulator.empty()) {
        float recentAccuracy = 1.0f - pImpl->predictionAccuracy;
        confidence_ = std::min(1.0f, confidence_ + recentAccuracy * 0.3f);
    }
}

void PredictionSystem::applyPredictionToMemory() {
    if (pImpl->brain && !pImpl->predictedPattern.empty()) {
        // Store prediction in working memory if available
        // This would require access to working memory, which is encapsulated
        // In a real implementation, this would be properly integrated
        NLM_LOG_INFO("Prediction applied to working memory");
    }
}

void PredictionSystem::storePredictionInMemory(const std::vector<float>& pattern) {
    // Store pattern in history
    pImpl->patternHistory.push_back(pattern);
    pImpl->patternTimestamps.push_back(pImpl->predictionStep);
    
    // Limit history size
    if (pImpl->patternHistory.size() > pImpl->maxHistorySize) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
        pImpl->patternTimestamps.erase(pImpl->patternTimestamps.begin());
    }
}

} // namespace nlm