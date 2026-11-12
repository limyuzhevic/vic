#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(0.8f)
        , decayRate(0.15f)
        , attentionGain(1.0f) {}
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
    // Acetylcholine enhances memory consolidation and attention
    // The effect on plasticity is complex: moderate levels improve learning,
    // high levels may saturate plasticity (inverted U-shaped relationship)
    float level = pImpl->level;
    // Inverted U-shaped relationship for plasticity enhancement
    return 0.5f + 0.5f * level - 0.2f * level * level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real acetylcholine dynamics with realistic time constants
    static constexpr float timeConstant = 50.0f;  // 50 ms time constant
    float decayFactor = 1.0f - std::exp(-pImpl->decayRate * static_cast<float>(dt) / timeConstant);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}

void Acetylcholine::triggerMemoryConsolidation() {
    // Transient burst of acetylcholine to trigger memory consolidation
    float consolidationBurst = 0.6f + std::sin(static_cast<float>(std::time(nullptr)) % 6.28f) * 0.4f;
    setLevel(std::min(consolidationBurst, 1.0f));
    NLM_LOG_INFO("Acetylcholine-triggered memory consolidation burst: " + std::to_string(consolidationBurst));
}

float Acetylcholine::getAChLevel() const {
    return getLevel();
}

float Acetylcholine::getAChBaseline() const {
    return pImpl->baseline;
}

void Acetylcholine::setAChBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

} // namespace nlm
