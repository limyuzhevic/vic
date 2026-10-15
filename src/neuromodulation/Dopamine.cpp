// Dopamine: Reward and reinforcement learning signal
// Implements real dopamine signaling for reward prediction error and reward delivery

#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float attentionModulation;
    
    Impl() 
        : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f)
        , learningRate(0.1f), attentionModulation(0.0f) {}
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
    // Real dopamine-modulated plasticity: higher dopamine increases LTP probability
    // and decreases LTD probability
    float plasticityFactor = 0.5f + 0.5f * pImpl->level;
    
    // Apply attention modulation
    plasticityFactor *= (1.0f + pImpl->attentionModulation);
    
    return std::clamp(plasticityFactor, 0.0f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention modulation based on novelty and curiosity
    pImpl->attentionModulation = std::min(0.5f, pImpl->attentionModulation * 0.9f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: response to primary rewards
    float burst = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update attention modulation based on reward
    pImpl->attentionModulation += reward * pImpl->learningRate * 0.1f;
    
    NLM_LOG_INFO("Dopamine signaling reward: " + std::to_string(reward) + " (level: " + 
                std::to_string(pImpl->level) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: resposnse to prediction error
    float errorSignal = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + errorSignal);
    
    // Stronger prediction errors create more attention modulation
    pImpl->attentionModulation += std::abs(error) * pImpl->learningRate * 0.2f;
    
    NLM_LOG_INFO("Dopamine signaling RPE: " + std::to_string(error) + " (level: " + 
                std::to_string(pImpl->level) + ")");
}

void Dopamine::signalCuriosity(float curiosity) {
    // Signal curiosity-driven exploration
    float curiositySignal = curiosity * 0.5f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + curiositySignal);
    
    // Curiosity increases attention and plasticity
    pImpl->attentionModulation += curiosity * pImpl->learningRate * 0.3f;
}

float Dopamine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

} // namespace nlm
