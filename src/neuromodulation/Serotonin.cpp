#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodWeight;
    float impulsivityWeight;
    float socialWeight;
    float baselineLevel;
    float decayRate;
    float socialQuality;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , moodWeight(0.5f)
        , impulsivityWeight(0.2f)
        , socialWeight(0.3f)
        , baselineLevel(0.1f)
        , decayRate(0.02f)
        , socialQuality(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin level depends on mood and social needs
    // In a full implementation, this would integrate with social and reward systems
    // For now, use baseline level with decay
    pImpl->level = pImpl->baselineLevel;
    
    // Modify based on social quality
    pImpl->level *= (1.0f + pImpl->socialQuality * 0.5f);
    
    // Decay toward baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Serotonin::getMoodFactor() const {
    // Positive mood enhancement when 5-HT is high
    return std::pow(1.0f + pImpl->level * pImpl->moodWeight, 1.5f);
}

float Serotonin::getImpulsivityFactor() const {
    // Reduced impulsivity when 5-HT is high (serotonin reduces impulsivity)
    return std::pow(1.0f - pImpl->level * pImpl->impulsivityWeight, 1.5f);
}

float Serotonin::getSocialFactor() const {
    // Enhanced social behavior when 5-HT is high
    return std::pow(1.0f + pImpl->level * pImpl->socialWeight, 1.3f);
}

void Serotonin::signalSocialReward(float socialQuality) {
    pImpl->socialQuality = socialQuality;
}

void Serotonin::setMoodWeight(float weight) {
    pImpl->moodWeight = weight;
}

void Serotonin::setImpulsivityWeight(float weight) {
    pImpl->impulsivityWeight = weight;
}

void Serotonin::setSocialWeight(float weight) {
    pImpl->socialWeight = weight;
}

void Serotonin::setBaselineLevel(float level) {
    pImpl->baselineLevel = level;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->socialQuality = 0.0f;
}

} // namespace nlm