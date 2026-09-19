// Neuromodulation/Curiosity.cpp - Implementation of curiosity neuromodulator

#include "Curiosity.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float explorationRate;
    float noveltySensitivity;
    
    // Exploration dynamics
    float explorationBias;
    float goalExploration;
    
    // History tracking
    std::vector<float> recentExperiences;
    size_t maxHistory;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), 
             explorationRate(0.2f), noveltySensitivity(0.8f),
             explorationBias(0.5f), goalExploration(0.3f), maxHistory(10) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity increases plasticity by encouraging exploration of novel actions
    return 0.5f + pImpl->level * 0.5f;
}

void Curiosity::update(TimestepDuration dt) {
    // Decay curiosity over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update recent experiences history
    if (pImpl->recentExperiences.size() > pImpl->maxHistory) {
        pImpl->recentExperiences.erase(pImpl->recentExperiences.begin());
    }
}

void Curiosity::signalNovelty(float novelty) {
    // Curiosity increases with novel experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->noveltySensitivity);
    
    // Record this experience for future reference
    pImpl->recentExperiences.push_back(novelty);
}

void Curiosity::signalExplorationOpportunity(float opportunity) {
    // Curiosity increases when there are opportunities for exploration
    pImpl->level = std::min(pImpl->peak, pImpl->level + opportunity * pImpl->explorationRate);
}

void Curiosity::setExplorationBias(float bias) {
    pImpl->explorationBias = std::clamp(bias, 0.0f, 1.0f);
}

float Curiosity::getExplorationBias() const {
    return pImpl->explorationBias;
}

void Curiosity::configure(const Config& config) {
    if (config.has("curiosity_baseline")) {
        pImpl->baseline = config.get<float>("curiosity_baseline");
    }
    if (config.has("curiosity_peak")) {
        pImpl->peak = config.get<float>("curiosity_peak");
    }
    if (config.has("curiosity_decay_rate")) {
        pImpl->decayRate = config.get<float>("curiosity_decay_rate");
    }
    if (config.has("curiosity_exploration_rate")) {
        pImpl->explorationRate = config.get<float>("curiosity_exploration_rate");
    }
}

} // namespace nlm
