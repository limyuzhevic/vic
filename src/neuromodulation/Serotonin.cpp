#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float mood;
    float impulsivity;
    float errorCorrection;
    float baseline;
    float tonicLevel;
    
    Impl() : level(0.0f), mood(1.0f), impulsivity(1.0f), errorCorrection(1.0f),
             baseline(0.05f), tonicLevel(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    mood_ = 1.0f;
    impulsivity_ = 1.0f;
    errorCorrection_ = 1.0f;
    baseline_ = 0.05f;
}

Serotonin::~Serotonin() = default;

void Serotonin::initialize() {
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived effects
    mood_ = 1.0f + pImpl->level * 0.3f;
    impulsivity_ = 1.0f - pImpl->level * 0.5f;  // Higher serotonin reduces impulsivity
    errorCorrection_ = 1.0f + pImpl->level * 0.4f;
}

void Serotonin::update(double dt) {
    // 5-HT dynamics: tonic baseline with phasic responses
    pImpl->level = tonicLevel - pImpl->level * 0.02f * static_cast<float>(dt);
}

float Serotonin::enhanceMood() const {
    // 5-HT enhances mood and emotional stability
    return mood_;
}

float Serotonin::reduceImpulsivity() const {
    // 5-HT reduces impulsive behavior and increases patience
    return std::max(0.1f, impulsivity_);
}

float Serotonin::correctPredictionErrors() const {
    // 5-HT helps correct prediction errors and maintain homeostasis
    return errorCorrection_;
}

void Serotonin::clear() {
    pImpl->level = 0.0f;
    mood_ = 1.0f;
    impulsivity_ = 1.0f;
    errorCorrection_ = 1.0f;
}

} // namespace nlm
