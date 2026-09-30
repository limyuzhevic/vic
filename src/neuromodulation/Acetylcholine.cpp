#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

Acetylcholine::Acetylcholine()
    : level(0.0f)
    , baseline(0.0f)
    , peak(1.0f)
    , decayRate(0.1f)
    , releaseRate(1.0f)
    , acetylcholineBoost(0.0f)
{
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return level;
}

void Acetylcholine::setLevel(float lvl) {
    level = std::clamp(lvl, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Implement real acetylcholine effects
    // ACh enhances working memory and attention plasticity
    float memoryBoost = std::min(1.0f, level * 1.5f);
    float attentionBoost = std::min(1.0f, level * 2.0f);
    
    // Combine with cholinergic effects on consolidation
    float consolidation = std::min(1.0f, acetylcholineBoost * 0.8f);
    
    return 0.5f + memoryBoost * 0.3f + attentionBoost * 0.2f + consolidation * 0.2f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    level = std::max(baseline, level - decayRate * static_cast<float>(dt));
    acetylcholineBoost = std::max(0.0f, acetylcholineBoost - decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalMemoryConsolidation() {
    // Burst of acetylcholine on memory consolidation events
    acetylcholineBoost = std::min(1.0f, acetylcholineBoost + 0.5f);
    level = std::min(peak, level + 0.3f);
}

void Acetylcholine::signalAttentionShift() {
    // Acetylcholine signals focus shifts and attention
    level = std::min(peak, level + 0.2f);
    acetylcholineBoost = std::min(1.0f, acetylcholineBoost + 0.3f);
}

void Acetylcholine::signalNovelty() {
    // ACh responds to novel stimuli
    level = std::min(peak, level + 0.15f);
}

float Acetylcholine::getAttentionEnhancement() const {
    // Return attention enhancement factor for neuromodulation
    return 1.0f + level * 1.5f;
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    // Return memory consolidation strength
    return 0.5f + acetylcholineBoost * 1.0f;
}

} // namespace nlm