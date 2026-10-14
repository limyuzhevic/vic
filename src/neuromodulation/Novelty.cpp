#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    // Acetylcholine integration
    float acetylcholineLevel;
    float attentionalGain;
    float encodingStrength;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             acetylcholineLevel(0.0f), attentionalGain(1.0f), encodingStrength(1.0f) {}
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
    // Enhanced novelty detection with acetylcholine integration
    detectNovelty(observation.getData(), previousObservation.getData());
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Get acetylcholine level from brain
    if (pImpl->brain && pImpl->brain->getAcetylcholine()) {
        pImpl->acetylcholineLevel = pImpl->brain->getAcetylcholine()->getLevel();
        pImpl->attentionalGain = 1.0f + pImpl->acetylcholineLevel * 2.0f; // ACh enhances attention to novel stimuli
        pImpl->encodingStrength = 0.5f + pImpl->acetylcholineLevel * 0.5f; // ACh enhances memory encoding
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
    
    // Apply attentional gain from acetylcholine
    float weightedDiff = avgDiff * pImpl->attentionalGain;
    
    // Update novelty level based on difference with ACh modulation
    float baseNovelty = std::min(1.0f, weightedDiff / pImpl->noveltyThreshold);
    pImpl->level = baseNovelty * pImpl->encodingStrength;  // ACh enhances novelty encoding
    
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Signal novelty detection to acetylcholine system
    if (pImpl->brain && pImpl->brain->getAcetylcholine() && pImpl->level > 0.5f) {
        pImpl->brain->getAcetylcholine()->updateNoveltySensitivity(baseNovelty);
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset attentional gain and encoding strength over time
    pImpl->attentionalGain = std::max(1.0f, pImpl->attentionalGain - 0.1f * static_cast<float>(dt));
    pImpl->encodingStrength = std::max(0.5f, pImpl->encodingStrength - 0.05f * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
