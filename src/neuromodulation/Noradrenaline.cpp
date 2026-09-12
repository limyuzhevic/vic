#include "Noradrenaline.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Noradrenaline::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilance;
    float stressResponse;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , arousalLevel(0.5f)
        , vigilance(0.5f)
        , stressResponse(0.0f) {}
};

Noradrenaline::Noradrenaline() : pImpl(new Impl) {}

Noradrenaline::~Noradrenaline() = default;

void Noradrenaline::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Noradrenaline system initialized");
}

const char* Noradrenaline::getName() const {
    return "NE";
}

float Noradrenaline::getLevel() const {
    return pImpl->level;
}

void Noradrenaline::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Noradrenaline::getPlasticityFactor() const {
    return 0.3f + 0.7f * pImpl->level;
}

void Noradrenaline::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Noradrenaline::signalArousal(float intensity, float threat) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    
    pImpl->stressResponse = std::min(1.0f, threat * 0.5f);
}

void Noradrenaline::setVigilance(float vigilance) {
    pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f);
}

float Noradrenaline::getVigilance() const {
    return pImpl->vigilance;
}

float Noradrenaline::getStressResponse() const {
    return pImpl->stressResponse;
}

void Noradrenaline::enhanceSensoryProcessing() {
    if (!pImpl->brain) return;
    
    float enhancement = pImpl->level * 0.3f;
    pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, enhancement);
}

void Noradrenaline::prepareFightOrFlight() {
    if (!pImpl->brain) return;
    
    float mobilization = pImpl->level * 0.5f;
    pImpl->brain->injectCurrentToNeurons(NeuronType::Motor, mobilization);
}

} // namespace nlm
