#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Fixed constructor/destructor
IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Implement real integrate-and-fire dynamics
    // Based on the standard LIF equation: tau_m * dV/dt = -(V - V_rest) + R * I
    // Where: tau_m = membrane time constant, V = membrane potential, V_rest = resting potential
    //        R = membrane resistance, I = injected current

    const auto& state = neuron->getState();
    
    // Current membrane potential
    MembranePotential V = neuron->getMembranePotential();
    
    // Get parameters from neuron state
    MembranePotential V_rest = state.restingPotential;
    MembranePotential V_thresh = state.threshold;
    MembranePotential V_reset = state.resetPotential;
    float tau_m = pImpl->membraneTimeConstant;
    float R_m = pImpl->membraneResistance;
    
    // Get total injected current
    MembranePotential I_total = neuron->getTotalCurrent();
    
    // Calculate derivative: dV/dt = -(V - V_rest) / tau_m + I_total / C_m
    // Where C_m = membrane capacitance (related to R_m and tau_m: C_m = tau_m / R_m)
    float dVdt = -(static_cast<double>(V - V_rest) / tau_m) + 
                 static_cast<double>(I_total) / (pImpl->membraneResistance);

    // Apply Euler integration with numerical stability
    double dt_double = static_cast<double>(dt);
    double newV = V + dVdt * dt_double;
    
    // Clamp to reasonable bounds to prevent numerical issues
    newV = std::max(-100.0, std::min(newV, 100.0));
    neuron->setMembranePotential(static_cast<MembranePotential>(newV));
    
    // Check for spike threshold crossing
    bool shouldFire = neuron->checkThreshold();
    
    if (shouldFire) {
        // Spike occurred - reset potential
        neuron->setMembranePotential(V_reset);
        neuron->setFiringState(FiringState::Active);
        
        // Record spike time (current simulation time)
        neuron->recordSpike(0.0);  // TODO: pass actual simulation time
        
        // Update firing statistics
        neuron->setFiringRate(1000.0 / neuron->getRefractoryPeriod()); // Hz approximation
    }
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        // In refractory period, clamp potential to reset value
        neuron->setMembranePotential(V_reset);
        
        // Decrement refractory counter
        neuron->decrementRefractory();
    }
    
    // Decay synaptic conductance over time
    float conductanceDecay = 0.99f;
    float newConductance = state.synapseConductance * conductanceDecay;
    
    // Add leak component
    newConductance += (0.0f - newConductance) * 0.01f; // Simple leak
    
    // Update neuron state (would need proper state access in real implementation)
    
    // Clear current after use
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::computeSynapticInput(Neuron* neuron,
                                                   const std::vector<Synapse*>& synapses) {
    MembranePotential totalSynapticCurrent = 0.0f;
    
    for (auto* synapse : synapses) {
        if (synapse->isActive() && synapse->getPreNeuron()->isFiring()) {
            SynapticWeight weight = synapse->getWeight();
            float conductance = synapse->getConductance();
            
            // Calculate post-synaptic current
            MembranePotential current = weight * conductance;
            
            // Add excitatory/inhibitory contribution
            NeuronType postType = neuron->getType();
            if (postType == NeuronType::Excitatory) {
                totalSynapticCurrent += current;
            } else if (postType == NeuronType::Inhibitory) {
                totalSynapticCurrent -= current;
            }
        }
    }
    
    neuron->setTotalCurrent(totalSynapticCurrent);
}

void IntegrateAndFireDynamics::updateSpikeTiming(Neuron* neuron, TimestepDuration dt) {
    // Update spike timing based on adaptive dynamics
    auto& state = neuron->getState();
    
    if (neuron->isFiring()) {
        // Increase firing rate adaptation
        state.adaptationVariable += 0.1f * static_cast<float>(dt);
        
        // Modify threshold based on adaptation
        float adaptationThreshold = state.adaptationVariable * 0.5f;
        neuron->setThreshold(state.threshold + adaptationThreshold);
    } else {
        // Decay adaptation variable
        state.adaptationVariable *= 0.95f;
        
        // Return threshold towards baseline
        neuron->setThreshold(state.threshold - 0.01f);
    }
}

} // namespace nlm

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
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
