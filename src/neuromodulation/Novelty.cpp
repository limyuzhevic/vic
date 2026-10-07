#include "Novelty.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    float level;
    float decayRate;
    float acquisitionRate;
    std::vector<float> history;
    static constexpr size_t MAX_HISTORY = 100;
    
    Impl() : level(0.0f), decayRate(0.1f), acquisitionRate(1.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(class Brain* brain) {
    // Store brain reference if needed for future integration
    // Currently placeholder implementation
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::detectNovelty(const class Observation& observation, 
                           const class Observation& previousObservation) {
    // Detect novelty by comparing current and previous observations
    // This is a placeholder implementation using simple difference metric
    
    // Get observation data (assuming float array)
    const auto& currentData = observation.getData();
    const auto& previousData = previousObservation.getData();
    
    if (currentData.size() != previousData.size() || currentData.empty()) {
        return;
    }
    
    // Calculate mean squared difference as novelty metric
    float sumSquaredDiff = 0.0f;
    for (size_t i = 0; i < currentData.size(); ++i) {
        float diff = currentData[i] - previousData[i];
        sumSquaredDiff += diff * diff;
    }
    
    float mse = sumSquaredDiff / currentData.size();
    
    // Convert to 0-1 scale using sigmoid function
    float noveltyScore = 1.0f / (1.0f + std::exp(-10.0f * (mse - 0.01f)));
    
    // Add to level with acquisition rate
    pImpl->level += (noveltyScore - pImpl->level) * pImpl->acquisitionRate;
    
    // Store in history
    pImpl->history.push_back(noveltyScore);
    if (pImpl->history.size() > Impl::MAX_HISTORY) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    // Detect novelty from sensory input patterns
    // Calculate pattern difference as novelty measure
    
    if (currentPattern.size() != previousPattern.size() || currentPattern.empty()) {
        return;
    }
    
    // Calculate Euclidean distance as novelty metric
    float sumSquaredDiff = 0.0f;
    for (size_t i = 0; i < currentPattern.size(); ++i) {
        float diff = currentPattern[i] - previousPattern[i];
        sumSquaredDiff += diff * diff;
    }
    
    float euclideanDist = std::sqrt(sumSquaredDiff);
    
    // Normalize and convert to novelty score
    float noveltyScore = std::min(1.0f, euclideanDist * 10.0f);  // Scale factor
    
    // Update novelty level
    pImpl->level += (noveltyScore - pImpl->level) * pImpl->acquisitionRate;
    
    // Store in history
    pImpl->history.push_back(noveltyScore);
    if (pImpl->history.size() > Impl::MAX_HISTORY) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time to simulate fading memory
    float decayFactor = std::exp(-pImpl->decayRate * dt);
    pImpl->level *= decayFactor;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm