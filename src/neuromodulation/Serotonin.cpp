#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float socialCohesion;
    float safetyLevel;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.08f), releaseRate(1.0f), socialCohesion(0.0f), safetyLevel(0.0f) {}
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

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT promotes calm, focused behavior
    return 1.0f + pImpl->level * 0.8f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialReward(float socialValue, float safety) {
    float levelIncrease = socialValue * 0.5f + safety * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + levelIncrease * pImpl->releaseRate);
    
    NLM_LOG_INFO("5-HT signaling social reward: level increased to " + std::to_string(pImpl->level));
}

void Serotonin::reduceAnxiety(float anxietyLevel) {
    if (anxietyLevel > 0.3f) {
        float anxietyReduction = std::min(0.5f, anxietyLevel * 0.4f);
        NLM_LOG_INFO("5-HT reducing anxiety by " + std::to_string(anxietyReduction * 100) + "%");
    }
}

void Serotonin::promoteCalmFocus(class NeuralPopulation* prefrontalPopulation) {
    if (!pImpl->brain || !prefrontalPopulation) return;
    
    for (class Neuron* neuron : prefrontalPopulation->getNeurons()) {
        float current = neuron->getMembraneCurrent();
        neuron->injectCurrent(current * 0.2f * pImpl->level);
    }
    
    NLM_LOG_INFO("5-HT promoting calm focus in prefrontal cortex");
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
    pImpl->socialCohesion = 0.0f;
    pImpl->safetyLevel = 0.0f;
}

} // namespace nlm