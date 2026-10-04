#include "Novelty.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>
#include <queue>

namespace nlm {

struct Novelty::Impl {
    // Statistical pattern detection
    struct {
        std::vector<float> patternHistory;        // History of input patterns
        std::vector<float> statisticalMoments;   // Statistical moments (mean, variance, skew, kurtosis)
        std::unordered_map<float, int> frequencyMap; // Frequency of pattern values
        float patternThreshold;                  // Threshold for novelty detection
        float adaptationRate;                    // Rate of adaptation to new patterns
        float noveltyDecay;                       // Decay rate of novelty signal
        size_t maxHistorySize;                   // Maximum size of history buffer
    } patternDetector;
    
    // Frequency-based novelty detection
    struct {
        std::unordered_map<size_t, int> occurrenceCounts; // Count of occurrences for each feature
        std::unordered_map<size_t, float> lastOccurrenceTime; // Last time each feature was observed
        float noveltyScore;                         // Current novelty score
        float frequencyThreshold;                   // Threshold for frequency-based novelty
        float recencyThreshold;                     // Threshold for recency-based novelty
    } frequencyDetector;
    
    // Contextual novelty assessment
    struct {
        std::vector<float> contextVector;          // Current context vector
        std::vector<float> contextMemory;          // Memory of past contexts
        std::vector<std::vector<float>> contextPatterns; // Patterns associated with contexts
        float contextSimilarityThreshold;          // Threshold for context similarity
        float contextAdaptationRate;               // Rate of context adaptation
    } contextDetector;
    
    // Memory-based comparison
    struct {
        std::vector<float> episodicMemory;         // Episodic memory buffer
        std::vector<float> semanticMemory;         // Semantic memory buffer
        float memoryDecayRate;                      // Rate of memory decay
        float similarityThreshold;                  // Threshold for pattern similarity
        std::priority_queue<float> topSimilarities; // Top N similarities for efficient retrieval
    } memorySystem;
    
    // Feature extraction
    struct {
        std::vector<float> extractedFeatures;       // Current extracted features
        std::vector<float> featureWeights;          // Weights for different features
        float featureExtractionRate;                // Rate of feature extraction
        size_t featureDimension;                    // Dimension of feature space
    } featureExtractor;
    
    // Implementation tracking
    bool initialized;                              // Whether novelty system has been initialized
    bool enabled;                                   // Whether novelty system is enabled
    float lastUpdateTime;                           // Last update timestamp
    float noveltyHistory;                            // History of novelty levels
    
    Impl() {
        // Initialize pattern detector
        patternDetector.patternHistory.reserve(1000);
        patternDetector.statisticalMoments.resize(4, 0.0f); // mean, variance, skew, kurtosis
        patternDetector.maxHistorySize = 1000;
        patternDetector.patternThreshold = 2.0f;
        patternDetector.adaptationRate = 0.1f;
        patternDetector.noveltyDecay = 0.95f;
        
        // Initialize frequency detector
        frequencyDetector.frequencyThreshold = 3.0f;
        frequencyDetector.recencyThreshold = 100.0f;
        frequencyDetector.noveltyScore = 0.0f;
        
        // Initialize context detector
        contextDetector.contextSimilarityThreshold = 0.7f;
        contextDetector.contextAdaptationRate = 0.05f;
        contextDetector.contextVector.resize(4, 0.0f);
        
        // Initialize memory system
        memorySystem.memoryDecayRate = 0.98f;
        memorySystem.similarityThreshold = 0.8f;
        
        // Initialize feature extractor
        featureExtractor.featureDimension = 16;
        featureExtractor.featureExtractionRate = 0.5f;
        featureExtractor.extractedFeatures.resize(featureExtractor.featureDimension, 0.0f);
        featureExtractor.featureWeights.resize(featureExtractor.featureDimension, 1.0f);
        
        // Implementation tracking
        initialized = false;
        enabled = true;
        lastUpdateTime = 0.0f;
        noveltyHistory = 0.0f;
    }
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize Novelty system with null brain");
        return;
    }
    
    pImpl->brain = brain;
    pImpl->initialized = true;
    
    NLM_LOG_INFO("Novelty detection system initialized with statistical pattern analysis");
}

float Novelty::getLevel() const {
    if (!pImpl->initialized) return 0.0f;
    
    // Compute novelty level from multiple sources
    float patternNovelty = detectPatternNovelty();
    float frequencyNovelty = detectFrequencyNovelty();
    float contextNovelty = detectContextNovelty();
    float memoryNovelty = detectMemoryNovelty();
    
    // Combine novelty signals
    float noveltyLevel = (patternNovelty * 0.3f + 
                        frequencyNovelty * 0.3f + 
                        contextNovelty * 0.2f + 
                        memoryNovelty * 0.2f);
    
    return std::min(1.0f, noveltyLevel);
}

void Novelty::setLevel(float level) {
    pImpl->noveltyHistory = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                           const Observation& previousObservation) {
    if (!pImpl->initialized) return;
    
    // Convert observations to patterns
    std::vector<float> currentPattern = observationToPattern(observation);
    std::vector<float> previousPattern = observationToPattern(previousObservation);
    
    // Detect novelty from both patterns
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (!pImpl->initialized || currentPattern.empty() || previousPattern.empty()) {
        pImpl->noveltyHistory = 0.0f;
        return;
    }
    
    // Extract features from current pattern
    std::vector<float> features = extractFeatures(currentPattern);
    
    // Compare with previous pattern
    float similarity = computePatternSimilarity(features, previousPattern);
    
    // Compute novelty based on similarity
    float novelty = 1.0f - similarity;
    
    // Apply multiple novelty detection mechanisms
    float patternNovelty = detectPatternNovelty();
    float frequencyNovelty = detectFrequencyNovelty();
    float contextNovelty = detectContextNovelty();
    
    // Combine novelty signals
    pImpl->noveltyHistory = std::min(1.0f, 
        (patternNovelty * 0.3f + frequencyNovelty * 0.3f + 
         contextNovelty * 0.2f + novelty * 0.2f));
    
    // Store patterns for future comparison
    storePattern(currentPattern);
    
    // Update history
    pImpl->patternDetector.patternHistory.push_back(currentPattern[0]); // Use first value for simplicity
    if (pImpl->patternDetector.patternHistory.size() > pImpl->patternDetector.maxHistorySize) {
        pImpl->patternDetector.patternHistory.erase(pImpl->patternDetector.patternHistory.begin());
    }
    
    NLM_LOG_DEBUG("Novelty detection: similarity=" + std::to_string(similarity) + 
                 ", novelty=" + std::to_string(pImpl->noveltyHistory));
}

void Novelty::update(TimestepDuration dt) {
    if (!pImpl->initialized) return;
    
    float dtFloat = static_cast<float>(dt);
    pImpl->lastUpdateTime += dtFloat;
    
    // Decay novelty over time
    pImpl->noveltyHistory *= pImpl->patternDetector.noveltyDecay;
    
    // Update statistical moments
    updateStatisticalMoments();
    
    // Update frequency detection
    updateFrequencyDetection(dtFloat);
    
    // Update context detection
    updateContextDetection();
    
    // Update memory system
    updateMemorySystem(dtFloat);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->patternDetector.patternHistory;
}

void Novelty::clearHistory() {
    pImpl->patternDetector.patternHistory.clear();
    pImpl->frequencyDetector.occurrenceCounts.clear();
    pImpl->contextDetector.contextMemory.clear();
    pImpl->memorySystem.episodicMemory.clear();
    pImpl->memorySystem.semanticMemory.clear();
    
    NLM_LOG_INFO("Novelty system history cleared");
}

float Novelty::detectPatternNovelty() {
    if (pImpl->patternDetector.patternHistory.size() < 2) return 0.0f;
    
    // Compute statistical moments
    float sum = 0.0f, sumSq = 0.0f;
    for (float val : pImpl->patternDetector.patternHistory) {
        sum += val;
        sumSq += val * val;
    }
    
    float n = static_cast<float>(pImpl->patternDetector.patternHistory.size());
    float mean = sum / n;
    float variance = (sumSq - sum * sum / n) / (n - 1);
    
    // Update statistical moments
    pImpl->patternDetector.statisticalMoments[0] = mean;        // mean
    pImpl->patternDetector.statisticalMoments[1] = variance;     // variance
    pImpl->patternDetector.statisticalMoments[2] = 0.0f;         // skew (simplified)
    pImpl->patternDetector.statisticalMoments[3] = 0.0f;         // kurtosis (simplified)
    
    // Compute deviation from expected pattern
    float currentPattern = pImpl->patternDetector.patternHistory.back();
    float deviation = std::abs(currentPattern - mean);
    
    // Normalize by standard deviation
    float stdDev = std::sqrt(variance);
    float zScore = (stdDev > 0.0f) ? (deviation / stdDev) : 0.0f;
    
    // Compute novelty based on statistical deviation
    float novelty = std::min(1.0f, zScore / pImpl->patternDetector.patternThreshold);
    
    return novelty;
}

float Novelty::detectFrequencyNovelty() {
    if (!pImpl->brain) return 0.0f;
    
    // Get current sensory input
    auto* sensoryInput = pImpl->brain->getSensoryInput();
    if (!sensoryInput) return 0.0f;
    
    const auto& sensoryData = sensoryInput->getData();
    if (sensoryData.empty()) return 0.0f;
    
    // Count occurrences of features
    size_t totalFeatures = 0;
    for (size_t i = 0; i < sensoryData.size(); ++i) {
        size_t featureId = static_cast<size_t>(sensoryData[i] * 100.0f); // Quantize feature id
        pImpl->frequencyDetector.occurrenceCounts[featureId]++;
        
        // Update last occurrence time
        pImpl->frequencyDetector.lastOccurrenceTime[featureId] = pImpl->lastUpdateTime;
        
        totalFeatures++;
    }
    
    // Compute novelty based on frequency and recency
    float noveltyScore = 0.0f;
    float currentTime = pImpl->lastUpdateTime;
    
    for (const auto& pair : pImpl->frequencyDetector.occurrenceCounts) {
        size_t featureId = pair.first;
        int count = pair.second;
        float lastTime = pImpl->frequencyDetector.lastOccurrenceTime[featureId];
        
        // Frequency-based novelty: rare features are novel
        float frequencyNovelty = 1.0f / std::max(1.0f, static_cast<float>(count));
        
        // Recency-based novelty: recent features are novel
        float recencyNovelty = std::min(1.0f, (currentTime - lastTime) / 
                                       pImpl->frequencyDetector.recencyThreshold);
        
        // Combine frequency and recency
        float featureNovelty = (frequencyNovelty * 0.7f + recencyNovelty * 0.3f);
        noveltyScore += featureNovelty;
    }
    
    // Normalize by number of features
    noveltyScore /= std::max(1.0f, static_cast<float>(pImpl->frequencyDetector.occurrenceCounts.size()));
    
    return std::min(1.0f, noveltyScore);
}

float Novelty::detectContextNovelty() {
    if (!pImpl->brain) return 0.0f;
    
    // Get current context from brain
    // This would typically come from attention or working memory systems
    // For now, we'll use a simple placeholder
    
    float novelty = 0.0f;
    
    // Update context memory
    pImpl->contextDetector.contextMemory.push_back(pImpl->noveltyHistory);
    if (pImpl->contextDetector.contextMemory.size() > 100) {
        pImpl->contextDetector.contextMemory.erase(pImpl->contextDetector.contextMemory.begin());
    }
    
    // Compute context similarity
    if (pImpl->contextDetector.contextMemory.size() >= 2) {
        float currentContext = pImpl->contextDetector.contextMemory.back();
        float previousContext = pImpl->contextDetector.contextMemory[pImpl->contextDetector.contextMemory.size() - 2];
        
        float contextSimilarity = 1.0f - std::abs(currentContext - previousContext);
        
        if (contextSimilarity < pImpl->contextDetector.contextSimilarityThreshold) {
            // Different contexts are novel
            novelty = (1.0f - contextSimilarity) * (1.0f - pImpl->contextDetector.contextSimilarityThreshold);
        }
    }
    
    return novelty;
}

float Novelty::detectMemoryNovelty() {
    if (pImpl->memorySystem.episodicMemory.empty()) return 0.0f;
    
    // Get current pattern
    float currentPattern = pImpl->patternDetector.patternHistory.empty() ? 
                          0.0f : pImpl->patternDetector.patternHistory.back();
    
    // Find most similar pattern in memory
    float bestSimilarity = 0.0f;
    
    // Compare with episodic memory
    for (float storedPattern : pImpl->memorySystem.episodicMemory) {
        float similarity = 1.0f - std::abs(currentPattern - storedPattern);
        bestSimilarity = std::max(bestSimilarity, similarity);
    }
    
    // Compare with semantic memory
    for (float storedPattern : pImpl->memorySystem.semanticMemory) {
        float similarity = 1.0f - std::abs(currentPattern - storedPattern);
        bestSimilarity = std::max(bestSimilarity, similarity);
    }
    
    // Novelty is inverse of best similarity
    float novelty = 1.0f - bestSimilarity;
    
    return novelty;
}

void Novelty::updateStatisticalMoments() {
    if (pImpl->patternDetector.patternHistory.size() < 2) return;
    
    // Compute statistical moments
    float sum = 0.0f, sumSq = 0.0f;
    for (float val : pImpl->patternDetector.patternHistory) {
        sum += val;
        sumSq += val * val;
    }
    
    float n = static_cast<float>(pImpl->patternDetector.patternHistory.size());
    float mean = sum / n;
    float variance = (sumSq - sum * sum / n) / (n - 1);
    
    // Update moments
    pImpl->patternDetector.statisticalMoments[0] = mean;
    pImpl->patternDetector.statisticalMoments[1] = variance;
    
    // Simple skew calculation
    if (variance > 0.0f) {
        float stdDev = std::sqrt(variance);
        float skew = 0.0f;
        for (float val : pImpl->patternDetector.patternHistory) {
            skew += std::pow((val - mean) / stdDev, 3);
        }
        skew /= n;
        pImpl->patternDetector.statisticalMoments[2] = skew;
    }
    
    // Simple kurtosis calculation
    if (variance > 0.0f) {
        float stdDev = std::sqrt(variance);
        float kurtosis = 0.0f;
        for (float val : pImpl->patternDetector.patternHistory) {
            kurtosis += std::pow((val - mean) / stdDev, 4);
        }
        kurtosis /= n;
        pImpl->patternDetector.statisticalMoments[3] = kurtosis;
    }
}

void Novelty::updateFrequencyDetection(float dt) {
    // Clean up old frequency entries
    float cleanupTime = pImpl->lastUpdateTime - 1000.0f; // Remove entries older than 1000ms
    
    std::vector<size_t> toRemove;
    for (const auto& pair : pImpl->frequencyDetector.lastOccurrenceTime) {
        if (pair.second < cleanupTime) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (size_t featureId : toRemove) {
        pImpl->frequencyDetector.occurrenceCounts.erase(featureId);
        pImpl->frequencyDetector.lastOccurrenceTime.erase(featureId);
    }
    
    // Decay novelty score
    pImpl->frequencyDetector.noveltyScore *= (1.0f - pImpl->patternDetector.adaptationRate * dt);
}

void Novelty::updateContextDetection() {
    // Update context vector based on current novelty level
    pImpl->contextDetector.contextVector[0] = pImpl->noveltyHistory;
    pImpl->contextDetector.contextVector[1] = pImpl->patternDetector.statisticalMoments[1]; // variance
    pImpl->contextDetector.contextVector[2] = pImpl->patternDetector.statisticalMoments[2]; // skew
    pImpl->contextDetector.contextVector[3] = pImpl->patternDetector.statisticalMoments[3]; // kurtosis
    
    // Store context state
    pImpl->contextDetector.contextPatterns.push_back(pImpl->contextDetector.contextVector);
    if (pImpl->contextDetector.contextPatterns.size() > 100) {
        pImpl->contextDetector.contextPatterns.erase(pImpl->contextDetector.contextPatterns.begin());
    }
}

void Novelty::updateMemorySystem(float dt) {
    // Decay episodic memory
    for (float& val : pImpl->memorySystem.episodicMemory) {
        val *= (1.0f - pImpl->memorySystem.memoryDecayRate * dt);
    }
    
    // Decay semantic memory
    for (float& val : pImpl->memorySystem.semanticMemory) {
        val *= (1.0f - pImpl->memorySystem.memoryDecayRate * dt);
    }
    
    // Clean up low-value memories
    auto cleanupValue = pImpl->memorySystem.similarityThreshold;
    pImpl->memorySystem.episodicMemory.erase(
        std::remove_if(pImpl->memorySystem.episodicMemory.begin(),
                      pImpl->memorySystem.episodicMemory.end(),
                      [cleanupValue](float val) { return val < cleanupValue; }),
        pImpl->memorySystem.episodicMemory.end()
    );
    
    pImpl->memorySystem.semanticMemory.erase(
        std::remove_if(pImpl->memorySystem.semanticMemory.begin(),
                      pImpl->memorySystem.semanticMemory.end(),
                      [cleanupValue](float val) { return val < cleanupValue; }),
        pImpl->memorySystem.semanticMemory.end()
    );
}

std::vector<float> Novelty::extractFeatures(const std::vector<float>& pattern) {
    std::vector<float> features(pImpl->featureExtractor.featureDimension, 0.0f);
    
    if (pattern.empty()) return features;
    
    // Simple feature extraction: compute statistics of the pattern
    float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    float mean = sum / pattern.size();
    
    float variance = 0.0f;
    for (float val : pattern) {
        variance += (val - mean) * (val - mean);
    }
    variance /= pattern.size();
    
    float stdDev = std::sqrt(variance);
    
    // Fill features with pattern statistics
    features[0] = mean;
    features[1] = stdDev;
    features[2] = mean / (std::max(1.0f, stdDev)); // Coefficient of variation
    
    // Fill remaining features with pattern values (limited)
    size_t featureCount = std::min(static_cast<size_t>(pattern.size()), 
                                 static_cast<size_t>(features.size()) - 3);
    for (size_t i = 0; i < featureCount; ++i) {
        features[i + 3] = pattern[i];
    }
    
    return features;
}

float Novelty::computePatternSimilarity(const std::vector<float>& pattern1,
                                       const std::vector<float>& pattern2) {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    // Compute cosine similarity
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    size_t compareSize = std::min(pattern1.size(), pattern2.size());
    
    for (size_t i = 0; i < compareSize; ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / (norm1 * norm2);
}

void Novelty::storePattern(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Store in episodic memory
    pImpl->memorySystem.episodicMemory.push_back(pattern[0]); // Use first value for simplicity
    
    // Store in semantic memory (cleaner, more abstract representation)
    float semanticValue = computePatternSemantics(pattern);
    pImpl->memorySystem.semanticMemory.push_back(semanticValue);
    
    // Maintain memory size limits
    if (pImpl->memorySystem.episodicMemory.size() > 1000) {
        pImpl->memorySystem.episodicMemory.erase(pImpl->memorySystem.episodicMemory.begin());
    }
    
    if (pImpl->memorySystem.semanticMemory.size() > 500) {
        pImpl->memorySystem.semanticMemory.erase(pImpl->memorySystem.semanticMemory.begin());
    }
}

float Novelty::computePatternSemantics(const std::vector<float>& pattern) {
    if (pattern.empty()) return 0.0f;
    
    // Compute semantic value: average of normalized pattern values
    float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    return sum / pattern.size();
}

std::vector<float> Novelty::observationToPattern(const Observation& observation) {
    std::vector<float> pattern;
    
    // Convert observation data to pattern
    // This would typically involve encoding the observation data
    // into a numerical pattern for analysis
    
    // For now, we'll use a simple conversion
    const auto& data = observation.getData();
    pattern.assign(data.begin(), data.end());
    
    return pattern;
}

} // namespace nlm
