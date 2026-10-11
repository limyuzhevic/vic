#pragma once

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionWeight;
    float noveltyThreshold;
    float decayRate;
    float baseline;
    float consolidationBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionWeight(1.0f)
        , noveltyThreshold(0.5f)
        , decayRate(0.05f)
        , baseline(0.1f)
        , consolidationBoost(2.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::update(float novelty, float predictionError, TimestepDuration dt) {
    // Acetylcholine responds to novelty and attention demands
    float attentionDemand = std::min(novelty, predictionError);
    
    // Threshold-based activation: only increase if above threshold
    if (attentionDemand > pImpl->noveltyThreshold) {
        // Strong increase based on attention demand
        pImpl->level += (attentionDemand - pImpl->level) * pImpl->attentionWeight;
    } else {
        // Gentle increase based on novelty
        pImpl->level += (novelty - pImpl->level) * 0.3f;
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getPlasticityFactor() const {
    // Higher ACh increases plasticity for learning novel information
    // Moderate levels promote exploration, high levels promote consolidation
    if (pImpl->level < 0.3f) {
        return 1.0f;  // Baseline plasticity
    } else if (pImpl->level < 0.7f) {
        return 1.5f;  // Enhanced plasticity for learning
    } else {
        return 2.0f;  // Strong consolidation, focused learning
    }
}

void Acetylcholine::enhanceAttention(NeuronId targetNeuron) {
    // Modulate target neuron's excitability
    if (pImpl->brain) {
        pImpl->brain->injectCurrent(targetNeuron, 5.0f * pImpl->level);
    }
}

void Acetylcholine::boostMemoryConsolidation(TimestepDuration dt) {
    // Boost memory consolidation based on ACh level
    if (pImpl->level > 0.5f && pImpl->brain) {
        // Enhance synaptic strengthening in memory systems
        pImpl->brain->develop();
    }
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Acetylcholine::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Acetylcholine::setConsolidationBoost(float boost) {
    pImpl->consolidationBoost = boost;
}

float Acetylcholine::getAttentionWeight() const {
    return pImpl->attentionWeight;
}

float Acetylcholine::getNoveltyThreshold() const {
    return pImpl->noveltyThreshold;
}

float Acetylcholine::getConsolidationBoost() const {
    return pImpl->consolidationBoost;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm