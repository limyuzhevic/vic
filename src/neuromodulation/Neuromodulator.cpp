#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Base implementation details
struct NeuromodulatorBase {
    float level = 0.0f;
    float baseline = 0.0f;
    float peak = 1.0f;
    float decayRate = 0.1f;
    float releaseRate = 1.0f;
    float adaptation = 0.0f;
    float historyDecay = 0.95f;
    float integrationRate = 0.1f;
    std::unordered_map<std::string, float> eventHistory;
};

struct Dopamine::Impl : NeuromodulatorBase {
    // Dopamine-specific parameters
    float tonicLevel = 0.1f;
    float phasicBurst = 2.0f;
    float predictionErrorGain = 1.5f;
    float rewardSensitivity = 1.0f;
    float punishmentSensitivity = 0.3f;
    float noveltySensitivity = 0.5f;
    float learningRate = 0.01f;
    float targetRange = 0.8f;
    
    // History tracking
    float lastReward = 0.0f;
    float lastPredictionError = 0.0f;
    float lastNovelty = 0.0f;
    float predictionErrorHistory = 0.0f;
    
    Impl() {
        baseline = tonicLevel;
        level = baseline;
    }
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Combines reward prediction error, novelty, and current dopamine level
    
    float error = pImpl->predictionErrorHistory;
    
    // Normalized reward prediction error contribution
    float errorContribution = std::tanh(error * pImpl->predictionErrorGain);
    
    // Novelty contribution (phasic burst)
    float noveltyContribution = pImpl->lastNovelty * pImpl->noveltySensitivity;
    
    // Tonic baseline contribution
    float tonicContribution = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    
    // Combined plasticity factor with biological constraints
    float plasticityFactor = 1.0f + errorContribution * 0.5f + 
                           noveltyContribution * 0.3f + 
                           tonicContribution * 0.2f;
    
    // Apply biological limits (0.2x to 3.0x normal plasticity)
    return std::clamp(plasticityFactor, 0.2f, 3.0f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: combines decay, learning, and prediction error integration
    
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate prediction error
    float errorIntegration = pImpl->predictionErrorHistory * pImpl->learningRate * dtFloat;
    pImpl->level += errorIntegration;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update history
    pImpl->predictionErrorHistory *= pImpl->historyDecay;
    pImpl->lastReward = 0.0f;
    pImpl->lastNovelty = 0.0f;
}

void Dopamine::signalEvent(const std::string& eventType, float magnitude) {
    if (eventType == "reward") {
        signalReward(magnitude);
    } else if (eventType == "punishment") {
        signalPunishment(magnitude);
    } else if (eventType == "reward_prediction_error") {
        signalRewardPredictionError(magnitude);
    } else if (eventType == "novelty") {
        signalNovelty(magnitude);
    } else {
        // Generic event response
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * pImpl->releaseRate * 0.5f);
    }
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionErrorHistory = 0.0f;
    pImpl->lastReward = 0.0f;
    pImpl->lastNovelty = 0.0f;
    pImpl->adaptation = 0.0f;
}

void Dopamine::signalReward(float reward) {
    pImpl->lastReward = reward;
    
    // Phasic burst response
    float burst = reward * pImpl->rewardSensitivity * pImpl->phasicBurst;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Positive prediction error
    float predictionError = std::max(0.0f, reward - pImpl->lastReward);
    pImpl->predictionErrorHistory = predictionError;
}

void Dopamine::signalPunishment(float punishment) {
    pImpl->lastReward = -punishment;
    
    // Phasic dip response
    float dip = punishment * pImpl->punishmentSensitivity * pImpl->phasicBurst * 0.7f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - dip);
    
    // Negative prediction error
    float predictionError = -std::max(0.0f, punishment - (-pImpl->lastReward));
    pImpl->predictionErrorHistory = predictionError;
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionErrorHistory = error;
    
    // Prediction error response is phasic
    float errorResponse = std::abs(error) * pImpl->predictionErrorGain;
    if (error > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorResponse * 0.3f);
    } else {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - errorResponse * 0.3f);
    }
}

void Dopamine::signalNovelty(float novelty) {
    pImpl->lastNovelty = novelty;
    
    // Novelty-triggered phasic response
    float noveltyResponse = novelty * pImpl->noveltySensitivity * pImpl->phasicBurst * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyResponse);
    
    // Novelty can enhance prediction error learning
    pImpl->predictionErrorHistory *= 1.2f; // Boost prediction error sensitivity
}

// Acetylcholine implementation
struct Acetylcholine::Impl : NeuromodulatorBase {
    // ACh-specific parameters
    float tonicLevel = 0.05f;
    float phasicBurst = 1.5f;
    float memoryConsolidationRate = 0.02f;
    float attentionGain = 1.2f;
    float workingMemoryCapacity = 1.0f;
    float goalRelevanceWeight = 0.8f;
    
    // History tracking
    float attentionHistory = 0.0f;
    float workingMemoryHistory = 0.0f;
    
    Impl() {
        baseline = tonicLevel;
        level = baseline;
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh-modulated plasticity: enhances attention-related learning
    
    float attentionFactor = pImpl->attentionHistory;
    float memoryFactor = pImpl->workingMemoryHistory;
    
    // Attention enhances plasticity for attended stimuli
    float attentionPlasticity = std::min(1.0f, attentionFactor * pImpl->attentionGain);
    
    // Working memory capacity influences plasticity
    float memoryCapacity = std::tanh(memoryFactor * pImpl->workingMemoryCapacity);
    
    // Goal relevance modulates plasticity
    float goalPlasticity = pImpl->eventHistory["goal_relevance"];
    
    // Combined plasticity factor
    float plasticityFactor = 1.0f + attentionPlasticity * 0.4f + 
                           memoryCapacity * 0.3f + 
                           goalPlasticity * 0.3f;
    
    return std::clamp(plasticityFactor, 0.5f, 2.5f);
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

float Acetylcholine::getPeak() const {
    return pImpl->peak;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate attention and memory signals
    float attentionIntegration = pImpl->attentionHistory * pImpl->integrationRate * dtFloat;
    float memoryIntegration = pImpl->workingMemoryHistory * pImpl->integrationRate * 0.5f * dtFloat;
    
    pImpl->level += attentionIntegration + memoryIntegration;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update history
    pImpl->attentionHistory *= pImpl->historyDecay;
    pImpl->workingMemoryHistory *= pImpl->historyDecay;
}

void Acetylcholine::signalEvent(const std::string& eventType, float magnitude) {
    if (eventType == "attention") {
        signalAttention(magnitude);
    } else if (eventType == "working_memory_load") {
        signalWorkingMemoryLoad(magnitude);
    } else if (eventType == "goal_relevance") {
        signalGoalRelevance(magnitude);
    } else {
        // Generic ACh response
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * pImpl->releaseRate * 0.3f);
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionHistory = 0.0f;
    pImpl->workingMemoryHistory = 0.0f;
    pImpl->adaptation = 0.0f;
}

void Acetylcholine::signalAttention(float attentionStrength) {
    pImpl->attentionHistory = attentionStrength;
    
    // Phasic ACh burst for salient attention
    float attentionResponse = attentionStrength * pImpl->phasicBurst;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionResponse);
}

void Acetylcholine::signalWorkingMemoryLoad(float load) {
    pImpl->workingMemoryHistory = load;
    
    // ACh modulates working memory consolidation
    float memoryModulation = load * pImpl->memoryConsolidationRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryModulation * 0.5f);
}

void Acetylcholine::signalGoalRelevance(float relevance) {
    pImpl->eventHistory["goal_relevance"] = relevance;
    
    // Goal-relevant stimuli trigger ACh response
    float goalResponse = relevance * pImpl->phasicBurst * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + goalResponse);
}

// Norepinephrine implementation
struct Norepinephrine::Impl : NeuromodulatorBase {
    // NE-specific parameters
    float tonicLevel = 0.05f;
    float phasicBurst = 1.8f;
    float arousalGain = 1.0f;
    float stressSensitivity = 0.7f;
    float vigilanceThreshold = 0.3f;
    float taskDifficultyWeight = 0.6f;
    
    // History tracking
    float arousalHistory = 0.0f;
    float stressHistory = 0.0f;
    float vigilanceHistory = 0.0f;
    
    Impl() {
        baseline = tonicLevel;
        level = baseline;
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE-modulated plasticity: enhances arousal and stress-related learning
    
    float arousalFactor = pImpl->arousalHistory;
    float stressFactor = pImpl->stressHistory;
    
    // Arousal enhances plasticity for emotionally salient events
    float arousalPlasticity = arousalFactor * pImpl->arousalGain;
    
    // Stress can enhance or impair plasticity depending on intensity
    float stressPlasticity = stressFactor * pImpl->stressSensitivity;
    if (stressFactor > 0.8f) {
        stressPlasticity *= 0.5f; // High stress impairs plasticity
    }
    
    // Vigilance modulates plasticity for task-relevant stimuli
    float vigilancePlasticity = pImpl->vigilanceHistory;
    
    // Combined plasticity factor
    float plasticityFactor = 1.0f + arousalPlasticity * 0.3f + 
                           stressPlasticity * 0.3f + 
                           vigilancePlasticity * 0.4f;
    
    return std::clamp(plasticityFactor, 0.3f, 3.0f);
}

float Norepinephrine::getBaseline() const {
    return pImpl->baseline;
}

float Norepinephrine::getPeak() const {
    return pImpl->peak;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate arousal and stress signals
    float arousalIntegration = pImpl->arousalHistory * pImpl->integrationRate * dtFloat;
    float stressIntegration = pImpl->stressHistory * pImpl->integrationRate * 0.7f * dtFloat;
    
    pImpl->level += arousalIntegration + stressIntegration;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update history
    pImpl->arousalHistory *= pImpl->historyDecay;
    pImpl->stressHistory *= pImpl->historyDecay;
    pImpl->vigilanceHistory *= pImpl->historyDecay;
}

void Norepinephrine::signalEvent(const std::string& eventType, float magnitude) {
    if (eventType == "stress") {
        signalStress(magnitude);
    } else if (eventType == "arousal") {
        signalArousal(magnitude);
    } else if (eventType == "task_difficulty") {
        signalTaskDifficulty(magnitude);
    } else {
        // Generic NE response
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * pImpl->releaseRate * 0.4f);
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalHistory = 0.0f;
    pImpl->stressHistory = 0.0f;
    pImpl->vigilanceHistory = 0.0f;
    pImpl->adaptation = 0.0f;
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressHistory = stressLevel;
    
    // Stress response is phasic
    float stressResponse = stressLevel * pImpl->phasicBurst;
    if (stressLevel > 0.5f) {
        // High stress causes dip (fight-or-flight response)
        pImpl->level = std::max(pImpl->baseline, pImpl->level - stressResponse * 0.5f);
    } else {
        // Moderate stress causes increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse * 0.3f);
    }
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousalHistory = arousalLevel;
    
    // Arousal response is phasic
    float arousalResponse = arousalLevel * pImpl->phasicBurst * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalResponse);
}

void Norepinephrine::signalTaskDifficulty(float difficulty) {
    pImpl->vigilanceHistory = std::min(1.0f, difficulty * pImpl->vigilanceThreshold);
    
    // Task difficulty increases NE for vigilance
    float difficultyResponse = difficulty * pImpl->taskDifficultyWeight * pImpl->phasicBurst * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + difficultyResponse);
}

// Serotonin implementation
struct Serotonin::Impl : NeuromodulatorBase {
    // Serotonin-specific parameters
    float tonicLevel = 0.02f;
    float phasicBurst = 1.2f;
    float moodGain = 1.0f;
    float punishmentSensitivity = 0.8f;
    float socialValueWeight = 0.7f;
    float toneWeight = 0.4f;
    
    // History tracking
    float moodHistory = 0.0f;
    float socialValueHistory = 0.0f;
    float toneHistory = 0.0f;
    
    Impl() {
        baseline = tonicLevel;
        level = baseline;
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
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin-modulated plasticity: mood and social behavior influence learning
    
    float moodFactor = pImpl->moodHistory;
    float socialFactor = pImpl->socialValueHistory;
    
    // Low mood can impair plasticity (depression effect)
    float moodPlasticity = (1.0f - moodFactor) * 0.5f;
    if (moodFactor > 0.7f) {
        moodPlasticity *= 0.3f; // Very low mood severely impairs learning
    }
    
    // Social value can enhance plasticity
    float socialPlasticity = socialFactor * pImpl->socialValueWeight;
    
    // Behavioral tone modulates plasticity
    float tonePlasticity = pImpl->toneHistory * pImpl->toneWeight;
    
    // Combined plasticity factor
    float plasticityFactor = 1.0f - moodPlasticity * 0.3f + 
                           socialPlasticity * 0.4f + 
                           tonePlasticity * 0.3f;
    
    return std::clamp(plasticityFactor, 0.2f, 2.5f);
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

float Serotonin::getPeak() const {
    return pImpl->peak;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate mood and social signals
    float moodIntegration = pImpl->moodHistory * pImpl->integrationRate * dtFloat;
    float socialIntegration = pImpl->socialValueHistory * pImpl->integrationRate * 0.6f * dtFloat;
    float toneIntegration = pImpl->toneHistory * pImpl->integrationRate * 0.5f * dtFloat;
    
    pImpl->level += moodIntegration + socialIntegration + toneIntegration;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update history
    pImpl->moodHistory *= pImpl->historyDecay;
    pImpl->socialValueHistory *= pImpl->historyDecay;
    pImpl->toneHistory *= pImpl->historyDecay;
}

void Serotonin::signalEvent(const std::string& eventType, float magnitude) {
    if (eventType == "social_feedback") {
        signalSocialFeedback(magnitude);
    } else if (eventType == "punishment_intensity") {
        signalPunishmentIntensity(magnitude);
    } else if (eventType == "overtone_behavior") {
        signalOvertoneBehavior(magnitude);
    } else {
        // Generic 5-HT response
        pImpl->level = std::min(pImpl->peak, pImpl->level + magnitude * pImpl->releaseRate * 0.2f);
    }
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodHistory = 0.0f;
    pImpl->socialValueHistory = 0.0f;
    pImpl->toneHistory = 0.0f;
    pImpl->adaptation = 0.0f;
}

void Serotonin::signalSocialFeedback(float socialValue) {
    pImpl->socialValueHistory = socialValue;
    
    // Social feedback response
    float socialResponse = socialValue * pImpl->phasicBurst * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialResponse);
    
    // Positive social value improves mood
    pImpl->moodHistory = socialValue * 0.5f;
}

void Serotonin::signalPunishmentIntensity(float intensity) {
    pImpl->moodHistory = 1.0f - intensity; // Punishement reduces mood
    
    // Punishment response can cause dip
    float punishmentResponse = intensity * pImpl->punishmentSensitivity * pImpl->phasicBurst * 0.8f;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - punishmentResponse);
}

void Serotonin::signalOvertoneBehavior(float tone) {
    pImpl->toneHistory = tone;
    
    // Behavioral tone response
    float toneResponse = tone * pImpl->phasicBurst * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + toneResponse);
}

} // namespace nlm
