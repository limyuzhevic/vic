#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodLevel;
    float decayRate;
    float enhancementRate;
    float baselineMood;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , moodLevel(0.5f)
        , decayRate(0.04f)
        , enhancementRate(1.2f)
        , baselineMood(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates emotional valence and stabilizes networks
    // Higher 5-HT reduces anxiety and stabilizes plasticity
    return 0.9f - 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood level based on serotonin concentration
    pImpl->moodLevel = pImpl->baselineMood + pImpl->level * 0.5f;
}

void Serotonin::improveMood(float moodBoost) {
    pImpl->moodLevel += moodBoost * pImpl->enhancementRate;
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel);
    pImpl->level = std::min(2.0f, pImpl->level + moodBoost);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

} // namespace nlm
