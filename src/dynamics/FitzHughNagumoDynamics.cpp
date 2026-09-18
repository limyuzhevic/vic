#include "FitzHughNagumoDynamics.hpp"

namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    float a;           // Parameter a (typically ~0.7)
    float b;           // Parameter b (typically ~0.8)
    float epsilon;     // Time constant ratio (typically ~0.08)
    float threshold;   // Firing threshold (typically -0.5)
    float reset_v;     // Reset potential (typically -2.0)
    float I_inj;       // Injected current parameter
    float v_max;       // Maximum membrane potential for spike detection
    
    // Recovery variable state (w)
    std::unordered_map<NeuronId, float> recoveryVariable;
    
    Impl() : a(0.7f), b(0.8f), epsilon(0.08f), threshold(-0.5f), reset_v(-2.0f),
             I_inj(0.0f), v_max(1.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics(float a, float b, float epsilon,
                                                 float threshold, float reset_v)
    : pImpl(new Impl) {
    if (a > 0) pImpl->a = a;
    if (b > 0) pImpl->b = b;
    if (epsilon > 0) pImpl->epsilon = epsilon;
    if (threshold > reset_v) pImpl->threshold = threshold;
    pImpl->reset_v = reset_v;
}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron || dt <= 0.0f) return;
    
    float v = neuron->getMembranePotential();
    NeuronId neuronId = neuron->getId();
    
    // Get recovery variable w (store in pImpl if not exists)
    float& w = pImpl->recoveryVariable[neuronId];
    
    // Convert dt from SimulationStep to actual time (assuming 1 step = 1ms)
    float dt_ms = static_cast<float>(dt);
    
    // FitzHugh-Nagumo equations:
    // dv/dt = I - v^3/3 + v + I_inj
    // dw/dt = epsilon * (v + a - b*w)
    
    float I = neuron->getTotalCurrent();
    
    // dv/dt = I - v^3/3 + v + I_inj
    float dv_dt = I - (v * v * v) / 3.0f + v + pImpl->I_inj;
    
    // dw/dt = epsilon * (v + a - b*w)
    float dw_dt = pImpl->epsilon * (v + pImpl->a - pImpl->b * w);
    
    // Update state using Euler integration
    v += dv_dt * dt_ms;
    w += dw_dt * dt_ms;
    
    // Update neuron membrane potential
    neuron->setMembranePotential(v);
    
    // Store recovery variable back
    pImpl->recoveryVariable[neuronId] = w;
    
    // Check for firing threshold
    if (v >= pImpl->threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(pImpl->reset_v);
        neuron->recordSpike(0.0); // TODO: pass actual time
    }
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0f) return;
    
    // For FHN, synaptic dynamics are simpler - just decay
    synapse->step(static_cast<float>(dt));
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // FHN synaptic transmission: excitatory synapses add to I_inj, inhibitory reduce it
    float weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        // Add excitatory input
        float& I_inj = pImpl->I_inj;
        if (weight > 0) I_inj += weight;
    } else {
        // Subtract inhibitory input
        float& I_inj = pImpl->I_inj;
        if (weight < 0) I_inj += weight; // weight is negative
    }
    
    // Store in neuron for integration
    neuron->receiveExcitatoryInput(std::max(0.0f, pImpl->I_inj));
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() >= pImpl->threshold;
}

void FitzHughNagumoDynamics::reset() {
    pImpl->recoveryVariable.clear();
    pImpl->I_inj = 0.0f;
}

void FitzHughNagumoDynamics::setA(float a) {
    if (a > 0) pImpl->a = a;
}

float FitzHughNagumoDynamics::getA() const {
    return pImpl->a;
}

void FitzHughNagumoDynamics::setB(float b) {
    if (b > 0) pImpl->b = b;
}

float FitzHughNagumoDynamics::getB() const {
    return pImpl->b;
}

void FitzHughNagumoDynamics::setEpsilon(float epsilon) {
    if (epsilon > 0) pImpl->epsilon = epsilon;
}

float FitzHughNagumoDynamics::getEpsilon() const {
    return pImpl->epsilon;
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float FitzHughNagumoDynamics::getThreshold() const {
    return pImpl->threshold;
}

void FitzHughNagumoDynamics::setResetV(float v) {
    pImpl->reset_v = v;
}

float FitzHughNagumoDynamics::getResetV() const {
    return pImpl->reset_v;
}

void FitzHughNagumoDynamics::setInjectedCurrent(float I_inj) {
    pImpl->I_inj = I_inj;
}

float FitzHughNagumoDynamics::getInjectedCurrent() const {
    return pImpl->I_inj;
}

} // namespace nlm