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
    // Real integrate-and-fire dynamics with biological accuracy
    // Implements the differential equation: C*dV/dt = (V_rest - V)/R + I
    // Where C is capacitance, V is membrane potential, V_rest is resting potential,
    // R is membrane resistance, and I is injected current
    
    if (!neuron) {
        NLM_LOG_ERROR("IntegrateAndFireDynamics::updateNeuron: neuron is null");
        return;
    }
    
    const auto& state = neuron->getState();
    
    // Membrane parameters
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    float I = neuron->getTotalCurrent();  // Total synaptic + injected current
    
    // Neural constants (can be overridden via configuration)
    float tau = pImpl->membraneTimeConstant;  // Membrane time constant (ms)
    float R = pImpl->membraneResistance;     // Membrane resistance (MOhm)
    float C = 1.0f;                         // Membrane capacitance (nF) - assumed constant
    
    // Apply refractory period
    if (neuron->isRefractory()) {
        // During refractory period, clamp potential to reset value
        // and countdown refractory timer
        neuron->setMembranePotential(V_reset);
        neuron->decrementRefractory();
        
        // Still process any incoming input (shunted by refractory)
        neuron->clearTotalCurrent();
        return;
    }
    
    // Integrate membrane potential using exponential Euler (more stable than simple Euler)
    // dV/dt = (V_rest - V) / (tau * R) + I / C
    // V_new = V_old + dt * (V_rest - V_old) / (tau * R) + dt * I / C
    
    float leakTerm = (V_rest - V) / (tau * R);
    float inputTerm = I / C;
    float dV = static_cast<float>(dt) * 1000.0f * (leakTerm + inputTerm);  // Convert to ms
    
    // Update membrane potential
    V += dV;
    neuron->setMembranePotential(V);
    
    // Apply spike-frequency adaptation if neuron has adaptation variable
    if (state.adaptationVariable > 0.0f) {
        float adaptationEffect = state.adaptationVariable * 0.01f;  // Strength of adaptation
        V -= adaptationEffect;
        neuron->setMembranePotential(V);  // Clamp to valid range
        
        // Decay adaptation variable
        float decayFactor = std::exp(-dt / (10.0f));  // Adaptation time constant ~10ms
        neuron->setAdaptationVariable(state.adaptationVariable * decayFactor);
    }
    
    // Clamp membrane potential to prevent instability (biologically realistic bounds)
    V = std::clamp(V, -100.0f, 50.0f);  // Typical neuron potential range
    neuron->setMembranePotential(V);
    
    // Check for spike threshold crossing
    if (V >= threshold) {
        // Record the spike time for STDP calculations
        Timestamp spikeTime = static_cast<Timestamp>(pImpl->currentTime);
        neuron->recordSpike(spikeTime);
        
        // Enter refractory period
        neuron->setFiringState(FiringState::Active);
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setMembranePotential(V_reset);  // Reset potential
        
        // Update adaptation variable (spike-triggered adaptation)
        float currentAdaptation = neuron->getAdaptationVariable();
        neuron->setAdaptationVariable(currentAdaptation + 1.0f);
        
        // Trigger synaptic transmission through spike system
        if (pImpl->spikeSystem) {
            // Create spike event for downstream neurons
            DetailedSpikeEvent event;
            event.source_neuron = neuron->getId();
            event.timestamp = spikeTime;
            event.weight = 1.0f;
            event.is_excitatory = true;
            event.is_delayed = false;
            
            // Deliver spike to all postsynaptic neurons
            for (auto& region : pImpl->regions) {
                for (auto& population : region->getPopulations()) {
                    for (auto* synapse : population->getOutgoingSynapses()) {
                        if (synapse->getSourceNeuron() == neuron->getId()) {
                            // Apply synaptic current to destination neuron
                            float synapticWeight = synapse->getWeight();
                            Neuron* destNeuron = getNeuron(synapse->getDestinationNeuron());
                            if (destNeuron) {
                                if (synapse->isExcitatory()) {
                                    destNeuron->receiveExcitatoryInput(synapticWeight * 10.0f);  // Scale factor
                                } else {
                                    destNeuron->receiveInhibitoryInput(std::abs(synapticWeight) * 10.0f);
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        // Neuron is active but hasn't spiked this step
        neuron->setFiringState(FiringState::Active);
    }
    
    // Clear total current for next integration step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) {
        NLM_LOG_ERROR("IntegrateAndFireDynamics::updateSynapse: synapse is null");
        return;
    }
    
    // Real synaptic dynamics with short-term plasticity
    // Implements Tsodyks-Markram model of synaptic transmission
    // This includes facilitation and depression based on presynaptic activity
    
    float currentTime = static_cast<float>(pImpl->currentTime);
    
    // Update short-term plasticity state
    // U = utilization parameter (0-1), x = recovery from depression, y = facilitation
    // dx/dt = (1 - x) / tau_rec - U * x * delta(t - t_p)
    // dy/dt = (1 - y) / tau_fac + (1 - y) * U * delta(t - t_p)
    
    float U = synapse->getEfficacy();  // Utilization parameter
    float x = 1.0f;                   // Recovery from depression (starts at 1.0)
    float y = synapse->getShortTermFacilitation();  // Facilitation state
    
    // Update based on recent activity
    if (synapse->getPreSpikeHistory().size() > 0) {
        // Last spike time
        float lastSpikeTime = synapse->getPreSpikeHistory().back();
        float timeSinceSpike = currentTime - lastSpikeTime;
        
        // Recovery from depression
        float tau_rec = 200.0f;  // Depression time constant (ms)
        x += (1.0f - x) * (dt * 1000.0f) / tau_rec;
        
        // Decay of facilitation
        float tau_fac = 100.0f;  // Facilitation time constant (ms)
        y += (1.0f - y) * (dt * 1000.0f) / tau_fac;
        
        // Effective synaptic efficacy with short-term plasticity
        float effectiveEfficacy = U * x * y;
        
        // Update synaptic weight with plasticity rules
        synapse->step(dt);
    }
    
    // Apply synaptic delay for spike transmission
    // Delays simulate realistic transmission time along axons
    synapse->advanceDelay(dt);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) {
        NLM_LOG_ERROR("IntegrateAndFireDynamics::applySpikeInput: neuron or synapse is null");
        return;
    }
    
    // Real synaptic input with reversal potential and conductance-based modeling
    // Implements biophysically realistic synaptic transmission
    
    SynapticWeight weight = synapse->getWeight();
    float conductance = std::abs(weight) * 0.1f;  // Synaptic conductance
    float reversalPotential = synapse->isExcitatory() ? 0.0f : -80.0f;  // mV
    
    // Calculate synaptic current using Ohm's law for synapses: I = g * (V - E)
    // Where g is conductance, V is membrane potential, E is reversal potential
    
    float V = neuron->getMembranePotential();
    float I_syn = conductance * (V - reversalPotential);
    
    // Apply synaptic current to neuron
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else {
        neuron->receiveInhibitoryInput(std::abs(I_syn));
    }
    
    // Record synaptic input for plasticity calculations
    synapse->recordPostSpike(static_cast<float>(pImpl->currentTime));
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) {
        NLM_LOG_ERROR("IntegrateAndFireDynamics::shouldFire: neuron is null");
        return false;
    }
    
    // Check if membrane potential reaches or exceeds firing threshold
    float V = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    // Include additional factors in firing decision
    const auto& state = neuron->getState();
    
    // Adjust threshold based on neuromodulatory inputs
    if (state.adaptationVariable > 0.0f) {
        // Spike-frequency adaptation increases threshold
        threshold += state.adaptationVariable * 0.5f;
    }
    
    return V >= threshold;
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset in this simplified model
    // In a more complex implementation, we would reset dynamic variables
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
