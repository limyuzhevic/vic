#include "Reward.hpp"
#include "../environment/Observation.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward computation parameters
    float weight;           // Learning weight (0.0 to 1.0)
    float baseline;         // Baseline reward level
    float explorationBonus; // Bonus for novel/explorative behavior
    float survivalBonus;    // Bonus for maintaining homeostasis
    
    // Statistical tracking
    float maxReward;
    float minReward;
    float recentAvg;        // Average of recent rewards
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), 
             weight(1.0f), baseline(0.0f), explorationBonus(0.0f),
             survivalBonus(0.0f), maxReward(0.0f), minReward(0.0f),
             recentAvg(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
    
    // Keep only recent history (last 1000 rewards)
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update statistics
    pImpl->maxReward = std::max(pImpl->maxReward, delta);
    pImpl->minReward = std::min(pImpl->minReward, delta);
    
    // Update recent average
    if (pImpl->history.size() > 0) {
        float sum = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
        pImpl->recentAvg = sum / static_cast<float>(pImpl->history.size());
    }
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
    pImpl->maxReward = 0.0f;
    pImpl->minReward = 0.0f;
    pImpl->recentAvg = 0.0f;
}

float Reward::computeReward(const SensoryInput& sensoryInput) const {
    float baseReward = computeBaseReward(sensoryInput);
    float noveltyReward = computeNoveltyReward(sensoryInput);
    float survivalReward = computeSurvivalReward(sensoryInput);
    float explorationReward = computeExplorationReward(sensoryInput);
    
    // Apply weights and sum
    float totalReward = baseReward + 
                       pImpl->explorationBonus * noveltyReward +
                       pImpl->survivalBonus * survivalReward +
                       pImpl->weight * explorationReward;
    
    return totalReward;
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    return computeReward(*sensoryInput);
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->maxReward = 0.0f;
    pImpl->minReward = 0.0f;
    pImpl->recentAvg = 0.0f;
}

float Reward::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

void Reward::setWeight(float weight) {
    pImpl->weight = std::max(0.0f, std::min(1.0f, weight));
}

void Reward::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Reward::getWeight() const {
    return pImpl->weight;
}

float Reward::getBaseline() const {
    return pImpl->baseline;
}

// Helper methods for reward computation

float Reward::computeBaseReward(const SensoryInput& sensoryInput) const {
    const std::vector<float>& data = sensoryInput.getData();
    float baseReward = 0.0f;
    
    // For vision input: reward based on image processing success
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        // Vision reward based on contrast and texture complexity
        float contrast = computeContrast(data);
        float complexity = computeComplexity(data);
        baseReward = (contrast + complexity) * 0.1f;
        
        // Positive reward for interesting visual scenes
        if (complexity > 0.5f && contrast > 0.3f) {
            baseReward += 1.0f;
        }
    }
    // For audio input: reward based on sound patterns
    else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        // Audio reward based on signal quality
        float clarity = computeAudioClarity(data);
        float rhythm = computeRhythm(data);
        baseReward = (clarity + rhythm) * 0.1f;
        
        // Positive reward for structured sounds
        if (rhythm > 0.6f) {
            baseReward += 0.5f;
        }
    }
    // For internal signals: reward based on neural activity patterns
    else if (strcmp(sensoryInput.getType(), "InternalSignals") == 0) {
        // Internal reward based on activation patterns
        float activation = computeInternalActivation(data);
        float coherence = computeCoherence(data);
        baseReward = (activation + coherence) * 0.05f;
        
        // Positive reward for coherent internal state
        if (coherence > 0.7f && activation > 0.3f) {
            baseReward += 0.3f;
        }
    }
    
    // Apply baseline offset
    baseReward += pImpl->baseline;
    
    return baseReward;
}

float Reward::computeNoveltyReward(const SensoryInput& sensoryInput) const {
    float novelty = 0.0f;
    const std::vector<float>& data = sensoryInput.getData();
    
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        // Novelty based on unexpected visual patterns
        novelty = computeVisualNovelty(data);
    } else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        // Novelty based on unexpected sound patterns
        novelty = computeAudioNovelty(data);
    } else if (strcmp(sensoryInput.getType(), "InternalSignals") == 0) {
        // Novelty based on unexpected internal states
        novelty = computeInternalNovelty(data);
    }
    
    return std::max(0.0f, novelty);
}

float Reward::computeSurvivalReward(const SensoryInput& sensoryInput) const {
    float survival = 0.0f;
    const std::vector<float>& data = sensoryInput.getData();
    
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        // Survival based on visual stability and object detection
        survival = computeVisualStability(data);
    } else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        // Survival based on auditory continuity
        survival = computeAuditoryContinuity(data);
    } else if (strcmp(sensoryInput.getType(), "InternalSignals") == 0) {
        // Survival based on physiological homeostasis
        survival = computeHomeostasis(data);
    }
    
    return survival;
}

float Reward::computeExplorationReward(const SensoryInput& sensoryInput) const {
    float exploration = 0.0f;
    const std::vector<float>& data = sensoryInput.getData();
    
    // Exploration reward based on uncertainty and information gain
    if (strcmp(sensoryInput.getType(), "Vision") == 0) {
        exploration = computeVisualExploration(data);
    } else if (strcmp(sensoryInput.getType(), "Audio") == 0) {
        exploration = computeAudioExploration(data);
    } else if (strcmp(sensoryInput.getType(), "InternalSignals") == 0) {
        exploration = computeInternalExploration(data);
    }
    
    return exploration;
}

// Specific computation helpers

float Reward::computeContrast(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    return std::sqrt(variance);
}

float Reward::computeComplexity(const std::vector<float>& data) const {
    if (data.size() < 3) return 0.0f;
    
    // Simple complexity metric based on gradient changes
    float complexity = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        complexity += std::abs(data[i] - data[i-1]);
    }
    complexity /= data.size() - 1;
    
    return std::min(1.0f, complexity);
}

float Reward::computeAudioClarity(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    return std::sqrt(variance) / (std::sqrt(variance) + 0.1f);
}

float Reward::computeRhythm(const std::vector<float>& data) const {
    if (data.size() < 3) return 0.0f;
    
    // Simple rhythm detection based on periodicity
    float rhythm = 0.0f;
    size_t peaks = 0;
    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (data[i] > data[i-1] && data[i] > data[i+1]) {
            peaks++;
        }
    }
    
    if (peaks > 0) {
        rhythm = static_cast<float>(peaks) / (data.size() - 2);
    }
    
    return rhythm;
}

float Reward::computeInternalActivation(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float avg = sum / data.size();
    
    // Activation based on deviation from baseline
    float activation = std::abs(avg - 0.5f);
    return std::min(1.0f, activation * 2.0f);
}

float Reward::computeCoherence(const std::vector<float>& data) const {
    if (data.size() < 2) return 0.0f;
    
    // Coherence based on signal correlation
    float coherence = 0.0f;
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    for (float val : data) {
        coherence += (val - mean) * (val - mean);
    }
    coherence /= data.size();
    
    return std::min(1.0f, coherence * 10.0f);
}

float Reward::computeVisualNovelty(const std::vector<float>& data) const {
    // Placeholder for visual novelty detection
    // Could be based on contrast, texture, or pattern changes
    if (data.empty()) return 0.0f;
    
    float novelty = computeContrast(data) + computeComplexity(data);
    return std::min(1.0f, novelty);
}

float Reward::computeAudioNovelty(const std::vector<float>& data) const {
    // Placeholder for audio novelty detection
    // Could be based on spectral changes, rhythm patterns
    if (data.empty()) return 0.0f;
    
    float novelty = computeAudioClarity(data) + computeRhythm(data);
    return std::min(1.0f, novelty);
}

float Reward::computeInternalNovelty(const std::vector<float>& data) const {
    // Placeholder for internal novelty detection
    // Could be based on unexpected activation patterns
    if (data.empty()) return 0.0f;
    
    float novelty = computeInternalActivation(data) + computeCoherence(data);
    return std::min(1.0f, novelty);
}

float Reward::computeVisualStability(const std::vector<float>& data) const {
    // Stability based on consistency of visual input
    if (data.size() < 3) return 0.0f;
    
    float stability = 0.0f;
    // Simple average of local consistency
    for (size_t i = 1; i < data.size() - 1; ++i) {
        float localDiff = std::abs(data[i] - data[i-1]);
        float localSmooth = std::abs(data[i] - data[i+1]);
        stability += 1.0f - (localDiff + localSmooth) * 0.5f;
    }
    
    stability /= data.size() - 2;
    return stability;
}

float Reward::computeAuditoryContinuity(const std::vector<float>& data) const {
    // Continuity based on temporal consistency
    if (data.size() < 3) return 0.0f;
    
    float continuity = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        continuity += 1.0f - std::abs(data[i] - data[i-1]);
    }
    
    continuity /= data.size() - 1;
    return continuity;
}

float Reward::computeHomeostasis(const std::vector<float>& data) const {
    // Homeostasis based on physiological balance
    if (data.empty()) return 0.0f;
    
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float avg = sum / data.size();
    
    // Reward for being near optimal level (0.5)
    float homeostasis = 1.0f - std::abs(avg - 0.5f);
    return std::max(0.0f, homeostasis);
}

float Reward::computeVisualExploration(const std::vector<float>& data) const {
    // Exploration based on information gain and uncertainty
    float exploration = computeVisualNovelty(data) * 0.7f + 
                        (1.0f - computeVisualStability(data)) * 0.3f;
    return exploration;
}

float Reward::computeAudioExploration(const std::vector<float>& data) const {
    // Exploration based on uncertainty and information gain
    float exploration = computeAudioNovelty(data) * 0.7f + 
                        (1.0f - computeAuditoryContinuity(data)) * 0.3f;
    return exploration;
}

float Reward::computeInternalExploration(const std::vector<float>& data) const {
    // Exploration based on uncertainty and information gain
    float exploration = computeInternalNovelty(data) * 0.7f + 
                        (1.0f - computeHomeostasis(data)) * 0.3f;
    return exploration;
}

} // namespace nlm
