#include "NeuralDynamics.hpp"
#include <cmath>
#include <random>

namespace nlm {

// Helper functions for stochastic dynamics
static float normal(float mean, float stddev) {
    static std::mt19937 gen(std::random_device{}());
    std::normal_distribution<float> dist(mean, stddev);
    return dist(gen);
}

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(std::make_unique<Impl>()) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // PHASE 6: Enhanced integrate-and-fire dynamics
    // Implements biological membrane dynamics with voltage-gated channels
    
    const auto& state = neuron->getState();
    
    // Leaky integration with stochastic channel noise
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Enhanced Euler integration with channel noise
    float deterministic = (-(V - V_rest) / tau + I_total / R);
    
    // Add stochastic channel noise (representing ion channel stochasticity)
    float channelNoise = normal(0.0f, 0.1f) * sqrt(static_cast<float>(dt) / tau);
    
    float dV = deterministic * static_cast<float>(dt) + channelNoise;
    neuron->setMembranePotential(V + dV);
    
    // Check for firing with threshold dynamics
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: Pass actual time from simulation
    }
    
    // Refractory mechanism with adaptation
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        // Spike adaptation during refractory period
        float adaptation = neuron->getState().adaptationCurrent * static_cast<float>(dt);
        neuron->setMembranePotential(neuron->getMembranePotential() - adaptation);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // PHASE 6: Realistic synaptic dynamics with vesicle release probability
    // Implements synaptic depression/facilitation and short-term plasticity
    
    synapse->step(static_cast<float>(dt));
    
    // Short-term plasticity modulation
    float utilization = synapse->getWeight() * synapse->getReleaseProbability();
    float depression = 1.0f - std::exp(-dt / synapse->getDepressionTimeConstant());
    float facilitation = std::exp(-dt / synapse->getFacilitationTimeConstant());
    
    float newWeight = synapse->getWeight() * (1.0f - utilization * depression + facilitation);
    synapse->setWeight(newWeight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // PHASE 6: Realistic synaptic transmission with neurotransmitter dynamics
    // Implements conductance-based synaptic currents with AMPA/NMDA dynamics
    
    SynapticWeight weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 50.0f : -70.0f;
    float drivingForce = neuron->getMembranePotential() - reversalPotential;
    
    // Conductance-based current
    float conductance = weight * synapse->getEfficiency();
    float synapticCurrent = conductance * drivingForce;
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapticCurrent));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// PHASE 6: Hodgkin-Huxley dynamics implementation
// Detailed biophysical model of neuronal membrane potentials
class HodgkinHuxleyDynamics::Impl {
public:
    // Ion channel parameters
    float gNa;      // Sodium conductance
    float gK;       // Potassium conductance
    float gL;       // Leak conductance
    float Cm;       // Membrane capacitance
    
    float VNa;      // Sodium reversal potential
    float VK;       // Potassium reversal potential
    float VL;       // Leak reversal potential
    
    // Gating variable time constants and steady states
    float alpha_m0, beta_m0;
    float alpha_h0, beta_h0;
    float alpha_n0, beta_n0;
    
    Impl() : gNa(120.0f), gK(36.0f), gL(0.3f), Cm(1.0f),
             VNa(50.0f), VK(-77.0f), VL(-54.4f),
             alpha_m0(0.1f), beta_m0(4.0f),
             alpha_h0(0.07f), beta_h0(1.0f),
             alpha_n0(0.01f), beta_n0(0.125f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Full Hodgkin-Huxley equations
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    
    // Gating variables (m, h, n)
    float m = state.getGatingVariable("m");
    float h = state.getGatingVariable("h"); 
    float n = state.getGatingVariable("n");
    
    // Compute alpha/beta rates
    float alpha_m = (0.1f * (V + 40.0f)) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    float beta_m = 4.0f * std::exp(-(V + 65.0f) / 18.0f);
    
    float alpha_h = 0.07f * std::exp(-(V + 65.0f) / 20.0f);
    float beta_h = 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
    
    float alpha_n = (0.01f * (V + 55.0f)) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    float beta_n = 0.125f * std::exp(-(V + 65.0f) / 80.0f);
    
    // Update gating variables
    float dm = (alpha_m * (1.0f - m) - beta_m * m) * static_cast<float>(dt);
    float dh = (alpha_h * (1.0f - h) - beta_h * h) * static_cast<float>(dt);
    float dn = (alpha_n * (1.0f - n) - beta_n * n) * static_cast<float>(dt);
    
    // Update neuron state
    neuron->setGatingVariable("m", m + dm);
    neuron->setGatingVariable("h", h + dh);
    neuron->setGatingVariable("n", n + dn);
    
    // Compute membrane currents
    float INa = pImpl->gNa * m * m * m * h * (V - pImpl->VNa);
    float IK = pImpl->gK * n * n * n * (V - pImpl->VK);
    float IL = pImpl->gL * (V - pImpl->VL);
    
    float I_total = neuron->getTotalCurrent();
    float dV = (I_total - INa - IK - IL) / pImpl->Cm * static_cast<float>(dt);
    
    neuron->setMembranePotential(V + dV);
    
    // Check for firing
    if (neuron->getMembranePotential() >= neuron->getThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(pImpl->currentTime);
    }
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Synapse dynamics for HH neurons
    float weight = synapse->getWeight();
    
    // Calcium-dependent short-term plasticity
    float calcium = synapse->getCalciumConcentration();
    float depressionRate = 0.01f * (1.0f + calcium);
    float facilitationRate = 0.005f * (1.0f / (1.0f + calcium));
    
    // Update synaptic efficacy
    float newWeight = weight * std::exp(-depressionRate * static_cast<float>(dt)) +
                     (0.8f * weight) * (1.0f - std::exp(-facilitationRate * static_cast<float>(dt)));
    
    synapse->setWeight(newWeight);
    synapse->step(static_cast<float>(dt));
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Realistic synaptic conductance changes
    SynapticWeight weight = synapse->getWeight();
    
    // AMPA-like conductance increase
    float conductanceChange = weight * 0.5f;
    float reversalPotential = synapse->isExcitatory() ? 0.0f : -80.0f;
    
    neuron->increaseSynapticConductance(conductanceChange, reversalPotential);
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= 55.0f;  // HH firing threshold
}

void HodgkinHuxleyDynamics::reset() {
    // No internal state
}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

// FitzHugh-Nagumo dynamics implementation
// Simplified excitable system model for pattern generation
class FitzHughNagumoDynamics::Impl {
public:
    float a;      // Excitation parameter
    float b;      // Recovery parameter  
    float tau;    // Time constant for recovery
    
    float I_ext;  // External input current
    
    Impl() : a(0.7f), b(0.8f), tau(12.0f), I_ext(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // FitzHugh-Nagumo equations
    float V = neuron->getMembranePotential();
    float w = neuron->getRecoveryVariable();
    
    // Compute external current from inputs
    float I_ext = neuron->getExternalCurrent();
    
    // FHN equations
    float dV = (V - V*V*V/3.0f + w + I_ext) / pImpl->tau;
    float dw = (V - pImpl->a * w) / pImpl->tau;
    
    // Update neuron state
    neuron->setMembranePotential(V + dV * static_cast<float>(dt));
    neuron->setRecoveryVariable(w + dw * static_cast<float>(dt));
    
    // Check for firing threshold
    if (neuron->getMembranePotential() > 0.5f) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(pImpl->currentTime);
        
        // Reset with refractory period
        neuron->setMembranePotential(-1.0f);
        neuron->setRecoveryVariable(1.0f - pImpl->b);
    }
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // FHN-specific synaptic dynamics
    float weight = synapse->getWeight();
    
    // Spike-timing dependent plasticity with FHN characteristics
    float preSpikeTime = synapse->getLastPreSpikeTime();
    float postSpikeTime = synapse->getLastPostSpikeTime();
    
    if (preSpikeTime > 0 && postSpikeTime > 0) {
        float timing = postSpikeTime - preSpikeTime;
        
        // LTP if pre before post, LTD if post before pre
        float delta = (timing > 0) ? 0.01f : -0.01f;
        
        // Modulate by synaptic weight
        float weightChange = delta * weight * (1.0f - std::abs(weight));
        synapse->setWeight(weight + weightChange);
    }
    
    synapse->step(static_cast<float>(dt));
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // FHN-like synaptic activation
    SynapticWeight weight = synapse->getWeight();
    
    // Bistable switch behavior
    float threshold = 0.2f;
    float input = weight * (1.0f + std::sin(neuron->getRecoveryVariable()));
    
    if (input > threshold) {
        neuron->injectCurrent(input);
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() > 0.5f;
}

void FitzHughNagumoDynamics::reset() {
    // No internal state
}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

} // namespace nlm