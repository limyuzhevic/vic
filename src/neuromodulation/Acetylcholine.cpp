#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(0.5f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // ACh enhances memory consolidation and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    // ACh modulates memory consolidation during novel experiences
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::signalNovelExperience(const std::vector<float>& input) {
    // Novel experiences trigger ACh release
    float novelty = std::min(1.0f, static_cast<float>(input.size()) * 0.1f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
}

} // namespace nlm
