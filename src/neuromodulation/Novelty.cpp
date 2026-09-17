#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Novelty::Impl {
    Brain* brain;
    float noveltyLevel;
    float noveltyThreshold;
    std::vector<float> recentPatterns;
    std::vector<float> patternHistory;
    
    Impl() : brain(nullptr), noveltyLevel(0.0f), noveltyThreshold(0.5f) {}
};

Novelty::Novelty() : pImpl(new Impl) {
    NLM_LOG_INFO("Novelty detector initialized");
}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->noveltyLevel = 0.0f;
    pImpl->recentPatterns.clear();
    pImpl->patternHistory.clear();
}

float Novelty::getLevel() const {
    return pImpl->noveltyLevel;
}

void Novelty::update(TimestepDuration dt) {
    // Simple novelty decay over time
    pImpl->noveltyLevel *= (1.0f - 0.01f * dt);
    
    // Add small random noise for exploration
    static uint64_t seed = 12345;
    static std::mt19937_64 rng(seed++);
    float noise = (static_cast<float>(rng()) / (static_cast<float>(rng.max()) + 1.0f)) * 0.1f - 0.05f;
    pImpl->noveltyLevel += noise;
    pImpl->noveltyLevel = std::max(0.0f, std::min(pImpl->noveltyLevel, 1.0f));
}

bool Novelty::isNovel(const std::vector<float>& pattern) const {
    if (!pImpl->brain) return false;
    
    if (pattern.empty()) return false;
    
    // Check against recent patterns
    for (const auto& recent : pImpl->recentPatterns) {
        if (computeSimilarity(pattern, std::vector<float>(1, recent)) > 0.8f) {
            return false;
        }
    }
    
    // If pattern is different enough, it's novel
    return true;
}

void Novelty::reset() {
    pImpl->noveltyLevel = 0.0f;
    pImpl->recentPatterns.clear();
    pImpl->patternHistory.clear();
}

} // namespace nlm
