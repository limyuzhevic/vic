#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float stress;
    float decayRate;
    float modulationRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousal(0.0f)
        , vigilance(0.0f)
        , stress(0.0f)
        , decayRate(0.04f)
        , modulationRate(0.1f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal
    return 1.0f + pImpl->level * 0.3f;
}

float Norepinephrine::getAttentionModulator() const {
    // NE enhances attention and signal-to-noise ratio
    return 1.0f + pImpl->level * 0.5f;
}

float Norepinephrine::getMemoryModulator() const {
    // NE affects different types of memory differently
    return 1.0f + pImpl->level * 0.2f;
}

float Norepinephrine::getLearningRateModulator() const {
    // NE enhances learning during novel or stressful situations
    return 1.0f + pImpl->level * 0.6f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->arousal = pImpl->level * 0.7f;
    pImpl->vigilance = pImpl->level * 0.5f;
    pImpl->stress = pImpl->level * 0.3f;
}

void Norepinephrine::onSpiking(class Brain* brain, NeuronId neuronId, float current) {
    if (!brain) return;
    
    // NE increases neural gain - amplifies firing rates
    auto* neuron = brain->getRegion(RegionId(0))->getPopulation(NeuronId(0))->getNeuron(neuronId);
    if (neuron) {
        // Increase membrane potential to make neuron more likely to fire
        float gainBoost = 1.0f + pImpl->level * 0.3f;
        neuron->injectCurrent(current * gainBoost);
    }
}

void Norepinephrine::onMemoryConsolidation(class Brain* brain, float consolidationSignal) {
    if (!brain) return;
    
    // NE enhances consolidation of emotionally salient memories
    float consolidationBoost = pImpl->level * consolidationSignal * 0.8f;
    
    // Strengthen memory traces
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Internal) {
                // Internal neurons integrate stress/arousal signals
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(consolidationBoost * 0.3f);
                }
            }
        }
    }
}

void Norepinephrine::increaseAlertness(class Brain* brain) {
    if (!brain) return;
    
    // Increase arousal - makes neurons more responsive
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Decrease threshold for firing
                if (neuron->getState().threshold > 0.5f) {
                    neuron->setThreshold(neuron->getState().threshold * 0.9f);
                }
            }
        }
    }
}

void Norepinephrine::enhanceLTMConsolidation(class Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // NE enhances long-term memory consolidation, especially for emotional memories
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Internal) {
                // Internal neurons are key for consolidation
                for (auto* neuron : pop->getNeurons()) {
                    // Stronger persistent activity for memory
                    neuron->injectCurrent(pImpl->level * 2.0f);
                }
            }
        }
    }
}

void Norepinephrine::modulateMonoaminergicModulation(class Brain* brain) {
    if (!brain) return;
    
    // NE interacts with other monoamine systems
    float totalModulation = pImpl->level * 0.4f;
    
    // Enhance dopamine-dopamine interactions
    // This is simulated by increasing dopamine effects
    for (auto& region : brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // NE enhances synaptic efficacy
            float weight = syn->getWeight();
            syn->setWeight(weight * (1.0f + totalModulation * 0.1f));
        }
    }
}

} // namespace nlm
