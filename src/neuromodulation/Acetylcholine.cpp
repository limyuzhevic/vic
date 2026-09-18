#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float signalToNoiseRatio;
    float workingMemoryBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , signalToNoiseRatio(1.0f)
        , workingMemoryBoost(1.0f) {}
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
    // ACh affects signal-to-noise ratio and working memory
    pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 3.0f;
    pImpl->workingMemoryBoost = 1.0f + pImpl->level * 2.0f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related processes
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // ACh can enhance attention through the attention system
    if (pImpl->brain && pImpl->brain->getAttention()) {
        // ACh increases attentional focus and signal-to-noise ratio
        pImpl->brain->getAttention()->setExcitationStrength(1.0f + pImpl->level * 2.0f);
        pImpl->brain->getAttention()->setInhibitionStrength(0.5f / (1.0f + pImpl->level));
    }
}

void Acetylcholine::enhanceAttention(float amount) {
    if (pImpl->brain && pImpl->brain->getAttention()) {
        float currentStrength = pImpl->brain->getAttention()->getExcitationStrength();
        pImpl->brain->getAttention()->setExcitationStrength(currentStrength + amount * 0.5f);
    }
}

void Acetylcholine::boostMemoryConsolidation() {
    if (pImpl->brain) {
        // Store current state in episodic memory for consolidation
        if (pImpl->brain->getEpisodicMemory() && pImpl->brain->getWorkingMemory()) {
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->brain->getCurrentStep();
            episode.reward = pImpl->level;
            
            // Capture current neural activity
            for (const auto& region : pImpl->brain->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 3.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 15.0f);
                        }
                    }
                }
            }
            
            pImpl->brain->getEpisodicMemory()->storeEpisode(episode);
        }
    }
}

} // namespace nlm