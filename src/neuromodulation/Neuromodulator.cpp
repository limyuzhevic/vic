#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

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
    // Standard plasticity factor
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Standard decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Standard reward burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Standard prediction error response
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// ========== ENHANCED DOPAMINE EFFECTS ==========

// Enhanced plasticity factor with more sophisticated modulation
float Dopamine::getEnhancedPlasticityFactor() const {
    // Implement full dopamine effects on plasticity:
    // 1. D1 receptor effect (increase LTP, decrease LTD)
    // 2. D2 receptor effect (decrease LTP, increase LTD)  
    // 3. Inverted-U relationship with dopamine level
    
    float level = pImpl->level;
    
    // Inverted-U relationship: moderate dopamine optimal for plasticity
    // Low dopamine: less stable
    // High dopamine: too much learning, instability
    // Medium dopamine: optimal learning
    float d1Effect = 0.3f * (1.0f - std::abs(level - 0.5f) / 0.5f);
    float d2Effect = -0.2f * (1.0f - std::abs(level - 0.5f) / 0.5f);
    float baselinePlasticity = 1.0f;
    
    return baselinePlasticity + d1Effect + d2Effect;
}

// Get dopamine effect on working memory consolidation
float Dopamine::getMemoryConsolidationFactor() const {
    // Moderate to high dopamine enhances memory consolidation
    // Too much dopamine can interfere with consolidation
    return 0.5f + 0.5f * (pImpl->level - 0.3f);
}

// Get dopamine effect on attention
float Dopamine::getAttentionModulationFactor() const {
    // Low dopamine reduces attention, high dopamine increases attention
    // Optimal range for cognitive focus
    float level = pImpl->level;
    return 0.3f + 0.4f * level;
}

// Signal learning event (combines reward and prediction error)
void Dopamine::signalLearningEvent(float reward, float predictionError) {
    // Real dopamine learning signal combines reward prediction error
    float lrnSignal = reward + predictionError;
    signalReward(lrnSignal);
}

// Enhanced plasticity update with dopamine receptor effects
void Dopamine::applyToPlasticity(float& ltpWeight, float& ltdWeight, float level) {
    // D1-like receptor effects:
    // - Increase LTP magnitude
    // - Decrease LTD magnitude
    // - Enhance learning rate
    
    // D2-like receptor effects:
    // - Decrease LTP magnitude  
    // - Increase LTD magnitude
    // - Stabilize learned connections
    
    float d1Mod = 1.0f + 0.5f * level;      // D1 enhances LTP, reduces LTD
    float d2Mod = 1.0f - 0.3f * level;      // D2 reduces LTP, enhances LTD
    
    // Apply to STDP parameters
    ltpWeight *= d1Mod;
    ltdWeight *= d2Mod;
}

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    float level;
    float attentionBaseline;
    float memoryBaseline;
    float attentionGain;
    float memoryGain;
    float decayRate;
    
    Impl() : level(0.0f), attentionBaseline(0.5f), memoryBaseline(0.3f),
             attentionGain(2.0f), memoryGain(1.5f), decayRate(0.05f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

static float getAChLevel() const { return pImpl->level; }

void setAChLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }

static float getAChPlasticityFactor() const {
    return 0.8f + 0.4f * pImpl->level;
}

void updateACh(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->attentionBaseline,
                         pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void signalAttentionACh(float attentionLevel) {
    pImpl->level = std::min(1.0f, pImpl->level + attentionLevel * pImpl->attentionGain);
}

void signalMemoryFocusACh(float memoryFocus) {
    pImpl->level = std::min(1.0f, pImpl->level + memoryFocus * pImpl->memoryGain);
}

float getAChAttentionEnhancement() const {
    return 0.5f + 0.5f * pImpl->level;
}

float getAChMemoryConsolidationStrength() const {
    return 1.0f + 0.8f * pImpl->level;
}

void applyToAttentionACh(float& inhibitionStrength, float& excitationStrength) {
    float level = pImpl->level;
    inhibitionStrength *= (1.0f + level * 0.5f);     // ACh reduces inhibition
    excitationStrength *= (1.0f + level * 1.0f);      // ACh increases excitation
}

void applyToMemoryACh(float& consolidationThreshold) {
    consolidationThreshold /= (1.0f + pImpl->level * 0.8f);  // ACh lowers consolidation threshold
}

// ========== NORADRENALINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    float level;
    float arousalBaseline;
    float vigilanceBaseline;
    float arousalGain;
    float vigilanceGain;
    float decayRate;
    
    Impl() : level(0.0f), arousalBaseline(0.3f), vigilanceBaseline(0.5f),
             arousalGain(1.5f), vigilanceGain(1.0f), decayRate(0.08f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

static float getNEArousalLevel() const { return pImpl->level; }

void setNEArousalLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }

static float getNEPlasticityFactor() const {
    return 1.0f + 0.2f * pImpl->level;  // Moderate arousal enhances plasticity
}

void updateNE(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->arousalBaseline,
                         pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void signalArousalNE(float arousalLevel) {
    pImpl->level = std::min(1.0f, pImpl->level + arousalLevel * pImpl->arousalGain);
}

float getNEArousalLevel() const {
    return pImpl->level;
}

float getNEVigilanceLevel() const {
    return 0.5f + 0.5f * pImpl->level;
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float moodBaseline;
    float socialBaseline;
    float moodGain;
    float socialGain;
    float decayRate;
    
    Impl() : level(0.0f), moodBaseline(0.3f), socialBaseline(0.3f),
             moodGain(1.2f), socialGain(0.8f), decayRate(0.06f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

static float get5HTMoodLevel() const { return pImpl->level; }

void set5HTMoodLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }

static float get5HTPlasticityFactor() const {
    return 1.0f - 0.3f * pImpl->level;  // High serotonin reduces plasticity (stability)
}

void update5HT(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->moodBaseline,
                         pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void signalMood5HT(float moodLevel) {
    pImpl->level = std::min(1.0f, pImpl->level + moodLevel * pImpl->moodGain);
}

float get5HTMoodLevel() const {
    return pImpl->level;
}

float get5HTSocialBehaviorModulator() const {
    return 0.3f + 0.7f * pImpl->level;
}

} // namespace nlm