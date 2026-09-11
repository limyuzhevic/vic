#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedValue;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), 
             releaseRate(1.0f), predictionError(0.0f), expectedValue(0.0f), 
             learningRate(0.1f) {}
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
    float basePlasticity = 0.5f;
    float activityModulation = pImpl->level * 0.6f;
    float peContribution = std::abs(pImpl->predictionError) * 0.3f;
    float plasticityFactor = basePlasticity + activityModulation + peContribution;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    float timeScaled = static_cast<float>(dt);
    
    if (pImpl->predictionError != 0.0f) {
        float phasicAmplitude = std::min(std::abs(pImpl->predictionError), 1.0f);
        pImpl->level += phasicAmplitude * pImpl->releaseRate * 0.5f;
    }
    
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * timeScaled);
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    pImpl->expectedValue = (1.0f - pImpl->learningRate) * pImpl->expectedValue + 
                          pImpl->learningRate * (pImpl->level * 2.0f - 1.0f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
}

void Dopamine::signalReward(float reward) {
    float normalizedReward = std::clamp(reward, 0.0f, 1.0f);
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + normalizedReward * pImpl->releaseRate);
    
    rewardHistory_.push_back(normalizedReward);
    
    if (rewardHistory_.size() > 1000) {
        rewardHistory_.erase(rewardHistory_.begin());
    }
    
    if (normalizedReward > 0.5f) {
        float prediction = pImpl->expectedValue / 2.0f;
        pImpl->predictionError = normalizedReward - prediction;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
    
    rewardHistory_.push_back(error > 0 ? 1.0f : 0.0f);
    
    if (rewardHistory_.size() > 1000) {
        rewardHistory_.erase(rewardHistory_.begin());
    }
}

void Dopamine::computePredictionError(float predicted, float actual) {
    pImpl->predictionError = actual - predicted;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

void Dopamine::setExpectedValue(float value) {
    pImpl->expectedValue = value;
}

void Dopamine::applyToBrain(Brain* brain) const {
    if (!brain) return;
    
    float plasticityFactor = getPlasticityFactor();
    
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float ltpWeight = 0.01f * plasticityFactor * (1.0f + getPredictionError());
        float ltdWeight = 0.012f * plasticityFactor * std::max(0.0f, 1.0f - getPredictionError());
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
    }
    
    auto* hebbian = brain->getHebbian();
    if (hebbian) {
        float hebbianModulation = plasticityFactor * 0.5f;
    }
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBaseline;
    float memoryBaseline;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.02f),
             releaseRate(0.8f), attentionBaseline(0.5f), memoryBaseline(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl), attentionLevel_(0.0f), 
                                memoryEnhancement_(1.0f) {}

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
    float baseline = 0.5f;
    float modulation = (1.0f - pImpl->level) * 0.4f;
    return std::clamp(baseline + modulation, 0.1f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeScaled = static_cast<float>(dt);
    
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->decayRate * timeScaled);
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    attentionLevel_ = pImpl->level * 0.8f + 0.2f;
    
    float optimalACh = 0.6f;
    memoryEnhancement_ = 1.0f - std::abs(pImpl->level - optimalACh) * 2.0f;
    memoryEnhancement_ = std::clamp(memoryEnhancement_, 0.2f, 1.5f);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    attentionLevel_ = pImpl->attentionBaseline;
    memoryEnhancement_ = pImpl->memoryBaseline;
}

void Acetylcholine::modulateAttention(float attentionLevel) {
    attentionLevel_ = std::clamp(attentionLevel, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionModulation() const {
    return attentionLevel_;
}

void Acetylcholine::enhanceEncoding(float encodingStrength) {
    encodingStrength = std::clamp(encodingStrength, 0.0f, 1.0f);
    
    encodingHistory_.push_back(encodingStrength);
    
    if (encodingHistory_.size() > 1000) {
        encodingHistory_.erase(encodingHistory_.begin());
    }
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + encodingStrength * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::enhanceMemoryConsolidation(float consolidationFactor) {
    consolidationFactor = std::clamp(consolidationFactor, 0.0f, 1.0f);
    
    if (consolidationFactor > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationFactor * 0.3f);
    }
}

void Acetylcholine::applyToBrain(Brain* brain) const {
    if (!brain) return;
    
    float attentionStrength = pImpl->level * 0.7f;
    
    auto* attention = brain->getAttention();
    if (attention) {
        attentionStrength = std::min(1.0f, attentionStrength * 1.5f);
    }
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.03f),
             releaseRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl), arousal_(0.0f), 
                                   learningRate_(0.1f) {}

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
    float baseline = 0.5f;
    float optimalArousal = 0.4f;
    float arousalModulation = -std::pow(std::abs(pImpl->level - optimalArousal), 2.0f) * 0.6f;
    return std::clamp(baseline + arousalModulation, 0.2f, 1.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeScaled = static_cast<float>(dt);
    
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->decayRate * timeScaled);
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    arousal_ = pImpl->level * 0.8f + 0.2f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    arousal_ = pImpl->baseline;
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * pImpl->releaseRate * 0.5f);
}

float Norepinephrine::enhanceSignalToNoiseRatio(float baseSignal, float noise) const {
    if (noise <= 0.0f) {
        return baseSignal;
    }
    
    float enhancedSignal = baseSignal + (baseSignal - noise) * arousal_ * 0.5f;
    
    signalNoiseHistory_.push_back(enhancedSignal / baseSignal);
    
    if (signalNoiseHistory_.size() > 1000) {
        signalNoiseHistory_.erase(signalNoiseHistory_.begin());
    }
    
    return enhancedSignal;
}

void Norepinephrine::applyToBrain(Brain* brain) const {
    if (!brain) return;
    
    float arousalBoost = arousal_ * 0.5f;
    
    auto* attention = brain->getAttention();
    if (attention) {
        float attentionGain = 1.0f + arousalBoost * 0.5f;
    }
    
    if (learningRate_ < 0.5f) {
        learningRate_ = std::min(0.5f, learningRate_ + arousalBoost * 0.1f);
    }
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.02f),
             releaseRate(0.7f) {}
};

Serotonin::Serotonin() : pImpl(new Impl), mood_(0.5f), impulseControl_(0.5f),
                          socialInfluence_(0.5f), socialLearningStrength_(0.5f),
                          decisionBias_(0.5f) {}

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
    float baseline = 0.5f;
    float moodModulation = (mood_ - 0.5f) * 0.4f;
    float biasModulation = (decisionBias_ - 0.5f) * 0.3f;
    return std::clamp(baseline + moodModulation + biasModulation, 0.2f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    float timeScaled = static_cast<float>(dt);
    
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->decayRate * timeScaled);
    
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    mood_ = pImpl->level * 0.6f + 0.4f;
    
    impulseControl_ = 0.5f + pImpl->level * 0.4f;
    
    socialInfluence_ = 0.5f + pImpl->level * 0.3f;
    
    socialLearningStrength_ = 0.4f + pImpl->level * 0.3f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    mood_ = 0.5f;
    impulseControl_ = 0.5f;
    socialInfluence_ = 0.5f;
    socialLearningStrength_ = 0.5f;
    decisionBias_ = 0.5f;
}

void Serotonin::regulateMood(float moodLevel) {
    moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
    mood_ = moodLevel;
}

void Serotonin::controlImpulsivity(float impulseControl) {
    impulseControl_ = std::clamp(impulseControl, 0.0f, 1.0f);
}

void Serotonin::influenceSocialBehavior(float socialInfluence) {
    socialInfluence_ = std::clamp(socialInfluence, 0.0f, 1.0f);
}

void Serotonin::enhanceSocialLearning(float socialLearningStrength) {
    socialLearningStrength_ = std::clamp(socialLearningStrength, 0.0f, 1.0f);
}

void Serotonin::applyToBrain(Brain* brain) const {
    if (!brain) return;
    
    float moodModulation = (mood_ - 0.5f) * 0.5f;
    
    auto* attention = brain->getAttention();
    if (attention) {
        if (mood_ < 0.4f) {
        } else if (mood_ > 0.7f) {
        }
    }
    
    auto* prediction = brain->getPredictionSystem();
    if (prediction) {
    }
    
    float impulseControlBonus = impulseControl_ * 0.3f;
}

// Neuromodulation helper functions implementation
namespace NeuromodulationHelpers {

void ApplyAllNeuromodulators(Brain* brain, const Neuromodulator& ach, 
                             const Neuromodulator& ne, 
                             const Neuromodulator& da, 
                             const Neuromodulator& serotonin) {
    if (!brain) return;
    
    ach.applyToBrain(brain);
    ne.applyToBrain(brain);
    da.applyToBrain(brain);
    serotonin.applyToBrain(brain);
}

void ApplyNeuromodulationToExcitability(Brain* brain, 
                                        const Neuromodulator& ach, 
                                        const Neuromodulator& ne,
                                        const Neuromodulator& da) {
    if (!brain) return;
    
    ach.applyToBrain(brain);
    ne.applyToBrain(brain);
    da.applyToBrain(brain);
}

void ApplyNeuromodulationToPlasticity(Brain* brain, 
                                       const Neuromodulator& ach,
                                       const Neuromodulator& da) {
    if (!brain) return;
    
    ach.applyToBrain(brain);
    da.applyToBrain(brain);
}

void ApplyNeuromodulationToMemory(Brain* brain, 
                                   const Neuromodulator& ach,
                                   const Neuromodulator& da) {
    if (!brain) return;
    
    ach.applyToBrain(brain);
    da.applyToBrain(brain);
}

void ApplyNeuromodulationToAttention(Brain* brain, 
                                     const Neuromodulator& ach,
                                     const Neuromodulator& ne) {
    if (!brain) return;
    
    ach.applyToBrain(brain);
    ne.applyToBrain(brain);
}

void ApplyNeuromodulationToPrediction(Brain* brain,
                                       const Neuromodulator& ne,
                                       const Neuromodulator& da) {
    if (!brain) return;
    
    ne.applyToBrain(brain);
    da.applyToBrain(brain);
}

} // namespace NeuromodulationHelpers

} // namespace nlm