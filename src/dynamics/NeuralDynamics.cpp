#include "NeuralDynamics.hpp"
#include "../core/Config/Config.hpp"
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
    // Phase 6: Use neuron's native stepLIF() method which implements
    // proper integrate-and-fire dynamics with spike detection,
    // refractory period, and adaptation variables.
    // The neuron's implementation is more biologically realistic
    // than the simple placeholder in NeuralDynamics::updateNeuron().
    
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    
    // Use neuron's built-in LIF implementation
    bool fired = neuron->stepLIF(static_cast<Timestamp>(0.0), dt);
    
    if (fired) {
        // The neuron automatically handles:
        // - Membrane potential reset to resetPotential
        // - Refractory period management
        // - Spike recording with timestamps
        // - Firing rate tracking
        // - Spike-frequency adaptation
        
        // Additional dynamics from NeuralDynamics
        // (e.g., synaptic conductance updates, if any)
        float synapticInput = neuron->getTotalCurrent();
        // In the neuron's LIF implementation, synaptic input
        // is already integrated into membrane potential dynamics
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Phase 6: Proper synaptic dynamics implementation
    // For now, use synapse's built-in step() method which handles
    // synaptic state, plasticity, e and potential depression/facilitation
    
    if (!synapse) return;
    
    // Use synapse's native dynamics implementation
    synapse->step(static_cast<double>(dt));
    
    // Additional synaptic dynamics could include:
    // - Short-term plasticity (STP)
    // - Synaptic resource depletion/recovery
    // - Neuromodulatory effects on synaptic efficacy
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Phase 6: Proper spike input handling
    // Use synapse's weight and type to determine input type
    
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(static_cast<MembranePotential>(weight));
    } else {
        neuron->receiveInhibitoryInput(static_cast<MembranePotential>(std::abs(weight)));
    }
    
    // Additional spike input effects:
    // - Synaptic facilitation/depression based on recent activity
    // - Neuromodulatory gain modulation
    // - Network-level synchronization effects
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Delegate to neuron's own threshold checking
    // The neuron's LIF implementation already includes
    // proper threshold detection and spike generation
    
    if (!neuron) return false;
    
    // The neuron's stepLIF() method handles:
    // - Membrane potential reaching threshold
    // - Refractory period
    // - Spike recording and reset
    // - Firing rate updates
    
    return neuron->checkThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
    // The neuron and synapse manage their own state
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    pImpl->membraneResistance = R;
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl->membraneResistance;
}

void IntegrateAndFireDynamics::configure(const std::shared_ptr<Config>& config) {
    if (!config) return;
    
    // Configure membrane time constant from config
    if (auto tauOpt = config->get<float>("neuron_membrane_time_constant")) {
        setMembraneTimeConstant(*tauOpt);
    }
    
    // Configure membrane resistance from config
    if (auto ROpt = config->get<float>("neuron_membrane_resistance")) {
        setMembraneResistance(*ROpt);
    }
    
    // Configure neuron-specific parameters
    if (auto thresholdOpt = config->get<float>("neuron_threshold")) {
        // Note: The neuron's threshold is managed by the neuron itself,
        // but NeuralDynamics could track it for consistency
    }
    
    if (auto restingOpt = config->get<float>("neuron_resting_potential")) {
        // Similar note as above - neuron manages its own resting potential
    }
}

} // namespace nlm
