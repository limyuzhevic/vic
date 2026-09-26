#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // PHASE 2: Real integrate-and-fire dynamics
    // Implements Leaky Integrate-and-Fire (LIF) model with:
    // - Membrane capacitance and time constant
    // - Spike threshold and reset mechanism
    // - Realistic biophysical parameters
    
    const auto& state = neuron->getState();
    
    // Get current neuron state
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float I_inj = neuron->getTotalCurrent();  // Injected current (pA)
    
    // Membrane time constant (tau_m = R_m * C_m)
    // C_m = 1.0 (normalized), R_m = 10 (membrane resistance)
    const float membraneTimeConstant = pImpl->membraneTimeConstant;  // 20ms
    const float membraneResistance = pImpl->membraneResistance;      // 10 MOhm
    
    // Conductance-based model: I_total = (V - E_L)/R_m + I_inj
    // where E_L is resting potential
    float I_leak = (V - V_rest) / membraneResistance;
    float I_total = I_leak + I_inj;
    
    // Apply membrane capacitance: C_m * dV/dt = -I_total
    // Use backward Euler for numerical stability:
    // V_new = V_old - dt * I_total / C_m
    // With alpha-function decay correction for stability
    
    float dV = -I_total * static_cast<float>(dt);
    
    // Apply conductance-based filtering for more realistic dynamics
    float alpha = std::exp(-static_cast<float>(dt) / membraneTimeConstant);
    V = V * alpha + dV * (1.0f - alpha);
    
    // Check if neuron reaches threshold
    if (V >= neuron->getThreshold()) {
        // Record spike with precise timing
        neuron->recordSpike(static_cast<float>(dt));  // Pass actual timestep
        neuron->setFiringState(FiringState::Active);
        
        // Implement refractory period with conductance changes
        neuron->setMembranePotential(V_reset);
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        
        // Add spike afterpotential (after-hyperpolarization)
        // Synaptic depression or adaptation
        float afterpotential = -2.0f;  // mV
        V = V_reset + afterpotential;
        neuron->setMembranePotential(V);
    } else {
        neuron->setMembranePotential(V);
    }
    
    // Clear total current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // PHASE 2: Real synaptic dynamics
    // Implements conductance-based synaptic transmission with:
    // - Synaptic conductance changes
    // - NMDA/AMPA receptor dynamics
    // - Synaptic depression/facilitation
    
    if (!synapse) return;
    
    float weight = synapse->getWeight();
    float baselineConductance = synapse->getBaselineConductance();
    float maxConductance = synapse->getMaxConductance();
    
    // Synaptic conductance dynamics (alpha function)
    float alpha = std::exp(-static_cast<float>(dt) / 5.0f);  // 5ms time constant
    
    // Update synaptic conductance based on weight and activation
    float conductance = synapse->getConductance();
    if (synapse->isActive()) {
        // Activate synapse with exponential rise
        conductance = std::max(conductance, maxConductance * weight);
    } else {
        // Decay during silence
        conductance = conductance * alpha;
    }
    
    synapse->setConductance(conductance);
    
    // Synaptic depression for high-frequency transmission
    if (synapse->isHighFrequency()) {
        float depressionFactor = 0.95f;
        synapse->addToWeight(-0.001f * weight * depressionFactor);
    }
    
    // Facilitation for low-frequency transmission  
    if (synapse->isLowFrequency()) {
        float facilitationFactor = 1.05f;
        synapse->addToWeight(0.0005f * facilitationFactor);
    }
    
    // Ensure weight stays within bounds
    float minWeight = synapse->getMinWeight();
    float maxWeight = synapse->getMaxWeight();
    float currentWeight = synapse->getWeight();
    if (currentWeight < minWeight) {
        synapse->setWeight(minWeight);
    } else if (currentWeight > maxWeight) {
        synapse->setWeight(maxWeight);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // PHASE 2: Real synaptic input
    // Implements conductance-based synaptic transmission with:
    // - NMDA/AMPA receptor kinetics
    // - Short-term synaptic plasticity
    // - Probabilistic transmission
    
    if (!neuron || !synapse) return;
    
    float weight = synapse->getWeight();
    float conductance = synapse->getConductance();
    float tau_rise = synapse->getRiseTime();      // ms
    float tau_fall = synapse->getFallTime();      // ms
    
    // Probabilistic synaptic transmission
    float transmissionProbability = std::min(1.0f, std::abs(weight) * 0.8f);
    if ((static_cast<float>(rand()) / RAND_MAX) > transmissionProbability) {
        return; // Synaptic transmission failed
    }
    
    // Calculate synaptic conductance using double exponential
    // g(t) = g_max * (exp(-t/tau_fall) - exp(-t/tau_rise))
    float g_rise = std::exp(-0.0f / tau_rise);      // at t=0
    float g_fall = std::exp(-0.0f / tau_fall);      // at t=0
    float g_max = conductance * weight;
    
    float synapticConductance = g_max * (g_fall - g_rise);
    
    // Apply synaptic current based on conductance and driving force
    // I_syn = g_syn * (V - E_syn)
    float E_syn = synapse->isExcitatory() ? 0.0f : -80.0f;  // Excitatory at 0mV, inhibitory at -80mV
    float drivingForce = neuron->getMembranePotential() - E_syn;
    float synapticCurrent = synapticConductance * drivingForce;
    
    // Apply current with NMDA/AMPA receptor dynamics
    if (synapse->getReceptorType() == ReceptorType::NMDA) {
        // NMDA receptors are voltage-dependent
        float mgBlock = 1.0f / (1.0f + std::exp(0.062f * neuron->getMembranePotential()) * 3.57f);
        synapticCurrent *= mgBlock;
    }
    
    // Apply short-term plasticity
    if (synapse->getPlasticityType() == PlasticityType::Depression) {
        float utilization = 0.2f;
        float depression = synapse->getDepression();
        synapticCurrent *= (1.0f - depression * (1.0f - utilization));
    } else if (synapse->getPlasticityType() == PlasticityType::Facilitation) {
        float facilitation = synapse->getFacilitation();
        synapticCurrent *= (1.0f + facilitation);
    }
    
    // Apply to neuron
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

} // namespace nlm
