#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float socialWeight;
    float bondingHistory;
    float socialContext;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , socialWeight(1.0f), bondingHistory(0.0f), socialContext(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system initialized");
}

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
    // Serotonin modulates plasticity - higher levels reduce impulsivity and increase stability
    // But also enhance social learning
    float basePlasticity = 0.5f;
    float levelFactor = pImpl->level * 0.3f;
    float socialFactor = pImpl->socialWeight * 0.2f;
    float bondingFactor = pImpl->bondingHistory * 0.1f;
    
    // Reduce overall plasticity with higher serotonin (stability)
    float result = std::max(0.2f, basePlasticity - levelFactor + socialFactor + bondingFactor);
    NLM_LOG_DEBUG("Serotonin plasticity factor: " + std::to_string(result));
    return result;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Social context updates
    pImpl->socialContext = std::max(0.0f, pImpl->socialContext - 0.01f * static_cast<float>(dt));
    
    NLM_LOG_DEBUG("Serotonin update: level=" + std::to_string(pImpl->level) + ", context=" + std::to_string(pImpl->socialContext));
}

void Serotonin::signalSocialReward(float reward) {
    // Social reward signaling (positive social interactions)
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.5f);
    pImpl->bondingHistory = std::min(1.0f, pImpl->bondingHistory + reward * 0.1f);
    NLM_LOG_INFO("Serotonin social reward: " + std::to_string(reward));
}

void Serotonin::signalSocialBonding(float bonding) {
    // Social bonding signals (form attachment)
    pImpl->level = std::min(pImpl->peak, pImpl->level + bonding * pImpl->releaseRate * 0.8f);
    pImpl->socialWeight = std::min(2.0f, pImpl->socialWeight + bonding * 0.05f);
    NLM_LOG_INFO("Serotonin bonding signal: " + std::to_string(bonding));
}

void Serotonin::updateSocialContext(float context) {
    // Update social context (current social environment)
    pImpl->socialContext = context;
    // High social context can modulate baseline
    if (context > 0.7f) {
        pImpl->baseline = std::min(1.0f, pImpl->baseline + 0.05f);
    }
}

} // namespace nlm