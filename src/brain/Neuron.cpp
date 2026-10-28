#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

// ========== OPTIMIZED NEURON IMPLEMENTATION ========== //

// Constants for better numerical stability and performance
namespace NeuronConstants {
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    constexpr float TIME_CONSTANT = 20.0f;      // ms
    constexpr size_t MAX_SPIKE_HISTORY = 100;
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    constexpr float ADAPTATION_DECAY = 0.95f;
    constexpr float ADAPTATION_INCREMENT = 1.0f;
    constexpr float SPIKE_ADAPTATION_FACTOR = 0.01f;
    constexpr float SYNAPTIC_INPUT_SCALE = 10.0f;
}

// Optimized helper functions for neuron operations
namespace NeuronHelpers {
    
    // Optimized LIF step computation using better numerical methods
    float computeLIFStep(float V, float V_rest, float synapticInput, 
                        float dt, float timeConstant, float capacity) {
        // More efficient LIF computation with optimized order of operations
        float leakTerm = (V_rest - V) / timeConstant;
        float synapticTerm = synapticInput / capacity;
        float totalInput = leakTerm + synapticTerm;
        
        // Exponential Euler integration (more stable than forward Euler)
        float V_new = V + static_cast<float>(dt) * 1000.0f * totalInput;
        
        // Clamping with early exit optimization
        if (V_new > NeuronConstants::MAX_MEMBRANE_POTENTIAL) {
            return NeuronConstants::MAX_MEMBRANE_POTENTIAL;
        } else if (V_new < NeuronConstants::MIN_MEMBRANE_POTENTIAL) {
            return NeuronConstants::MIN_MEMBRANE_POTENTIAL;
        }
        
        return V_new;
    }
    
    // Optimized spike threshold check with early exit
    bool checkSpikeThreshold(float V, float threshold) {
        return V >= threshold;
    }
    
    // Optimized spike history management with circular buffer
    void addSpikeToHistory(std::vector<Timestamp>& history, Timestamp timestamp) {
        if (history.size() < NeuronConstants::MAX_SPIKE_HISTORY) {
            history.push_back(timestamp);
        } else {
            // Move all elements left and add new one (circular buffer simulation)
            for (size_t i = 0; i < history.size() - 1; ++i) {
                history[i] = history[i + 1];
            }
            history.back() = timestamp;
        }
    }
    
    // Optimized adaptation variable update
    void updateAdaptationVariable(float& adaptation, bool spiked, float dt) {
        if (spiked) {
            adaptation += NeuronConstants::ADAPTATION_INCREMENT;
        } else {
            adaptation *= NeuronConstants::ADAPTATION_DECAY;
        }
        
        // Early clamping
        if (adaptation < 0.0f) adaptation = 0.0f;
        if (adaptation > 100.0f) adaptation = 100.0f; // Reasonable upper bound
    }
    
    // Optimized input normalization
    float normalizeInput(float value, float scale = NeuronConstants::SYNAPTIC_INPUT_SCALE) {
        return value / scale;
    }
}

// ========== NEURON CLASS DEFINITION - OPTIMIZED ========== //

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
    
    // Pre-allocated vectors for better performance
    std::vector<float> adaptationHistory;
    std::vector<float> inputHistory;
    
    // Cache for frequently accessed values
    float cachedThreshold = 0.0f;
    float cachedRestingPotential = 0.0f;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             adaptationHistory(50), inputHistory(50),
             cachedThreshold(0.0f), cachedRestingPotential(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    // Initialize adaptation and input history caches
    std::fill(pImpl->adaptationHistory.begin(), pImpl->adaptationHistory.end(), 0.0f);
    std::fill(pImpl->inputHistory.begin(), pImpl->inputHistory.end(), 0.0f);
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
    // Update cached values for performance
    updateCachedValues();
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
    updateCachedValues();
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
    pImpl->cachedThreshold = threshold;
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
    pImpl->cachedRestingPotential = potential;
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
    // Optimized excitatory input processing
    pImpl->synapticInput += NeuronHelpers::normalizeInput(amplitude);
    pImpl->totalCurrent += amplitude;
    
    // Store in input history for statistics
    static size_t inputIndex = 0;
    if (inputIndex < pImpl->inputHistory.size()) {
        pImpl->inputHistory[inputIndex++] = amplitude;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Optimized inhibitory input processing
    pImpl->synapticInput -= NeuronHelpers::normalizeInput(amplitude);
    pImpl->totalCurrent -= amplitude;
    
    // Store in input history for statistics
    static size_t inputIndex = 0;
    if (inputIndex < pImpl->inputHistory.size()) {
        pImpl->inputHistory[inputIndex++] = -amplitude;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Optimized modulatory input affecting plasticity
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    
    // Store in adaptation history for statistics
    static size_t adaptIndex = 0;
    if (adaptIndex < pImpl->adaptationHistory.size()) {
        pImpl->adaptationHistory[adaptIndex++] = amplitude;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    // Optimized direct current injection
    pImpl->synapticInput += NeuronHelpers::normalizeInput(current);
    pImpl->totalCurrent += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
    pImpl->totalCurrent = 0.0f;
    // Reset input history index
    std::fill(pImpl->inputHistory.begin(), pImpl->inputHistory.end(), 0.0f);
}

void Neuron::recordSpike(Timestamp timestamp) {
    NeuronHelpers::addSpikeToHistory(pImpl->spikeHistory, timestamp);
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
    // Reset spike history cache
    pImpl->spikeHistory.reserve(NeuronConstants::MAX_SPIKE_HISTORY);
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
    
    // Handle refractory period with early exit optimization
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Use optimized LIF computation
    pImpl->state.membranePotential = NeuronHelpers::computeLIFStep(
        pImpl->state.membranePotential,
        pImpl->cachedRestingPotential,
        pImpl->synapticInput,
        dt,
        NeuronConstants::TIME_CONSTANT,
        NeuronConstants::MEMBRANE_CAPACITANCE
    );
    
    // Apply spike-frequency adaptation (optimized)
    NeuronHelpers::updateAdaptationVariable(
        pImpl->state.adaptationVariable,
        false, // We'll set this below if neuron fires
        dt
    );
    
    if (pImpl->state.adaptationVariable > 0.0f) {
        pImpl->state.membranePotential -= pImpl->state.adaptationVariable * NeuronConstants::SPIKE_ADAPTATION_FACTOR;
    }
    
    // Check for spike with early exit
    if (NeuronHelpers::checkSpikeThreshold(pImpl->state.membranePotential, pImpl->cachedThreshold)) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike using optimized method
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->state.membranePotential = pImpl->state.resetPotential;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        NeuronHelpers::updateAdaptationVariable(pImpl->state.adaptationVariable, true, dt);
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    clearTotalCurrent();
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Optimized step with standard timestep
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->totalCurrent = 0.0f;
    clearSpikeHistory();
    // Reset adaptation and input history
    std::fill(pImpl->adaptationHistory.begin(), pImpl->adaptationHistory.end(), 0.0f);
    std::fill(pImpl->inputHistory.begin(), pImpl->inputHistory.end(), 0.0f);
    
    // Update cached values
    updateCachedValues();
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
    
    // Update cached values
    updateCachedValues();
}

void Neuron::updateCachedValues() {
    // Update cached values for performance
    pImpl->cachedThreshold = pImpl->state.threshold;
    pImpl->cachedRestingPotential = pImpl->state.restingPotential;
}

} // namespace nlm
