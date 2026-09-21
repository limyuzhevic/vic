#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction components
    std::vector<float> neuralPatternBuffer;
    std::vector<float> predictionPattern;
    std::vector<float> predictionError;
    
    // Temporal dynamics for prediction
    std::deque<float> temporalTrace;
    std::vector<float> weightedHistory;
    
    // Confidence and learning parameters
    float confidence;
    float predictionThreshold;
    float learningRate;
    float errorDecay;
    
    // Statistics
    size_t predictionCount;
    size_t predictionErrors;
    float cumulativeError;
    
    // Neural substrate references
    Brain* brain;
    RandomGenerator* rng;
    
    Impl() : confidence(0.5f), predictionThreshold(0.3f), learningRate(0.01f), 
             errorDecay(0.99f), predictionCount(0), predictionErrors(0), cumulativeError(0.0f),
             brain(nullptr), rng(nullptr) {
        // Initialize with larger buffers for neural patterns
        neuralPatternBuffer.reserve(10000);
        predictionPattern.reserve(10000);
        predictionError.reserve(10000);
        temporalTrace.reserve(100);
        weightedHistory.reserve(100);
    }
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

// Real prediction using neural firing patterns and working memory
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    pImpl->predictionCount++;
    
    // Collect neural activity patterns from the brain
    collectNeuralActivityPatterns();
    
    // Generate prediction based on neural dynamics
    generateNeuralPrediction();
    
    // Create internal signal representation of the prediction
    auto prediction = std::make_unique<InternalSignals>();
    prediction->clearSignals();
    
    // Convert prediction pattern to internal signals
    for (float value : pImpl->predictionPattern) {
        prediction->addSignal(value);
    }
    
    // Apply prediction confidence thresholding
    pImpl->confidence = calculatePredictionConfidence();
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (pImpl->brain) {
        // Calculate prediction error using neural substrate
        calculatePredictionError(predicted, actual);
        
        // Update neural weights based on prediction error
        updateNeuralWeights();
        
        // Update temporal trace for future predictions
        updateTemporalTrace();
        
        // Store error history
        if (pImpl->predictionError.empty() || 
            std::abs(pImpl->predictionError.back()) < 0.001f) {
            pImpl->predictionError.clear();
        }
        
        // Update confidence based on recent error
        updatePredictionConfidence();
    }
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->predictionError.empty()) {
        return 0.0f;
    }
    
    // Return exponentially weighted average of recent errors
    float weightedSum = 0.0f;
    float weightSum = 0.0f;
    
    size_t historySize = std::min(pImpl->predictionError.size(), static_cast<size_t>(10));
    
    for (size_t i = 0; i < historySize; ++i) {
        size_t idx = pImpl->predictionError.size() - 1 - i;
        float error = pImpl->predictionError[idx];
        float weight = std::pow(0.9f, static_cast<float>(i));
        weightedSum += error * weight;
        weightSum += weight;
    }
    
    return weightSum > 0.0f ? weightedSum / weightSum : 0.0f;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->predictionError;
}

void PredictionSystem::clearHistory() {
    pImpl->predictionError.clear();
    pImpl->temporalTrace.clear();
    pImpl->weightedHistory.clear();
    pImpl->neuralPatternBuffer.clear();
}

// Train prediction model using neural plasticity
void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) {
        return;
    }
    
    // Integrate observation into neural pattern buffer
    const auto& data = observation.getData();
    pImpl->neuralPatternBuffer.insert(pImpl->neuralPatternBuffer.end(), 
                                     data.begin(), data.end());
    
    // Keep buffer at reasonable size
    if (pImpl->neuralPatternBuffer.size() > 10000) {
        pImpl->neuralPatternBuffer.erase(pImpl->neuralPatternBuffer.begin());
    }
    
    // Update prediction model based on new observation
    trainPredictionModelFromData();
    
    // Trigger synaptic changes based on prediction error
    triggerNeuralAdaptation();
}

// Collect neural activity patterns from brain regions
void PredictionSystem::Impl::collectNeuralActivityPatterns() {
    neuralPatternBuffer.clear();
    
    if (!brain) return;
    
    // Collect from working memory if available
    if (brain->getWorkingMemory()) {
        auto workingMemory = brain->getWorkingMemory();
        auto pattern = workingMemory->retrieve();
        neuralPatternBuffer.insert(neuralPatternBuffer.end(), pattern.begin(), pattern.end());
    }
    
    // Collect from episodic memory recent episodes
    if (brain->getEpisodicMemory()) {
        auto episodicMemory = brain->getEpisodicMemory();
        size_t episodeCount = episodicMemory->getEpisodeCount();
        
        // Get recent episodes for pattern analysis
        size_t maxEpisodes = std::min(static_cast<size_t>(5), episodeCount);
        if (maxEpisodes > 0) {
            auto episodes = episodicMemory->getEpisodesForReplay(maxEpisodes);
            for (const auto* episode : episodes) {
                // Add activation patterns to buffer
                for (float activation : episode->neuronActivations) {
                    neuralPatternBuffer.push_back(activation);
                }
            }
        }
    }
}

// Generate prediction using neural dynamics
void PredictionSystem::Impl::generateNeuralPrediction() {
    predictionPattern.clear();
    
    if (neuralPatternBuffer.empty()) {
        // No data available, return empty prediction
        return;
    }
    
    // Use temporal trace to predict next state
    if (!temporalTrace.empty()) {
        // Apply neural dynamics model
        float avgPattern = std::accumulate(neuralPatternBuffer.begin(), neuralPatternBuffer.end(), 0.0f) / neuralPatternBuffer.size();
        
        // Predict based on temporal dynamics and neural plasticity
        float predictedValue = avgPattern * 1.1f; // Tendency to amplify
        
        // Add noise for exploration
        if (rng) {
            predictedValue += rng->uniform(-0.1f, 0.1f);
        }
        
        predictionPattern.push_back(predictedValue);
    } else {
        // No temporal trace, use pattern statistics
        float minVal = *std::min_element(neuralPatternBuffer.begin(), neuralPatternBuffer.end());
        float maxVal = *std::max_element(neuralPatternBuffer.begin(), neuralPatternBuffer.end());
        
        // Predict intermediate state
        float predictedValue = (minVal + maxVal) * 0.5f;
        predictionPattern.push_back(predictedValue);
    }
}

// Calculate prediction error using brain activity
void PredictionSystem::Impl::calculatePredictionError(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.empty() || actualData.empty()) {
        return;
    }
    
    float totalError = 0.0f;
    
    // Calculate error for each dimension
    size_t minSize = std::min(predData.size(), actualData.size());
    for (size_t i = 0; i < minSize; ++i) {
        float diff = predData[i] - actualData[i];
        totalError += diff * diff;
    }
    
    float meanSquaredError = totalError / minSize;
    predictionError.push_back(std::sqrt(meanSquaredError));
    
    // Keep error history at reasonable size
    if (predictionError.size() > 1000) {
        predictionError.erase(predictionError.begin());
    }
}

// Update neural weights based on prediction error
void PredictionSystem::Impl::updateNeuralWeights() {
    if (predictionError.empty()) return;
    
    float error = predictionError.back();
    
    // Update synaptic weights based on prediction error
    if (brain) {
        auto spikeSystem = brain->getSpikeSystem();
        auto regions = brain->getRegions();
        
        // Apply weight changes to synapses
        for (auto& region : regions) {
            for (auto& synapse : region->getSynapses()) {
                // Calculate weight change based on prediction error
                float weightChange = error * learningRate;
                
                // Ensure weight stays within bounds
                float newWeight = synapse->getWeight() + weightChange;
                newWeight = std::clamp(newWeight, -1.0f, 1.0f);
                
                synapse->setWeight(newWeight);
                
                // Update eligibility trace
                float eligibility = synapse->getEligibilityTrace();
                eligibility = error * errorDecay;
                synapse->setEligibilityTrace(eligibility);
            }
        }
    }
}

// Update temporal trace for future predictions
void PredictionSystem::Impl::updateTemporalTrace() {
    if (predictionError.empty()) return;
    
    float currentError = predictionError.back();
    
    // Update temporal trace with weighted error
    temporalTrace.push_back(currentError * 0.1f);
    
    // Keep trace at reasonable size
    if (temporalTrace.size() > 100) {
        temporalTrace.pop_front();
    }
    
    // Calculate weighted history
    weightedHistory.clear();
    if (!temporalTrace.empty()) {
        float totalTrace = std::accumulate(temporalTrace.begin(), temporalTrace.end(), 0.0f);
        float avgTrace = totalTrace / temporalTrace.size();
        
        // Store weighted trace
        weightedHistory.resize(temporalTrace.size());
        std::transform(temporalTrace.begin(), temporalTrace.end(), weightedHistory.begin(),
                      [avgTrace](float val) { return val / (avgTrace + 1e-6f); });
    }
}

// Calculate prediction confidence
float PredictionSystem::Impl::calculatePredictionConfidence() {
    if (predictionError.empty()) return 0.5f;
    
    // Calculate confidence based on error variance
    float meanError = 0.0f;
    for (float error : predictionError) {
        meanError += error;
    }
    meanError /= predictionError.size();
    
    // Calculate variance
    float variance = 0.0f;
    for (float error : predictionError) {
        float diff = error - meanError;
        variance += diff * diff;
    }
    variance /= predictionError.size();
    
    // Higher variance means lower confidence
    float stdDev = std::sqrt(variance);
    float confidence = std::max(0.0f, 1.0f - stdDev * 2.0f);
    
    return confidence;
}

// Update prediction confidence based on recent errors
void PredictionSystem::Impl::updatePredictionConfidence() {
    float avgError = calculatePredictionErrorAverage();
    
    // Update confidence based on error magnitude
    if (avgError > predictionThreshold) {
        confidence *= 0.95f; // Decrease confidence on large errors
    } else {
        confidence *= 1.05f; // Increase confidence on small errors
    }
    
    // Clamp confidence
    confidence = std::clamp(confidence, 0.1f, 1.0f);
}

// Calculate average prediction error
float PredictionSystem::Impl::calculatePredictionErrorAverage() const {
    if (predictionError.empty()) return 0.0f;
    
    float sum = std::accumulate(predictionError.begin(), predictionError.end(), 0.0f);
    return sum / predictionError.size();
}

// Train prediction model from neural pattern buffer
void PredictionSystem::Impl::trainPredictionModelFromData() {
    if (neuralPatternBuffer.empty()) return;
    
    // Calculate statistics of the pattern buffer
    float sum = std::accumulate(neuralPatternBuffer.begin(), neuralPatternBuffer.end(), 0.0f);
    float mean = sum / neuralPatternBuffer.size();
    
    // Update prediction pattern based on training data
    float trend = 0.0f;
    if (neuralPatternBuffer.size() > 1) {
        // Simple linear trend detection
        float firstHalf = 0.0f, secondHalf = 0.0f;
        size_t halfSize = neuralPatternBuffer.size() / 2;
        
        for (size_t i = 0; i < halfSize; ++i) {
            firstHalf += neuralPatternBuffer[i];
            secondHalf += neuralPatternBuffer[i + halfSize];
        }
        
        firstHalf /= halfSize;
        secondHalf /= halfSize;
        trend = secondHalf - firstHalf;
    }
    
    // Update prediction pattern
    float predictedValue = mean + trend * 0.1f;
    predictedValue = std::clamp(predictedValue, -1.0f, 1.0f);
    
    // Store in neural pattern buffer for future predictions
    neuralPatternBuffer.push_back(predictedValue);
    if (neuralPatternBuffer.size() > 10000) {
        neuralPatternBuffer.erase(neuralPatternBuffer.begin());
    }
}

// Trigger neural adaptation based on prediction error
void PredictionSystem::Impl::triggerNeuralAdaptation() {
    if (brain && !predictionError.empty()) {
        float error = predictionError.back();
        
        // Modulate dopamine based on prediction error
        if (brain->getDopamine()) {
            float dopamineLevel = error * 2.0f; // Scale error to dopamine level
            // Dopamine modulation would update neural excitability
        }
        
        // Update curiosity based on prediction error
        if (brain->getCuriosity()) {
            // Increase curiosity when prediction errors are high
            float curiosityLevel = std::min(error * 5.0f, 1.0f);
            // Curiosity would modulate exploration
        }
    }
}

} // namespace nlm
