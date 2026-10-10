#include "NoveltySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace nlm {

struct NoveltySystem::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    NoveltyState state;
    
    // Pattern database for novelty detection
    std::unordered_map<size_t, Pattern> patternDatabase;
    std::vector<size_t> recentPatterns;
    
    // History tracking
    std::vector<float> noveltyHistory;
    std::vector<float> patternDeviationHistory;
    std::vector<size_t> patternHashHistory;
    
    // Integration state
    float dopamineIntegration;
    float curiosityIntegration;
    float predictionErrorIntegration;
    
    // Pattern statistics
    std::vector<float> patternFeatures;
    std::vector<float> featureMeans;
    std::vector<float> featureVariances;
    
    // Novelty amplification
    float noveltyAmplificationFactor;
    float noveltyDecayFactor;
    
    // Safety bounds
    float minNoveltyLevel;
    float maxNoveltyLevel;
    float minPatternDeviation;
    float maxPatternDeviation;
    
    Impl() : brain(nullptr), engine(nullptr),
             dopamineIntegration(0.0f), curiosityIntegration(0.0f),
             predictionErrorIntegration(0.0f),
             noveltyAmplificationFactor(1.0f), noveltyDecayFactor(0.95f),
             minNoveltyLevel(0.0f), maxNoveltyLevel(1.0f),
             minPatternDeviation(0.0f), maxPatternDeviation(1.0f) {
        
        // Initialize feature statistics
        patternFeatures.resize(20, 0.0f);  // Default 20 features
        featureMeans.resize(20, 0.0f);
        featureVariances.resize(20, 0.0f);
        
        // Initialize recent patterns storage
        recentPatterns.reserve(100);
    }
};

NoveltySystem::NoveltySystem() : pImpl(new Impl) {
    NLM_LOG_INFO("NoveltySystem created");
}

NoveltySystem::~NoveltySystem() {
    NLM_LOG_INFO("NoveltySystem destroyed");
}

NoveltySystem::NoveltySystem(NoveltySystem&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

NoveltySystem& NoveltySystem::operator=(NoveltySystem&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
        if (pImpl) {
            pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
            pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
        }
    }
    return *this;
}

bool NoveltySystem::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("NoveltySystem initialization failed: invalid dependencies");
            return false;
        }
        
        // Initialize with some base patterns
        learnPattern(std::vector<float>(20, 0.0f));  // Empty pattern as baseline
        
        NLM_LOG_INFO("NoveltySystem initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NoveltySystem initialization failed: " + std::string(e.what()));
        return false;
    }
}

void NoveltySystem::update(TimestepDuration dt, SimulationStep step) {
    // Update state
    updateState(dt);
    
    // Decay novelty levels
    pImpl->state.noveltyLevel *= pImpl->noveltyDecayFactor;
    pImpl->state.patternDeviation *= pImpl->noveltyDecayFactor;
    pImpl->state.predictionSurprise *= pImpl->noveltyDecayFactor;
    
    // Update adaptive threshold
    pImpl->state.adaptiveThreshold = 
        0.5f + 0.3f * pImpl->errorState.uncertaintyEstimate;
    
    // Update pattern statistics
    updatePatternStatistics({});  // Would be called with actual patterns
}

void NoveltySystem::detectNovelty(const Observation& observation,
                                 const Observation& previousObservation) {
    // Compute pattern features from observations
    std::vector<float> currentPattern = computePatternFeatures(observation);
    std::vector<float> previousPattern = computePatternFeatures(previousObservation);
    
    // Detect novelty
    detectNovelty(currentPattern, previousPattern);
}

void NoveltySystem::detectNovelty(const std::vector<float>& currentPattern,
                                 const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        return;
    }
    
    // Compute pattern similarity
    float similarity = computePatternSimilarity(currentPattern, previousPattern);
    
    // Novelty is high when similarity is low
    pImpl->state.patternDeviation = 1.0f - similarity;
    
    // Store hash for recent patterns
    size_t currentHash = std::hash<std::vector<float>>{}(currentPattern);
    pImpl->recentPatterns.push_back(currentHash);
    if (pImpl->recentPatterns.size() > 100) {
        pImpl->recentPatterns.erase(pImpl->recentPatterns.begin());
    }
    
    // Update pattern database
    learnPattern(currentPattern);
    
    // Store in history
    pImpl->noveltyHistory.push_back(pImpl->state.patternDeviation);
    pImpl->patternDeviationHistory.push_back(pImpl->state.patternDeviation);
    pImpl->patternHashHistory.push_back(currentHash);
    
    if (pImpl->noveltyHistory.size() > 1000) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        pImpl->patternDeviationHistory.erase(pImpl->patternDeviationHistory.begin());
        pImpl->patternHashHistory.erase(pImpl->patternHashHistory.begin());
    }
}

void NoveltySystem::detectNovelty(const class NeuralPattern& neuralPattern) {
    // Extract features from neural pattern
    std::vector<float> currentPattern = neuralPattern.getFeatureVector();
    
    // Compute novelty based on neural pattern
    computePatternNovelty(pImpl->patternDatabase[std::hash<std::vector<float>>{}(currentPattern)]);
}

void NoveltySystem::learnPattern(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    size_t patternHash = std::hash<std::vector<float>>{}(pattern);
    
    // Check if pattern already exists
    auto it = pImpl->patternDatabase.find(patternHash);
    if (it != pImpl->patternDatabase.end()) {
        // Update existing pattern
        it->second.occurrenceCount += 1.0f;
        it->second.lastSeenTime = 0.0f;  // Reset time
        
        // Update statistics
        updatePatternStatistics(pattern);
        
        return;
    }
    
    // Create new pattern
    Pattern newPattern;
    newPattern.features = pattern;
    newPattern.occurrenceCount = 1.0f;
    newPattern.lastSeenTime = 0.0f;
    newPattern.activationLevel = 1.0f;
    
    // Compute initial novelty
    computePatternNovelty(newPattern);
    
    // Add to database
    pImpl->patternDatabase[patternHash] = newPattern;
    
    // Update statistics
    updatePatternStatistics(pattern);
}

void NoveltySystem::forgetPattern(const std::vector<float>& pattern) {
    size_t patternHash = std::hash<std::vector<float>>{}(pattern);
    pImpl->patternDatabase.erase(patternHash);
}

void NoveltySystem::updatePatternStatistics(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Update feature means and variances
    for (size_t i = 0; i < pattern.size() && i < pImpl->patternFeatures.size(); ++i) {
        float oldMean = pImpl->featureMeans[i];
        pImpl->featureMeans[i] = pImpl->featureMeans[i] * 0.99f + pattern[i] * 0.01f;
        
        pImpl->featureVariances[i] = pImpl->featureVariances[i] * 0.99f + 
                                     std::pow(pattern[i] - pImpl->featureMeans[i], 2) * 0.01f;
    }
}

void NoveltySystem::computePatternNovelty(Pattern& pattern) {
    if (pattern.features.empty()) return;
    
    // Compute novelty based on occurrence count (rare patterns are novel)
    pattern.noveltyScore = std::max(0.0f, 1.0f - pattern.occurrenceCount / 100.0f);
    
    // Compute deviation from mean features
    float totalDeviation = 0.0f;
    for (size_t i = 0; i < pattern.features.size() && i < pImpl->featureMeans.size(); ++i) {
        float deviation = std::abs(pattern.features[i] - pImpl->featureMeans[i]);
        totalDeviation += deviation;
    }
    
    // Normalize deviation
    float normalizedDeviation = totalDeviation / 
                               std::max(1.0f, static_cast<float>(pattern.features.size()));
    
    // Combine with occurrence-based novelty
    pattern.noveltyScore = (pattern.noveltyScore + normalizedDeviation) * 0.5f;
    
    // Apply bounds
    pattern.noveltyScore = std::clamp(pattern.noveltyScore, 0.0f, 1.0f);
}

NoveltyType NoveltySystem::classifyNoveltyType(const Observation& observation) const {
    // Simple classification based on observation properties
    // In a real system, this would be more sophisticated
    if (observation.type == ObservationType::Sensory) {
        return NoveltyType::SENSORY;
    } else if (observation.type == ObservationType::Cognitive) {
        return NoveltyType::COGNITIVE;
    } else {
        return NoveltyType::COMBINED;
    }
}

float NoveltySystem::computeSensoryNovelty(const Observation& observation) const {
    // Compute sensory novelty based on intensity and novelty
    float novelty = std::min(1.0f, observation.intensity * 0.1f);
    return novelty;
}

float NoveltySystem::computeCognitiveNovelty(const Observation& observation) const {
    // Compute cognitive novelty based on complexity
    float novelty = std::min(1.0f, observation.complexity * 0.05f);
    return novelty;
}

float NoveltySystem::computeTemporalNovelty(const Observation& observation) const {
    // Compute temporal novelty based on time since last similar observation
    // Placeholder implementation
    return 0.5f;
}

float NoveltySystem::computeSpatialNovelty(const Observation& observation) const {
    // Compute spatial novelty based on position changes
    // Placeholder implementation
    return 0.5f;
}

float NoveltySystem::computeSocialNovelty(const Observation& observation) const {
    // Compute social novelty based on social context
    // Placeholder implementation
    return 0.5f;
}

void NoveltySystem::updateState(TimestepDuration dt) {
    // Update novelty level with smooth transitions
    float targetNovelty = pImpl->state.patternDeviation;
    float transitionRate = 0.1f;
    
    float delta = targetNovelty - pImpl->state.noveltyLevel;
    pImpl->state.noveltyLevel += delta * transitionRate * static_cast<float>(dt);
    
    // Update other novelty components
    pImpl->state.sensoryNovelty = computeSensoryNovelty({});  // Placeholder
    pImpl->state.cognitiveNovelty = computeCognitiveNovelty({});  // Placeholder
    pImpl->state.temporalNovelty = computeTemporalNovelty({});  // Placeholder
    pImpl->state.spatialNovelty = computeSpatialNovelty({});  // Placeholder
    pImpl->state.socialNovelty = computeSocialNovelty({});  // Placeholder
    
    // Apply adaptation
    pImpl->state.adaptiveThreshold += 
        (pImpl->state.adaptiveThreshold * 0.5f - pImpl->state.adaptiveThreshold) * 
        pImpl->state.adaptationRate * static_cast<float>(dt);
    
    // Apply bounds
    enforceBounds();
}

void NoveltySystem::reset() {
    NLM_LOG_INFO("NoveltySystem reset");
    
    // Reset state
    pImpl->state = NoveltyState();
    
    // Reset pattern database (keep baseline)
    auto baselinePattern = pImpl->patternDatabase.find(std::hash<std::vector<float>>{}(std::vector<float>(20, 0.0f)));
    if (baselinePattern != pImpl->patternDatabase.end()) {
        pImpl->patternDatabase.clear();
        pImpl->patternDatabase[baselinePattern->first] = baselinePattern->second;
    }
    
    // Clear histories
    clearHistory();
}

void NoveltySystem::integrateWithDopamine(float dopamineLevel) {
    pImpl->dopamineIntegration = dopamineLevel;
    
    // Modulate novelty detection based on dopamine
    if (dopamineLevel > 0.5f) {
        // High dopamine enhances novelty detection
        pImpl->state.adaptiveThreshold *= 0.8f;
    } else if (dopamineLevel < 0.2f) {
        // Low dopamine reduces novelty detection
        pImpl->state.adaptiveThreshold *= 1.2f;
    }
}

void NoveltySystem::integrateWithCuriosity(float curiosityLevel) {
    pImpl->curiosityIntegration = curiosityLevel;
    
    // Enhance novelty detection with curiosity
    pImpl->state.learnedPatternWeight += curiosityLevel * 0.01f;
    pImpl->state.learnedPatternWeight = std::clamp(pImpl->state.learnedPatternWeight, 0.5f, 2.0f);
}

void NoveltySystem::integrateWithPredictionError(float predictionError) {
    pImpl->predictionErrorIntegration = predictionError;
    
    // Adjust novelty detection based on prediction error
    pImpl->state.adaptiveThreshold += predictionError * 0.1f;
}

void NoveltySystem::integrateWithBrainPatterns(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Update pattern database with brain patterns
    learnPattern(pattern);
}

void NoveltySystem::amplifyNovelty(float factor) {
    if (enableNoveltyAmplification_) {
        pImpl->noveltyAmplificationFactor *= factor;
        pImpl->noveltyAmplificationFactor = std::clamp(pImpl->noveltyAmplificationFactor, 0.1f, 10.0f);
        
        // Amplify novelty levels
        pImpl->state.noveltyLevel *= pImpl->noveltyAmplificationFactor;
        pImpl->state.patternDeviation *= pImpl->noveltyAmplificationFactor;
    }
}

void NoveltySystem::attenuateNovelty(float factor) {
    pImpl->noveltyAmplificationFactor /= factor;
    pImpl->noveltyAmplificationFactor = std::clamp(pImpl->noveltyAmplificationFactor, 0.1f, 10.0f);
    
    // Attenuate novelty levels
    pImpl->state.noveltyLevel *= (1.0f / pImpl->noveltyAmplificationFactor);
    pImpl->state.patternDeviation *= (1.0f / pImpl->noveltyAmplificationFactor);
}

void NoveltySystem::applyNoveltyFilter(std::vector<float>& features) {
    if (features.empty() || !enableCrossModalIntegration_) return;
    
    // Apply cross-modal integration to features
    for (size_t i = 0; i < features.size() && i < pImpl->patternFeatures.size(); ++i) {
        // Integrate with memory and expectation
        float memoryInfluence = pImpl->state.memoryConsolidation * 
                               (pImpl->featureMeans[i] - features[i]);
        
        features[i] += memoryInfluence;
        
        // Apply association strength
        features[i] *= pImpl->state.associationStrength;
    }
}

float NoveltySystem::computePatternSimilarity(const std::vector<float>& pattern1,
                                            const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t minSize = std::min(pattern1.size(), pattern2.size());
    float totalDiff = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDiff += std::abs(pattern1[i] - pattern2[i]);
    }
    
    // Compute cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f, norm2 = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);
    
    float cosineSimilarity = 0.0f;
    if (norm1 > 0.0f && norm2 > 0.0f) {
        cosineSimilarity = dotProduct / (norm1 * norm2);
    }
    
    return cosineSimilarity;
}

std::vector<float> NoveltySystem::computePatternFeatures(const Observation& observation) const {
    // Extract features from observation
    std::vector<float> features;
    
    // Simple feature extraction
    features.push_back(observation.intensity);
    features.push_back(observation.complexity);
    features.push_back(std::sin(observation.timestamp * 0.1f));  // Temporal feature
    features.push_back(std::cos(observation.timestamp * 0.1f));  // Temporal feature
    
    // Add more features based on observation type
    switch (observation.type) {
        case ObservationType::Sensory:
            features.push_back(0.8f);  // Sensory weight
            break;
        case ObservationType::Cognitive:
            features.push_back(0.6f);  // Cognitive weight
            break;
        case ObservationType::Motor:
            features.push_back(0.4f);  // Motor weight
            break;
        case ObservationType::Internal:
            features.push_back(0.2f);  // Internal weight
            break;
    }
    
    return features;
}

void NoveltySystem::updatePatternDatabase(const std::vector<float>& pattern) {
    learnPattern(pattern);
}

const std::vector<float>& NoveltySystem::getNoveltyHistory() const {
    return pImpl->noveltyHistory;
}

const std::vector<float>& NoveltySystem::getPatternDeviationHistory() const {
    return pImpl->patternDeviationHistory;
}

const std::vector<Pattern>& NoveltySystem::getPatternDatabase() const {
    // Convert to vector for return (simplified)
    static std::vector<Pattern> empty;
    empty.clear();
    for (const auto& pair : pImpl->patternDatabase) {
        empty.push_back(pair.second);
    }
    return empty;
}

void NoveltySystem::clearHistory() {
    pImpl->noveltyHistory.clear();
    pImpl->patternDeviationHistory.clear();
    pImpl->patternHashHistory.clear();
}

bool NoveltySystem::validateSafetyConstraints() const {
    if (pImpl->state.noveltyLevel < pImpl->state.minNoveltyLevel ||
        pImpl->state.noveltyLevel > pImpl->state.maxNoveltyLevel) {
        NLM_LOG_ERROR("NoveltySystem: Novelty level out of bounds");
        return false;
    }
    
    if (pImpl->state.patternDeviation < pImpl->state.minPatternDeviation ||
        pImpl->state.patternDeviation > pImpl->state.maxPatternDeviation) {
        NLM_LOG_ERROR("NoveltySystem: Pattern deviation out of bounds");
        return false;
    }
    
    // Check for numerical instabilities
    if (std::isnan(pImpl->state.noveltyLevel) || 
        std::isnan(pImpl->state.patternDeviation)) {
        NLM_LOG_ERROR("NoveltySystem: Numerical instability detected");
        return false;
    }
    
    return true;
}

void NoveltySystem::enforceBounds() {
    pImpl->state.noveltyLevel = std::clamp(pImpl->state.noveltyLevel,
                                          pImpl->state.minNoveltyLevel,
                                          pImpl->state.maxNoveltyLevel);
    
    pImpl->state.patternDeviation = std::clamp(pImpl->state.patternDeviation,
                                               pImpl->state.minPatternDeviation,
                                               pImpl->state.maxPatternDeviation);
}

void NoveltySystem::exportState(std::ostream& stream) const {
    stream.write(reinterpret_cast<const char*>(&pImpl->state), sizeof(NoveltyState));
    
    // Export pattern database (simplified)
    size_t databaseSize = pImpl->patternDatabase.size();
    stream.write(reinterpret_cast<const char*>(&databaseSize), sizeof(size_t));
    
    // Export histories
    size_t historySize = pImpl->noveltyHistory.size();
    stream.write(reinterpret_cast<const char*>(&historySize), sizeof(size_t));
    if (historySize > 0) {
        stream.write(reinterpret_cast<const char*>(pImpl->noveltyHistory.data()),
                    historySize * sizeof(float));
    }
}

void NoveltySystem::importState(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&pImpl->state), sizeof(NoveltyState));
    
    // Import pattern database
    size_t databaseSize;
    stream.read(reinterpret_cast<char*>(&databaseSize), sizeof(size_t));
    pImpl->patternDatabase.clear();
    
    // Import histories
    size_t historySize;
    stream.read(reinterpret_cast<char*>(&historySize), sizeof(size_t));
    pImpl->noveltyHistory.resize(historySize);
    if (historySize > 0) {
        stream.read(reinterpret_cast<char*>(pImpl->noveltyHistory.data()),
                   historySize * sizeof(float));
    }
}

std::vector<float> NoveltySystem::computeStatisticalFeatures(const std::vector<float>& pattern) const {
    // Compute statistical features from pattern
    std::vector<float> features;
    features.reserve(20);
    
    if (pattern.empty()) return features;
    
    // Basic statistics
    float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    float mean = sum / pattern.size();
    features.push_back(mean);
    
    float varianceSum = 0.0f;
    for (float value : pattern) {
        varianceSum += (value - mean) * (value - mean);
    }
    float variance = varianceSum / pattern.size();
    features.push_back(variance);
    
    float minVal = *std::min_element(pattern.begin(), pattern.end());
    features.push_back(minVal);
    
    float maxVal = *std::max_element(pattern.begin(), pattern.end());
    features.push_back(maxVal);
    
    // Add more features
    for (size_t i = 0; i < 16 && i + 4 < 100; i += 4) {
        features.push_back(static_cast<float>(i) / 100.0f);
    }
    
    return features;
}

std::vector<float> NoveltySystem::computeTemporalFeatures(const Observation& observation) const {
    // Compute temporal features
    std::vector<float> features;
    features.push_back(std::sin(observation.timestamp * 0.1f));
    features.push_back(std::cos(observation.timestamp * 0.1f));
    features.push_back(std::exp(-observation.timestamp * 0.01f));
    return features;
}

std::vector<float> NoveltySystem::computeSpatialFeatures(const Observation& observation) const {
    // Compute spatial features
    std::vector<float> features;
    features.push_back(observation.position.x);
    features.push_back(observation.position.y);
    features.push_back(observation.position.z);
    features.push_back(std::sqrt(observation.position.x * observation.position.x +
                               observation.position.y * observation.position.y +
                               observation.position.z * observation.position.z));
    return features;
}

std::vector<float> NoveltySystem::computeSocialFeatures(const Observation& observation) const {
    // Compute social features
    std::vector<float> features;
    features.push_back(observation.socialStrength);
    features.push_back(observation.interactionCount);
    features.push_back(observation.groupCohesion);
    return features;
}

float NoveltySystem::getNoveltyLevel() const {
    return pImpl->state.noveltyLevel;
}

float NoveltySystem::getPatternDeviation() const {
    return pImpl->state.patternDeviation;
}

float NoveltySystem::getPredictionSurprise() const {
    return pImpl->state.predictionSurprise;
}

float NoveltySystem::getSensoryNovelty() const {
    return pImpl->state.sensoryNovelty;
}

float NoveltySystem::getCognitiveNovelty() const {
    return pImpl->state.cognitiveNovelty;
}

float NoveltySystem::getTemporalNovelty() const {
    return pImpl->state.temporalNovelty;
}

float NoveltySystem::getSpatialNovelty() const {
    return pImpl->state.spatialNovelty;
}

float NoveltySystem::getSocialNovelty() const {
    return pImpl->state.socialNovelty;
}

float NoveltySystem::getAdaptiveThreshold() const {
    return pImpl->state.adaptiveThreshold;
}

void NoveltySystem::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm