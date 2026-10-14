#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // ACh-specific state
    float attentionBias;          // Attention modulation
    float memoryEncodingGain;     // Memory encoding enhancement
    float learningRate;           // Learning rate modulation
    float corticalExcitability;   // Cortical excitability
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.5f)           // ACh can significantly enhance functions
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionBias(1.0f)
        , memoryEncodingGain(1.0f)
        , learningRate(1.0f)
        , corticalExcitability(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

void Acetylcholine::signalAttentionGain(float gain) {
    // Enhance attentional focus and filtering
    pImpl->attentionBias = std::min(pImpl->peak, pImpl->attentionBias + gain);
}

void Acetylcholine::signalMemoryEncoding(float strength) {
    // Enhance memory encoding consolidation
    pImpl->memoryEncodingGain = std::min(pImpl->peak, pImpl->memoryEncodingGain + strength);
}

void Acetylcholine::signalLearningRate(float rate) {
    // Modulate learning rate for plasticity
    pImpl->learningRate = std::clamp(rate, 0.5f, 2.0f);
}

void Acetylcholine::signalCorticalExcitability(float excitability) {
    // Enhance cortical processing capabilities
    pImpl->corticalExcitability = std::clamp(excitability, 0.5f, 2.0f);
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update attention bias based on ACh level
    pImpl->attentionBias = 1.0f + pImpl->level * 0.5f;
    // Update memory encoding based on ACh level
    pImpl->memoryEncodingGain = 1.0f + pImpl->level * 0.3f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for learning and memory
    return 0.7f + 0.3f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay ACh level
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Get current attention bias
float Acetylcholine::getAttentionBias() const {
    return pImpl->attentionBias;
}

// Get memory encoding enhancement
float Acetylcholine::getMemoryEncodingGain() const {
    return pImpl->memoryEncodingGain;
}

// Get learning rate modulation
float Acetylcholine::getLearningRate() const {
    return pImpl->learningRate;
}

// Get cortical excitability
float Acetylcholine::getCorticalExcitability() const {
    return pImpl->corticalExcitability;
}

} // namespace nlm