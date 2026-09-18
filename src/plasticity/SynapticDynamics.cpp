#include "SynapticDynamics.hpp"
#include <algorithm>

namespace nlm {

struct SynapticDynamics::Impl {
    float time_constant;     // ms
    float reversal_potential; // mV
    float decay_rate;         // 1/ms
    
    // Conductance state for each synapse
    std::unordered_map<SynapseId, float> conductances;
    
    Impl() : time_constant(5.0f), reversal_potential(0.0f), decay_rate(0.2f) {}
};

SynapticDynamics::SynapticDynamics(float time_constant, float reversal_potential)
    : pImpl(new Impl) {
    if (time_constant > 0) pImpl->time_constant = time_constant;
    pImpl->reversal_potential = reversal_potential;
    pImpl->decay_rate = 1.0f / time_constant;
}

SynapticDynamics::~SynapticDynamics() = default;

void SynapticDynamics::update(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) return;
    
    float& g = pImpl->conductances[synapse->getId()];
    
    // Exponential decay of conductance: dg/dt = -g/tau
    float dt_ms = static_cast<float>(dt);
    g *= std::exp(-pImpl->decay_rate * dt_ms);
    
    // Update synapse effective weight based on conductance
    synapse->updateEffectiveWeight(g * pImpl->reversal_potential);
}

void SynapticDynamics::applyInput(Synapse* synapse, SynapticWeight weight) {
    if (!synapse) return;
    
    float& g = pImpl->conductances[synapse->getId()];
    
    if (synapse->isExcitatory()) {
        // Increase conductance for excitatory synapses
        g += std::abs(weight) * 0.1f; // Scaling factor
    } else {
        // Decrease conductance for inhibitory synapses (or make negative)
        g -= std::abs(weight) * 0.1f;
    }
}

void SynapticDynamics::reset() {
    pImpl->conductances.clear();
}

void SynapticDynamics::setTimeConstant(float tau) {
    if (tau > 0) {
        pImpl->time_constant = tau;
        pImpl->decay_rate = 1.0f / tau;
    }
}

float SynapticDynamics::getTimeConstant() const {
    return pImpl->time_constant;
}

void SynapticDynamics::setReversalPotential(float reversal) {
    pImpl->reversal_potential = reversal;
}

float SynapticDynamics::getReversalPotential() const {
    return pImpl->reversal_potential;
}

void SynapticDynamics::setDecayRate(float rate) {
    if (rate > 0) pImpl->decay_rate = rate;
}

float SynapticDynamics::getDecayRate() const {
    return pImpl->decay_rate;
}

float SynapticDynamics::getConductance(SynapseId synapseId) const {
    auto it = pImpl->conductances.find(synapseId);
    return (it != pImpl->conductances.end()) ? it->second : 0.0f;
}

void SynapticDynamics::setConductance(SynapseId synapseId, float conductance) {
    pImpl->conductances[synapseId] = std::max(0.0f, conductance);
}

} // namespace nlm