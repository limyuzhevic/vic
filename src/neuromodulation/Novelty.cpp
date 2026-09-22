#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    // Real-time novelty detection parameters
    float adaptationRate;      // Rate of adaptation to new patterns
    float detectionSensitivity; // Sensitivity to differences
    float temporalContrast;    // Contrast with recent history
    float patternLength;       // Length of patterns for comparison
    float encodingPrecision;   // Precision of pattern encoding
    
    // Novelty computation
    std::vector<float> novelFeatures;
    std::vector<float> baselineFeatures;
    float noveltyScore;
    Timestamp lastUpdateTime;
    
    // Integration with curiosity and learning
    float noveltyLearningRate;  // How novelty affects learning
    float attentionGain;        // Novelty effect on attention
    float memoryEncoding;       // Novelty effect on memory encoding
    
    // Statistics
    float meanNovelty;
    float maxNovelty;
    size_t totalNovelties;
    
    // Random number generator
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f)
           , adaptationRate(0.05f), detectionSensitivity(0.7f), temporalContrast(0.5f)
           , patternLength(50.0f), encodingPrecision(0.8f)
           , noveltyScore(0.0f), noveltyLearningRate(0.2f), attentionGain(0.3f)
           , memoryEncoding(0.4f), meanNovelty(0.0f), maxNovelty(0.0f), totalNovelties(0)
    {
        std::random_device rd;
        rng.seed(rd());
        noiseDist = std::normal_distribution<float>(0.0f, 0.05f);
        novelFeatures.reserve(100);
        baselineFeatures.reserve(100);
    }
};

Novelty::Novelty() : pImpl(new Impl) {
    NLM_LOG_INFO("Novelty detection system initialized with real dynamics");
}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->lastUpdateTime = 0.0f;
    NLM_LOG_INFO("Novelty detection initialized with brain integration");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations (simplified)
    std::vector<float> currentPattern;
    
    // Convert observation to feature vector
    // In a real implementation, this would extract meaningful features
    // For now, use a simplified approach
    if (!observation.sensoryData.empty()) {
        currentPattern = observation.sensoryData;
    } else if (!observation.actionHistory.empty()) {
        // Use action history as proxy for pattern
        currentPattern.resize(observation.actionHistory.size());
        for (size_t i = 0; i < observation.actionHistory.size(); ++i) {
            currentPattern[i] = observation.actionHistory[i] ? 1.0f : 0.0f;
        }
    } else {
        // Fallback to random pattern
        currentPattern.resize(10);
        if (pImpl->brain) {
            auto* rng = pImpl->brain->getRandomGenerator();
            for (float& val : currentPattern) {
                val = rng->generateFloat(0.0f, 1.0f);
            }
        } else {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            for (float& val : currentPattern) {
                val = dist(pImpl->rng);
            }
        }
    }
    
    // Compare with previous observation if available
    if (!previousObservation.sensoryData.empty() && 
        !observation.sensoryData.empty()) {
        const auto& previousPattern = previousObservation.sensoryData;
        detectNovelty(currentPattern, previousPattern);
    } else {
        // If no previous pattern, set novelty to maximum (completely new)
        pImpl->level = 1.0f;
        pImpl->noveltyScore = 1.0f;
    }
    
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        pImpl->noveltyScore = 0.0f;
        return;
    }
    
    // Normalize patterns to [0, 1] range
    std::vector<float> normCurrent = currentPattern;
    std::vector<float> normPrevious = previousPattern;
    
    // Apply normalization
    float currentMax = *std::max_element(currentPattern.begin(), currentPattern.end());
    float previousMax = *std::max_element(previousPattern.begin(), previousPattern.end());
    
    if (currentMax > 0.0f) {
        for (float& val : normCurrent) val /= currentMax;
    }
    if (previousMax > 0.0f) {
        for (float& val : normPrevious) val /= previousMax;
    }
    
    // Compute multiple novelty metrics
    std::vector<float> noveltyMetrics;
    
    // 1. Euclidean distance (L2 norm)
    float euclideanDiff = 0.0f;
    size_t compareLen = std::min(normCurrent.size(), normPrevious.size());
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = normCurrent[i] - normPrevious[i];
        euclideanDiff += diff * diff;
    }
    euclideanDiff = std::sqrt(euclideanDiff);
    noveltyMetrics.push_back(euclideanDiff);
    
    // 2. Cosine similarity (1 - cosine distance)
    float dotProduct = 0.0f;
    float normCurrentLen = 0.0f;
    float normPreviousLen = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        dotProduct += normCurrent[i] * normPrevious[i];
        normCurrentLen += normCurrent[i] * normCurrent[i];
        normPreviousLen += normPrevious[i] * normPrevious[i];
    }
    normCurrentLen = std::sqrt(normCurrentLen);
    normPreviousLen = std::sqrt(normPreviousLen);
    float cosineSimilarity = 0.0f;
    if (normCurrentLen > 0.0f && normPreviousLen > 0.0f) {
        cosineSimilarity = dotProduct / (normCurrentLen * normPreviousLen);
    }
    noveltyMetrics.push_back(1.0f - cosineSimilarity);  // Convert to novelty
    
    // 3. Statistical difference (mean absolute difference)
    float statDiff = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        statDiff += std::abs(normCurrent[i] - normPrevious[i]);
    }
    statDiff /= static_cast<float>(compareLen);
    noveltyMetrics.push_back(statDiff);
    
    // 4. Pattern shift (first derivative difference)
    float shiftDiff = 0.0f;
    if (normCurrent.size() > 1 && normPrevious.size() > 1 && compareLen > 1) {
        for (size_t i = 1; i < compareLen; ++i) {
            float currentShift = normCurrent[i] - normCurrent[i-1];
            float previousShift = normPrevious[i] - normPrevious[i-1];
            shiftDiff += std::abs(currentShift - previousShift);
        }
        shiftDiff /= static_cast<float>(compareLen - 1);
    }
    noveltyMetrics.push_back(shiftDiff);
    
    // Weighted combination of novelty metrics
    float totalNovelty = 0.0f;
    float weightSum = 0.0f;
    
    // Euclidean distance has higher weight (sensitive to magnitude changes)
    totalNovelty += noveltyMetrics[0] * 0.3f;
    weightSum += 0.3f;
    
    // Cosine similarity has medium weight (sensitive to pattern direction)
    totalNovelty += noveltyMetrics[1] * 0.25f;
    weightSum += 0.25f;
    
    // Statistical difference has medium weight (sensitive to average differences)
    totalNovelty += noveltyMetrics[2] * 0.25f;
    weightSum += 0.25f;
    
    // Pattern shift has lower weight (sensitive to dynamics)
    totalNovelty += noveltyMetrics[3] * 0.2f;
    weightSum += 0.2f;
    
    pImpl->noveltyScore = totalNovelty / weightSum;
    
    // Apply adaptation (novelty decreases with exposure)
    float adaptation = 1.0f - pImpl->adaptationRate;
    pImpl->level = pImpl->noveltyScore * adaptation;
    
    // Add noise for realistic biological variability
    float noise = noiseDist(pImpl->rng);
    pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level + noise));
    
    // Update temporal contrast (compare with recent history)
    if (!pImpl->history.empty()) {
        float recentMean = 0.0f;
        size_t recentCount = std::min(static_cast<size_t>(10), pImpl->history.size());
        for (size_t i = 0; i < recentCount; ++i) {
            recentMean += pImpl->history[pImpl->history.size() - 1 - i];
        }
        recentMean /= static_cast<float>(recentCount);
        pImpl->temporalContrast = std::abs(pImpl->level - recentMean);
    }
    
    // Update baseline features for adaptation
    pImpl->baselineFeatures = normCurrent;
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Update statistics
    pImpl->meanNovelty = (pImpl->meanNovelty * pImpl->totalNovelties + pImpl->level) / 
                        (static_cast<float>(++pImpl->totalNovelties));
    pImpl->maxNovelty = std::max(pImpl->maxNovelty, pImpl->level);
    
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Log significant novelties
    if (pImpl->level > 0.7f) {
        NLM_LOG_DEBUG("High novelty detected: score={}, contrast={}", 
                     pImpl->level, pImpl->temporalContrast);
    }
}

void Novelty::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Decay novelty over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * timeStep);
    
    // Further decay based on temporal contrast
    if (pImpl->temporalContrast > 0.5f) {
        pImpl->level *= (1.0f - pImpl->decayRate * 0.5f * timeStep);
    }
    
    // Maintain time since last update
    pImpl->lastUpdateTime += timeStep;
    
    // Sample from history for statistics
    if (pImpl->history.size() > 10) {
        float historyMean = 0.0f;
        for (float val : pImpl->history) {
            historyMean += val;
        }
        historyMean /= static_cast<float>(pImpl->history.size());
        pImpl->meanNovelty = historyMean;
        
        pImpl->maxNovelty = *std::max_element(pImpl->history.begin(), pImpl->history.end());
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->meanNovelty = 0.0f;
    pImpl->maxNovelty = 0.0f;
    pImpl->totalNovelties = 0;
}

// New methods for enhanced novelty detection
void Novelty::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 1.0f);
}

void Novelty::setDetectionSensitivity(float sensitivity) {
    pImpl->detectionSensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Novelty::setNoveltyLearningRate(float rate) {
    pImpl->noveltyLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Novelty::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 1.0f);
}

void Novelty::setMemoryEncoding(float encoding) {
    pImpl->memoryEncoding = std::clamp(encoding, 0.0f, 1.0f);
}

float Novelty::getNoveltyScore() const {
    return pImpl->noveltyScore;
}

float Novelty::getTemporalContrast() const {
    return pImpl->temporalContrast;
}

float Novelty::getMeanNovelty() const {
    return pImpl->meanNovelty;
}

float Novelty::getMaxNovelty() const {
    return pImpl->maxNovelty;
}

void Novelty::updateAttentionModulation(float &attentionWeight) {
    // Novelty modulates attentional gain
    // High novelty increases attentional gain for exploration
    float normalized = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Novelty enhances attentional scope
    attentionWeight += normalized * pImpl->attentionGain;
    attentionWeight = std::clamp(attentionWeight, 0.5f, 2.0f);
    
    // Very high novelty causes attentional narrowing (focus on novel stimulus)
    if (normalized > 0.8f) {
        attentionWeight *= 0.8f;
    }
}

void Novelty::updateMemoryEncoding(float &encodingWeight) {
    // Novelty enhances memory encoding
    float normalized = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Novelty boosts memory encoding strength
    encodingWeight = 0.7f + normalized * pImpl->memoryEncoding;
    encodingWeight = std::clamp(encodingWeight, 0.5f, 1.5f);
    
    // Very high novelty causes memory saturation
    if (normalized > 0.9f) {
        encodingWeight *= 0.9f;
    }
}

void Novelty::updateLearningRate(float &learningRate) {
    // Novelty modulates learning rate
    float normalized = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Novelty increases learning rate
    learningRate = 0.01f + normalized * pImpl->noveltyLearningRate * 0.1f;
    learningRate = std::clamp(learningRate, 0.001f, 0.2f);
}

} // namespace nlm
