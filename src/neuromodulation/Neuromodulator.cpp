#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    class Brain* brain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), brain(nullptr) {}
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
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

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionEnhancement;
    float memoryEncoding;
    float memoryReverie;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), attentionEnhancement(1.0f), 
             memoryEncoding(1.0f), memoryReverie(0.0f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // Acetylcholine enhances plasticity for memory encoding
    float attentionMod = 1.0f + pImpl->attentionEnhancement * 0.5f;
    float memoryMod = 1.0f + pImpl->memoryEncoding * 0.3f;
    return std::min(2.0f, attentionMod * memoryMod);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(class Brain* brain) {
    (void)brain; // Not used currently
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

void Acetylcholine::enhanceAttention(float enhancement) {
    pImpl->attentionEnhancement = std::clamp(enhancement, 0.0f, 1.0f);
}

void Acetylcholine::enhanceMemoryEncoding(float strength) {
    pImpl->memoryEncoding = std::clamp(strength, 0.0f, 1.0f);
}

void Acetylcholine::setMemoryReverie(bool reverie) {
    pImpl->memoryReverie = reverie ? 1.0f : 0.0f;
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalLevel;
    float sensoryEnhancement;
    float focusLevel;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), arousalLevel(1.0f), 
             sensoryEnhancement(1.0f), focusLevel(0.5f), 
             decayRate(0.05f), releaseRate(1.0f) {}
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
    // Norepinephrine increases plasticity for alertness and focus
    float arousalMod = 1.0f + pImpl->arousalLevel * 0.3f;
    float focusMod = 1.0f + pImpl->focusLevel * 0.2f;
    float sensoryMod = 1.0f + pImpl->sensoryEnhancement * 0.2f;
    return std::min(2.0f, arousalMod * focusMod * sensoryMod);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::initialize(class Brain* brain) {
    (void)brain; // Not used currently
}

void Norepinephrine::reset() {
    pImpl->level = 0.0f;
}

void Norepinephrine::increaseArousal(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::enhanceSensoryProcessing(float enhancement) {
    pImpl->sensoryEnhancement = std::clamp(enhancement, 0.0f, 1.0f);
}

void Norepinephrine::setFocusLevel(float level) {
    pImpl->focusLevel = std::clamp(level, 0.0f, 1.0f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float moodLevel;
    float impulsivityReduction;
    float socialBehaviorStrength;
    float socialBonding;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), moodLevel(1.0f), 
             impulsivityReduction(0.5f), socialBehaviorStrength(1.0f),
             socialBonding(0.5f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // Serotonin modulates plasticity based on mood and social state
    float moodMod = 1.0f + pImpl->moodLevel * 0.2f;
    float impulseMod = 1.0f + (1.0f - pImpl->impulsivityReduction) * 0.1f;
    float socialMod = 1.0f + pImpl->socialBehaviorStrength * 0.3f;
    return std::min(2.0f, moodMod * impulseMod * socialMod);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(class Brain* brain) {
    (void)brain; // Not used currently
}

void Serotonin::reset() {
    pImpl->level = 0.0f;
}

void Serotonin::modulateMood(float level) {
    pImpl->moodLevel = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::reduceImpulsivity(float reduction) {
    pImpl->impulsivityReduction = std::clamp(reduction, 0.0f, 1.0f);
}

void Serotonin::enhanceSocialBehavior(float strength) {
    pImpl->socialBehaviorStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Serotonin::setSocialBonding(float bonding) {
    pImpl->socialBonding = std::clamp(bonding, 0.0f, 1.0f);
}

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations and compare
    // Simple implementation: just set to a placeholder
    pImpl->level = 1.0f;
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference
    pImpl->level = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
    pImpl->lastPattern.clear();
}

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
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

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->history.clear();
}

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    class Brain* brain;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f), brain(nullptr) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
    pImpl->history.push_back(delta);
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->history.clear();
}

float Reward::computeReward(const class Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm