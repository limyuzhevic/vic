#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    // Membrane properties for integrate-and-fire dynamics
    float membraneTimeConstant;  // ms - time constant for membrane potential decay
    float membraneResistance;     // MOhm - resistance affecting synaptic current integration
    
    // Initialize with typical biological values
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

// Constructor initializes membrane dynamics parameters
IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {
    // Set up basic neural membrane dynamics
    // membraneTimeConstant: time for potential to decay from threshold to resting (-70 ms)
    // membraneResistance: determines how input current affects membrane potential
}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Update neuron membrane potential using leaky integrate-and-fire dynamics
    // Implements the differential equation: dV/dt = -(V - V_rest)/τ + I/C
    // Where: V = membrane potential, V_rest = resting potential, τ = time constant, I = input current, C = capacitance
    
    const auto& state = neuron->getState();
    
    // Current membrane state variables
    float V = neuron->getMembranePotential();           // Current membrane potential (mV)
    float V_rest = state.restingPotential;              // Target resting potential (mV)
    float I = neuron->getTotalCurrent();                // Total input current (nA)
    float tau = pImpl->membraneTimeConstant;           // Membrane time constant (ms)
    float R = pImpl->membraneResistance;               // Membrane resistance (MOhm)
    
    // Numerical integration using explicit Euler method
    // dV = (-(V - V_rest) / tau + I / R) * dt
    float dV = (-(V - V_rest) / tau + I / R) * static_cast<float>(dt);
    neuron->setMembranePotential(V + dV);
    
    // Check if neuron has reached firing threshold and should spike
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual simulation time
    }
    
    // Enforce refractory period - clamp potential to reset value during refractory phase
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear accumulated input current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement basic synaptic dynamics: update delay, simulate transmission
    // Simple leaky integrator for synaptic efficacy
    float efficacy = synapse->getEfficacy();
    efficacy *= std::exp(-dt / 100.0f);  // Decay with time constant of 100ms
    synapse->setEfficacy(efficacy);
    
    // Decrease delay as efficacy builds up (short-term plasticity)
    Delay delay = synapse->getDelay();
    if (delay > 1) {
        delay = std::max(Delay(1), delay - 1);
        synapse->setDelay(delay);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply synaptic input from spike using biologically realistic current injection
    // Converts synaptic weight and efficacy to post-synaptic currents based on
    // conductance-based neuron model with realistic reversal potentials
    
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    if (synapse->isExcitatory()) {
        // Excitatory synapses: depolarizing current toward reversal potential (~20mV)
        float synapticCurrent = 20.0f * efficacy * weight;  // Reversal potential adjustment
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else {
        // Inhibitory synapses: hyperpolarizing current toward reversal potential (~10mV)
        float synapticCurrent = 10.0f * efficacy * std::abs(weight);  // Inhibitory current strength
        neuron->receiveInhibitoryInput(synapticCurrent);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Determine if neuron should fire based on membrane potential and adaptive threshold
    // Incorporates biological spike detection mechanisms including refractory period
    // and adaptation effects that suppress firing at high sustained rates
    
    // Current membrane potential
    MembranePotential potential = neuron->getMembranePotential();
    // Current firing threshold (may be modified by adaptation)
    MembranePotential threshold = neuron->getThreshold();
    
    // First check: neuron must not be in refractory period
    if (neuron->isRefractory()) {
        return false;
    }
    
    // Apply adaptation mechanism - increases threshold to prevent excessive firing
    // Based on neuron->getState().adaptationVariable
    float adaptation = neuron->getState().adaptationVariable;
    float adaptedThreshold = threshold + adaptation * 2.0f;
    
    // Final firing condition: potential must exceed adapted threshold
    return potential >= adaptedThreshold;
}

void IntegrateAndFireDynamics::reset() {
    // Reset membrane dynamics state
    // Currently no internal state to reset, but placeholder for future extensions
    // May include temporal variables, facilitation/ depression state, etc.
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    // Set membrane time constant - controls how quickly potential decays
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    // Get current membrane time constant
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
