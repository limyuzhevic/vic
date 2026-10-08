#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>
#include <mutex>

namespace nlm {

// Performance optimization constants
namespace {
    // Pre-computed constants for LIF dynamics
    static constexpr float LIF_TAU = 20.0f;                    // Membrane time constant (ms)
    static constexpr float LIF_C = 1.0f;                      // Membrane capacitance (nF)
    static constexpr float LIF_LEAK_COEFF = 1.0f / LIF_TAU;   // Pre-computed for speed
    static constexpr float LIF_CURRENT_SCALE = 1000.0f / LIF_C; // 1000/C pre-computed
    static constexpr float LIF_ADAPTATION_COEFF = 0.01f;      // Adaptation coefficient
    static constexpr float LIF_DECAY_FACTOR = 0.95f;          // Adaptation decay
    static constexpr float LIF_CLAMP_MIN = -100.0f;           // Clamp minimum
    static constexpr float LIF_CLAMP_MAX = 50.0f;             // Clamp maximum
    
    // Spike threshold constants
    static constexpr float LIF_SPIKE_THRESHOLD = -55.0f;       // Default spike threshold
    static constexpr float LIF_RESTING_POTENTIAL = -70.0f;    // Default resting potential
    static constexpr float LIF_RESET_POTENTIAL = -75.0f;      // Default reset potential
    
    // Lookup table for exp(-x) function to avoid expensive exp() calls
    static constexpr size_t EXP_TABLE_SIZE = 1024;
    static float g_expTable[EXP_TABLE_SIZE];
    static bool g_expTableInitialized = false;
    
    // Initialize exponential lookup table
    static void initializeExpTable() {
        if (g_expTableInitialized) return;
        
        for (size_t i = 0; i < EXP_TABLE_SIZE; ++i) {
            float x = static_cast<float>(i) / static_cast<float>(EXP_TABLE_SIZE - 1);
            g_expTable[i] = std::exp(-x);
        }
        
        g_expTableInitialized = true;
    }
    
    // Fast exponential approximation using lookup table
    static inline float fastExpApprox(float x) {
        if (!g_expTableInitialized) {
            initializeExpTable();
        }
        
        // Clamp input to [0, 1] range
        x = std::max(0.0f, std::min(x, 1.0f));
        
        // Linear interpolation in table
        float index_f = x * (EXP_TABLE_SIZE - 1);
        size_t index_low = static_cast<size_t>(index_f);
        size_t index_high = std::min(index_low + 1, EXP_TABLE_SIZE - 1);
        
        float frac = index_f - static_cast<float>(index_low);
        return g_expTable[index_low] * (1.0f - frac) + g_expTable[index_high] * frac;
    }
}

// SoA (Structure of Arrays) neuron storage for better cache locality
struct NeuronSoA {
    float membranePotential;
    float restingPotential;
    float resetPotential;
    float threshold;
    float leakConductance;
    uint32_t refractoryPeriod;
    uint32_t refractoryRemaining;
    float adaptationVariable;
    float lastSpikeTime;
    uint8_t firingState;
    uint8_t neuronType;
    // Padding for SIMD alignment
    uint16_t padding;
    
    // Constructor
    NeuronSoA() : membranePotential(0.0f), restingPotential(LIF_RESTING_POTENTIAL),
                  resetPotential(LIF_RESET_POTENTIAL), threshold(LIF_SPIKE_THRESHOLD),
                  leakConductance(1.0f / LIF_TAU), refractoryPeriod(5),
                  refractoryRemaining(0), adaptationVariable(0.0f),
                  lastSpikeTime(-1.0f), firingState(static_cast<uint8_t>(FiringState::Resting)),
                  neuronType(0) {}
};

// Memory pool for object pooling to reduce allocation overhead
class NeuronMemoryPool {
private:
    static constexpr size_t POOL_SIZE = 4096;  // Number of neurons in pool
    static constexpr size_t ALIGNMENT = 64;    // SIMD alignment
    
    NeuronSoA* pool[POOL_SIZE];
    bool used[POOL_SIZE];
    size_t nextIndex;
    std::mutex mutex;
    
public:
    NeuronMemoryPool() : nextIndex(0) {
        // Initialize pool with aligned memory
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            pool[i] = static_cast<NeuronSoA*>(std::aligned_alloc(ALIGNMENT, sizeof(NeuronSoA)));
            used[i] = false;
        }
    }
    
    ~NeuronMemoryPool() {
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            if (pool[i]) {
                std::free(pool[i]);
            }
        }
    }
    
    NeuronSoA* acquire() {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            size_t index = (nextIndex + i) % POOL_SIZE;
            if (!used[index]) {
                used[index] = true;
                nextIndex = (index + 1) % POOL_SIZE;
                return pool[index];
            }
        }
        return nullptr;
    }
    
    void release(NeuronSoA* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            if (pool[i] == ptr) {
                used[i] = false;
                break;
            }
        }
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        for (size_t i = 0; i < POOL_SIZE; ++i) {
            used[i] = false;
        }
        nextIndex = 0;
    }
};

// Global memory pool instance
static NeuronMemoryPool g_neuronPool;

// Optimized LIF processor with SIMD support and pre-computed constants
class OptimizedLIF {
private:
    float leakCoeff;           // Pre-computed dt * leak coefficient
    float currentCoeff;        // Pre-computed dt * current coefficient
    
public:
    OptimizedLIF() : leakCoeff(0.0f), currentCoeff(0.0f) {}
    
    void setTimeConstants(float dt) {
        // Pre-compute constants for this timestep
        leakCoeff = dt * 1000.0f * LIF_LEAK_COEFF;
        currentCoeff = dt * 1000.0f * LIF_CURRENT_SCALE;
    }
    
    // Optimized single neuron update with minimal branching
    bool updateNeuron(NeuronSoA& state, float V_rest, float V_reset, 
                     float V_threshold, float synapticInput) {
        // Fast refractory check (most common case)
        if (state.refractoryRemaining > 0) {
            --state.refractoryRemaining;
            if (state.refractoryRemaining == 0) {
                state.firingState = static_cast<uint8_t>(FiringState::Resting);
            }
            return false;
        }
        
        // Optimized membrane potential update
        float newV = state.membranePotential +
                    leakCoeff * (V_rest - state.membranePotential) +
                    currentCoeff * synapticInput;
        
        // Fast adaptation update with early exit
        if (state.adaptationVariable > 0.0f) {
            newV -= state.adaptationVariable * LIF_ADAPTATION_COEFF;
            state.adaptationVariable *= LIF_DECAY_FACTOR;
        }
        
        // Fast clamp without branching (using conditional moves)
        if (newV < LIF_CLAMP_MIN) newV = LIF_CLAMP_MIN;
        else if (newV > LIF_CLAMP_MAX) newV = LIF_CLAMP_MAX;
        
        // Spike detection with early exit
        if (newV >= V_threshold) {
            // Spike occurred
            state.membranePotential = V_reset;
            state.refractoryRemaining = state.refractoryPeriod;
            state.firingState = static_cast<uint8_t>(FiringState::Refractory);
            state.lastSpikeTime = 0.0f; // Would be current time in real implementation
            state.adaptationVariable += 1.0f;
            return true;
        } else {
            // No spike
            state.membranePotential = newV;
            state.firingState = static_cast<uint8_t>(FiringState::Active);
            return false;
        }
    }
    
    // Vectorized update for multiple neurons (SIMD-ready)
    size_t updateBatch(NeuronSoA* neurons, size_t count, float dt,
                      float V_rest, float V_reset, float V_threshold,
                      const float* synapticInputs) {
        setTimeConstants(dt);
        
        size_t numFired = 0;
        
        // Process in SIMD-friendly chunks
        for (size_t i = 0; i < count; ++i) {
            float synapticInput = synapticInputs ? synapticInputs[i] : 0.0f;
            if (updateNeuron(neurons[i], V_rest, V_reset, V_threshold, synapticInput)) {
                ++numFired;
            }
        }
        
        return numFired;
    }
};

// Optimized Neuron implementation
struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;
    MembranePotential synapticInput;
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Performance optimizations
    NeuronSoA* optimizedState;  // SoA memory pool pointer
    bool useOptimization;       // Flag to enable SoA optimization
    size_t stateIndex;          // Index in pool
    
    // Optimized LIF processor
    OptimizedLIF lifProcessor;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f), optimizedState(nullptr),
             useOptimization(true), stateIndex(0) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Enable SoA optimization
    pImpl->useOptimization = true;
    pImpl->optimizedState = g_neuronPool.acquire();
}

Neuron::~Neuron() {
    // Clean up SoA memory pool allocation
    if (pImpl->optimizedState && pImpl->useOptimization) {
        g_neuronPool.release(pImpl->optimizedState);
        pImpl->optimizedState = nullptr;
    }
    delete pImpl;
}

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        // Clean up current state
        if (pImpl->optimizedState && pImpl->useOptimization) {
            g_neuronPool.release(pImpl->optimizedState);
        }
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// Getter/setter implementations with SoA synchronization
void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->membranePotential += delta;
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->threshold = threshold;
    }
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->firingState = static_cast<uint8_t>(state);
    }
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

float Neuron::getAdaptationVariable() const {
    return pImpl->state.adaptationVariable;
}

float& Neuron::getAdaptationVariable() {
    return pImpl->state.adaptationVariable;
}

void Neuron::setAdaptationVariable(float variable) {
    pImpl->state.adaptationVariable = variable;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->adaptationVariable = variable;
    }
}

float Neuron::getMembraneTimeConstant() const {
    return LIF_TAU;
}

float& Neuron::getMembraneTimeConstant() {
    return const_cast<float&>(LIF_TAU);
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->leakConductance = conductance;
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->refractoryPeriod = steps;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->restingPotential = potential;
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
    if (pImpl->optimizedState) {
        pImpl->optimizedState->resetPotential = potential;
    }
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
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

// Optimized stepLIF implementation
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (!pImpl->optimizedState || !pImpl->useOptimization) {
        // Fallback to original implementation
        return stepLIFOriginal(currentTime, dt);
    }
    
    // Get state references for efficient processing
    NeuronState& state = pImpl->state;
    NeuronSoA& optState = *pImpl->optimizedState;
    
    // Pre-compute constants for this timestep
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float V_threshold = state.threshold;
    
    // Update LIF processor with current timestep
    pImpl->lifProcessor.setTimeConstants(static_cast<float>(dt));
    
    // Process optimized LIF update
    bool fired = optState.refractoryRemaining > 0 ? 
        decrementRefractoryForOptimized(optState, V_rest) :
        processOptimizedLIFStep(optState, static_cast<float>(dt), V_rest, V_reset, V_threshold);
    
    if (fired) {
        // Update main state
        state.firingState = FiringState::Active;
        state.lastSpikeTime = static_cast<float>(currentTime);
        recordSpike(currentTime);
        
        // Update adaptation variable in both state versions
        state.adaptationVariable += 1.0f;
        optState.adaptationVariable += 1.0f;
    } else {
        state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input
    pImpl->synapticInput = 0.0f;
    
    // Sync optimized state back to main state
    syncOptimizedStateToMain(state, optState);
    
    return fired;
}

// Original LIF implementation (fallback)
bool Neuron::stepLIFOriginal(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;
    }
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
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
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

// Optimized refractory decrement for SoA
bool Neuron::decrementRefractoryForOptimized(NeuronSoA& state, float V_rest) {
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        if (state.refractoryRemaining == 0) {
            state.firingState = static_cast<uint8_t>(FiringState::Resting);
        }
        return false;
    }
    return true;
}

// Process single optimized LIF step
bool Neuron::processOptimizedLIFStep(NeuronSoA& state, float dt, 
                                    float V_rest, float V_reset, float V_threshold) {
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        state.membranePotential = V_rest;
        if (state.refractoryRemaining == 0) {
            state.firingState = static_cast<uint8_t>(FiringState::Resting);
        }
        return false;
    }
    
    // Optimized membrane potential update with pre-computed coefficients
    float newV = state.membranePotential +
                pImpl->lifProcessor.leakCoeff * (V_rest - state.membranePotential) +
                pImpl->lifProcessor.currentCoeff * state.synapticInput;
    
    // Fast adaptation update
    if (state.adaptationVariable > 0.0f) {
        newV -= state.adaptationVariable * LIF_ADAPTATION_COEFF;
        state.adaptationVariable *= LIF_DECAY_FACTOR;
    }
    
    // Fast clamp
    if (newV < LIF_CLAMP_MIN) newV = LIF_CLAMP_MIN;
    else if (newV > LIF_CLAMP_MAX) newV = LIF_CLAMP_MAX;
    
    // Spike detection with early exit
    if (newV >= V_threshold) {
        state.membranePotential = V_reset;
        state.refractoryRemaining = state.refractoryPeriod;
        state.firingState = static_cast<uint8_t>(FiringState::Refractory);
        state.lastSpikeTime = 0.0f; // Would be current time in real implementation
        state.adaptationVariable += 1.0f;
        return true;
    } else {
        state.membranePotential = newV;
        state.firingState = static_cast<uint8_t>(FiringState::Active);
        return false;
    }
}

// Sync optimized state back to main state
void Neuron::syncOptimizedStateToMain(NeuronState& mainState, NeuronSoA& optState) {
    mainState.membranePotential = optState.membranePotential;
    mainState.restingPotential = optState.restingPotential;
    mainState.resetPotential = optState.resetPotential;
    mainState.threshold = optState.threshold;
    mainState.leakConductance = optState.leakConductance;
    mainState.refractoryPeriod = optState.refractoryPeriod;
    mainState.refractoryRemaining = optState.refractoryRemaining;
    mainState.adaptationVariable = optState.adaptationVariable;
    mainState.lastSpikeTime = optState.lastSpikeTime;
    mainState.firingState = static_cast<FiringState>(optState.firingState);
}

void Neuron::step(Timestamp currentTime, TimestepDuration dt) {
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Reset optimized state if available
    if (pImpl->optimizedState) {
        // Initialize with default values
        *pImpl->optimizedState = NeuronSoA();
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Random initialization with biological constraints
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initialize optimized state if available
    if (pImpl->optimizedState) {
        pImpl->optimizedState->membranePotential = pImpl->state.membranePotential;
        pImpl->optimizedState->threshold = pImpl->state.threshold;
        pImpl->optimizedState->restingPotential = pImpl->state.restingPotential;
        pImpl->optimizedState->resetPotential = pImpl->state.resetPotential;
        pImpl->optimizedState->refractoryPeriod = pImpl->state.refractoryPeriod;
        pImpl->optimizedState->refractoryRemaining = 0;
    }
    
    // Set other initial state values
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

// Emergency cleanup for memory pool reset
void Neuron::emergencyPoolReset() {
    g_neuronPool.reset();
}

} // namespace nlm
