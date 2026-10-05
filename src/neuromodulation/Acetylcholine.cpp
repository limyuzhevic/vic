#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float cholinergicTone;
    float memoryEnhancement;
    float encodingBoost;
    float decayRate;
    float baselineLevel;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    std::vector<float> history;
    std::vector<float> recentInputs;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , cholinergicTone(0.0f)
        , memoryEnhancement(1.0f)
        , encodingBoost(1.0f)
        , decayRate(0.05f)
        , baselineLevel(0.1f)
        , dist(0.0f, 1.0f) {
        rng.seed(42);
    }
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->cholinergicTone = 0.0f;
    pImpl->memoryEnhancement = 1.0f;
    pImpl->encodingBoost = 1.0f;
    pImpl->recentInputs.clear();
    pImpl->history.clear();
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attention, memory encoding, and cognitive control");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    updateEffects();
}

void Acetylcholine::signalSensoryInput(float inputStrength, float novelty) {
    // Strong sensory inputs trigger cholinergic responses
    pImpl->level += inputStrength * 0.5f;
    
    if (novelty > 0.5f) {
        // Novel inputs cause phasic bursts
        pImpl->level += 0.5f + novelty * 0.5f;
    }
    
    pImpl->level = std::min(2.0f, pImpl->level);
    
    // Store input for memory enhancement calculation
    pImpl->recentInputs.push_back(inputStrength);
    if (pImpl->recentInputs.size() > 50) {
        pImpl->recentInputs.erase(pImpl->recentInputs.begin());
    }
}

void Acetylcholine::signalWorkingMemoryAccess(NeuronId neuron, float activation) {
    // Acetylcholine enhances working memory retrieval
    if (activation > 0.3f) {
        // Strong activation boosts cholinergic tone
        pImpl->cholinergicTone = std::min(1.0f, pImpl->cholinergicTone + activation * 0.2f);
        
        // Update memory enhancement factor
        pImpl->memoryEnhancement = 1.0f + activation * 0.5f;
    }
}

void Acetylcholine::updateAttention(float topDownBias, float bottomUpSalience, float goalRelevance) {
    // Integrate multiple attention signals to modulate cholinergic activity
    float integratedAttention = topDownBias * 0.3f + 
                               bottomUpSalience * 0.4f + 
                               goalRelevance * 0.3f;
    
    // Cholinergic tone represents integrated attentional state
    pImpl->cholinergicTone = integratedAttention;
}

void Acetylcholine::updateEncoding(float inputPatternStrength) {
    // Enhance encoding of new information
    pImpl->encodingBoost = 1.0f + inputPatternStrength * 0.8f;
    
    // This will be used by episodic memory during encoding
    if (brain_) {
        // Signal encoding enhancement to brain
        // (Implementation details depend on Brain interface)
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attended and novel inputs
    return 0.9f + 0.6f * pImpl->cholinergicTone;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay of cholinergic signals
    float targetLevel = pImpl->baselineLevel;
    pImpl->level += (targetLevel - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay cholinergic tone
    pImpl->cholinergicTone *= 0.95f;
    
    // Update derived measures
    updateEffects();
}

void Acetylcholine::updateEffects() {
    // Memory enhancement based on cholinergic level
    pImpl->memoryEnhancement = 1.0f + pImpl->level * 0.7f;
    
    // Encoding boost based on cholinergic tone
    pImpl->encodingBoost = 1.0f + pImpl->cholinergicTone * 0.8f;
}

float Acetylcholine::getMemoryEnhancement() const {
    return pImpl->memoryEnhancement;
}

float Acetylcholine::getEncodingBoost() const {
    return pImpl->encodingBoost;
}

const std::vector<float>& Acetylcholine::getHistory() const {
    return pImpl->history;
}

void Acetylcholine::addToHistory(float value) {
    pImpl->history.push_back(value);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->cholinergicTone = 0.0f;
    pImpl->memoryEnhancement = 1.0f;
    pImpl->encodingBoost = 1.0f;
    pImpl->recentInputs.clear();
    pImpl->history.clear();
}

} // namespace nlm