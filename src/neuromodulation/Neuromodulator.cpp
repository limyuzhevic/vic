#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// ========== DOPAMINE IMPLEMENTATION ==========

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityMultiplier;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             plasticityMultiplier(1.0f) {}
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
    // Dopamine modulates plasticity: higher levels increase learning rate
    // This is the classic dopamine prediction error signal
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add decay history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on unexpected reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->history.push_back(pImpl->level);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine signals prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->history.push_back(pImpl->level);
}

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidationStrength;
    std::vector<float> attentionWeights;
    std::vector<float> memoryTraces;
    float lastAttentionUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             memoryConsolidationStrength(0.8f), lastAttentionUpdate(0.0f) {
        attentionWeights.resize(256, 1.0f);  // 256 attentional weights
        memoryTraces.resize(128, 0.0f);     // 128 memory traces
    }
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
    pImpl->level = level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates synaptic plasticity for memory encoding
    // Promotes LTP for attended/novel information
    return 1.0f + pImpl->level * pImpl->memoryConsolidationStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: slow rise and decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention weights
    for (float& weight : pImpl->attentionWeights) {
        weight = 1.0f / (1.0f + pImpl->level * 2.0f);  // Attention inversely related to ACh
    }
    
    // Update memory traces
    for (float& trace : pImpl->memoryTraces) {
        trace = std::max(0.0f, trace - 0.01f * static_cast<float>(dt));  // Decay
    }
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh triggers memory consolidation from working to episodic memory
    float consolidationProbability = std::min(1.0f, pImpl->level * 2.0f);
    
    if (std::rand() % 1000 < consolidationProbability * 1000) {
        // Store attention-weighted memory trace
        pImpl->memoryTraces.push_back(pImpl->level);
        if (pImpl->memoryTraces.size() > 128) {
            pImpl->memoryTraces.erase(pImpl->memoryTraces.begin());
        }
        
        NLM_LOG_INFO("Acetylcholine-triggered memory consolidation");
    }
}

// ========== NOREPINEPHRINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float alertness;
    float locusCoeruleusGain;
    std::vector<float> arousalSignals;
    std::vector<float> vigilanceFilters;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             alertness(0.5f), locusCoeruleusGain(2.0f) {
        arousalSignals.resize(128, 0.0f);
        vigilanceFilters.resize(64, 1.0f);
    }
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
    pImpl->level = level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance, affecting learning
    // Higher NE increases alertness but may reduce fine detail processing
    float attentionGain = 1.0f + pImpl->alertness * 0.5f;
    float detailGain = 1.0f / (1.0f + pImpl->alertness * 2.0f);
    
    return attentionGain * detailGain;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: burst-like responses to novel/stimulating events
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal signals
    for (float& signal : pImpl->arousalSignals) {
        signal = std::max(0.0f, signal - 0.02f * static_cast<float>(dt));
    }
    
    // Update vigilance filters
    pImpl->alertness = std::min(1.0f, pImpl->alertness + 0.01f * static_cast<float>(dt));
    for (float& filter : pImpl->vigilanceFilters) {
        filter = 1.0f / (1.0f + pImpl->alertness * 1.5f);
    }
}

void Norepinephrine::increaseAlertness(float amount) {
    pImpl->alertness = std::min(1.0f, pImpl->alertness + amount);
    pImpl->level = std::min(pImpl->peak, pImpl->level + amount * pImpl->releaseRate * 0.5f);
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float behavioralInhibition;
    std::vector<float> moodSignals;
    std::vector<float> socialReinforcement;
    float punishmentSensitivity;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f),
             behavioralInhibition(0.5f), punishmentSensitivity(0.7f) {
        moodSignals.resize(64, 0.0f);
        socialReinforcement.resize(32, 0.0f);
    }
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
    pImpl->level = level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and behavioral inhibition
    // Higher serotonin generally reduces impulsivity and promotes persistent behavior
    float inhibitionFactor = 1.0f + pImpl->behavioralInhibition * 0.5f;
    return 0.5f / inhibitionFactor;  // Higher serotonin = lower plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: tonic firing modulated by rewards/punishments
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood signals
    for (float& signal : pImpl->moodSignals) {
        signal = std::max(0.0f, signal - 0.015f * static_cast<float>(dt));
    }
}

void Serotonin::inhibitBehavior(float strength) {
    pImpl->behavioralInhibition = std::min(1.0f, pImpl->behavioralInhibition + strength);
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
}

bool Serotonin::isBehaviorInhibited() const {
    return pImpl->behavioralInhibition > 0.7f;
}

} // namespace nlm
