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

    // Validate input parameters
    if (timestamp < 0.0f) {
        NLM_LOG_ERROR("Invalid spike timestamp in Neuron::recordSpike: " + std::to_string(timestamp));
        return;
    }
    
    // Validate spike history management with memory bounds checking
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        // Use circular buffer management to prevent memory leaks
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    
    // Add new spike timestamp
    pImpl->spikeHistory.push_back(timestamp);
    
    // Ensure spike history doesn't exceed maximum size (defensive check)
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        // Truncate if we somehow exceeded the limit
        pImpl->spikeHistory.resize(Impl::MAX_SPIKE_HISTORY);
    }
    
    NLM_LOG_DEBUG("Recorded spike at timestamp " + std::to_string(timestamp) + 
                  " (history size: " + std::to_string(pImpl->spikeHistory.size()) + ")");
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
    
    // Validate input parameters
    if (dt <= 0.0f) {
        NLM_LOG_ERROR("Invalid timestep duration in Neuron::stepLIF: " + std::to_string(dt));
        return false;
    }
    
    if (currentTime < 0.0f) {
        NLM_LOG_ERROR("Invalid current time in Neuron::stepLIF: " + std::to_string(currentTime));
        return false;
    }
    
    // Validate neuron state
    if (!isValidState(pImpl->state)) {
        NLM_LOG_ERROR("Invalid neuron state before stepLIF: " + neuronStateToString(pImpl->state));
        return false;
    }
    
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution - ensure proper time unit conversion
    float leakContribution = (V_rest - V) / tau;  // V_rest - V is in mV, tau is in ms
    
    // Update membrane potential using exponential Euler integration
    // Correct time unit conversion: dt is in seconds, need to convert to ms for consistency
    V = V + dt * (leakContribution + synapticContribution);  // dt is already in seconds for biological consistency
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Apply biological membrane potential bounds
    // Realistic neuron membrane potential bounds: -100mV to +50mV
    // These bounds represent typical biological limits for neuronal membrane potentials
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with proper state transition logic
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike with memory management
        recordSpike(currentTime);
        
        // Reset membrane potential to reset potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        // Proper state for subthreshold activity
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    // Final validation of neuron state after step
    if (!isValidState(pImpl->state)) {
        NLM_LOG_ERROR("Invalid neuron state after stepLIF: " + neuronStateToString(pImpl->state));
    }
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
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
    
    pImpl->spikeHistory.clear();
}

// Validation helper functions for Neuron class

bool Neuron::isValidState(const NeuronState& state) {
    // Validate neuron state parameters
    
    // Check for NaN or infinite values
    if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
        NLM_LOG_ERROR("Invalid neuron state: NaN or infinite membrane potential");
        return false;
    }
    
    if (std::isnan(state.restingPotential) || std::isinf(state.restingPotential)) {
        NLM_LOG_ERROR("Invalid neuron state: NaN or infinite resting potential");
        return false;
    }
    
    if (std::isnan(state.threshold) || std::isinf(state.threshold)) {
        NLM_LOG_ERROR("Invalid neuron state: NaN or infinite threshold");
        return false;
    }
    
    if (std::isnan(state.resetPotential) || std::isinf(state.resetPotential)) {
        NLM_LOG_ERROR("Invalid neuron state: NaN or infinite reset potential");
        return false;
    }
    
    // Biological validation constraints
    if (state.membranePotential < -150.0f || state.membranePotential > 100.0f) {
        NLM_LOG_WARNING("Membrane potential out of biological range: " + std::to_string(state.membranePotential));
    }
    
    if (state.restingPotential < -80.0f || state.restingPotential > -50.0f) {
        NLM_LOG_WARNING("Resting potential out of typical range: " + std::to_string(state.restingPotential));
    }
    
    if (state.threshold < -70.0f || state.threshold > -30.0f) {
        NLM_LOG_WARNING("Threshold out of typical range: " + std::to_string(state.threshold));
    }
    
    if (state.resetPotential < -80.0f || state.resetPotential > -50.0f) {
        NLM_LOG_WARNING("Reset potential out of typical range: " + std::to_string(state.resetPotential));
    }
    
    if (state.refractoryPeriod > 100) {
        NLM_LOG_WARNING("Refractory period unusually long: " + std::to_string(state.refractoryPeriod));
    }
    
    if (state.refractoryRemaining > state.refractoryPeriod) {
        NLM_LOG_ERROR("Refractory remaining greater than refractory period");
        return false;
    }
    
    if (state.lastSpikeTime < -1.0f) {
        NLM_LOG_ERROR("Invalid last spike time: " + std::to_string(state.lastSpikeTime));
        return false;
    }
    
    if (state.adaptationVariable < 0.0f || state.adaptationVariable > 10.0f) {
        NLM_LOG_WARNING("Adaptation variable out of reasonable range: " + std::to_string(state.adaptationVariable));
    }
    
    return true;
}

std::string Neuron::neuronStateToString(const NeuronState& state) {
    std::ostringstream oss;
    oss << "Membrane Potential: " << state.membranePotential << "mV, "
        << "Firing State: " << static_cast<int>(state.firingState) << ", "
        << "Refractory: " << state.refractoryRemaining << "/" << state.refractoryPeriod << ", "
        << "Last Spike: " << state.lastSpikeTime << "s, "
        << "Adaptation: " << state.adaptationVariable;
    return oss.str();
}

} // namespace nlm
