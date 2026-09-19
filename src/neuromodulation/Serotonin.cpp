#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodSensitivity;
    float flexibilitySensitivity;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , moodSensitivity(1.0f)
        , flexibilitySensitivity(0.5f)
        , decayRate(0.05f) {}
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
    // 5-HT modulates plasticity to promote behavioral flexibility
    return 1.0f + 0.2f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin increases with mood changes and behavioral flexibility demands
    float mood = 0.0f;  // TODO: Get actual mood signal
    float behavioralFlexibility = 0.0f;  // TODO: Get actual flexibility demand
    
    // Serotonin increases with mood changes and behavioral flexibility demands
    float target = pImpl->moodSensitivity * mood + 
                   pImpl->flexibilitySensitivity * behavioralFlexibility;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getMoodModulation() const {
    return pImpl->level * pImpl->moodSensitivity;
}

float Serotonin::getBehavioralFlexibilityModulation() const {
    // 5-HT allows switching strategies and reduces impulsivity
    return 1.0f + 0.3f * pImpl->level;
}

float Serotonin::getMemoryConsolidationModulation() const {
    // 5-HT promotes forgetting of irrelevant information
    return 1.0f - 0.4f * pImpl->level;
}

void Serotonin::setMoodSensitivity(float sensitivity) {
    pImpl->moodSensitivity = sensitivity;
}

void Serotonin::setFlexibilitySensitivity(float sensitivity) {
    pImpl->flexibilitySensitivity = sensitivity;
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm