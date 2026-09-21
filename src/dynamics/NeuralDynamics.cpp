#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>

namespace nlm {

struct IonChannel {
    float conductance;
    float equilibriumPotential;
    float inactivationVariable;
    float activationVariable;
    float timeConstant;
    float relativeConductance;
    
    IonChannel() : conductance(0.0f), equilibriumPotential(0.0f), 
                   inactivationVariable(1.0f), activationVariable(0.0f),
                   timeConstant(1.0f), relativeConductance(1.0f) {}
};

struct IntegrateAndFireDynamics::Impl {
    // Enhanced LIF neuron parameters
    float membraneTimeConstant;           // ms
    float membraneResistance;              // MOhm
    float membraneCapacitance;             // nF
    
    // Ion channel dynamics
    float sodiumMaxConductance;            // mS/cm²
    float potassiumMaxConductance;         // mS/cm²
    float leakConductance;                 // mS/cm²
    float sodiumReversalPotential;         // mV
    float potassiumReversalPotential;      // mV
    
    // Adaptive threshold dynamics
    float thresholdBaseline;               // mV
    float thresholdAdaptationRate;         // mV/ms
    float thresholdHistorySize;            // Number of spikes to track
    
    // Spike frequency adaptation
    float adaptationTimeConstant;          // ms
    float maxAdaptationCurrent;            // nA
    
    // Neuromodulation
    float dopamineLevel;                   // Neuromodulation level (0-1)
    float serotoninLevel;                  // Neuromodulation level (0-1)
    float acetylcholineLevel;               // Neuromodulation level (0-1)
    
    // Synaptic dynamics
    float synapticTimeConstant;            // ms
    float synapticDepressionFactor;        // Utilization parameter
    float synapticFacilitationTimeConstant; // ms
    
    // Performance optimizations
    bool useVectorized;                   // Enable SIMD
    bool usePrecisionMode;                 // High precision mode
    
    // Instance-specific state
    IonChannel sodiumChannel;
    IonChannel potassiumChannel;
    IonChannel leakChannel;
    
    // Simulation state
    float lastSpikeTime;                  // ms - time of last spike
    float adaptation;                      // mV - adaptation level
    float noiseAmplitude;                  // mV - input noise amplitude
    
    // Calcium dynamics for plasticity
    float calciumConcentration;           // uM - intracellular calcium
    float internalCalcium;                 // uM - internal calcium state
    
    // Spike tracking
    std::vector<float> spikeHistory;      // Track recent spike times
    size_t spikeHistorySize;               // Maximum history size
    
    // Threshold and reset potentials
    float thresholdV;                      // mV - firing threshold
    float resetV;                          // mV - post-spike reset potential
    
    // Refractory period
    float refractoryPeriod;                // ms - refractory period after spike
    
    // Adaptation threshold
    float adaptationThreshold;             // mV - maximum adaptation
    
    Impl() : 
        membraneTimeConstant(20.0f),
        membraneResistance(10.0f),
        membraneCapacitance(1.0f),
        sodiumMaxConductance(120.0f),
        potassiumMaxConductance(36.0f),
        leakConductance(0.3f),
        sodiumReversalPotential(50.0f),
        potassiumReversalPotential(-77.0f),
        thresholdBaseline(-55.0f),
        thresholdAdaptationRate(0.1f),
        thresholdHistorySize(100),
        adaptationTimeConstant(100.0f),
        maxAdaptationCurrent(5.0f),
        dopamineLevel(0.0f),
        serotoninLevel(0.0f),
        acetylcholineLevel(0.0f),
        synapticTimeConstant(5.0f),
        synapticDepressionFactor(0.15f),
        synapticFacilitationTimeConstant(50.0f),
        useVectorized(true),
        usePrecisionMode(false),
        lastSpikeTime(-1000.0f),
        adaptation(0.0f),
        noiseAmplitude(0.5f),
        calciumConcentration(0.1f),
        internalCalcium(0.0f),
        spikeHistorySize(1000),
        thresholdV(-55.0f),
        resetV(-70.0f),
        refractoryPeriod(2.0f),
        adaptationThreshold(25.0f) {
        
        // Initialize spike history
        spikeHistory.reserve(spikeHistorySize);
        
        // Initialize ion channel parameters
        sodiumChannel.conductance = sodiumMaxConductance;
        sodiumChannel.equilibriumPotential = sodiumReversalPotential;
        sodiumChannel.timeConstant = 0.1f;
        
        potassiumChannel.conductance = potassiumMaxConductance;
        potassiumChannel.equilibriumPotential = potassiumReversalPotential;
        potassiumChannel.timeConstant = 1.0f;
        
        leakChannel.conductance = leakConductance;
        leakChannel.equilibriumPotential = -54.4f;
        leakChannel.timeConstant = 10.0f;
    }
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I_input = neuron->getTotalCurrent();
    float currentTime = state.lastSpikeTime; // Use this as approximation for time
    
    // Check if neuron is refractory before processing
    if (neuron->isRefractory()) {
        // During refractory period, just reset and return
        neuron->setMembranePotential(pImpl->resetV);
        neuron->decrementRefractory();
        neuron->clearTotalCurrent();
        return;
    }
    
    // Generate biological noise
    static std::mt19937 rng(42);
    static std::normal_distribution<float> noise(0.0f, pImpl->noiseAmplitude);
    float noise_input = noise(rng);
    
    // Update ion channel states with differential equations
    updateIonChannels(currentTime, dt, V, pImpl->internalCalcium);
    
    // Calculate total synaptic current (conductance-based model)
    float I_syn = calculateSynapticCurrent(V, V_rest);
    
    // Update membrane potential using Hodgkin-Huxley style equations
    float dVdt = calculateMembranePotentialChange(V, V_rest, I_input, I_syn, dt);
    
    // Integrate using Euler method
    V += dVdt * static_cast<float>(dt);
    neuron->setMembranePotential(V);
    
    // Update adaptation current based on recent spiking
    updateAdaptationCurrent(currentTime, V);
    
    // Apply adaptation to membrane potential
    if (pImpl->adaptation > 0.0f) {
        V -= pImpl->adaptation * 0.1f;  // Adaptation current effect
        neuron->setMembranePotential(V);
    }
    
    // Check for spike generation with precise timing
    if (V >= pImpl->thresholdV && !neuron->isRefractory()) {
        // Generate spike with precise timing
        float spikeTime = currentTime;
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(spikeTime);
        
        // Update internal state
        pImpl->lastSpikeTime = spikeTime;
        pImpl->spikeHistory.push_back(spikeTime);
        if (pImpl->spikeHistory.size() > pImpl->spikeHistorySize) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        // Trigger immediate reset
        neuron->setMembranePotential(pImpl->resetV);
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod * 1000.0f));  // Convert ms to steps
        
        // Start adaptation
        pImpl->adaptation = pImpl->adaptationThreshold;
        
        // Log spike generation
        NLM_LOG_DEBUG("Neuron " + std::to_string(neuron->getId().index()) + 
                     " spiked at time " + std::to_string(spikeTime) +
                     " (V=" + std::to_string(V) + "mV)");
    }
    
    // Update calcium dynamics for plasticity
    updateCalciumDynamics(currentTime, V);
    
    // Apply neuromodulation effects
    applyNeuromodulationEffects(V, dt);
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    float weight = synapse->getWeight();
    
    // Apply synaptic dynamics based on synapse type and plasticity flags
    // Simple exponential decay with noise for now
    float timeConstant = pImpl->synapticTimeConstant;
    float filteredWeight = weight * exp(-static_cast<float>(dt) / timeConstant);
    
    // Add stochastic element for facilitation/depression
    static std::mt19937 rng(42);
    static std::uniform_real_distribution<float> uniform(0.9f, 1.1f);
    float stochasticFactor = uniform(rng);
    
    float newWeight = filteredWeight * stochasticFactor;
    
    // Check for synaptic plasticity triggers
    if (synapse->getPlasticityFlags().stdp || synapse->getPlasticityFlags().hebbian) {
        // Plasticity will be applied by the calling Brain::step method
        // Just record that this synapse is plastic
    }
    
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Conductance-based synaptic transmission
    float weight = synapse->getWeight();
    float reversalPotential = synapse->isExcitatory() ? 50.0f : -70.0f;
    float conductance = std::abs(weight) * 0.01f;  // Scale conductance
    
    // Calculate synaptic current using Ohm's law: I = g * (V - E)
    float V = neuron->getMembranePotential();
    float I_syn = conductance * (V - reversalPotential);
    
    // Apply synaptic current
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(std::abs(I_syn));
    } else {
        neuron->receiveInhibitoryInput(std::abs(I_syn));
    }
    
    // Record synaptic input for plasticity
    synapse->recordPostSpike(static_cast<float>(0.0));  // Time will be set by caller
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= pImpl->thresholdV && !neuron->isRefractory();
}

void IntegrateAndFireDynamics::reset() {
    pImpl->adaptation = 0.0f;
    pImpl->calciumConcentration = 0.0f;
    pImpl->internalCalcium = 0.0f;
    pImpl->spikeHistory.clear();
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void IntegrateAndFireDynamics::updateIonChannels(float currentTime, TimestepDuration dt, float V, float calcium) {
    // Update sodium channel activation variable (m)
    float m = 0.1f * (V + 40.0f) / (1.0f - exp(-0.1f * (V + 40.0f)));
    float dm_dt = (m - pImpl->sodiumChannel.activationVariable) / pImpl->sodiumChannel.timeConstant;
    pImpl->sodiumChannel.activationVariable += dm_dt * static_cast<float>(dt);
    
    // Update potassium channel activation variable (n)
    float n = 0.01f * (V + 60.0f) / (1.0f - exp(-0.1f * (V + 60.0f)));
    float dn_dt = (n - pImpl->potassiumChannel.activationVariable) / pImpl->potassiumChannel.timeConstant;
    pImpl->potassiumChannel.activationVariable += dn_dt * static_cast<float>(dt);
    
    // Update inactivation variable (h)
    float h = 0.07f * exp(-0.017f * (V + 60.0f));
    float dh_dt = (h - pImpl->sodiumChannel.inactivationVariable) / pImpl->sodiumChannel.timeConstant;
    pImpl->sodiumChannel.inactivationVariable += dh_dt * static_cast<float>(dt);
}

float IntegrateAndFireDynamics::calculateSynapticCurrent(float V, float V_rest) const {
    // Calculate total synaptic current from all active synapses
    // Using conductance-based formulation: I_syn = sum(g * (V - E))
    
    float totalI_syn = 0.0f;
    
    // Calculate synaptic conductance based on last spike timing
    // G_syn = G_max * exp(-(t - t_last_spike)/tau_syn)
    float g_syn = pImpl->synapticDepressionFactor;  // Base conductance
    
    // Add facilitation based on recent pre-synaptic activity
    if (pImpl->lastSpikeTime >= 0.0f) {
        float timeSinceLastSpike = 1000.0f - pImpl->lastSpikeTime;  // Convert ms to seconds
        g_syn *= exp(-timeSinceLastSpike / pImpl->synapticFacilitationTimeConstant);
    }
    
    // Reversal potentials
    const float E_exc = 50.0f;    // Excitatory reversal potential
    const float E_inh = -70.0f;   // Inhibitory reversal potential
    
    // Calculate total synaptic current (excitatory component)
    totalI_syn += g_syn * (V - E_exc);
    
    // Add adaptation current (spike-frequency adaptation)
    if (pImpl->adaptation > 0.0f) {
        totalI_syn += pImpl->adaptation * 0.1f;  // Adaptation current contribution
    }
    
    // Apply neuromodulation scaling factors
    totalI_syn *= pImpl->dopamineLevel * pImpl->acetylcholineLevel;
    
    return totalI_syn;
}

float IntegrateAndFireDynamics::calculateMembranePotentialChange(float V, float V_rest, 
                                                                float I_input, float I_syn, 
                                                                TimestepDuration dt) const {
    // Membrane equation: Cm * dV/dt = I_leak + I_input + I_syn + I_ion
    
    // Calculate total synaptic current using the separate function
    float I_total = I_input + I_syn;
    
    // Ionic currents (simplified Hodgkin-Huxley with conductance-based model)
    // Sodium current
    float I_Na = pImpl->sodiumChannel.conductance * 
                 std::pow(pImpl->sodiumChannel.activationVariable, 3) * 
                 pImpl->sodiumChannel.inactivationVariable * 
                 (V - pImpl->sodiumChannel.equilibriumPotential);
    
    // Potassium current  
    float I_K = pImpl->potassiumChannel.conductance * 
                std::pow(pImpl->potassiumChannel.activationVariable, 4) * 
                (V - pImpl->potassiumChannel.equilibriumPotential);
    
    // Leak current
    float I_L = pImpl->leakChannel.conductance * (V - pImpl->leakChannel.equilibriumPotential);
    
    // Calcium dynamics (for plasticity)
    float I_Ca = pImpl->calciumConcentration * (V - 50.0f);
    
    // Total current including ion currents
    I_total += I_Na + I_K + I_L + I_Ca;
    
    // Membrane potential change: dV/dt = I_total / Cm
    return I_total / pImpl->capacitance;
}

void IntegrateAndFireDynamics::updateAdaptationCurrent(float currentTime, float V) {
    // Spike-activated potassium current adaptation
    float timeSinceLastSpike = currentTime - pImpl->lastSpikeTime;
    
    if (timeSinceLastSpike < 100.0f) {  // Recent spike
        pImpl->adaptationCurrent = 20.0f * exp(-timeSinceLastSpike / 50.0f);
    } else {
        pImpl->adaptationCurrent = 0.0f;
    }
    
    // Update adaptation level
    if (pImpl->adaptationCurrent > 0.0f) {
        pImpl->adaptation = std::min(pImpl->adaptationThreshold, 
                                   pImpl->adaptation + pImpl->adaptationCurrent * 0.01f);
    } else {
        pImpl->adaptation *= 0.95f;  // Decay adaptation
    }
}

void IntegrateAndFireDynamics::updateCalciumDynamics(float currentTime, float V) {
    // Intracellular calcium dynamics
    float dCa_dt = -0.1f * pImpl->calciumConcentration;  // Decay
    
    // Calcium influx through voltage-gated channels
    if (V > 0.0f) {
        dCa_dt += 0.05f * (V - 0.0f);  // Spike-dependent calcium influx
    }
    
    // Update calcium concentration
    pImpl->calciumConcentration += dCa_dt * static_cast<float>(1.0f / 1000.0f);  // ms^-1
    
    // Store internal calcium
    pImpl->internalCalcium = pImpl->calciumConcentration;
}

void IntegrateAndFireDynamics::applyNeuromodulationEffects(float V, TimestepDuration dt) {
    // Apply neuromodulator effects on neuron excitability
    
    // Dopamine effects on excitability
    if (pImpl->dopamineLevel > 0.0f) {
        // Positive dopamine increases excitability (lowers threshold)
        float thresholdAdjustment = pImpl->dopamineLevel * 5.0f;  // mV
        pImpl->thresholdV = 30.0f - thresholdAdjustment;
    }
    
    // Serotonin effects on bursting
    if (pImpl->serotoninLevel > 0.5f) {
        // High serotonin promotes burst firing
        if (V > -50.0f && V < -30.0f) {
            // Enhance firing probability in this range
            // This would be applied as a probability multiplier
        }
    }
    
    // Time-dependent recovery of thresholds
    pImpl->thresholdV = std::min(30.0f, pImpl->thresholdV + 0.1f * static_cast<float>(dt));
}

} // namespace nlm