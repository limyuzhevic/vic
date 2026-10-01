#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float timeConstant;
    float predictionError;
    bool enabled;
    float lastReward;
    float lastPredictionError;
    
    // Time integration
    float integral;
    float derivative;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), releaseRate(1.5f),
             learningRate(0.01f), timeConstant(100.0f), predictionError(0.0f), enabled(true),
             lastReward(0.0f), lastPredictionError(0.0f), integral(0.0f), derivative(0.0f) {}
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
    // Real dopamine modulation based on level
    // Higher dopamine increases plasticity up to a point, but excessive dopamine can impair learning
    if (pImpl->level < 0.3f) {
        return 0.5f + 1.5f * pImpl->level;  // Low to moderate: enhancement
    } else if (pImpl->level < 0.7f) {
        return 1.0f;  // Optimal range
    } else {
        return 1.0f - 0.5f * (pImpl->level - 0.7f) / 0.3f;  // High: impairment
    }
}

float Dopamine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Dopamine::getPeakLevel() const {
    return pImpl->peak;
}

bool Dopamine::isActive() const {
    return pImpl->enabled && pImpl->level > pImpl->baseline * 1.5f;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->integral = 0.0f;
    pImpl->derivative = 0.0f;
    pImpl->lastReward = 0.0f;
    pImpl->lastPredictionError = 0.0f;
}

void Dopamine::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    if (!pImpl->enabled) return;
    
    // Store current inputs
    pImpl->lastPredictionError = predictionError;
    pImpl->lastReward = reward;
    
    // Dopamine response to prediction error (temporal difference learning)
    // Positive prediction error = reward prediction error (learning)
    // Negative prediction error = surprise (increased exploration)
    float tdError = predictionError;
    
    // Integration of prediction error over time (reservoir computation)
    pImpl->integral += tdError * dt * 0.001f;  // Scale dt to appropriate units
    pImpl->integral = std::clamp(pImpl->integral, -1.0f, 1.0f);
    
    // Decay of dopamine level towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 0.001f);
    
    // Response to reward (burst firing)
    if (reward > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * dt * 0.001f);
    }
    
    // Response to prediction error (sustained firing)
    if (std::abs(tdError) > 0.01f) {
        float response = pImpl->learningRate * std::abs(tdError);
        pImpl->level += (tdError > 0 ? response : -response) * dt * 0.001f;
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
    
    // Novelty modulation (attention-grabbing)
    if (novelty > 0.5f) {
        float noveltyResponse = 0.1f * novelty;
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyResponse * dt * 0.001f);
    }
    
    // Cap at peak level
    pImpl->level = std::min(pImpl->level, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    if (reward <= 0.0f) return;
    
    // Burst response to reward delivery
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.1f);
    pImpl->lastReward = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    if (std::abs(error) < 0.001f) return;
    
    // Sustained response to prediction error
    pImpl->lastPredictionError = error;
    
    // Update level based on prediction error
    float response = pImpl->learningRate * std::abs(error);
    pImpl->level += (error > 0 ? response : -response) * 0.1f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

float Dopamine::getLastPredictionError() const {
    return pImpl->lastPredictionError;
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

void Dopamine::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 1000f);
    pImpl->decayRate = 1.0f / pImpl->timeConstant;
}

float Dopamine::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Dopamine::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    if (!enabled) {
        reset();
    }
}

bool Dopamine::isEnabled() const {
    return pImpl->enabled;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float timeConstant;
    float attentionGain;
    float learningModulation;
    bool enabled;
    
    // Sensory input history
    float lastSensoryInput;
    float integral;
    
    Impl() : level(0.0f), baseline(0.02f), peak(0.8f), decayRate(0.2f), timeConstant(50.0f),
             attentionGain(2.0f), learningModulation(1.5f), enabled(true),
             lastSensoryInput(0.0f), integral(0.0f) {}
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
    // ACh modulates learning rate based on attentional demand
    // Higher ACh increases plasticity for attended stimuli
    return 1.0f + pImpl->learningModulation * pImpl->level;
}

float Acetylcholine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Acetylcholine::getPeakLevel() const {
    return pImpl->peak;
}

bool Acetylcholine::isActive() const {
    return pImpl->enabled && pImpl->level > pImpl->baseline * 2.0f;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->integral = 0.0f;
    pImpl->lastSensoryInput = 0.0f;
}

void Acetylcholine::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    if (!pImpl->enabled) return;
    
    // ACh increases with unexpected events (prediction errors)
    if (std::abs(predictionError) > 0.01f) {
        float errorResponse = 0.5f * std::abs(predictionError);
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorResponse * dt * 0.001f);
    }
    
    // ACh increases with novel or salient stimuli
    if (novelty > 0.3f) {
        float noveltyResponse = 0.3f * novelty;
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyResponse * dt * 0.001f);
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 0.001f);
}

void Acetylcholine::signalAttention(float attention) {
    if (attention <= 0.0f) return;
    
    pImpl->lastSensoryInput = attention;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * 0.3f);
}

void Acetylcholine::signalSensoryInput(float inputStrength) {
    if (inputStrength <= 0.0f) return;
    
    // Integrate sensory input over time
    pImpl->integral += inputStrength * dt * 0.001f;
    pImpl->integral = std::min(pImpl->integral, 1.0f);
    
    // Boost ACh level for strong sensory input
    pImpl->level = std::min(pImpl->peak, pImpl->level + inputStrength * 0.2f * dt * 0.001f);
    pImpl->lastSensoryInput = inputStrength;
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->level * pImpl->attentionGain;
}

void Acetylcholine::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 1000f);
    pImpl->decayRate = 1.0f / pImpl->timeConstant;
}

float Acetylcholine::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Acetylcholine::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    if (!enabled) {
        reset();
    }
}

bool Acetylcholine::isEnabled() const {
    return pImpl->enabled;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float timeConstant;
    float arousalGain;
    float vigilanceGain;
    float adaptationRate;
    bool enabled;
    
    // Stress and arousal signals
    float lastStress;
    float integral;
    float lastArousal;
    
    Impl() : level(0.0f), baseline(0.01f), peak(0.6f), decayRate(0.15f), timeConstant(80.0f),
             arousalGain(3.0f), vigilanceGain(2.0f), adaptationRate(0.05f), enabled(true),
             lastStress(0.0f), integral(0.0f), lastArousal(0.0f) {}
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
    // NE modulates attention and arousal, affecting plasticity indirectly
    // Moderate NE enhances learning, high NE can impair it ( Yerkes-Dodson law )
    if (pImpl->level < 0.3f) {
        return 0.8f + 1.2f * pImpl->level;  // Low to moderate: enhancement
    } else {
        return 1.2f - 0.5f * pImpl->level;  // High: impairment
    }
}

float Norepinephrine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Norepinephrine::getPeakLevel() const {
    return pImpl->peak;
}

bool Norepinephrine::isActive() const {
    return pImpl->enabled && pImpl->level > pImpl->baseline * 2.5f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->integral = 0.0f;
    pImpl->lastStress = 0.0f;
    pImpl->lastArousal = 0.0f;
}

void Norepinephrine::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    if (!pImpl->enabled) return;
    
    // NE increases with stress and uncertainty
    if (novelty > 0.2f) {
        float noveltyResponse = 0.4f * novelty;
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyResponse * dt * 0.001f);
    }
    
    // NE responds to prediction errors (threat detection)
    if (std::abs(predictionError) > 0.01f) {
        float errorResponse = 0.3f * std::abs(predictionError);
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorResponse * dt * 0.001f);
    }
    
    // Slow decay (NE has longer time constant than DA)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 0.001f);
}

void Norepinephrine::signalArousal(float arousal) {
    if (arousal <= 0.0f) return;
    
    pImpl->lastArousal = arousal;
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * 0.2f);
}

void Norepinephrine::signalStress(float stress) {
    if (stress <= 0.0f) return;
    
    pImpl->lastStress = stress;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * 0.25f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->level * pImpl->arousalGain;
}

void Norepinephrine::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 1000f);
    pImpl->decayRate = 1.0f / pImpl->timeConstant;
}

float Norepinephrine::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Norepinephrine::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.01f, 0.2f);
}

float Norepinephrine::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void Norepinephrine::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    if (!enabled) {
        reset();
    }
}

bool Norepinephrine::isEnabled() const {
    return pImpl->enabled;
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float timeConstant;
    float moodGain;
    float inhibitionFactor;
    bool enabled;
    
    // Mood and social context
    float lastMood;
    float lastSocialContext;
    float integral;
    
    Impl() : level(0.0f), baseline(0.03f), peak(0.7f), decayRate(0.18f), timeConstant(60.0f),
             moodGain(2.5f), inhibitionFactor(0.8f), enabled(true),
             lastMood(0.0f), lastSocialContext(0.0f), integral(0.0f) {}
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
    // 5-HT modulates mood and impulsivity, affecting plasticity
    // Higher 5-HT generally reduces impulsivity and stabilizes learning
    return 1.0f / (1.0f + pImpl->inhibitionFactor * pImpl->level);
}

float Serotonin::getBaselineLevel() const {
    return pImpl->baseline;
}

float Serotonin::getPeakLevel() const {
    return pImpl->peak;
}

bool Serotonin::isActive() const {
    return pImpl->enabled && pImpl->level > pImpl->baseline * 3.0f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->integral = 0.0f;
    pImpl->lastMood = 0.0f;
    pImpl->lastSocialContext = 0.0f;
}

void Serotonin::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    if (!pImpl->enabled) return;
    
    // 5-HT increases with positive mood and social context
    if (reward > 0.0f) {
        float rewardResponse = 0.3f * reward;
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse * dt * 0.001f);
    }
    
    if (novelty > 0.1f) {
        float noveltyResponse = 0.2f * novelty;
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyResponse * dt * 0.001f);
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 0.001f);
}

void Serotonin::signalMood(float mood) {
    if (mood <= 0.0f) return;
    
    pImpl->lastMood = mood;
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.15f);
}

void Serotonin::signalSocialContext(float context) {
    if (context <= 0.0f) return;
    
    pImpl->lastSocialContext = context;
    pImpl->level = std::min(pImpl->peak, pImpl->level + context * 0.1f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->level * pImpl->moodGain;
}

float Serotonin::getImpulsivityLevel() const {
    // Impulsivity is inversely related to 5-HT levels
    return 1.0f - pImpl->level * pImpl->inhibitionFactor;
}

void Serotonin::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 1000f);
    pImpl->decayRate = 1.0f / pImpl->timeConstant;
}

float Serotonin::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Serotonin::setInhibitionFactor(float factor) {
    pImpl->inhibitionFactor = std::clamp(factor, 0.1f, 2.0f);
}

float Serotonin::getInhibitionFactor() const {
    return pImpl->inhibitionFactor;
}

void Serotonin::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    if (!enabled) {
        reset();
    }
}

bool Serotonin::isEnabled() const {
    return pImpl->enabled;
}

// NeuromodulationSystem implementation
struct NeuromodulationSystem::Impl {
    std::vector<std::shared_ptr<Neuromodulator>> neuromodulators;
    float combinedPlasticityFactor;
    bool enabled;
    
    Impl() : combinedPlasticityFactor(1.0f), enabled(true) {}
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(std::make_unique<Impl>()) {}

NeuromodulationSystem::~NeuromodulationSystem() = default;

void NeuromodulationSystem::addNeuromodulator(std::shared_ptr<Neuromodulator> neuromodulator) {
    if (neuromodulator) {
        pImpl->neuromodulators.push_back(neuromodulator);
    }
}

std::shared_ptr<Neuromodulator> NeuromodulationSystem::getNeuromodulator(const std::string& name) const {
    for (auto& neuromodulator : pImpl->neuromodulators) {
        if (std::string(neuromodulator->getName()) == name) {
            return neuromodulator;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Neuromodulator>>& NeuromodulationSystem::getAllNeuromodulators() const {
    return pImpl->neuromodulators;
}

void NeuromodulationSystem::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    if (!pImpl->enabled) return;
    
    float totalPlasticityFactor = 1.0f;
    
    for (auto& neuromodulator : pImpl->neuromodulators) {
        neuromodulator->update(dt, predictionError, reward, novelty);
        totalPlasticityFactor *= neuromodulator->getPlasticityFactor();
    }
    
    pImpl->combinedPlasticityFactor = totalPlasticityFactor;
}

void NeuromodulationSystem::reset() {
    for (auto& neuromodulator : pImpl->neuromodulators) {
        neuromodulator->reset();
    }
    pImpl->combinedPlasticityFactor = 1.0f;
}

float NeuromodulationSystem::getCombinedPlasticityFactor() const {
    return pImpl->combinedPlasticityFactor;
}

void NeuromodulationSystem::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
    if (!enabled) {
        reset();
    }
}

bool NeuromodulationSystem::isEnabled() const {
    return pImpl->enabled;
}

} // namespace nlm
