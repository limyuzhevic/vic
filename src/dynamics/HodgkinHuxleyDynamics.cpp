// Hodgkin-Huxley dynamics implementation
// Detailed biophysical model with ion channels and membrane potentials

#include "HodgkinHuxleyDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>

namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    // Membrane parameters (typical values for squid giant axon)
    float membraneCapacitance;  // μF/cm²
    float sodiumMaxConductance; // mS/cm²
    float potassiumMaxConductance; // mS/cm²
    float leakConductance;      // mS/cm²
    float leakReversalPotential; // mV
    
    // Gating variable time constants
    float alpha_m, beta_m;       // Sodium activation
    float alpha_h, beta_h;       // Sodium inactivation
    float alpha_n, beta_n;       // Potassium activation
    
    // State variables
    float V_m;                  // Membrane potential (mV)
    float m, h, n;              // Gating variables
    float I_ext;                // External current
    
    Impl() : membraneCapacitance(1.0f), sodiumMaxConductance(120.0f), 
             potassiumMaxConductance(36.0f), leakConductance(0.3f),
             leakReversalPotential(10.613f), V_m(-65.0f), m(0.05f), h(0.6f), n(0.32f),
             I_ext(0.0f) {}
    
    // Compute alpha/beta rates based on voltage
    void computeGatingRates(float V) {
        float V_half = V / 10.0f;  // Normalized voltage
        
        // Sodium activation (m)
        alpha_m = (2.5f - 0.1f * V_half) / (std::exp(2.5f - 0.1f * V_half) - 1.0f);
        if (std::abs(alpha_m) < 1e-6f) alpha_m = 0.1f;
        beta_m = 4.0f * std::exp(-V_half / 18.0f);
        
        // Sodium inactivation (h)
        alpha_h = 0.07f * std::exp(-V_half / 20.0f);
        beta_h = 1.0f / (std::exp(3.0f - 0.1f * V_half) + 1.0f);
        
        // Potassium activation (n)
        alpha_n = 0.01f * (V_half + 10.0f) / (std::exp(1.0f + 0.1f * V_half) - 1.0f);
        if (std::abs(alpha_n) < 1e-6f) alpha_n = 0.01f;
        beta_n = 0.125f * std::exp(-V_half / 80.0f);
    }
    
    // Update gating variables using differential equations
    void updateGatingVariables(float V, TimestepDuration dt) {
        float V_half = V / 10.0f;
        
        computeGatingRates(V);
        
        float dm = (alpha_m * (1.0f - m) - beta_m * m) * dt;
        float dh = (alpha_h * (1.0f - h) - beta_h * h) * dt;
        float dn = (alpha_n * (1.0f - n) - beta_n * n) * dt;
        
        m += dm;
        h += dh;
        n += dn;
        
        // Clamp to [0, 1] range
        m = std::max(0.0f, std::min(1.0f, m));
        h = std::max(0.0f, std::min(1.0f, h));
        n = std::max(0.0f, std::min(1.0f, n));
    }
    
    // Compute ionic currents
    float computeIonicCurrent(float V) {
        float I_Na = sodiumMaxConductance * std::pow(m, 3.0f) * h * (V - 50.0f);  // Na+ reversal ~50mV
        float I_K = potassiumMaxConductance * std::pow(n, 4.0f) * (V - (-77.0f)); // K+ reversal ~-77mV
        float I_L = leakConductance * (V - leakReversalPotential);
        
        return I_Na + I_K + I_L;
    }
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = pImpl->V_m;
    float I_total = neuron->getTotalCurrent();
    
    // Update HH dynamics
    pImpl->I_ext = I_total;
    pImpl->updateGatingVariables(V, dt);
    
    // Compute membrane current
    float I_ionic = pImpl->computeIonicCurrent(V);
    
    // Update membrane potential: C*dV/dt = -I_ionic + I_ext
    float dV = (pImpl->I_ext - I_ionic) / pImpl->membraneCapacitance * static_cast<float>(dt);
    V += dV;
    
    // Update state
    pImpl->V_m = V;
    neuron->setMembranePotential(V);
    
    // Check for firing threshold (spike detection)
    if (V >= state.threshold && !neuron->isRefractory()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(dt));
        neuron->setRefractory(state.refractoryDuration);
        neuron->setMembranePotential(state.resetPotential); // Reset to resting
        
        // Update gating variables after spike
        pImpl->computeGatingRates(state.resetPotential);
        pImpl->m = pImpl->computeSteadyStateGating(pImpl->alpha_m, pImpl->beta_m);
        pImpl->h = pImpl->computeSteadyStateGating(pImpl->alpha_h, pImpl->beta_h);
        pImpl->n = pImpl->computeSteadyStateGating(pImpl->alpha_n, pImpl->beta_n);
    }
    
    // Update refractory period
    if (neuron->isRefractory()) {
        neuron->updateRefractory(dt);
        if (!neuron->isRefractory()) {
            // Allow neuron to resume normal dynamics after refractory period
            neuron->setMembranePotential(state.resetPotential);
        }
    }
    
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // HH synapses have instantaneous dynamics
    // Synapses contribute to I_ext
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        synapse->getDestinationNeuron()->receiveExcitatoryInput(weight * static_cast<float>(dt));
    } else {
        synapse->getDestinationNeuron()->receiveInhibitoryInput(weight * static_cast<float>(dt));
    }
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // HH neurons have direct synaptic coupling
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold() && 
           !neuron->isRefractory();
}

void HodgkinHuxleyDynamics::reset() {
    pImpl->V_m = -65.0f;
    pImpl->m = 0.05f;
    pImpl->h = 0.6f;
    pImpl->n = 0.32f;
    pImpl->I_ext = 0.0f;
}

void HodgkinHuxleyDynamics::setMembraneCapacitance(float cm) {
    pImpl->membraneCapacitance = cm;
}

float HodgkinHuxleyDynamics::getMembraneCapacitance() const {
    return pImpl->membraneCapacitance;
}

void HodgkinHuxleyDynamics::setSodiumMaxConductance(float gNa) {
    pImpl->sodiumMaxConductance = gNa;
}

float HodgkinHuxleyDynamics::getSodiumMaxConductance() const {
    return pImpl->sodiumMaxConductance;
}

void HodgkinHuxleyDynamics::setPotassiumMaxConductance(float gK) {
    pImpl->potassiumMaxConductance = gK;
}

float HodgkinHuxleyDynamics::getPotassiumMaxConductance() const {
    return pImpl->potassiumMaxConductance;
}

void HodgkinHuxleyDynamics::setLeakConductance(float gL) {
    pImpl->leakConductance = gL;
}

float HodgkinHuxleyDynamics::getLeakConductance() const {
    return pImpl->leakConductance;
}

} // namespace nlm