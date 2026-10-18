#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace nlm {

// Optimized neuron constants for better performance and maintainability
namespace NeuronConstants {
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;  // nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;      // ms
    constexpr size_t MAX_SPIKE_HISTORY = 100;
    constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV
    constexpr float DEFAULT_THRESHOLD = -55.0f;         // mV
    constexpr float DEFAULT_RESET_POTENTIAL_FACTOR = 1.0f;  // Relative to resting
    constexpr uint32_t MIN_REFRACTORY_PERIOD = 2;       // ms
    constexpr uint32_t MAX_REFRACTORY_PERIOD = 10;      // ms
    constexpr float ADAPTATION_INCREMENT = 1.0f;
    constexpr float ADAPTATION_DECAY_RATE = 0.95f;
    constexpr float ADAPTATION_EFFECT = 0.01f;
    constexpr float MEMBRANE_CLAMP_MIN = -100.0f;       // mV
    constexpr float MEMBRANE_CLAMP_MAX = 50.0f;        // mV
    constexpr float INPUT_VALIDATION_MAX = 1e6f;       // Safety limit
    constexpr float SYNAPTIC_INPUT_DECAY = 1.0f;         // Clear to zero each step
}

class NeuronStepOptimizer {
public:
    NeuronStepOptimizer(Neuron::Impl& impl) : pImpl(impl) {}
    
    // Optimized LIF step processing with improved performance
    bool stepLIFOptimized(Timestamp currentTime, TimestepDuration dt) {
        // Fast path: check refractory period first (most common case)
        if (pImpl->state.refractoryRemaining > 0) {
            handleRefractoryPeriodOptimized();
            return false;
        }
        
        // Optimized membrane potential integration
        return integrateMembranePotentialOptimized(dt);
    }
    
    // Optimized refractory period handling
    void handleRefractoryPeriodOptimized() {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;  // Clear input during refractory period
        
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
    
    // Optimized membrane potential integration
    bool integrateMembranePotentialOptimized(TimestepDuration dt) {
        auto& state = pImpl->state;
        const float timestepScaled = static_cast<float>(dt) * 1000.0f;
        
        // Pre-calculate frequently used values for optimization
        const float V = state.membranePotential;
        const float V_rest = state.restingPotential;
        const float V_reset = state.resetPotential;
        const float threshold = state.threshold;
        const float tau = NeuronConstants::DEFAULT_TIME_CONSTANT;
        const float C = NeuronConstants::DEFAULT_MEMBRANE_CAPACITANCE;
        
        // Synaptic contribution (vectorized operation)
        float synapticContribution = pImpl->synapticInput / C;
        
        // Leak contribution (vectorized operation)
        float leakContribution = (V_rest - V) / tau;
        
        // Combined update for better performance
        float totalContribution = leakContribution + synapticContribution;
        V = V + timestepScaled * totalContribution;
        
        // Apply spike-frequency adaptation
        applyAdaptationOptimized(state, V);
        
        // Clamp membrane potential to prevent instability
        state.membranePotential = std::clamp(V, NeuronConstants::MEMBRANE_CLAMP_MIN, NeuronConstants::MEMBRANE_CLAMP_MAX);
        
        // Check for spike
        if (state.membranePotential >= threshold) {
            return handleSpikeOptimized(currentTime, V_reset);
        }
        
        state.firingState = FiringState::Active;
        
        // Clear synaptic input for next step
        pImpl->synapticInput = 0.0f;
        
        return false;
    }
    
    // Optimized adaptation handling
    void applyAdaptationOptimized(NeuronState& state, float& V) {
        if (state.adaptationVariable > 0.0f) {
            V -= state.adaptationVariable * NeuronConstants::ADAPTATION_EFFECT;
            state.adaptationVariable *= NeuronConstants::ADAPTATION_DECAY_RATE;
        }
    }
    
    // Optimized spike handling
    bool handleSpikeOptimized(Timestamp currentTime, float V_reset) {
        auto& state = pImpl->state;
        
        // Update state atomically
        state.firingState = FiringState::Active;
        state.lastSpikeTime = static_cast<float>(currentTime);
        recordSpikeOptimized(currentTime);
        
        // Reset membrane potential
        state.membranePotential = V_reset;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        state.adaptationVariable += NeuronConstants::ADAPTATION_INCREMENT;
        
        return true;
    }
    
    // Optimized spike recording
    void recordSpikeOptimized(Timestamp timestamp) {
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > NeuronConstants::MAX_SPIKE_HISTORY) {
            // Use erase+begin for efficient removal of oldest spike
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
private:
    Neuron::Impl& pImpl;
};

// Neuron implementation with improved organization and error handling
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    initializeImpl(id);
}

Neuron::~Neuron() {
    delete pImpl;
}

void Neuron::initializeImpl(NeuronId id) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
    
    // Initialize with default values
    pImpl->state = NeuronState();
    pImpl->plasticityFlags = PlasticityFlags();
}

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

NeuronType Neuron::getType() const {
    return pImpl->type;
}

NeuronId Neuron::getId() const {
    return pImpl->id;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate input
    if (std::isnan(potential) || std::isinf(potential)) {
        throw std::invalid_argument("Invalid membrane potential value");
    }
    pImpl->state.membranePotential = potential;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate input
    if (std::isnan(delta) || std::isinf(delta)) {
        throw std::invalid_argument("Invalid membrane potential delta");
    }
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (std::isnan(threshold) || std::isinf(threshold)) {
        throw std::invalid_argument("Invalid threshold value");
    }
    pImpl->state.threshold = threshold;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
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
    if (steps < NeuronConstants::MIN_REFRACTORY_PERIOD) {
        pImpl->state.refractoryPeriod = NeuronConstants::MIN_REFRACTORY_PERIOD;
    } else if (steps > NeuronConstants::MAX_REFRACTORY_PERIOD) {
        pImpl->state.refractoryPeriod = NeuronConstants::MAX_REFRACTORY_PERIOD;
    } else {
        pImpl->state.refractoryPeriod = steps;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
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

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (std::isnan(conductance) || std::isinf(conductance)) {
        throw std::invalid_argument("Invalid leak conductance value");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        throw std::invalid_argument("Invalid resting potential value");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (std::isnan(potential) || std::isinf(potential)) {
        throw std::invalid_argument("Invalid reset potential value");
    }
    pImpl->state.resetPotential = potential;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    validateInput(amplitude);
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    validateInput(amplitude);
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    validateInput(amplitude);
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    validateInput(current);
    pImpl->synapticInput += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->synapticInput;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    validateTimestamp(timestamp);
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > NeuronConstants::MAX_SPIKE_HISTORY) {
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
    // Create optimizer for this neuron
    NeuronStepOptimizer optimizer(*pImpl);
    return optimizer.stepLIFOptimized(currentTime, dt);
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
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    if (!rng.isValid()) {
        throw std::invalid_argument("Invalid random generator");
    }
    
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL + 
                                     rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = NeuronConstants::DEFAULT_THRESHOLD + 
                             rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = NeuronConstants::DEFAULT_RESTING_POTENTIAL + 
                                    rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + 
                                  rng.uniformReal(0.0f, 5.0f);
    
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

// Helper validation functions
void Neuron::validateInput(MembranePotential value) {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument("Invalid input value");
    }
    if (std::abs(value) > NeuronConstants::INPUT_VALIDATION_MAX) {
        throw std::invalid_argument("Input value exceeds safety limit");
    }
}

void Neuron::validateTimestamp(Timestamp timestamp) {
    if (std::isnan(timestamp) || std::isinf(timestamp) || timestamp < 0.0) {
        throw std::invalid_argument("Invalid timestamp value");
    }
}

}} // namespace nlm