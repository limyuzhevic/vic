#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float vigilance;
    float baseline;
    float decayRate;
    float releaseRate;
    float taskSalience;
    
    Impl() : level(0.0f), arousal(0.0f), vigilance(0.0f), baseline(0.02f), decayRate(0.8f), releaseRate(3.0f), taskSalience(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity through arousal and novelty detection
    // Higher arousal increases plasticity for novel stimuli
    float arousalEffect = pImpl->arousal * 0.7f;  // Max 70% enhancement
    float noveltyEffect = pImpl->taskSalience * 0.5f;  // Additional novelty boost
    
    float satArousal = arousalEffect / (1.0f + arousalEffect);
    float satNovelty = noveltyEffect / (1.0f + noveltyEffect);
    
    float basePlasticity = 0.25f;
    float plasticityFactor = basePlasticity * (1.0f + satArousal + satNovelty * 0.5f);
    
    return std::clamp(plasticityFactor, 0.25f, 1.2f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics based on Sara & Bouret (2017) LLC model
    float deltaT = static_cast<float>(dt) * 0.001f;
    
    // Decay towards baseline (longer time constant than ACh)
    float decayFactor = std::exp(-pImpl->decayRate * deltaT);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Update vigilance and arousal components
    pImpl->vigilance = std::max(0.0f, pImpl->vigilance - 0.05f * deltaT);
    pImpl->arousal = std::max(0.0f, pImpl->arousal - 0.08f * deltaT);
    
    // Ensure physiological bounds
    pImpl->level = std::max(0.0f, std::min(pImpl->level, 1.0f));
    pImpl->vigilance = std::max(0.0f, std::min(pImpl->vigilance, 1.0f));
    pImpl->arousal = std::max(0.0f, std::min(pImpl->arousal, 1.0f));
}

void Norepinephrine::signalTaskSalience(float salience) {
    // NE signals task-relevant information and promotes cognitive flexibility
    float clampedSalience = std::clamp(salience, 0.0f, 1.0f);
    pImpl->taskSalience = clampedSalience;
    
    // Increase arousal and vigilance
    pImpl->arousal = std::min(1.0f, pImpl->arousal + clampedSalience * pImpl->releaseRate * 3.0f);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + clampedSalience * pImpl->releaseRate * 2.0f);
    
    // Transient NE burst response
    pImpl->level = std::min(1.0f, pImpl->level + clampedSalience * pImpl->releaseRate * 4.0f);
}

void Norepinephrine::signalNoveltySurprise(float surprise) {
    // NE responds to unexpected stimuli and promotes exploration
    float clampedSurprise = std::clamp(surprise, 0.0f, 1.0f);
    
    // Novelty increases arousal and vigilance
    pImpl->arousal = std::min(1.0f, pImpl->arousal + clampedSurprise * pImpl->releaseRate * 2.0f);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + clampedSurprise * pImpl->releaseRate * 1.5f);
    
    // Surprise can trigger a NE burst for enhanced learning
    if (clampedSurprise > 0.7f) {
        pImpl->level = std::min(1.0f, pImpl->level + 0.4f * pImpl->releaseRate);
    }
}

void Norepinephrine::initialize(class Brain* brain) {
    // Initialize norepinephrine system with brain reference
    if (brain) {
        // TODO: Store brain reference if needed for future extensions
        // NE operates as an arousal and vigilance signal that can be integrated
        // with brain systems through the Neuromodulation interface
    }
}