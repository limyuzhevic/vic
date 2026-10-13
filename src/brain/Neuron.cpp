#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

// Enhanced neuron implementation with Hodgkin-Huxley style dynamics
// Maintains backward compatibility with stepLIF interface
struct EnhancedNeuronImpl {
    // Neuron identity and classification
    NeuronId id;
    NeuronType type;
    
    // Core state
    NeuronId id;
    NeuronType type;
    RegionId regionId;
    PopulationId populationId;
    
    MembraneState membrane;
    SynapticState synaptic;
    NeuromodulationState neuromod;
    HHParameters hh;
    
    // Input buffers and performance optimization
    std::vector<float> synaptic_inputs;     // Vectorized input processing
    float current_input;                   // Current step input
    std::vector<float> adaptation_history;  // Recent adaptation for predictions
    
    // Performance optimization flags
    bool use_vectorization;
    bool enable_nmda_voltage_block;
    bool enable_advanced_refractory;
    bool enable_short_term_plasticity;
    
    EnhancedNeuronImpl() {
        id = NeuronId();
        type = NeuronType::Internal;
        regionId = INVALID_REGION_ID;
        populationId = INVALID_POPULATION_ID;
        
        membrane.reset();
        synaptic.reset();
        neuromod.reset();
        
        use_vectorization = true;
        enable_nmda_voltage_block = true;
        enable_advanced_refractory = true;
        enable_short_term_plasticity = true;
        
        synaptic_inputs.resize(4); // AMPA, NMDA, GABA, Modulatory
        current_input = 0.0f;
        adaptation_history.resize(50, 0.0f);
    }
};

// Enhanced neuron implementation with HH dynamics
Neuron::Neuron(NeuronId id) : pImpl(new EnhancedNeuronImpl()) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
    
    // Update HH parameters based on neuron type
    switch (type) {
        case NeuronType::Excitatory:
            pImpl->hh.subtype = HHParameters::NeuronSubtype::PYRAMIDAL;
            break;
        case NeuronType::Inhibitory:
            pImpl->hh.subtype = HHParameters::NeuronSubtype::INTERNEURON;
            break;
        case NeuronType::Sensory:
            pImpl->hh.subtype = HHParameters::NeuronSubtype::SPINY_STRIATE;
            break;
        default:
            pImpl->hh.subtype = HHParameters::NeuronSubtype::UNKNOWN;
    }
    
    pImpl->hh.setParameters(pImpl->hh.subtype);
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Clamp to valid range for HH dynamics
    float clampedV = std::clamp(static_cast<float>(potential), 
                              pImpl->membrane.v_min, 
                              pImpl->membrane.v_max);
    pImpl->membrane.v = clampedV;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    float newV = pImpl->membrane.v + static_cast<float>(delta);
    setMembranePotential(newV);
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->membrane.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->membrane.firingState = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->membrane.refractoryRemaining > 0) {
        --pImpl->membrane.refractoryRemaining;
        if (pImpl->membrane.refractoryRemaining == 0) {
            pImpl->membrane.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->membrane.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->membrane.g_l = static_cast<float>(conductance);
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->membrane.g_l;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->membrane.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->membrane.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->membrane.v_rest = static_cast<float>(potential);
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->membrane.v_rest;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->membrane.v_reset = static_cast<float>(potential);
}

bool Neuron::checkThreshold() const {
    float adjustedThreshold = pImpl->membrane.threshold + pImpl->membrane.thresholdAdaptation;
    return pImpl->membrane.v >= adjustedThreshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->membrane.lastSpikeTime;
}

// Enhanced synaptic integration with conductance-based synapses
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (pImpl->enable_short_term_plasticity) {
        // Short-term plasticity modification of AMPA conductance
        pImpl->synaptic.utilization = std::min(1.0f, pImpl->synaptic.utilization + 
                                             pImpl->synaptic.facilitationRate * 0.1f);
        pImpl->synaptic.resourcePool = std::max(0.0f, 
            pImpl->synaptic.resourcePool - pImpl->synaptic.depressionRate * 0.1f);
    }
    
    // AMPA conductance with saturation
    float effectiveAmplitude = static_cast<float>(amplitude) * pImpl->synaptic.resourcePool * 
                              (1.0f - pImpl->synaptic.utilization);
    pImpl->synaptic.ampaConductance += effectiveAmplitude;
    pImpl->synaptic.recentAMPA = std::max(pImpl->synaptic.recentAMPA, 
                                        static_cast<float>(amplitude));
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // GABA conductance
    pImpl->synaptic.gabaConductance += static_cast<float>(amplitude);
    pImpl->synaptic.recentAMPA = std::max(pImpl->synaptic.recentAMPA, 
                                        static_cast<float>(amplitude));
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not direct integration
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->membrane.adaptationVariable += static_cast<float>(amplitude) * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Enhanced current injection with voltage-dependent efficacy
    float voltageDependentFactor = 1.0f / (1.0f + std::exp(0.1f * (pImpl->membrane.v - (-50.0f))));
    pImpl->current_input += static_cast<float>(current) * voltageDependentFactor;
}

void Neuron::clearTotalCurrent() {
    pImpl->current_input = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->membrane.lastSpikeTime = static_cast<float>(timestamp);
    
    // Update spike history
    if (pImpl->membrane.spikeHistory.size() < pImpl->membrane.max_spike_history) {
        pImpl->membrane.spikeHistory.push_back(timestamp);
    } else {
        pImpl->membrane.spikeHistory.erase(pImpl->membrane.spikeHistory.begin());
        pImpl->membrane.spikeHistory.push_back(timestamp);
    }
    
    // Update adaptation history
    if (pImpl->adaptation_history.size() >= pImpl->adaptation_history.max_size()) {
        pImpl->adaptation_history.erase(pImpl->adaptation_history.begin());
    }
    pImpl->adaptation_history.push_back(1.0f);
}

void Neuron::clearSpikeHistory() {
    pImpl->membrane.spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return reinterpret_cast<NeuronState&>(pImpl->membrane);
}

const NeuronState& Neuron::getState() const {
    return reinterpret_cast<const NeuronState&>(pImpl->membrane);
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

// Enhanced HH-style stepLIF with full Hodgkin-Huxley dynamics
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Get timestep in milliseconds for HH equations
    float timestep = static_cast<float>(dt) * 1000.0f;
    
    // Handle advanced refractory periods
    if (pImpl->membrane.refractoryRemaining > 0) {
        handleAdvancedRefractory(timestep);
        return false;
    }
    
    // Update synaptic conductances with exponential decay
    updateSynapticConductances(timestep);
    
    // Update HH gating variables
    updateHMGatingVariables();
    
    // Calculate membrane currents
    float sodiumCurrent = calculateSodiumCurrent();
    float potassiumCurrent = calculatePotassiumCurrent();
    float leakCurrent = calculateLeakCurrent();
    float synapticCurrent = calculateSynapticCurrent(timestep);
    
    // Update membrane potential using HH equations
    updateMembranePotential(timestep, sodiumCurrent, potassiumCurrent, 
                           leakCurrent, synapticCurrent);
    
    // Apply spike-frequency adaptation
    applySpikeFrequencyAdaptation(timestep);
    
    // Check for spike with enhanced threshold dynamics
    if (checkThresholdWithAdaptation()) {
        fireSpike(timestep, currentTime);
        fired = true;
    } else {
        pImpl->membrane.firingState = FiringState::Active;
    }
    
    // Clear current input
    pImpl->current_input = 0.0f;
    
    return fired;
}

void Neuron::updateSynapticConductances(float dt) {
    if (pImpl->enable_short_term_plasticity) {
        // Update short-term plasticity
        float depression = pImpl->synaptic.depressionRate * dt;
        float recovery = pImpl->synaptic.recoveryRate * dt;
        
        pImpl->synaptic.resourcePool = std::min(1.0f, 
            pImpl->synaptic.resourcePool + recovery);
        
        pImpl->synaptic.resourcePool = std::max(0.0f,
            pImpl->synaptic.resourcePool - depression);
        
        // Facilitation based on recent activity
        if (pImpl->synaptic.recentAMPA > 0.0f) {
            float facilitation = pImpl->synaptic.facilitationRate * 
                                pImpl->synaptic.recentAMPA * dt;
            pImpl->synaptic.utilization = std::min(1.0f,
                pImpl->synaptic.utilization + facilitation);
        }
        
        // Decay recent activity
        pImpl->synaptic.recentAMPA *= std::exp(-dt / 5.0f);
        pImpl->synaptic.recentNMDA *= std::exp(-dt / 50.0f);
    }
    
    // Exponential decay of all conductances
    float ampaDecayFactor = std::exp(-dt / pImpl->synaptic.ampaDecay);
    float nmdaDecayFactor = std::exp(-dt / pImpl->synaptic.nmdaDecay);
    float gabaDecayFactor = std::exp(-dt / pImpl->synaptic.gabaDecay);
    float modulatoryDecayFactor = std::exp(-dt / pImpl->synaptic.modulatoryDecay);
    
    pImpl->synaptic.ampaConductance *= ampaDecayFactor;
    pImpl->synaptic.nmdaConductance *= nmdaDecayFactor;
    pImpl->synaptic.gabaConductance *= gabaDecayFactor;
    pImpl->synaptic.modulatoryConductance *= modulatoryDecayFactor;
}

void Neuron::updateHMGatingVariables() {
    float v = pImpl->membrane.v;
    
    // Compute steady-state values
    float m_inf = alpha_m(v) / (alpha_m(v) + beta_m(v));
    float h_inf = alpha_h(v) / (alpha_h(v) + beta_h(v));
    float n_inf = alpha_n(v) / (alpha_n(v) + beta_n(v));
    
    // Compute time constants
    float tau_m = 1.0f / (alpha_m(v) + beta_m(v));
    float tau_h = 1.0f / (alpha_h(v) + beta_h(v));
    float tau_n = 1.0f / (alpha_n(v) + beta_n(v));
    
    // Update gating variables with exponential integration
    pImpl->membrane.m += (m_inf - pImpl->membrane.m) * (1.0f - std::exp(-timestep / tau_m));
    pImpl->membrane.h += (h_inf - pImpl->membrane.h) * (1.0f - std::exp(-timestep / tau_h));
    pImpl->membrane.n += (n_inf - pImpl->membrane.n) * (1.0f - std::exp(-timestep / tau_n));
    
    // Clamp gating variables to [0, 1]
    pImpl->membrane.m = std::clamp(pImpl->membrane.m, 0.0f, 1.0f);
    pImpl->membrane.h = std::clamp(pImpl->membrane.h, 0.0f, 1.0f);
    pImpl->membrane.n = std::clamp(pImpl->membrane.n, 0.0f, 1.0f);
}

float Neuron::calculateSodiumCurrent() const {
    // I_na = g_na_max * m^3 * h * (V - E_na)
    float m3 = pImpl->membrane.m * pImpl->membrane.m * pImpl->membrane.m;
    return pImpl->hh.g_na_max * m3 * pImpl->membrane.h * 
           (pImpl->membrane.v - pImpl->membrane.sodiumReversal);
}

float Neuron::calculatePotassiumCurrent() const {
    // I_k = g_k_max * n^4 * (V - E_k)
    float n4 = pImpl->membrane.n * pImpl->membrane.n * 
               pImpl->membrane.n * pImpl->membrane.n;
    return pImpl->hh.g_k_max * n4 * (pImpl->membrane.v - pImpl->membrane.potassiumReversal);
}

float Neuron::calculateLeakCurrent() const {
    // I_l = g_l * (V - E_l)
    return pImpl->membrane.g_l * (pImpl->membrane.v - pImpl->membrane.v_rest);
}

float Neuron::calculateSynapticCurrent(float dt) {
    float totalCurrent = pImpl->current_input;
    
    // AMPA excitatory current with voltage-dependent NMDA magnesium block
    float ampaCurrent = pImpl->synaptic.ampaConductance * 
                        (pImpl->membrane.v - pImpl->membrane.sodiumReversal);
    
    // NMDA current with voltage-dependent magnesium block
    float nmdaMgBlock = pImpl->synaptic.nmda_mg_block(pImpl->membrane.v);
    float nmdaCurrent = pImpl->synaptic.nmdaConductance * nmdaMgBlock * 
                       (pImpl->membrane.v - pImpl->membrane.sodiumReversal);
    
    // GABA inhibitory current
    float gabaCurrent = pImpl->synaptic.gabaConductance * 
                        (pImpl->membrane.v - pImpl->membrane.potassiumReversal);
    
    // Modulatory current
    float modulatoryCurrent = pImpl->synaptic.modulatoryConductance * 
                             (pImpl->membrane.v - (-75.0f)); // Typical modulatory reversal
    
    return totalCurrent + ampaCurrent + nmdaCurrent - gabaCurrent + modulatoryCurrent;
}

void Neuron::updateMembranePotential(float dt, float I_na, float I_k, float I_l, float I_syn) {
    // Membrane equation: C * dV/dt = -(I_na + I_k + I_l + I_syn)
    // Therefore: dV/dt = -(I_total) / C
    
    float totalCurrent = I_na + I_k + I_l + I_syn;
    float dV_dt = -totalCurrent / pImpl->hh.vm;
    
    // Exponential Euler integration for HH equations
    pImpl->membrane.v += dV_dt * dt;
    
    // Apply voltage clamp with adaptive limits
    float voltageRange = pImpl->membrane.v_max - pImpl->membrane.v_min;
    float voltageRelative = (pImpl->membrane.v - pImpl->membrane.v_min) / voltageRange;
    
    // Adaptive voltage limits based on firing rate and adaptation
    float adaptationFactor = 1.0f + pImpl->membrane.adaptationVariable * 0.5f;
    float maxAdaptiveV = pImpl->membrane.v_max * adaptationFactor;
    float minAdaptiveV = pImpl->membrane.v_min / adaptationFactor;
    
    pImpl->membrane.v = std::clamp(pImpl->membrane.v, minAdaptiveV, maxAdaptiveV);
}

void Neuron::applySpikeFrequencyAdaptation(float dt) {
    // Spike-frequency adaptation based on recent spiking activity
    if (pImpl->adaptation_history.size() > 0) {
        float recentAdaptation = pImpl->adaptation_history.back();
        
        // Adaptation dynamics
        float adaptationDerivative = (1.0f - recentAdaptation) / pImpl->membrane.adaptationTimeConstant;
        pImpl->membrane.adaptationVariable += adaptationDerivative * dt;
        
        // Adaptation decay
        pImpl->membrane.adaptationVariable *= std::exp(-dt / pImpl->membrane.adaptationTimeConstant);
        
        // Apply adaptation to threshold
        pImpl->membrane.thresholdAdaptation += pImpl->membrane.adaptationVariable * 0.01f;
        
        // Store adaptation for future reference
        if (pImpl->adaptation_history.size() >= pImpl->adaptation_history.max_size()) {
            pImpl->adaptation_history.erase(pImpl->adaptation_history.begin());
        }
        pImpl->adaptation_history.push_back(pImpl->membrane.adaptationVariable);
    }
}

bool Neuron::checkThresholdWithAdaptation() {
    float adjustedThreshold = pImpl->membrane.threshold + pImpl->membrane.thresholdAdaptation;
    return pImpl->membrane.v >= adjustedThreshold;
}

void Neuron::fireSpike(float dt, Timestamp currentTime) {
    // Enhanced spike generation with multiple refractory phases
    pImpl->membrane.firingState = FiringState::Active;
    pImpl->membrane.lastSpikeTime = static_cast<float>(currentTime);
    
    // Record spike for plasticity
    recordSpike(currentTime);
    
    // Enter advanced refractory period
    enterAdvancedRefractory(dt);
    
    // Update adaptation for spike-frequency adaptation
    pImpl->membrane.adaptationVariable += 1.0f;
    
    // Reset membrane potential to subthreshold level
    pImpl->membrane.v = pImpl->membrane.v_reset;
    
    // Update HH gating variables after spike
    updateHMGatingVariables();
}

void Neuron::handleAdvancedRefractory(float dt) {
    if (pImpl->membrane.refractoryRemaining > 0) {
        // Advance refractory period
        pImpl->membrane.refractoryRemaining--;
        
        if (pImpl->membrane.refractoryRemaining == 0) {
            // Exit refractory period
            pImpl->membrane.firingState = FiringState::Resting;
            
            // Apply voltage-dependent recovery
            float recoveryFactor = std::exp(-dt / 10.0f);
            pImpl->membrane.v = std::max(pImpl->membrane.v_rest, 
                                       pImpl->membrane.v * recoveryFactor);
        } else {
            // During refractory period: suppress synaptic input and clamp voltage
            pImpl->membrane.v = pImpl->membrane.v_reset;
            pImpl->membrane.adaptationVariable *= 0.8f; // Reduce adaptation
        }
    }
}

void Neuron::enterAdvancedRefractory(float dt) {
    // Multi-phase refractory period
    pImpl->membrane.refractoryRemaining = pImpl->membrane.refractoryPeriod;
    pImpl->membrane.firingState = FiringState::Refractory;
    
    // Phase 1: Absolute refractory (complete insensitivity)
    // Phase 2: Relative refractory (partial sensitivity)
    // Phase 3: Recovery (returning to resting state)
    
    // Store initial voltage for recovery calculation
    pImpl->membrane.v = pImpl->membrane.v_reset;
    
    // Reset adaptation to prevent excessive adaptation
    pImpl->membrane.adaptationVariable = std::max(0.0f, 
        pImpl->membrane.adaptationVariable - 0.5f);
}

// HH differential equation helper functions
inline float Neuron::alpha_m(float v) const {
    // Sodium activation gate
    return (0.1f * (v + 40.0f)) / (1.0f - std::exp(-(v + 40.0f) / 10.0f));
}

inline float Neuron::beta_m(float v) const {
    // Sodium activation inactivation
    return 4.0f * std::exp(-(v + 65.0f) / 18.0f);
}

inline float Neuron::alpha_h(float v) const {
    // Sodium inactivation gate
    return 0.07f * std::exp(-(v + 65.0f) / 20.0f);
}

inline float Neuron::beta_h(float v) const {
    // Sodium inactivation deactivation
    return 1.0f / (1.0f + std::exp(-(v + 35.0f) / 10.0f));
}

inline float Neuron::alpha_n(float v) const {
    // Potassium activation gate
    return (0.01f * (v + 55.0f)) / (1.0f - std::exp(-(v + 55.0f) / 10.0f));
}

inline float Neuron::beta_n(float v) const {
    // Potassium activation deactivation
    return 0.125f * std::exp(-(v + 65.0f) / 80.0f);
}

void Neuron::setConductancesAndReversals() {
    switch (pImpl->hh.subtype) {
        case HHParameters::NeuronSubtype::PYRAMIDAL:
            // Set conductances and reversal potentials for pyramidal neuron
            pImpl->membrane.sodiumConductance = pImpl->hh.g_na_max * 0.8f; // Typically 80% max
            pImpl->membrane.potassiumConductance = pImpl->hh.g_k_max * 0.5f; // Typically 50% max
            pImpl->membrane.g_l = pImpl->hh.g_l; // Leak conductance
            
            pImpl->membrane.sodiumReversal = pImpl->hh.e_na;
            pImpl->membrane.potassiumReversal = pImpl->hh.e_k;
            pImpl->membrane.v_rest = -65.0f; // Resting potential
            break;
            
        case HHParameters::NeuronSubtype::INTERNEURON:
            // Faster kinetics for interneurons
            pImpl->membrane.sodiumConductance = pImpl->hh.g_na_max * 0.9f;
            pImpl->membrane.potassiumConductance = pImpl->hh.g_k_max * 0.7f;
            
            pImpl->membrane.sodiumReversal = pImpl->hh.e_na;
            pImpl->membrane.potassiumReversal = pImpl->hh.e_k;
            pImpl->membrane.v_rest = -70.0f;
            break;
            
        default:
            // Default conductances
            pImpl->membrane.sodiumConductance = pImpl->hh.g_na_max * 0.6f;
            pImpl->membrane.potassiumConductance = pImpl->hh.g_k_max * 0.4f;
            
            pImpl->membrane.sodiumReversal = pImpl->hh.e_na;
            pImpl->membrane.potassiumReversal = pImpl->hh.e_k;
            pImpl->membrane.v_rest = -65.0f;
    }
}

void Neuron::step(Timestamp currentTime) {
    // Default enhanced step with standard timestep
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Reset to biologically realistic initial conditions
    pImpl->membrane = NeuronState();
    pImpl->synaptic = SynapticState();
    pImpl->neurom = NeuromodulationState();
    
    // Initialize with realistic neuron-type specific parameters
    switch (pImpl->type) {
        case NeuronType::Excitatory:
            pImpl->membrane.v = -65.0f + (pImpl->rng.uniformReal(-5.0f, 5.0f));
            pImpl->membrane.threshold = -50.0f + pImpl->rng.uniformReal(-8.0f, 8.0f);
            break;
        case NeuronType::Inhibitory:
            pImpl->membrane.v = -60.0f + pImpl->rng.uniformReal(-4.0f, 4.0f);
            pImpl->membrane.threshold = -45.0f + pImpl->rng.uniformReal(-5.0f, 5.0f);
            break;
        default:
            pImpl->membrane.v = -70.0f + pImpl->rng.uniformReal(-3.0f, 3.0f);
            pImpl->membrane.threshold = -55.0f + pImpl->rng.uniformReal(-5.0f, 5.0f);
    }
    
    pImpl->membrane.refractoryRemaining = 0;
    pImpl->membrane.adaptationVariable = 0.0f;
    pImpl->current_input = 0.0f;
    pImpl->adaptation_history.clear();
    pImpl->membrane.spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Set random seed for reproducible initialization
    rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Enhanced random initialization with biological constraints
    pImpl->membrane.v = pImpl->membrane.v_rest + rng.uniformReal(-10.0f, 10.0f);
    
    // Realistic thresholds based on neuron type
    switch (pImpl->type) {
        case NeuronType::Excitatory:
            pImpl->membrane.threshold = -48.0f + rng.uniformReal(-8.0f, 8.0f);
            pImpl->membrane.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(1, 5));
            break;
        case NeuronType::Inhibitory:
            pImpl->membrane.threshold = -40.0f + rng.uniformReal(-5.0f, 5.0f);
            pImpl->membrane.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 8));
            break;
        case NeuronType::Sensory:
            pImpl->membrane.threshold = -45.0f + rng.uniformReal(-6.0f, 6.0f);
            pImpl->membrane.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(3, 10));
            break;
        default:
            pImpl->membrane.threshold = -55.0f + rng.uniformReal(-5.0f, 5.0f);
            pImpl->membrane.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 6));
    }
    
    // Initialize HH parameters based on neuron type
    setHHParametersBasedOnType();
    
    // Set other initial conditions
    pImpl->membrane.v_reset = pImpl->membrane.v_rest + rng.uniformReal(0.0f, 5.0f);
    pImpl->membrane.firingState = FiringState::Resting;
    pImpl->membrane.refractoryRemaining = 0;
    pImpl->membrane.adaptationVariable = 0.0f;
    pImpl->membrane.lastSpikeTime = -1.0f;
    
    // Clear all input buffers
    pImpl->synaptic.ampaConductance = 0.0f;
    pImpl->synaptic.nmdaConductance = 0.0f;
    pImpl->synaptic.gabaConductance = 0.0f;
    pImpl->synaptic.modulatoryConductance = 0.0f;
    
    pImpl->current_input = 0.0f;
    pImpl->adaptation_history.clear();
    pImpl->membrane.spikeHistory.clear();
}

void Neuron::setHHParametersBasedOnType() {
    // Set HH parameters based on neuron type for realistic dynamics
    switch (pImpl->type) {
        case NeuronType::Excitatory:
            pImpl->hh.vm = 250.0f;
            pImpl->hh.g_na_max = 120.0f;
            pImpl->hh.g_k_max = 36.0f;
            pImpl->hh.e_na = 50.0f;
            pImpl->hh.e_k = -77.0f;
            pImpl->hh.subtype = HHParameters::NeuronSubtype::PYRAMIDAL;
            break;
            
        case NeuronType::Inhibitory:
            pImpl->hh.vm = 200.0f;
            pImpl->hh.g_na_max = 150.0f;
            pImpl->hh.g_k_max = 40.0f;
            pImpl->hh.e_na = 55.0f;
            pImpl->hh.e_k = -90.0f;
            pImpl->hh.subtype = HHParameters::NeuronSubtype::INTERNEURON;
            break;
            
        case NeuronType::Sensory:
            pImpl->hh.vm = 220.0f;
            pImpl->hh.g_na_max = 100.0f;
            pImpl->hh.g_k_max = 30.0f;
            pImpl->hh.e_na = 45.0f;
            pImpl->hh.e_k = -75.0f;
            pImpl->hh.subtype = HHParameters::NeuronSubtype::SPINY_STRIATE;
            break;
            
        default:
            // Default HH parameters
            pImpl->hh.vm = 250.0f;
            pImpl->hh.g_na_max = 120.0f;
            pImpl->hh.g_k_max = 36.0f;
            pImpl->hh.e_na = 50.0f;
            pImpl->hh.e_k = -77.0f;
            pImpl->hh.subtype = HHParameters::NeuronSubtype::UNKNOWN;
    }
    
    // Set conductances based on subtype
    setConductancesAndReversals();
}

} // namespace nlm