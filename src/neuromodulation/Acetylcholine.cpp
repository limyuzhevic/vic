#include "../neuromodulation/Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float recoveryRate;
    float attentionGain;
    float learningModulation;
    float workingMemoryActivation;
    float noveltySensitivity;
    std::vector<float> history;
    float locusCoeruleusInput;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.08f), recoveryRate(0.05f),
             attentionGain(1.0f), learningModulation(1.0f), workingMemoryActivation(0.5f),
             noveltySensitivity(0.5f), locusCoeruleusInput(0.0f) {}
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
    // Update related modulations
    pImpl->attentionGain = std::min(2.0f, 1.0f + pImpl->level * 1.5f);
    pImpl->learningModulation = 0.8f + 0.4f * pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine facilitates LTP (learning) and promotes memory encoding
    // Higher levels enhance plasticity for encoding new information
    float basePlasticity = 0.6f + 0.8f * pImpl->level;
    return std::min(1.5f, std::max(0.3f, basePlasticity * pImpl->attentionGain));
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Recovery dynamics and theta phase coupling
    float recovery = pImpl->recoveryRate * static_cast<float>(dt);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Acetylcholine::encodeWorkingMemory(float activation, float novelty, float thetaPhase) {
    // ACh release is enhanced by novelty during theta phase
    // This facilitates working memory encoding
    float encodingBoost = 0.0f;
    
    // Novelty detection enhances ACh release
    if (novelty > 0.3f) {
        encodingBoost += (novelty - 0.3f) * 2.0f * thetaPhase;
    }
    
    // Working memory activation response
    encodingBoost += activation * 0.5f * (1.0f - thetaPhase);
    
    // Apply boost
    setLevel(pImpl->level + encodingBoost * 0.01f);
    pImpl->workingMemoryActivation = std::min(1.0f, pImpl->workingMemoryActivation + activation * 0.02f);
}

void Acetylcholine::consolidateWorkingMemory() {
    // During offline periods (theta-gamma coupling), ACh promotes consolidation
    // This strengthens working memory traces
    if (pImpl->workingMemoryActivation > 0.3f) {
        float consolidationBoost = pImpl->workingMemoryActivation * 0.3f;
        setLevel(pImpl->level + consolidationBoost * 0.02f);
        pImpl->workingMemoryActivation = std::max(0.0f, pImpl->workingMemoryActivation - consolidationBoost * 0.1f);
    }
}

void Acetylcholine::modulateAttention(float sensoryInput, float topDownSignal) {
    // ACh modulates attentional gain based on both bottom-up and top-down signals
    float attentionMod = (sensoryInput * 0.6f + topDownSignal * 0.4f) * 0.01f;
    setLevel(pImpl->level + attentionMod);
    
    // Update attention gain factor
    pImpl->attentionGain = 1.0f + pImpl->level * 1.5f;
}

void Acetylcholine::modulateLearning(float learningRate, float predictionError) {
    // ACh modulates learning: enhances encoding of novel information, stabilizes existing memories
    if (predictionError != 0.0f) {
        // Novel prediction errors trigger ACh release
        float learningBoost = std::abs(predictionError) * 0.5f;
        setLevel(pImpl->level + learningBoost * 0.02f);
    }
    
    // Update learning modulation factor
    pImpl->learningModulation = 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::facilitateEncoding(float memoryStrength) {
    // ACh facilitates encoding of new information, especially when memory strength is low
    if (memoryStrength < 0.3f) {
        float encodingBoost = (0.3f - memoryStrength) * 2.0f;
        setLevel(pImpl->level + encodingBoost * 0.015f);
    }
}

void Acetylcholine::facilitateRetrieval(float memoryPriority) {
    // ACh also modulates retrieval, but with inverted relationship to encoding
    float retrievalMod = memoryPriority * 0.5f;
    // Note: ACh level affects retrieval through attention gain
}

void Acetylcholine::setLocusCoeruleusInput(float arousal, float novelty, float error) {
    // ACh interacts with locus coeruleus system
    pImpl->locusCoeruleusInput = (arousal * 0.4f + novelty * 0.4f + std::abs(error) * 0.2f);
}

} // namespace nlm