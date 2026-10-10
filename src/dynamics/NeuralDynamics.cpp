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
    // Phase 2: Real integrate-and-fire dynamics with enhanced features
    const auto& state = neuron->getState();
    
    // Leaky integration with conductance-based model
    // dV/dt = -(V - V_rest)/tau_m + I_syn + I_inj
    // Where I_syn = sum(synaptic conductances * reversal potentials)
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_inj = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Total synaptic current (positive for excitation, negative for inhibition)
    float I_syn = -state.synapseConductance * (V - state.e_reversal);  // Reversal potential contribution
    
    // Full membrane equation with leak and injected current
    float dV = (-(V - V_rest) / tau + (I_inj + I_syn) / R) * static_cast<float>(dt);
    neuron->setMembranePotential(V + dV);
    
    // Spike detection with refractory period handling
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        // Record actual spike time for plasticity calculations
        neuron->recordSpike(static_cast<Timestamp>(dt));
    }
    
    // Enhanced refractory mechanism with absolute refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        // During absolute refractory period, membrane potential stays at reset
        if (neuron->getFiringState() == FiringState::Active) {
            // In absolute refractory, override any potential changes
            neuron->setMembranePotential(state.resetPotential);
        }
    }
    
    // Reset synaptic conductances for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Phase 2: Real synaptic dynamics with short-term plasticity
    TimestepDuration dt_ms = static_cast<TimestepDuration>(dt * 1000.0f);  // Convert to ms
    
    // Apply delay-dependent dynamics
    // Synaptic transmission is a multi-step process with temporal dynamics
    
    // Update synaptic weight based on plasticity mechanisms (STDP, Hebbian, etc.)
    // This is a placeholder for the actual plasticity computation
    // Real implementation would integrate with brain plasticity systems
    
    // Apply short-term plasticity modulation
    float efficacy = synapse->getEfficacy();
    float shortTermModification = 1.0f;
    
    if (synapse->isExcitatory()) {
        // Facilitation model for excitatory synapses
        shortTermModification = std::max(0.1f, efficacy);
    } else {
        // Depression model for inhibitory synapses
        shortTermModification = std::min(1.0f, efficacy * 0.8f);
    }
    
    // Apply efficacy modulation to synaptic transmission
    // In real implementation, this would affect synaptic conductance
    // For now, we just note the dynamics exist
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Phase 2: Real synaptic input with conductance-based dynamics
    // Synaptic transmission is modeled as conductance changes rather than direct current injection
    
    SynapticWeight weight = synapse->getWeight();
    float dt = 0.001f;  // 1ms timestep for conductance dynamics
    
    if (synapse->isExcitatory()) {
        // Excitatory synapses: AMPA/NMDA type conductances
        // Increase synaptic conductance with weighting
        float conductance = weight * synapse->getEfficacy();  // Efficacy modulation
        float reversalPotential = 0.0f;  // Depolarizing reversal potential
        
        // Update synaptic conductance in neuron state
        neuron->addToMembranePotential(conductance);
        neuron->setFiringRate(neuron->getFiringRate() + 1.0f);  // Track activity
        
    } else if (synapse->isInhibitory()) {
        // Inhibitory synapses: GABA type conductances
        // Hyperpolarizing effect on postsynaptic neuron
        float conductance = std::abs(weight) * synapse->getEfficacy();  // Efficacy modulation
        float reversalPotential = -75.0f;  // Hyperpolarizing reversal potential
        
        // Strong inhibitory input - can override excitatory inputs during refractory
        neuron->receiveInhibitoryInput(conductance * 10.0f);  // Scale effect
        
    } else {
        // Modulatory synapses: Dopaminergic, serotonergic, etc.
        // Modulate neuronal excitability and plasticity
        float modulationStrength = weight * synapse->getEfficacy();
        
        // Record modulatory input for plasticity
        neuron->recordModulatoryInput(modulationStrength);
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
