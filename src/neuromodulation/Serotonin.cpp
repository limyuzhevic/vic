#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float satietyLevel;
    float socialModulation;
    float impulsivityMod;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , decayRate(0.02f)
        , releaseRate(1.0f)
        , satietyLevel(0.3f)
        , socialModulation(0.5f)
        , impulsivityMod(0.5f) {}
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
    // 5-HT modulates social behavior, satiety, and impulsivity
    // Higher 5-HT reduces impulsivity and promotes stability
    float satietyEffect = 1.0f - pImpl->level * pImpl->satietyLevel;
    float socialEffect = 1.0f + pImpl->level * pImpl->socialModulation;
    float impulsivityReduction = 1.0f - pImpl->level * pImpl->impulsivityMod;
    return satietyEffect * socialEffect * impulsivityReduction;
}

void Serotonin::update(TimestepDuration dt) {
    if (pImpl->brain) {
        // 5-HT modulates neural excitability for stable behavior
        float serotoninLevel = pImpl->level;
        
        // Find all neurons and modulate their properties
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 5-HT typically has inhibitory effects
                    if (serotoninLevel > 0.3f) {
                        // Increase threshold for stable firing
                        float oldThreshold = neuron->getState().threshold;
                        float newThreshold = oldThreshold * (1.0f + serotoninLevel * 0.3f);
                        neuron->setThreshold(newThreshold);
                        
                        // Increase reset potential for stability
                        float oldReset = neuron->getState().resetPotential;
                        neuron->setResetPotential(oldReset * (1.0f + serotoninLevel * 0.2f));
                    }
                }
            }
        }
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSatiety(float satiety) {
    // Boost 5-HT level for satiety and reward evaluation
    setLevel(std::min(1.0f, pImpl->level + satiety * pImpl->releaseRate * 0.5f));
}

void Serotonin::signalSocialBehavior() {
    // Enhance social behavior modulation
    setLevel(std::min(1.0f, pImpl->level * 1.2f));
}

void Serotonin::signalReward(float reward) {
    // 5-HT responds to reward prediction errors
    if (reward > 0.0f) {
        setLevel(std::min(1.0f, pImpl->level + reward * pImpl->releaseRate * 0.3f));
    }
}

float Serotonin::getSatietyLevel() const {
    return pImpl->satietyLevel;
}

} // namespace nlm
