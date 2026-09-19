#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(class Brain* brain) {}

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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float consolidationBoost;
    float plasticityBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , attentionBoost(0.0f)
        , consolidationBoost(0.0f)
        , plasticityBoost(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
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
    // ACh modulates plasticity for enhanced learning
    float plasticityFactor = 1.0f + pImpl->level * 0.5f;
    return plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention and consolidation effects
    if (pImpl->level > 0.5f) {
        // Strong ACh enhances attention and consolidation
        pImpl->attentionBoost = (pImpl->level - 0.5f) * 2.0f;  // 0 to 1 range
        pImpl->consolidationBoost = pImpl->attentionBoost * 0.8f;
        pImpl->plasticityBoost = pImpl->attentionBoost * 0.6f;
    } else {
        // Low ACh reduces attention effects
        pImpl->attentionBoost = 0.0f;
        pImpl->consolidationBoost = 0.0f;
        pImpl->plasticityBoost = 0.0f;
    }
}

void Acetylcholine::boostAttention(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Acetylcholine::consolidateMemory() {
    if (pImpl->brain && pImpl->level > 0.3f) {
        // Enhance memory consolidation in working memory
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        if (workingMemory) {
            // Increase memory trace strength based on ACh level
            float consolidationFactor = pImpl->level * pImpl->level;  // Non-linear effect
            // (Working memory would typically apply this factor)
        }
        
        // Enhance synaptic consolidation in episodic memory
        auto* episodicMemory = pImpl->brain->getEpisodicMemory();
        if (episodicMemory) {
            // Recent memories get strengthened by ACh
            // (Episodic memory would typically apply this effect)
        }
    }
}

void Acetylcholine::enhancePlasticity(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionBoost = 0.0f;
    pImpl->consolidationBoost = 0.0f;
    pImpl->plasticityBoost = 0.0f;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    float vigilance;
    float noveltySensitivity;
    float habituation;
    float phasicBurst;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.5f)  // Higher release rate for phasic responses
        , arousal(0.0f)
        , vigilance(0.0f)
        , noveltySensitivity(1.0f)
        , habituation(0.0f)
        , phasicBurst(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(class Brain* brain) {
    pImpl->brain = brain;
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
    // NE modulates plasticity for arousal-dependent learning
    float arousalFactor = std::clamp(pImpl->level, 0.2f, 1.0f) * 0.8f;  // Minimum 0.2 arousal for plasticity
    return 0.5f + arousalFactor * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply habituation to sustained NE
    if (pImpl->level > 0.3f) {
        pImpl->habituation += (pImpl->level - 0.3f) * 0.1f * static_cast<float>(dt);
        pImpl->habituation = std::min(1.0f, pImpl->habituation);
    } else {
        // Recover from habituation
        pImpl->habituation = std::max(0.0f, pImpl->habituation - 0.05f * static_cast<float>(dt));
    }
    
    // Update arousal and vigilance based on level and habituation
    float effectiveLevel = pImpl->level * (1.0f - pImpl->habituation * 0.5f);  // Habituation reduces effect
    pImpl->arousal = std::clamp(effectiveLevel, 0.0f, 1.0f);
    pImpl->vigilance = pImpl->arousal * 0.8f + pImpl->phasicBurst * 0.2f;
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
    // Reset habituation on phasic increase
    pImpl->habituation = std::max(0.0f, pImpl->habituation - 0.5f);
}

void Norepinephrine::enhanceVigilance(float amount) {
    pImpl->phasicBurst = std::min(1.0f, amount);
}

void Norepinephrine::processNovelty(float novelty, TimestepDuration dt) {
    if (novelty > 0.5f) {
        // Strong novelty triggers phasic NE release
        float burstAmount = (novelty - 0.5f) * 2.0f;
        pImpl->phasicBurst = std::max(pImpl->phasicBurst, burstAmount);
        
        // Phasic burst increases arousal and vigilance
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount * pImpl->releaseRate * 2.0f);
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + burstAmount * 0.5f);
    }
    
    // Sustained novelty increases general arousal
    if (novelty > 0.3f) {
        float sustainedAmount = (novelty - 0.3f) * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + sustainedAmount * pImpl->releaseRate * 0.5f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousal = 0.0f;
    pImpl->vigilance = 0.0f;
    pImpl->noveltySensitivity = 1.0f;
    pImpl->habituation = 0.0f;
    pImpl->phasicBurst = 0.0f;
}

// Serotonin implementation
struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodEffect;  // Positive for positive mood, negative for negative mood
    float inhibitionStrength;  // Reduces impulsivity
    float flexibilityBoost;  // Enhances behavioral flexibility
    float socialEngagement;  // Affects social behavior
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(0.8f)
        , moodEffect(0.0f)
        , inhibitionStrength(0.0f)
        , flexibilityBoost(0.0f)
        , socialEngagement(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
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
    // Serotonin modulates plasticity - low levels increase neural excitability and impulsivity
    // High levels stabilize networks and reduce impulsive behavior
    float stabilityFactor = 1.0f - pImpl->level * 0.5f;  // 1.0 at low 5-HT, 0.5 at high 5-HT
    
    // Also factor in mood effect
    float moodFactor = std::clamp(1.0f + pImpl->moodEffect * 0.3f, 0.5f, 2.0f);
    
    return stabilityFactor * moodFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood effect based on level
    pImpl->moodEffect = (pImpl->level - 0.5f) * 2.0f;  // -1 to +1 range
    
    // Update inhibition and flexibility based on level
    if (pImpl->level > 0.3f) {
        // Moderate to high levels reduce impulsivity
        pImpl->inhibitionStrength = (pImpl->level - 0.3f) * 1.5f;  // 0 to 1.05 range (capped at 1)
        pImpl->flexibilityBoost = (pImpl->level - 0.4f) * 1.25f;  // 0 to 0.75 range
        pImpl->socialEngagement = (pImpl->level - 0.5f) * 1.5f;  // -0.5 to 0.5 range
    } else {
        // Low levels increase impulsivity
        pImpl->inhibitionStrength = 0.0f;
        pImpl->flexibilityBoost = 0.0f;
        pImpl->socialEngagement = 0.0f;
    }
}

void Serotonin::reduceImpulsivity(float amount) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate);
}

void Serotonin::enhanceBehavioralFlexibility(float amount) {
    // Flexibility is tied to overall 5-HT level
    if (amount > 0.5f) {
        // Strong positive mood change
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f);
    } else if (amount < -0.5f) {
        // Strong negative mood change
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f);
    }
}

void Serotonin::respondToReward(float reward) {
    if (reward > 0.5f) {
        // Positive reward prediction error - increase serotonin
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
        // Enhance positive mood effect
        pImpl->moodEffect = std::max(pImpl->moodEffect, 0.3f);
    } else if (reward < -0.5f) {
        // Negative reward prediction error - decrease serotonin
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f);
        // Enhance negative mood effect
        pImpl->moodEffect = std::min(pImpl->moodEffect, -0.3f);
    }
}

void Serotonin::respondToPredictionError(float error) {
    // Prediction error also affects serotonin
    if (error > 0.5f) {
        // Unexpected positive outcome
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f);
        pImpl->flexibilityBoost = std::min(1.0f, pImpl->flexibilityBoost + 0.1f);
    } else if (error < -0.5f) {
        // Unexpected negative outcome
        pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f);
        pImpl->inhibitionStrength = std::max(0.0f, pImpl->inhibitionStrength - 0.1f);
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodEffect = 0.0f;
    pImpl->inhibitionStrength = 0.0f;
    pImpl->flexibilityBoost = 0.0f;
    pImpl->socialEngagement = 0.0f;
}

} // namespace nlm
