#include "Acetylcholine.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory encoding strength
    float memoryEncodingStrength;
    float encodingSaturation;
    std::unordered_map<uint64_t, float> encodingTags; // NeuronId -> encoding tag
    
    // Attention gating
    std::unordered_map<uint64_t, float> attentionGating; // NeuronId -> gating strength
    
    // Feature binding
    std::vector<std::vector<uint64_t>> activeFeatureGroups;
    float bindingThreshold;
    
    // Reward prediction error modulation
    float rpeModulation;
    std::deque<float> rpeHistory;
    size_t maxRPEHistorySize;
    
    // Sleep-wake cycle
    bool isAwake;
    float sleepState;
    float wakeState;
    float sleepDecayRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , memoryEncodingStrength(1.0f), encodingSaturation(0.95f)
        , bindingThreshold(0.7f), isAwake(true)
        , sleepState(0.0f), wakeState(1.0f), sleepDecayRate(0.05f)
        , maxRPEHistorySize(50)
        , rpeModulation(1.0f) {}
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
    // ACh modulates plasticity based on memory encoding needs
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryEncodingStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Sleep-wake cycle dynamics
    if (pImpl->isAwake) {
        pImpl->sleepState = std::max(0.0f, pImpl->sleepState - pImpl->sleepDecayRate * static_cast<float>(dt));
        pImpl->wakeState = std::min(1.0f, 1.0f - pImpl->sleepState);
    } else {
        pImpl->wakeState = std::max(0.0f, pImpl->wakeState - pImpl->sleepDecayRate * static_cast<float>(dt));
        pImpl->sleepState = std::min(1.0f, 1.0f - pImpl->wakeState);
    }
    
    // Clean up RPE history
    while (pImpl->rpeHistory.size() > pImpl->maxRPEHistorySize) {
        pImpl->rpeHistory.pop_front();
    }
}

void Acetylcholine::enhanceMemoryEncoding(float encodingStrength) {
    pImpl->memoryEncodingStrength = std::clamp(encodingStrength, 0.0f, pImpl->encodingSaturation);
}

float Acetylcholine::getMemoryEncodingStrength() const {
    return pImpl->memoryEncodingStrength;
}

void Acetylcholine::gateSensoryInput(NeuronId neuron, float gatingStrength) {
    pImpl->attentionGating[neuron.index()] = std::clamp(gatingStrength * pImpl->level, 0.0f, 1.0f);
}

void Acetylcholine::facilitateFeatureBinding(const std::vector<NeuronId>& neuronIds) {
    // Create feature binding groups
    if (neuronIds.size() >= 2) {
        pImpl->activeFeatureGroups.push_back(std::vector<uint64_t>());
        auto& group = pImpl->activeFeatureGroups.back();
        for (const auto& neuron : neuronIds) {
            group.push_back(neuron.index());
        }
    }
}

void Acetylcholine::modulateRewardPredictionError(float predictionError) {
    pImpl->rpeModulation = 1.0f + predictionError * 0.5f; // Enhance or dampen RPE signaling
    pImpl->rpeHistory.push_back(predictionError);
}

void Acetylcholine::enterSleepState() {
    pImpl->isAwake = false;
}

void Acetylcholine::enterWakeState() {
    pImpl->isAwake = true;
}

void Acetylcholine::updateCholinergicTone(bool isActive, float attentionLevel) {
    if (isActive) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + attentionLevel * pImpl->releaseRate);
    }
}

} // namespace nlm
