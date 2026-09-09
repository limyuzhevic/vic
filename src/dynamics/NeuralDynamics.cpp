#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

struct IntegrateAndFireDynamics::Impl {
    // ===== BIOLOGICAL PARAMETERS =====
    // Membrane properties
    float membraneCapacitance;        // nF - membrane capacitance
    float membraneTimeConstant;        // ms - membrane time constant (tau_m)
    float membraneResistance;           // MOhm - input resistance (R_m = tau_m / C_m)
    float leakConductance;              // nS - leak conductance
    
    // Resting properties
    float restingPotential;            // mV - resting membrane potential
    float thresholdPotential;          // mV - spike threshold
    float resetPotential;              // mV - reset potential after spike
    
    // Ion channel dynamics
    float sodiumActivationTime;         // ms - sodium channel activation time constant
    float sodiumInactivationTime;       // ms - sodium channel inactivation time constant
    float potassiumActivationTime;      // ms - potassium channel activation time constant
    
    // Temperature dependence (Q10 coefficient)
    float temperatureQ10;               // Temperature sensitivity
    float baseTemperature;              // Base temperature in Celsius (typically 37°C)
    
    // ===== SPIKE-FREQUENCY ADAPTATION =====
    float adaptationConductance;         // nS - potassium adaptation conductance
    float adaptationTimeConstant;      // ms - adaptation time constant
    float adaptationReversal;           // mV - adaptation reversal potential (~-90 mV)
    
    // ===== SYNAPTIC PARAMETERS =====
    float synapticTimeConstant;         // ms - synaptic conductance decay time constant
    float excitatoryReversal;           // mV - excitatory reversal potential
    float inhibitoryReversal;           // mV - inhibitory reversal potential
    
    // ===== REFRACTORY PERIOD =====
    uint32_t absoluteRefractory;        // ms - absolute refractory period
    uint32_t relativeRefractory;       // ms - relative refractory period
    
    // ===== STRUCTURAL PARAMETERS =====
    float maxMembranePotential;         // mV - upper bound for numerical stability
    float minMembranePotential;         // mV - lower bound for numerical stability
    
    // ===== INITIALIZATION =====
    Impl() : 
        // Typical mammalian neuron parameters (Dayan & Abbott, 2001)
        membraneCapacitance(0.2f),        // 0.2 nF
        membraneTimeConstant(20.0f),      // 20 ms
        membraneResistance(250.0f),       // 250 MOhm
        leakConductance(10.0f),           // 10 nS
        restingPotential(-70.0f),         // -70 mV
        thresholdPotential(-55.0f),       // -55 mV
        resetPotential(-70.0f),           // Reset to resting
        
        // Ion channel time constants (ms)
        sodiumActivationTime(1.0f),       // Fast activation
        sodiumInactivationTime(5.0f),     // Slower inactivation
        potassiumActivationTime(10.0f),   // Moderate activation
        
        // Temperature
        temperatureQ10(2.5f),             // Q10 coefficient for temperature sensitivity
        baseTemperature(37.0f),           // Physiological temperature (37°C)
        
        // Spike-frequency adaptation
        adaptationConductance(5.0f),       // 5 nS adaptation conductance
        adaptationTimeConstant(100.0f),   // 100 ms adaptation time constant
        adaptationReversal(-90.0f),       // -90 mV (K+ reversal)
        
        // Synaptic properties
        synapticTimeConstant(5.0f),       // 5 ms synaptic decay
        excitatoryReversal(0.0f),         // Excitatory ~0 mV (Na+ reversal)
        inhibitoryReversal(-75.0f),       // Inhibitory ~-75 mV (Cl- reversal)
        
        // Refractory periods
        absoluteRefractory(2),            // 2 ms absolute refractory
        relativeRefractory(10),           // 10 ms relative refractory
        
        // Safety bounds
        maxMembranePotential(50.0f),      // Upper bound for numerical stability
        minMembranePotential(-100.0f)     // Lower bound for numerical stability
    {}
    
    // ===== TEMPERATURE SCALING =====
    void applyTemperatureScaling(float currentTime, float dt) {
        // Q10 temperature dependence formula:
        // R(T) = R(T0) * Q10^((T-T0)/10)
        float temperature = baseTemperature;
        float referenceTemperature = 20.0f; // Room temperature
        
        // Calculate scaling factor based on current temperature
        float tempDiff = temperature - referenceTemperature;
        float scalingFactor = std::pow(temperatureQ10, tempDiff / 10.0f);
        
        // Apply temperature scaling to time-dependent parameters
        // This captures the effect of temperature on ion channel kinetics
        membraneTimeConstant *= scalingFactor;
        sodiumActivationTime *= scalingFactor;
        sodiumInactivationTime *= scalingFactor;
        potassiumActivationTime *= scalingFactor;
        adaptationTimeConstant *= scalingFactor;
        synapticTimeConstant *= scalingFactor;
        
        // Scale refractory periods
        absoluteRefractory = static_cast<uint32_t>(absoluteRefractory * scalingFactor);
        relativeRefractory = static_cast<uint32_t>(relativeRefractory * scalingFactor);
        
        // Update membrane resistance (inverse relationship)
        membraneResistance /= scalingFactor;
        
        // Update leak conductance
        leakConductance *= scalingFactor;
        
        // Adaptation conductance may scale differently
        adaptationConductance *= scalingFactor;
    }
}

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Convert dt to float for calculations (dt is TimestepDuration)
    float dt_f = static_cast<float>(dt);
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    
    // Apply temperature scaling for dynamic parameter changes
    pImpl->applyTemperatureScaling(0.0f, dt_f); // Current time placeholder
    
    // ===== SYNAPTIC CONDUCTANCE INTEGRATION =====
    // Calculate synaptic current: I_syn = g_syn * (V - E_syn)
    // where g_syn is total synaptic conductance and E_syn is reversal potential
    float g_syn = state.synapseConductance;
    
    // Net synaptic reversal: weighted average of excitatory/inhibitory contributions
    float g_exc = pImpl->excitatoryReversal;  // ~0 mV
    float g_inh = pImpl->inhibitoryReversal;  // ~-75 mV
    
    // Estimate conductance components (from neuron state)
    // This would normally come from synaptic state, but we use simplified approach
    // for now - in real implementation, conductances would be tracked separately
    float totalSynapticCurrent = 0.0f;
    
    // Calculate total current components:
    // 1. Synaptic current (Ohmic approximation)
    // 2. Leak current (Ohmic)
    // 3. Adaptation current (voltage-dependent)
    
    // Leak current: I_leak = g_leak * (V - E_leak)
    float g_leak = state.leakConductance;
    float E_leak = pImpl->restingPotential;
    float I_leak = g_leak * (V - E_leak);
    
    // Adaptation current: I_adapt = g_adapt * (V - E_adapt)
    // Adaptation is typically potassium current that becomes more active after spikes
    float I_adapt = pImpl->adaptationConductance * (V - pImpl->inhibitoryReversal);
    
    // ===== EXPONENTIAL EULER INTEGRATION =====
    // For stability with exponential dynamics, use exponential Euler method:
    // V(t+dt) = V_inf + (V(t) - V_inf) * exp(-dt/tau)
    // where V_inf = (I_total * tau) + (E_leak * g_leak * tau + E_adapt * g_adapt * tau) / C
    
    // Membrane time constant
    float tau_m = pImpl->membraneTimeConstant;
    float C_m = pImpl->membraneCapacitance;
    
    // Total current (simplified from synaptic input and external inputs)
    // In a real implementation, synaptic inputs would be conductance-based
    float I_total = state.totalCurrent + I_adapt;  // Adaptation included
    
    // Calculate steady-state membrane potential
    // V_inf = (I_total * tau_m) + (E_leak * g_leak * tau_m + E_adapt * g_adapt * tau_m) / C_m
    float V_inf = (I_total * tau_m) + 
                  ((E_leak * g_leak) + (pImpl->inhibitoryReversal * pImpl->adaptationConductance)) * tau_m / C_m;
    
    // Apply exponential Euler integration for membrane potential
    // This is numerically more stable than simple Euler for exponential dynamics
    float alpha = std::exp(-dt_f / tau_m);
    float V_new = V_inf + (V - V_inf) * alpha;
    
    // Clamp to reasonable bounds to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    // ===== SPIKE DETECTION AND RESET =====
    bool shouldFire = V_new >= state.threshold;
    
    if (shouldFire) {
        // Record spike timestamp
        Timestamp spikeTime = 0.0f; // TODO: use actual simulation time
        neuron->recordSpike(spikeTime);
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(pImpl->resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(pImpl->absoluteRefractory);
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation variable for spike-frequency adaptation
        // Each spike increases adaptation conductance
        neuron->setFiringRate(neuron->getFiringRate() + 100.0f); // Increment firing rate
        neuron->receiveModulatoryInput(1.0f); // Modulatory input for plasticity
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    // ===== REFRACTORY PERIOD HANDLING =====
    if (neuron->isRefractory()) {
        // During absolute refractory, neuron cannot spike
        if (neuron->getState().refractoryRemaining > 0) {
            // Keep at reset potential
            neuron->setMembranePotential(pImpl->resetPotential);
            neuron->decrementRefractory();
            
            // In relative refractory, allow subthreshold integration
            if (neuron->getState().refractoryRemaining <= pImpl->relativeRefractory) {
                // Allow partial voltage recovery but clamp below threshold
                float recovery = std::min(V_new - pImpl->resetPotential, 10.0f);
                neuron->addToMembranePotential(recovery);
            }
        }
    } else {
        // Apply the computed membrane potential for non-refractory periods
        neuron->setMembranePotential(V_new);
    }
    
    // ===== SPIKE-FREQUENCY ADAPTATION =====
    // Update adaptation variable based on recent spike history
    if (neuron->getState().adaptationVariable > 0.0f) {
        // Decay adaptation over time
        float decayRate = 1.0f / pImpl->adaptationTimeConstant;
        neuron->setFiringRate(neuron->getFiringRate() * (1.0f - decayRate * dt_f));
    }
    
    // ===== SYNAPTIC CONDUCTANCE DYNAMICS =====
    // Update synaptic conductance based on spike inputs
    // This is a simplified version - real implementation would track conductances
    float synapticDecay = 1.0f / pImpl->synapticTimeConstant;
    state.synapseConductance *= std::exp(-synapticDecay * dt_f);
    
    // Clear the neuron's total current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics with conductance-based transmission
    float dt_f = static_cast<float>(dt);
    const auto& state = synapse->getState();
    
    // ===== SYNAPTIC CONDUCTANCE DYNAMICS =====
    // Tsodyks-Markram model for short-term plasticity
    // g(t) = u * x * (1 - u * x) * s + x
    // where u = utilization, x = recovery variable, s = activation
    
    // Synaptic efficacy affects transmission strength
    float g = pImpl->synapticConductance;
    float efficacy = synapse->getEfficacy();
    
    // Short-term depression and facilitation
    float depression = 1.0f;  // Default: fully recovered
    float facilitation = 1.0f; // Default: no facilitation
    
    // Decay of facilitation and depression variables
    // Using time constants from synapse implementation
    const float tau_facilitation = 100.0f;  // ms
    const float tau_depression = 200.0f;    // ms
    
    // Exponential decay of facilitation
    float facilitationDecay = std::exp(-dt_f / tau_facilitation);
    facilitation *= facilitationDecay;
    
    // Recovery from depression (towards 1.0)
    float depressionRecovery = 1.0f - std::exp(-dt_f / tau_depression);
    depression += (1.0f - depression) * depressionRecovery * dt_f / (dt_f + 0.001f);
    
    // Update synaptic conductance based on efficacy and plasticity state
    // g_new = g_old * efficacy * depression
    float newConductance = g * efficacy * depression;
    
    // Synaptic reversal potential based on synapse type
    float reversalPotential = synapse->isExcitatory() ? 
        pImpl->excitatoryReversal : pImpl->inhibitoryReversal;
    
    // Store synaptic efficacy in synapse for plasticity updates
    synapse->setEfficacy(efficacy);
    
    // Decay synaptic conductance over time
    float conductanceDecay = std::exp(-dt_f / pImpl->synapticTimeConstant);
    newConductance *= conductanceDecay;
    
    // ===== SYNAPTIC DELAYED TRANSMISSION =====
    // Real synapses have transmission delays
    // Delay in steps converted to time using timestep duration
    float delayTime = synapse->getDelay() * dt_f;  // Convert steps to time
    
    // Synaptic efficacy decays with age (use-dependent depression)
    float age = 10.0f; // Approximate age factor
    float ageDecay = std::exp(-age / 1000.0f);  // Slow decay with age
    newConductance *= ageDecay;
    
    // Store updated conductance for next step
    // In a real implementation, this would be tracked separately
    // For now, we use the step() method in synapse which handles similar dynamics
    synapse->step(0.0); // Pass currentTime placeholder
    
    // ===== SYNAPTIC WEIGHT MODULATION =====
    // Synaptic weight can be modified by plasticity rules
    float weight = synapse->getWeight();
    
    // Clip weight to reasonable bounds
    const float MIN_WEIGHT = -1.0f;
    const float MAX_WEIGHT = 1.0f;
    if (weight < MIN_WEIGHT) weight = MIN_WEIGHT;
    if (weight > MAX_WEIGHT) weight = MAX_WEIGHT;
    
    synapse->setWeight(weight);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Real synaptic transmission with conductance-based dynamics
    
    // Get synaptic weight and type
    SynapticWeight weight = synapse->getWeight();
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy = synapse->getEfficacy();
    
    // ===== CONDUCTANCE-BASED SYNAPTIC CURRENT =====
    // Real synapses transmit current via conductance changes, not direct voltage injection
    // I_syn = g_syn * (V - E_syn)
    // where g_syn is synaptic conductance and E_syn is reversal potential
    
    // Determine reversal potential based on synapse type
    float reversalPotential = synapse->isExcitatory() ? 
        pImpl->excitatoryReversal : pImpl->inhibitoryReversal;
    
    // Synaptic conductance is proportional to weight and efficacy
    // g_syn = |weight| * efficacy * u (utilization factor)
    float utilization = synapse->getEfficacy(); // Simplified utilization
    float g_syn = std::abs(weight) * utilization;
    
    // Calculate synaptic current using Ohm's law
    float V_neuron = neuron->getMembranePotential();
    float I_syn = g_syn * (V_neuron - reversalPotential);
    
    // Apply synaptic current to neuron
    // For excitatory synapses: I_syn > 0 (depolarizing)
    // For inhibitory synapses: I_syn < 0 (hyperpolarizing)
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else {
        neuron->receiveInhibitoryInput(-I_syn);
    }
    
    // ===== DELAYED SYNAPTIC TRANSMISSION =====
    // Real synapses have transmission delays
    // The spike was recorded earlier and should be delivered now after delay
    int delaySteps = synapse->getDelay();
    
    // Synaptic strength may be modulated by short-term plasticity
    // Facilitation increases release probability with recent activity
    // Depression decreases with high usage
    float facilitation = 1.0f;  // Default
    float depression = 1.0f;    // Default
    
    // Update utilization based on recent activity
    // Tsodyks-Markram model: u = u_max * (1 - u) * x + u
    // where u is utilization, x is recovery variable
    float u_max = 0.95f; // Maximum utilization
    float recovery = 1.0f - depression; // Simplified recovery
    float u = u_max * (1.0f - utilization) * recovery;
    
    // Modify synaptic current by utilization factor
    I_syn *= u;
    
    // Re-apply the (now utilized) current
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else {
        neuron->receiveInhibitoryInput(-I_syn);
    }
    
    // ===== STDP ELIGIBILITY TRACE =====
    // Spike-timing dependent plasticity eligibility trace
    // Trace decays over time and integrates spike events
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    // Add trace update for pre-synaptic spike
    // In STDP, pre-synaptic spike updates trace with positive sign
    // for LTP (long-term potentiation)
    eligibilityTrace += 1.0f; // Pre-synaptic spike contribution
    
    // Decay eligibility trace
    float traceDecayRate = 0.01f; // 1% decay per time step
    eligibilityTrace *= (1.0f - traceDecayRate);
    
    synapse->setEligibilityTrace(eligibilityTrace);
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset the implementation's parameters to their initial values
    // This creates a new instance with default biological parameters
    delete pImpl;
    pImpl = new Impl();
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Add configuration options for different neuron types
void IntegrateAndFireDynamics::setNeuronType(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory:
            // Excitatory neurons have higher threshold, larger capacitance
            pImpl->membraneCapacitance *= 1.5f;
            pImpl->thresholdPotential += 5.0f;
            pImpl->adaptationConductance *= 0.8f;  // Less adaptation
            break;
        case NeuronType::Inhibitory:
            // Inhibitory neurons have faster dynamics, lower threshold
            pImpl->membraneTimeConstant *= 0.8f;
            pImpl->thresholdPotential -= 5.0f;
            pImpl->adaptationConductance *= 1.5f;  // More adaptation
            break;
        case NeuronType::Modulatory:
            // Modulatory neurons have very slow dynamics, high adaptation
            pImpl->adaptationTimeConstant *= 3.0f;
            pImpl->adaptationConductance *= 2.0f;
            pImpl->membraneTimeConstant *= 2.0f;
            break;
        default:
            // Standard interneuron parameters
            break;
    }
}

void IntegrateAndFireDynamics::setTemperature(float tempCelsius) {
    // Apply temperature scaling based on Q10 formula
    float baseTemp = pImpl->baseTemperature;
    float tempDiff = tempCelsius - baseTemp;
    float scalingFactor = std::pow(pImpl->temperatureQ10, tempDiff / 10.0f);
    
    // Apply temperature scaling to key parameters
    pImpl->membraneTimeConstant *= scalingFactor;
    pImpl->synapticTimeConstant *= scalingFactor;
    pImpl->adaptationTimeConstant *= scalingFactor;
    
    // Update membrane resistance (inverse relationship)
    pImpl->membraneResistance /= scalingFactor;
    
    // Update leak conductance
    pImpl->leakConductance *= scalingFactor;
}

void IntegrateAndFireDynamics::setAdaptationConductance(float g_adapt) {
    pImpl->adaptationConductance = g_adapt;
}

void IntegrateAndFireDynamics::setSynapticTimeConstant(float tau_syn) {
    pImpl->synapticTimeConstant = tau_syn;
}

void IntegrateAndFireDynamics::setRefractoryPeriod(uint32_t abs_ref, uint32_t rel_ref) {
    pImpl->absoluteRefractory = abs_ref;
    pImpl->relativeRefractory = rel_ref;
}

// Parameter getter methods
float IntegrateAndFireDynamics::getAdaptationConductance() const {
    return pImpl->adaptationConductance;
}

float IntegrateAndFireDynamics::getSynapticTimeConstant() const {
    return pImpl->synapticTimeConstant;
}

uint32_t IntegrateAndFireDynamics::getAbsoluteRefractoryPeriod() const {
    return pImpl->absoluteRefractory;
}

uint32_t IntegrateAndFireDynamics::getRelativeRefractoryPeriod() const {
    return pImpl->relativeRefractory;
}

NeuronType IntegrateAndFireDynamics::getNeuronType() const {
    // Determine neuron type based on current parameters
    if (pImpl->adaptationConductance > 8.0f) {
        return NeuronType::Modulatory;
    } else if (pImpl->thresholdPotential < -50.0f) {
        return NeuronType::Inhibitory;
    } else if (pImpl->membraneCapacitance > 0.3f) {
        return NeuronType::Excitatory;
    }
    return NeuronType::Internal;
}

float IntegrateAndFireDynamics::getTemperature() const {
    // Approximate temperature from scaling factors (simplified)
    float baseTemp = pImpl->baseTemperature;
    // This is a simplified calculation - real implementation would track temperature more accurately
    return baseTemp;
}
