// Neuron.cpp improvements implementation

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Constants for neuron behavior
namespace NeuronConstants {
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms
    constexpr size_t DEFAULT_MAX_SPIKE_HISTORY = 100;
    constexpr float DEFAULT_MAX_MEMBRANE_POTENTIAL = 50.0f;  // mV
    constexpr float DEFAULT_MIN_MEMBRANE_POTENTIAL = -100.0f;  // mV
    constexpr float ADAPTATION_DECAY_FACTOR = 0.95f;
    constexpr float ADAPTATION_GAIN = 0.01f;
    constexpr float DEFAULT_REFACTORY_PERIOD = 5;  // steps
}

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = NeuronConstants::DEFAULT_MEMBRANE_CAPACITANCE;  // nF
    static constexpr float TIME_CONSTANT = NeuronConstants::DEFAULT_TIME_CONSTANT;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = NeuronConstants::DEFAULT_MAX_SPIKE_HISTORY;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (!pImpl) {
        throw std::bad_alloc();  // Handle memory allocation failure
    }
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->synapticInput = 0.0f;
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

NeuronId Neuron::getId() const {
    return pImpl ? pImpl->id : INVALID_NEURON_ID;
}

NeuronType Neuron::getType() const {
    return pImpl ? pImpl->type : NeuronType::Internal;
}

void Neuron::setType(NeuronType type) {
    if (pImpl) {
        pImpl->type = type;
    }
}

const NeuronState& Neuron::getState() const {
    static const NeuronState defaultState;
    return pImpl ? pImpl->state : defaultState;
}

NeuronState& Neuron::getState() {
    if (!pImpl) {
        static NeuronState defaultState;
        return defaultState;
    }
    return pImpl->state;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (pImpl) {
        pImpl->state.membranePotential += delta;
    }
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl ? pImpl->state.membranePotential : NeuronConstants::DEFAULT_MIN_MEMBRANE_POTENTIAL;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (pImpl) {
        pImpl->state.threshold = threshold;
    }
}

MembranePotential Neuron::getThreshold() const {
    return pImpl ? pImpl->state.threshold : NeuronConstants::DEFAULT_MIN_MEMBRANE_POTENTIAL + 25.0f;  // -55mV default
}

bool Neuron::isFiring() const {
    return pImpl && pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl && pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    if (pImpl) {
        pImpl->state.firingState = state;
    }
}

void Neuron::decrementRefractory() {
    if (pImpl && pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (pImpl) {
        pImpl->state.refractoryPeriod = steps;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl ? pImpl->state.refractoryPeriod : NeuronConstants::DEFAULT_REFACTORY_PERIOD;
}

void Neuron::setFiringRate(FiringRate rate) {
    if (pImpl) {
        pImpl->state.firingRate = rate;
    }
}

FiringRate Neuron::getFiringRate() const {
    return pImpl ? pImpl->state.firingRate : 0.0f;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (pImpl) {
        pImpl->state.leakConductance = conductance;
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl ? pImpl->state.leakConductance : NeuronConstants::DEFAULT_MEMBRANE_CAPACITANCE;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.restingPotential = potential;
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl ? pImpl->state.restingPotential : -70.0f;  // Default resting potential
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.resetPotential = potential;
    }
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl ? pImpl->state.resetPotential : getRestingPotential();
}

bool Neuron::checkThreshold() const {
    return pImpl && pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl ? pImpl->state.lastSpikeTime : -1.0f;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (pImpl) {
        pImpl->synapticInput += amplitude;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (pImpl) {
        pImpl->synapticInput -= amplitude;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (pImpl) {
        pImpl->state.adaptationVariable += amplitude * 0.1f;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    if (pImpl) {
        pImpl->synapticInput += current;
    }
}

void Neuron::clearTotalCurrent() {
    if (pImpl) {
        pImpl->synapticInput = 0.0f;
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (pImpl) {
        // Validate timestamp
        if (timestamp < 0) {
            return;  // Invalid timestamp
        }
        
        pImpl->spikeHistory.push_back(timestamp);
        
        // Maintain spike history size constraint
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
}

void Neuron::clearSpikeHistory() {
    if (pImpl) {
        pImpl->spikeHistory.clear();
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    static const std::vector<Timestamp> emptyHistory;
    return pImpl ? pImpl->spikeHistory : emptyHistory;
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->incomingSynapses.push_back(handle);
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (pImpl) {
        pImpl->outgoingSynapses.push_back(handle);
    }
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    static PlasticityFlags defaultFlags;
    return pImpl ? pImpl->plasticityFlags : defaultFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (pImpl) {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    }
}

void Neuron::setRegionId(RegionId region) {
    if (pImpl) {
        pImpl->regionId = region;
    }
}

RegionId Neuron::getRegionId() const {
    return pImpl ? pImpl->regionId : INVALID_REGION_ID;
}

void Neuron::setPopulationId(PopulationId population) {
    if (pImpl) {
        pImpl->populationId = population;
    }
}

PopulationId Neuron::getPopulationId() const {
    return pImpl ? pImpl->populationId : INVALID_POPULATION_ID;
}

void Neuron::reset() {
    if (pImpl) {
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) return;
    
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

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (!pImpl) {
        return false;  // Invalid neuron
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
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * NeuronConstants::ADAPTATION_GAIN;
        pImpl->state.adaptationVariable *= NeuronConstants::ADAPTATION_DECAY_FACTOR;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, NeuronConstants::DEFAULT_MIN_MEMBRANE_POTENTIAL, 
                   NeuronConstants::DEFAULT_MAX_MEMBRANE_POTENTIAL);
    
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

} // namespace nlm
