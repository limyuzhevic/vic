#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Neuromodulator base implementation
void Neuromodulator::update(TimestepDuration dt) {
    // Default implementation - decays toward baseline
    if (impl->level > impl->baseline) {
        impl->level = std::max(impl->baseline, 
                             impl->level - impl->decayRate * static_cast<float>(dt));
    }
}

// Dopamine implementation
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
    // Base plasticity factor
    float baseFactor = 1.0f;
    
    // Dopamine enhances plasticity
    if (pImpl->level > 0.5f) {
        baseFactor *= (0.5f + 0.5f * (pImpl->level - 0.5f) / 0.5f);
    }
    
    return baseFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::updateWorkingMemory(float memoryStrength) {
    // Dopamine modulates working memory updating
    if (memoryStrength > 0.0f) {
        float updateAmount = 0.1f * memoryStrength * pImpl->level;
        pImpl->level = std::min(pImpl->peak, pImpl->level + updateAmount);
    }
}

void Dopamine::modulateAttention(float attentionFocus) {
    // Dopamine modulates attention selection
    if (attentionFocus > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f * attentionFocus);
    }
}

void Dopamine::consolidateMemory(float memoryStrength) {
    // Dopamine supports memory consolidation
    if (memoryStrength > 0.0f) {
        float consolidationBoost = 0.15f * memoryStrength * pImpl->level;
        pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationBoost);
    }
}

void Dopamine::updateEpisodicMemory(float rewardValue) {
    // Dopamine encodes reward in episodic memory
    if (rewardValue > 0.0f) {
        float rewardEncoding = 0.2f * rewardValue;
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardEncoding);
    }
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float memoryConsolidationStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionModulation(1.0f), memoryConsolidationStrength(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

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
    // Cholinergic baseline plasticity factor
    return 1.0f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float stimulusSalience) {
    // Cholinergic enhancement of attention based on stimulus salience
    if (stimulusSalience > 0.0f) {
        pImpl->attentionModulation = 1.0f + 0.5f * stimulusSalience;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * stimulusSalience);
    }
}

void Acetylcholine::facilitateMemoryConsolidation(float memoryStrength) {
    // Cholinergic facilitation of memory consolidation
    if (memoryStrength > 0.0f) {
        pImpl->memoryConsolidationStrength = 1.0f + 0.5f * memoryStrength;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.15f * memoryStrength);
    }
}

void Acetylcholine::modulateSynapticPlasticity(float plasticityRate) {
    // Cholinergic modulation of synaptic plasticity
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f * plasticityRate);
}

void Acetylcholine::updateWorkingMemoryEncoding(const std::vector<float>& pattern) {
    // Cholinergic enhancement of working memory encoding
    if (!pattern.empty()) {
        float patternStrength = 0.0f;
        for (float val : pattern) {
            patternStrength += std::abs(val);
        }
        patternStrength /= pattern.size();
        
        if (patternStrength > 0.0f) {
            pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * patternStrength);
        }
    }
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidationStrength;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float betaOscillationFrequency;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(1.0f), vigilanceLevel(1.0f), betaOscillationFrequency(13.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

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
    // Noradrenergic baseline plasticity factor
    return 1.0f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float stimulusIntensity) {
    // Noradrenergic arousal based on stimulus intensity
    if (stimulusIntensity > 0.0f) {
        pImpl->arousalLevel = 1.0f + 0.8f * stimulusIntensity;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.15f * stimulusIntensity);
    }
}

void Norepinephrine::enhanceVigilance(float alertness) {
    // Noradrenergic enhancement of vigilance
    if (alertness > 0.0f) {
        pImpl->vigilanceLevel = 1.0f + 0.6f * alertness;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * alertness);
    }
}

void Norepinephrine::modulateBetaOscillations(float frequency) {
    // Noradrenergic modulation of beta oscillations
    pImpl->betaOscillationFrequency = 13.0f + 5.0f * frequency;
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f * frequency);
}

void Norepinephrine::updateLocusCoeruleusActivity(float activity) {
    // Noradrenergic update of locus coeruleus activity
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f * activity);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulseControlLevel;
    float socialEngagementLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             moodLevel(1.0f), impulseControlLevel(1.0f), socialEngagementLevel(1.0f) {}
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
    // Serotonergic baseline plasticity factor
    return 1.0f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateMood(float moodState) {
    // Serotonergic mood modulation
    if (moodState > 0.0f) {
        pImpl->moodLevel = 1.0f + 0.7f * moodState;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.12f * moodState);
    }
}

void Serotonin::regulateImpulsivity(float impulseControl) {
    // Serotonergic regulation of impulsivity
    if (impulseControl > 0.0f) {
        pImpl->impulseControlLevel = 1.0f + 0.5f * impulseControl;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.08f * impulseControl);
    }
}

void Serotonin::influenceSocialBehavior(float socialEngagement) {
    // Serotonergic influence on social behavior
    if (socialEngagement > 0.0f) {
        pImpl->socialEngagementLevel = 1.0f + 0.6f * socialEngagement;
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * socialEngagement);
    }
}

void Serotonin::updateSocialMemoryFormation(float socialStrength) {
    // Serotonergic update of social memory formation
    if (socialStrength > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * socialStrength);
    }
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulseControlLevel() const {
    return pImpl->impulseControlLevel;
}

} // namespace nlm