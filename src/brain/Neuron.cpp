#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Validation helper functions
namespace NeuronValidation {
    
    void validateMembranePotential(const std::string& context, MembranePotential potential) {
        constexpr float MIN_POTENTIAL = -100.0f;
        constexpr float MAX_POTENTIAL = 50.0f;
        if (potential < MIN_POTENTIAL || potential > MAX_POTENTIAL) {
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Membrane potential " + std::to_string(potential) + 
                " outside valid range [" + std::to_string(MIN_POTENTIAL) + ", " + 
                std::to_string(MAX_POTENTIAL) + "]");
        }
    }
    
    void validateThreshold(const std::string& context, MembranePotential threshold, 
                          MembranePotential restingPotential) {
        if (threshold <= restingPotential) {
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Threshold (" + std::to_string(threshold) + ") must be > resting potential (" + 
                std::to_string(restingPotential) + ")");
        }
        validateMembranePotential(context + " (threshold)", threshold);
    }
    
    void validateRefractoryPeriod(const std::string& context, uint32_t period) {
        if (period == 0 || period > 1000) { // Reasonable range
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Refractory period " + std::to_string(period) + 
                " outside valid range [1, 1000]");
        }
    }
    
    void validateSynapticInput(const std::string& context, MembranePotential amplitude) {
        constexpr float MAX_INPUT = 100.0f;
        if (std::abs(amplitude) > MAX_INPUT) {
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Synaptic input amplitude " + std::to_string(amplitude) + 
                " exceeds maximum allowed " + std::to_string(MAX_INPUT));
        }
    }
    
    void validateTimeStep(const std::string& context, TimestepDuration dt) {
        if (dt <= 0.0 || dt > 0.1) { // Max 100ms time step
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Time step " + std::to_string(dt) + " must be > 0 and <= 0.1");
        }
    }
    
    void validateTimestamp(const std::string& context, Timestamp timestamp) {
        if (timestamp < 0.0) {
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Timestamp " + std::to_string(timestamp) + " must be >= 0");
        }
    }
    
    void validateSpikeHistory(const std::string& context, const std::vector<Timestamp>& history) {
        if (history.size() > 1000) {
            throw std::invalid_argument("NeuronValidation: " + context + 
                " - Spike history exceeds maximum size of 1000");
        }
        for (size_t i = 0; i < history.size(); ++i) {
            validateTimestamp(context + " (spike " + std::to_string(i) + ")", history[i]);
        }
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    NeuronValidation::validateMembranePotential("setMembranePotential", potential);
    pImpl->state.membranePotential = potential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Get current resting potential for validation
    MembranePotential restingPotential = pImpl->state.restingPotential;
    NeuronValidation::validateThreshold("setThreshold", threshold, restingPotential);
    pImpl->state.threshold = threshold;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    NeuronValidation::validateMembranePotential("setRestingPotential", potential);
    pImpl->state.restingPotential = potential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    NeuronValidation::validateMembranePotential("setResetPotential", potential);
    pImpl->state.resetPotential = potential;
}

void Neuron::setRefractoryPeriod(uint32_t period) {
    NeuronValidation::validateRefractoryPeriod("setRefractoryPeriod", period);
    pImpl->state.refractoryPeriod = period;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    NeuronValidation::validateSynapticInput("receiveExcitatoryInput", amplitude);
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    NeuronValidation::validateSynapticInput("receiveInhibitoryInput", amplitude);
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    NeuronValidation::validateSynapticInput("receiveModulatoryInput", amplitude);
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    NeuronValidation::validateSynapticInput("injectCurrent", current);
    pImpl->synapticInput += current;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    // Validate inputs
    NeuronValidation::validateTimestamp("stepLIF", currentTime);
    NeuronValidation::validateTimeStep("stepLIF", dt);
    
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
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike with validation
        NeuronValidation::validateTimestamp("recordSpike", currentTime);
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

void Neuron::recordSpike(Timestamp timestamp) {
    NeuronValidation::validateTimestamp("recordSpike", timestamp);
    
    // Check spike history size before adding
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    
    pImpl->spikeHistory.push_back(timestamp);
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Validate RNG state
    if (!rng.isValid()) {
        throw std::runtime_error("Neuron::initializeRandom - Invalid random generator state");
    }
    
    try {
        // Membrane potential starts near resting potential with biological constraints
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
        
        // Validate that threshold > resting potential
        NeuronValidation::validateThreshold("initializeRandom", 
            pImpl->state.threshold, pImpl->state.restingPotential);
        
        // Clear any residual state
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
        
    } catch (const std::exception& e) {
        // Reset state if initialization fails
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
        throw std::runtime_error("Neuron::initializeRandom - Failed to initialize neuron: " + std::string(e.what()));
    }
}

std::string Neuron::getStateSummary() const {
    std::stringstream ss;
    ss << "Neuron State:" << std::endl;
    ss << "  ID: " << pImpl->id.value << std::endl;
    ss << "  Type: " << static_cast<int>(pImpl->type) << std::endl;
    ss << "  Membrane Potential: " << std::fixed << std::setprecision(3) 
       << pImpl->state.membranePotential << " mV" << std::endl;
    ss << "  Threshold: " << std::fixed << std::setprecision(3) 
       << pImpl->state.threshold << " mV" << std::endl;
    ss << "  Resting Potential: " << std::fixed << std::setprecision(3) 
       << pImpl->state.restingPotential << " mV" << std::endl;
    ss << "  Reset Potential: " << std::fixed << std::setprecision(3) 
       << pImpl->state.resetPotential << " mV" << std::endl;
    ss << "  Firing State: " << static_cast<int>(pImpl->state.firingState) << std::endl;
    ss << "  Refractory Remaining: " << pImpl->state.refractoryRemaining << std::endl;
    ss << "  Adaptation Variable: " << std::fixed << std::setprecision(3) 
       << pImpl->state.adaptationVariable << std::endl;
    ss << "  Last Spike Time: " << std::fixed << std::setprecision(3) 
       << pImpl->state.lastSpikeTime << std::endl;
    ss << "  Spike History Size: " << pImpl->spikeHistory.size() << std::endl;
    ss << "  Synaptic Input: " << std::fixed << std::setprecision(3) 
       << pImpl->synapticInput << std::endl;
    
    return ss.str();
}

} // namespace nlm

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
    V = std::clamp(V, -100.0f, 50.0f);
    
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
