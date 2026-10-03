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
    float stressLevel;
    float alertness;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.05f), releaseRate(1.0f), stressLevel(0.0f), alertness(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE increases arousal and enhances encoding
    return 1.0f + pImpl->level * 0.8f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stressIntensity, float surprise) {
    float levelIncrease = stressIntensity * 0.6f + surprise * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + levelIncrease * pImpl->releaseRate);
    
    NLM_LOG_INFO("NE signaling stress: level increased to " + std::to_string(pImpl->level));
}

void Norepinephrine::enhanceSensoryProcessing(class NeuralPopulation* sensoryPopulation) {
    if (!pImpl->brain || !sensoryPopulation) return;
    
    for (class Neuron* neuron : sensoryPopulation->getNeurons()) {
        float current = neuron->getMembraneCurrent();
        neuron->injectCurrent(current * 0.3f * pImpl->level);
    }
}

void Norepinephrine::promoteMemoryConsolidation() {
    NLM_LOG_INFO("NE promoting memory consolidation");
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
    pImpl->stressLevel = 0.0f;
    pImpl->alertness = 0.0f;
}

} // namespace nlm