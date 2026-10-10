#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float vigilance;
    float workingMemoryBoost;
    float baseline;
    float tonicRelease;
    
    Impl() : level(0.0f), arousal(1.0f), vigilance(1.0f), workingMemoryBoost(1.0f),
             baseline(0.05f), tonicRelease(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    arousal_ = 1.0f;
    vigilance_ = 1.0f;
    workingMemoryBoost_ = 1.0f;
    baseline_ = 0.05f;
}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize() {
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived effects
    arousal_ = 1.0f + pImpl->level * 0.3f;
    vigilance_ = 1.0f + pImpl->level * 0.4f;
    workingMemoryBoost_ = 1.0f + pImpl->level * 0.2f;
}

void Norepinephrine::update(double dt) {
    // NE dynamics: phasic responses to novel stimuli, tonic background
    pImpl->level = std::max(baseline_, pImpl->level - pImpl->level * 0.03f * static_cast<float>(dt));
}

float Norepinephrine::enhanceArousal() const {
    // NE increases physiological arousal
    return arousal_;
}

float Norepinephrine::increaseVigilance() const {
    // NE increases vigilance and alertness
    return vigilance_;
}

float Norepinephrine::boostWorkingMemory() const {
    // NE enhances working memory capacity
    return workingMemoryBoost_;
}

void Norepinephrine::clear() {
    pImpl->level = 0.0f;
    arousal_ = 1.0f;
    vigilance_ = 1.0f;
    workingMemoryBoost_ = 1.0f;
}

} // namespace nlm
