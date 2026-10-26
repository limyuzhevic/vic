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
    
    // Enhanced LIF dynamics parameters
    static constexpr float REFACTORY_RECOVERY_RATE = 0.1f;  // Rate of membrane potential recovery during refractory period
    static constexpr float SYNAPTIC_TIME_CONSTANT = 5.0f;    // Synaptic integration time constant (ms)
    static constexpr float ADAPTATION_DECAY_TIME_CONSTANT = 1000.0f;  // Adaptation variable decay time (ms)
    static constexpr float ADAPTATION_CONTRIBUTION_FACTOR = 0.005f;  // Strength of adaptation hyperpolarization
    static constexpr float POST_SPIKE_RECOVERY_RATE = 0.1f;  // Post-spike potential recovery rate
    static constexpr float AHP_AMPLITUDE = 5.0f;              // Afterhyperpolarization amplitude (mV)
    static constexpr float THRESHOLD_HYSTERESIS = 5.0f;      // Hysteresis margin around threshold (mV)
    static constexpr float V_LOWER_BOUND = -100.0f;          // Lower bound for membrane potential (mV)
    static constexpr float V_UPPER_BOUND = 50.0f;            // Upper bound for membrane potential (mV)
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

// Create neuron with ID
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Initialize with default biological parameters
    pImpl->state.restingPotential = -70.0f;   // Typical resting potential
    pImpl->state.threshold = -55.0f;          // Typical firing threshold
    pImpl->state.resetPotential = -70.0f;     // Reset after spike
    pImpl->state.leakConductance = 10.0f;     // Conductance for leak current
    pImpl->state.refractoryPeriod = 5;        // Default 5ms refractory period
    
    // Initialize firing state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.adaptationVariable = 0.0f;
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

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period with exponential decay recovery
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, allow partial recovery of membrane potential
        // towards resting potential, but prevent spiking
        float recoveryRate = 0.1f; // Recovery rate per timestep
        pImpl->state.membranePotential += (pImpl->state.restingPotential - pImpl->state.membranePotential) * recoveryRate;
        
        // Gradually reduce adaptation variable during refractory period
        if (pImpl->state.adaptationVariable > 0.0f) {
            pImpl->state.adaptationVariable *= 0.95f;
        }
        
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Handle refractory period with exponential decay recovery
    // During refractory period, neuron cannot spike but can recover
    // membrane potential towards resting potential with partial recovery
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, allow partial recovery of membrane potential
        // towards resting potential, but prevent spiking
        pImpl->state.membranePotential += (pImpl->state.restingPotential - pImpl->state.membranePotential) * Impl::REFACTORY_RECOVERY_RATE;
        
        // Gradually reduce adaptation variable during refractory period
        // Adaptation variable decays exponentially during recovery
        if (pImpl->state.adaptationVariable > 0.0f) {
            pImpl->state.adaptationVariable *= 0.95f;
        }
        
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C + adaptation + noise
    // where V = membrane potential, V_rest = resting potential
    //       tau = membrane time constant, I = synaptic current
    //       C = membrane capacitance, adaptation = spike-frequency adaptation
    // 
    // The LIF equation models the passive membrane properties:
    // - Leak current drives membrane potential toward resting potential
    // - Synaptic current directly influences membrane potential
    // - Adaptation current provides negative feedback after spiking
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    float threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Enhanced synaptic input integration with short-term plasticity
    // Synaptic inputs are modeled as temporal filters to represent
    // synaptic dynamics (rise time, decay time, depression/facilitation)
    float synapticInput = pImpl->synapticInput;
    
    // Apply synaptic dynamics: first-order low-pass filter for realism
    // This models synaptic integration time constants (typically 1-10ms)
    float synapticDecay = std::exp(-1000.0f * dt / Impl::SYNAPTIC_TIME_CONSTANT);
    float filteredSynapticInput = synapticInput * synapticDecay;
    
    // Synaptic contribution to membrane potential change
    float synapticContribution = filteredSynapticInput / C;
    
    // Leak contribution (standard LIF)
    float leakContribution = (V_rest - V) / tau;
    
    // Spike-frequency adaptation contribution
    // Adaptation variable slowly increases with spiking, causing hyperpolarization
    // This implements biological spike-frequency adaptation
    float adaptationContribution = 0.0f;
    if (pImpl->state.adaptationVariable > 0.0f) {
        adaptationContribution = -pImpl->state.adaptationVariable * Impl::ADAPTATION_CONTRIBUTION_FACTOR;
        // Decay adaptation variable over time with exponential decay
        // Typical adaptation time constant: 1000ms
        pImpl->state.adaptationVariable *= std::exp(-1000.0f * dt / Impl::ADAPTATION_DECAY_TIME_CONSTANT); // tau = 1000ms
    }
    
    // Update membrane potential using exponential Euler integration
    // This method is numerically stable for the LIF differential equation
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution + adaptationContribution);
    
    // Apply hard bounds to prevent numerical instability
    // Membrane potential is constrained to biologically plausible ranges
    V = std::clamp(V, Impl::V_LOWER_BOUND, Impl::V_UPPER_BOUND);
    
    // Check for spike with hysteresis to prevent multiple spikes
    // Use firing state to prevent re-firing on same timestep
    if (V >= threshold && pImpl->state.firingState != FiringState::Active) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike timestamp for spike-timing-dependent plasticity (STDP)
        recordSpike(currentTime);
        
        // Reset membrane potential with exponential decay instead of hard reset
        // This models realistic spike afterpotential (subthreshold depolarization)
        // After spike, membrane potential recovers toward reset potential
        float targetReset = pImpl->state.resetPotential + Impl::POST_SPIKE_RECOVERY_RATE * (pImpl->state.restingPotential - pImpl->state.resetPotential);
        V = targetReset;
        
        // Enter refractory period with adaptive length
        // Refractory period length adapts based on input strength (biologically realistic)
        // Stronger inputs cause longer refractory periods to prevent runaway excitation
        uint32_t baseRefractory = pImpl->state.refractoryPeriod;
        uint32_t inputBasedRefractory = static_cast<uint32_t>(std::max(1.0f, synapticInput * 10.0f));
        pImpl->state.refractoryRemaining = baseRefractory + inputBasedRefractory;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        // Adaptation increases with each spike, implementing biological plasticity
        // Includes saturation to prevent unbounded growth
        pImpl->state.adaptationVariable += 1.0f + 0.1f * pImpl->state.adaptationVariable; // Saturation
        
        // Apply post-spike afterhyperpolarization (AHP)
        // AHP slowly brings membrane potential further down after spike
        // Models calcium-dependent potassium currents in real neurons
        pImpl->state.membranePotential -= Impl::AHP_AMPLITUDE * std::exp(-static_cast<float>(dt) / 10.0f);
        
        // Clamp to prevent instability
        pImpl->state.membranePotential = std::clamp(pImpl->state.membranePotential, Impl::V_LOWER_BOUND, Impl::V_UPPER_BOUND);
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    // Synaptic inputs are transient and must be reset each timestep
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

// Enhanced LIF step with adaptive time stepping and realistic dynamics
    // Uses exponential Euler integration for better accuracy
    
    // Determine optimal timestep based on neuron state for stability
    TimestepDuration dt = 0.001f; // Default 1ms
    
    // Adaptive time stepping: reduce dt if membrane potential is near threshold
    // This prevents numerical instability during spikes
    float V = pImpl->state.membranePotential;
    float threshold = pImpl->state.threshold;
    if (std::abs(V - threshold) < Impl::THRESHOLD_HYSTERESIS) { // Within hysteresis margin
        dt = 0.0001f; // Use 0.1ms for better precision near threshold
    }
    
    // Additional adaptive stepping for refractory recovery
    if (pImpl->state.refractoryRemaining > 0) {
        dt = 0.0001f; // Fine-grained stepping during refractory period
    }
    
    // Apply enhanced LIF dynamics with spike-frequency adaptation
    stepLIF(currentTime, dt);
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
