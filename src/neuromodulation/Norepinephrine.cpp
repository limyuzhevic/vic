#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float workingMemoryBoost;
    float arousalLevel;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , decayRate(0.03f)
        , releaseRate(1.0f)
        , workingMemoryBoost(0.8f)
        , arousalLevel(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances working memory and arousal
    // Higher NE improves attention and signal-to-noise ratio
    float attentionBoost = 1.0f + pImpl->level * 0.3f;
    float workingMemoryEnhancement = 1.0f + pImpl->level * pImpl->workingMemoryBoost;
    return std::min(3.0f, attentionBoost * workingMemoryEnhancement);
}

void Norepinephrine::update(TimestepDuration dt) {
    if (pImpl->brain) {
        // NE modulates neural excitability and working memory
        float norepinephrineLevel = pImpl->level;
        pImpl->arousalLevel = norepinephrineLevel * 0.5f + 0.5f;
        
        // Find all neurons and modulate their properties
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Modulate neural excitability based on arousal
                    if (norepinephrineLevel > 0.3f) {
                        // Increase firing rate and reduce threshold
                        float oldThreshold = neuron->getState().threshold;
                        float newThreshold = oldThreshold * (1.0f - norepinephrineLevel * 0.3f);
                        neuron->setThreshold(newThreshold);
                        
                        // Boost leak conductance for faster dynamics
                        float oldLeak = neuron->getState().leakConductance;
                        neuron->setLeakConductance(oldLeak * (1.0f + norepinephrineLevel * 0.2f));
                    }
                }
            }
        }
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Boost NE level for increased arousal and vigilance
    setLevel(std::min(1.0f, pImpl->level + arousal * pImpl->releaseRate * 0.5f));
}

void Norepinephrine::signalWorkingMemoryTask() {
    // Enhance working memory encoding
    setLevel(std::min(1.0f, pImpl->level * 1.3f));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

} // namespace nlm
