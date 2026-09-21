#include "NeuralDynamics.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() 
    : membraneTimeConstant_(1.0f), membraneCapacitance_(1.0f),
      gNaMax_(120.0f), gKMax_(36.0f), gL_(0.3f), ENa_(50.0f), EK_(-77.0f), EL_(-54.387f) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    
    // Calculate gating variables
    float m = alphaM(V) / (alphaM(V) + betaM(V));
    float h = alphaH(V) / (alphaH(V) + betaH(V));
    float n = alphaN(V) / (alphaN(V) + betaN(V));
    
    // Calculate currents
    float INa = gNaMax_ * m * m * m * h * (V - ENa_);
    float IK = gKMax_ * n * n * n * (V - EK_);
    float IL = gL_ * (V - EL_);
    
    // Calculate membrane current
    float Im = state.synapseConductance * (V - EL_) + INa + IK + IL;
    
    // Update membrane potential (dV/dt = I/C)
    float dVdt = Im / membraneCapacitance_;
    
    // Integrate using forward Euler
    float newV = V + dVdt * dt;
    neuron->setMembranePotential(newV);
    
    // Update gating variables
    float dm = alphaM(V) * (1.0f - m) - betaM(V) * m;
    float dh = alphaH(V) * (1.0f - h) - betaH(V) * h;
    float dn = alphaN(V) * (1.0f - n) - betaN(V) * n;
    
    // Store gating variables in neuron state (need to extend NeuronState)
    // For now, just update the membrane potential
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // HH synaptic dynamics
    const auto& state = synapse->getState();
    float weight = synapse->getWeight();
    
    // Calcium-dependent synaptic plasticity
    float calcium = state.plasticityFlags.plasticity ? 1.0f : 0.0f;
    
    // Update synaptic weight based on calcium dynamics
    float weightChange = calcium * dt * 0.001f;
    
    // Apply STDP-like dynamics
    if (synapse->getPlasticityFlags().stdp) {
        const auto& preSpikes = synapse->getPreSpikeHistory();
        const auto& postSpikes = synapse->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Calculate STDP weight change
            float timeDiff = postSpikes.back() - preSpikes.front();
            if (timeDiff > 0) {  // Post before pre = LTD
                weightChange -= 0.01f * std::exp(-timeDiff / 20.0f);
            } else {  // Pre before post = LTP
                weightChange += 0.01f * std::exp(timeDiff / 20.0f);
            }
        }
    }
    
    // Apply weight change
    float newWeight = weight + weightChange;
    synapse->setWeight(std::clamp(newWeight, 0.0f, 1.0f));
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // HH spike transmission
    float synapticCurrent = synapse->getWeight() * synapse->getEfficacy() * 10.0f;  // Scaling factor
    
    // Apply current to neuron
    neuron->injectCurrent(synapticCurrent);
    
    // Update synapse efficacy for short-term plasticity
    float& efficacy = const_cast<float&>(synapse->getEfficacy());
    efficacy *= 0.9f;  // Depression
    if (efficacy < 0.1f) efficacy = 0.1f;
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    const auto& state = neuron->getState();
    return state.firingState == FiringState::Refractory;
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH state variables
}

HodgkinHuxleyDynamics::FitzHughNagumoDynamics::FitzHughNagumoDynamics() 
    : tau_(20.0f), theta_(-1.5f), Vr_(-1.0f), a_(0.7f), w_(0.8f), epsilon_(0.08f),
      VSteadyState_(0.0f), wSteadyState_(0.0f) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    
    // Calculate recovery variable derivatives
    float dVdt = (V - VSteadyState_ - w_ + state.synapseConductance) / tau_;
    float dWdt = epsilon_ * (V - a_ * w_ - b_);  // Need to add b_ parameter
    
    // Integrate using forward Euler
    float newV = V + dVdt * dt;
    float newW = w_ + dWdt * dt;
    
    // Update neuron state
    neuron->setMembranePotential(newV);
    // Store recovery variable (need to extend NeuronState)
    w_ = newW;
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // FHN synaptic dynamics
    float weight = synapse->getWeight();
    
    // Excitable synapse model
    float calcium = state.plasticityFlags.plasticity ? 1.0f : 0.0f;
    
    // Synaptic facilitation and depression
    float utilization = synapse->getUtilization();
    float depressionRate = synapse->getDepressionRate();
    float facilitationRate = synapse->getFacilitationRate();
    
    // Update synaptic variables
    float dUdt = (-u_ + R * (V - E)) / tau_s_;
    
    // Update synaptic efficacy
    float& efficacy = const_cast<float&>(synapse->getEfficacy());
    efficacy = std::clamp(efficacy + dt * dUdt, 0.0f, 1.0f);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // FHN spike transmission
    float synapticCurrent = synapse->getWeight() * synapse->getEfficacy() * 5.0f;
    
    // Apply to neuron
    neuron->injectCurrent(synapticCurrent);
    
    // Update synaptic variables
    float& u = const_cast<float&>(synapse->getUtilization());
    u = std::min(u + 0.1f, 1.0f);  // Facilitation
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    const auto& state = neuron->getState();
    return state.membranePotential > theta_;
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN state variables
    w_ = wSteadyState_;
    u_ = 0.0f;
}

BrainNetworkDynamics::BrainNetworkDynamics() 
    : leakRate_(0.1f), threshold_(-55.0f), refractoryPeriod_(5),
      depressionRate_(0.5f), facilitationRate_(0.5f), utilization_(0.5f) {}

BrainNetworkDynamics::~BrainNetworkDynamics() = default;

void BrainNetworkDynamics::updateNetwork(std::vector<Neuron*>& neurons, std::vector<Synapse*>& synapses, TimestepDuration dt) {
    // Update all neurons
    for (auto* neuron : neurons) {
        if (!neuron) continue;
        
        // Update neuron using LIF dynamics
        const auto& state = neuron->getState();
        
        // Calculate total input
        float totalInput = state.synapseConductance - leakRate_ * (state.membranePotential - EL_);
        
        // Update membrane potential
        float newV = state.membranePotential + totalInput * dt;
        neuron->setMembranePotential(newV);
        
        // Check for threshold crossing
        if (newV >= threshold_) {
            neuron->setFiringState(FiringState::Refractory);
            neuron->setRefractoryPeriod(refractoryPeriod_);
        }
    }
    
    // Update synapses
    for (auto* synapse : synapses) {
        if (!synapse) continue;
        
        updateSynapticVariables(synapse, 0.0f, dt);
    }
}

void BrainNetworkDynamics::applySynapticTransmission(Neuron* preNeuron, Neuron* postNeuron, const Synapse* synapse) {
    if (!preNeuron || !postNeuron || !synapse) return;
    
    float efficacy = getSynapticEfficacy(synapse);
    float synapticCurrent = efficacy * synapse->getWeight() * 10.0f;
    
    postNeuron->injectCurrent(synapticCurrent);
}

void BrainNetworkDynamics::updateSynapticVariables(Synapse* synapse, float preSpikeTime, TimestepDuration dt) {
    if (!synapse) return;
    
    // Short-term synaptic plasticity
    float u = synapse->getUtilization();
    float x = synapse->getFacilitationTrace();
    float y = synapse->getDepressionTrace();
    
    // Update variables
    float du = (u * (1.0f - u) * x - u / tau_f_) * dt;
    float dx = (x_0_ - x) / tau_x_ * dt;
    float dy = (-y * u + y_0_) / tau_y_ * dt;
    
    // Update synapse state
    synapse->setUtilization(u + du);
    synapse->setFacilitationTrace(x + dx);
    synapse->setDepressionTrace(y + dy);
    
    // Calculate efficacy
    float& efficacy = const_cast<float&>(synapse->getEfficacy());
    efficacy = u * y;
}

float BrainNetworkDynamics::getSynapticEfficacy(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    
    float u = synapse->getUtilization();
    float y = synapse->getDepressionTrace();
    return u * y;
}

} // namespace nlm

// Need to add missing Synapse.hpp functions for the synaptic variables
// Also need to add to NeuronState for HH and FHN dynamics
// This is a simplified implementation for demonstration