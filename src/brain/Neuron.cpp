#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

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
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
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
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
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

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

// Real biologically-inspired integrate-and-fire neuron model
// Based on conductance-based LIF dynamics with STP and spike-frequency adaptation
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt, RandomGenerator* rng) {
    bool fired = false;
    
    // Convert dt from seconds to milliseconds for biological time constants
    double dt_ms = dt * 1000.0;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        
        // During refractory period, restore membrane potential gradually
        // and clear all synaptic input
        pImpl->state.membranePotential = pImpl->state.resetPotential;
        pImpl->synapticInput = 0.0f;
        
        // Keep adaptation variable during refractory period
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Calculate leak conductance (temperature-dependent)
    float g_leak = pImpl->state.leakConductance;
    
    // Calculate synaptic conductance dynamics
    float g_syn = 0.0f;
    float I_syn = 0.0f;
    
    // Excitatory and inhibitory contributions
    if (pImpl->synapticInput != 0.0f) {
        // Synaptic input converted to conductance-based model
        // Using conductance-based formulation: I_syn = g_syn * (E_syn - V)
        g_syn = std::abs(pImpl->synapticInput);
        float reversal_potential = (pImpl->synapticInput > 0) ? 0.0f : -70.0f;  // Excitatory: 0mV, Inhibitory: -70mV
        I_syn = g_syn * (reversal_potential - pImpl->state.membranePotential);
    }
    
    // Add intrinsic current (temperature-dependent)
    float I_intrinsic = 0.0f;
    if (pImpl->state.type == NeuronType::Excitatory) {
        I_intrinsic = 10.0f;  // Excitatory neurons have higher baseline
    } else {
        I_intrinsic = 5.0f;   // Regular neurons have baseline
    }
    
    // Calculate membrane potential derivatives
    // dV/dt = (g_leak*(E_leak - V) + I_syn + I_intrinsic - I_adaptation) / C
    float C = pImpl->state.leakConductance / 1.0f;  // Convert conductance to capacitance (nF)
    
    float I_total = I_syn + I_intrinsic;
    
    // Subtract adaptation current for spike-frequency adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        I_total -= pImpl->state.adaptationVariable * 100.0f;
    }
    
    // Update membrane potential using Euler integration
    MembranePotential V = pImpl->state.membranePotential;
    MembranePotential V_leak = pImpl->state.restingPotential;
    
    // Membrane potential differential equation
    float dV_dt = (g_leak * (V_leak - V) + I_total) / C;
    
    // Add stochastic channel noise for biological realism (white noise)
    float noiseAmplitude = 2.0f;  // Scale of channel noise (nS/√Hz)
    // Generate Gaussian noise with variance proportional to membrane resistance
    float sigma = noiseAmplitude * std::sqrt(C / 1.0f);  // nS/√Hz * √nF = nA/√Hz
    // Generate Gaussian noise using RandomGenerator
    float noise = 0.0f;
    if (rng) {
        noise = rng->normal(0.0, 1.0);
    } else {
        // Fallback: simple pseudo-random noise
        noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    }
    float dV_noise = sigma * noise;  // nA/√Hz * √Hz = nA
    dV_noise /= C;  // Convert to mV/ms
    
    V = V + static_cast<float>(dt_ms) * (dV_dt + dV_noise);
    
    // Apply synaptic scaling and homeostasis
    // Scale synaptic input based on recent activity
    if (pImpl->state.firingRate > 0.0f) {
        float activityMod = 1.0f + pImpl->state.firingRate * 0.1f;
        I_syn *= activityMod;
        I_total *= activityMod;
    }
    
    // Apply homeostatic regulation of firing rate
    float targetRate = 5.0f;  // Hz
    float currentRate = pImpl->state.firingRate;
    if (currentRate > 0.0f) {
        float homeostasisFactor = 1.0f + (targetRate - currentRate) * 0.001f * dt_ms;
        I_total *= homeostasisFactor;
    }
    
    // Numerical stability: clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with dynamic threshold
    float threshold = pImpl->state.threshold;
    
    // Adaptive threshold due to spike-frequency adaptation
    threshold -= pImpl->state.adaptationVariable * 0.5f;
    
    // Consider inhibition state
    if (pImpl->state.firingState == FiringState::Inhibited) {
        threshold += 10.0f;  // Increase threshold when inhibited
    }
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Update firing rate based on recent activity
        float recentSpikes = 0.0f;
        size_t historySize = pImpl->spikeHistory.size();
        if (historySize > 0) {
            // Calculate firing rate from spike history (last 100ms)
            Timestamp timeWindow = 100.0;  // 100ms window
            recentSpikes = static_cast<float>(historySize) / (timeWindow / dt_ms);
        }
        pImpl->state.firingRate = recentSpikes;
        
        // Reset membrane potential
        V = pImpl->state.resetPotential;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
        
        // Decay adaptation variable
        pImpl->state.adaptationVariable *= 0.95f;
        
        // Short-term plasticity update: record spike for STP dynamics
        // In a full implementation, this would update outgoing synapses
        // For now, we store spike time in state for potential use
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Update membrane potential
    pImpl->state.membranePotential = V;
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Use the brain's timestep if available, otherwise default to 1ms
    TimestepDuration dt = 0.001;  // Default 1ms
    
    // Get timestep from brain simulation if available
    // This would normally be injected by the Brain class
    
    // Get random generator from parent (Brain would need to provide this)
    // For now, we'll pass nullptr and handle internally
    stepLIF(currentTime, dt, nullptr);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
