#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <queue>

namespace nlm {

struct Neuromodulator::Impl {
    // Dopamine system
    float dopamineLevel;
    float dopamineBaseline;
    float dopaminePeak;
    float dopamineDecayRate;
    float dopamineReleaseRate;
    float rewardPredictionError;
    
    // Novelty detection
    float noveltySignal;
    float noveltyBaseline;
    float noveltyAdaptationRate;
    std::queue<float> noveltyHistory;
    
    // Curiosity system
    float curiosityLevel;
    float curiosityBaseline;
    float explorationBias;
    float intrinsicMotivation;
    
    // Attention system
    float attentionLevel;
    float attentionCapacity;
    float attentionFocus;
    float attentionDecay;
    
    // Prediction error components
    float predictionErrorSignal;
    float precisionSignal;
    float confidenceSignal;
    
    // Synaptic modulation state
    float synapticModulation;
    float plasticityGate;
    float synapticWeight;
    
    // Biological parameters
    float learningRate;
    float neuromodulationStrength;
    float adaptationTimeConstant;
    float noiseLevel;
    
    Impl() : 
        dopamineLevel(0.0f),
        dopamineBaseline(0.1f),
        dopaminePeak(1.0f),
        dopamineDecayRate(0.05f),
        dopamineReleaseRate(2.0f),
        rewardPredictionError(0.0f),
        noveltySignal(0.0f),
        noveltyBaseline(0.5f),
        noveltyAdaptationRate(0.1f),
        curiosityLevel(0.5f),
        curiosityBaseline(0.3f),
        explorationBias(0.5f),
        intrinsicMotivation(0.5f),
        attentionLevel(0.5f),
        attentionCapacity(1.0f),
        attentionFocus(0.5f),
        attentionDecay(0.02f),
        predictionErrorSignal(0.0f),
        precisionSignal(1.0f),
        confidenceSignal(0.5f),
        synapticModulation(1.0f),
        plasticityGate(1.0f),
        synapticWeight(1.0f),
        learningRate(0.01f),
        neuromodulationStrength(1.0f),
        adaptationTimeConstant(100.0f),
        noiseLevel(0.1f) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

Neuromodulator::~Neuromodulator() = default;

const char* Neuromodulator::getName() const {
    return "Neuromodulator";
}

float Neuromodulator::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

void Neuromodulator::setDopamineLevel(float level) {
    pImpl->dopamineLevel = std::clamp(level, 0.0f, 1.0f);
}

float Neuromodulator::getDopamineBaseline() const {
    return pImpl->dopamineBaseline;
}

void Neuromodulator::setDopamineBaseline(float baseline) {
    pImpl->dopamineBaseline = std::clamp(baseline, 0.0f, 1.0f);
}

float Neuromodulator::getDopaminePeak() const {
    return pImpl->dopaminePeak;
}

void Neuromodulator::setDopaminePeak(float peak) {
    pImpl->dopaminePeak = std::clamp(peak, 0.0f, 1.0f);
}

float Neuromodulator::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on biological evidence
    return calculateDopaminePlasticityFactor();
}

void Neuromodulator::update(TimestepDuration dt) {
    // Biological dopamine dynamics
    updateDopamineDynamics(dt);
    
    // Novelty adaptation
    updateNoveltySignal(dt);
    
    // Curiosity system updates
    updateCuriosity(dt);
    
    // Attention system updates
    updateAttention(dt);
    
    // Prediction error integration
    updatePredictionError(dt);
    
    // Synaptic modulation updates
    updateSynapticModulation(dt);
}

void Neuromodulator::signalReward(float reward) {
    // Biological reward signaling with temporal dynamics
    dopamineBurst(reward, RewardType::Primary);
    
    // Update reward prediction error
    rewardPredictionError += reward;
    
    // Enhance novelty signal for reward-predicted events
    noveltySignal *= 1.1f;
}

void Neuromodulator::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (Miller-Kaczmarek model)
    updateRewardPredictionError(error);
    
    // Dopamine response to RPE
    dopamineRPEResponse(error);
    
    // Update curiosity based on prediction error
    updateCuriosityFromRPE(error);
}

void Neuromodulator::signalNovelty(float novelty) {
    // Biological novelty detection based on prediction errors
    updateNoveltySignal(novelty);
    
    // Enhance attention for novel stimuli
    enhanceAttentionForNovelty(novelty);
    
    // Update exploration bias
    updateExplorationBias(novelty);
}

void Neuromodulator::updateDopamineDynamics(TimestepDuration dt) {
    // Biological dopamine dynamics with exponential decay
    float decay = std::exp(-pImpl->dopamineDecayRate * static_cast<float>(dt) / 1000.0f);
    
    // Decay towards baseline
    pImpl->dopamineLevel = pImpl->dopamineBaseline + 
                         (pImpl->dopamineLevel - pImpl->dopamineBaseline) * decay;
    
    // Ensure within bounds
    pImpl->dopamineLevel = std::clamp(pImpl->dopamineLevel, 0.0f, 1.0f);
}

void Neuromodulator::dopamineBurst(float reward, RewardType type) {
    // Real dopamine burst response to rewards
    float burstMagnitude = calculateBurstMagnitude(reward, type);
    
    // Phasic dopamine response
    pImpl->dopamineLevel = std::min(pImpl->dopaminePeak, 
                                 pImpl->dopamineLevel + burstMagnitude);
    
    // Temporal dynamics with adaptation
    adaptDopamineResponse(burstMagnitude);
}

void Neuromodulator::dopamineRPEResponse(float rpe) {
    // Reward prediction error response (Schultz model)
    float rpeResponse = calculateRPEResponse(rpe);
    
    // Update dopamine based on RPE
    pImpl->dopamineLevel = std::clamp(pImpl->dopamineLevel + rpeResponse, 0.0f, 1.0f);
    
    // RPE also modulates attention
    attentionFromRPE(rpe);
}

void Neuromodulator::updateNoveltySignal(TimestepDuration dt) {
    // Novelty detection based on prediction errors
    float noveltyInput = calculateNoveltyInput();
    
    // Adaptation dynamics
    float noveltyAdaptation = 1.0f - std::exp(-pImpl->noveltyAdaptationRate * static_cast<float>(dt) / 1000.0f);
    
    // Update novelty signal
    pImpl->noveltySignal = pImpl->noveltyBaseline + 
                         (noveltyInput - pImpl->noveltyBaseline) * noveltyAdaptation;
    
    // Store in history for learning
    pImpl->noveltyHistory.push(pImpl->noveltySignal);
    if (pImpl->noveltyHistory.size() > 100) {
        pImpl->noveltyHistory.pop();
    }
}

void Neuromodulator::updateNoveltySignal(float noveltyInput) {
    // Overload for external novelty input
    float noveltyAdaptation = 1.0f - std::exp(-pImpl->noveltyAdaptationRate * 0.001f);
    
    pImpl->noveltySignal = pImpl->noveltyBaseline + 
                         (noveltyInput - pImpl->noveltyBaseline) * noveltyAdaptation;
    
    pImpl->noveltyHistory.push(pImpl->noveltySignal);
    if (pImpl->noveltyHistory.size() > 100) {
        pImpl->noveltyHistory.pop();
    }
}

void Neuromodulator::updateCuriosity(float dt) {
    // Curiosity system based on novelty and uncertainty
    float uncertainty = calculateUncertainty();
    float intrinsicDrive = calculateIntrinsicDrive();
    
    // Adaptation dynamics
    float curiosityAdaptation = 1.0f - std::exp(-pImpl->adaptationTimeConstant * 0.001f);
    
    // Update curiosity level
    float targetCuriosity = uncertainty * intrinsicDrive;
    pImpl->curiosityLevel = pImpl->curiosityBaseline + 
                         (targetCuriosity - pImpl->curiosityBaseline) * curiosityAdaptation;
    
    // Ensure within bounds
    pImpl->curiosityLevel = std::clamp(pImpl->curiosityLevel, 0.0f, 1.0f);
}

void Neuromodulator::updateCuriosityFromRPE(float rpe) {
    // Update curiosity based on reward prediction error
    float curiosityChange = rpe * pImpl->explorationBias * 0.1f;
    pImpl->curiosityLevel = std::clamp(pImpl->curiosityLevel + curiosityChange, 0.0f, 1.0f);
}

void Neuromodulator::updateAttention(TimestepDuration dt) {
    // Attention system with capacity constraints
    float attentionDemand = calculateAttentionDemand();
    
    // Adaptation dynamics
    float attentionAdaptation = 1.0f - std::exp(-pImpl->attentionDecay * static_cast<float>(dt) / 1000.0f);
    
    // Update attention level
    float targetAttention = std::min(attentionDemand, pImpl->attentionCapacity);
    pImpl->attentionLevel = targetAttention * attentionAdaptation + 
                           pImpl->attentionLevel * (1.0f - attentionAdaptation);
    
    // Update focus based on novelty
    pImpl->attentionFocus = calculateAttentionFocus();
}

void Neuromodulator::enhanceAttentionForNovelty(float novelty) {
    // Enhance attention for novel stimuli
    float attentionBoost = novelty * 0.3f;
    pImpl->attentionLevel = std::min(pImpl->attentionLevel + attentionBoost, pImpl->attentionCapacity);
    
    // Update focus toward novel stimuli
    pImpl->attentionFocus = std::clamp(pImpl->attentionFocus + attentionBoost * 0.2f, 0.1f, 1.0f);
}

void Neuromodulator::updatePredictionError(TimestepDuration dt) {
    // Prediction error signal computation
    float predictionError = pImpl->predictionErrorSignal;
    
    // Precision weighting (higher confidence = higher weight)
    float precision = pImpl->precisionSignal;
    float weightedError = predictionError * precision;
    
    // Update prediction error with decay
    float errorDecay = std::exp(-1.0f / pImpl->adaptationTimeConstant * static_cast<float>(dt) / 1000.0f);
    pImpl->predictionErrorSignal = pImpl->predictionErrorSignal * errorDecay;
    
    // Store in history
    // (Would normally store in longer-term memory)
}

void Neuromodulator::updateSynapticModulation(TimestepDuration dt) {
    // Synaptic weight modulation based on neuromodulatory state
    float modulationFactor = calculateModulationFactor();
    
    // Update synaptic weight
    pImpl->synapticWeight = std::clamp(
        pImpl->synapticWeight + modulationFactor * pImpl->learningRate * static_cast<float>(dt) / 1000.0f,
        0.1f, 2.0f
    );
    
    // Update plasticity gate
    pImpl->plasticityGate = calculatePlasticityGate();
}

void Neuromodulator::attentionFromRPE(float rpe) {
    // Attention modulation based on reward prediction error
    float attentionChange = rpe * 0.2f;
    pImpl->attentionLevel = std::clamp(pImpl->attentionLevel + attentionChange, 0.0f, pImpl->attentionCapacity);
}

float Neuromodulator::calculateDopaminePlasticityFactor() const {
    // Calculate dopamine-modulated plasticity factor
    float baselinePlasticity = 0.5f;
    float dopamineEffect = pImpl->dopamineLevel * 0.5f;
    float noveltyEffect = pImpl->noveltySignal * 0.3f;
    
    return baselinePlasticity + dopamineEffect + noveltyEffect;
}

float Neuromodulator::calculateBurstMagnitude(float reward, RewardType type) const {
    // Calculate dopamine burst magnitude based on reward type and size
    float magnitude = reward * 1.0f;  // Scale factor
    
    // Different responses for primary vs secondary rewards
    if (type == RewardType::Primary) {
        magnitude *= 2.0f;  // Primary rewards stronger response
    } else {
        magnitude *= 1.0f;  // Secondary rewards standard response
    }
    
    return std::min(magnitude, pImpl->dopaminePeak);
}

float Neuromodulator::calculateRPEResponse(float rpe) const {
    // Calculate dopamine response to reward prediction error
    // Based on Schultz et al. model
    if (rpe > 0.0f) {
        // Positive RPE: dopamine increase
        return rpe * 1.0f;
    } else {
        // Negative RPE: dopamine decrease (suppression)
        return rpe * 0.5f;
    }
}

float Neuromodulator::calculateNoveltyInput() const {
    // Calculate novelty input from prediction errors
    if (pImpl->noveltyHistory.empty()) {
        return 0.5f;
    }
    
    float sumNovelty = 0.0f;
    size_t count = 0;
    for (float novelty : pImpl->noveltyHistory) {
        sumNovelty += novelty;
        count++;
    }
    
    return sumNovelty / static_cast<float>(count);
}

float Neuromodulator::calculateUncertainty() const {
    // Calculate prediction uncertainty
    float baseUncertainty = 0.5f;
    float noveltyUncertainty = pImpl->noveltySignal * 0.5f;
    float confidenceUncertainty = (1.0f - pImpl->confidenceSignal) * 0.3f;
    
    return baseUncertainty + noveltyUncertainty + confidenceUncertainty;
}

float Neuromodulator::calculateIntrinsicDrive() const {
    // Calculate intrinsic motivation drive
    return pImpl->curiosityLevel * pImpl->explorationBias;
}

float Neuromodulator::calculateAttentionDemand() const {
    // Calculate attention demand based on multiple factors
    float noveltyDemand = pImpl->noveltySignal * 0.4f;
    float curiosityDemand = pImpl->curiosityLevel * 0.3f;
    float rewardDemand = pImpl->dopamineLevel * 0.3f;
    
    return noveltyDemand + curiosityDemand + rewardDemand;
}

float Neuromodulator::calculateAttentionFocus() const {
    // Calculate attention focus based on current state
    float focusFromNovelty = pImpl->noveltySignal * 0.5f;
    float focusFromCuriosity = pImpl->curiosityLevel * 0.3f;
    float focusFromDopamine = pImpl->dopamineLevel * 0.2f;
    
    return focusFromNovelty + focusFromCuriosity + focusFromDopamine;
}

void Neuromodulator::adaptDopamineResponse(float burstMagnitude) {
    // Adapt dopamine response based on recent bursts
    float adaptation = burstMagnitude * 0.01f;
    pImpl->dopamineBaseline = std::clamp(pImpl->dopamineBaseline + adaptation, 0.0f, 1.0f);
}

float Neuromodulator::calculateModulationFactor() const {
    // Calculate overall neuromodulatory effect
    float dopamineModulation = pImpl->dopamineLevel;
    float noveltyModulation = pImpl->noveltySignal;
    float curiosityModulation = pImpl->curiosityLevel;
    
    return (dopamineModulation + noveltyModulation + curiosityModulation) / 3.0f;
}

float Neuromodulator::calculatePlasticityGate() const {
    // Calculate plasticity gate based on neuromodulatory state
    float gateFromDopamine = pImpl->dopamineLevel * 0.4f;
    float gateFromNovelty = pImpl->noveltySignal * 0.3f;
    float gateFromCuriosity = pImpl->curiosityLevel * 0.3f;
    
    return gateFromDopamine + gateFromNovelty + gateFromCuriosity;
}

void Neuromodulator::updateRewardPredictionError(float rpe) {
    // Update reward prediction error
    pImpl->rewardPredictionError = rpe;
    pImpl->predictionErrorSignal = rpe;
}

float Neuromodulator::getNoveltyLevel() const {
    return pImpl->noveltySignal;
}

void Neuromodulator::setNoveltyLevel(float novelty) {
    pImpl->noveltySignal = std::clamp(novelty, 0.0f, 1.0f);
}

float Neuromodulator::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

void Neuromodulator::setCuriosityLevel(float curiosity) {
    pImpl->curiosityLevel = std::clamp(curiosity, 0.0f, 1.0f);
}

float Neuromodulator::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Neuromodulator::setAttentionLevel(float attention) {
    pImpl->attentionLevel = std::clamp(attention, 0.0f, pImpl->attentionCapacity);
}

float Neuromodulator::getPredictionError() const {
    return pImpl->predictionErrorSignal;
}

float Neuromodulator::getConfidence() const {
    return pImpl->confidenceSignal;
}

void Neuromodulator::setConfidence(float confidence) {
    pImpl->confidenceSignal = std::clamp(confidence, 0.0f, 1.0f);
}

const char* Neuromodulator::getDopamineName() const {
    return "Dopamine";
}

const char* Neuromodulator::getNoveltyName() const {
    return "Novelty";
}

const char* Neuromodulator::getCuriosityName() const {
    return "Curiosity";
}

const char* Neuromodulator::getAttentionName() const {
    return "Attention";
}

} // namespace nlm
