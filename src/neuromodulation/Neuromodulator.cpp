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

} // namespace nlm

// Acetylcholine implementation
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float potentiationRate;
    std::vector<float> memoryTrace;
    float attentionBoost;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , maxLevel(1.0f)
        , decayRate(0.1f)
        , potentiationRate(0.3f)
        , memoryTrace()
        , attentionBoost(1.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + pImpl->level * pImpl->potentiationRate;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float amount) {
    setLevel(getLevel() + amount * pImpl->attentionBoost);
}

void Acetylcholine::storeMemoryTrace(const std::vector<float>& trace) {
    pImpl->memoryTrace = trace;
    if (!trace.empty()) {
        float avgTrace = std::accumulate(trace.begin(), trace.end(), 0.0f) / trace.size();
        setLevel(avgTrace * pImpl->potentiationRate);
    }
}

const std::vector<float>& Acetylcholine::getMemoryTrace() const {
    return pImpl->memoryTrace;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->memoryTrace.clear();
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

void Acetylcholine::setAttentionBoost(float boost) {
    pImpl->attentionBoost = boost;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float recoveryRate;
    bool isSleep;
    float arousalLevel;
    std::vector<float> alertnessHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , maxLevel(1.0f)
        , decayRate(0.2f)
        , recoveryRate(0.15f)
        , isSleep(false)
        , arousalLevel(0.5f)
        , alertnessHistory() {}
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
    if (pImpl->isSleep) {
        pImpl->level = pImpl->baseline;
        return;
    }
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->maxLevel);
    pImpl->arousalLevel = pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    if (pImpl->isSleep) return 0.1f;
    return 1.0f + pImpl->level * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    if (pImpl->isSleep) {
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level + pImpl->recoveryRate * static_cast<float>(dt));
        pImpl->isSleep = false;
        return;
    }
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = pImpl->level;
    pImpl->alertnessHistory.push_back(pImpl->arousalLevel);
}

void Norepinephrine::increaseArousal(float amount) {
    if (!pImpl->isSleep) {
        setLevel(getLevel() + amount);
    }
}

void Norepinephrine::decreaseArousal(float amount) {
    setLevel(getLevel() - amount);
}

void Norepinephrine::setSleepState(bool sleeping) {
    if (pImpl->isSleep != sleeping) {
        pImpl->isSleep = sleeping;
        if (sleeping) {
            // TODO: Add proper logging
            // NLM_LOG_INFO("Norepinephrine system entering sleep state");
        }
    }
}

bool Norepinephrine::isAsleep() const {
    return pImpl->isSleep;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->isSleep = false;
    pImpl->arousalLevel = pImpl->baseline;
    pImpl->alertnessHistory.clear();
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

void Norepinephrine::setReactivity(float reactivity) {
    pImpl->decayRate = reactivity;
}

const std::vector<float>& Norepinephrine::getAlertnessHistory() const {
    return pImpl->alertnessHistory;
}

// Serotonin implementation
struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float recoveryRate;
    float moodState;
    float impulsivity;
    std::string socialContext;
    std::vector<float> moodHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.2f)
        , maxLevel(1.0f)
        , decayRate(0.15f)
        , recoveryRate(0.1f)
        , moodState(0.5f)
        , impulsivity(0.5f)
        , socialContext("neutral")
        , moodHistory() {}
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->maxLevel);
    pImpl->moodState = (pImpl->level - pImpl->baseline) / (pImpl->maxLevel - pImpl->baseline);
}

float Serotonin::getPlasticityFactor() const {
    float moodFactor = 1.0f + pImpl->moodState * 0.3f;
    return moodFactor * (1.0f - pImpl->impulsivity * 0.2f);
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->moodState = pImpl->moodState * 0.95f + 0.5f * 0.05f;
    pImpl->moodHistory.push_back(pImpl->moodState);
}

void Serotonin::modulateMood(float moodState) {
    pImpl->moodState = std::clamp(moodState, 0.0f, 1.0f);
    setLevel(pImpl->baseline + pImpl->moodState * (pImpl->maxLevel - pImpl->baseline));
}

void Serotonin::adjustImpulsivity(float level) {
    pImpl->impulsivity = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::encodeSocialContext(const std::string& context) {
    pImpl->socialContext = context;
}

const std::string& Serotonin::getSocialContext() const {
    return pImpl->socialContext;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodState = 0.5f;
    pImpl->impulsivity = 0.5f;
    pImpl->socialContext = "neutral";
    pImpl->moodHistory.clear();
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

void Serotonin::setReactivity(float reactivity) {
    pImpl->decayRate = reactivity;
}

const std::vector<float>& Serotonin::getMoodHistory() const {
    return pImpl->moodHistory;
}

} // namespace nlm
