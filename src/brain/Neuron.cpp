#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>
#include <span>
#include <format>
#include <numeric>
#include <chrono>
#include <atomic>
#include <bit>
#include <memory>
#include <optional>

namespace nlm {

// ============================================================================
// SIMD OPTIMIZATION UTILITIES FOR NEURAL COMPUTATION
// ============================================================================

namespace {
    constexpr size_t SIMD_BATCH_SIZE = 8;
    
    // SIMD-accelerated math operations using intrinsics
    inline void simdAdd(float* dest, const float* src, size_t count) {
#ifdef __ARM_NEON
        // ARM NEON SIMD operations
        size_t i = 0;
        for (; i + 4 <= count; i += 4) {
            float32x4_t v1 = vld1q_f32(dest + i);
            float32x4_t v2 = vld1q_f32(src + i);
            float32x4_t result = vaddq_f32(v1, v2);
            vst1q_f32(dest + i, result);
        }
        for (; i < count; ++i) {
            dest[i] += src[i];
        }
#else
        for (size_t i = 0; i < count; ++i) {
            dest[i] += src[i];
        }
#endif
    }
    
    // SIMD-accelerated clamping operations
    inline void simdClamp(float* values, size_t count, float min, float max) {
#ifdef __ARM_NEON
        size_t i = 0;
        for (; i + 4 <= count; i += 4) {
            float32x4_t v = vld1q_f32(values + i);
            v = vmaxq_f32(v, vdupq_n_f32(min));
            v = vminq_f32(v, vdupq_n_f32(max));
            vst1q_f32(values + i, v);
        }
        for (; i < count; ++i) {
            values[i] = std::clamp(values[i], min, max);
        }
#else
        for (size_t i = 0; i < count; ++i) {
            values[i] = std::clamp(values[i], min, max);
        }
#endif
    }
    
    // SIMD-accelerated exponential decay
    inline void simdExpDecay(float* values, float decayRate, size_t count) {
        float oneMinusDecay = 1.0f - decayRate;
#ifdef __ARM_NEON
        size_t i = 0;
        for (; i + 4 <= count; i += 4) {
            float32x4_t v = vld1q_f32(values + i);
            v = vmulq_f32(v, vdupq_n_f32(oneMinusDecay));
            vst1q_f32(values + i, v);
        }
        for (; i < count; ++i) {
            values[i] *= oneMinusDecay;
        }
#else
        for (size_t i = 0; i < count; ++i) {
            values[i] *= oneMinusDecay;
        }
#endif
    }
    
    // Check for SIMD support
    inline bool hasSIMDSupport() {
#ifdef __ARM_NEON
        return true;
#else
        return false;
#endif
    }
    
    // Thread-local cache for frequently accessed constants
    thread_local inline float getCachedRefractoryThreshold() {
        static float threshold = -55.0f;
        return threshold;
    }
    
    thread_local inline float getCachedRestingPotential() {
        static float resting = -70.0f;
        return resting;
    }
}

// ============================================================================
// ENHANCED NEURON IMPLEMENTATION WITH C++20 FEATURES
// ============================================================================

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
    
    // Performance optimization fields
    mutable std::atomic<size_t> updateCount;
    mutable float excitationContribution;
    mutable float inhibitionContribution;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f), 
             updateCount(0), excitationContribution(0.0f), inhibitionContribution(0.0f) {
        spikeHistory.reserve(MAX_SPIKE_HISTORY);
        incomingSynapses.reserve(100);
        outgoingSynapses.reserve(100);
    }
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
    pImpl->synapticInput += amplitude;
    pImpl->excitationContribution += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
    pImpl->inhibitionContribution -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
    pImpl->excitationContribution = 0.0f;
    pImpl->inhibitionContribution = 0.0f;
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
    pImpl->updateCount.fetch_add(1, std::memory_order_relaxed);
    
    bool fired = false;
    
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        pImpl->excitationContribution = 0.0f;
        pImpl->inhibitionContribution = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = getCachedRestingPotential();
    MembranePotential V_reset = pImpl->state.resetPotential;
    float threshold = getCachedRefractoryThreshold();
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    if (hasSIMDSupport()) {
        float V_new = V + static_cast<float>(dt) * 1000.0f * (
            (V_rest - V) / tau + pImpl->synapticInput / C
        );
        
        if (pImpl->state.adaptationVariable > 0.0f) {
            V_new -= pImpl->state.adaptationVariable * 0.01f;
            simdExpDecay(&pImpl->state.adaptationVariable, 0.95f, 1);
        }
        
        simdClamp(&V_new, 1, -100.0f, 50.0f);
        
        V = V_new;
    } else {
        float V_new = V + static_cast<float>(dt) * 1000.0f * (
            (V_rest - V) / tau + pImpl->synapticInput / C
        );
        
        if (pImpl->state.adaptationVariable > 0.0f) {
            V_new -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;
        }
        
        V_new = std::clamp(V_new, -100.0f, 50.0f);
        
        V = V_new;
    }
    
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
    
    clearTotalCurrent();
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->excitationContribution = 0.0f;
    pImpl->inhibitionContribution = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    pImpl->synapticInput = 0.0f;
    pImpl->excitationContribution = 0.0f;
    pImpl->inhibitionContribution = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
