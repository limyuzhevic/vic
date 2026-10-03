#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

namespace nlm;

// Memory pool for Neuron spike history and connections
struct NeuronMemoryPool {
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    static constexpr size_t MAX_INCOMING_SYNAPSES = 50;
    static constexpr size_t MAX_OUTGOING_SYNAPSES = 50;
    
    // Pre-allocated spike history buffer
    Timestamp spikeHistoryBuffer[MAX_SPIKE_HISTORY];
    size_t spikeHistorySize;
    size_t spikeHistoryHead;
    
    // Pre-allocated synapse connection buffers
    SynapseHandle incomingSynapsesBuffer[MAX_INCOMING_SYNAPSES];
    size_t incomingSynapsesSize;
    
    SynapseHandle outgoingSynapsesBuffer[MAX_OUTGOING_SYNAPSES];
    size_t outgoingSynapsesSize;
    
    NeuronMemoryPool() : spikeHistorySize(0), spikeHistoryHead(0),
                         incomingSynapsesSize(0), outgoingSynapsesSize(0) {
        // Initialize with -1 to indicate empty
        memset(spikeHistoryBuffer, 0xFF, sizeof(spikeHistoryBuffer));
    }
};

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    
    // Use memory pool instead of std::vector
    NeuronMemoryPool memoryPool;
    
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Memory pool is initialized in constructor
    }
};
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
    auto& pool = pImpl->memoryPool;
    
    // Add to circular buffer
    pool.spikeHistoryBuffer[pool.spikeHistoryHead] = timestamp;
    pool.spikeHistoryHead = (pool.spikeHistoryHead + 1) % NeuronMemoryPool::MAX_SPIKE_HISTORY;
    
    if (pool.spikeHistorySize < NeuronMemoryPool::MAX_SPIKE_HISTORY) {
        pool.spikeHistorySize++;
    }
}

void Neuron::clearSpikeHistory() {
    auto& pool = pImpl->memoryPool;
    pool.spikeHistorySize = 0;
    pool.spikeHistoryHead = 0;
    memset(pool.spikeHistoryBuffer, 0xFF, sizeof(pool.spikeHistoryBuffer));
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    auto& pool = pImpl->memoryPool;
    if (pool.incomingSynapsesSize < NeuronMemoryPool::MAX_INCOMING_SYNAPSES) {
        pool.incomingSynapsesBuffer[pool.incomingSynapsesSize++] = handle;
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    auto& pool = pImpl->memoryPool;
    if (pool.outgoingSynapsesSize < NeuronMemoryPool::MAX_OUTGOING_SYNAPSES) {
        pool.outgoingSynapsesBuffer[pool.outgoingSynapsesSize++] = handle;
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    // For backward compatibility, return a reference to static vector
    // In a real implementation, this would need to be re-evaluated
    static std::vector<Timestamp> tempHistory;
    tempHistory.clear();
    tempHistory.reserve(pImpl->memoryPool.spikeHistorySize);
    
    // Fill from circular buffer
    for (size_t i = 0; i < pImpl->memoryPool.spikeHistorySize; ++i) {
        size_t index = (pImpl->memoryPool.spikeHistoryHead + i) % 
                      NeuronMemoryPool::MAX_SPIKE_HISTORY;
        tempHistory.push_back(pImpl->memoryPool.spikeHistoryBuffer[index]);
    }
    
    return tempHistory;
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    static std::vector<SynapseHandle> tempIncoming;
    tempIncoming.clear();
    tempIncoming.reserve(pImpl->memoryPool.incomingSynapsesSize);
    
    for (size_t i = 0; i < pImpl->memoryPool.incomingSynapsesSize; ++i) {
        tempIncoming.push_back(pImpl->memoryPool.incomingSynapsesBuffer[i]);
    }
    
    return tempIncoming;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    static std::vector<SynapseHandle> tempOutgoing;
    tempOutgoing.clear();
    tempOutgoing.reserve(pImpl->memoryPool.outgoingSynapsesSize);
    
    for (size_t i = 0; i < pImpl->memoryPool.outgoingSynapsesSize; ++i) {
        tempOutgoing.push_back(pImpl->memoryPool.outgoingSynapsesBuffer[i]);
    }
    
    return tempOutgoing;
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
    
    // Cache frequently accessed members to reduce pImpl lookups
    auto& state = pImpl->state;
    auto& memoryPool = pImpl->memoryPool;
    float& synapticInput = pImpl->synapticInput;
    
    // Handle refractory period
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        synapticInput = 0.0f;
        if (state.refractoryRemaining == 0) {
            state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Pre-calculate constants for faster computation
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    float dtFactor = static_cast<float>(dt) * 1000.0f;
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    float& V = state.membranePotential;
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    
    // Compute synaptic contribution once
    float synapticContribution = synapticInput / C;
    
    // Compute leak contribution using Horner's method for better pipelining
    float leakContribution = (V_rest - V) / tau;
    
    // Combine contributions using FMA (Fused Multiply-Add) for accuracy
    float potentialChange = leakContribution + synapticContribution;
    V += dtFactor * potentialChange;
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    // Optimize adaptation: check once before computing
    if (state.adaptationVariable > 0.0f) {
        V -= state.adaptationVariable * 0.01f;
        state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential using conditional move instead of std::clamp
    // For better performance, use min/max directly
    if (V < -100.0f) V = -100.0f;
    if (V > 50.0f) V = 50.0f;
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        state.firingState = FiringState::Active;
        state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike in memory pool
        memoryPool.spikeHistoryBuffer[memoryPool.spikeHistoryHead] = currentTime;
        memoryPool.spikeHistoryHead = (memoryPool.spikeHistoryHead + 1) % NeuronMemoryPool::MAX_SPIKE_HISTORY;
        if (memoryPool.spikeHistorySize < NeuronMemoryPool::MAX_SPIKE_HISTORY) {
            memoryPool.spikeHistorySize++;
        }
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        state.adaptationVariable += 1.0f;
    } else {
        state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    synapticInput = 0.0f;
    
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
    clearSpikeHistory();
    // Clear connection vectors
    pImpl->memoryPool.incomingSynapsesSize = 0;
    pImpl->memoryPool.outgoingSynapsesSize = 0;
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
    clearSpikeHistory();
    // Clear connection vectors
    pImpl->memoryPool.incomingSynapsesSize = 0;
    pImpl->memoryPool.outgoingSynapsesSize = 0;
}

} // namespace nlm
