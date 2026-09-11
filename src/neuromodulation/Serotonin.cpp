#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float releaseRate;
    
    // Behavioral modulation
    float rewardSensitivity;
    float punishmentSensitivity;
    float learningRate;
    
    // Synaptic effects
    float inhibitoryGain;
    float excitatoryGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(1.0f)
        , impulsivity(1.0f)
        , socialBehavior(1.0f)
        , baselineLevel(0.0f)
        , peakLevel(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f)
        , rewardSensitivity(1.0f)
        , punishmentSensitivity(1.0f)
        , learningRate(1.0f)
        , inhibitoryGain(1.0f)
        , excitatoryGain(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (mood/social) system initialized");
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

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates learning and memory
    // High 5-HT reduces plasticity (stabilizes memories)
    // Low 5-HT increases plasticity (promotes learning)
    float basePlasticity = 1.0f;
    
    if (pImpl->level > 0.6f) {
        // High serotonin stabilizes existing connections
        basePlasticity = 0.7f + (pImpl->level - 0.6f) * 0.1f;
    } else {
        // Low serotonin promotes exploration and new learning
        basePlasticity = 0.9f + (1.0f - pImpl->level) * 0.2f;
    }
    
    return basePlasticity;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood and behavioral states
    pImpl->mood = 1.0f + pImpl->level * 1.5f;        // 1.0x to 2.5x mood
    pImpl->impulsivity = 1.0f - pImpl->level * 0.8f; // 0.2x to 1.0x impulsivity
    pImpl->socialBehavior = 1.0f + pImpl->level * 1.2f; // 1.0x to 2.2x social
    
    // 5-HT effects on neural activity
    if (pImpl->brain) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Modulate inhibitory/excitatory balance
                    if (pImpl->inhibitoryGain > 1.0f) {
                        // Enhance inhibition (calming effect)
                        neuron->setRefractoryPeriod(neuron->getState().refractoryPeriod * 0.9f);
                    }
                    
                    // Adjust response thresholds
                    if (pImpl->level > 0.5f) {
                        // High serotonin increases threshold (reduced impulsivity)
                        neuron->setThreshold(neuron->getState().threshold * 1.1f);
                    }
                }
            }
        }
    }
}

void Serotonin::enhanceMood(float moodBoost) {
    // Positive social feedback or reward increases serotonin
    setLevel(std::min(pImpl->peakLevel, getLevel() + moodBoost * pImpl->releaseRate * 0.7f));
    pImpl->rewardSensitivity = 1.0f + getLevel() * 1.5f;
    pImpl->punishmentSensitivity = 1.5f - getLevel() * 0.8f; // Less sensitive to punishment when happy
    
    NLM_LOG_INFO("Serotonin enhanced mood to level " + std::to_string(getLevel()));
}

void Serotonin::reduceImpulsivity(float controlSignal) {
    // Cognitive control reduces impulsivity
    setLevel(std::min(pImpl->peakLevel, getLevel() + controlSignal * 0.3f));
    pImpl->impulsivity = 1.0f - getLevel() * 0.8f;
    
    NLM_LOG_INFO("Serotonin reduced impulsivity to level " + std::to_string(pImpl->impulsivity));
}

void Serotonin::promoteSocialBehavior(float socialBond) {
    // Social interaction increases serotonin
    setLevel(std::min(pImpl->peakLevel, getLevel() + socialBond * pImpl->releaseRate * 0.6f));
    pImpl->socialBehavior = 1.0f + getLevel() * 1.2f;
    
    NLM_LOG_INFO("Serotonin promoted social behavior to level " + std::to_string(pImpl->socialBehavior));
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->mood = 1.0f;
    pImpl->impulsivity = 1.0f;
    pImpl->socialBehavior = 1.0f;
    pImpl->rewardSensitivity = 1.0f;
    pImpl->punishmentSensitivity = 1.0f;
    pImpl->learningRate = 1.0f;
    pImpl->inhibitoryGain = 1.0f;
    pImpl->excitatoryGain = 1.0f;
}

} // namespace nlm