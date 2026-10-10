#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <unordered_map>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float moodLevel;
    float impulsivity;
    std::unordered_map<uint64_t, float> serotoninForNeuron;
    
    Impl() 
        : level(0.0f), moodLevel(0.5f), impulsivity(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay level over time
    pImpl->level = std::max(0.0f, pImpl->level - 0.01f * static_cast<float>(dt));
}

void Serotonin::modulateMood(float moodAdjustment) {
    pImpl->moodLevel = std::clamp(pImpl->moodLevel + moodAdjustment, 0.0f, 1.0f);
    pImpl->level = std::min(1.0f, pImpl->level + moodAdjustment * 0.3f);
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.0f, 1.0f);
}

float Serotonin::getSerotoninFor(const NeuronId& neuronId) const {
    auto it = pImpl->serotoninForNeuron.find(neuronId.value);
    return it != pImpl->serotoninForNeuron.end() ? it->second : 0.0f;
}

} // namespace nlm