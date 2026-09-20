#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <mutex>
#include <atomic>

namespace nlm {

// Thread-safe circular buffer for spike history with O(1) operations
class CircularBuffer {
public:
    explicit CircularBuffer(size_t capacity) : head(0), count(0), data(new Timestamp[capacity]) {}
    ~CircularBuffer() { delete[] data; }
    
    // Push new element - automatically overwrites oldest when full
    void push(Timestamp value) {
        data[head] = value;
        head = (head + 1) % capacity;
        if (count < capacity) {
            ++count;
        }
    }
    
    // Access element by index (0 = oldest, size()-1 = newest)
    Timestamp operator[](size_t index) const {
        size_t idx = (head + index) % capacity;
        return data[idx];
    }
    
    // Get current size (<= capacity)
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
    void clear() {
        head = 0;
        count = 0;
    }
    
    // For compatibility with existing API
    Timestamp front() const { return (*this)[0]; }
    const Timestamp* data() const { return data; }
    const Timestamp* begin() const { return data; }
    const Timestamp* end() const { return data + count; }

private:
    size_t head;
    size_t count;
    Timestamp* data;
    const size_t capacity = MAX_SPIKE_HISTORY;
};

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;
    MembranePotential synapticInput;
    CircularBuffer spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    mutable std::mutex mutex;
    std::atomic<uint64_t> accessCount;
    
    // constexpr static constants for parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Default constructor
    Impl() noexcept : id(), type(NeuronType::Internal), regionId(), populationId(),
                      totalCurrent(0.0f), synapticInput(0.0f), spikeHistory(MAX_SPIKE_HISTORY),
                      accessCount(0) {}
    
    // Delete copy operations for exclusive ownership semantics
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    
    // Move constructor - enable safe transfer of ownership
    Impl(Impl&& other) noexcept : id(std::move(other.id)),
                                 type(std::move(other.type)),
                                 state(std::move(other.state)),
                                 regionId(std::move(other.regionId)),
                                 populationId(std::move(other.populationId)),
                                 totalCurrent(std::move(other.totalCurrent)),
                                 synapticInput(std::move(other.synapticInput)),
                                 spikeHistory(std::move(other.spikeHistory)),
                                 incomingSynapses(std::move(other.incomingSynapses)),
                                 outgoingSynapses(std::move(other.outgoingSynapses)),
                                 plasticityFlags(std::move(other.plasticityFlags)),
                                 accessCount(other.accessCount.load(std::memory_order_relaxed)) {}
    
    // Move assignment operator
    Impl& operator=(Impl&& other) noexcept {
        if (this != &other) {
            id = std::move(other.id);
            type = std::move(other.type);
            state = std::move(other.state);
            regionId = std::move(other.regionId);
            populationId = std::move(other.populationId);
            totalCurrent = std::move(other.totalCurrent);
            synapticInput = std::move(other.synapticInput);
            spikeHistory = std::move(other.spikeHistory);
            incomingSynapses = std::move(other.incomingSynapses);
            outgoingSynapses = std::move(other.outgoingSynapses);
            plasticityFlags = std::move(other.plasticityFlags);
            accessCount = other.accessCount.load(std::memory_order_relaxed);
        }
        return *this;
    }
    
    // Safe increment for access tracking
    void incrementAccess() const noexcept {
        accessCount.fetch_add(1, std::memory_order_relaxed);
    }
};

// Neuron implementation with thread safety and modern C++ patterns
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

// Identity accessor
NeuronId Neuron::getId() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->id;
}

// Type access
NeuronType Neuron::getType() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->type = type;
}

// State access with thread safety
const NeuronState& Neuron::getState() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state;
}

// Membrane potential
MembranePotential Neuron::getMembranePotential() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.membranePotential += delta;
}

// Threshold
MembranePotential Neuron::getThreshold() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.threshold = threshold;
}

// Firing state
bool Neuron::isFiring() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.firingState == FiringState::Active ||
           pImpl->state.firingState == FiringState::Refractory;
}

bool Neuron::isRefractory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.refractoryPeriod = steps;
}

void Neuron::decrementRefractory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

// Firing rate (for rate-based computation)
FiringRate Neuron::getFiringRate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.firingRate = rate;
}

// LIF neuron parameters
void Neuron::setLeakConductance(MembranePotential conductance) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.leakConductance;
}

uint32_t Neuron::getRefractoryPeriod() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.resetPotential;
}

// Spike detection
bool Neuron::checkThreshold() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->state.lastSpikeTime;
}

// Incoming signals (post-synaptic potentials)
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

// Current injection (from external sources)
void Neuron::injectCurrent(MembranePotential current) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    pImpl->synapticInput += current;
}

MembranePotential Neuron::getTotalCurrent() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->synapticInput;
}

void Neuron::clearTotalCurrent() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->synapticInput = 0.0f;
}

// Spike history with thread-safe circular buffer
void Neuron::recordSpike(Timestamp timestamp) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    pImpl->spikeHistory.push(timestamp);
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    // Convert circular buffer to vector for API compatibility
    static thread_local std::vector<Timestamp> buffer;
    buffer.clear();
    buffer.reserve(pImpl->spikeHistory.size());
    for (size_t i = 0; i < pImpl->spikeHistory.size(); ++i) {
        buffer.push_back(pImpl->spikeHistory[i]);
    }
    return buffer;
}

void Neuron::clearSpikeHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->spikeHistory.clear();
}

// Synapse management with thread safety
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->outgoingSynapses;
}

// Plasticity state with thread safety
const PlasticityFlags& Neuron::getPlasticityFlags() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

// Region/population membership
void Neuron::setRegionId(RegionId region) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    return pImpl->populationId;
}

// Enhanced thread-safe LIF step function with bounds checking
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementAccess();
    
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
    
    // Bounds checking for dt
    const float maxDt = 100.0f; // max timestep
    const float minDt = 0.0001f; // min timestep
    float safeDt = std::clamp(static_cast<float>(dt), minDt, maxDt);
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration with safe dt
    V = V + safeDt * 1000.0f * (leakContribution + synapticContribution);
    
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
        
        // Record spike using circular buffer
        pImpl->spikeHistory.push(currentTime);
        
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

// Reset to initial state with thread safety
void Neuron::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

// Initialize with random parameters with thread safety
void Neuron::initializeRandom(RandomGenerator& rng) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
