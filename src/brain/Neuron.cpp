#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Neuron implementation - improved memory safety and validation

Neuron::Neuron(NeuronId id) 
    : id_(id), type_(NeuronType::Internal), regionId_(INVALID_REGION_ID), 
      populationId_(INVALID_POPULATION_ID), totalCurrent_(0.0f) {}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept
    : id_(other.id_), type_(other.type_), state_(std::move(other.state_)),
      regionId_(other.regionId_), populationId_(other.populationId_),
      totalCurrent_(other.totalCurrent_), spikeHistory_(std::move(other.spikeHistory_)),
      incomingSynapses_(std::move(other.incomingSynapses_)),
      outgoingSynapses_(std::move(other.outgoingSynapses_)),
      plasticityFlags_(std::move(other.plasticityFlags_)) {}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        type_ = other.type_;
        state_ = std::move(other.state_);
        regionId_ = other.regionId_;
        populationId_ = other.populationId_;
        totalCurrent_ = other.totalCurrent_;
        spikeHistory_ = std::move(other.spikeHistory_);
        incomingSynapses_ = std::move(other.incomingSynapses_);
        outgoingSynapses_ = std::move(other.outgoingSynapses_);
        plasticityFlags_ = std::move(other.plasticityFlags_);
    }
    return *this;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    totalCurrent_ += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    totalCurrent_ -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    state_.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    totalCurrent_ += current;
}

void Neuron::recordSpike(Timestamp timestamp) {
    spikeHistory_.push_back(timestamp);
    validateSpikeHistory();
}

// Real LIF neuron dynamics implementation
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (state_.refractoryRemaining > 0) {
        --state_.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        totalCurrent_ = 0.0f;
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
    float tau = NeuronConstants::TIME_CONSTANT;  // ms
    float C = NeuronConstants::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = totalCurrent_ / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (state_.adaptationVariable > 0.0f) {
        V -= state_.adaptationVariable * NeuronConstants::ADAPTATION_EFFECT;
        state_.adaptationVariable *= NeuronConstants::ADAPTATION_DECAY;  // Decay adaptation
    }
    
    // Validate potential bounds
    if (V < NeuronConstants::MIN_POTENTIAL) V = NeuronConstants::MIN_POTENTIAL;
    if (V > NeuronConstants::MAX_POTENTIAL) V = NeuronConstants::MAX_POTENTIAL;
    
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
        state_.adaptationVariable += NeuronConstants::DEFAULT_ADAPTATION;
    }
    
    // Clear synaptic input for next step
    totalCurrent_ = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Use default timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    state_ = NeuronState();
    totalCurrent_ = 0.0f;
    spikeHistory_.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    state_.membranePotential = state_.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    state_.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    state_.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    state_.resetPotential = state_.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    state_.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    state_.firingState = FiringState::Resting;
    state_.refractoryRemaining = 0;
    state_.adaptationVariable = 0.0f;
    state_.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    totalCurrent_ = 0.0f;
    spikeHistory_.clear();
}

} // namespace nlm
