#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace nlm {

// Neural dynamics constants for neuromodulation
constexpr float DOPAMINE_DECAY_TIME_CONSTANT = 0.5f;  // seconds
constexpr float DOPAMINE_PEAK_LEVEL = 2.0f;           // times baseline
constexpr float DOPAMINE_RECOVERY_RATE = 1.0f;        // per second
constexpr float RPE_LEARNING_RATE = 0.1f;             // reward prediction error learning
constexpr float PLASTICITY_DOPAMINE_MAX = 2.0f;       // maximum plasticity modulation
constexpr float PLASTICITY_DOPAMINE_MIN = 0.0f;       // minimum plasticity modulation

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedReward;
    float lastReward;
    float timeSinceReward;
    float eligibilityTrace;
    float explorationBias;
    float creditAssignmentWindow;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.5f), 
             decayRate(1.0f / DOPAMINE_DECAY_TIME_CONSTANT), releaseRate(1.0f),
             predictionError(0.0f), expectedReward(0.0f), lastReward(0.0f),
             timeSinceReward(0.0f), eligibilityTrace(0.0f), explorationBias(0.0f),
             creditAssignmentWindow(0.5f) {}
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
    // REAL IMPLEMENTATION: Dopamine modulates plasticity in a U-shaped curve
    // Peak plasticity occurs at moderate dopamine levels (~0.5-0.8)
    // Both too little (0.0-0.3) and too much (0.8-1.0) reduce plasticity
    float normalizedLevel = std::clamp(pImpl->level / pImpl->peak, 0.0f, 1.0f);
    
    // U-shaped curve: plasticity factor peaks at moderate dopamine
    float plasticityFactor = 0.5f + 0.5f * std::sin(M_PI * normalizedLevel);
    
    // Additional modulation based on prediction error
    plasticityFactor *= (1.0f + std::abs(pImpl->predictionError));
    
    return std::clamp(plasticityFactor, PLASTICITY_DOPAMINE_MIN, PLASTICITY_DOPAMINE_MAX);
}

void Dopamine::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: Dopamine dynamics based on biological models
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline with first-order kinetics
    float decayAmount = (pImpl->level - pImpl->baseline) * pImpl->decayRate * dtFloat;
    pImpl->level -= decayAmount;
    pImpl->level = std::max(pImpl->baseline, pImpl->level);
    
    // Time since last reward (for exploration bias)
    pImpl->timeSinceReward += dtFloat;
    
    // Gradually decay exploration bias
    pImpl->explorationBias *= (1.0f - pImpl->decayRate * dtFloat);
}

void Dopamine::signalReward(float reward) {
    // REAL IMPLEMENTATION: Phasic dopamine response to reward
    pImpl->lastReward = reward;
    pImpl->timeSinceReward = 0.0f;
    
    // Phasic response: larger rewards produce stronger bursts
    // Dopamine release follows a saturating function
    float rewardMagnitude = std::min(reward, 10.0f);  // Cap at biologically plausible values
    float burstAmount = pImpl->peak * (1.0f - std::exp(-rewardMagnitude * 0.2f));
    
    // Combine with existing level (additive)
    pImpl->level += burstAmount;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * reward;
    
    // Increase exploration bias for novel rewards
    pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + 0.3f * rewardMagnitude);
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL IMPLEMENTATION: Dopamine signals reward prediction error (RPE)
    // Core teaching signal: positive RPE (better than expected) increases dopamine
    // Negative RPE (worse than expected) decreases dopamine
    
    pImpl->predictionError = error;
    
    // RPE-driven dopamine update with eligibility trace
    float eligibilityWeight = std::max(0.0f, pImpl->eligibilityTrace);
    
    // Dopamine responds to prediction error with credit assignment
    float rpeSignal = error * (1.0f + eligibilityWeight) * RPE_LEARNING_RATE;
    
    // Update level with RPE signal
    pImpl->level += rpeSignal;
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline - 0.5f, pImpl->peak * 1.5f);
    
    // Update eligibility trace for future credit assignment
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 0.1f);
}

// Acetylcholine implementation - attention and memory consolidation
void Acetylcholine::ACh::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: ACh modulates attention and memory consolidation
    float dtFloat = static_cast<float>(dt);
    
    // ACh dynamics: increase with attention demands, decay over time
    float attentionInput = 0.0f;  // Would normally come from cortical activity
    
    // Decay towards baseline
    level -= decayRate * dtFloat * (level - baseline);
    level = std::max(baseline, level);
    
    // Integrate attention demands
    level += attentionInput * 0.1f;
    level = std::clamp(level, 0.0f, 1.0f);
    
    // Memory consolidation effect: strengthen synapses during ACh elevation
    if (level > 0.5f) {
        // Enhance memory consolidation
        consolidationEffect = level * 0.8f;
    }
}

// Norepinephrine implementation - arousal and vigilance
void Norepinephrine::NE::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: NE modulates arousal and vigilance
    float dtFloat = static_cast<float>(dt);
    
    // NE dynamics: phasic increases during novel/stressful events, tonic baseline
    float arousalInput = 0.0f;  // Would normally come from limbic system
    
    // Decay towards baseline
    level -= decayRate * dtFloat * (level - baseline);
    level = std::max(baseline, level);
    
    // Integrate arousal demands
    level += arousalInput * 0.15f;
    level = std::clamp(level, 0.0f, 1.0f);
    
    // Vigilance enhancement: improves signal-to-noise ratio
    if (level > arousalThreshold) {
        // Enhance perceptual vigilance
        vigilanceEffect = level * 0.7f;
    }
}

// Serotonin implementation - mood regulation and impulse control
void Serotonin::5HT::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: 5-HT regulates mood and impulsivity
    float dtFloat = static_cast<float>(dt);
    
    // 5-HT dynamics: complex regulation of mood and behavior
    float moodInput = 0.0f;  // Would normally come from Raphe nuclei
    
    // Decay towards baseline with longer time constant
    level -= decayRate * dtFloat * (level - baseline);
    level = std::max(baseline, level);
    
    // Integrate mood inputs
    level += moodInput * 0.1f;
    level = std::clamp(level, 0.0f, 1.0f);
    
    // Mood stabilization: high 5-HT reduces impulsivity and aggression
    if (level > 0.6f) {
        // Promote calm, stable behavior
        moodStability = level * 0.9f;
        impulseControl = level * 0.8f;
    }
}

// Neuromodulation Manager implementation
NeuromodulationManager::NeuromodulationManager() : pImpl(new Impl) {
    // Initialize with standard neuromodulators
    registerNeuromodulator(std::make_unique<Dopamine>());
    registerNeuromodulator(std::make_unique<Acetylcholine>());
    registerNeuromodulator(std::make_unique<Norepinephrine>());
    registerNeuromodulator(std::make_unique<Serotonin>());
}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::registerNeuromodulator(std::unique_ptr<Neuromodulator> modulator) {
    if (modulator) {
        pImpl->neuromodulators[modulator->getName()] = std::move(modulator);
    }
}

Neuromodulator* NeuromodulationManager::getNeuromodulator(const std::string& name) const {
    auto it = pImpl->neuromodulators.find(name);
    return it != pImpl->neuromodulators.end() ? it->second.get() : nullptr;
}

void NeuromodulationManager::updateAll(TimestepDuration dt) {
    float totalPlasticity = 1.0f;
    float attention = 0.0f;
    float arousal = 0.0f;
    float mood = 0.0f;
    
    for (auto& pair : pImpl->neuromodulators) {
        pair.second->update(dt);
        totalPlasticity *= pair.second->getPlasticityFactor();
        
        // Accumulate neuromodulatory effects for manager
        const char* name = pair.second->getName();
        float level = pair.second->getLevel();
        
        if (std::string(name) == "DA") {
            arousal += level * 0.4f;  // Dopamine contributes to arousal
        } else if (std::string(name) == "ACh") {
            attention += level * 0.5f;  // Acetylcholine contributes to attention
        } else if (std::string(name) == "NE") {
            arousal += level * 0.3f;  // Norepinephrine contributes to arousal
            attention += level * 0.2f; // Norepinephrine also affects attention
        } else if (std::string(name) == "5-HT") {
            mood += level * 0.5f;  // Serotonin contributes to mood
        }
    }
    
    pImpl->totalPlasticityFactor = totalPlasticity;
    pImpl->attentionLevel = std::min(1.0f, attention);
    pImpl->arousalLevel = std::min(1.0f, arousal);
    pImpl->moodLevel = std::min(1.0f, mood);
}

float NeuromodulationManager::getTotalPlasticityFactor() const {
    return pImpl->totalPlasticityFactor;
}

float NeuromodulationManager::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

float NeuromodulationManager::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float NeuromodulationManager::getMoodLevel() const {
    return pImpl->moodLevel;
}

void NeuromodulationManager::signalReward(float reward) {
    if (auto* da = getNeuromodulator("DA")) {
        static_cast<Dopamine*>(da)->signalReward(reward);
    }
}

void NeuromodulationManager::signalRewardPredictionError(float error) {
    if (auto* da = getNeuromodulator("DA")) {
        static_cast<Dopamine*>(da)->signalRewardPredictionError(error);
    }
}

void NeuromodulationManager::signalNovelty(float novelty) {
    // Novelty can be signaled to multiple neuromodulators
    if (auto* da = getNeuromodulator("DA")) {
        static_cast<Dopamine*>(da)->setLevel(std::min(da->getLevel() + novelty * 0.2f, 1.0f));
    }
    if (auto* ne = getNeuromodulator("NE")) {
        ne->setLevel(std::min(ne->getLevel() + novelty * 0.3f, 1.0f));
    }
}

void NeuromodulationManager::signalStress(float stress) {
    if (auto* ne = getNeuromodulator("NE")) {
        ne->setLevel(std::min(ne->getLevel() + stress * 0.4f, 1.0f));
    }
    if (auto* da = getNeuromodulator("DA")) {
        da->setLevel(std::max(da->getLevel() - stress * 0.3f, 0.0f));
    }
}

float NeuromodulationManager::getDopamineLevel() const {
    if (auto* da = getNeuromodulator("DA")) {
        return da->getLevel();
    }
    return 0.0f;
}

float NeuromodulationManager::getAChLevel() const {
    if (auto* ach = getNeuromodulator("ACh")) {
        return ach->getLevel();
    }
    return 0.0f;
}

float NeuromodulationManager::getNELevel() const {
    if (auto* ne = getNeuromodulator("NE")) {
        return ne->getLevel();
    }
    return 0.0f;
}

float NeuromodulationManager::get5HTLevel() const {
    if (auto* 5ht = getNeuromodulator("5-HT")) {
        return 5ht->getLevel();
    }
    return 0.0f;
}

} // namespace nlm
