// Dopamine: Reward and reinforcement learning signal
// PHASE 6 IMPLEMENTATION - Real dopamine dynamics with biological accuracy

#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine dynamics parameters
    float level;              // Current dopamine concentration (0.0-1.0)
    float baseline;           // Baseline dopamine level
    float peak;               // Maximum dopamine level
    float decayRate;          // Rate of decay towards baseline (per ms)
    float releaseRate;        // Rate of increase with reward (per unit reward)
    
    // Learning modulation factors
    float plasticityFactor;   // Current plasticity modulation factor (0.0-2.0)
    float predictionWeight;  // Weight of prediction error signaling
    float noveltyWeight;      // Weight of novelty detection
    
    // State tracking
    float lastRewardTime;     // Last time reward was signaled (ms)
    float totalRewardHistory; // Cumulative reward received
    size_t predictionCount;   // Number of prediction errors received
    
    // Biological timing parameters
    float phasicAdaptationTime; // Time constant for phasic adaptation (ms)
    float tonicAdaptationTime;  // Time constant for tonic adaptation (ms)
    float learningRate;         // Learning rate factor
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), 
             decayRate(0.05f), releaseRate(0.3f),
             plasticityFactor(1.0f), predictionWeight(0.5f), 
             noveltyWeight(0.3f), lastRewardTime(-1000.0f),
             totalRewardHistory(0.0f), predictionCount(0),
             phasicAdaptationTime(100.0f), tonicAdaptationTime(5000.0f),
             learningRate(0.01f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized with biological parameters");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine (DA)";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Biological model: dopamine modulates plasticity through two mechanisms:
    // 1. Phasic bursts enhance learning (high plasticity)
    // 2. Tonic baseline maintains stability (modulated plasticity)
    float phasicModulation = pImpl->level / pImpl->baseline; // Phasic boost
    float tonicModulation = 1.0f + (pImpl->baseline - pImpl->level) * 0.5f; // Tonic adjustment
    
    // Combine effects: phasic increases plasticity, tonic fine-tunes it
    return std::clamp(1.0f + (phasicModulation - 1.0f) * pImpl->learningRate + 
                      tonicModulation * 0.1f, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Two-component dopamine model: phasic and tonic dynamics
    float dt_ms = static_cast<float>(dt);
    
    // Phasic component: rapid response to salient events
    // Decays exponentially towards baseline
    float phasicComponent = pImpl->level - pImpl->baseline;
    phasicComponent *= std::exp(-dt_ms / pImpl->phasicAdaptationTime);
    
    // Tonic component: slow background modulation
    // Driven by cumulative reward history
    float tonicTarget = std::min(1.0f, pImpl->totalRewardHistory * 0.001f);
    float tonicComponent = tonicTarget - pImpl->level;
    tonicComponent *= std::exp(-dt_ms / pImpl->tonicAdaptationTime);
    
    // Update combined level
    pImpl->level += (phasicComponent + tonicComponent) * dt_ms;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Update plasticity factor based on current level
    pImpl->plasticityFactor = getPlasticityFactor();
}

void Dopamine::signalReward(float reward) {
    if (reward <= 0.0f) return;
    
    // Biological reward signaling:
    // 1. Phasic burst proportional to reward magnitude
    // 2. Integration with prediction error
    // 3. Habituation to repeated rewards
    
    float currentTime = pImpl->lastRewardTime + pImpl->phasicAdaptationTime; // Simplified
    float rewardRate = reward / std::max(1.0f, currentTime - pImpl->lastRewardTime);
    
    // Phasic burst: strong, fast response to unexpected reward
    float phasicBurst = reward * pImpl->releaseRate * 0.8f;
    
    // Temporal difference: larger response to better-than-expected reward
    float predictionError = reward - (pImpl->totalRewardHistory * 0.001f);
    float temporalComponent = predictionError * pImpl->predictionWeight;
    
    // Update dopamine level with combined signals
    float newLevel = pImpl->level + phasicBurst + temporalComponent;
    setLevel(std::min(newLevel, pImpl->peak));
    
    // Update history
    pImpl->totalRewardHistory += reward;
    pImpl->lastRewardTime = currentTime;
    pImpl->predictionCount++;
    
    NLM_LOG_INFO("Dopamine phasic burst: " + std::to_string(phasicBurst) + 
                 " | Prediction error: " + std::to_string(predictionError) +
                 " | New level: " + std::to_string(getLevel()));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling:
    // Positive error: received more reward than expected (learning)
    // Negative error: received less reward than expected (avoidance)
    
    if (error == 0.0f) return;
    
    float response = error * pImpl->predictionWeight * pImpl->releaseRate;
    
    // Different responses to positive vs negative prediction errors
    if (error > 0) {
        // Positive prediction error: strengthen associations
        setLevel(getLevel() + response * 1.2f); // Stronger response
        NLM_LOG_INFO("Dopamine RPE+: Strengthening associations (error=" + std::to_string(error) + ")");
    } else {
        // Negative prediction error: weaken associations
        setLevel(std::max(0.0f, getLevel() + response * 0.5f)); // Weaker response
        NLM_LOG_INFO("Dopamine RPE-: Weakening associations (error=" + std::to_string(error) + ")");
    }
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, pImpl->peak);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::max(peak, pImpl->baseline);
    setLevel(std::min(getLevel(), pImpl->peak));
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.0f, rate);
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.0f, rate);
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

void Dopamine::updateWithPrediction(float prediction, TimestepDuration dt) {
    // Integration with prediction system
    // Dopamine modulates prediction accuracy based on prediction error
    
    float predictionError = prediction - getLevel();
    signalRewardPredictionError(predictionError * pImpl->predictionWeight);
    
    // Update exploration drive based on prediction confidence
    float confidence = 1.0f - std::abs(predictionError);
    setLevel(getLevel() + confidence * 0.01f * static_cast<float>(dt));
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->plasticityFactor = 1.0f;
    pImpl->lastRewardTime = -1000.0f;
    pImpl->totalRewardHistory = 0.0f;
    pImpl->predictionCount = 0;
    
    NLM_LOG_INFO("Dopamine system reset to baseline level");
}

float Dopamine::getExplorationDrive() const {
    // Exploration drive combines curiosity and dopamine level
    float curiosityComponent = std::min(1.0f, pImpl->level * 0.5f + pImpl->noveltyWeight * 0.3f);
    float uncertaintyComponent = std::min(1.0f, 1.0f - getPlasticityFactor() * 0.3f);
    
    return (curiosityComponent + uncertaintyComponent) * 0.5f;
}

} // namespace nlm