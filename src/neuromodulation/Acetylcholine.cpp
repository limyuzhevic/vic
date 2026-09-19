// Neuromodulation/Acetylcholine.cpp - Implementation of acetylcholine neuromodulator

#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity and memory consolidation
    return 0.5f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Acetylcholine increases with focused attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate);
}

void Acetylcholine::signalLearning(float learningSignal) {
    // Acetylcholine reinforces rewarding learning experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + learningSignal * pImpl->learningRate);
}

void Acetylcholine::configure(const Config& config) {
    if (config.has("acetylcholine_baseline")) {
        pImpl->baseline = config.get<float>("acetylcholine_baseline");
    }
    if (config.has("acetylcholine_peak")) {
        pImpl->peak = config.get<float>("acetylcholine_peak");
    }
    if (config.has("acetylcholine_decay_rate")) {
        pImpl->decayRate = config.get<float>("acetylcholine_decay_rate");
    }
    if (config.has("acetylcholine_release_rate")) {
        pImpl->releaseRate = config.get<float>("acetylcholine_release_rate");
    }
    if (config.has("acetylcholine_learning_rate")) {
        pImpl->learningRate = config.get<float>("acetylcholine_learning_rate");
    }
}

} // namespace nlm
