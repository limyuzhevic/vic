#include "Acetylcholine.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    Brain* brain;
    float attentionGain;
    float memoryConsolidationRate;
    std::vector<float> attentionFocus;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f), brain(nullptr),
             attentionGain(1.5f), memoryConsolidationRate(0.8f) {}
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
    // ACh enhances synaptic plasticity for attended information
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    if (brain) {
        pImpl->baseline = 0.05f;
    }
}

void Acetylcholine::focusAttention(const std::vector<float>& features) {
    // Normalize features to determine attention focus
    pImpl->attentionFocus = features;
    float maxFeature = 0.0f;
    for (float feat : features) {
        maxFeature = std::max(maxFeature, feat);
    }
    if (maxFeature > 0.0f) {
        for (float& feat : pImpl->attentionFocus) {
            feat /= maxFeature;
        }
    }
    
    // Increase ACh level based on attention focus
    float attentionStrength = std::accumulate(pImpl->attentionFocus.begin(), 
                                            pImpl->attentionFocus.end(), 0.0f) / 
                            pImpl->attentionFocus.size();
    setLevel(std::min(1.0f, getLevel() + attentionStrength * pImpl->releaseRate * 0.5f));
}

void Acetylcholine::consolidateMemory() {
    if (!pImpl->brain) return;
    
    // ACh enhances working memory consolidation
    NeuralWorkingMemory* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        // Increase consolidation rate based on ACh level
        float consolidationBoost = getLevel() * pImpl->memoryConsolidationRate;
        // In real implementation, this would affect memory consolidation timing
        NLM_LOG_INFO("Acetylcholine: Enhancing memory consolidation (boost: " + 
                    std::to_string(consolidationBoost) + ")");
    }
}

float Acetylcholine::getAttentionModulation() const {
    // ACh directly enhances attentional processes
    return 1.0f + pImpl->attentionGain * getLevel();
}

float Acetylcholine::getMemoryModulation() const {
    // ACh enhances memory encoding and consolidation
    return 1.0f + pImpl->memoryConsolidationRate * getLevel();
}

} // namespace nlm