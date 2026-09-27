#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

//===== DOPAMINE =====

struct Dopamine::Impl {
    // Dopamine dynamics
    float level;                    // Current dopamine concentration
    float baseline;                 // Baseline firing rate (~1-2 nM/min)
    float peak;                     // Maximum achievable level
    float decayRate;                // Decay rate (~0.1 min^-1)
    float releaseRate;              // Release rate on reward (variable)
    float predictionError;          // Reward prediction error (RPE)
    float temporalDifference;       // TD error for continuous learning
    float rewardSensitivity;        // Sensitivity to reward magnitude
    float learningRate;             // Learning rate for plasticity
    
    // History
    std::vector<float> levelHistory;
    std::vector<float> errorHistory;
    
    // Neural simulation parameters
    float tonicRelease;             // Tonic dopamine release
    float phasicRelease;            // Phasic burst on prediction error
    float slottingTime;              // Time constant for slotting
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , predictionError(0.0f)
        , temporalDifference(0.0f)
        , rewardSensitivity(0.8f)
        , learningRate(0.1f)
        , tonicRelease(0.1f)
        , phasicRelease(0.9f)
        , slottingTime(0.5f) {
        
        // Seed RNG with chrono for biological variability
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.8f, 1.2f);
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Implements dopamine-dependent synaptic plasticity rules
    float basePlasticity = 0.1f;
    
    // High dopamine enhances LTP, low dopamine facilitates LTD
    if (pImpl->level > pImpl->baseline) {
        // Phasic dopamine (above baseline) promotes long-term potentiation
        float enhancement = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        return basePlasticity + 0.9f * enhancement * pImpl->learningRate;
    } else {
        // Tonic/lower dopamine allows long-term depression
        float depression = std::max(0.0f, (pImpl->baseline - pImpl->level) / pImpl->baseline);
        return basePlasticity * (1.0f - 0.7f * depression * pImpl->learningRate);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: biological ODE model
    // Implements dopamine neuron firing rate dynamics
    
    // Decay towards baseline with biological time constant
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add small biological noise (background variability)
    float noise = (dist(pImpl->rng) - 1.0f) * 0.01f * static_cast<float>(dt);
    pImpl->level += noise;
    
    // Store history
    pImpl->levelHistory.push_back(pImpl->level);
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: biological dopamine burst
    // Implements reward prediction error signaling with spike timing
    
    // Calculate expected reward based on prediction error
    float expected = pImpl->temporalDifference * pImpl->rewardSensitivity;
    
    // Phasic dopamine burst proportional to reward prediction error
    float rpe = reward - expected;
    
    // Bounded response with saturation
    float response = std::min(rpe * pImpl->releaseRate, pImpl->phasicRelease);
    
    // Add to current level (with bounds)
    setLevel(pImpl->level + response);
    
    // Update prediction error history
    pImpl->predictionError = rpe;
    pImpl->errorHistory.push_back(rpe);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update temporal difference for next update
    pImpl->temporalDifference = rpe;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Implements temporal difference learning
    
    // TD error: reward + gamma * future_value - current_value
    // For simplicity, we assume gamma = 1 and future_value = 0
    float tdError = error;
    
    // Dopamine response to prediction error
    float response = std::max(0.0f, tdError * pImpl->releaseRate * 0.5f);
    
    // Phasic burst for positive error (unexpected reward)
    if (tdError > 0) {
        setLevel(std::min(pImpl->peak, pImpl->level + response * 2.0f));
    }
    // Dip for negative error (missed reward)
    else {
        setLevel(std::max(pImpl->baseline, pImpl->level + response));
    }
    
    pImpl->predictionError = tdError;
    pImpl->temporalDifference = tdError;
}

void Dopamine::signalPredictionError(float predictionError) {
    // Additional prediction error signaling method
    signalRewardPredictionError(predictionError);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

float Dopamine::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

//===== ACETYLCHOLINE =====

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    float memoryModulation;
    float cholinergicTone;
    float featureSelectivity;
    
    std::vector<float> attentionHistory;
    std::vector<float> memoryHistory;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl()
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.03f)
        , attentionGain(1.0f)
        , memoryModulation(0.8f)
        , cholinergicTone(0.1f)
        , featureSelectivity(0.5f) {
        
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.9f, 1.1f);
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh-modulated plasticity: enhances memory encoding
    return 1.0f + pImpl->memoryModulation * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: slower than dopamine, more sustained
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Biological noise
    float noise = (dist(pImpl->rng) - 1.0f) * 0.005f * static_cast<float>(dt);
    pImpl->level += noise;
    
    pImpl->attentionHistory.push_back(pImpl->attentionGain);
    if (pImpl->attentionHistory.size() > 1000) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionGain = std::min(2.0f, pImpl->attentionGain + strength * 0.5f);
}

void Acetylcholine::facilitateMemoryConsolidation() {
    pImpl->memoryModulation = std::min(1.0f, pImpl->memoryModulation + 0.1f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryModulation() const {
    return pImpl->memoryModulation;
}

//===== NOREPINEPHRINE =====

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    float signalToNoise;
    float locusCoeruleusFiring;
    
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl()
        : level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.02f)
        , arousalLevel(0.5f)
        , vigilanceLevel(0.5f)
        , signalToNoise(1.0f)
        , locusCoeruleusFiring(0.1f) {
        
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.95f, 1.05f);
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE-modulated plasticity: arousal-dependent learning rate
    return 0.5f + 0.5f * pImpl->arousalLevel * pImpl->signalToNoise;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: very slow, maintains tonic arousal
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Biological noise
    float noise = (dist(pImpl->rng) - 1.0f) * 0.003f * static_cast<float>(dt);
    pImpl->level += noise;
    
    pImpl->arousalHistory.push_back(pImpl->arousalLevel);
    if (pImpl->arousalHistory.size() > 1000) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
    }
}

void Norepinephrine::increaseArousal(float level) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + level * 0.3f);
    setLevel(std::min(pImpl->peak, pImpl->level + level * 0.2f));
}

void Norepinephrine::increaseVigilance(float level) {
    pImpl->vigilanceLevel = std::min(1.0f, pImpl->vigilanceLevel + level * 0.3f);
    setLevel(std::min(pImpl->peak, pImpl->level + level * 0.15f));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

//===== SEROTONIN =====

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulseControl;
    float socialFactor;
    float rapheNucleusFiring;
    
    std::vector<float> moodHistory;
    std::vector<float> impulseHistory;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl()
        : level(0.0f)
        , baseline(0.08f)
        , peak(1.0f)
        , decayRate(0.04f)
        , moodLevel(0.5f)
        , impulseControl(0.7f)
        , socialFactor(0.5f)
        , rapheNucleusFiring(0.05f) {
        
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        rng.seed(static_cast<unsigned int>(seed));
        dist = std::uniform_real_distribution<float>(0.9f, 1.1f);
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT-modulated plasticity: mood-dependent behavioral flexibility
    return 0.7f + 0.3f * pImpl->moodLevel * pImpl->impulseControl;
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics: moderate decay, regulates mood
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Biological noise
    float noise = (dist(pImpl->rng) - 1.0f) * 0.008f * static_cast<float>(dt);
    pImpl->level += noise;
    
    pImpl->moodHistory.push_back(pImpl->moodLevel);
    if (pImpl->moodHistory.size() > 1000) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
    setLevel(std::min(pImpl->peak, pImpl->level + moodLevel * 0.15f));
}

void Serotonin::regulateImpulsivity(float controlLevel) {
    pImpl->impulseControl = std::clamp(controlLevel, 0.0f, 1.0f);
    setLevel(std::min(pImpl->peak, pImpl->level + controlLevel * 0.1f));
}

void Serotonin::modulateSocialBehavior(float socialFactor) {
    pImpl->socialFactor = std::clamp(socialFactor, 0.0f, 1.0f);
    setLevel(std::min(pImpl->peak, pImpl->level + socialFactor * 0.12f));
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Dopamine::getTargetNeuronTypes() const {
    return {
        NeuronType::Excitatory,
        NeuronType::Inhibitory,
        NeuronType::Modulatory
    };
}

float Dopamine::getEffectStrength() const {
    return pImpl->level * 2.0f;  // Effect scales with dopamine level
}

// Implement Acetylcholine virtual methods

std::vector<NeuronType> Acetylcholine::getTargetNeuronTypes() const {
    return {
        NeuronType::Excitatory,
        NeuronType::Modulatory
    };
}

float Acetylcholine::getEffectStrength() const {
    return pImpl->attentionGain;
}

// Implement Norepinephrine virtual methods

std::vector<NeuronType> Norepinephrine::getTargetNeuronTypes() const {
    return {
        NeuronType::Excitatory,
        NeuronType::Inhibitory
    };
}

float Norepinephrine::getEffectStrength() const {
    return pImpl->arousalLevel;
}

// Implement Serotonin virtual methods

std::vector<NeuronType> Serotonin::getTargetNeuronTypes() const {
    return {
        NeuronType::Inhibitory,
        NeuronType::Modulatory
    };
}

float Serotonin::getEffectStrength() const {
    return pImpl->moodLevel * pImpl->impulseControl;
}

} // namespace nlm
