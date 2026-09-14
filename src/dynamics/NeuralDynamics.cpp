#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    // Membrane properties
    float membraneTimeConstant;      // ms (tau_m)
    float membraneCapacitance;       // nF
    float membraneResistance;        // MOhm
    
    // Synaptic properties
    float excitatoryReversalPotential;  // mV (E_e)
    float inhibitoryReversalPotential;  // mV (E_i)
    float excitatorySynapticTimeConstant; // ms (tau_e)
    float inhibitorySynapticTimeConstant; // ms (tau_i)
    
    // Short-term plasticity
    bool shortTermPlasticityEnabled;
    
    // STDP parameters
    float stdpLearningRate;           // Learning rate for STDP
    float stdpTimeConstant;           // Time constant for STDP decay
    float stdpTraceDecay;             // Trace decay rate
    
    // Neuromodulation parameters
    float rewardModulationStrength;   // Strength of reward modulation
    float neuromodulationDecay;       // Decay rate for neuromodulatory signals
    
    // Spike recording
    float minSpikeInterval;           // Minimum interval between spikes (ms)
    float maxAdaptationCurrent;       // Maximum adaptation current
    
    Impl() : 
        membraneTimeConstant(20.0f),     // Typical neuron tau_m: 10-20ms
        membraneCapacitance(1.0f),      // Typical neuron C: 0.5-2nF
        membraneResistance(10.0f),      // R = tau_m / C = 20MOhm
        excitatoryReversalPotential(0.0f),  // Excitatory reversal
        inhibitoryReversalPotential(-80.0f), // Inhibitory reversal
        excitatorySynapticTimeConstant(5.0f), // AMPA/NMDA time constants
        inhibitorySynapticTimeConstant(10.0f), // GABA time constants
        shortTermPlasticityEnabled(true),
        stdpLearningRate(0.01f),        // STDP learning rate
        stdpTimeConstant(20.0f),        // STDP time constant
        stdpTraceDecay(0.005f),         // Trace decay per ms
        rewardModulationStrength(0.1f), // Reward modulation strength
        neuromodulationDecay(0.95f),    // Fast decay
        minSpikeInterval(2.0f),         // Minimum 2ms between spikes
        maxAdaptationCurrent(10.0f)     // Maximum adaptation current
    {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const NeuronState& state = neuron->getState();
    TimestepDuration currentTime = neuron->getLastSpikeTime();
    
    // Convert to milliseconds for easier calculations
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Get synaptic conductance
    float g_syn = state.synapseConductance;
    float g_leak = state.leakConductance;
    
    // Calculate currents
    float I_leak = g_leak * (state.restingPotential - neuron->getMembranePotential());
    float I_syn = g_syn * (neuron->getMembranePotential() - pImpl->excitatoryReversalPotential);
    
    // Membrane potential dynamics: C*dV/dt = I_leak + I_syn - I_adaptation
    // Using exponential Euler integration for stability
    float C = pImpl->membraneCapacitance;
    
    // Exponential leak term
    float leakFactor = std::exp(-dt_ms / pImpl->membraneTimeConstant);
    
    // Update membrane potential with leak, synaptic input, and adaptation
    float V = neuron->getMembranePotential();
    float I_adaptation = state.adaptationVariable;
    
    // Integration: V(t+dt) = V(t)*exp(-dt/tau) + (I/C)*(tau*(1-exp(-dt/tau)))
    float V_new = V * leakFactor + (I_leak + I_syn - I_adaptation) * pImpl->membraneTimeConstant / C * (1.0f - leakFactor);
    
    // Apply spike-frequency adaptation
    if (I_adaptation > 0.0f) {
        float decayFactor = std::exp(-dt_ms / 100.0f); // 100ms time constant
        neuron->getState().adaptationVariable = I_adaptation * decayFactor;
        V_new -= I_adaptation * dt_ms * 0.01f; // Slow hyperpolarization
    }
    
    // Clamp membrane potential to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    neuron->setMembranePotential(V_new);
    
    // Check for spike
    if (neuron->getMembranePotential() >= neuron->getThreshold()) {
        Timestamp spikeTime = currentTime;
        
        // Record spike with actual timing
        neuron->recordSpike(spikeTime);
        
        // Set firing state
        neuron->setFiringState(FiringState::Active);
        
        // Update last spike time
        neuron->getState().lastSpikeTime = spikeTime;
        
        // Reset membrane potential to reset value
        neuron->setMembranePotential(neuron->getState().resetPotential);
        
        // Enter refractory period
        uint32_t refractorySteps = static_cast<uint32_t>(pImpl->minSpikeInterval / dt_ms);
        neuron->setRefractoryPeriod(refractorySteps);
        neuron->decrementRefractory(); // Initialize refractory counter
        
        // Increase adaptation current for spike-frequency adaptation
        neuron->getState().adaptationVariable = std::min(neuron->getState().adaptationVariable + 2.0f, pImpl->maxAdaptationCurrent);
        
        // Enable plasticity for this neuron
        neuron->enablePlasticity(true, true, true);
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    const auto& preHistory = synapse->getPreSpikeHistory();
    const auto& postHistory = synapse->getPostSpikeHistory();
    
    if (preHistory.empty() && postHistory.empty()) {
        return; // No activity, nothing to update
    }
    
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Get current time from most recent spike
    Timestamp currentTime = std::numeric_limits<float>::lowest();
    if (!preHistory.empty()) {
        currentTime = preHistory.back();
    } else if (!postHistory.empty()) {
        currentTime = postHistory.back();
    }
    
    // Update synaptic weight based on STDP
    if (synapse->getPlasticityFlags().stdp) {
        // Implement STDP: pre-before-post strengthens, post-before-pre weakens
        float totalWeightChange = 0.0f;
        
        for (auto preSpike : preHistory) {
            for (auto postSpike : postHistory) {
                float delta_t = static_cast<float>(preSpike - postSpike); // Δt = t_pre - t_post
                
                if (delta_t > 0) {
                    // Pre before post (Δt > 0): POTENTIATION
                    // NMDA receptor-dependent calcium influx
                    totalWeightChange += pImpl->stdpLearningRate * std::exp(-delta_t / pImpl->stdpTimeConstant);
                } else if (delta_t < 0) {
                    // Post before pre (Δt < 0): DEPRESSION
                    // Backpropagating action potential without NMDA activation
                    totalWeightChange -= pImpl->stdpLearningRate * std::exp(delta_t / pImpl->stdpTimeConstant);
                }
                // Δt == 0: no change (simultaneous spikes)
            }
        }
        
        if (std::abs(totalWeightChange) > 1e-6f) {
            synapse->addToWeight(totalWeightChange);
        }
    }
    
    // Update synaptic efficacy (use-dependent modulation)
    if (pImpl->shortTermPlasticityEnabled && !preHistory.empty()) {
        Timestamp lastPreSpike = preHistory.back();
        float timeSinceLastPre = static_cast<float>(currentTime - lastPreSpike);
        
        // Facilitation factor: Tsodyks-Markram model
        float U = synapse->getEfficacy(); // Utilization factor
        float x_f = std::exp(-timeSinceLastPre / pImpl->excitatorySynapticTimeConstant); // Facilitation
        
        // Update efficacy with use-dependent modulation
        float efficacy = U * (1.0f + x_f);
        efficacy = std::min(efficacy, 2.0f); // Cap maximum efficacy
        synapse->setEfficacy(efficacy);
    }
    
    // Apply neuromodulation if reward-modulated learning is enabled
    if (synapse->getPlasticityFlags().reward_modulated) {
        float eligibilityTrace = synapse->getEligibilityTrace();
        
        // Decay eligibility trace
        eligibilityTrace *= std::exp(-dt_ms * pImpl->stdpTraceDecay);
        synapse->setEligibilityTrace(eligibilityTrace);
        
        // Update weight based on eligibility trace and reward
        if (std::abs(eligibilityTrace) > 1e-6f) {
            // Simplified reward modulation: weight change proportional to eligibility trace
            float weightChange = pImpl->rewardModulationStrength * eligibilityTrace;
            synapse->addToWeight(weightChange);
        }
    }
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!synapse->isExcitatory() && !synapse->isInhibitory()) {
        return; // No effect for non-excitatory/inhibitory synapses
    }
    
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Calculate synaptic current using conductance-based model
    float reversalPotential = synapse->isExcitatory() ? 
        pImpl->excitatoryReversalPotential : 
        pImpl->inhibitoryReversalPotential;
    
    // Compute conductance: g = weight * efficacy
    float conductance = weight * efficacy;
    
    // Calculate current: I_syn = g * (V_m - E)
    float V_m = neuron->getMembranePotential();
    float I_syn = conductance * (V_m - reversalPotential);
    
    // Apply to neuron based on synapse type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(I_syn);
    } else {
        neuron->receiveInhibitoryInput(std::abs(I_syn));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Check if membrane potential reaches threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal dynamics state
    delete pImpl;
    pImpl = new Impl();
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
    pImpl->membraneResistance = tau / pImpl->membraneCapacitance;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::setMembraneCapacitance(float C) {
    pImpl->membraneCapacitance = C;
    pImpl->membraneResistance = pImpl->membraneTimeConstant / C;
}

float IntegrateAndFireDynamics::getMembraneCapacitance() const {
    return pImpl->membraneCapacitance;
}

void IntegrateAndFireDynamics::setExcitatoryReversalPotential(float V_e) {
    pImpl->excitatoryReversalPotential = V_e;
}

void IntegrateAndFireDynamics::setInhibitoryReversalPotential(float V_i) {
    pImpl->inhibitoryReversalPotential = V_i;
}

void IntegrateAndFireDynamics::enableShortTermPlasticity(bool enable) {
    pImpl->shortTermPlasticityEnabled = enable;
}

void IntegrateAndFireDynamics::setExcitatorySynapticTimeConstant(float tau_e) {
    pImpl->excitatorySynapticTimeConstant = tau_e;
}

void IntegrateAndFireDynamics::setInhibitorySynapticTimeConstant(float tau_i) {
    pImpl->inhibitorySynapticTimeConstant = tau_i;
}

void IntegrateAndFireDynamics::setSTDPParameters(float learningRate, float timeConstant, float traceDecay) {
    pImpl->stdpLearningRate = learningRate;
    pImpl->stdpTimeConstant = timeConstant;
    pImpl->stdpTraceDecay = traceDecay;
}

void IntegrateAndFireDynamics::setRewardModulation(float strength) {
    pImpl->rewardModulationStrength = strength;
}

void IntegrateAndFireDynamics::setMinSpikeInterval(float minInterval) {
    pImpl->minSpikeInterval = minInterval;
}

void IntegrateAndFireDynamics::setMaxAdaptationCurrent(float maxCurrent) {
    pImpl->maxAdaptationCurrent = maxCurrent;
}

struct HodgkinHuxleyDynamics::Impl {
    // HH parameters
    float membraneCapacitance;       // nF
    float membraneLeakConductance;   // mS/cm²
    float membraneLeakReversal;      // mV
    float sodiumConductanceMax;      // mS/cm²
    float potassiumConductanceMax;   // mS/cm²
    float sodiumReversalPotential;   // mV
    float potassiumReversalPotential; // mV
    
    // Temperature factor (Q10 rule)
    float temperature;               // Celsius
    float tempCorrection;            // Scaling factor
    
    // Gating variable dynamics
    float m, h, n;                    // Activation variables [0,1]
    float alpha_m, beta_m;            // Sodium activation rates
    float alpha_h, beta_h;            // Sodium inactivation rates
    float alpha_n, beta_n;            // Potassium activation rates
    
    Impl() : 
        membraneCapacitance(0.1f),      // Typical HH C_m
        membraneLeakConductance(0.003f), // Leak conductance
        membraneLeakReversal(-54.387f), // Leak reversal
        sodiumConductanceMax(120.0f),   // Max sodium conductance
        potassiumConductanceMax(36.0f), // Max potassium conductance
        sodiumReversalPotential(50.0f), // Sodium reversal
        potassiumReversalPotential(-77.0f), // Potassium reversal
        temperature(6.3f),              // Room temperature ~37°C
        tempCorrection(1.0f),
        m(0.05f), h(0.6f), n(0.325f),   // Initial gating variables
        alpha_m(0.0f), alpha_h(0.0f), alpha_n(0.0f),
        beta_m(0.0f), beta_h(0.0f), beta_n(0.0f)
    {
        updateGatingRates(0.0f); // Initialize
    }
    
    void updateGatingRates(float V) {
        // Temperature correction factor
        float phi = std::pow(3.0f, (temperature - 6.3f) / 10.0f);
        
        // Sodium activation (m)
        alpha_m = phi * 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
        beta_m = phi * 4.0f * std::exp(-(V + 65.0f) / 18.0f);
        
        // Sodium inactivation (h)
        alpha_h = phi * 0.07f * std::exp(-(V + 65.0f) / 20.0f);
        beta_h = phi * 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
        
        // Potassium activation (n)
        alpha_n = phi * 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
        beta_n = phi * 0.125f * std::exp(-(V + 65.0f) / 80.0f);
        
        // Update steady-state values
        float alpha_m_inf = alpha_m / (alpha_m + beta_m);
        float beta_m_inf = beta_m / (alpha_m + beta_m);
        m = alpha_m_inf + (m - alpha_m_inf) * std::exp(-dt / (alpha_m + beta_m));
        
        float alpha_h_inf = alpha_h / (alpha_h + beta_h);
        float beta_h_inf = beta_h / (alpha_h + beta_h);
        h = alpha_h_inf + (h - alpha_h_inf) * std::exp(-dt / (alpha_h + beta_h));
        
        float alpha_n_inf = alpha_n / (alpha_n + beta_n);
        float beta_n_inf = beta_n / (alpha_n + beta_n);
        n = alpha_n_inf + (n - alpha_n_inf) * std::exp(-dt / (alpha_n + beta_n));
    }
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const NeuronState& state = neuron->getState();
    
    // Convert to milliseconds for easier calculations
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Get membrane potential
    float V = neuron->getMembranePotential();
    
    // Update gating variables
    pImpl->updateGatingRates(V);
    
    // Calculate ionic currents
    float I_Na = pImpl->sodiumConductanceMax * std::pow(pImpl->m, 3) * pImpl->h * (V - pImpl->sodiumReversalPotential);
    float I_K = pImpl->potassiumConductanceMax * std::pow(pImpl->n, 4) * (V - pImpl->potassiumReversalPotential);
    float I_L = pImpl->membraneLeakConductance * (V - pImpl->membraneLeakReversal);
    
    // Get synaptic input
    float I_syn = neuron->getTotalCurrent();
    
    // Membrane potential dynamics: C*dV/dt = -I_Na - I_K - I_L + I_syn
    float dV_dt = (-I_Na - I_K - I_L + I_syn) / pImpl->membraneCapacitance;
    
    // Update membrane potential (Euler integration)
    float V_new = V + dV_dt * dt_ms;
    
    // Clamp membrane potential
    V_new = std::clamp(V_new, -100.0f, 100.0f);
    neuron->setMembranePotential(V_new);
    
    // Check for spike (threshold-based detection)
    if (neuron->getMembranePotential() >= neuron->getThreshold()) {
        Timestamp spikeTime = neuron->getLastSpikeTime();
        
        // Record spike with actual timing
        neuron->recordSpike(spikeTime);
        
        // Set firing state
        neuron->setFiringState(FiringState::Active);
        
        // Update last spike time
        neuron->getState().lastSpikeTime = spikeTime;
        
        // Reset membrane potential to reset value
        neuron->setMembranePotential(neuron->getState().resetPotential);
        
        // Enter refractory period
        uint32_t refractorySteps = static_cast<uint32_t>(2.0f / dt_ms); // 2ms refractory period
        neuron->setRefractoryPeriod(refractorySteps);
        neuron->decrementRefractory(); // Initialize refractory counter
        
        // Enable plasticity for this neuron
        neuron->enablePlasticity(true, true, true);
    }
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // For HH dynamics, use IntegrateAndFireDynamics for synaptic updates
    IntegrateAndFireDynamics* lif = const_cast<IntegrateAndFireDynamics*>(this);
    lif->updateSynapse(synapse, dt);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // For HH dynamics, use IntegrateAndFireDynamics for synaptic input
    IntegrateAndFireDynamics* lif = const_cast<IntegrateAndFireDynamics*>(this);
    lif->applySpikeInput(neuron, synapse);
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    // For HH, firing is determined by membrane potential exceeding threshold
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH internal state
    delete pImpl;
    pImpl = new Impl();
}

void HodgkinHuxleyDynamics::setSodiumConductanceMax(float g_Na) {
    pImpl->sodiumConductanceMax = g_Na;
}

void HodgkinHuxleyDynamics::setPotassiumConductanceMax(float g_K) {
    pImpl->potassiumConductanceMax = g_K;
}

void HodgkinHuxleyDynamics::setLeakConductance(float g_L) {
    pImpl->membraneLeakConductance = g_L;
}

void HodgkinHuxleyDynamics::setSodiumReversalPotential(float V_Na) {
    pImpl->sodiumReversalPotential = V_Na;
}

void HodgkinHuxleyDynamics::setPotassiumReversalPotential(float V_K) {
    pImpl->potassiumReversalPotential = V_K;
}

void HodgkinHuxleyDynamics::setLeakReversalPotential(float V_L) {
    pImpl->membraneLeakReversal = V_L;
}

void HodgkinHuxleyDynamics::setTemperature(float temp) {
    pImpl->temperature = temp;
    // Recalculate temperature correction
    pImpl->tempCorrection = std::pow(3.0f, (temp - 6.3f) / 10.0f);
}

struct FitzHughNagumoDynamics::Impl {
    // FHN parameters
    float recoveryTimeConstant;       // tau_w
    float threshold;                   // Threshold for excitation
    float resetValue;                  // Reset value for V
    float referencePotential;         // Reference potential for w
    float positiveSlopeFactor;         // Positive slope factor
    
    // State variables
    float V, w;                       // Membrane potential and recovery variable
    
    Impl() : 
        recoveryTimeConstant(12.5f),    // Typical FHN tau
        threshold(0.5f),                 // Threshold for spike initiation
        resetValue(-1.2f),               // Reset after spike
        referencePotential(0.7f),       // Reference potential
        positiveSlopeFactor(0.8f),       // Positive slope factor
        V(-1.2f), w(0.0f)               // Initial state
    {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const NeuronState& state = neuron->getState();
    
    // Convert to milliseconds for easier calculations
    float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Get synaptic input
    float I_syn = neuron->getTotalCurrent();
    
    // FitzHugh-Nagumo equations:
    // dV/dt = (V - V^3/3 - w + I_syn) / tau_V
    // dw/dt = epsilon * (V + a - b*w)
    float V = pImpl->V;
    float w = pImpl->w;
    
    float tau_V = pImpl->recoveryTimeConstant;
    float epsilon = 0.08f; // Time scale separation
    float a = pImpl->referencePotential;
    float b = 0.5f; // Recovery rate
    
    // Update recovery variable
    float dw_dt = epsilon * (V + a - b * w);
    
    // Update membrane potential
    float dV_dt = (V - V*V*V/3.0f - w + I_syn) / tau_V;
    
    // Euler integration
    float V_new = V + dV_dt * dt_ms;
    float w_new = w + dw_dt * dt_ms;
    
    // Update state
    pImpl->V = V_new;
    pImpl->w = w_new;
    
    // Clamp membrane potential
    V_new = std::clamp(V_new, -2.0f, 2.0f);
    neuron->setMembranePotential(V_new);
    
    // Check for spike (threshold-based detection)
    if (neuron->getMembranePotential() >= pImpl->threshold) {
        Timestamp spikeTime = neuron->getLastSpikeTime();
        
        // Record spike with actual timing
        neuron->recordSpike(spikeTime);
        
        // Set firing state
        neuron->setFiringState(FiringState::Active);
        
        // Update last spike time
        neuron->getState().lastSpikeTime = spikeTime;
        
        // Reset membrane potential to reset value
        neuron->setMembranePotential(pImpl->resetValue);
        pImpl->w = pImpl->referencePotential; // Reset recovery variable
        
        // Enter refractory period
        uint32_t refractorySteps = static_cast<uint32_t>(2.0f / dt_ms); // 2ms refractory period
        neuron->setRefractoryPeriod(refractorySteps);
        neuron->decrementRefractory(); // Initialize refractory counter
        
        // Enable plasticity for this neuron
        neuron->enablePlasticity(true, true, true);
    }
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // For FHN dynamics, use IntegrateAndFireDynamics for synaptic updates
    IntegrateAndFireDynamics* lif = const_cast<IntegrateAndFireDynamics*>(this);
    lif->updateSynapse(synapse, dt);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // For FHN dynamics, use IntegrateAndFireDynamics for synaptic input
    IntegrateAndFireDynamics* lif = const_cast<IntegrateAndFireDynamics*>(this);
    lif->applySpikeInput(neuron, synapse);
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    // For FHN, firing is determined by whether V exceeds threshold
    return neuron->getMembranePotential() >= pImpl->threshold;
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN internal state
    delete pImpl;
    pImpl = new Impl();
}

void FitzHughNagumoDynamics::setRecoveryTimeConstant(float tau_w) {
    pImpl->recoveryTimeConstant = tau_w;
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

void FitzHughNagumoDynamics::setResetValue(float resetValue) {
    pImpl->resetValue = resetValue;
}

void FitzHughNagumoDynamics::setReferencePotential(float v_ref) {
    pImpl->referencePotential = v_ref;
}

void FitzHughNagumoDynamics::setPositiveSlopeFactor(float a) {
    pImpl->positiveSlopeFactor = a;
}

} // namespace nlm