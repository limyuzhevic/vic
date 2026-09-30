#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    Brain* brain;
    float level;
    float plasticityFactor;
    float arousal;
    float vigilance;
    float locusCoeruleusInput;
    float decayRate;
    float learningRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , arousal(0.0f)
        , vigilance(0.0f)
        , locusCoeruleusInput(0.0f)
        , decayRate(0.05f)
        , learningRate(0.01f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
}

float Norepinephrine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Apply decay to norepinephrine level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal based on current level
    pImpl->arousal = pImpl->level * 0.8f;
    
    // Update vigilance
    pImpl->vigilance = pImpl->level * 0.6f;
    
    // Update locus coeruleus input
    pImpl->locusCoeruleusInput = pImpl->level * 0.9f;
}

void Norepinephrine::signalArousal(float arousal) {
    // Increase norepinephrine level based on arousal
    pImpl->level += arousal * pImpl->learningRate;
    
    // Update plasticity factor
    pImpl->plasticityFactor = 1.0f + pImpl->arousal * 0.3f;
    
    NLM_LOG_INFO("Norepinephrine: arousal signal, level = " + std::to_string(pImpl->level));
}

void Norepinephrine::signalVigilance(float vigilance) {
    // Increase norepinephrine level based on vigilance demands
    pImpl->level += vigilance * pImpl->learningRate * 1.5f;
    
    // Update vigilance
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + vigilance * 0.1f);
    
    NLM_LOG_INFO("Norepinephrine: vigilance signal, level = " + std::to_string(pImpl->level));
}

void Norepinephrine::signalStress(float stress) {
    // Increase norepinephrine level based on stress
    pImpl->level += stress * pImpl->learningRate * 2.0f;
    
    // Update arousal and vigilance based on stress
    pImpl->arousal = std::min(1.0f, pImpl->arousal + stress * 0.15f);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + stress * 0.1f);
    
    // Increase plasticity factor under stress
    pImpl->plasticityFactor = 1.0f + pImpl->arousal * 0.5f;
    
    NLM_LOG_INFO("Norepinephrine: stress signal, level = " + std::to_string(pImpl->level));
}

} // namespace nlm