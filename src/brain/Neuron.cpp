#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>

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
    
    // Enhanced LIF parameters with configurable bounds
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms
    static constexpr float MIN_TIME_CONSTANT = 1.0f;
    static constexpr float MAX_TIME_CONSTANT = 1000.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 1000;  // Increased for better analysis
    
    // Enhanced biophysical parameters
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_RESTING_POTENTIAL = -90.0f;
    static constexpr float MAX_RESTING_POTENTIAL = -60.0f;
    static constexpr float MIN_THRESHOLD = -50.0f;
    static constexpr float MAX_THRESHOLD = -20.0f;
    static constexpr float MIN_RESET_POTENTIAL = -85.0f;
    static constexpr float MAX_RESET_POTENTIAL = -65.0f;
    
    // Adaptation parameters
    static constexpr float MAX_ADAPTATION_VARIABLE = 10.0f;
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;
    static constexpr float ADAPTATION_INCREMENT = 1.0f;
    static constexpr float MIN_ADAPTATION_INCREMENT = 0.0f;
    static constexpr float MAX_ADAPTATION_INCREMENT = 5.0f;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize state with validated ranges
        state.restingPotential = -70.0f;
        state.threshold = -55.0f;
        state.resetPotential = -70.0f;
        state.leakConductance = 10.0f;
        state.membranePotential = state.restingPotential;
        state.refractoryPeriod = 5;
        state.refractoryRemaining = 0;
        state.adaptationVariable = 0.0f;
        state.firingRate = 0.0f;
        state.firingState = FiringState::Resting;
        state.lastSpikeTime = -1.0f;
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Validate neuron ID
    if (static_cast<uint64_t>(id) == 0) {
        throw std::invalid_argument("Neuron ID cannot be zero");
    }
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
    // Validate potential range
    if (potential < Impl::MIN_MEMBRANE_POTENTIAL || potential > Impl::MAX_MEMBRANE_POTENTIAL) {
        throw std::out_of_range("Membrane potential out of valid range");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate delta doesn't push potential out of bounds
    if (pImpl->state.membranePotential + delta < Impl::MIN_MEMBRANE_POTENTIAL ||
        pImpl->state.membranePotential + delta > Impl::MAX_MEMBRANE_POTENTIAL) {
        throw std::out_of_range("Membrane potential change would exceed valid range");
    }
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold range
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
    if (rate < 0.0f || rate > 1000.0f) {
        throw std::out_of_range("Firing rate out of valid range");
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < 0.0f || conductance > 100.0f) {
        throw std::out_of_range("Leak conductance out of valid range");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps == 0) {
        throw std::invalid_argument("Refractory period cannot be zero");
    }
    if (steps > 10000) {
        throw std::out_of_range("Refractory period too large");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    // Validate resting potential range
    if (potential < Impl::MIN_RESTING_POTENTIAL || potential > Impl::MAX_RESTING_POTENTIAL) {
        throw std::out_of_range("Resting potential out of valid range");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    // Validate reset potential range
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
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Excitatory input cannot be negative");
    }
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Inhibitory input magnitude must be positive");
    }
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    if (std::abs(amplitude) > 100.0f) {
        throw std::out_of_range("Modulatory input amplitude too large");
    }
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    if (current < -100.0f || current > 100.0f) {
        throw std::out_of_range("Injected current amplitude out of valid range");
    }
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Spike timestamp cannot be negative");
    }
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
    
    // Validate timestep
    if (dt <= 0.0) {
        throw std::invalid_argument("Timestep must be positive");
    }
    
    // Validate time constant
    float tau = pImpl->state.leakConductance * Impl::DEFAULT_TIME_CONSTANT;
    if (tau < Impl::MIN_TIME_CONSTANT || tau > Impl::MAX_TIME_CONSTANT) {
        tau = Impl::DEFAULT_TIME_CONSTANT;  // Use default if out of range
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
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // Convert dt from seconds to milliseconds for consistency
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation with validated bounds
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= Impl::ADAPTATION_DECAY_RATE;
    }
    
    // Clamp membrane potential to prevent instability with validated bounds
    V = std::clamp(V, Impl::MIN_MEMBRANE_POTENTIAL, Impl::MAX_MEMBRANE_POTENTIAL);
    
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
        
        // Update adaptation for spike-frequency adaptation with validated bounds
        pImpl->state.adaptationVariable += Impl::ADAPTATION_INCREMENT;
        if (pImpl->state.adaptationVariable > Impl::MAX_ADAPTATION_VARIABLE) {
            pImpl->state.adaptationVariable = Impl::MAX_ADAPTATION_VARIABLE;
        }
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
    
    // Reinitialize state with validated ranges
    pImpl->state.restingPotential = -70.0f;
    pImpl->state.threshold = -55.0f;
    pImpl->state.resetPotential = -70.0f;
    pImpl->state.leakConductance = 10.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints and validation
    try {
        // Membrane potential starts near resting potential with validation
        pImpl->state.membranePotential = pImpl->state.restingPotential + 
            rng.uniformReal(-3.0f, 3.0f);
        
        // Validate membrane potential range
        if (pImpl->state.membranePotential < Impl::MIN_MEMBRANE_POTENTIAL ||
            pImpl->state.membranePotential > Impl::MAX_MEMBRANE_PENTIAL) {
            pImpl->state.membranePotential = pImpl->state.restingPotential;
        }
        
        // Threshold is typically -55mV with small variation and validation
        pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
        if (pImpl->state.threshold < Impl::MIN_THRESHOLD || 
            pImpl->state.threshold > Impl::MAX_THRESHOLD) {
            pImpl->state.threshold = -55.0f;
        }
        
        // Resting potential typically -70mV with validation
        pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
        if (pImpl->state.restingPotential < Impl::MIN_RESTING_POTENTIAL ||
            pImpl->state.restingPotential > Impl::MAX_RESTING_POTENTIAL) {
            pImpl->state.restingPotential = -70.0f;
        }
        
        // Reset potential is usually close to resting
        pImpl->state.resetPotential = pImpl->state.restingPotential + 
            rng.uniformReal(0.0f, 5.0f);
        if (pImpl->state.resetPotential < Impl::MIN_RESET_POTENTIAL ||
            pImpl->state.resetPotential > Impl::MAX_RESET_POTENTIAL) {
            pImpl->state.resetPotential = pImpl->state.restingPotential;
        }
        
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
        
    } catch (const std::exception& e) {
        // If initialization fails, set safe defaults
        reset();
    }
}

} // namespace nlm
