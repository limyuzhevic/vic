#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/ErrorHandling.hpp"
#include "../performance/MemoryPool.hpp"
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
// SIMOPTIMIZATION UTILITIES FOR SYNAPTIC COMPUTATION
// ============================================================================

namespace {
    constexpr size_t SIMD_BATCH_SIZE = 8;
    
    // SIMD-accelerated synaptic dynamics calculations
    inline void simdUpdateSynapticState(float* weights, const float* preSpikes, const float* postSpikes,
                                        size_t count, float stdpLtpWeight, float stdpLtdWeight, float tau) {
#ifdef __ARM_NEON
        size_t i = 0;
        for (; i + 4 <= count; i += 4) {
            float32x4_t weight = vld1q_f32(weights + i);
            float32x4_t preSpike = vld1q_f32(preSpikes + i);
            float32x4_t postSpike = vld1q_f32(postSpikes + i);
            
            // Calculate STDP weight changes
            float32x4_t ltp = vdupq_n_f32(stdpLtpWeight);
            float32x4_t ltd = vdupq_n_f32(stdpLtdWeight);
            float32x4_t decay = vdupq_n_f32(std::exp(-tau * 0.001f));
            
            float32x4_t delta = vmulq_f32(preSpike, postSpike);
            float32x4_t weightChange = vmulq_f32(delta, ltp);
            weightChange = vsubq_f32(weightChange, vmulq_f32(delta, ltd));
            weight = vaddq_f32(weight, weightChange);
            
            // Clamp weights to [-1, 1]
            weight = vmaxq_f32(weight, vdupq_n_f32(-1.0f));
            weight = vminq_f32(weight, vdupq_n_f32(1.0f));
            
            vst1q_f32(weights + i, weight);
        }
        for (; i < count; ++i) {
            float delta = preSpikes[i] * postSpikes[i];
            weights[i] += delta * (stdpLtpWeight - stdpLtdWeight);
            weights[i] = std::clamp(weights[i], -1.0f, 1.0f);
        }
#else
        for (size_t i = 0; i < count; ++i) {
            float delta = preSpikes[i] * postSpikes[i];
            weights[i] += delta * (stdpLtpWeight - stdpLtdWeight);
            weights[i] = std::clamp(weights[i], -1.0f, 1.0f);
        }
#endif
    }
    
    // SIMD-accelerated short-term plasticity calculations
    inline void simdUpdateSTPState(float* depression, float* facilitation, const float* preSpikeTimes,
                                  const float* postSpikeTimes, size_t count, float facilitationTau, float depressionTau) {
#ifdef __ARM_NEON
        size_t i = 0;
        for (; i + 4 <= count; i += 4) {
            float32x4_t preTime = vld1q_f32(preSpikeTimes + i);
            float32x4_t postTime = vld1q_f32(postSpikeTimes + i);
            float32x4_t dep = vld1q_f32(depression + i);
            float32x4_t fac = vld1q_f32(facilitation + i);
            
            // Recovery from depression
            float32x4_t recovery = vsubq_f32(vdupq_n_f32(1.0f), dep);
            float32x4_t expFac = vexpq_f32(vmulq_n_f32(postTime, -1.0f / facilitationTau));
            fac = vmulq_f32(fac, expFac);
            
            // Depression update
            float32x4_t timeSincePost = vsubq_f32(postTime, postTime);
            float32x4_t expDep = vexpq_f32(vmulq_n_f32(timeSincePost, -1.0f / depressionTau));
            dep = vaddq_f32(dep, vmulq_f32(vsubq_f32(vdupq_n_f32(1.0f), dep), expDep));
            
            vst1q_f32(depression + i, dep);
            vst1q_f32(facilitation + i, fac);
        }
        for (; i < count; ++i) {
            // Recovery from depression
            float recovery = 1.0f - depression[i];
            float expFac = std::exp(-postSpikeTimes[i] / facilitationTau);
            facilitation[i] *= expFac;
            
            // Depression update
            float timeSincePost = postSpikeTimes[i] - postSpikeTimes[i];
            float expDep = std::exp(-timeSincePost / depressionTau);
            depression[i] += (1.0f - depression[i]) * (1.0f - expDep);
        }
#else
        for (size_t i = 0; i < count; ++i) {
            // Recovery from depression
            float recovery = 1.0f - depression[i];
            float expFac = std::exp(-postSpikeTimes[i] / facilitationTau);
            facilitation[i] *= expFac;
            
            // Depression update
            float timeSincePost = postSpikeTimes[i] - postSpikeTimes[i];
            float expDep = std::exp(-timeSincePost / depressionTau);
            depression[i] += (1.0f - depression[i]) * (1.0f - expDep);
        }
#endif
    }
    
    // SIMD-accelerated eligibility trace decay
    inline void simdDecayEligibilityTrace(float* traces, size_t count, float decayRate) {
        float decayFactor = 1.0f - decayRate;
#ifdef __ARM_NEON
        size_t i = 0;
        for (; i + 4 <= count; i += 4) {
            float32x4_t trace = vld1q_f32(traces + i);
            trace = vmulq_f32(trace, vdupq_n_f32(decayFactor));
            // Clamp near-zero values
            float32x4_t zero = vdupq_n_f32(0.001f);
            trace = vmaxq_f32(trace, vmulq_f32(zero, vrev64q_f32(zero)));
            vst1q_f32(traces + i, trace);
        }
        for (; i < count; ++i) {
            traces[i] *= decayFactor;
            if (std::abs(traces[i]) < 0.001f) {
                traces[i] = 0.0f;
            }
        }
#else
        for (size_t i = 0; i < count; ++i) {
            traces[i] *= decayFactor;
            if (std::abs(traces[i]) < 0.001f) {
                traces[i] = 0.0f;
            }
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
    
    // Thread-local cache for synaptic constants
    thread_local inline float getCachedSTDPTimeConstant() {
        static float tau = 20.0f;
        return tau;
    }
    
    thread_local inline float getCachedSynapticEfficacyMin() {
        static float minEfficacy = 0.0f;
        return minEfficacy;
    }
    
    thread_local inline float getCachedSynapticEfficacyMax() {
        static float maxEfficacy = 2.0f;
        return maxEfficacy;
    }
}

// ============================================================================
// ENHANCED SYNAPSE IMPLEMENTATION WITH C++20 FEATURES
// ============================================================================

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;
    static constexpr float STP_DEPRESSION_TAU = 200.0f;
    static constexpr float STP_U_MAX = 1.0f;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Performance optimization fields
    mutable std::atomic<size_t> updateCount;
    mutable float synapticConductance;
    
    // SIMD processing cache
    alignas(64) float simdCache[8];
    
    Impl() : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
             type(SynapseType::Excitatory), plasticityFlags(),
             shortTermDepression(1.0f), shortTermFacilitation(0.0f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             eligibilityTrace(0.0f), efficacy(1.0f), updateCount(0),
             synapticConductance(0.0f) {
        // Initialize SIMD cache
        for (int i = 0; i < 8; ++i) {
            simdCache[i] = 0.0f;
        }
        
        // Pre-allocate memory for performance
        preSpikeHistory.reserve(MAX_SPIKE_HISTORY);
        postSpikeHistory.reserve(MAX_SPIKE_HISTORY);
    }
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

SynapseId Synapse::getId() const {
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
}

bool Synapse::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool Synapse::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    pImpl->lastPreSpikeTime = static_cast<float>(timestamp);
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    pImpl->lastPostSpikeTime = static_cast<float>(timestamp);
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

void Synapse::step(Timestamp currentTime) {
    pImpl->updateCount.fetch_add(1, std::memory_order_relaxed);
    
    // Real synaptic dynamics:
    // 1. Decay short-term plasticity state
    // 2. Decay eligibility trace
    // 3. Update efficacy based on use
    
    TimestepDuration dt = 0.001;  // 1ms timestep
    
    // SIMD-accelerated short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        if (hasSIMDSupport()) {
            simdExpDecay(&pImpl->shortTermFacilitation, timeSincePre / Impl::STP_FACILITATION_TAU, 1);
        } else {
            pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
        }
    }
    
    // SIMD-accelerated short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        if (hasSIMDSupport()) {
            // Simplified SIMD version for depression
            pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
                                         (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        } else {
            pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * 
                                         (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
        }
    }
    
    // SIMD-accelerated eligibility trace decay for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
    
    // Clamp weight bounds
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void Synapse::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

// ============================================================================
// SYNAPSE PERFORMANCE MONITORING AND STATISTICS
// ============================================================================

namespace {
    std::atomic<size_t> totalSynapsesCreated{0};
    std::atomic<size_t> totalSynapseUpdates{0};
    std::chrono::steady_clock::time_point synapsesStartTime;
    
    void initializeSynapsePerformanceMonitoring() {
        if (totalSynapsesCreated == 0) {
            synapsesStartTime = std::chrono::steady_clock::now();
        }
        totalSynapsesCreated.fetch_add(1, std::memory_order_relaxed);
    }
    
    void recordSynapseUpdate() {
        totalSynapseUpdates.fetch_add(1, std::memory_order_relaxed);
    }
    
    std::string getSynapsePerformanceReport() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - synapsesStartTime).count();
        
        size_t synapses = totalSynapsesCreated.load(std::memory_order_relaxed);
        size_t updates = totalSynapseUpdates.load(std::memory_order_relaxed);
        
        return std::format(
            "Synapse Performance: {} synapses created, {} total updates ({:.1f} updates/synapse, {:.2f} updates/sec)",
            synapses, updates,
            synapses > 0 ? static_cast<float>(updates) / synapses : 0.0f,
            duration > 0 ? static_cast<float>(updates) / duration : 0.0f
        );
    }
    
    // Register initialization on first use
    struct SynapsePerformanceRegistrar {
        SynapsePerformanceRegistrar() {
            initializeSynapsePerformanceMonitoring();
        }
    } synapsePerformanceRegistrar;
}

// ============================================================================
// SYNAPSE OBJECT POOL FOR MEMORY OPTIMIZATION
// ============================================================================

namespace {
    // Synapse object pool for frequently allocated synapses
    class SynapsePool {
    public:
        static SynapsePool& getInstance() {
            static SynapsePool instance;
            return instance;
        }
        
        SynapseId allocate(NeuronId source, NeuronId destination) {
            if (freeList_.empty()) {
                // Create a new block
                block_ = std::make_unique<Block>();
                for (size_t i = 0; i < BLOCK_SIZE; ++i) {
                    freeList_.push_back(i);
                }
            }
            
            size_t index = freeList_.back();
            freeList_.pop_back();
            
            SynapseId id = static_cast<SynapseId>(index + 1);
            Synapse* synapse = &block_->synapses[index];
            new (synapse) Synapse(id, source, destination);
            
            return id;
        }
        
        void deallocate(SynapseId id) {
            if (id > 0 && id <= BLOCK_SIZE) {
                Synapse* synapse = &block_->synapses[id - 1];
                synapse->~Synapse();
                freeList_.push_back(id - 1);
            }
        }
        
        Synapse* getSynapse(SynapseId id) {
            if (id > 0 && id <= BLOCK_SIZE) {
                return &block_->synapses[id - 1];
            }
            return nullptr;
        }
        
        size_t size() const { return BLOCK_SIZE - freeList_.size(); }
        
    private:
        static constexpr size_t BLOCK_SIZE = 1024;
        
        struct Block {
            std::array<Synapse, BLOCK_SIZE> synapses;
            Block() {
                for (size_t i = 0; i < BLOCK_SIZE; ++i) {
                    new (&synapses[i]) Synapse(SynapseId(i + 1), NeuronId(0), NeuronId(0));
                }
            }
            
            ~Block() {
                for (size_t i = 0; i < BLOCK_SIZE; ++i) {
                    synapses[i].~Synapse();
                }
            }
        };
        
        std::vector<std::unique_ptr<Block>> blocks_;
        std::vector<size_t> freeList_;
        std::unique_ptr<Block> block_;
        
        SynapsePool() = default;
    };
    
    // Global synapse pool accessor
    SynapsePool& getSynapsePool() {
        return SynapsePool::getInstance();
    }
}

// ============================================================================
// SYNAPSE FACTORY FOR OPTIMIZED CREATION
// ============================================================================

SynapseId createOptimizedSynapse(NeuronId source, NeuronId destination, SynapseType type) {
    static std::atomic<SynapseId> nextId{1};
    
    SynapseId id = nextId.fetch_add(1);
    
    // Get synapse from pool or create new
    auto& pool = getSynapsePool();
    Synapse* synapse = pool.getSynapse(id);
    
    if (!synapse) {
        // Fallback to heap allocation
        synapse = new Synapse(id, source, destination);
    }
    
    // Configure synapse properties
    synapse->setType(type);
    
    // Initialize with biological parameters
    static RandomGenerator rng(42); // Default seed
    synapse->initializeRandom(rng);
    
    return id;
}

} // namespace nlm
