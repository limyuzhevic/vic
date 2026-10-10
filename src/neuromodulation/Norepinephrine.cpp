#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <unordered_map>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilance;
    float vigilanceThreshold;
    std::unordered_map<uint64_t, float> arousalForNeuron;
    
    Impl() 
        : level(0.0f), arousalLevel(0.5f), vigilance(0.5f), vigilanceThreshold(0.5f) {}
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
    return 1.0f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay level over time
    pImpl->level = std::max(0.0f, pImpl->level - 0.01f * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + amount);
    pImpl->level = std::min(1.0f, pImpl->level + amount * 0.5f);
}

void Norepinephrine::decreaseArousal(float amount) {
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - amount);
    pImpl->level = std::max(0.0f, pImpl->level - amount * 0.3f);
}

void Norepinephrine::setVigilanceThreshold(float threshold) {
    pImpl->vigilanceThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Norepinephrine::getArousalFor(const NeuronId& neuronId) const {
    auto it = pImpl->arousalForNeuron.find(neuronId.value);
    return it != pImpl->arousalForNeuron.end() ? it->second : 0.0f;
}

} // namespace nlm