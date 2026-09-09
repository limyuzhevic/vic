#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// ==================== DOPAMINE ====================

struct Dopamine::Impl {
    // Signal components
    float phasic;          // Phasic dopamine component
    float tonic;           // Tonic dopamine component
    float level;           // Combined level (backward compatibility)
    
    // Baseline and reference
    float baseline;        // Tonic baseline level
    float reference;       // Reward prediction reference
    float predictionError; // Current prediction error
    
    // Temporal dynamics
    float phasicTimeConstant;  // Phasic response time constant (ms)
    float tonicTimeConstant;   // Tonic decay time constant (ms)
    float integrationFactor;   // Signal integration factor
    
    // Plasticity modulation parameters
    float learningRate;    // Learning rate for prediction
    float plasticityGain;  // Dopamine effect on plasticity
    float predictionGain;  // Prediction error effect
    
    // History tracking
    std::vector<float> rewardHistory;
    std::vector<float> predictionHistory;
    std::vector<float> phasicHistory;
    std::vector<float> tonicHistory;
    
    // State tracking
    float maxObservedReward;
    float minObservedReward;
    float accumulatedPredictionError;
    
    // Mode control
    float mode;            // 0.0 = tonic only, 1.0 = phasic only
    float phasicGain;      // Phasic response gain
    float tonicGain;       // Tonic response gain
    
    // Internal dynamics
    float temporalIntegral; // For integration
    float filteredPredictionError; // For temporal filtering
    
    Impl()
        : phasic(0.0f), tonic(0.0f), level(0.0f)
        , baseline(0.1f), reference(0.0f), predictionError(0.0f)
        , phasicTimeConstant(100.0f), tonicTimeConstant(1000.0f)
        , integrationFactor(0.95f), learningRate(0.1f), plasticityGain(1.0f)
        , predictionGain(1.0f), maxObservedReward(0.0f), minObservedReward(0.0f)
        , accumulatedPredictionError(0.0f), mode(1.0f), phasicGain(1.0f)
        , tonicGain(0.1f), temporalIntegral(0.0f), filteredPredictionError(0.0f) {}
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
    pImpl->phasic = level * pImpl->mode;  // Phasic component based on mode
    pImpl->tonic = level * (1.0f - pImpl->mode) * pImpl->tonicGain; // Tonic component
}

void Dopamine::setMode(float mode) {
    pImpl->mode = std::clamp(mode, 0.0f, 1.0f);
    updateComponents();
}

void Dopamine::setPhasicGain(float gain) {
    pImpl->phasicGain = std::max(0.1f, gain);
    updateComponents();
}

void Dopamine::setTonicGain(float gain) {
    pImpl->tonicGain = std::max(0.01f, gain);
    updateComponents();
}

void Dopamine::setTimeConstant(float tc) {
    pImpl->phasicTimeConstant = std::max(10.0f, tc);
    pImpl->tonicTimeConstant = std::max(100.0f, tc * 10.0f);
}

void Dopamine::setLearningRate(float lr) {
    pImpl->learningRate = std::clamp(lr, 0.01f, 1.0f);
}

float Dopamine::getPhasicComponent() const {
    return pImpl->phasic;
}

float Dopamine::getTonicComponent() const {
    return pImpl->tonic;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getRewardPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on established models
    // Combines phasic and tonic components with learning theory
    
    float totalDopamine = pImpl->phasic + pImpl->tonic;
    
    // Tonic dopamine: baseline modulation of plasticity (tonic = 0.1 for baseline)
    float tonicPlasticity = 0.1f + 0.5f * pImpl->tonic;
    
    // Phasic dopamine: prediction error-driven learning
    // Phasic should have stronger effect on plasticity for learning
    float phasicPlasticity = 0.2f + 1.5f * pImpl->phasic;
    
    // Combined factor with nonlinear interaction
    float plasticityFactor = tonicPlasticity * 0.3f + phasicPlasticity * 0.7f;
    
    // Apply gain factor for clinical plausibility
    float modulation = std::clamp(plasticityFactor * pImpl->plasticityGain, 0.1f, 3.0f);
    
    // Add prediction error modulation for adaptive learning
    float predictionModulation = 1.0f + std::abs(pImpl->predictionError) * pImpl->predictionGain * 0.5f;
    
    return modulation * predictionModulation;
}

float Dopamine::getPlasticityModulation() const {
    return getPlasticityFactor();
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal dynamics
    // Updates prediction reference and triggers phasic response
    
    // Update prediction reference with temporal learning
    float error = reward - pImpl->reference;
    pImpl->predictionError = error;
    
    // Temporal filtering of prediction error
    pImpl->filteredPredictionError = pImpl->integrationFactor * pImpl->filteredPredictionError + 
                                     (1.0f - pImpl->integrationFactor) * error;
    
    // Update reference with temporal difference learning
    pImpl->reference += pImpl->learningRate * error;
    
    // Update observed reward statistics
    pImpl->maxObservedReward = std::max(pImpl->maxObservedReward, reward);
    pImpl->minObservedReward = std::min(pImpl->minObservedReward, reward);
    pImpl->accumulatedPredictionError += std::abs(error);
    
    // Add to history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 1000) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Generate phasic dopamine response
    // Phasic response scales with prediction error magnitude
    float phasicResponse = std::max(0.0f, error * pImpl->phasicGain);
    
    // Also include some reward magnitude effect
    float rewardMagnitude = std::min(1.0f, reward * 0.5f);
    
    // Combine components for total phasic response
    float totalPhasic = phasicResponse + rewardMagnitude * 0.3f;
    pImpl->phasic = totalPhasic;
    
    // Update level based on mode and components
    updateComponents();
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling
    pImpl->predictionError = error;
    
    // Temporal integration of prediction error
    pImpl->filteredPredictionError = pImpl->integrationFactor * pImpl->filteredPredictionError + 
                                     (1.0f - pImpl->integrationFactor) * error;
    
    // Update prediction reference
    pImpl->reference += pImpl->learningRate * error;
    
    // Add to history
    pImpl->predictionHistory.push_back(error);
    if (pImpl->predictionHistory.size() > 1000) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Generate phasic response
    float phasicResponse = std::max(0.0f, std::abs(error) * pImpl->phasicGain * 2.0f);
    
    // For positive errors (better than expected), increase motivation
    // For negative errors (worse than expected), decrease motivation
    if (error > 0.0f) {
        pImpl->phasic = phasicResponse * 1.5f;  // Excitatory response
    } else {
        pImpl->phasic = -phasicResponse * 0.5f;  // Inhibitory response
    }
    
    updateComponents();
}

void Dopamine::signalNovelty(float novelty) {
    // Novelty signaling for exploration
    // Higher novelty increases phasic dopamine
    float noveltyResponse = novelty * pImpl->phasicGain * 0.8f;
    
    // Combine with existing phasic level
    pImpl->phasic = std::max(pImpl->phasic, noveltyResponse);
    
    // Update components
    updateComponents();
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with temporal filtering
    // Updates both phasic and tonic components
    
    if (dt > 0.0) {
        float dtMs = static_cast<float>(dt) * 1000.0f;  // Convert to ms
        
        // Tonic dopamine: exponential decay towards baseline
        float tonicDecay = std::exp(-dtMs / pImpl->tonicTimeConstant);
        pImpl->tonic = pImpl->tonic * tonicDecay + pImpl->baseline * (1.0f - tonicDecay);
        
        // Phasic dopamine: decay over time
        float phasicDecay = std::exp(-dtMs / pImpl->phasicTimeConstant);
        pImpl->phasic = pImpl->phasic * phasicDecay;
        
        // Apply mode weighting
        pImpl->level = pImpl->phasic * pImpl->mode + pImpl->tonic * (1.0f - pImpl->mode);
        
        // Apply gains
        pImpl->level *= pImpl->phasicGain * pImpl->mode + pImpl->tonicGain * (1.0f - pImpl->mode);
        
        // Clamp to reasonable bounds
        pImpl->phasic = std::clamp(pImpl->phasic, -1.0f, 2.0f);
        pImpl->tonic = std::clamp(pImpl->tonic, 0.0f, 1.0f);
        pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    }
}

void Dopamine::updateComponents() {
    // Update combined level and components based on current state
    if (pImpl->mode > 0.5f) {
        // Phasic-dominant mode
        pImpl->level = pImpl->phasic * pImpl->mode * pImpl->phasicGain;
    } else {
        // Tonic-dominant mode
        pImpl->level = pImpl->tonic * (1.0f - pImpl->mode) * pImpl->tonicGain;
    }
    
    // Add baseline to tonic component
    pImpl->tonic += pImpl->baseline;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void Dopamine::clearHistory() {
    pImpl->rewardHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->phasicHistory.clear();
    pImpl->tonicHistory.clear();
    pImpl->maxObservedReward = 0.0f;
    pImpl->minObservedReward = 0.0f;
    pImpl->accumulatedPredictionError = 0.0f;
    pImpl->filteredPredictionError = 0.0f;
    pImpl->phasic = 0.0f;
    pImpl->tonic = pImpl->baseline;
    pImpl->level = pImpl->baseline;
}

float Dopamine::getExplorationValue() const {
    // Exploration value combines novelty and prediction error
    float noveltyComponent = std::min(1.0f, std::abs(pImpl->predictionError) * 2.0f);
    float surpriseComponent = std::min(1.0f, std::abs(pImpl->filteredPredictionError));
    
    return noveltyComponent * 0.6f + surpriseComponent * 0.4f;
}

// ==================== ACETYLCHOLINE ====================

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionSignal;
    float memoryTrace;
    float consolidationValue;
    float timeConstant;
    float integrationFactor;
    
    // Attention modulation
    float attentionGain;
    float arousalLevel;
    float memoryCapacity;
    
    // History
    std::vector<float> attentionHistory;
    std::vector<float> memoryHistory;
    
    Impl()
        : level(0.0f), baseline(0.1f), attentionSignal(0.0f), memoryTrace(0.0f)
        , consolidationValue(1.0f), timeConstant(100.0f), integrationFactor(0.95f)
        , attentionGain(1.0f), arousalLevel(0.5f), memoryCapacity(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine::Impl() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity for memory encoding
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    if (dt > 0.0) {
        // Leak towards baseline
        pImpl->level = pImpl->level * std::exp(-dt / pImpl->timeConstant) +
                      pImpl->baseline * (1.0f - std::exp(-dt / pImpl->timeConstant));
        
        // Apply integration
        pImpl->attentionSignal = pImpl->integrationFactor * pImpl->attentionSignal +
                                (1.0f - pImpl->integrationFactor) * pImpl->level;
    }
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Signal attention from cortical areas
    pImpl->attentionSignal = attentionSignal;
    pImpl->attentionHistory.push_back(attentionSignal);
    if (pImpl->attentionHistory.size() > 1000) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
    }
    
    // Update level based on attention
    pImpl->level = std::min(1.0f, pImpl->level + attentionSignal * 0.1f);
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryTrace) {
    // Consolidate memory traces
    float traceStrength = 0.0f;
    for (float value : memoryTrace) {
        traceStrength += std::abs(value);
    }
    traceStrength /= memoryTrace.size();
    
    pImpl->memoryTrace = traceStrength;
    pImpl->memoryHistory.push_back(traceStrength);
    if (pImpl->memoryHistory.size() > 1000) {
        pImpl->memoryHistory.erase(pImpl->memoryHistory.begin());
    }
    
    // Increase level based on memory strength
    pImpl->level = std::min(1.0f, pImpl->level + traceStrength * 0.05f);
}

float Acetylcholine::getAttentionValue() const {
    return pImpl->attentionSignal;
}

float Acetylcholine::getMemoryConsolidationValue() const {
    return pImpl->memoryTrace;
}

// ==================== NOREPINEPHRINE ====================

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousal;
    float vigilance;
    float stress;
    float timeConstant;
    float integrationFactor;
    
    // Modulation parameters
    float arousalGain;
    float vigilanceGain;
    float stressGain;
    
    // History
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    std::vector<float> stressHistory;
    
    Impl()
        : level(0.0f), baseline(0.1f), arousal(0.5f), vigilance(0.5f), stress(0.0f)
        , timeConstant(100.0f), integrationFactor(0.95f)
        , arousalGain(1.0f), vigilanceGain(1.0f), stressGain(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE affects plasticity for arousal and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    if (dt > 0.0) {
        // Leak towards baseline
        float decay = std::exp(-dt / pImpl->timeConstant);
        pImpl->level = pImpl->level * decay + pImpl->baseline * (1.0f - decay);
        
        // Apply integration
        pImpl->arousal = pImpl->integrationFactor * pImpl->arousal +
                        (1.0f - pImpl->integrationFactor) * pImpl->level;
    }
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousal = arousalLevel;
    pImpl->arousalHistory.push_back(arousalLevel);
    if (pImpl->arousalHistory.size() > 1000) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
    }
    
    pImpl->level = std::min(1.0f, pImpl->level + arousalLevel * 0.1f);
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    pImpl->vigilance = vigilanceLevel;
    pImpl->vigilanceHistory.push_back(vigilanceLevel);
    if (pImpl->vigilanceHistory.size() > 1000) {
        pImpl->vigilanceHistory.erase(pImpl->vigilanceHistory.begin());
    }
    
    pImpl->level = std::min(1.0f, pImpl->level + vigilanceLevel * 0.05f);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stress = stressLevel;
    pImpl->stressHistory.push_back(stressLevel);
    if (pImpl->stressHistory.size() > 1000) {
        pImpl->stressHistory.erase(pImpl->stressHistory.begin());
    }
    
    // Stress increases level but with saturation
    pImpl->level = std::min(1.0f, pImpl->level + stressLevel * 0.15f);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stress;
}

// ==================== SEROTONIN ====================

struct Serotonin::Impl {
    float level;
    float baseline;
    float mood;
    float socialBehavior;
    float impulseControl;
    float timeConstant;
    float integrationFactor;
    
    // Modulation parameters
    float moodGain;
    float socialGain;
    float impulseGain;
    
    // History
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    std::vector<float> impulseHistory;
    
    Impl()
        : level(0.0f), baseline(0.1f), mood(0.5f), socialBehavior(0.5f), impulseControl(0.5f)
        , timeConstant(100.0f), integrationFactor(0.95f)
        , moodGain(1.0f), socialGain(1.0f), impulseGain(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for mood and social behavior
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    if (dt > 0.0) {
        // Leak towards baseline
        float decay = std::exp(-dt / pImpl->timeConstant);
        pImpl->level = pImpl->level * decay + pImpl->baseline * (1.0f - decay);
        
        // Apply integration
        pImpl->mood = pImpl->integrationFactor * pImpl->mood +
                    (1.0f - pImpl->integrationFactor) * pImpl->level;
    }
}

void Serotonin::signalMood(float moodValue) {
    pImpl->mood = moodValue;
    pImpl->moodHistory.push_back(moodValue);
    if (pImpl->moodHistory.size() > 1000) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    
    pImpl->level = std::min(1.0f, pImpl->level + moodValue * 0.1f);
}

void Serotonin::signalSocialBehavior(float socialValue) {
    pImpl->socialBehavior = socialValue;
    pImpl->socialHistory.push_back(socialValue);
    if (pImpl->socialHistory.size() > 1000) {
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
    
    pImpl->level = std::min(1.0f, pImpl->level + socialValue * 0.05f);
}

void Serotonin::signalImpulse(float impulseValue) {
    pImpl->impulseControl = impulseValue;
    pImpl->impulseHistory.push_back(impulseValue);
    if (pImpl->impulseHistory.size() > 1000) {
        pImpl->impulseHistory.erase(pImpl->impulseHistory.begin());
    }
    
    // Impulse increases level but with saturation
    pImpl->level = std::min(1.0f, pImpl->level + impulseValue * 0.08f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

// Implementation for Acetylcholine destructor
Acetylcholine::~Acetylcholine() = default;

// Implementation for Norepinephrine destructor
Norepinephrine::~Norepinephrine() = default;

// Implementation for Serotonin destructor
Serotonin::~Serotonin() = default;
