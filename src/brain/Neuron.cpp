#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Validation constants
namespace {
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    constexpr float MIN_RESTING_POTENTIAL = -90.0f;
    constexpr float MAX_RESTING_POTENTIAL = -50.0f;
    constexpr float MIN_THRESHOLD = -60.0f;
    constexpr float MAX_THRESHOLD = -40.0f;
    constexpr float MIN_RESET_POTENTIAL = -80.0f;
    constexpr float MAX_RESET_POTENTIAL = -60.0f;
    constexpr float MIN_LEAK_CONDUCTANCE = 1.0f;
    constexpr float MAX_LEAK_CONDUCTANCE = 100.0f;
    constexpr uint32_t MIN_REFECTORY_PERIOD = 1;
    constexpr uint32_t MAX_REFECTORY_PERIOD = 100;
    constexpr size_t MAX_SPIKE_HISTORY_SIZE = 1000;
}

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
    
    // LIF parameters with validation ranges
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Validation ranges
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_RESTING_POTENTIAL = -90.0f;
    static constexpr float MAX_RESTING_POTENTIAL = -50.0f;
    static constexpr float MIN_THRESHOLD = -60.0f;
    static constexpr float MAX_THRESHOLD = -40.0f;
    static constexpr float MIN_RESET_POTENTIAL = -80.0f;
    static constexpr float MAX_RESET_POTENTIAL = -60.0f;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Validate initial state
        validateNeuronState();
    }
    
    void validateNeuronState() {
        // Validate membrane potential
        if (state.membranePotential < MIN_MEMBRANE_POTENTIAL || 
            state.membranePotential > MAX_MEMBRANE_POTENTIAL) {
            state.membranePotential = std::clamp(state.membranePotential, 
                                               MIN_MEMBRANE_POTENTIAL, MAX_MEMBRANE_POTENTIAL);
        }
        
        // Validate resting potential
        if (state.restingPotential < MIN_RESTING_POTENTIAL || 
            state.restingPotential > MAX_RESTING_POTENTIAL) {
            state.restingPotential = std::clamp(state.restingPotential, 
                                               MIN_RESTING_POTENTIAL, MAX_RESTING_POTENTIAL);
        }
        
        // Validate threshold
        if (state.threshold < MIN_THRESHOLD || state.threshold > MAX_THRESHOLD) {
            state.threshold = std::clamp(state.threshold, MIN_THRESHOLD, MAX_THRESHOLD);
        }
        
        // Validate reset potential
        if (state.resetPotential < MIN_RESET_POTENTIAL || 
            state.resetPotential > MAX_RESET_POTENTIAL) {
            state.resetPotential = std::clamp(state.resetPotential, 
                                             MIN_RESET_POTENTIAL, MAX_RESET_POTENTIAL);
        }
        
        // Validate leak conductance
        if (state.leakConductance < MIN_LEAK_CONDUCTANCE || 
            state.leakConductance > MAX_LEAK_CONDUCTANCE) {
            state.leakConductance = std::clamp(state.leakConductance, 
                                              MIN_LEAK_CONDUCTANCE, MAX_LEAK_CONDUCTANCE);
        }
        
        // Validate refractory period
        if (state.refractoryPeriod < MIN_REFECTORY_PERIOD || 
            state.refractoryPeriod > MAX_REFECTORY_PERIOD) {
            state.refractoryPeriod = std::clamp(state.refractoryPeriod, 
                                               MIN_REFECTORY_PERIOD, MAX_REFECTORY_PERIOD);
        }
        
        // Ensure threshold is above resting potential (physiological constraint)
        if (state.threshold <= state.restingPotential) {
            state.threshold = state.restingPotential + 10.0f;  // 10mV above resting
        }
        
        // Ensure reset potential is between resting and threshold
        if (state.resetPotential < state.restingPotential || 
            state.resetPotential > state.threshold) {
            state.resetPotential = state.restingPotential + 
                                 (state.threshold - state.restingPotential) * 0.5f;  // midpoint
        }
    }
    
    void clampMembranePotential() {
        // Clamp membrane potential to biologically plausible range
        state.membranePotential = std::clamp(state.membranePotential, 
                                           MIN_MEMBRANE_POTENTIAL, MAX_MEMBRANE_POTENTIAL);
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (id.value == 0) {
        throw std::invalid_argument("Neuron ID cannot be zero");
    }
    
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Validate initial state
    pImpl->validateNeuronState();
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
    if (potential < Impl::MIN_MEMBRANE_POTENTIAL || potential > Impl::MAX_MEMBRANE_POTENTIAL) {
        throw std::out_of_range("Membrane potential out of valid range");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    float newPotential = pImpl->state.membranePotential + delta;
    if (newPotential < Impl::MIN_MEMBRANE_POTENTIAL || newPotential > Impl::MAX_MEMBRANE_POTENTIAL) {
        // Clamp to valid range instead of throwing exception for numerical stability
        pImpl->state.membranePotential = std::clamp(newPotential, 
                                                  Impl::MIN_MEMBRANE_POTENTIAL, 
                                                  Impl::MAX_MEMBRANE_POTENTIAL);
    } else {
        pImpl->state.membranePotential = newPotential;
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (threshold < Impl::MIN_THRESHOLD || threshold > Impl::MAX_THRESHOLD) {
        throw std::out_of_range("Threshold out of valid range");
    }
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
    if (conductance < Impl::MIN_LEAK_CONDUCTANCE || conductance > Impl::MAX_LEAK_CONDUCTANCE) {
        throw std::out_of_range("Leak conductance out of valid range");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps < MIN_REFECTORY_PERIOD || steps > MAX_REFECTORY_PERIOD) {
        throw std::out_of_range("Refractory period out of valid range");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (potential < Impl::MIN_RESTING_POTENTIAL || potential > Impl::MAX_RESTING_POTENTIAL) {
        throw std::out_of_range("Resting potential out of valid range");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (potential < Impl::MIN_RESET_POTENTIAL || potential > Impl::MAX_RESET_POTENTIAL) {
        throw std::out_of_range("Reset potential out of valid range");
    }
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
    // Prevent spike history overflow
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    pImpl->spikeHistory.push_back(timestamp);
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
    
    // Validate timestep
    if (dt <= 0.0) {
        throw std::invalid_argument("Timestep must be positive");
    }
    
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
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    pImpl->clampMembranePotential();
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
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
    
    // Validate state after reset
    pImpl->validateNeuronState();
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
    
    // Ensure all parameters are valid
    pImpl->validateNeuronState();
    
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
