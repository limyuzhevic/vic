#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// ========== DOPAMINE IMPLEMENTATION ==========

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
    // Dopamine modulates plasticity: higher levels increase LTP, decrease LTD
    // Enhanced dynamics beyond simple scaling - dopamine can switch plasticity regimes
    if (pImpl->level < 0.3f) {
        return 0.3f;  // Low dopamine: conservative plasticity
    } else if (pImpl->level < 0.7f) {
        return 0.5f + 0.5f * ((pImpl->level - 0.3f) / 0.4f);  // Medium: linear increase
    } else {
        return 1.2f;  // High dopamine: enhanced LTP, LTD suppression
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline with time constant tau_dopamine
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst on unexpected reward
    // Creates episodic memory tags for reward prediction learning
    float burst = std::min(pImpl->peak, reward * pImpl->releaseRate * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Store reward signal for memory integration
    // Dopamine signals trigger memory consolidation
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction errors: positive for better-than-expected, negative for worse
    // Error-guided learning updates synaptic weights
    float update = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, pImpl->level + update));
    
    // Also modulate working memory encoding based on prediction error
    // Error signals update memory traces and promote learning
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    bool novelFlag;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), novelFlag(false) {}
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
    // Serotonin has complex effects: moderate levels enhance consolidation, high levels suppress
    if (pImpl->level < 0.3f) {
        return 0.8f;  // Low serotonin: basic plasticity
    } else if (pImpl->level < 0.7f) {
        return 1.0f;  // Medium: optimal consolidation
    } else {
        return 0.6f;  // High: reduced plasticity, stress response
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Decay with longer time constant
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::updateFromNovelty(bool isNovel) {
    // Serotonin detects novel experiences and enhances consolidation
    pImpl->novelFlag = isNovel;
    
    if (isNovel) {
        // Novelty triggers serotonin release for memory encoding
        pImpl->level = std::min(1.0f, pImpl->level + 0.3f);
    }
}

void Serotonin::affectMemoryConsolidation() {
    // Moderate serotonin enhances memory consolidation
    // Novelty with serotonin leads to strong, lasting memories
    if (pImpl->novelFlag && pImpl->level > 0.3f) {
        // Strong consolidation for novel experiences
        // Serotonin promotes systems consolidation
    }
    
    // Reset novelty flag
    pImpl->novelFlag = false;
}

// ========== NOREPINEPHRINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float attentionLevel;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.08f), attentionLevel(0.5f) {}
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
    // Update attention level based on NE
    pImpl->attentionLevel = 0.3f + 0.7f * level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE has inverted U-shaped effect: optimal attention around 0.7
    float attention = pImpl->attentionLevel;
    if (attention < 0.4f) {
        return 0.6f + attention;  // Increasing from low arousal
    } else if (attention < 0.8f) {
        return 1.0f;  // Peak performance
    } else {
        return 1.0f - 0.6f * (attention - 0.8f);  // Declining from high arousal
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay with moderate time constant
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::updateFromAttention(float attentionLevel) {
    // NE modulates attention and encoding strength
    pImpl->attentionLevel = std::clamp(attentionLevel, 0.0f, 1.0f);
    
    // High attention increases encoding efficiency
    if (pImpl->attentionLevel > 0.7f) {
        // Strong encoding with high NE
    }
}

void Norepinephrine::modulateEncoding() {
    // NE enhances synaptic encoding in hippocampus and cortex
    // Attention-grabbing events trigger NE release for memory formation
    if (pImpl->attentionLevel > 0.6f) {
        // Boost encoding of attended information
        // NE facilitates synaptic consolidation of important memories
    }
}

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.06f), learningRate(0.5f) {}
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
    // Update learning rate based on ACh
    pImpl->learningRate = 0.3f + 0.7f * level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity and memory formation
    return 0.7f + 0.6f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay with moderate time constant
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::updateFromLearningEvent(float learningRate) {
    // ACh responds to learning events and enhances plasticity
    pImpl->learningRate = std::clamp(learningRate, 0.0f, 1.0f);
    
    // Learning events trigger ACh release
    if (learningRate > 0.5f) {
        pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
    }
}

void Acetylcholine::enhancePlasticity() {
    // ACh modulates synaptic plasticity and memory encoding
    // In the hippocampus, ACh enhances LTP and promotes memory formation
    // In the cortex, ACh facilitates working memory and attention
    
    // Strong ACh supports:
    // - Long-term potentiation (LTP) in hippocampal circuits
    // - Working memory maintenance through persistent activity
    // - Encoding of new information into long-term storage
    // - Integration of sensory inputs with stored memories
}

} // namespace nlm
