#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Enhanced Dopamine neuromodulator with attention and learning effects
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Enhanced effects
    float attentionModulator;      // Modulates attentional selection
    float memoryModulator;         // Modulates memory encoding/retrieval
    float learningRate;            // Modulates overall learning rate
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , attentionModulator(1.0f)
        , memoryModulator(1.0f)
        , learningRate(1.0f) {}
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
    
    // Update modulated effects based on new level
    pImpl->attentionModulator = 1.0f + pImpl->level * 0.5f;  // Boost attention
    pImpl->memoryModulator = 1.0f + pImpl->level * 0.3f;     // Enhance memory
    pImpl->learningRate = 0.5f + pImpl->level * 1.0f;         // Increase learning
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced plasticity factor: higher dopamine leads to more plastic but more stable changes
    float baseFactor = 0.5f + 0.5f * pImpl->level;
    float attentionBonus = std::min(0.5f, pImpl->attentionModulator * 0.3f);
    return std::min(2.0f, baseFactor + attentionBonus);
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
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// Enhanced Novelty detector with attentional capture
struct Novelty::Impl {
    float level;
    float decayRate;
    float threshold;
    float attentionalCapture;
    float memoryTraceStrength;
    
    Impl() 
        : level(0.0f)
        , decayRate(0.05f)
        , threshold(0.1f)
        , attentionalCapture(2.0f)
        , memoryTraceStrength(1.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

float Novelty::getAttentionalCapture() const {
    return pImpl->level > pImpl->threshold ? pImpl->attentionalCapture : 1.0f;
}

float Novelty::getMemoryTraceStrength() const {
    return pImpl->level * pImpl->memoryTraceStrength;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty level
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::detectNovelty(float inputValue, float previousValue) {
    float diff = std::abs(inputValue - previousValue);
    pImpl->level = std::max(pImpl->level, diff);
}

void Novelty::reset() {
    pImpl->level = 0.0f;
}

// Enhanced PredictionError with learning modulation
struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float magnitude;
    float historyMax;
    float learningInfluence;
    
    Impl() 
        : brain(nullptr)
        , error(0.0f)
        , magnitude(0.0f)
        , historyMax(0.0f)
        , learningInfluence(1.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Prediction error system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->error = actual - predicted;
    pImpl->magnitude = std::abs(pImpl->error);
    pImpl->historyMax = std::max(pImpl->historyMax, pImpl->magnitude);
}

void PredictionError::updatePrediction(float newPrediction) {
    // Update prediction based on new information
    if (pImpl->error != 0.0f) {
        float adjustment = pImpl->error * 0.1f;  // Learning rate
        newPrediction += adjustment;
    }
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->magnitude = 0.0f;
}

// Enhanced Curiosity system with attention and memory integration
struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float attentionalStrength;
    float memorySearchStrength;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , attentionalStrength(1.0f)
        , memorySearchStrength(0.8f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Enhanced curiosity calculation
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // More dynamic response
    float response = 0.2f * (target - pImpl->level);
    pImpl->level += response;
    
    // Apply attentional and memory strengths
    pImpl->level *= pImpl->attentionalStrength * pImpl->memorySearchStrength;
    
    // Decay with environmental modulation
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

// Enhanced Reward system with neuromodulatory integration
struct Reward::Impl {
    float value;
    float accumulated;
    std::vector<float> history;
    float decayRate;
    float attentionalModulation;
    float learningModulation;
    
    Impl() 
        : value(0.0f)
        , accumulated(0.0f)
        , decayRate(0.01f)
        , attentionalModulation(1.0f)
        , learningModulation(1.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->value;
}

void Reward::setValue(float value) {
    pImpl->value = value;
    pImpl->accumulated += value;
}

void Reward::add(float delta) {
    pImpl->value = delta;
    pImpl->accumulated += delta;
    pImpl->history.push_back(delta);
    
    // Keep history manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Reward::reset() {
    pImpl->value = 0.0f;
    pImpl->accumulated = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const Observation& observation) const {
    // Enhanced reward computation with sensory integration
    float baseReward = pImpl->value;
    
    // Integrate with sensory observation if possible
    // (In a real implementation, this would access observation properties)
    float sensoryFactor = 1.0f;
    
    // Time-based reward modulation
    float timeFactor = 1.0f;
    if (!pImpl->history.empty()) {
        float recentAverage = 0.0f;
        size_t recentCount = std::min(size_t(10), pImpl->history.size());
        for (size_t i = 0; i < recentCount; ++i) {
            recentAverage += pImpl->history[pImpl->history.size() - 1 - i];
        }
        recentAverage /= recentCount;
        timeFactor = std::max(0.5f, std::min(1.5f, recentAverage));
    }
    
    return baseReward * sensoryFactor * timeFactor * pImpl->attentionalModulation * pImpl->learningModulation;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

// Acetylcholine neuromodulator for attention and memory
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionalGain;
    float memoryTraceStrength;
    float encodingModulation;
    float retrievalModulation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , attentionalGain(1.0f)
        , memoryTraceStrength(1.0f)
        , encodingModulation(1.0f)
        , retrievalModulation(1.0f) {}
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
    
    // Update attentional and memory effects
    pImpl->attentionalGain = 1.0f + pImpl->level * 2.0f;      // Strong attentional effects
    pImpl->memoryTraceStrength = 1.0f + pImpl->level * 1.5f; // Enhanced memory traces
    pImpl->encodingModulation = 1.0f + pImpl->level * 0.8f;  // Better encoding
    pImpl->retrievalModulation = 1.0f + pImpl->level * 0.5f; // Better retrieval
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine strongly promotes memory-related plasticity
    return 1.0f + pImpl->level * 1.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Slow decay, maintaining baseline attention
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.01f * static_cast<float>(dt));
}

// Norepinephrine neuromodulator for arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float vigilance;
    float arousal;
    float stressResponse;
    float focusModulation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , vigilance(1.0f)
        , arousal(1.0f)
        , stressResponse(1.0f)
        , focusModulation(1.0f) {}
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
    
    // Update vigilance and arousal effects
    pImpl->vigilance = 1.0f + pImpl->level * 1.0f;      // Increased vigilance
    pImpl->arousal = 1.0f + pImpl->level * 0.8f;       // Increased arousal
    pImpl->stressResponse = 1.0f + pImpl->level * 0.6f; // Stress response
    pImpl->focusModulation = 1.0f + pImpl->level * 0.9f; // Enhanced focus
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates attention and stress-related plasticity
    return 1.0f + pImpl->level * 0.8f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Slow, sustained modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.005f * static_cast<float>(dt));
}

// Serotonin neuromodulator for mood and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float moodModulation;
    float socialModulation;
    float impulsivityModulation;
    float sleepModulation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , moodModulation(1.0f)
        , socialModulation(1.0f)
        , impulsivityModulation(1.0f)
        , sleepModulation(1.0f) {}
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
    
    // Update mood and behavioral effects
    pImpl->moodModulation = 1.0f + pImpl->level * 0.7f;      // Improved mood
    pImpl->socialModulation = 1.0f + pImpl->level * 0.5f;   // Enhanced social behavior
    pImpl->impulsivityModulation = 1.0f - pImpl->level * 0.8f; // Reduced impulsivity
    pImpl->sleepModulation = 1.0f + pImpl->level * 1.2f;    // Enhanced sleep
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood-related plasticity and behavioral flexibility
    return 1.0f + pImpl->level * 0.6f;
}

void Serotonin::update(TimestepDuration dt) {
    // Slow, steady modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.02f * static_cast<float>(dt));
}

} // namespace nlm