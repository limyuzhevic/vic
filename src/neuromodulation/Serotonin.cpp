#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float socialBonding;
    float impulsivity;
    float regulatoryGain;
    float decayRate;
    float baseline;
    float peak;
    float satiationRate;
    float tonicLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(0.0f)
        , socialBonding(0.0f)
        , impulsivity(0.0f)
        , regulatoryGain(1.0f)
        , decayRate(0.12f)
        , baseline(0.1f)
        , peak(2.5f)
        , satiationRate(0.02f)
        , tonicLevel(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Calculate mood based on level
    pImpl->mood = 0.5f * pImpl->level - 0.25f;  // Mood peaks at moderate levels
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for behavioral flexibility
    // Moderate levels enhance learning, extreme levels suppress it
    float moodMod = std::max(0.0f, 1.0f - std::abs(pImpl->mood - 0.5f) * 2.0f);
    float socialMod = 1.0f + 0.5f * pImpl->socialBonding;
    return 0.4f + 0.6f * moodMod * socialMod;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayFactor = std::pow(1.0f - pImpl->decayRate, static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Apply regulatory effects
    if (pImpl->brain && pImpl->regulatoryGain > 0.0f) {
        // Modulate firing rates based on level
        float regulation = pImpl->level * pImpl->regulatoryGain * 0.3f;
        
        // Apply to modulatory neurons for behavioral regulation
        pImpl->brain->injectCurrentToNeurons(NeuronType::Modulatory, regulation * 1.5f);
        
        // Apply to internal neurons for emotional processing
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, regulation * 0.8f);
    }
    
    // Calculate satiation
    if (pImpl->level > 0.5f) {
        // Satiation accumulates with high serotonin
        float satiation = pImpl->satiationRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - satiation);
        pImpl->mood = std::max(0.0f, pImpl->mood - satiation * 0.5f);
    }
}

void Serotonin::signalPositiveSocialInteraction(float strength, float duration) {
    // Enhance social bonding and mood
    pImpl->socialBonding = std::min(1.0f, pImpl->socialBonding + strength);
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - strength * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 1.5f);
}

void Serotonin::signalNegativeSocialExperience(float strength, float duration) {
    // Reduce social bonding and increase impulsivity
    pImpl->socialBonding = std::max(0.0f, pImpl->socialBonding - strength);
    pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + strength * 0.5f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - strength * 1.5f);
}

void Serotonin::resetSocialState() {
    // Gradually return social bonding and mood to baseline
    pImpl->socialBonding = std::max(0.0f, pImpl->socialBonding - 0.05f);
    pImpl->impulsivity = std::max(0.5f, pImpl->impulsivity - 0.05f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

bool Serotonin::isSatiated() const {
    return pImpl->level < 0.3f;
}

} // namespace nlm