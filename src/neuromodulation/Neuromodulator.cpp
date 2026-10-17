#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidation;
    float attentionFocus;
    float learningRate;
    float workingMemoryCapacity;
    float goalDirectedBias;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), memoryConsolidation(0.5f),
             attentionFocus(1.0f), learningRate(1.0f), workingMemoryCapacity(1.0f),
             goalDirectedBias(0.5f) {}
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
    // Dopamine modulates plasticity - moderate levels optimize learning
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Memory consolidation and learning rate decay
    pImpl->memoryConsolidation = std::max(0.1f, pImpl->memoryConsolidation - pImpl->decayRate * static_cast<float>(dt));
    pImpl->learningRate = std::max(0.5f, pImpl->learningRate - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionFocus = std::max(0.5f, pImpl->attentionFocus - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst release of dopamine on reward receipt
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    
    // Enhance memory consolidation for rewarded experiences
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + reward * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction error - key for reinforcement learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Modulate learning rate based on prediction error
    pImpl->learningRate = std::min(2.0f, pImpl->learningRate + std::abs(error) * 0.3f);
}

void Dopamine::modulateMemoryConsolidation(float consolidationStrength) {
    // Dopamine modulates memory consolidation in working and episodic memory
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + consolidationStrength * 0.8f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * 0.3f * pImpl->releaseRate);
}

void Dopamine::enhanceAttentionAndFocus(float focusGain) {
    // Dopamine enhances attentional focus and selection
    pImpl->attentionFocus = std::min(1.5f, pImpl->attentionFocus + focusGain * 0.9f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + focusGain * pImpl->releaseRate * 0.7f);
}

void Dopamine::modulateConceptFormationLearning(float learningRate) {
    // Dopamine affects concept formation learning rates
    pImpl->learningRate = std::min(2.0f, pImpl->learningRate + learningRate * 0.6f);
    if (learningRate > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + learningRate * 0.2f);
    }
}

void Dopamine::enhanceWorkingMemory(float capacityBoost) {
    // Dopamine enhances working memory capacity and retention
    pImpl->workingMemoryCapacity = std::min(1.5f, pImpl->workingMemoryCapacity + capacityBoost * 0.5f);
    if (capacityBoost > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + capacityBoost * 0.2f);
    }
}

void Dopamine::modulateGoalDirectedBehavior(float goalBias) {
    // Dopamine biases actions toward goals based on reward value
    pImpl->goalDirectedBias = std::min(1.0f, pImpl->goalDirectedBias + goalBias * 0.8f);
    if (goalBias > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + goalBias * pImpl->releaseRate * 0.4f);
    }
}

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
    // Dopamine modulates plasticity - moderate levels optimize learning
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Memory consolidation and learning rate decay
    pImpl->memoryConsolidation = std::max(0.1f, pImpl->memoryConsolidation - pImpl->decayRate * static_cast<float>(dt));
    pImpl->learningRate = std::max(0.5f, pImpl->learningRate - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionFocus = std::max(0.5f, pImpl->attentionFocus - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst release of dopamine on reward receipt
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    
    // Enhance memory consolidation for rewarded experiences
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + reward * 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to reward prediction error - key for reinforcement learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Modulate learning rate based on prediction error
    pImpl->learningRate = std::min(2.0f, pImpl->learningRate + std::abs(error) * 0.3f);
}

void Dopamine::initialize(Brain* brain) {
    // Initialize dopamine system with brain reference
    // Reset to baseline but allow for immediate signaling
    pImpl->baseline = 0.05f;
    pImpl->level = 0.05f;
    pImpl->memoryConsolidation = 0.1f;
    pImpl->attentionFocus = 0.5f;
    pImpl->learningRate = 0.5f;
}

void Dopamine::modulateMemoryConsolidation(float consolidationStrength) {
    // Dopamine modulates memory consolidation in working and episodic memory
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + consolidationStrength * 0.8f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * 0.3f * pImpl->releaseRate);
}

void Dopamine::enhanceAttentionAndFocus(float focusGain) {
    // Dopamine enhances attentional focus and selection
    pImpl->attentionFocus = std::min(1.5f, pImpl->attentionFocus + focusGain * 0.9f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + focusGain * pImpl->releaseRate * 0.7f);
}

void Dopamine::modulateConceptFormationLearning(float learningRate) {
    // Dopamine affects concept formation learning rates
    pImpl->learningRate = std::min(2.0f, pImpl->learningRate + learningRate * 0.6f);
    if (learningRate > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + learningRate * 0.2f);
    }
}

void Dopamine::enhanceWorkingMemory(float capacityBoost) {
    // Dopamine enhances working memory capacity and retention
    pImpl->workingMemoryCapacity = std::min(1.5f, pImpl->workingMemoryCapacity + capacityBoost * 0.5f);
    if (capacityBoost > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + capacityBoost * 0.2f);
    }
}

void Dopamine::modulateGoalDirectedBehavior(float goalBias) {
    // Dopamine biases actions toward goals based on reward value
    pImpl->goalDirectedBias = std::min(1.0f, pImpl->goalDirectedBias + goalBias * 0.8f);
    if (goalBias > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + goalBias * pImpl->releaseRate * 0.4f);
    }
}

} // namespace nlm
