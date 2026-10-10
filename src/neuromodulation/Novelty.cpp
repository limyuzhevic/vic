#include "Novelty.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Novelty::Impl {
    float level;
    float peak;
    float decayRate;
    float detectionThreshold;
    float surpriseMagnitude;
    
    Impl() : level(0.0f), peak(1.0f), decayRate(0.05f), 
             detectionThreshold(0.3f), surpriseMagnitude(0.5f) {
        history.clear();
    }
    
    std::vector<float> history;
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->history.clear();
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        setLevel(0.0f);
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
    setLevel(std::min(peak, avgDiff / pImpl->detectionThreshold) * pImpl->surpriseMagnitude);
    pImpl->history.push_back(getLevel());
}

void Novelty::update(float dt) {
    setLevel(std::max(0.0f, getLevel() - pImpl->decayRate * dt));
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
