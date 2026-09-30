#pragma once

#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Working memory encoding and retrieval
    float encodingStrength;
    float retrievalStrength;
    std::string encodingTarget;
    std::string retrievalTarget;
    
    // Attention and feature binding
    float attentionFocus;
    float featureBindingStrength;
    std::vector<std::string> featureSet;
    
    // Novel experience detection
    float noveltyThreshold;
    std::vector<float> novelExperienceHistory;
    
    // Memory consolidation
    float consolidationStrength;
    float workingMemoryConsolidation;
    float episodicMemoryConsolidation;
    
    // Synaptic plasticity
    std::map<NeuronId, float> synapticTargets;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             encodingStrength(0.0f), retrievalStrength(0.0f), noveltyThreshold(0.5f),
             consolidationStrength(0.0f), workingMemoryConsolidation(0.0f), episodicMemoryConsolidation(0.0f) {}
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
    return 1.0f; // ACh enhances all plasticity
}

void Acetylcholine::update(TimestepDuration dt) {
    // Enhanced ACh dynamics with working memory integration
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Enhanced working memory encoding and retrieval functions
void Acetylcholine::setEncodingTarget(const std::string& target) {
    pImpl->encodingTarget = target;
    pImpl->encodingStrength = 1.0f; // Strong encoding when target is set
}

void Acetylcholine::setRetrievalTarget(const std::string& target) {
    pImpl->retrievalTarget = target;
    pImpl->retrievalStrength = 1.0f; // Strong retrieval when target is set
}

float Acetylcholine::getEncodingStrength() const {
    return pImpl->encodingStrength;
}

float Acetylcholine::getRetrievalStrength() const {
    return pImpl->retrievalStrength;
}

// Enhanced attention and feature binding functions
void Acetylcholine::setFeatureSet(const std::vector<std::string>& features) {
    pImpl->featureSet = features;
    pImpl->featureBindingStrength = std::min(1.0f, static_cast<float>(features.size()) / 10.0f);
}

float Acetylcholine::getFeatureBindingStrength() const {
    return pImpl->featureBindingStrength;
}

float Acetylcholine::getAttentionFocus() const {
    return pImpl->attentionFocus;
}

void Acetylcholine::setAttentionFocus(float focus) {
    pImpl->attentionFocus = std::clamp(focus, 0.0f, 1.0f);
}

// Enhanced novel experience detection functions
bool Acetylcholine::isNovelExperience(float noveltyValue) const {
    return noveltyValue > pImpl->noveltyThreshold;
}

float Acetylcholine::getNoveltyThreshold() const {
    return pImpl->noveltyThreshold;
}

void Acetylcholine::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

// Enhanced memory consolidation functions
void Acetylcholine::consolidateWorkingMemory() {
    pImpl->workingMemoryConsolidation = std::min(1.0f, pImpl->workingMemoryConsolidation + 0.1f);
}

void Acetylcholine::consolidateEpisodicMemory(float strength) {
    pImpl->episodicMemoryConsolidation = std::clamp(strength, 0.0f, 1.0f);
    pImpl->consolidationStrength = (pImpl->consolidationStrength + strength) * 0.5f;
}

float Acetylcholine::getConsolidationStrength() const {
    return pImpl->consolidationStrength;
}

// Enhanced synaptic plasticity functions
float Acetylcholine::getAChPlasticityModulation() const {
    float modulation = 1.0f;
    
    // Increase plasticity for encoding
    modulation += pImpl->encodingStrength * 0.3f;
    
    // Increase plasticity for retrieval
    modulation += pImpl->retrievalStrength * 0.2f;
    
    // Increase plasticity for feature binding
    modulation += pImpl->featureBindingStrength * 0.2f;
    
    // Increase plasticity for consolidation
    modulation += pImpl->consolidationStrength * 0.3f;
    
    return std::min(2.0f, modulation);
}

void Acetylcholine::setSynapticTarget(NeuronId target, float weight) {
    pImpl->synapticTargets[target] = std::clamp(weight, 0.0f, 1.0f);
}

// Cross-system integration functions
void Acetylcholine::integrateWithWorkingMemory(float encoding, float retrieval) {
    setEncodingTarget(""); // Reset target
    setRetrievalTarget(""); // Reset target
    
    if (encoding > 0.0f) {
        setEncodingTarget("working_memory_encode");
    }
    
    if (retrieval > 0.0f) {
        setRetrievalTarget("working_memory_retrieve");
    }
    
    pImpl->encodingStrength = encoding;
    pImpl->retrievalStrength = retrieval;
}

void Acetylcholine::integrateWithEpisodicMemory(float consolidation) {
    consolidateEpisodicMemory(consolidation);
}

void Acetylcholine::integrateWithAttentionalSelection(float focus) {
    setAttentionFocus(focus);
}

void Acetylcholine::integrateWithSensoryProcessing(float binding) {
    std::vector<std::string> features;
    if (binding > 0.0f) {
        features.push_back("visual");
        features.push_back("auditory");
    }
    setFeatureSet(features);
}

} // namespace nlm
