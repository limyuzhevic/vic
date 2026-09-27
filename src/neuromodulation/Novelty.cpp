#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float baseline;        // Baseline novelty level
    float peak;           // Maximum achievable novelty
    float adaptationRate; // Rate of adaptation to new patterns
    float memoryDecayRate; // Rate of memory decay for novelty
    float sensitivity;    // Sensitivity to feature changes
    float forgetThreshold; // Threshold below which novelty is forgotten
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), baseline(0.05f), 
             peak(1.0f), adaptationRate(0.15f), memoryDecayRate(0.05f),
             sensitivity(1.2f), forgetThreshold(0.1f), noveltyThreshold(0.3f) {}
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
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations using their data vector
    // Compute novelty based on feature differences
    const std::vector<float>& currentData = observation.getData();
    const std::vector<float>& previousData = previousObservation.getData();
    
    if (currentData.empty() || previousData.empty()) {
        pImpl->level = 0.0f;
        pImpl->history.push_back(pImpl->level);
        return;
    }
    
    // Compute feature-based novelty
    float totalFeatureDiff = 0.0f;
    size_t compareLen = std::min(currentData.size(), previousData.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float featureDiff = std::abs(currentData[i] - previousData[i]);
        totalFeatureDiff += featureDiff;
    }
    
    // Normalize by feature count for robustness
    float avgFeatureDiff = totalFeatureDiff / compareLen;
    
    // Apply novelty detection algorithm with biological parameters
    // Novelty increases with significant feature changes but saturates
    float noveltySignal = std::min(1.0f, avgFeatureDiff / pImpl->noveltyThreshold);
    
    // Apply response curve similar to neural adaptation
    // Strong responses trigger proportionally stronger novelty signals
    pImpl->level = std::pow(noveltySignal, 0.8f) * (0.9f + noveltySignal * 0.2f);
    
    // Store current observation pattern for next comparison
    pImpl->lastPattern = currentData;
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference
    pImpl->level = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Apply multiple decay mechanisms:
    // 1. Primary decay over time
    // 2. Memory decay (novelty fades from memory)
    // 3. Adaptation towards baseline
    
    // Primary decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Memory decay - novel experiences fade from immediate awareness
    // This simulates the temporal dynamics of working memory for novel events
    float memoryDecay = pImpl->memoryDecayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - memoryDecay);
    
    // Adaptation towards baseline - homeostatic regulation
    // Prevents novelty signal from drifting to extreme values
    // Adaptation strength depends on distance from baseline
    float distanceFromBaseline = std::max(0.0f, pImpl->level - pImpl->baseline);
    float adaptation = pImpl->adaptationRate * static_cast<float>(dt) * distanceFromBaseline;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - adaptation);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
