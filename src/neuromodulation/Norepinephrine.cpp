#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             arousal(1.0f), vigilance(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal and vigilance based on NE level
    pImpl->arousal = std::max(0.5f, 0.5f + pImpl->level);
    pImpl->vigilance = std::max(0.5f, 0.5f + pImpl->level * 0.8f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE affects arousal and vigilance
    // Higher NE increases alertness and focus
    float factor = 1.0f + pImpl->level * 0.3f;
    return std::clamp(factor, 0.7f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::enhanceArousal(float strength) {
    setLevel(std::max(getLevel(), strength * 0.7f));
    NLM_LOG_INFO("Norepinephrine enhanced arousal: level=" + std::to_string(getLevel()) + ", strength=" + std::to_string(strength));
}

void Norepinephrine::increaseVigilance(float strength) {
    setLevel(std::max(getLevel(), strength));
    NLM_LOG_INFO("Norepinephrine increased vigilance: level=" + std::to_string(getLevel()) + ", strength=" + std::to_string(strength));
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

} // namespace nlm