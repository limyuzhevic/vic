#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    Brain* brain;
    float level;
    float plasticityFactor;
    float mood;
    float impulsivity;
    float socialBehavior;
    float rapheNucleiInput;
    float decayRate;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , mood(0.0f)
        , impulsivity(0.0f)
        , socialBehavior(0.0f)
        , rapheNucleiInput(0.0f)
        , decayRate(0.05f)
        , learningRate(0.01f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

const char* Serotonin::getName() const {
    return "Serotonin (5-HT)";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
}

float Serotonin::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Apply decay to serotonin level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood based on current level
    pImpl->mood = pImpl->level * 0.7f;
    
    // Update impulsivity
    pImpl->impulsivity = pImpl->level * 0.5f;
    
    // Update social behavior
    pImpl->socialBehavior = pImpl->level * 0.9f;
    
    // Update raphe nuclei input
    pImpl->rapheNucleiInput = pImpl->level * 0.8f;
}

void Serotonin::signalMood(float mood) {
    // Increase serotonin level based on mood
    pImpl->level += mood * pImpl->learningRate;
    
    // Update plasticity factor
    pImpl->plasticityFactor = 1.0f + pImpl->mood * 0.4f;
    
    NLM_LOG_INFO("Serotonin: mood signal, level = " + std::to_string(pImpl->level));
}

void Serotonin::signalImpulsivity(float impulsivity) {
    // Increase serotonin level based on impulsivity
    pImpl->level += impulsivity * pImpl->learningRate * 1.2f;
    
    // Update impulsivity
    pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + impulsivity * 0.1f);
    
    NLM_LOG_INFO("Serotonin: impulsivity signal, level = " + std::to_string(pImpl->level));
}

void Serotonin::signalSocialBehavior(float socialBehavior) {
    // Increase serotonin level based on social behavior
    pImpl->level += socialBehavior * pImpl->learningRate * 1.5f;
    
    // Update social behavior
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialBehavior * 0.1f);
    
    NLM_LOG_INFO("Serotonin: social behavior signal, level = " + std::to_string(pImpl->level));
}

void Serotonin::signal anxiogenic(float anxiogenic) {
    // Increase serotonin level based on anxiogenic signals
    pImpl->level += anxiogenic * pImpl->learningRate * 1.8f;
    
    // Update mood and social behavior based on anxiety
    pImpl->mood = std::max(0.0f, pImpl->mood - anxiogenic * 0.2f);
    pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - anxiogenic * 0.1f);
    
    // Decrease plasticity under anxiety
    pImpl->plasticityFactor = 1.0f - anxiogenic * 0.3f;
    
    NLM_LOG_INFO("Serotonin: anxiogenic signal, level = " + std::to_string(pImpl->level));
}

} // namespace nlm