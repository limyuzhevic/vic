#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidationRate;
    float attentionGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , memoryConsolidationRate(0.5f)
        , attentionGain(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

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
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalNovelty(float novelty) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemory(const std::vector<NeuronId>& neurons, float strength) {
    if (!pImpl->brain || neurons.empty()) return;
    
    float modulation = pImpl->level * pImpl->memoryConsolidationRate;
    for (NeuronId neuronId : neurons) {
        pImpl->brain->injectCurrent(neuronId, modulation * strength);
    }
}

void Acetylcholine::modulateAttention(float gain) {
    pImpl->attentionGain = std::clamp(gain * pImpl->level, 0.1f, 2.0f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

} // namespace nlm
