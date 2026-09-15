// Generate Gaussian noise for stochastic dynamics
static float generateGaussianNoise() {
    // Box-Muller transform for Gaussian random numbers
    static thread_local std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::normal_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    TimestepDuration currentTime = state.lastSpikeTime;  // Approximation
    
    // Get biophysical parameters
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = pImpl->membraneCapacitance;
    
    // Apply synaptic conductances to compute current
    float synapticConductance = state.synapseConductance;
    float leakConductance = state.leakConductance;
    
    // Calculate total synaptic current using conductance-based model
    // I_syn = g_syn * (V - E_syn) where E_syn depends on synapse type
    // For simplicity, we compute effective reversal potential
    float effectiveReversal = 0.0f;
    if (synapticConductance > 0) {
        effectiveReversal = (synapticConductance * pImpl->reversalPotentialEx) / 
                           (synapticConductance + leakConductance);
    }
    
    // External current input (e.g., from sensory inputs)
    float I_ext = neuron->getTotalCurrent();
    
    // Compute membrane current components
    float leakCurrent = (V - V_rest) / R;
    float synapticCurrent = synapticConductance * (effectiveReversal - V);
    
    // Apply stochastic noise for realistic dynamics
    float noise = pImpl->noiseLevel * static_cast<float>(std::sqrt(dt)) * 
                  generateGaussianNoise();
    
    // Stable exponential Euler integration for LIF dynamics
    // dV/dt = (V_rest - V)/tau + (I_ext + I_syn)/C
    float dV_leak = (V_rest - V) / tau * 1000.0f;  // Convert ms to s
    float dV_synaptic = (I_ext + synapticCurrent) / C * 1000.0f;
    
    // Total derivative
    float dV_total = dV_leak + dV_synaptic + noise;
    
    // Update membrane potential with exponential Euler
    V = V + static_cast<float>(dt) * dV_total;
    
    // Apply spike-frequency adaptation
    // Adaptation current increases after each spike and decays
    pImpl->adaptationVariable *= std::exp(-dt / (tau * 0.01f));  // Fast decay
    V -= pImpl->adaptationVariable * 0.01f;  // Subtractive adaptation
    
    // Check for spike threshold crossing
    if (V >= threshold) {
        // Record spike with current time
        neuron->recordSpike(static_cast<float>(currentTime + dt));
        
        // Apply spike-triggered processes
        neuron->setFiringState(FiringState::Active);
        
        // Reset membrane potential with reset potential
        V = V_reset;
        
        // Enter refractory period with adaptive duration
        uint32_t refractorySteps = static_cast<uint32_t>(
            pImpl->refractoryPeriod / (dt * 1000.0f));
        neuron->setRefractoryPeriod(refractorySteps);
        
        // Increase adaptation variable for next spike (spike-frequency adaptation)
        pImpl->adaptationVariable += 1.0f;
        
        // Modulate synaptic efficacy based on spike timing
        neuron->setFiringState(FiringState::Refractory);
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    neuron->setMembranePotential(V);
    
    // Clear synaptic input for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Implement Tsodyks-Markram short-term plasticity dynamics
    // This is the workhorse of realistic synaptic transmission
    
    float currentTime = static_cast<float>(dt * 1000.0f);  // Convert to ms
    
    // Update short-term plasticity using Tsodyks-Markram equations
    // Equations for synaptic efficacy dynamics:
    // dx/dt = (1 - x)/tau_d - u*x*delta(t - t_pre)
    // du/dt = (U - u)/tau_f + (U - u)*x*delta(t - t_pre)
    
    // Get current synaptic state from implementation
    struct Synapse::Impl {
        float shortTermDepression;  // Depression variable (R)
        float shortTermFacilitation; // Facilitation variable (u)
        float lastPreSpikeTime;      // Last presynaptic spike time
        float lastPostSpikeTime;     // Last postsynaptic spike time
    } * synImpl = reinterpret_cast<Impl*>(synapse->pImpl);
    
    float& x = synapse->getEfficacy();  // x: available resources (synaptic efficacy)
    float& u = synImpl->shortTermFacilitation; // u: utilization factor (facilitation)
    float& R = synImpl->shortTermDepression;   // R: recovery from depression
    
    // Update short-term plasticity
    float U = pImpl->U;  // Utilization parameter from dynamics
    float tau_f = pImpl->tau_f;  // Facilitation time constant
    float tau_d = pImpl->tau_d;  // Depression time constant
    
    // Track last spike times
    float& t_pre = synImpl->lastPreSpikeTime;
    float& t_post = synImpl->lastPostSpikeTime;
    
    // Tsodyks-Markram STP dynamics
    // 1. Facilitation decay (recovery toward U)
    if (t_pre >= 0.0f) {
        u += (U - u) * (1.0f - std::exp(-dt * 1000.0f / tau_f));
    } else {
        u += (U - u) * (1.0f - std::exp(-dt * 1000.0f / tau_f));
    }
    
    // 2. Depression (recovery of resources)
    if (t_pre >= 0.0f || t_post >= 0.0f) {
        float timeSinceLastActivity = std::max(
            t_post >= 0.0f ? currentTime - t_post : 0.0f,
            t_pre >= 0.0f ? currentTime - t_pre : 0.0f
        );
        // Recovery equation: dx/dt = (1 - x)/tau_d
        float recoveryRate = (1.0f - x) / (tau_d / 1000.0f); // Convert ms to s
        x += recoveryRate * dt;
    }
    
    // 3. Spike-triggered depression (use-dependent depression)
    if (t_pre >= 0.0f) {
        x = x * (1.0f - U * x);
    }
    
    // Clamp values to physiological ranges
    u = std::clamp(u, 0.0f, U);
    x = std::clamp(x, 0.0f, 1.0f);
    
    // Update synaptic efficacy (normalized between 0 and 1)
    synapse->setEfficacy(x);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply synaptic input with conductance-based dynamics
    // Realistic synaptic transmission with reversal potentials
    
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();  // Use-dependent synaptic strength
    
    if (synapse->isExcitatory()) {
        // Excitatory synapse: add to synaptic conductance
        float reversalPotential = pImpl->reversalPotentialEx;
        float synapticCurrent = weight * efficacy * 0.1f;  // Scale factor
        neuron->receiveExcitatoryInput(synapticCurrent);
        
        // Also add to neuron's synaptic conductance for conductance-based model
        neuron->addToSynapticConductance(synapticCurrent);
    } else {
        // Inhibitory synapse: subtract from synaptic conductance
        float reversalPotential = pImpl->reversalPotentialIn;
        float synapticCurrent = -std::abs(weight) * efficacy * 0.1f;  // Scale factor
        neuron->receiveInhibitoryInput(synapticCurrent);
        
        // neuron->addToSynapticConductance(synapticCurrent);
    }
    
    // Apply spike-timing dependent effects on synaptic release
    // For realistic synapses, spike timing affects release probability
    neuron->setFiringState(FiringState::Active);
}

} // namespace nlm
