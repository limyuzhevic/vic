#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialCohesion;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(0.5f)
        , impulsivity(0.3f)
        , socialCohesion(0.4f)
        , decayRate(0.06f) {}
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
    // Serotonin affects mood and social behavior
    // Higher 5-HT improves mood and reduces impulsivity
    float moodMod = 1.0f + pImpl->mood * pImpl->level;
    float impulseMod = 1.0f - pImpl->impulsivity * pImpl->level; // Inverse relationship
    float socialMod = 1.0f + pImpl->socialCohesion * pImpl->level;
    return moodMod * impulseMod * socialMod;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay with spontaneous fluctuations
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small random fluctuations
    static float seed = 123.0f;
    seed = std::fmod(seed * 1.0001f, 1.0f);
    pImpl->level += (seed - 0.5f) * 0.02f * static_cast<float>(dt);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

void Serotonin::setMood(float mood) {
    pImpl->mood = mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = impulsivity;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

void Serotonin::setSocialCohesion(float cohesion) {
    pImpl->socialCohesion = cohesion;
}

} // namespace nlm
