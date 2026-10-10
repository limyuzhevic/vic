#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    // Core dopamine level dynamics
    float level;
    float baseline;
    float peak;
    
    // TD learning parameters
    float tdLearningError;
    float predictionError;
    float expectedValue;
    float actualValue;
    float tdAlpha;           // Learning rate
    
    // Eligibility traces for reward-modulated learning
    std::unordered_map<NeuronId, float> eligibilityTraces;
    std::unordered_map<NeuronId, float> postSynapticActivity;
    float eligibilityDecayRate;
    float eligibilityLearnRate;
    
    // Burst signaling
    float burstIntensity;
    float burstStartTime;
    float burstDuration;
    bool isBursting;
    float burstThreshold;
    
    // Recovery dynamics
    float recoveryRate;
    float recoveryTarget;
    bool recovering;
    
    // Memory consolidation
    std::unordered_map<NeuronId, float> activityHistory;
    float importanceThreshold;
    float consolidationStrength;
    
    // Temporal dynamics
    TimestepDuration currentTime;
    float decayRate;
    float releaseRate;
    
    // Statistics
    float eligibilityTraceSum;
    float totalBursts;
    float totalConsolidations;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)  // Tonic baseline
        , peak(1.0f)
        , tdLearningError(0.0f)
        , predictionError(0.0f)
        , expectedValue(0.0f)
        , actualValue(0.0f)
        , tdAlpha(0.1f)  // TD learning rate
        , eligibilityDecayRate(0.01f)
        , eligibilityLearnRate(0.5f)
        , burstIntensity(0.0f)
        , burstStartTime(0.0f)
        , burstDuration(0.1f)
        , isBursting(false)
        , burstThreshold(0.8f)
        , recoveryRate(0.05f)
        , recoveryTarget(0.0f)
        , recovering(false)
        , importanceThreshold(0.3f)
        , consolidationStrength(0.5f)
        , currentTime(0.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , eligibilityTraceSum(0.0f)
        , totalBursts(0.0f)
        , totalConsolidations(0.0f) {}
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
    // Enhanced plasticity factor based on TD error
    float tdInfluence = 0.5f + 0.5f * std::abs(pImpl->tdLearningError);
    float burstInfluence = pImpl->isBursting ? 1.5f : 1.0f;
    float eligibilityInfluence = std::min(1.0f, pImpl->eligibilityTraceSum / 10.0f);
    
    return 0.5f * tdInfluence * burstInfluence * eligibilityInfluence;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->currentTime = static_cast<float>(dt);
    
    // Update burst state
    if (pImpl->isBursting) {
        float elapsed = pImpl->currentTime - pImpl->burstStartTime;
        if (elapsed >= pImpl->burstDuration) {
            pImpl->isBursting = false;
            pImpl->burstIntensity = 0.0f;
        }
    }
    
    // Decay dopamine level towards baseline
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Recovery towards baseline if recovering
    if (pImpl->recovering) {
        float recoveryAmount = pImpl->recoveryRate * static_cast<float>(dt);
        pImpl->level = std::min(pImpl->baseline, pImpl->level + recoveryAmount);
        if (std::abs(pImpl->level - pImpl->baseline) < 0.01f) {
            pImpl->recovering = false;
        }
    }
    
    // Update eligibility traces with decay
    for (auto& pair : pImpl->eligibilityTraces) {
        pair.second *= (1.0f - pImpl->eligibilityDecayRate * static_cast<float>(dt));
    }
    
    // Recalculate eligibility trace sum
    pImpl->eligibilityTraceSum = 0.0f;
    for (const auto& pair : pImpl->eligibilityTraces) {
        pImpl->eligibilityTraceSum += pair.second;
    }
}

void Dopamine::signalReward(float reward) {
    // Immediate dopamine response
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Calculate prediction error if expected value exists
    if (pImpl->expectedValue > 0.0f) {
        computeTDLearningError(pImpl->expectedValue, reward);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic dopamine response to prediction error
    pImpl->predictionError = error;
    
    // Calculate TD learning error
    if (pImpl->expectedValue > 0.0f) {
        computeTDLearningError(pImpl->expectedValue, pImpl->expectedValue + error);
    }
    
    // Apply prediction error to dopamine level
    float errorResponse = error * pImpl->releaseRate;
    if (error > 0.0f) {
        // Positive prediction error (unexpected reward): burst
        triggerBurst(std::min(1.0f, error), 0.1f);
    } else {
        // Negative prediction error (omitted reward): dip
        pImpl->level = std::max(pImpl->baseline, pImpl->level + errorResponse);
    }
}

void Dopamine::recordPostSynapticActivity(NeuronId neuronId, float activity) {
    pImpl->postSynapticActivity[neuronId] = activity;
    
    // Update eligibility trace
    if (activity > pImpl->burstThreshold) {
        float eligibilityUpdate = pImpl->eligibilityLearnRate * activity;
        pImpl->eligibilityTraces[neuronId] += eligibilityUpdate;
    }
}

void Dopamine::updateEligibilityTrace(NeuronId neuronId, TimestepDuration dt) {
    if (pImpl->eligibilityTraces.find(neuronId) != pImpl->eligibilityTraces.end()) {
        // Decay eligibility trace
        pImpl->eligibilityTraces[neuronId] *= (1.0f - pImpl->eligibilityDecayRate * static_cast<float>(dt));
    }
}

void Dopamine::computeTDLearningError(float expectedReward, float actualReward) {
    pImpl->expectedValue = expectedReward;
    pImpl->actualValue = actualReward;
    pImpl->tdLearningError = actualReward - expectedReward;
    
    // Apply TD learning error to dopamine level
    float tdErrorResponse = pImpl->tdLearningError * pImpl->tdAlpha * pImpl->releaseRate;
    pImpl->level += tdErrorResponse;
    
    // Record this error in history
    pImpl->activityHistory[actualReward] += 1.0f;
}

void Dopamine::consolidateToMemory(float importanceThreshold) {
    pImpl->importanceThreshold = importanceThreshold;
    
    // Find neurons with high eligibility traces for consolidation
    for (const auto& pair : pImpl->eligibilityTraces) {
        if (pair.second >= pImpl->importanceThreshold) {
            NeuronId neuronId = pair.first;
            float importance = pair.second * pImpl->consolidationStrength;
            
            // Record in activity history for memory consolidation
            pImpl->activityHistory[neuronId] += importance;
            pImpl->totalConsolidations += importance;
        }
    }
}

void Dopamine::triggerBurst(float intensity, TimestepDuration duration) {
    pImpl->burstIntensity = intensity;
    pImpl->burstDuration = duration;
    pImpl->burstStartTime = pImpl->currentTime;
    pImpl->isBursting = true;
    
    // Immediate burst increase
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity);
    pImpl->totalBursts += intensity;
}

bool Dopamine::isBursting() const {
    return pImpl->isBursting;
}

void Dopamine::startRecoveryPhase(float recoveryRate) {
    pImpl->recoveryRate = recoveryRate;
    pImpl->recoveryTarget = pImpl->baseline;
    pImpl->recovering = true;
}

void Dopamine::resetToBaseline() {
    pImpl->level = pImpl->baseline;
    pImpl->recovering = false;
    pImpl->tdLearningError = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->expectedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->eligibilityTraceSum = 0.0f;
}

float Dopamine::getTDLearningError() const {
    return pImpl->tdLearningError;
}

float Dopamine::getEligibilityTraceSum() const {
    return pImpl->eligibilityTraceSum;
}

float Dopamine::getBaselineLevel() const {
    return pImpl->baseline;
}

float Dopamine::getBurstStrength() const {
    return pImpl->totalBursts / 100.0f;
}

void Dopamine::setTDAlpha(float alpha) {
    pImpl->tdAlpha = alpha;
}

void Dopamine::setElearnCoefficient(float coeff) {
    pImpl->eligibilityLearnRate = coeff;
}

void Dopamine::setBurstThreshold(float threshold) {
    pImpl->burstThreshold = threshold;
}

void Dopamine::setRecoveryTime(float time) {
    pImpl->recoveryRate = 1.0f / time;
}

// ===============================================
// Acetylcholine (ACh) Implementation
// ===============================================

struct Acetylcholine::Impl {
    // Core ACh dynamics
    float level;
    float baseline;
    float peak;
    
    // Attention modulation
    float attentionGain;
    float attentionDecay;
    float attentionStrength;
    NeuronId currentFocus;
    std::unordered_map<NeuronId, float> attentionWeights;
    
    // Memory consolidation
    std::vector<float> memoryPatterns;
    std::unordered_map<NeuronId, float> memoryTraces;
    float consolidationStrength;
    float memoryReverberation;
    
    // Noise and dynamics
    float noiseLevel;
    float leakRate;
    float recoveryRate;
    
    // Temporal dynamics
    TimestepDuration currentTime;
    
    // Statistics
    float totalAttentionSwitch;
    float memoryConsolidationEvents;
    
    Impl()
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , attentionGain(1.0f)
        , attentionDecay(0.05f)
        , attentionStrength(0.0f)
        , currentFocus(0)
        , consolidationStrength(0.5f)
        , memoryReverberation(0.3f)
        , noiseLevel(0.01f)
        , leakRate(0.1f)
        , recoveryRate(0.05f)
        , currentTime(0.0f)
        , totalAttentionSwitch(0.0f)
        , memoryConsolidationEvents(0.0f) {}
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
    // ACh modulates plasticity based on attention
    float attentionFactor = pImpl->attentionStrength;
    float memoryFactor = std::min(1.0f, pImpl->memoryConsolidationEvents / 10.0f);
    
    return 1.0f + attentionFactor * 0.5f + memoryFactor * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->currentTime = static_cast<float>(dt);
    
    // Apply attention decay
    pImpl->attentionStrength *= (1.0f - pImpl->attentionDecay * static_cast<float>(dt));
    
    // Decay ACh level towards baseline
    float decayAmount = pImpl->leakRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add noise to level
    float noise = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * pImpl->noiseLevel;
    pImpl->level = std::clamp(pImpl->level + noise, 0.0f, 1.0f);
    
    // Update memory traces with reverberation
    for (auto& pair : pImpl->memoryTraces) {
        pair.second *= (1.0f - pImpl->memoryReverberation * static_cast<float>(dt));
    }
}

void Acetylcholine::increaseAttention(float attentionGain) {
    pImpl->attentionStrength += attentionGain;
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength);
    
    // Apply attention gain to level
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionGain * 0.2f);
}

void Acetylcholine::decreaseAttention(float attentionDecay) {
    pImpl->attentionStrength *= (1.0f - attentionDecay);
}

void Acetylcholine::switchAttention(NeuronId sourceNeuron, NeuronId targetNeuron) {
    pImpl->currentFocus = targetNeuron;
    pImpl->attentionWeights[sourceNeuron] = 0.0f;
    pImpl->attentionWeights[targetNeuron] = 1.0f;
    pImpl->totalAttentionSwitch++;
    
    // Boost target neuron attention
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + 0.3f);
}

void Acetylcholine::consolidateToLongTermMemory(const std::vector<float>& pattern) {
    pImpl->memoryPatterns.push_back(pattern);
    
    // Update memory traces for pattern elements
    for (size_t i = 0; i < pattern.size(); ++i) {
        float patternElement = pattern[i];
        if (patternElement > 0.5f) {
            // Convert to neuron ID (simplified mapping)
            NeuronId neuronId(i % 1000 + 1);
            pImpl->memoryTraces[neuronId] += patternElement * pImpl->consolidationStrength;
        }
    }
    
    pImpl->memoryConsolidationEvents++;
}

void Acetylcholine::updateMemoryTrace(NeuronId neuronId, float activity) {
    pImpl->memoryTraces[neuronId] += activity * pImpl->consolidationStrength;
    
    // Store in memory patterns if high activity
    if (activity > 0.7f) {
        // Add to memory patterns
        float traceValue = pImpl->memoryTraces[neuronId];
        pImpl->memoryPatterns.push_back(std::vector<float>(1, traceValue));
    }
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidationEvents / 100.0f;
}

void Acetylcholine::processCompetition(const std::vector<NeuronId>& competitors) {
    float totalWeight = 0.0f;
    
    // Calculate competition weights based on attention
    for (NeuronId neuronId : competitors) {
        float weight = pImpl->attentionWeights.find(neuronId) != pImpl->attentionWeights.end() 
                      ? pImpl->attentionWeights[neuronId] 
                      : 0.0f;
        totalWeight += weight;
    }
    
    // Normalize weights
    if (totalWeight > 0.0f) {
        for (auto& pair : pImpl->attentionWeights) {
            pair.second /= totalWeight;
        }
    }
}

NeuronId Acetylcholine::getCurrentFocus() const {
    return pImpl->currentFocus;
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionStrength;
}

void Acetylcholine::setBaselineLevel(float baseline) {
    pImpl->baseline = baseline;
}

void Acetylcholine::setNoiseLevel(float noise) {
    pImpl->noiseLevel = noise;
}

void Acetylcholine::setReverberationTime(float time) {
    pImpl->memoryReverberation = 1.0f / time;
}

float Acetylcholine::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

float Acetylcholine::getReverberationTime() const {
    return 1.0f / pImpl->memoryReverberation;
}

// ===============================================
// Norepinephrine (NE) Implementation
// ===============================================

struct Norepinephrine::Impl {
    // Core NE dynamics (locus coerulean system)
    float level;
    float baselineFiring;
    float peakFiring;
    
    // Arousal and vigilance
    float arousalLevel;
    float noveltyScore;
    float externalNovelty;
    float internalPredictability;
    
    // Attention switching
    float switchProbability;
    std::deque<NeuronId> attentionHistory;
    
    // Exploration bias
    float explorationBias;
    float exploitationBias;
    
    // NE dynamics
    float firingVariability;
    float adaptationRate;
    float adaptationAccumulator;
    
    // Temporal dynamics
    TimestepDuration currentTime;
    
    // Statistics
    float totalNoveltyEvents;
    float explorationEvents;
    
    Impl()
        : level(0.0f)
        , baselineFiring(0.1f)
        , peakFiring(1.0f)
        , arousalLevel(0.0f)
        , noveltyScore(0.0f)
        , externalNovelty(0.0f)
        , internalPredictability(1.0f)
        , switchProbability(0.1f)
        , explorationBias(0.5f)
        , exploitationBias(0.5f)
        , firingVariability(0.2f)
        , adaptationRate(0.05f)
        , adaptationAccumulator(0.0f)
        , currentTime(0.0f)
        , totalNoveltyEvents(0.0f)
        , explorationEvents(0.0f) {}
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
    // NE modulates plasticity based on arousal and novelty
    float arousalFactor = pImpl->arousalLevel;
    float noveltyFactor = pImpl->noveltyScore;
    
    // High arousal increases plasticity for novel information
    return 1.0f + arousalFactor * noveltyFactor * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->currentTime = static_cast<float>(dt);
    
    // Decay NE level towards baseline
    float decayAmount = 0.1f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baselineFiring, pImpl->level - decayAmount);
    
    // Apply adaptation (response to predictable environments)
    pImpl->adaptationAccumulator = std::max(0.0f, pImpl->adaptationAccumulator - pImpl->adaptationRate * static_cast<float>(dt));
    
    // Update arousal based on novelty
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + pImpl->noveltyScore * 0.1f);
    pImpl->arousalLevel *= (1.0f - 0.05f * static_cast<float>(dt)); // Decay arousal
    
    // Calculate total variability
    float variability = pImpl->firingVariability * (1.0f + pImpl->noveltyScore);
    pImpl->level += ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * variability * 0.01f;
}

void Norepinephrine::increaseArousal(float arousalLevel) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + arousalLevel);
    pImpl->level = std::min(pImpl->peakFiring, pImpl->level + arousalLevel * 0.3f);
}

void Norepinephrine::decreaseArousal(float decayRate) {
    pImpl->arousalLevel *= (1.0f - decayRate);
}

void Norepinephrine::detectNovelty(float noveltyScore) {
    pImpl->externalNovelty = noveltyScore;
    pImpl->noveltyScore = std::max(pImpl->noveltyScore, noveltyScore);
    pImpl->totalNoveltyEvents++;
    
    // Increase NE level for novel stimuli
    if (noveltyScore > 0.5f) {
        pImpl->level = std::min(pImpl->peakFiring, pImpl->level + noveltyScore * 0.2f);
        
        // Adapt to reduce response to novel stimuli over time
        pImpl->adaptationAccumulator += noveltyScore * 0.1f;
    }
}

void Norepinephrine::updateLocusCoeruleanFiring(float externalNovelty, float internalPredictability) {
    pImpl->externalNovelty = externalNovelty;
    pImpl->internalPredictability = internalPredictability;
    
    // Calculate firing based on novelty and predictability
    float firingRate = baselineFiring * (1.0f + externalNovelty * 2.0f) * (2.0f - internalPredictability);
    
    // Apply adaptation
    firingRate *= (1.0f - pImpl->adaptationAccumulator * 0.1f);
    
    pImpl->level = std::clamp(firingRate, pImpl->baselineFiring, pImpl->peakFiring);
}

void Norepinephrine::enableAttentionSwitching(float switchProbability) {
    pImpl->switchProbability = switchProbability;
}

NeuronId Norepinephrine::selectNextFocus(const std::vector<NeuronId>& availableNeurons) {
    if (availableNeurons.empty()) return 0;
    
    // Random selection based on switch probability
    if ((static_cast<float>(rand()) / RAND_MAX) < pImpl->switchProbability) {
        // Random neuron selection
        size_t randomIndex = static_cast<size_t>((static_cast<float>(rand()) / RAND_MAX) * availableNeurons.size());
        NeuronId selected = availableNeurons[randomIndex];
        
        // Update attention history
        pImpl->attentionHistory.push_back(selected);
        if (pImpl->attentionHistory.size() > 10) {
            pImpl->attentionHistory.pop_front();
        }
        
        return selected;
    }
    
    // Otherwise, maintain current focus (would need to track this in real implementation)
    return availableNeurons[0];
}

void Norepinephrine::promoteExploration(float explorationBias) {
    pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + explorationBias);
    pImpl->exploitationBias = std::max(0.0f, pImpl->exploitationBias - explorationBias);
    pImpl->explorationEvents++;
}

void Norepinephrine::promoteExploitation(float exploitationBias) {
    pImpl->exploitationBias = std::min(1.0f, pImpl->exploitationBias + exploitationBias);
    pImpl->explorationBias = std::max(0.0f, pImpl->explorationBias - exploitationBias);
}

void Norepinephrine::setBaselineFiring(float baseline) {
    pImpl->baselineFiring = baseline;
}

void Norepinephrine::setFiringVariability(float variability) {
    pImpl->firingVariability = variability;
}

void Norepinephrine::setAdaptationRate(float rate) {
    pImpl->adaptationRate = rate;
}

float Norepinephrine::getBaselineFiring() const {
    return pImpl->baselineFiring;
}

float Norepinephrine::getFiringVariability() const {
    return pImpl->firingVariability;
}

float Norepinephrine::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

bool Norepinephrine::isArousalHigh() const {
    return pImpl->arousalLevel > 0.7f;
}

// ===============================================
// Serotonin (5-HT) Implementation
// ===============================================

struct Serotonin::Impl {
    // Core 5-HT dynamics
    float level;
    float baselineMood;
    float peakMood;
    
    // Mood regulation
    float moodLevel;
    float negativeOutcomeSeverity;
    float recoveryAccumulator;
    
    // Behavioral inhibition
    float inhibitionStrength;
    float goalValue;
    bool actionInhibited;
    
    // Social behavior
    float socialBonding;
    float socialStatus;
    float socialCohesion;
    
    // 5-HT dynamics
    float recoveryRate;
    float inhibitionThreshold;
    
    // Temporal dynamics
    TimestepDuration currentTime;
    
    // Statistics
    float moodEvents;
    float inhibitionEvents;
    
    Impl()
        : level(0.0f)
        , baselineMood(0.5f)
        , peakMood(1.0f)
        , moodLevel(0.5f)
        , negativeOutcomeSeverity(0.0f)
        , recoveryAccumulator(0.0f)
        , inhibitionStrength(0.0f)
        , goalValue(0.5f)
        , actionInhibited(false)
        , socialBonding(0.5f)
        , socialStatus(0.0f)
        , socialCohesion(0.5f)
        , recoveryRate(0.05f)
        , inhibitionThreshold(0.3f)
        , currentTime(0.0f)
        , moodEvents(0.0f)
        , inhibitionEvents(0.0f) {}
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
    // 5-HT modulates plasticity based on mood and inhibition
    float moodFactor = pImpl->moodLevel;
    float inhibitionFactor = pImpl->actionInhibited ? 0.5f : 0.0f;
    
    return 1.0f - moodFactor * 0.2f - inhibitionFactor * 0.3f;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->currentTime = static_cast<float>(dt);
    
    // Decay 5-HT level towards baseline
    float decayAmount = 0.1f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baselineMood, pImpl->level - decayAmount);
    
    // Recovery from negative states
    if (pImpl->recoveryAccumulator > 0.0f) {
        float recoveryAmount = pImpl->recoveryRate * static_cast<float>(dt);
        pImpl->level = std::min(pImpl->peakMood, pImpl->level + recoveryAmount);
        pImpl->recoveryAccumulator = std::max(0.0f, pImpl->recoveryAccumulator - recoveryAmount);
    }
    
    // Update social cohesion based on bonding and status
    pImpl->socialCohesion = (pImpl->socialBonding + (1.0f - std::abs(pImpl->socialStatus))) * 0.5f;
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->moodLevel = moodLevel;
    pImpl->level = std::clamp(pImpl->level + moodLevel * 0.1f, pImpl->baselineMood, pImpl->peakMood);
    pImpl->moodEvents++;
}

void Serotonin::respondToNegativeOutcome(float outcomeSeverity) {
    pImpl->negativeOutcomeSeverity = outcomeSeverity;
    
    // Decrease 5-HT level for negative outcomes
    pImpl->level = std::max(pImpl->baselineMood, pImpl->level - outcomeSeverity * 0.3f);
    
    // Increase inhibition
    pImpl->inhibitionStrength += outcomeSeverity * 0.2f;
    pImpl->inhibitionStrength = std::min(1.0f, pImpl->inhibitionStrength);
    
    // Start recovery
    pImpl->recoveryAccumulator += outcomeSeverity * 0.5f;
}

void Serotonin::recoverFromNegativeState(float recoveryRate) {
    pImpl->recoveryAccumulator += recoveryRate;
}

void Serotonin::inhibitImpulsiveActions(float inhibitionStrength) {
    pImpl->inhibitionStrength += inhibitionStrength;
    pImpl->inhibitionStrength = std::min(1.0f, pImpl->inhibitionStrength);
    pImpl->actionInhibited = pImpl->inhibitionStrength > pImpl->inhibitionThreshold;
    pImpl->inhibitionEvents++;
}

void Serotonin::enableGoalDirectedBehavior(float goalValue) {
    pImpl->goalValue = goalValue;
    
    // Reduce inhibition when pursuing goals
    pImpl->inhibitionStrength *= (1.0f - goalValue * 0.5f);
    pImpl->actionInhibited = pImpl->inhibitionStrength > pImpl->inhibitionThreshold;
}

bool Serotonin::isActionInhibited() const {
    return pImpl->actionInhibited;
}

void Serotonin::modulateSocialBonding(float bondingLevel) {
    pImpl->socialBonding = bondingLevel;
    
    // Increase 5-HT level with social bonding
    pImpl->level = std::min(pImpl->peakMood, pImpl->level + bondingLevel * 0.1f);
}

void Serotonin::respondToSocialStatus(float statusChange) {
    pImpl->socialStatus = statusChange;
    
    // Negative status changes can reduce 5-HT
    if (statusChange < 0.0f) {
        pImpl->level = std::max(pImpl->baselineMood, pImpl->level - std::abs(statusChange) * 0.1f);
    }
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

void Serotonin::setBaselineMood(float baseline) {
    pImpl->baselineMood = baseline;
}

void Serotonin::setRecoveryRate(float rate) {
    pImpl->recoveryRate = rate;
}

void Serotonin::setInhibitionThreshold(float threshold) {
    pImpl->inhibitionThreshold = threshold;
}

float Serotonin::getBaselineMood() const {
    return pImpl->baselineMood;
}

float Serotonin::getRecoveryRate() const {
    return pImpl->recoveryRate;
}

float Serotonin::getInhibitionThreshold() const {
    return pImpl->inhibitionThreshold;
}

} // namespace nlm
