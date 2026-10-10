#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialModulation;
    float decayRate;
    float firingRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(0.0f)
        , impulsivity(0.0f)
        , socialModulation(0.0f)
        , decayRate(0.035f)
        , firingRate(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity patterns
    return 1.0f + pImpl->level * 0.2f;
}

float Serotonin::getAttentionModulator() const {
    // Serotonin affects attention - high levels can reduce impulsivity
    return 1.0f - pImpl->level * 0.3f;
}

float Serotonin::getMemoryModulator() const {
    // Serotonin affects mood-related memory
    return 1.0f + pImpl->level * 0.4f;
}

float Serotonin::getLearningRateModulator() const {
    // Serotonin supports pattern learning and generalization
    return 1.0f + pImpl->level * 0.3f;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->mood = pImpl->level * 0.6f;
    pImpl->impulsivity = std::max(0.0f, 1.0f - pImpl->level * 0.5f);
    pImpl->socialModulation = pImpl->level * 0.4f;
}

void Serotonin::onSpiking(class Brain* brain, NeuronId neuronId, float current) {
    if (!brain) return;
    
    // Serotonin generally decreases overall excitability
    // but can enhance pattern learning
    auto* neuron = brain->getRegion(RegionId(0))->getPopulation(NeuronId(0))->getNeuron(neuronId);
    if (neuron) {
        // Reduce firing probability for high serotonin
        float firingMod = 1.0f - pImpl->level * 0.2f;
        
        // But enhance pattern detection
        if (neuron->getType() == NeuronType::Internal) {
            // Internal neurons learn patterns better with serotonin
            neuron->injectCurrent(current * firingMod * 0.9f);
        } else {
            neuron->injectCurrent(current * firingMod);
        }
    }
}

void Serotonin::onMemoryConsolidation(class Brain* brain, float consolidationSignal) {
    if (!brain) return;
    
    // Serotonin enhances consolidation of learned patterns
    float consolidationBoost = pImpl->level * consolidationSignal * 0.5f;
    
    // Support pattern stabilization
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Internal) {
                // Internal neurons integrate serotonergic modulation
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(consolidationBoost * 0.4f);
                }
            }
        }
    }
}

void Serotonin::modulateAggression(class Brain* brain) {
    if (!brain) return;
    
    // Serotonin reduces aggression and impulsivity
    float aggressionMod = std::max(0.0f, 1.0f - pImpl->level * 0.8f);
    
    // Reduce excitation of motor neurons
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    // Reduce motor neuron excitability
                    neuron->setThreshold(neuron->getState().threshold * aggressionMod);
                }
            }
        }
    }
}

void Serotonin::supportPatternLearning(class Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Serotonin supports pattern learning and generalization
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Internal) {
                // Internal neurons learn patterns better with serotonin
                for (auto* neuron : pop->getNeurons()) {
                    // Enhance pattern storage
                    neuron->injectCurrent(pImpl->level * 1.5f);
                }
            }
        }
    }
}

void Serotonin::regulateSleepWakeCycle(class Brain* brain) {
    if (!brain) return;
    
    // Serotonin helps regulate sleep-wake cycle
    // High levels promote wakefulness, low levels promote sleep
    float wakePromotion = pImpl->level * 0.5f;
    
    // Effect on neural circuits
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                // Sensory processing modulated by serotonin
                for (auto* neuron : pop->getNeurons()) {
                    // Enhance sensory processing during wakefulness
                    neuron->injectCurrent(wakePromotion * 0.3f);
                }
            }
        }
    }
}

} // namespace nlm
