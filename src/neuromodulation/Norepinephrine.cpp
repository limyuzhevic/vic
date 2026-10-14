#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // NE-specific state
    float arousal;              // Arousal and vigilance
    float alertness;             // Alertness and focus
    float stressLevel;           // Stress response
    float memoryConsolidation;   // Memory consolidation
    float adaptation;            // Adaptive processing
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(2.0f)              // NE can have strong effects
        , decayRate(0.03f)
        , releaseRate(1.5f)
        , arousal(1.0f)
        , alertness(1.0f)
        , stressLevel(0.0f)
        , memoryConsolidation(1.0f)
        , adaptation(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

void Norepinephrine::signalArousal(float level) {
    // Increase arousal and vigilance
    pImpl->arousal = std::min(pImpl->peak, pImpl->arousal + level);
}

void Norepinephrine::signalAlertness(float level) {
    // Enhance alertness and focused attention
    pImpl->alertness = std::clamp(level, 0.5f, 2.0f);
}

void Norepinephrine::signalStress(float intensity) {
    // Initiate stress response
    pImpl->stressLevel = std::clamp(intensity, 0.0f, 1.0f);
}

void Norepinephrine::signalMemoryConsolidation(float factor) {
    // Enhance memory consolidation during alert states
    pImpl->memoryConsolidation = std::clamp(factor, 0.5f, 2.0f);
}

void Norepinephrine::signalAdaptation(float rate) {
    // Increase adaptive processing and flexibility
    pImpl->adaptation = std::clamp(rate, 0.5f, 2.0f);
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal based on NE level
    pImpl->arousal = 1.0f + pImpl->level * 0.8f;
    // Update alertness based on NE level
    pImpl->alertness = std::clamp(1.0f + pImpl->level * 0.5f, 0.5f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for learning and stress-based adaptation
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay NE level
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Get current arousal level
float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

// Get current alertness level
float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

// Get current stress level
float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

// Get memory consolidation enhancement
float Norepinephrine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

// Get adaptive processing rate
float Norepinephrine::getAdaptation() const {
    return pImpl->adaptation;
}

} // namespace nlm