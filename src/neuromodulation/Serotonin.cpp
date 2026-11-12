#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulseControl;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.12f)
        , moodLevel(0.0f)
        , impulseControl(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and social context
    // Higher serotonin enhances stable learning and reduces impulsivity
    // Low serotonin may lead to faster but less stable learning
    float level = pImpl->level;
    float moodFactor = pImpl->moodLevel * 0.5f + 0.5f;
    return 0.4f + 0.6f * level * moodFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Real serotonin dynamics with realistic time constants
    static constexpr float timeConstant = 150.0f;  // 150 ms time constant
    float decayFactor = 1.0f - std::exp(-pImpl->decayRate * static_cast<float>(dt) / timeConstant);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

void Serotonin::signalSocialFeedback(float socialValue) {
    // Social feedback signal that modulates serotonin levels
    float socialSignal = std::clamp(socialValue, -1.0f, 1.0f);
    
    // Update serotonin with social feedback
    setLevel(pImpl->level + socialSignal * 0.2f);
    
    // Update mood and impulse control based on social feedback
    if (socialSignal > 0.0f) {
        pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + socialSignal * 0.3f);
        pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl - socialSignal * 0.1f);
    } else {
        pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel + socialSignal * 0.2f);
        pImpl->impulseControl = std::max(0.1f, pImpl->impulseControl - socialSignal * 0.05f);
    }
    
    NLM_LOG_INFO("Serotonin-social feedback signal: " + std::to_string(socialValue));
}

float Serotonin::get5HTLevel() const {
    return getLevel();
}

float Serotonin::get5HTBaseline() const {
    return pImpl->baseline;
}

void Serotonin::set5HTBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

} // namespace nlm
