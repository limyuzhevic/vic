#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_map>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Enhanced LIF parameters for real neuron dynamics
    float membraneCapacitance;    // Membrane capacitance (nF)
    float membraneResistance;     // Membrane resistance (MΩ)
    float timeConstant;           // Membrane time constant (ms)
    float restingConductance;     // Leak conductance (nS)
    float inputResistance;        // Input resistance (MΩ)
    
    // Adaptation mechanisms
    float adaptationConductance;  // Spike-triggered adaptation conductance
    float adaptationTimeConstant; // Adaptation time constant (ms)
    float adaptationThreshold;    // Adaptation threshold
    
    // Excitability dynamics
    float excitability;           // Neuron excitability (0-1)
    float thresholdAdaptation;    // Threshold adaptation rate
    
    // Input filtering
    float alpha;                  // Low-pass filter coefficient
    float filteredInput;          // Low-pass filtered input
    
    // Refractory dynamics
    float refractoryConductance;  // Conductance during refractory period
    
    // STDP state
    float tracePre;               // Pre-synaptic spike trace
    float tracePost;              // Post-synaptic spike trace
    float stdpTimeConstant;       // STDP trace decay constant (ms)
    
    // Homeostatic plasticity
    float synapticScaling;        // Global synaptic scaling factor
    float homeostaticTarget;      // Target firing rate for homeostasis
    float homeostaticError;       // Current firing rate error
    
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;      // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    static constexpr float ADAPTATION_CONDUCTANCE = 0.5f;     // nS
    static constexpr float ADAPTATION_TIME_CONSTANT = 100.0f;  // ms
    static constexpr float EXCITABILITY_BASE = 0.5f;
    static constexpr float THRESHOLD_ADAPTATION = 0.01f;      // mV per spike
    static constexpr float ALPHA = 0.9f;                       // Low-pass filter
    static constexpr float REFACTORY_CONDUCTANCE = 20.0f;     // nS
    static constexpr float STDP_TIME_CONSTANT = 20.0f;        // ms
    static constexpr float SYNAPTIC_SCALING_RATE = 0.001f;     // Per ms
    static constexpr float HOMEOSTATIC_TARGET_RATE = 5.0f;     // Hz
    
    Impl() : id(), type(NeuronType::Internal), state(), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             membraneCapacitance(MEMBRANE_CAPACITANCE),
             membraneResistance(MEMBRANE_RESISTANCE),
             timeConstant(TIME_CONSTANT),
             restingConductance(state.leakConductance),
             inputResistance(1.0f / restingConductance),
             adaptationConductance(ADAPTATION_CONDUCTANCE),
             adaptationTimeConstant(ADAPTATION_TIME_CONSTANT),
             adaptationThreshold(10.0f),
             excitability(EXCITABILITY_BASE),
             thresholdAdaptation(THRESHOLD_ADAPTATION),
             alpha(ALPHA),
             refractoryConductance(REFACTORY_CONDUCTANCE),
             tracePre(0.0f), tracePost(0.0f), stdpTimeConstant(STDP_TIME_CONSTANT),
             synapticScaling(1.0f), homeostaticTarget(HOMEOSTATIC_TARGET_RATE),
             homeostaticError(0.0f), filteredInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Initialize LIF parameters with biologically plausible values
    pImpl->membraneCapacitance = Impl::MEMBRANE_CAPACITANCE;
    pImpl->membraneResistance = 1.0f / pImpl->state.leakConductance;
    pImpl->timeConstant = Impl::TIME_CONSTANT;
    pImpl->inputResistance = pImpl->membraneResistance;
    pImpl->synapticScaling = 1.0f;
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
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
    pImpl->membraneResistance = 1.0f / conductance;
    pImpl->inputResistance = pImpl->membraneResistance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    float adjustedThreshold = pImpl->state.threshold + 
                              pImpl->state.adaptationVariable * pImpl->thresholdAdaptation;
    return pImpl->state.membranePotential >= adjustedThreshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    pImpl->excitability += amplitude * 0.01f;
    pImpl->excitability = std::clamp(pImpl->excitability, 0.0f, 2.0f);
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->synapticInput += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
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

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period with conductance increase
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, increase leak conductance to clamp voltage
        pImpl->state.leakConductance = pImpl->refractoryConductance;
        // Clear synaptic input during refractory period
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
            pImpl->state.leakConductance = pImpl->restingConductance;
        }
        return false;
    }
    
    // Update homeostatic error based on firing rate
    pImpl->homeostaticError = pImpl->state.firingRate - pImpl->homeostaticTarget;
    
    // Apply synaptic scaling to maintain firing rate stability
    if (fabs(pImpl->homeostaticError) > 0.1f) {
        float scalingFactor = 1.0f - pImpl->homeostaticError * Impl::SYNAPTIC_SCALING_RATE * dt;
        pImpl->synapticScaling *= scalingFactor;
        pImpl->synapticScaling = std::clamp(pImpl->synapticScaling, 0.5f, 2.0f);
    }
    
    // LIF dynamics with enhanced biophysical model
    // dV/dt = (I_leak + I_syn + I_adapt + I_excitability)/C
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float C = pImpl->membraneCapacitance;  // nF
    float g_leak = pImpl->state.leakConductance;  // nS
    
    // Low-pass filter input for temporal smoothing
    pImpl->filteredInput = pImpl->alpha * pImpl->synapticInput + (1.0f - pImpl->alpha) * pImpl->filteredInput;
    
    // Calculate total current contributions
    float I_leak = g_leak * (V_rest - V);  // Ohm's law: I = g * (V_rest - V)
    float I_syn = pImpl->filteredInput * pImpl->synapticScaling;  // Synaptic current scaled
    float I_adapt = pImpl->adaptationConductance * pImpl->state.adaptationVariable;  // Adaptation current
    float I_excitability = (pImpl->excitability - 1.0f) * 5.0f;  // Excitability modulates input resistance
    
    // Total current
    float I_total = I_leak + I_syn + I_adapt + I_excitability;
    
    // Update membrane potential using exponential Euler integration
    float dV = (I_total / C) * 0.001f * dt;  // Convert to mV
    V += dV;
    
    // Apply spike-frequency adaptation (slow hyperpolarization)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f * dt;
        pImpl->state.adaptationVariable *= std::exp(-dt / pImpl->adaptationTimeConstant);
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with adjusted threshold
    float adjustedThreshold = threshold + pImpl->state.adaptationVariable * pImpl->thresholdAdaptation;
    if (V >= adjustedThreshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike for STDP
        recordSpike(currentTime);
        
        // Update firing rate estimate (exponential moving average)
        if (pImpl->state.firingRate > 0.0f) {
            pImpl->state.firingRate = 0.9f * pImpl->state.firingRate + 0.1f * 1000.0f / dt;
        } else {
            pImpl->state.firingRate = 1000.0f / dt;
        }
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period with reduced excitability
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Spike-triggered adaptation
        pImpl->state.adaptationVariable += pImpl->adaptationConductance * dt;
        
        // Reduce excitability after spike ( spike frequency adaptation )
        pImpl->excitability *= 0.95f;
        
        // Homeostatic adjustment
        if (pImpl->state.firingRate > pImpl->homeostaticTarget * 1.5f) {
            pImpl->synapticScaling *= 0.99f;  // Reduce overall excitability
        } else if (pImpl->state.firingRate < pImpl->homeostaticTarget * 0.5f) {
            pImpl->synapticScaling *= 1.01f;  // Increase overall excitability
        }
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Real integrate-and-fire dynamics with 1ms timestep
    TimestepDuration dt = 0.001;  // 1ms timestep
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->filteredInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->adaptationVariable = 0.0f;
    pImpl->excitability = Impl::EXCITABILITY_BASE;
    pImpl->synapticScaling = 1.0f;
    pImpl->homeostaticError = 0.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real biological initialization with constraints
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.firingRate = 0.0f;
    
    // Initialize excitability with biological constraints
    pImpl->excitability = rng.uniformReal(0.3f, 1.5f);
    
    // Initialize synaptic scaling
    pImpl->synapticScaling = 1.0f;
    
    // Initialize low-pass filter state
    pImpl->filteredInput = 0.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->tracePre = 0.0f;
    pImpl->tracePost = 0.0f;
}

} // namespace nlm
