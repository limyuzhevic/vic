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
    float plasticityBoost;
    float attentionModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.05f)  // Baseline level
        , baseline(0.05f)
        , peak(0.3f)
        , decayRate(0.02f)
        , releaseRate(0.5f)
        , plasticityBoost(1.5f)
        , attentionModulation(1.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized for attention and memory");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

void Acetylcholine::enhanceAttention(float amount) {
    pImpl->attentionModulation = 1.0f + amount;
}

void Acetylcholine::boostMemoryEncoding(float amount) {
    // Store for use in memory systems
    if (pImpl->brain) {
        // TODO: Integrate with memory systems
        // For now, just log the effect
        NLM_LOG_INFO(std::string("Acetylcholine boosting memory encoding by ") + std::to_string(amount));
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for learning and memory
    // Higher ACh levels increase plasticity
    float factor = 1.0f + pImpl->plasticityBoost * pImpl->level;
    return std::clamp(factor, 0.5f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: baseline with task-dependent bursts
    float targetLevel = pImpl->baseline;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise
    static float noise = 0.0f;
    noise = noise * 0.95f + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    pImpl->level += noise * 0.1f;
}

void Acetylcholine::signalAttentionDemand(float demand) {
    // ACh responds to attention demands (learning, novelty)
    if (demand > 0.0f) {
        float burst = std::min(0.1f, demand * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Boost memory encoding
        boostMemoryEncoding(demand * 0.5f);
    }
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionModulation = 1.2f;
}

} // namespace nlm