// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation - Neuron Implementation
//
// This file implements the Leaky Integrate-and-Fire (LIF) neuron model with:
// - Clean separation of concerns
// - Better encapsulation of neuron dynamics
// - Improved modularity and maintainability
// - Enhanced testability

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

// Constants for neuron behavior (moved to namespace for better organization)
namespace NeuronConstants {
    // LIF model parameters
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms
    constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;  // nS
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV
    constexpr float DEFAULT_THRESHOLD = -55.0f;  // mV
    constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;  // mV
    
    // Safety limits
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;  // mV
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;   // mV
    constexpr float MAX_ADAPTATION_VARIABLE = 10.0f;
    constexpr uint32_t MIN_REFECTORY_PERIOD = 2;      // ms
    constexpr uint32_t MAX_REFECTORY_PERIOD = 100;    // ms
    
    // Randomization ranges
    constexpr float RESTING_POTENTIAL_VARIATION = 2.0f;  // mV
    constexpr float THRESHOLD_VARIATION = 2.0f;          // mV
    constexpr float RESET_POTENTIAL_VARIATION = 5.0f;   // mV
    constexpr float INITIAL_SPIKE_RATE_VARIATION = 0.2f; // Hz
    
    // Stability constants
    constexpr float ADAPTATION_DECAY_RATE = 0.95f;
    constexpr float ADAPTATION_EFFECT = 0.01f;
    constexpr float SYNAPTIC_INPUT_SCALE = 10.0f;  // Scale factor for synaptic current
}

// Helper functions for neuron dynamics
namespace NeuronUtils {
    float clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
    
    float calculateMembraneChange(float V, float V_rest, float I_syn, 
                                 float tau, float C, float dt) {
        // LIF equation: dV/dt = (V_rest - V)/tau + I/C
        // Using Euler integration: V_new = V + dt * ((V_rest - V)/tau + I/C)
        float leakContribution = (V_rest - V) / tau;
        float synapticContribution = I_syn / C;
        return dt * 1000.0f * (leakContribution + synapticContribution);  // Convert ms to s
    }
    
    uint32_t clampRefractoryPeriod(uint32_t period) {
        return std::max(NeuronConstants::MIN_REFECTORY_PERIOD,
                       std::min(period, NeuronConstants::MAX_REFECTORY_PERIOD));
    }
}

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
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

// Basic property accessors
NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = NeuronUtils::clamp(potential,
        NeuronConstants::MIN_MEMBRANE_POTENTIAL, NeuronConstants::MAX_MEMBRANE_POTENTIAL);
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential = NeuronUtils::clamp(
        pImpl->state.membranePotential + delta,
        NeuronConstants::MIN_MEMBRANE_POTENTIAL,
        NeuronConstants::MAX_MEMBRANE_POTENTIAL);
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = NeuronUtils::clampRefractoryPeriod(steps);
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = NeuronUtils::clamp(conductance, 0.0f, NeuronConstants::MAX_LEAK_CONDUCTANCE);
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = NeuronUtils::clamp(potential,
        NeuronConstants::MIN_MEMBRANE_POTENTIAL, NeuronConstants::MAX_MEMBRANE_POTENTIAL);
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = NeuronUtils::clamp(potential,
        NeuronConstants::MIN_MEMBRANE_POTENTIAL, NeuronConstants::MAX_MEMBRANE_POTENTIAL);
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    pImpl->synapticInput += amplitude * NeuronConstants::SYNAPTIC_INPUT_SCALE;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    pImpl->synapticInput -= amplitude * NeuronConstants::SYNAPTIC_INPUT_SCALE;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    pImpl->state.adaptationVariable = NeuronUtils::clamp(
        pImpl->state.adaptationVariable, 
        -NeuronConstants::MAX_ADAPTATION_VARIABLE, 
        NeuronConstants::MAX_ADAPTATION_VARIABLE);
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    pImpl->synapticInput += current * NeuronConstants::SYNAPTIC_INPUT_SCALE;
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

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
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

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
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
    
    // LIF dynamics with improved separation of concerns
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Calculate membrane potential change using helper function
    float dV = NeuronUtils::calculateMembraneChange(V, V_rest, pImpl->synapticInput, tau, C, dt);
    V += dV;
    
    // Apply spike-frequency adaptation (improved implementation)
    applyAdaptationDynamics(V, dt);
    
    // Clamp membrane potential to prevent instability
    V = NeuronUtils::clamp(V, NeuronConstants::MIN_MEMBRANE_POTENTIAL, 
                         NeuronConstants::MAX_MEMBRANE_POTENTIAL);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        handleSpike(currentTime, V, V_reset);
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::applyAdaptationDynamics(MembranePotential& V, TimestepDuration dt) {
    // Improved adaptation dynamics with better separation
    if (pImpl->state.adaptationVariable > 0.0f) {
        // Apply adaptation effect
        V -= pImpl->state.adaptationVariable * NeuronConstants::ADAPTATION_EFFECT;
        
        // Decay adaptation variable
        pImpl->state.adaptationVariable *= NeuronConstants::ADAPTATION_DECAY_RATE;
    }
}

void Neuron::handleSpike(Timestamp currentTime, MembranePotential V, MembranePotential V_reset) {
    // Handle spike event with better separation
    pImpl->state.firingState = FiringState::Active;
    pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
    
    // Record spike
    recordSpike(currentTime);
    
    // Reset membrane potential
    pImpl->state.membranePotential = V_reset;
    
    // Enter refractory period
    pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
    pImpl->state.firingState = FiringState::Refractory;
    
    // Update adaptation for spike-frequency adaptation
    pImpl->state.adaptationVariable += NeuronConstants::MAX_ADAPTATION_VARIABLE;
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
    // Improved random initialization with biological constraints
    pImpl->state.membranePotential = pImpl->state.restingPotential + 
        rng.uniformReal(-NeuronConstants::RESTING_POTENTIAL_VARIATION, 
                       NeuronConstants::RESTING_POTENTIAL_VARIATION);
    
    pImpl->state.threshold = NeuronConstants::DEFAULT_THRESHOLD + 
        rng.uniformReal(-NeuronConstants::THRESHOLD_VARIATION, 
                       NeuronConstants::THRESHOLD_VARIATION);
    
    pImpl->state.restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL + 
        rng.uniformReal(-NeuronConstants::RESTING_POTENTIAL_VARIATION, 
                       NeuronConstants::RESTING_POTENTIAL_VARIATION);
    
    pImpl->state.resetPotential = pImpl->state.restingPotential + 
        rng.uniformReal(-NeuronConstants::RESET_POTENTIAL_VARIATION, 
                       NeuronConstants::RESET_POTENTIAL_VARIATION);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(
        rng.uniformInt(NeuronConstants::MIN_REFECTORY_PERIOD, 
                      NeuronConstants::MAX_REFECTORY_PERIOD));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Set leak conductance to default
    pImpl->state.leakConductance = NeuronConstants::DEFAULT_LEAK_CONDUCTANCE;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
