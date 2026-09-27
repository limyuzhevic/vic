#include "MetaLearning.hpp"
#include "../core/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>
#include <tuple>
#include <unordered_map>

namespace nlm {

struct MetaLearning::Impl {
    class Brain* brain;
    float baseLearningRate;
    float adaptiveLearningRate;
    size_t memorySize;
    
    // Meta-memory for storing learning patterns
    std::vector<std::pair<std::vector<float>, float>> metaMemory;
    
    // Learning state tracking
    bool enabled;
    float learningEfficiency;
    size_t samplesProcessed;
    float transferLearningScore;
    float adaptationSpeed;
    
    // Adaptation parameters
    float learningRateMin;
    float learningRateMax;
    float learningRateTarget;
    float adaptationFactor;
    
    // Learning strategy
    std::string currentStrategy;
    bool transferLearningEnabled;
    bool curriculumLearningEnabled;
    
    // Statistics
    float totalLearningPerformance;
    size_t goodLearningCount;
    std::vector<float> performanceHistory;
    
    Impl() : brain(nullptr), baseLearningRate(0.01f), adaptiveLearningRate(0.01f),
             memorySize(1000), enabled(true), learningEfficiency(0.0f),
             samplesProcessed(0), transferLearningScore(0.0f), adaptationSpeed(0.0f),
             learningRateMin(0.001f), learningRateMax(0.1f), learningRateTarget(0.01f),
             adaptationFactor(0.1f), transferLearningEnabled(true),
             curriculumLearningEnabled(false),
             totalLearningPerformance(0.0f), goodLearningCount(0) {}
};

MetaLearning::MetaLearning() : pImpl(std::make_unique<Impl>()) {}

MetaLearning::~MetaLearning() = default;

void MetaLearning::initialize(class Brain* brain, float initialLearningRate,
                             size_t memorySize) {
    pImpl->brain = brain;
    pImpl->baseLearningRate = initialLearningRate;
    pImpl->adaptiveLearningRate = initialLearningRate;
    pImpl->memorySize = memorySize;
    
    // Initialize meta-memory
    pImpl->metaMemory.reserve(memorySize);
    
    // Set default learning strategy
    pImpl->currentStrategy = "adaptive_rate";
    
    NLM_LOG_INFO("Meta-learning system initialized");
    NLM_LOG_INFO("Base learning rate: " + std::to_string(initialLearningRate));
    NLM_LOG_INFO("Memory size: " + std::to_string(memorySize));
    NLM_LOG_INFO("Current strategy: " + pImpl->currentStrategy);
}

void MetaLearning::update(float learningPerformance, const std::vector<float>& state) {
    if (!pImpl->enabled) {
        return;
    }
    
    // Store current learning performance
    pImpl->totalLearningPerformance += learningPerformance;
    pImpl->samplesProcessed++;
    pImpl->performanceHistory.push_back(learningPerformance);
    
    // Update learning efficiency
    if (learningPerformance > 0.7f) {
        pImpl->goodLearningCount++;
    }
    
    // Calculate learning efficiency
    if (!pImpl->performanceHistory.empty()) {
        float recentAvg = std::accumulate(pImpl->performanceHistory.end() - 
                                         std::min(size_t(10), pImpl->performanceHistory.size()),
                                         pImpl->performanceHistory.end(), 0.0f) /
                         std::min(size_t(10), pImpl->performanceHistory.size());
        pImpl->learningEfficiency = recentAvg;
    }
    
    // Update adaptive learning rate
    updateAdaptiveLearningRate(learningPerformance, state);
    
    // Store learning pattern
    storeLearningPattern(state, learningPerformance);
    
    // Apply meta-learning strategies
    applyLearningStrategies(state, learningPerformance);
}

float MetaLearning::getAdaptiveLearningRate() const {
    return pImpl->adaptiveLearningRate;
}

std::vector<float> MetaLearning::getLearningStrategy(const std::vector<float>& state) const {
    std::vector<float> strategy;
    
    // Retrieve best learning pattern for current state
    std::vector<float> pattern = retrieveLearningPattern(state);
    
    // Convert pattern to learning strategy recommendations
    // This is a simplified implementation - real meta-learning would be more complex
    for (size_t i = 0; i < std::min(size_t(10), pattern.size()); ++i) {
        // Scale pattern to reasonable learning rate range
        float strategyValue = pImpl->baseLearningRate + pattern[i] * 
                             (pImpl->learningRateMax - pImpl->learningRateMin);
        strategyValue = std::clamp(strategyValue, pImpl->learningRateMin, pImpl->learningRateMax);
        strategy.push_back(strategyValue);
    }
    
    // If no pattern found, use default strategy
    if (pattern.empty()) {
        strategy.assign(10, pImpl->baseLearningRate);
    }
    
    return strategy;
}

float MetaLearning::getLearningEfficiency() const {
    return pImpl->learningEfficiency;
}

size_t MetaLearning::getMetaMemoryUsage() const {
    return pImpl->metaMemory.size();
}

float MetaLearning::getTransferLearningScore() const {
    return pImpl->transferLearningScore;
}

const std::vector<std::pair<std::vector<float>, float>>& MetaLearning::getMetaMemory() const {
    return pImpl->metaMemory;
}

void MetaLearning::storeLearningPattern(const std::vector<float>& pattern, float performance) {
    // Store pattern in meta-memory
    pImpl->metaMemory.emplace_back(pattern, performance);
    
    // Trim memory if too large
    if (pImpl->metaMemory.size() > pImpl->memorySize) {
        pImpl->metaMemory.erase(pImpl->metaMemory.begin());
    }
    
    // Update transfer learning score based on pattern effectiveness
    if (performance > 0.8f) {
        pImpl->transferLearningScore = std::min(1.0f, pImpl->transferLearningScore + 0.01f);
    }
}

std::vector<float> MetaLearning::retrieveLearningPattern(const std::vector<float>& state) const {
    if (pImpl->metaMemory.empty()) {
        return std::vector<float>();
    }
    
    // Find best matching pattern in meta-memory
    std::vector<float> bestPattern;
    float bestScore = -1.0f;
    
    for (const auto& entry : pImpl->metaMemory) {
        const std::vector<float>& pattern = entry.first;
        float performance = entry.second;
        
        // Calculate pattern similarity (simplified: dot product)
        float similarity = 0.0f;
        size_t compareSize = std::min(pattern.size(), state.size());
        
        for (size_t i = 0; i < compareSize; ++i) {
            similarity += pattern[i] * state[i];
        }
        
        // Score based on similarity and performance
        float score = similarity * performance;
        
        if (score > bestScore) {
            bestScore = score;
            bestPattern = pattern;
        }
    }
    
    return bestPattern;
}

void MetaLearning::enable(bool enable) {
    pImpl->enabled = enable;
}

bool MetaLearning::isEnabled() const {
    return pImpl->enabled;
}

void MetaLearning::setInitialLearningRate(float rate) {
    pImpl->baseLearningRate = rate;
    pImpl->learningRateTarget = rate;
}

void MetaLearning::setMemorySize(size_t size) {
    pImpl->memorySize = size;
    pImpl->metaMemory.reserve(size);
}

void MetaLearning::setTransferThreshold(float threshold) {
    // Store threshold for transfer learning decisions
}

void MetaLearning::setLearningStrategy(const std::string& strategy) {
    pImpl->currentStrategy = strategy;
}

void MetaLearning::enableTransferLearning(bool enable) {
    pImpl->transferLearningEnabled = enable;
}

void MetaLearning::enableCurriculumLearning(bool enable) {
    pImpl->curriculumLearningEnabled = enable;
}

float MetaLearning::getLearningProgress() const {
    if (pImpl->performanceHistory.empty()) {
        return 0.0f;
    }
    
    // Calculate learning progress based on performance improvement
    float firstPerformance = pImpl->performanceHistory.front();
    float lastPerformance = pImpl->performanceHistory.back();
    
    return std::max(0.0f, lastPerformance - firstPerformance);
}

size_t MetaLearning::getSamplesProcessed() const {
    return pImpl->samplesProcessed;
}

float MetaLearning::getAdaptationSpeed() const {
    if (pImpl->performanceHistory.size() < 2) {
        return 0.0f;
    }
    
    // Calculate adaptation speed based on performance changes
    size_t recentSize = std::min(size_t(10), pImpl->performanceHistory.size());
    
    float earlyAvg = std::accumulate(pImpl->performanceHistory.begin(),
                                     pImpl->performanceHistory.begin() + recentSize / 2,
                                     0.0f) / (recentSize / 2);
    
    float laterAvg = std::accumulate(pImpl->performanceHistory.end() - recentSize / 2,
                                     pImpl->performanceHistory.end(), 0.0f) / (recentSize / 2);
    
    return laterAvg - earlyAvg;
}

void MetaLearning::updateAdaptiveLearningRate(float learningPerformance, 
                                                const std::vector<float>& state) {
    // Update adaptive learning rate based on learning performance
    
    // Target learning rate
    float targetLR = pImpl->baseLearningRate;
    
    // Adjust target based on learning performance
    if (learningPerformance < 0.3f) {
        // Poor performance: decrease learning rate to avoid instability
        targetLR *= 0.5f;
    } else if (learningPerformance > 0.8f) {
        // Good performance: can increase learning rate
        targetLR *= 1.2f;
    }
    
    // Store pattern for future reference
    storeLearningPattern(state, learningPerformance);
    
    // Smooth adaptation
    pImpl->adaptiveLearningRate += (targetLR - pImpl->adaptiveLearningRate) * pImpl->adaptationFactor;
    
    // Clamp to reasonable bounds
    pImpl->adaptiveLearningRate = std::clamp(pImpl->adaptiveLearningRate,
                                           pImpl->learningRateMin, pImpl->learningRateMax);
}

void MetaLearning::applyLearningStrategies(const std::vector<float>& state, float performance) {
    // Apply various meta-learning strategies based on current state and performance
    
    // Transfer learning
    if (pImpl->transferLearningEnabled && performance > 0.7f) {
        // Store pattern for potential transfer to similar states
        storeLearningPattern(state, performance * 0.9f); // Slightly reduced confidence for transfer
    }
    
    // Curriculum learning
    if (pImpl->curriculumLearningEnabled) {
        // Adapt learning based on state complexity
        float complexity = calculateStateComplexity(state);
        
        if (complexity > 0.7f && performance > 0.5f) {
            // High complexity with good performance: increase learning rate
            pImpl->adaptiveLearningRate = std::min(pImpl->learningRateMax,
                                                 pImpl->adaptiveLearningRate * 1.1f);
        } else if (complexity < 0.3f && performance > 0.8f) {
            // Low complexity with good performance: decrease learning rate to fine-tune
            pImpl->adaptiveLearningRate = std::max(pImpl->learningRateMin,
                                                 pImpl->adaptiveLearningRate * 0.9f);
        }
    }
    
    // Adaptive exploration
    float explorationRate = calculateExplorationRate(performance);
    // Note: In a real implementation, this would affect exploration rates
}

float MetaLearning::calculateStateComplexity(const std::vector<float>& state) const {
    // Calculate complexity of a state
    if (state.empty()) {
        return 0.0f;
    }
    
    // Use variance as a simple measure of complexity
    float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
    float variance = 0.0f;
    
    for (float value : state) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    variance /= state.size();
    
    // Normalize variance to [0, 1] range
    return std::min(1.0f, variance * 10.0f); // Scale for reasonable range
}

float MetaLearning::calculateExplorationRate(float performance) const {
    // Calculate optimal exploration rate based on performance
    if (performance < 0.3f) {
        return 0.3f; // High exploration when performance is low
    } else if (performance < 0.7f) {
        return 0.1f; // Moderate exploration
    } else {
        return 0.01f; // Low exploration when performance is high
    }
}

} // namespace nlm