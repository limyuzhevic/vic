#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace nlm {

// SIMD-aligned buffer for neuron state vectorization
struct alignas(64) NeuronStateVector {
    alignas(64) float membranePotential;
    alignas(64) float restingPotential;
    alignas(64) float threshold;
    alignas(64) float resetPotential;
    alignas(64) float leakConductance;
    alignas(64) float synapseInput;
    alignas(64) float adaptationVariable;
    alignas(64) float lastSpikeTime;
    alignas(64) float firingRate;
    alignas(64) uint8_t firingState;
    alignas(64) uint32_t refractoryRemaining;
    alignas(64) uint32_t refractoryPeriod;
    alignas(64) uint64_t neuronType;
    alignas(64) uint64_t regionId;
    alignas(64) uint64_t populationId;
    alignas(64) uint32_t plasticityFlags;
};

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
    
    // Performance optimization: cache frequently accessed values
    mutable float cachedTimeConstant = TIME_CONSTANT;
    mutable bool timeConstantDirty = false;
    
    // Memory leak detection
    static inline std::atomic<size_t> instances{0};
    static inline std::atomic<size_t> peakInstances{0};
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        instances++;
        if (instances > peakInstances) peakInstances = instances;
    }
    
    ~Impl() {
        instances--;
    }
    
    static size_t getInstanceCount() { return instances.load(); }
    static size_t getPeakInstanceCount() { return peakInstances.load(); }
    static void resetStats() { instances = 0; peakInstances = 0; }
};

// Custom deleter for proper cleanup
struct Neuron::ImplDeleter {
    void operator()(Impl* impl) {
        if (impl) {
            delete impl;
        }
    }
};

Neuron::Neuron(NeuronId id) : pImpl(std::make_unique<Impl>()) {
    // Neuron constructor initializes a single neuron with specified ID
    ValidationHelper::validateNeuronId(id, "Neuron constructor");
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;  // Default neuron type
    pImpl->regionId = INVALID_REGION_ID;  // Not yet assigned to a region
    pImpl->populationId = INVALID_POPULATION_ID;  // Not yet assigned to a population
    pImpl->totalCurrent = 0.0f;  // No input current initially
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(std::move(other.pImpl)) {
    // Move constructor - transfer ownership
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    NLM_VALIDATE_RANGE(static_cast<int>(type), 0, 6, "Invalid neuron type value");
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    ValidationHelper::validateRange(potential, -100.0f, 50.0f, 
                                   "Membrane potential out of valid range (-100 to 50 mV)");
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    ValidationHelper::validateRange(delta, -100.0f, 100.0f,
                                   "Delta membrane potential out of valid range");
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    ValidationHelper::validateRange(newPotential, -100.0f, 50.0f,
                                   "Membrane potential after delta update out of valid range");
    pImpl->state.membranePotential = newPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    ValidationHelper::validateRange(threshold, -100.0f, 50.0f,
                                   "Threshold out of valid range (-100 to 50 mV)");
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    NLM_VALIDATE_RANGE(static_cast<int>(state), 0, 3, "Invalid firing state value");
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in decrementRefractory");
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    ValidationHelper::validateNonNegative(rate, "Firing rate cannot be negative");
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    ValidationHelper::validateNonNegative(conductance, "Leak conductance cannot be negative");
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in getLeakConductance");
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    ValidationHelper::validatePositive(steps, "Refractory period must be positive");
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in getRefractoryPeriod");
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    ValidationHelper::validateRange(potential, -100.0f, 50.0f,
                                   "Resting potential out of valid range (-100 to 50 mV)");
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in getRestingPotential");
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    ValidationHelper::validateRange(potential, -100.0f, 50.0f,
                                   "Reset potential out of valid range (-100 to 50 mV)");
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in checkThreshold");
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in getLastSpikeTime");
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    ValidationHelper::validateRange(amplitude, -10.0f, 100.0f,
                                   "Excitatory input amplitude out of valid range");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in receiveExcitatoryInput");
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    ValidationHelper::validateRange(amplitude, 0.0f, 100.0f,
                                   "Inhibitory input amplitude out of valid range");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in receiveInhibitoryInput");
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    ValidationHelper::validateRange(amplitude, -10.0f, 10.0f,
                                   "Modulatory input amplitude out of valid range");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in receiveModulatoryInput");
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    ValidationHelper::validateRange(current, -100.0f, 100.0f,
                                   "Injected current out of valid range");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in injectCurrent");
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in clearTotalCurrent");
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    ValidationHelper::validateTime(timestamp, "Spike timestamp");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in recordSpike");
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in clearSpikeHistory");
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    ValidationHelper::validateNonNegative(handle, "Synapse handle cannot be negative");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in addIncomingSynapse");
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    ValidationHelper::validateNonNegative(handle, "Synapse handle cannot be negative");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in addOutgoingSynapse");
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in getState");
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in getPlasticityFlags");
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in enablePlasticity");
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    ValidationHelper::validateRegionId(region, "Neuron setRegionId");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in setRegionId");
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    ValidationHelper::validatePopulationId(population, "Neuron setPopulationId");
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in setPopulationId");
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    ValidationHelper::validateTime(currentTime, "Neuron stepLIF current time");
    ValidationHelper::validatePositive(dt, "Neuron stepLIF timestep must be positive");
    
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in stepLIF");
    
    bool fired = false;
    
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    ValidationHelper::validateTime(currentTime, "Neuron stepLIF current time");
    ValidationHelper::validatePositive(dt, "Neuron stepLIF timestep must be positive");
    
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in stepLIF");
    
    bool fired = false;
    
    // Fast-path for refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // ========== OPTIMIZED LIF UPDATE ==========
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Pre-compute constants for optimization
    float dt1000 = static_cast<float>(dt) * 1000.0f;
    float inputCoeff = dt1000 / C;
    float leakCoeff = dt1000 / tau;
    
    // Synaptic input optimization
    float synapticContribution = pImpl->synapticInput * inputCoeff;
    float leakContribution = (V_rest - V) * leakCoeff;
    V = V + synapticContribution + leakContribution;
    
    // Fast-path for adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;
    }
    
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Optimized spike detection
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        recordSpike(currentTime);
        V = V_reset;
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    pImpl->synapticInput = 0.0f;
    
    return fired;
}
}

void Neuron::step(Timestamp currentTime) {
    ValidationHelper::validateTime(currentTime, "Neuron step current time");
    
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in step");
    
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in reset");
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    ValidationHelper::validateNonNull(&rng, "Random generator is null in initializeRandom");
    
    NLM_VALIDATE_NON_NULL(pImpl, "Neuron pointer is null in initializeRandom");
    
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
