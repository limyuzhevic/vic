#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Neuromodulator implementation - base class with empty implementations
void Neuromodulator::initialize(Brain* brain) {}
void Neuromodulator::reset() {}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionGain;
    float memoryConsolidationFactor;
    float learningRateModulation;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float focusStrength;
    
    Impl() : brain(nullptr), level(0.0f), attentionGain(1.0f), memoryConsolidationFactor(1.0f),
             learningRateModulation(1.0f), baselineLevel(0.0f), peakLevel(1.0f), decayRate(0.1f),
             releaseRate(1.0f), attentionBoost(1.5f), focusStrength(1.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine system created");
}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (attention) system initialized");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryConsolidationFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->attentionGain = 1.0f;
    pImpl->memoryConsolidationFactor = 1.0f;
    pImpl->learningRateModulation = 1.0f;
    pImpl->focusStrength = 1.0f;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalLevel;
    float vigilance;
    float locusCoeruleusGain;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float releaseRate;
    float signalDetection;
    float phasicResponse;
    float tonicResponse;
    float ltpBoost;
    float ltdBoost;
    
    Impl() : brain(nullptr), level(0.0f), arousalLevel(1.0f), vigilance(1.0f), locusCoeruleusGain(1.0f),
             baselineLevel(0.0f), peakLevel(1.0f), decayRate(0.05f), releaseRate(1.0f),
             signalDetection(1.0f), phasicResponse(1.0f), tonicResponse(1.0f),
             ltpBoost(1.0f), ltdBoost(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system created");
}

Norepinephrine::~Norepinephrine() {
    delete pImpl;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (arousal/vigilance) system initialized");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    float plasticity = 1.0f;
    if (pImpl->level > 0.5f) {
        plasticity = 1.2f + (pImpl->level - 0.5f) * 0.5f;
    } else if (pImpl->level < 0.3f) {
        plasticity = 0.8f - (0.3f - pImpl->level) * 0.4f;
    }
    return plasticity;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->arousalLevel = 1.0f;
    pImpl->vigilance = 1.0f;
    pImpl->locusCoeruleusGain = 1.0f;
    pImpl->signalDetection = 1.0f;
    pImpl->phasicResponse = 1.0f;
    pImpl->tonicResponse = 1.0f;
    pImpl->ltpBoost = 1.0f;
    pImpl->ltdBoost = 1.0f;
}

// Serotonin implementation
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
    float rewardSensitivity;
    float punishmentSensitivity;
    float learningRate;
    float inhibitoryGain;
    float excitatoryGain;
    
    Impl() : brain(nullptr), level(0.0f), mood(1.0f), impulsivity(1.0f), socialBehavior(1.0f),
             baselineLevel(0.0f), peakLevel(1.0f), decayRate(0.08f), releaseRate(1.0f),
             rewardSensitivity(1.0f), punishmentSensitivity(1.0f), learningRate(1.0f),
             inhibitoryGain(1.0f), excitatoryGain(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system created");
}

Serotonin::~Serotonin() {
    delete pImpl;
}

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
    float basePlasticity = 1.0f;
    
    if (pImpl->level > 0.6f) {
        basePlasticity = 0.7f + (pImpl->level - 0.6f) * 0.1f;
    } else {
        basePlasticity = 0.9f + (1.0f - pImpl->level) * 0.2f;
    }
    
    return basePlasticity;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
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

// Dopamine implementation
struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system created");
}

Dopamine::~Dopamine() {
    delete pImpl;
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine (reward prediction error) system initialized");
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    float plasticity = 0.5f + 0.5f * pImpl->level;
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    if (pImpl->brain && pImpl->level > 0.0f) {
        float excitabilityMod = pImpl->level * 0.5f;
        if (excitabilityMod > 0.0f) {
            for (auto& region : pImpl->brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    NLM_LOG_INFO("Dopamine reward signal: level=" + std::to_string(pImpl->level) + " reward=" + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    NLM_LOG_INFO("Dopamine prediction error signal: level=" + std::to_string(pImpl->level) + " error=" + std::to_string(error));
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm