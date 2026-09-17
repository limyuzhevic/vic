// Novelty.cpp - Implementation
#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    Brain* brain;
    float noveltyLevel;
    std::vector<float> recentSensoryHistory;
    size_t historySize;
    
    Impl() : brain(nullptr), noveltyLevel(0.0f), historySize(100) {
        recentSensoryHistory.resize(historySize, 0.0f);
    }
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection system initialized");
}

void Novelty::detectNovelty(const SensoryInput& input) {
    if (!pImpl->brain) return;
    
    const auto& newInput = input.getData();
    
    // Calculate difference from recent history
    float avgPrevious = 0.0f;
    for (float val : pImpl->recentSensoryHistory) {
        avgPrevious += val;
    }
    avgPrevious /= pImpl->recentSensoryHistory.size();
    
    // Novelty is based on deviation from average
    float sumDiff = 0.0f;
    for (size_t i = 0; i < newInput.size() && i < pImpl->recentSensoryHistory.size(); ++i) {
        sumDiff += std::abs(newInput[i] - pImpl->recentSensoryHistory[i]);
    }
    
    if (!pImpl->recentSensoryHistory.empty()) {
        float avgDiff = sumDiff / pImpl->recentSensoryHistory.size();
        pImpl->noveltyLevel = std::min(1.0f, avgDiff * 10.0f); // Scale factor
    }
    
    // Store for next comparison
    if (!pImpl->recentSensoryHistory.empty() && !newInput.empty()) {
        size_t idx = pImpl->recentSensoryHistory.size() - 1;
        for (size_t i = 0; i < newInput.size() && i < pImpl->recentSensoryHistory.size(); ++i) {
            pImpl->recentSensoryHistory[i] = newInput[i];
        }
    }
}

void Novelty::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Decay novelty level
    pImpl->noveltyLevel *= 0.9f;
}

float Novelty::getLevel() const {
    return pImpl->noveltyLevel;
}

float Novelty::getPredictionErrorContribution() const {
    // Novelty contributes to prediction error
    return pImpl->noveltyLevel * 0.5f;
}

} // namespace nlm