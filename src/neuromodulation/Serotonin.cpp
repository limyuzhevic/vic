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
    float moodEffect;
    float learningEffect;
    float attentionEffect;
    float avoidanceLearningEffect;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f), moodEffect(0.8f),
             learningEffect(0.7f), attentionEffect(0.6f), avoidanceLearningEffect(0.9f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (5-HT) system initialized - mood, learning, sleep, attention");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply neuromodulatory effects
    applyEffects(static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Positive mood boosts serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalLearning(float learningSignal) {
    // Learning events boost serotonin for memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + learningSignal * pImpl->releaseRate * 1.0f);
}

void Serotonin::signalAvoidance(float avoidanceStrength) {
    // Avoidance learning boosts serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + avoidanceStrength * pImpl->releaseRate * 0.7f);
}

void Serotonin::applyEffects(float dt) {
    if (!pImpl->brain || pImpl->level <= 0.0f) return;
    
    float effect = pImpl->level * pImpl->moodEffect;
    
    // Mood enhancement - affects neural excitability
    if (effect > 0.0f) {
        // Enhance inhibitory control, reduce impulsive firing
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Serotonin enhances inhibitory tone
                    float inhibitoryEffect = effect * 0.3f;
                    neuron->injectCurrent(-inhibitoryEffect);  // Inhibition
                }
            }
        }
    }
    
    // Learning and memory consolidation
    if (pImpl->level > 0.5f) {
        effect = (pImpl->level - 0.5f) * 2.0f;
        // Enhance synaptic plasticity for memory consolidation
        if (pImpl->brain->getSTDP()) {
            float currentLTP = pImpl->brain->getSTDP()->getLTPWeight();
            pImpl->brain->getSTDP()->setLTPWeight(currentLTP * (1.0f + effect * 0.5f));
        }
    }
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin enhances learning rate and memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

float Serotonin::getAttentionEffect() const {
    return pImpl->level * pImpl->attentionEffect;
}

float Serotonin::getAvoidanceLearningEffect() const {
    return pImpl->level * pImpl->avoidanceLearningEffect;
}

} // namespace nlm