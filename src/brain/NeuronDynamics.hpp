class IntegrateAndFireDynamics {
public:
    IntegrateAndFireDynamics() = default;
    ~IntegrateAndFireDynamics() = default;
    
    // Update neuron state for one step using real integrate-and-fire dynamics
    void updateNeuron(Neuron* neuron, float dt) {
        if (!neuron) {
            NLM_LOG_ERROR("Null neuron pointer in updateNeuron");
            return;
        }
        
        NeuronState& state = neuron->getState();
        
        // LIF dynamics: dV/dt = (V_rest - V)/tau + I/C
        // V = membrane potential, V_rest = resting potential
        // tau = membrane time constant, I = total current, C = membrane capacitance
        
        float V = state.membranePotential;
        float V_rest = state.restingPotential;
        float I = neuron->getTotalCurrent();
        float tau = 20.0f;  // Membrane time constant (ms)
        float C = 1.0f;     // Membrane capacitance (nF)
        
        // Apply leak term: (V_rest - V)/tau
        float leakTerm = (V_rest - V) / tau;
        
        // Apply current term: I/C
        float currentTerm = I / C;
        
        // Update membrane potential with exponential Euler integration
        // V_new = V + dt * (leakTerm + currentTerm)
        V += dt * (leakTerm + currentTerm);
        
        // Apply spike-frequency adaptation
        if (state.adaptationVariable > 0.0f) {
            V -= state.adaptationVariable * 0.01f;
            state.adaptationVariable *= 0.95f;  // Decay adaptation
        }
        
        // Clamp membrane potential to biologically plausible range
        // Neuron typically fires between -55mV and 50mV
        V = std::clamp(V, -100.0f, 50.0f);
        
        state.membranePotential = V;
        
        // Check for spike threshold crossing
        if (V >= state.threshold && state.firingState != FiringState::Refractory) {
            // Record spike with current time
            Timestamp currentTime = neuron->getLastSpikeTime();
            if (currentTime < 0.0f) {
                // This is the first spike, set a reasonable timestamp
                currentTime = dt;  // Approximate first spike time
            }
            neuron->recordSpike(currentTime);
            
            // Set firing state
            state.firingState = FiringState::Active;
            state.lastSpikeTime = currentTime;
            
            // Reset membrane potential to reset potential
            state.membranePotential = state.resetPotential;
            
            // Enter refractory period
            state.refractoryRemaining = state.refractoryPeriod;
            state.firingState = FiringState::Refractory;
            
            // Increase adaptation variable for spike-frequency adaptation
            state.adaptationVariable += 1.0f;
            
            // Generate spike output
            return;
        }
        
        // Update firing rate based on membrane potential proximity to threshold
        // Firing rate increases as potential approaches threshold
        if (dt > 0.0f) {
            float thresholdProximity = (V - state.threshold) / (state.threshold - V_rest);
            thresholdProximity = std::clamp(thresholdProximity, 0.0f, 1.0f);
            
            // Exponential moving average of firing rate
            state.firingRate = 0.9f * state.firingRate + 0.1f * thresholdProximity * (1000.0f / dt);
        }
        
        // Update firing state based on membrane potential
        if (state.firingState == FiringState::Refractory && state.refractoryRemaining <= 0) {
            state.firingState = FiringState::Resting;
        } else if (state.firingState == FiringState::Active) {
            // Check if neuron should remain active
            if (V < state.threshold * 1.1f) {  // Allow small overshoot
                state.firingState = FiringState::Resting;
            }
        }
    }
    
    // Update synapse state for one step
    void updateSynapse(Synapse* synapse, float dt) {
        if (!synapse) {
            NLM_LOG_ERROR("Null synapse pointer in updateSynapse");
            return;
        }
        
        // Simple exponential decay of synaptic weight
        float weight = synapse->getWeight();
        // STDP will modify this during plasticity updates
        float decayFactor = std::exp(-dt / 10.0f);  // Time constant 10ms
        float newWeight = weight * decayFactor;
        
        // Ensure weight stays within valid bounds
        newWeight = std::clamp(newWeight, 0.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
    
    // Apply synaptic input from spike
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) {
        if (!neuron || !synapse) {
            NLM_LOG_ERROR("Null neuron or synapse pointer in applySpikeInput");
            return;
        }
        
        SynapticWeight weight = synapse->getWeight();
        if (synapse->isExcitatory()) {
            neuron->receiveExcitatoryInput(weight);
        } else if (synapse->isInhibitory()) {
            neuron->receiveInhibitoryInput(std::abs(weight));
        } else if (synapse->isModulatory()) {
            neuron->receiveModulatoryInput(weight);
        }
    }
    
    // Check if neuron should fire based on its current state
    bool shouldFire(const Neuron* neuron) const {
        if (!neuron) {
            NLM_LOG_ERROR("Null neuron pointer in shouldFire");
            return false;
        }
        
        const NeuronState& state = neuron->getState();
        
        // Check threshold crossing
        bool thresholdCrossed = state.membranePotential >= state.threshold;
        
        // Check refractory period
        bool isRefractory = state.refractoryRemaining > 0;
        
        // Check if neuron is in appropriate state to fire
        bool inActiveState = state.firingState == FiringState::Active || 
                           state.firingState == FiringState::Resting;
        
        return thresholdCrossed && !isRefractory && inActiveState;
    }
    
    // Reset dynamics state
    void reset() {
        // No internal state to reset for LIF dynamics
    }
};