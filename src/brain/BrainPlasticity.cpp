// BrainPlasticity.cpp - Plasticity system integration implementation
#include "BrainPlasticity.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainPlasticity::BrainPlasticity() : brain_(nullptr), stdp_(nullptr), 
                                  hebbian_(nullptr), structuralPlasticity_(nullptr) {}

BrainPlasticity::~BrainPlasticity() = default;

void BrainPlasticity::applyNeuromodulation(const Neuromodulator& signal) {
    if (!brain_ || !brain_->getSTDP()) {
        NLM_LOG_WARNING("BrainPlasticity: Cannot apply neuromodulation - systems not available");
        return;
    }
    
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    brain_->getSTDP()->setLTPWeight(0.01f * modulation);
    brain_->getSTDP()->setLTDWeight(0.012f * modulation);
    
    NLM_LOG_DEBUG("BrainPlasticity: Applied neuromodulation with level " + 
               std::to_string(modulation));
}

float BrainPlasticity::getExcitationInhibitionRatio() const {
    if (!brain_) return 0.0f;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainPlasticity::getTotalSynapseCount() const {
    if (!brain_) return 0;
    
    size_t total = 0;
    for (const auto& region : brain_->getRegions()) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t BrainPlasticity::getFiringNeuronCount() const {
    if (!brain_) return 0;
    return brain_->getFiringNeuronCount();
}

STDP* BrainPlasticity::getSTDP() {
    if (!brain_) return nullptr;
    return brain_->getSTDP();
}

Hebbian* BrainPlasticity::getHebbian() {
    if (!brain_) return nullptr;
    return brain_->getHebbian();
}

StructuralPlasticity* BrainPlasticity::getStructuralPlasticity() {
    if (!brain_) return nullptr;
    return brain_->getStructuralPlasticity();
}

} // namespace nlm