#include "Dopamine.hpp"
#include <algorithm>
#include <random>
#include "../cognition/NeuralAttentionalSelection.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../motor/ActionSelection.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    std::mt19937 rng;
    
    // Enhanced dopamine effects
    float attentionGain;
    float memoryConsolidation;
    float actionSelectionTemperature;
    float learningRate;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , rng(std::random_device{}())
        , attentionGain(1.0f)
        , memoryConsolidation(1.0f)
        , actionSelectionTemperature(1.0f)
        , learningRate(1.0f) {}
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
    // Dopamine modulates plasticity through multiple mechanisms:
    // 1. Global scaling of all plasticity processes (STDP, Hebbian)
    // 2. Protection against overwriting recently strengthened synapses
    // 3. Resetting of eligibility traces
    float factor = 0.5f + 0.5f * pImpl->level;
    
    // Enhanced plasticity at moderate dopamine levels (e.g., during reward learning)
    if (pImpl->level > 0.3f && pImpl->level < 0.8f) {
        factor = std::min(factor * 1.5f, 2.0f);
    }
    
    // High dopamine can inhibit plasticity (e.g., during satiety/overshadowing)
    if (pImpl->level > 0.9f) {
        factor = std::max(factor * 0.7f, 0.3f);
    }
    
    return factor;
}

void Dopamine::update(TimestepDuration dt) {
    // Time-based decay with stochastic bursts
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise for realistic dynamics
    std::uniform_real_distribution<float> dist(-0.01f, 0.01f);
    pImpl->level += dist(pImpl->rng);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with temporal dynamics
    float rewardSignal = std::min(1.0f, reward * pImpl->releaseRate);
    
    // Temporal difference: larger for unexpected rewards
    float predictionError = rewardSignal - (pImpl->level * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * 1.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct response to reward prediction error
    // Phasic dopamine bursts for positive prediction error
    // Dip for negative prediction error (but not below baseline)
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 2.0f);
}

// Enhanced dopamine effects for all systems
void Dopamine::applyAttentionEffect(NeuralAttentionalSelection* attention, float deltaTime) {
    if (!attention || pImpl->level <= 0.0f) return;
    
    // Dopamine modulates attention strength and saliency
    float attentionModulation = 1.0f + pImpl->level * 0.5f;
    attention->modulateAttentionGain(attentionModulation);
    
    // Novel stimuli get enhanced attention bias
    float noveltySalience = pImpl->level * 0.3f;
    attention->enhanceNovelSalience(noveltySalience);
}

void Dopamine::applyWorkingMemoryEffect(NeuralWorkingMemory* workingMemory, float deltaTime) {
    if (!workingMemory || pImpl->level <= 0.0f) return;
    
    // Dopamine enhances working memory consolidation
    float consolidationStrength = 1.0f + pImpl->level * 0.7f;
    workingMemory->enhanceConsolidation(consolidationStrength);
    
    // Boost encoding of reward-relevant information
    float encodingBoost = 0.5f * pImpl->level;
    workingMemory->boostEncodingRelevance(encodingBoost);
}

void Dopamine::applyEpisodicMemoryEffect(NeuralEpisodicMemory* episodicMemory, float deltaTime) {
    if (!episodicMemory || pImpl->level <= 0.0f) return;
    
    // Dopamine enhances episodic memory storage and retrieval
    float memoryStrength = 1.0f + pImpl->level * 0.4f;
    episodicMemory->enhanceStorageStrength(memoryStrength);
    
    // Strengthen reward-associated memories
    episodicMemory->reinforceRewardAssociations(pImpl->level);
}

void Dopamine::applyActionSelectionEffect(MotorActionSelection* actionSelection, float deltaTime) {
    if (!actionSelection) return;
    
    // Dopamine modulates exploration-exploitation balance via softmax temperature
    pImpl->actionSelectionTemperature = 1.0f / (1.0f + pImpl->level * 2.0f);  // Lower = more exploitation
    actionSelection->setTemperature(pImpl->actionSelectionTemperature);
    
    // Dopamine reinforces previously rewarded actions
    actionSelection->applyDopamineReward(pImpl->level);
}

void Dopamine::applyPredictionSystemEffect(PredictionSystem* predictionSystem, float deltaTime) {
    if (!predictionSystem) return;
    
    // Dopamine affects prediction learning rate and error sensitivity
    float predictionModulation = 1.0f + pImpl->level * 0.8f;
    predictionSystem->modulateLearningRate(predictionModulation);
    
    // Enhanced response to prediction errors
    predictionSystem->enhanceErrorSensitivity(pImpl->level);
}

void Dopamine::applyDevelopmentSystemEffect(DevelopmentSystem* developmentSystem, float deltaTime) {
    if (!developmentSystem) return;
    
    // Dopamine accelerates development through plasticity modulation
    float developmentModulation = 1.0f + pImpl->level * 0.6f;
    developmentSystem->accelerateDevelopment(developmentModulation);
    
    // Dopamine triggers developmental transitions at appropriate levels
    if (pImpl->level > 0.7f && pImpl->level < 0.9f) {
        developmentSystem->triggerTransitionEvent(DevelopmentTransition::CriticalPeriod);
    }
}

} // namespace nlm
