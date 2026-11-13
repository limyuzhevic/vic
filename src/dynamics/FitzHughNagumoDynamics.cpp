#include "FitzHughNagumoDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    // FHN model parameters
    float tau;                     // Time constant (ms)
    float threshold;               // Threshold voltage (mV)
    float resetValue;              // Reset voltage (mV)
    float conductance;             // Conductance (mS)
    
    // Recovery variable (voltage-independent)
    float v;                       // Membrane potential (mV)
    float w;                       // Recovery variable
    
    // External inputs
    float I_ext;                   // External current input
    
    Impl() 
        : tau(12.5f), threshold(0.5f), resetValue(-1.2f), conductance(0.8f)
        , v(-1.2f), w(0.0f), I_ext(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    pImpl->v = V;
    pImpl->I_ext = neuron->getTotalCurrent();
    
    // FHN dynamics: dv/dt = (v - v³/3 - w + I_ext) / tau
    // dw/dt = epsilon * (v + a - b * w)
    float dv = (pImpl->v - pImpl->v * pImpl->v * pImpl->v / 3.0f - pImpl->w + pImpl->I_ext) / pImpl->tau;
    
    // Recovery variable dynamics (simplified)
    float dw = 0.08f * (pImpl->v + 0.7f - 0.8f * pImpl->w);
    
    // Update using Euler integration
    pImpl->v += dt * dv;
    pImpl->w += dt * dw;
    
    // Clamp to reasonable bounds
    pImpl->v = std::clamp(pImpl->v, -3.0f, 3.0f);
    pImpl->w = std::clamp(pImpl->w, -2.0f, 2.0f);
    
    // Update neuron state
    neuron->setMembranePotential(pImpl->v);
    neuron->clearTotalCurrent();
    
    // Check for spike (threshold crossing)
    if (pImpl->v >= pImpl->threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // FHN synaptic dynamics: exponential decay and facilitation
    float conductance = synapse->getSynapticConductance();
    
    // Decay of synaptic conductance
    conductance *= std::exp(-dt / 10.0f);
    
    // Facilitation based on presynaptic activity
    float facilitation = synapse->getFacilitation();
    facilitation = std::clamp(facilitation + dt * 0.01f, 0.0f, 2.0f);
    
    // Combined synaptic effect
    conductance *= facilitation;
    conductance = std::clamp(conductance, 0.0f, 1.0f);
    
    synapse->setSynapticConductance(conductance);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Spike-induced input with FHN characteristics
    float weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 1.0f : (-1.0f);
    
    // Calculate synaptic conductance using FHN-like dynamics
    float g_syn = weight * 0.2f * std::exp(-0.1f * synapse->getAge());
    float I_syn = g_syn * (neuron->getMembranePotential() - reversalPotential);
    
    // Add to neuron's total current
    neuron->receiveExcitatoryInput(I_syn);
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() >= pImpl->threshold;
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN internal state
    pImpl->v = pImpl->resetValue;
    pImpl->w = 0.0f;
    pImpl->I_ext = 0.0f;
}

void FitzHughNagumoDynamics::setTimeConstant(float tau) {
    pImpl->tau = tau;
}

float FitzHughNagumoDynamics::getTimeConstant() const {
    return pImpl->tau;
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float FitzHughNagumoDynamics::getThreshold() const {
    return pImpl->threshold;
}

void FitzHughNagumoDynamics::setResetValue(float resetValue) {
    pImpl->resetValue = resetValue;
}

float FitzHughNagumoDynamics::getResetValue() const {
    return pImpl->resetValue;
}

void FitzHughNagumoDynamics::setConductance(float g) {
    pImpl->conductance = g;
}

float FitzHughNagumoDynamics::getConductance() const {
    return pImpl->conductance;
}

} // namespace nlm
