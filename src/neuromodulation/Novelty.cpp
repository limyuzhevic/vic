#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include "../core/Utils.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decay_rate;
    std::vector<float> history;
    std::vector<float> last_pattern;
    float novelty_threshold;
    
    Impl() : brain(nullptr), level(0.0f), decay_rate(Constants::NOVELTY_DECAY_MS), 
             novelty_threshold(Constants::NOVELTY_THRESHOLD) {}
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
    // Extract features from observations and compare
    // Simple implementation: just set to a placeholder
    pImpl->level = 1.0f;
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                             const std::vector<float>& previousPattern) {
    Utils::validateArraysNotEmpty(currentPattern, "Novelty detection");
    Utils::validateArraysNotEmpty(previousPattern, "Novelty detection");
    
    // Compute difference between patterns
    float total_diff = Utils::computePatternDifference(currentPattern, previousPattern);
    
    // Normalize by pattern size
    float avg_diff = total_diff / static_cast<float>(std::min(currentPattern.size(), previousPattern.size()));
    
    // Update novelty level based on difference
    pImpl->level = std::min(1.0f, avg_diff / pImpl->novelty_threshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->last_pattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty using exponential decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decay_rate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
