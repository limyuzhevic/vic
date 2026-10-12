#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Neuromodulator::update(TimestepDuration dt) {
    // Base neuromodulator update (default behavior)
    // Decrease level over time
    for (auto& n : neuromodulators_) {
        n.second->update(dt);
    }
}

// Update acetylcholine level
void Neuromodulator::updateAcetylcholine(float level) {
    for (auto& n : neuromodulators_) {
        if (n.first == "ACh") {
            n.second->setLevel(level);
            break;
        }
    }
}

// Update norepinephrine level  
void Neuromodulator::updateNorepinephrine(float level) {
    for (auto& n : neuromodulators_) {
        if (n.first == "NE") {
            n.second->setLevel(level);
            break;
        }
    }
}

// Update serotonin level
void Neuromodulator::updateSerotonin(float level) {
    for (auto& n : neuromodulators_) {
        if (n.first == "5-HT") {
            n.second->setLevel(level);
            break;
        }
    }
}

// Get current neuromodulator levels
float Neuromodulator::getAcetylcholine() const {
    for (const auto& n : neuromodulators_) {
        if (n.first == "ACh") {
            return n.second->getLevel();
        }
    }
    return 0.0f;
}

float Neuromodulator::getNorepinephrine() const {
    for (const auto& n : neuromodulators_) {
        if (n.first == "NE") {
            return n.second->getLevel();
        }
    }
    return 0.0f;
}

float Neuromodulator::getSerotonin() const {
    for (const auto& n : neuromodulators_) {
        if (n.first == "5-HT") {
            return n.second->getLevel();
        }
    }
    return 0.0f;
}

// Get all neuromodulator levels as a map
std::unordered_map<std::string, float> Neuromodulator::getAllLevels() const {
    std::unordered_map<std::string, float> levels;
    for (const auto& n : neuromodulators_) {
        levels[n.first] = n.second->getLevel();
    }
    return levels;
}

// Update neuromodulators for cognitive functions
void Neuromodulator::updateForAttention(float novelty, float predictionError) {
    // ACh: Increases with novelty and attention demand
    float achLevel = 0.3f + 0.7f * novelty;
    updateAcetylcholine(achLevel);
    
    // NE: Increases with prediction error and arousal
    float neLevel = 0.2f + 0.8f * predictionError;
    updateNorepinephrine(neLevel);
    
    // 5-HT: Decreases with prediction error (mood modulation)
    float serotoninLevel = 1.0f - 0.5f * std::abs(predictionError);
    updateSerotonin(serotoninLevel);
}

// Update neuromodulators for memory consolidation
void Neuromodulator::updateForMemoryConsolidation(float reward) {
    // ACh: Important for memory encoding and consolidation
    float achLevel = 0.5f + 0.5f * reward;
    updateAcetylcholine(achLevel);
    
    // NE: Modulates memory strength and emotional salience
    float neLevel = 0.3f + 0.4f * reward;
    updateNorepinephrine(neLevel);
    
    // 5-HT: Modulates memory consolidation and social aspects
    float serotoninLevel = 0.7f;
    updateSerotonin(serotoninLevel);
}
