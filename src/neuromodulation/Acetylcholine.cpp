#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionBoost;
    float memoryEnhancement;
    float decayRate;
    float cholinergicSensitivity;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionBoost(0.0f)
        , memoryEnhancement(0.0f)
        , decayRate(0.03f)
        , cholinergicSensitivity(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity - enhances encoding of novel experiences
    return 1.0f + pImpl->level * 0.2f;
}

float Acetylcholine::getAttentionModulator() const {
    // ACh is the primary attention modulator
    return 1.0f + pImpl->level * 0.8f;
}

float Acetylcholine::getMemoryModulator() const {
    // ACh enhances memory consolidation and working memory
    return 1.0f + pImpl->level * 0.6f;
}

float Acetylcholine::getLearningRateModulator() const {
    // ACh supports learning, especially for novel stimuli
    return 1.0f + pImpl->level * 0.4f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->attentionBoost = pImpl->level * 0.8f;
    pImpl->memoryEnhancement = pImpl->level * 0.6f;
}

void Acetylcholine::onSpiking(class Brain* brain, NeuronId neuronId, float current) {
    if (!brain) return;
    
    // ACh enhances firing of neurons that are part of working memory or receiving sensory input
    auto* neuron = brain->getRegion(RegionId(0))->getPopulation(NeuronId(0))->getNeuron(neuronId);
    if (neuron && (neuron->getType() == NeuronType::Sensory || neuron->getState().firingState != FiringState::Refractory)) {
        // Boost firing rate of attended neurons
        float boostFactor = 1.0f + pImpl->level * 0.5f;
        neuron->injectCurrent(current * boostFactor);
    }
}

void Acetylcholine::onMemoryConsolidation(class Brain* brain, float consolidationSignal) {
    if (!brain) return;
    
    // ACh facilitates memory consolidation
    float consolidationBoost = pImpl->level * consolidationSignal;
    
    // Enhance synaptic strengthening during consolidation
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Internal) {
                // Internal neurons are more involved in memory consolidation
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(consolidationBoost * 0.5f);
                }
            }
        }
    }
}

void Acetylcholine::enhanceAttention(class Brain* brain, const std::vector<NeuronId>& neurons) {
    if (!brain) return;
    
    for (NeuronId neuronId : neurons) {
        // Find neuron in brain
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        // Increase excitability
                        neuron->injectCurrent(pImpl->level * 2.0f);
                        break;
                    }
                }
            }
        }
    }
}

void Acetylcholine::facilitateMemoryEncoding(class Brain* brain, const std::vector<NeuronId>& neurons) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Strengthen synapses of attended neurons
    for (NeuronId neuronId : neurons) {
        for (auto& region : brain->getRegions()) {
            auto* neuron = region->getNeuron(neuronId);
            if (neuron) {
                // Enhance the neuron's membrane potential to strengthen connections
                neuron->injectCurrent(pImpl->level * 1.5f);
            }
        }
    }
}

void Acetylcholine::modulateThalamicReticularNucleus(class Brain* brain) {
    // Simplified thalamic modulation
    if (!brain) return;
    
    // Thalamic reticular nucleus modulates sensory gating
    // ACh suppresses TRN, allowing more sensory information to reach cortex
    float thalamicOpening = pImpl->level * 0.7f;
    
    // Effect: more sensory information allowed through to cortex
    // This is simulated by reducing the inhibition on sensory neurons
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    // Reduce inhibition
                    neuron->setFiringState(FiringState::Quiescent);
                }
            }
        }
    }
}

} // namespace nlm
