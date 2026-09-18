#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    // Enhanced novelty detection state
    std::vector<float> featureBuffer;
    std::vector<float> referencePattern;
    float adaptationRate;
    float surpriseThreshold;
    float maxHistorySize;
    
    // Temporal novelty components
    float recentNovelty;
    float sustainedNovelty;
    float noveltyVelocity;
    
    // Detection parameters
    float featureWeight;
    float patternMismatchWeight;
    float surpriseMagnitude;
    
    // Novelty prediction model
    std::vector<float> noveltyPrediction;
    std::vector<float> temporalFeatureHistory;
    
    // Novelty dynamics parameters
    float burstPhase;
    float recoveryPhase;
    float surpriseIntegral;
    float predictionConfidence;
    
    // Time constants for temporal processing
    float detectionTimeConstant;
    float adaptationTimeConstant;
    float decayTimeConstant;
    float integrationTimeConstant;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             adaptationRate(0.05f), surpriseThreshold(0.7f), maxHistorySize(100.0f),
             recentNovelty(0.0f), sustainedNovelty(0.0f), noveltyVelocity(0.0f),
             featureWeight(0.6f), patternMismatchWeight(0.4f), surpriseMagnitude(0.0f),
             burstPhase(0.0f), recoveryPhase(0.0f), surpriseIntegral(0.0f),
             predictionConfidence(0.5f), detectionTimeConstant(0.5f),
             adaptationTimeConstant(1.0f), decayTimeConstant(2.0f),
             integrationTimeConstant(1.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const Observation& observation,
                            const Observation& previousObservation) {
    // Convert observation objects to feature vectors
    std::vector<float> currentFeatures;
    std::vector<float> previousFeatures;
    extractFeatures(observation, currentFeatures);
    extractFeatures(previousObservation, previousFeatures);
    
    detectNovelty(currentFeatures, previousFeatures);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    float floatDt = static_cast<float>(1.0);  // Default timestep for detection
    
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute feature differences with adaptive weighting
    float totalFeatureDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalFeatureDiff += diff * (1.0f + pImpl->adaptationRate * i / compareLen);
    }
    
    // Normalize feature difference with temporal context
    float avgFeatureDiff = totalFeatureDiff / compareLen;
    
    // Compute pattern mismatch using weighted Euclidean distance
    float patternMismatch = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        patternMismatch += diff * diff * (1.0f + pImpl->adaptationRate * 0.5f);
    }
    patternMismatch = std::sqrt(patternMismatch / compareLen);
    
    // Combine feature differences and pattern mismatch with temporal filtering
    float noveltyScore = pImpl->featureWeight * avgFeatureDiff + 
                        pImpl->patternMismatchWeight * patternMismatch;
    
    // Apply novelty threshold with dynamic adjustment
    float dynamicThreshold = pImpl->noveltyThreshold * (1.0f - pImpl->adaptationRate);
    pImpl->level = std::min(1.0f, noveltyScore / dynamicThreshold);
    
    // Compute surprise magnitude with integral estimation
    pImpl->surpriseMagnitude = pImpl->level * (1.0f - pImpl->noveltyThreshold) * pImpl->predictionConfidence;
    
    // Update temporal novelty components with dynamics
    pImpl->recentNovelty = pImpl->level;
    
    // Sustained novelty with exponential smoothing
    pImpl->sustainedNovelty += (pImpl->level - pImpl->sustainedNovelty) * 0.1f * floatDt;
    
    // Novelty velocity (rate of change) with saturation
    pImpl->noveltyVelocity = (pImpl->level - pImpl->level) * floatDt; // Simplified for now
    
    // Store current pattern and update history
    pImpl->lastPattern = currentPattern;
    pImpl->history.push_back(pImpl->level);
    
    // Maintain history size limit
    if (pImpl->history.size() > static_cast<size_t>(pImpl->maxHistorySize)) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update novelty prediction model
    pImpl->noveltyPrediction.push_back(noveltyScore);
    if (pImpl->noveltyPrediction.size() > 10) {
        pImpl->noveltyPrediction.erase(pImpl->noveltyPrediction.begin());
    }
    
    // Update surprise integral for temporal accumulation
    pImpl->surpriseIntegral += pImpl->surpriseMagnitude * floatDt;
}

void Novelty::update(TimestepDuration dt) {
    float floatDt = static_cast<float>(dt);
    
    // Burst phase for high novelty detection
    if (pImpl->surpriseMagnitude > pImpl->surpriseThreshold) {
        pImpl->burstPhase = 1.0f - std::exp(-floatDt / pImpl->detectionTimeConstant);
        pImpl->level = std::min(1.0f, pImpl->level + 0.2f * pImpl->burstPhase * floatDt);
    } else {
        pImpl->burstPhase = 0.0f;
    }
    
    // Decay novelty with biologically realistic exponential decay
    pImpl->level = std::max(0.0f, pImpl->level * std::exp(-pImpl->decayRate * floatDt));
    
    // Update sustained novelty component with differential decay
    pImpl->sustainedNovelty = std::max(0.0f, pImpl->sustainedNovelty - 
                                      pImpl->decayRate * 0.5f * floatDt);
    
    // Update surprise magnitude based on detection
    if (pImpl->surpriseMagnitude > pImpl->surpriseThreshold) {
        // High surprise triggers stronger novelty signal
        pImpl->level = std::min(1.0f, pImpl->level + 0.15f * floatDt);
        
        // Recovery phase for novelty detection
        pImpl->recoveryPhase = 1.0f - std::exp(-floatDt / pImpl->recoveryTimeConstant);
    }
    
    // Temporal novelty filtering - maintain recent novelty
    if (pImpl->recentNovelty > pImpl->noveltyThreshold) {
        // Trigger exploration behavior based on high novelty
        pImpl->explorationDrive = pImpl->recentNovelty * 0.8f;
    }
    
    // Update feature buffer with temporal history
    pImpl->featureBuffer.push_back(pImpl->level);
    if (pImpl->featureBuffer.size() > 50) {
        pImpl->featureBuffer.erase(pImpl->featureBuffer.begin());
    }
    
    // Integration of temporal novelty components
    float integrationRate = static_cast<float>(1.0) / pImpl->integrationTimeConstant;
    pImpl->predictionConfidence += (0.7f - pImpl->predictionConfidence) * integrationRate * floatDt;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->lastPattern.clear();
    pImpl->referencePattern.clear();
    pImpl->featureBuffer.clear();
    pImpl->noveltyPrediction.clear();
    pImpl->temporalFeatureHistory.clear();
    pImpl->surpriseIntegral = 0.0f;
    pImpl->predictionConfidence = 0.5f;
}

float Novelty::getSurpriseMagnitude() const {
    return pImpl->surpriseMagnitude;
}

float Novelty::getRecentNovelty() const {
    return pImpl->recentNovelty;
}

float Novelty::getSustainedNovelty() const {
    return pImpl->sustainedNovelty;
}

float Novelty::getNoveltyVelocity() const {
    return pImpl->noveltyVelocity;
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = threshold;
}

void Novelty::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Novelty::setAdaptationRate(float rate) {
    pImpl->adaptationRate = rate;
}

void Novelty::setSurpriseThreshold(float threshold) {
    pImpl->surpriseThreshold = threshold;
}

void Novelty::extractFeatures(const Observation& observation, std::vector<float>& features) const {
    // Simple feature extraction - placeholder for real implementation
    features.clear();
    features.resize(10, 0.0f);  // 10-dimensional feature vector
    
    // Extract meaningful features from observation
    // This would include visual, auditory, proprioceptive features, etc.
    // For now, just return empty features
    
    // In a real implementation, this would:
    // 1. Process raw sensory input
    // 2. Apply feature extraction algorithms
    // 3. Normalize features
    // 4. Return meaningful feature vector
}