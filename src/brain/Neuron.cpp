#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// LIF parameters (moved from Impl)
static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
static constexpr float TIME_CONSTANT = 20.0f;  // ms
static constexpr size_t MAX_SPIKE_HISTORY = 100;

Neuron::Neuron(NeuronId id) 
    : id_(id)
    , type_(NeuronType::Internal)
    , state_()
    , regionId_(INVALID_REGION_ID)
    , populationId_(INVALID_POPULATION_ID)
    , synapticInput_(0.0f)
    , spikeHistory_()
    , incomingSynapses_()
    , outgoingSynapses_()
    , plasticityFlags_() {
    // Initialize neuron state
    state_.membranePotential = -70.0f;
    state_.restingPotential = -70.0f;
    state_.threshold = -55.0f;
    state_.resetPotential = -70.0f;
    state_.leakConductance = 10.0f;
    state_.firingState = FiringState::Resting;
    state_.refractoryRemaining = 0;
    state_.refractoryPeriod = 5;
    state_.adaptationVariable = 0.0f;
    state_.lastSpikeTime = -1.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept
    : id_(other.id_)
    , type_(other.type_)
    , state_(std::move(other.state_))
    , regionId_(other.regionId_)
    , populationId_(other.populationId_)
    , synapticInput_(other.synapticInput_)
    , spikeHistory_(std::move(other.spikeHistory_))
    , incomingSynapses_(std::move(other.incomingSynapses_))
    , outgoingSynapses_(std::move(other.outgoingSynapses_))
    , plasticityFlags_(other.plasticityFlags_) {}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        type_ = other.type_;
        state_ = std::move(other.state_);
        regionId_ = other.regionId_;
        populationId_ = other.populationId_;
        synapticInput_ = other.synapticInput_;
        spikeHistory_ = std::move(other.spikeHistory_);
        incomingSynapses_ = std::move(other.incomingSynapses_);
        outgoingSynapses_ = std::move(other.outgoingSynapses_);
        plasticityFlags_ = other.plasticityFlags_;
    }
    return *this;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    synapticInput_ += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    synapticInput_ -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    state_.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    synapticInput_ += current;
}

void Neuron::recordSpike(Timestamp timestamp) {
    spikeHistory_.push_back(timestamp);
    if (spikeHistory_.size() > MAX_SPIKE_HISTORY) {
        spikeHistory_.erase(spikeHistory_.begin());
    }
}

float Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (state_.refractoryRemaining > 0) {
        --state_.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        synapticInput_ = 0.0f;
        if (state_.refractoryRemaining == 0) {
            state_.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = state_.membranePotential;
    MembranePotential V_rest = state_.restingPotential;
    MembranePotential V_reset = state_.resetPotential;
    MembranePotential threshold = state_.threshold;
    float tau = TIME_CONSTANT;  // ms
    float C = MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = synapticInput_ / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (state_.adaptationVariable > 0.0f) {
        V -= state_.adaptationVariable * 0.01f;
        state_.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        state_.firingState = FiringState::Active;
        state_.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        state_.refractoryRemaining = state_.refractoryPeriod;
        state_.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        state_.adaptationVariable += 1.0f;
    } else {
        state_.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    synapticInput_ = 0.0f;
    
    return fired;
}

void Neuron::reset() {
    state_.membranePotential = -70.0f;
    state_.restingPotential = -70.0f;
    state_.threshold = -55.0f;
    state_.resetPotential = -70.0f;
    state_.leakConductance = 10.0f;
    state_.firingState = FiringState::Resting;
    state_.refractoryRemaining = 0;
    state_.refractoryPeriod = 5;
    state_.adaptationVariable = 0.0f;
    state_.lastSpikeTime = -1.0f;
    
    synapticInput_ = 0.0f;
    spikeHistory_.clear();
    incomingSynapses_.clear();
    outgoingSynapses_.clear();
    
    plasticityFlags_.hebbian = false;
    plasticityFlags_.stdp = false;
    plasticityFlags_.reward_modulated = false;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    plasticityFlags_.hebbian = hebbian;
    plasticityFlags_.stdp = stdp;
    plasticityFlags_.reward_modulated = rewardModulated;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Set random neuron parameters
    state_.threshold = rng.uniformReal(-60.0f, -40.0f);
    state_.restingPotential = rng.uniformReal(-75.0f, -65.0f);
    state_.resetPotential = state_.restingPotential;
    state_.leakConductance = rng.uniformReal(5.0f, 15.0f);
    state_.refractoryPeriod = rng.uniformInt(3, 10);
    state_.adaptationVariable = rng.uniformReal(0.0f, 0.5f);
    
    // Set random firing rate (baseline)
    state_.firingRate = rng.uniformReal(0.0f, 20.0f);
    
    // Random plasticity configuration
    enablePlasticity(
        rng.randomFloat() > 0.3f,
        rng.randomFloat() > 0.3f,
        rng.randomFloat() > 0.8f
    );
}

} // namespace nlm
