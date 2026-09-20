#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Novelty::Novelty() : pImpl(std::make_unique<Impl>()) {}

Novelty::~Novelty() = default;

Novelty::Novelty(Novelty&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Novelty& Novelty::operator=(Novelty&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Novelty::initialize(Brain* brain) {
    if (pImpl) {
        pImpl->brain = brain;
    }
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
    // Implement novelty detection based on sensory input changes
    // Novelty is the degree to which current sensory input differs from expected
    
    // Extract features from observations
    // In a real implementation, this would use neural representations
    // For now, use simple difference
    
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // Try to get vision data if available
    const auto* visionInput = dynamic_cast<const Vision*>(&observation);
    const auto* prevVisionInput = dynamic_cast<const Vision*>(&previousObservation);
    
    if (visionInput && prevVisionInput) {
        currentPattern = visionInput->getData();
        previousPattern = prevVisionInput->getData();
    } else {
        // Fallback: use internal signals
        currentPattern = observation.getInternal();
        previousPattern = previousObservation.getInternal();
    }
    
    // Detect novelty based on pattern differences
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute pattern difference (L2 distance normalized)
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    if (compareLen == 0) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute Euclidean distance between patterns
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        totalDiff += diff * diff;
    }
    
    // Normalize by pattern size
    float avgDiff = std::sqrt(totalDiff / compareLen);
    
    // Normalize by novelty threshold to get 0-1 level
    // Patterns closer than threshold: low novelty
    // Patterns farther than threshold: high novelty
    pImpl->level = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    
    // Store in history for temporal analysis
    pImpl->history.push_back(pImpl->level);
    
    // Keep history bounded to prevent memory issues
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update last pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    NLM_LOG_DEBUG(std::string("Novelty detected: ") + 
                 std::to_string(pImpl->level) + 
                 std::string(" (diff: ") + std::to_string(avgDiff) + ")");
}

void Novelty::update(TimestepDuration dt) {
    // Implement novelty decay
    // Novelty naturally decays over time as patterns are experienced repeatedly
    
    float timeDelta = static_cast<float>(dt);
    
    // Decay novelty towards zero
    // Novelty is a transient response to unexpected input
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * timeDelta);
    
    // Decay historical novelty levels
    for (auto& historyLevel : pImpl->history) {
        historyLevel = std::max(0.0f, historyLevel - pImpl->decayRate * timeDelta);
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
