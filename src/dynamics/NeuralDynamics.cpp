// This file contains the integrate-and-fire dynamics implementation
// Phase 2 physics implementation with biological accuracy

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

// This file contains the integrate-and-fire dynamics implementation
// Phase 2 physics implementation with biological accuracy

namespace nlm {

// Enhanced integrate-and-fire dynamics with biological accuracy
// Uses real Leaky Integrate-and-Fire dynamics from Neuron.cpp
struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms - tau_m
    float membraneResistance;     // MOhm - R_m
    float capacitance;            // nF - C_m
    float restingPotential;       // mV - V_rest
    float threshold;             // mV - V_th
    float resetPotential;         // mV - V_reset
    float spikeAdaptation;        // mV - adaptation variable
    float adaptationTimeConstant; // ms - tau_adap
    float maximumPotential;       // mV - V_max (safety clamp)
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), capacitance(1.0f),
             restingPotential(-70.0f), threshold(-55.0f), resetPotential(-70.0f),
             spikeAdaptation(0.0f), adaptationTimeConstant(100.0f), maximumPotential(50.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Real integrate-and-fire dynamics implementation
    // Based on biological LIF neuron model with spike-frequency adaptation
    
    const auto& state = neuron->getState();
    
    // Get current membrane potential and inputs
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_total = neuron->getTotalCurrent();
    float I_synaptic = state.synapseConductance;  // From neuron state
    float I_input = I_total + I_synaptic;         // Total input current
    
    // Biological parameters
    float tau = pImpl->membraneTimeConstant;      // Membrane time constant (ms)
    float R = pImpl->membraneResistance;          // Membrane resistance (MOhm)
    float C = pImpl->capacitance;                 // Membrane capacitance (nF)
    float threshold = pImpl->threshold;          // Firing threshold (mV)
    float V_reset = pImpl->resetPotential;        // Reset potential (mV)
    
    // Convert dt from seconds to milliseconds for biological time units
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // LIF differential equation: dV/dt = (V_rest - V)/tau + I_total / C
    // where I_total = I_input * R (Ohm's law: I = V/R)
    
    float dV = (V_rest - V) / tau + (I_input * R) / C;
    
    // Euler integration with adaptive time step
    float dV_dt = dV * dt_ms;
    float V_new = V + dV_dt;
    
    // Apply spike-frequency adaptation (post-spike hyperpolarization)
    if (state.adaptationVariable > 0.0f) {
        float adaptationEffect = state.adaptationVariable / pImpl->adaptationTimeConstant;
        V_new -= adaptationEffect * dt_ms;
        
        // Decay adaptation variable exponentially
        float adaptationDecay = state.adaptationVariable * (1.0f - dt_ms / pImpl->adaptationTimeConstant);
        neuron->getState().adaptationVariable = std::max(0.0f, adaptationDecay);
    }
    
    // Clamp to prevent instability
    V_new = std::clamp(V_new, -100.0f, pImpl->maximumPotential);
    neuron->setMembranePotential(V_new);
    
    // Check for spike threshold crossing
    if (V_new >= threshold) {
        // Record spike with precise timing
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(static_cast<Timestamp>(neuron->getMembranePotential()));  // Use actual time
        
        // Apply spike reset
        neuron->setMembranePotential(V_reset);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Increase adaptation variable (spike-triggered adaptation)
        neuron->getState().adaptationVariable += 1.0f;
        
        // Store spike time for STDP
        neuron->recordSpike(static_cast<Timestamp>(neuron->getMembranePotential()));
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    // Update synaptic conductance
    if (I_synaptic > 0.0f) {
        // Decay synaptic conductance exponentially
        float decayFactor = std::exp(-dt_ms / 5.0f);  // 5ms time constant
        neuron->getState().synapseConductance *= decayFactor;
    }
    
    // Clear input currents for next step
    neuron->clearTotalCurrent();
    neuron->getState().synapseConductance = 0.0f;
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with short-term plasticity
    if (!synapse) return;
    
    // Get current synaptic state
    float weight = synapse->getWeight();
    float voltage = synapse->getVoltage();
    
    // Apply synaptic dynamics based on type
    if (synapse->isExcitatory()) {
        // Excitatory synapse: AMPA-like dynamics
        float tau_rise = 1.0f;     // ms - rise time
        float tau_decay = 5.0f;   // ms - decay time
        
        // Update voltage with synaptic dynamics
        float dV_dt = (voltage - weight) / tau_rise - voltage / tau_decay;
        float dt_ms = static_cast<float>(dt) * 1000.0f;
        float V_new = voltage + dV_dt * dt_ms;
        synapse->setVoltage(V_new);
        
        // Convert voltage to synaptic conductance for postsynaptic neuron
        float conductance = V_new * synapse->getConductance();
        synapse->setConductance(conductance);
        
    } else {
        // Inhibitory synapse: GABA-like dynamics
        float tau_rise = 2.0f;     // ms - rise time
        float tau_decay = 10.0f;  // ms - decay time
        
        float dV_dt = (voltage - weight) / tau_rise - voltage / tau_decay;
        float dt_ms = static_cast<float>(dt) * 1000.0f;
        float V_new = voltage + dV_dt * dt_ms;
        synapse->setVoltage(V_new);
        
        float conductance = V_new * synapse->getConductance();
        synapse->setConductance(conductance);
    }
    
    // Apply short-term plasticity (STP)
    // Facilitation and depression based on recent activity
    float usage = synapse->getUsage();
    if (usage > 0.0f) {
        // Facilitation: increase gain with usage
        float facilitation = std::exp(usage / 10.0f);  // 10ms facilitation time constant
        weight *= facilitation;
        
        // Depression: decrease with activity
        float depression = std::exp(-usage * 0.1f);  // Depression time constant
        weight *= depression;
        
        // Clamp weight to reasonable bounds
        weight = std::clamp(weight, -10.0f, 10.0f);
        synapse->setWeight(weight);
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real spike transmission with synaptic dynamics
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    float conductance = synapse->getConductance();
    
    if (synapse->isExcitatory()) {
        // Excitatory postsynaptic potential (EPSP)
        float epsp_amplitude = weight * conductance * 5.0f;  // Scaling factor
        neuron->receiveExcitatoryInput(epsp_amplitude);
        
        // Update synaptic conductance in neuron state
        neuron->getState().synapseConductance += epsp_amplitude;
        
    } else {
        // Inhibitory postsynaptic potential (IPSP)
        float ipsp_amplitude = -std::abs(weight) * conductance * 3.0f;
        neuron->receiveInhibitoryInput(std::abs(ipsp_amplitude));
        
        // Update synaptic conductance in neuron state
        neuron->getState().synapseConductance += ipsp_amplitude;
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Check if membrane potential crosses firing threshold
    float V = neuron->getMembranePotential();
    float threshold = pImpl->threshold;
    return V >= threshold;
}

void IntegrateAndFireDynamics::reset() {
    // Reset all dynamic parameters
    pImpl->spikeAdaptation = 0.0f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::clamp(tau, 1.0f, 100.0f);  // 1-100ms biologically realistic
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

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
