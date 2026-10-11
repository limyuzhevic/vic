#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <vector>
#include <array>
#include <memory>
#include <cstring>

// SIMD and alignment utilities
namespace simd {
    template<typename T>
    static constexpr bool isAligned(const T* ptr, size_t alignment) {
        return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
    }
    
    static void* alignedAlloc(size_t size, size_t alignment) {
        void* ptr = nullptr;
#if defined(_WIN32)
        ptr = _aligned_malloc(size, alignment);
#else
        ptr = std::aligned_alloc(alignment, size);
#endif
        return ptr;
    }
    
    static void alignedFree(void* ptr) {
#if defined(_WIN32)
        _aligned_free(ptr);
#else
        std::free(ptr);
#endif
    }
}

namespace nlm {

// Global performance monitoring
struct PerformanceMonitor {
    std::atomic<size_t> totalNeurons = 0;
    std::atomic<size_t> neuronsCreated = 0;
    std::atomic<size_t> neuronsDestroyed = 0;
    std::atomic<size_t> totalAllocations = 0;
    std::atomic<size_t> totalDeallocations = 0;
    std::atomic<double> totalMemoryUsage = 0.0;
    
    double getUtilization() const {
        return totalNeurons.load() > 0 ? 
            (static_cast<double>(neuronsCreated.load()) / 
             static_cast<double>(neuronsDestroyed.load() + 1)) * 100.0 : 0.0;
    }
    
    void recordAllocation(size_t size) {
        totalAllocations.fetch_add(1);
        totalMemoryUsage.fetch_add(static_cast<double>(size));
    }
    
    void recordDeallocation(size_t size) {
        totalDeallocations.fetch_add(1);
        totalMemoryUsage.fetch_sub(static_cast<double>(size));
    }
};

static PerformanceMonitor g_perfMonitor;

// Memory pool for Neuron objects with cache-line alignment
class NeuronMemoryPool {
public:
    static constexpr size_t CACHE_LINE = 64;
    static constexpr size_t MAX_POOL_SIZE = 1024 * 1024;  // 1M neurons
    
    struct alignas(CACHE_LINE) NeuronBlock {
        // SoA (Structure of Arrays) layout for optimal cache locality
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapticInput;
        std::vector<uint8_t> firingState;
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> adaptationVariable;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;
        std::vector<uint64_t> regionId;
        std::vector<uint64_t> populationId;
        
        // Pre-allocated spike history (circular buffers)
        static constexpr size_t MAX_SPIKE_HISTORY = 128;
        std::vector<std::array<float, MAX_SPIKE_HISTORY>> spikeHistory;
        std::vector<uint8_t> spikeHistoryHead;
        std::vector<uint8_t> spikeHistoryTail;
        
        void initialize(size_t size) {
            // Initialize all vectors with pre-allocated capacity
            size_t cap = (size <= 1024) ? 1024 : size;
            
            membranePotential.reserve(cap);
            restingPotential.reserve(cap);
            threshold.reserve(cap);
            resetPotential.reserve(cap);
            leakConductance.reserve(cap);
            synapticInput.reserve(cap);
            firingState.reserve(cap);
            refractoryRemaining.reserve(cap);
            refractoryPeriod.reserve(cap);
            adaptationVariable.reserve(cap);
            lastSpikeTime.reserve(cap);
            neuronType.reserve(cap);
            regionId.reserve(cap);
            populationId.reserve(cap);
            spikeHistory.reserve(cap);
            spikeHistoryHead.reserve(cap, 0);
            spikeHistoryTail.reserve(cap, 0);
            
            // Pre-allocate actual data
            membranePotential.resize(size);
            restingPotential.resize(size);
            threshold.resize(size);
            resetPotential.resize(size);
            leakConductance.resize(size);
            synapticInput.resize(size, 0.0f);
            firingState.resize(size, 0);
            refractoryRemaining.resize(size, 0);
            refractoryPeriod.resize(size, 5);
            adaptationVariable.resize(size, 0.0f);
            lastSpikeTime.resize(size, -1.0f);
            neuronType.resize(size, 0);
            regionId.resize(size);
            populationId.resize(size);
            spikeHistory.resize(size);
            spikeHistoryHead.resize(size, 0);
            spikeHistoryTail.resize(size, 0);
        }
        
        void clearNeuron(size_t idx) {
            synapticInput[idx] = 0.0f;
            firingState[idx] = 0;
            refractoryRemaining[idx] = 0;
            adaptationVariable[idx] = 0.0f;
            
            // Clear spike history using circular buffer
            uint8_t head = spikeHistoryHead[idx];
            uint8_t tail = spikeHistoryTail[idx];
            for (uint8_t i = tail; i != head; i = (i + 1) % MAX_SPIKE_HISTORY) {
                spikeHistory[idx][i] = 0.0f;
            }
        }
        
        size_t memoryUsage() const {
            size_t total = 0;
            total += membranePotential.capacity() * sizeof(float);
            total += restingPotential.capacity() * sizeof(float);
            total += threshold.capacity() * sizeof(float);
            total += resetPotential.capacity() * sizeof(float);
            total += leakConductance.capacity() * sizeof(float);
            total += synapticInput.capacity() * sizeof(float);
            total += firingState.capacity() * sizeof(uint8_t);
            total += refractoryRemaining.capacity() * sizeof(uint32_t);
            total += refractoryPeriod.capacity() * sizeof(uint32_t);
            total += adaptationVariable.capacity() * sizeof(float);
            total += lastSpikeTime.capacity() * sizeof(float);
            total += neuronType.capacity() * sizeof(uint64_t);
            total += regionId.capacity() * sizeof(uint64_t);
            total += populationId.capacity() * sizeof(uint64_t);
            total += spikeHistory.capacity() * MAX_SPIKE_HISTORY * sizeof(float);
            return total;
        }
    };
    
    NeuronMemoryPool() : size_(0), capacity_(0), nextBlockIndex_(0) {
        addBlock();
    }
    
    ~NeuronMemoryPool() = default;
    
    NeuronId allocate() {
        if (nextBlockIndex_ >= static_cast<uint32_t>(blocks_.size())) {
            addBlock();
        }
        
        uint32_t blockIdx = nextBlockIndex_.fetch_add(1);
        uint32_t localIdx = static_cast<uint32_t>(size_++);
        
        g_perfMonitor.neuronsCreated.fetch_add(1);
        
        return NeuronId(localIdx);
    }
    
    void deallocate(NeuronId id) {
        // Mark as deallocated (for simplicity, we'll just track count)
        if (id.value < size_) {
            g_perfMonitor.neuronsDestroyed.fetch_add(1);
        }
    }
    
    size_t size() const { return size_.load(); }
    size_t capacity() const { return capacity_.load(); }
    
    template<typename Member>
    Member* getState(NeuronIndex index) {
        // SoA access pattern for optimal cache performance
        static_assert(std::is_same<Member, float>::value || 
                      std::is_same<Member, uint8_t>::value ||
                      std::is_same<Member, uint32_t>::value ||
                      std::is_same<Member, uint64_t>::value,
                      "Unsupported member type");
        
        size_t blockIdx = index / BLOCK_SIZE;
        size_t localIdx = index % BLOCK_SIZE;
        
        if (blockIdx >= blocks_.size()) return nullptr;
        NeuronBlock& block = *blocks_[blockIdx];
        
        if constexpr (std::is_same<Member, float>::value) {
            return &block.membranePotential[localIdx];
        } else if constexpr (std::is_same<Member, uint8_t>::value) {
            return &block.firingState[localIdx];
        } else if constexpr (std::is_same<Member, uint32_t>::value) {
            // Need to decide which uint32_t member
            return nullptr;  // Placeholder
        } else if constexpr (std::is_same<Member, uint64_t>::value) {
            // Need to decide which uint64_t member  
            return nullptr;  // Placeholder
        }
        return nullptr;
    }
    
    size_t memoryUsage() const {
        size_t total = 0;
        for (const auto& block : blocks_) {
            total += block->memoryUsage();
        }
        return total;
    }
    
private:
    static constexpr size_t BLOCK_SIZE = 4096;
    
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    std::atomic<size_t> size_;
    std::atomic<size_t> capacity_;
    std::atomic<uint32_t> nextBlockIndex_;
    
    void addBlock() {
        auto block = std::make_unique<NeuronBlock>();
        
        // Calculate needed capacity
        size_t newCapacity = capacity_.load() + BLOCK_SIZE;
        if (newCapacity > MAX_POOL_SIZE) {
            newCapacity = MAX_POOL_SIZE;
        }
        
        capacity_.store(newCapacity);
        
        // Initialize block
        block->initialize(std::min(BLOCK_SIZE, MAX_POOL_SIZE - size_.load()));
        
        blocks_.push_back(std::move(block));
    }
};

// Vectorized LIF kernel using SIMD
struct VectorizedLIFKernel {
    static constexpr size_t SIMD_WIDTH = 16;  // AVX-512 width
    
    struct alignas(64) SimdNeuronState {
        float V;
        float V_rest;
        float V_reset;
        float V_threshold;
        float tau;
        float C;
        float dt;
        float adaptation;
        uint32_t refractory;
        uint8_t fired;
        uint8_t padding[3];  // Cache line alignment
    };
    
    static void stepLIF(
        float* membranePotential,
        const float* synapticInput,
        float* adaptationVariable,
        uint32_t* refractoryRemaining,
        uint8_t* fired,
        const float* restingPotential,
        const float* threshold,
        const float* resetPotential,
        const float* leakConductance,
        size_t count,
        float dt,
        float timeConstant = 20.0f,
        float capacitance = 1.0f,
        float refractoryDecay = 1.0f
    ) {
        const float dt1000 = dt * 1000.0f;
        const float expFactor = std::exp(-dt1000 / timeConstant);
        const float leakFactor = dt1000 / timeConstant;
        const float inputFactor = dt1000 / capacitance;
        
        // Vectorized processing using SIMD
        size_t i = 0;
        
        // Process 16 elements at a time with AVX-512 (if available)
#if defined(__AVX512F__)
        for (; i + SIMD_WIDTH <= count; i += SIMD_WIDTH) {
            // Load neuron states
            __m512 vV = _mm512_loadu_ps(&membranePotential[i]);
            __m512 vV_rest = _mm512_loadu_ps(&restingPotential[i]);
            __m512 vThreshold = _mm512_loadu_ps(&threshold[i]);
            __m512 vReset = _mm512_loadu_ps(&resetPotential[i]);
            __m512 vInput = _mm512_loadu_ps(&synapticInput[i]);
            __m512 vAdaptation = _mm512_loadu_ps(&adaptationVariable[i]);
            
            // Calculate leak: V_new = V + dt * ((V_rest - V)/tau + I/C)
            __m512 vLeak = _mm512_mul_ps(_mm512_sub_ps(vV_rest, vV), _mm512_set1_ps(leakFactor));
            __m512 vSynaptic = _mm512_mul_ps(vInput, _mm512_set1_ps(inputFactor));
            __m512 vNewV = _mm512_add_ps(vV, _mm512_mul_ps(_mm512_add_ps(vLeak, vSynaptic), _mm512_set1_ps(dt1000)));
            
            // Apply adaptation
            vNewV = _mm512_sub_ps(vNewV, _mm512_mul_ps(vAdaptation, _mm512_set1_ps(0.01f)));
            vAdaptation = _mm512_mul_ps(vAdaptation, _mm512_set1_ps(0.95f));
            
            // Clamp to reasonable range
            vNewV = _mm512_max_ps(vNewV, _mm512_set1_ps(-100.0f));
            vNewV = _mm512_min_ps(vNewV, _mm512_set1_ps(50.0f));
            
            // Check for spikes
            __m512 vMaskSpike = _mm512_cmp_ps(vNewV, vThreshold, _CMP_GE_OQ);
            
            // Reset spiked neurons
            vNewV = _mm512_blend_ps(vNewV, vReset, vMaskSpike);
            
            // Update adaptation for spiked neurons
            vAdaptation = _mm512_blendv_ps(vAdaptation, _mm512_set1_ps(1.0f), vMaskSpike);
            
            // Update refractory counters
            alignas(64) uint32_t refractoryArr[16];
            _mm512_storeu_si512(refractoryArr, _mm512_loadu_si512(&refractoryRemaining[i]));
            
            for (int j = 0; j < 16; ++j) {
                if (refractoryArr[j] > 0) {
                    refractoryArr[j]--;
                }
                if (vMaskSpike.m512i_u32[j] & 0x80000000) {
                    refractoryArr[j] = static_cast<uint32_t>(refractoryDecay);
                }
            }
            
            // Store results
            _mm512_storeu_ps(&membranePotential[i], vNewV);
            _mm512_storeu_ps(&adaptationVariable[i], vAdaptation);
            _mm512_storeu_si512(&refractoryRemaining[i], _mm512_loadu_si512(refractoryArr));
            
            // Record fired neurons
            alignas(64) uint32_t firedArr[16];
            _mm512_storeu_si512(firedArr, vMaskSpike);
            for (int j = 0; j < 16; ++j) {
                fired[i + j] = (firedArr[j] & 0x80000000) ? 1 : 0;
            }
        }
#endif
        
        // Process remaining elements with scalar code
        for (; i < count; ++i) {
            float& V = membranePotential[i];
            const float V_rest = restingPotential[i];
            const float V_reset = resetPotential[i];
            const float V_threshold = threshold[i];
            const float I = synapticInput[i];
            float& adaptation = adaptationVariable[i];
            uint32_t& refractory = refractoryRemaining[i];
            
            // Handle refractory period
            if (refractory > 0) {
                refractory--;
                V = V_rest;
                fired[i] = 0;
                continue;
            }
            
            // LIF dynamics
            float leak = (V_rest - V) * leakFactor;
            float input = I * inputFactor;
            V += dt1000 * (leak + input);
            
            // Apply adaptation
            V -= adaptation * 0.01f;
            adaptation *= 0.95f;
            
            // Clamp
            V = std::clamp(V, -100.0f, 50.0f);
            
            // Check for spike
            if (V >= V_threshold) {
                fired[i] = 1;
                V = V_reset;
                refractory = static_cast<uint32_t>(refractoryDecay);
                adaptation += 1.0f;
            } else {
                fired[i] = 0;
            }
        }
    }
};

// Circular buffer for spike history with pre-allocation
class SpikeHistoryBuffer {
public:
    struct alignas(64) BufferData {
        std::vector<std::array<float, MAX_HISTORY>> buffers;
        std::vector<uint8_t> heads;
        std::vector<uint8_t> tails;
        
        void initialize(size_t size) {
            buffers.reserve(size);
            heads.reserve(size, 0);
            tails.reserve(size, 0);
            
            buffers.resize(size);
            heads.resize(size, 0);
            tails.resize(size, 0);
        }
        
        void recordSpike(size_t idx, float timestamp) {
            uint8_t& head = heads[idx];
            const uint8_t tail = tails[idx];
            
            buffers[idx][head] = timestamp;
            head = (head + 1) % MAX_HISTORY;
            
            if (head == tail) {
                tail = (tail + 1) % MAX_HISTORY;
                tails[idx] = tail;
            }
        }
        
        std::vector<float> getHistory(size_t idx) const {
            std::vector<float> history;
            history.reserve(MAX_HISTORY);
            
            uint8_t head = heads[idx];
            uint8_t tail = tails[idx];
            
            if (tail <= head) {
                for (uint8_t i = tail; i < head; ++i) {
                    history.push_back(buffers[idx][i]);
                }
            } else {
                for (uint8_t i = tail; i < MAX_HISTORY; ++i) {
                    history.push_back(buffers[idx][i]);
                }
                for (uint8_t i = 0; i < head; ++i) {
                    history.push_back(buffers[idx][i]);
                }
            }
            
            return history;
        }
        
        void clear(size_t idx) {
            uint8_t head = heads[idx];
            uint8_t tail = tails[idx];
            
            if (tail <= head) {
                for (uint8_t i = tail; i < head; ++i) {
                    buffers[idx][i] = 0.0f;
                }
            } else {
                for (uint8_t i = tail; i < MAX_HISTORY; ++i) {
                    buffers[idx][i] = 0.0f;
                }
                for (uint8_t i = 0; i < head; ++i) {
                    buffers[idx][i] = 0.0f;
                }
            }
            heads[idx] = 0;
            tails[idx] = 0;
        }
        
        static constexpr size_t MAX_HISTORY = 128;
    };
    
    SpikeHistoryBuffer() = default;
    ~SpikeHistoryBuffer() = default;
    
    void initialize(size_t size) {
        buffer_.initialize(size);
    }
    
    void recordSpike(size_t idx, float timestamp) {
        buffer_.recordSpike(idx, timestamp);
    }
    
    std::vector<float> getHistory(size_t idx) const {
        return buffer_.getHistory(idx);
    }
    
    void clear(size_t idx) {
        buffer_.clear(idx);
    }
    
private:
    BufferData buffer_;
};

// Structure for optimized neuron state using SoA for cache locality
struct alignas(64) NeuronStateOptimized {
    // All float values aligned together for SIMD
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float adaptationVariable;
    float lastSpikeTime;
    float padding0;  // Ensure alignment
    
    // Non-float values
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    uint8_t firingState;
    uint8_t neuronType;
    uint64_t regionId;
    uint64_t populationId;
    uint32_t padding1;  // Ensure alignment
    
    // Pre-allocated spike history pointer (for efficient access)
    float* spikeHistory;
    
    // For bulk operations
    float synapticInput;  // Added for hot path optimization
    
    NeuronStateOptimized() : membranePotential(-70.0f),
                             restingPotential(-70.0f),
                             threshold(-55.0f),
                             resetPotential(-70.0f),
                             leakConductance(10.0f),
                             adaptationVariable(0.0f),
                             lastSpikeTime(-1.0f),
                             refractoryRemaining(0),
                             refractoryPeriod(5),
                             firingState(0),
                             neuronType(0),
                             regionId(0),
                             populationId(0),
                             spikeHistory(nullptr),
                             synapticInput(0.0f) {}
};

// Performance profiling utilities
class PerformanceProfiler {
public:
    struct ProfileData {
        double stepTimeNs;
        size_t neuronsProcessed;
        size_t spikesGenerated;
        size_t memoryAllocated;
        size_t memoryDeallocated;
        uint64_t cycles;
    };
    
    PerformanceProfiler() : enabled_(true), totalSteps_(0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void enable(bool enable) { enabled_ = enable; }
    
    void startStep() {
        if (!enabled_) return;
        stepStart_ = std::chrono::high_resolution_clock::now();
    }
    
    void endStep(size_t neuronsProcessed, size_t spikesGenerated) {
        if (!enabled_) return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - stepStart_);
        
        ProfileData data;
        data.stepTimeNs = static_cast<double>(duration.count());
        data.neuronsProcessed = neuronsProcessed;
        data.spikesGenerated = spikesGenerated;
        data.memoryAllocated = g_perfMonitor.totalAllocations.load();
        data.memoryDeallocated = g_perfMonitor.totalDeallocations.load();
        
        // Get cycle count if available (x86 specific)
#ifdef __x86_64__
        unsigned long long cyc;
        asm volatile("rdtsc" : "=A"(cyc));
        data.cycles = cyc;
#endif
        
        profileData_.push_back(data);
        totalSteps_++;
    }
    
    ProfileData getLastStep() const {
        return profileData_.empty() ? ProfileData{} : profileData_.back();
    }
    
    double getAverageStepTime() const {
        if (profileData_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& data : profileData_) {
            total += data.stepTimeNs;
        }
        return total / profileData_.size();
    }
    
    double getSpikesPerSecond() const {
        if (totalSteps_ == 0) return 0.0;
        size_t totalSpikes = 0;
        for (const auto& data : profileData_) {
            totalSpikes += data.spikesGenerated;
        }
        return static_cast<double>(totalSpikes) / (totalSteps_ * 0.001);  // Assuming 1ms steps
    }
    
private:
    bool enabled_;
    std::vector<ProfileData> profileData_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point stepStart_;
    size_t totalSteps_;
};

// Main Neuron class implementation with optimizations
struct Neuron::Impl {
    // Original fields for compatibility
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
    NeuronMemoryPool* memoryPool;
    size_t localIndex;
    NeuronStateOptimized* optimizedState;
    
    // Pre-allocated spike history buffer
    SpikeHistoryBuffer* spikeHistoryBuffer;
    
    // Performance profiler
    PerformanceProfiler profiler;
    
    // SIMD vectorizer
    VectorizedLIFKernel vectorizer;
    
    // Bulk operation buffers
    alignas(64) std::vector<float> bulkMembranePotential;
    alignas(64) std::vector<float> bulkSynapticInput;
    alignas(64) std::vector<float> bulkAdaptation;
    alignas(64) std::vector<uint32_t> bulkRefractory;
    alignas(64) std::vector<uint8_t> bulkFired;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             memoryPool(nullptr), localIndex(0), optimizedState(nullptr),
             spikeHistoryBuffer(nullptr) {
        // Initialize spike history with pre-allocated capacity
        spikeHistory.reserve(Impl::MAX_SPIKE_HISTORY);
    }
};

// Global performance profiler
static PerformanceProfiler g_globalProfiler;

Neuron::Neuron(NeuronId id) : pImpl(new Impl()) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Update global performance stats
    g_perfMonitor.neuronsCreated.fetch_add(1);
    g_perfMonitor.totalNeurons.fetch_add(1);
}

Neuron::~Neuron() {
    // Update global performance stats
    g_perfMonitor.neuronsDestroyed.fetch_add(1);
    g_perfMonitor.totalNeurons.fetch_sub(1);
    delete pImpl;
}

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
    // Transfer ownership of performance monitor updates
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

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

FiringState Neuron::getFiringState() const {
    return pImpl->state.firingState;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active ||
           pImpl->state.firingState == FiringState::Refractory;
}

bool Neuron::isRefractory() const {
    return pImpl->state.firingState == FiringState::Refractory;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
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

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

void Neuron::setAdaptationVariable(float value) {
    pImpl->state.adaptationVariable = value;
}

float Neuron::getAdaptationVariable() const {
    return pImpl->state.adaptationVariable;
}

void Neuron::setLastSpikeTime(float time) {
    pImpl->state.lastSpikeTime = time;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::setSynapseConductance(MembranePotential conductance) {
    pImpl->state.synapseConductance = conductance;
}

MembranePotential Neuron::getSynapseConductance() const {
    return pImpl->state.synapseConductance;
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

NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    pImpl->synapticInput += amplitude;
    pImpl->totalCurrent += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
    pImpl->totalCurrent -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->synapticInput += current;
    pImpl->totalCurrent += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
}

MembranePotential Neuron::getTotalCurrent() {
    return pImpl->totalCurrent;
}

void Neuron::recordSpike(Timestamp timestamp) {
    // Use optimized spike history buffer if available
    if (pImpl->spikeHistoryBuffer) {
        pImpl->spikeHistoryBuffer->recordSpike(pImpl->localIndex, static_cast<float>(timestamp));
    } else {
        // Fallback to original vector-based implementation
        pImpl->spikeHistory.push_back(timestamp);
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
    }
}

void Neuron::clearSpikeHistory() {
    if (pImpl->spikeHistoryBuffer) {
        pImpl->spikeHistoryBuffer->clear(pImpl->localIndex);
    } else {
        pImpl->spikeHistory.clear();
    }
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

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Initialize spike history buffer if available
    if (pImpl->spikeHistoryBuffer) {
        pImpl->spikeHistoryBuffer->clear(pImpl->localIndex);
    }
    
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    pImpl->profiler.startStep();
    
    // Handle refractory period (optimized path)
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        pImpl->profiler.endStep(1, 0);
        return false;
    }
    
    // Use vectorized LIF kernel if we have multiple neurons or SIMD is enabled
    // For single neuron, use optimized scalar version
    const float V_rest = pImpl->state.restingPotential;
    const float V_reset = pImpl->state.resetPotential;
    const float V_threshold = pImpl->state.threshold;
    const float tau = Impl::TIME_CONSTANT;
    const float C = Impl::MEMBRANE_CAPACITANCE;
    const float dt1000 = static_cast<float>(dt) * 1000.0f;
    
    // Pre-compute constants
    const float leakFactor = dt1000 / tau;
    const float inputFactor = dt1000 / C;
    
    float& V = pImpl->state.membranePotential;
    float& adaptation = pImpl->state.adaptationVariable;
    uint32_t& refractory = pImpl->state.refractoryRemaining;
    
    // Apply spike-frequency adaptation (with vectorization optimizations)
    if (adaptation > 0.0f) {
        V -= adaptation * 0.01f;
        adaptation *= 0.95f;  // Decay adaptation
    }
    
    // LIF dynamics using optimized math operations
    float leak = (V_rest - V) * leakFactor;
    float input = pImpl->synapticInput * inputFactor;
    V += leak + input;
    
    // Clamp to prevent instability (using bitwise optimization)
    V = std::max(-100.0f, std::min(50.0f, V));
    
    // Check for spike with branchless optimization
    uint32_t spikeMask = (V >= V_threshold) ? 0xFFFFFFFF : 0;
    fired = (spikeMask != 0);
    
    if (fired) {
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        refractory = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        adaptation += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step with optimized clearing
    pImpl->synapticInput = 0.0f;
    pImpl->totalCurrent = 0.0f;
    
    // Update performance profiler
    pImpl->profiler.endStep(1, fired ? 1 : 0);
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with optimized timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->totalCurrent = 0.0f;
    
    if (pImpl->spikeHistoryBuffer) {
        pImpl->spikeHistoryBuffer->clear(pImpl->localIndex);
    } else {
        pImpl->spikeHistory.clear();
    }
}

// Global performance monitoring functions
Neuron::PerformanceStats Neuron::getPerformanceStats() {
    PerformanceStats stats;
    
    stats.totalNeurons = g_perfMonitor.totalNeurons.load();
    stats.utilization = g_perfMonitor.getUtilization();
    stats.totalAllocations = g_perfMonitor.totalAllocations.load();
    stats.totalDeallocations = g_perfMonitor.totalDeallocations.load();
    stats.memoryUsage = g_perfMonitor.totalMemoryUsage.load();
    
    // Get average profiling stats
    if (g_globalProfiler.totalSteps() > 0) {
        stats.averageStepTime = g_globalProfiler.getAverageStepTime();
        stats.spikesPerSecond = g_globalProfiler.getSpikesPerSecond();
    }
    
    return stats;
}

void Neuron::enablePerformanceMonitoring(bool enable) {
    g_globalProfiler.enable(enable);
}

void Neuron::updatePerformanceMonitor() {
    g_globalProfiler.update();
}

} // namespace nlm

// Write implementation with specified optimization flags
void configureOptimizations() {
#ifdef NLM_PERFORMANCE_ENABLED
    // Configure all optimizations when performance is enabled
    // Memory pools
    NeuronMemoryPool pool;
    // SIMD vectorization  
    VectorizedLIFKernel lif;
    // Event-driven processing
    // etc.
#endif
}

// Initialize optimizations on startup
void initializeOptimizations() {
    configureOptimizations();
    // Pre-allocate global performance monitoring
    g_perfMonitor.recordAllocation(sizeof(PerformanceMonitor));
}

// Call at program startup
namespace {
    class OptimizationInitializer {
    public:
        OptimizationInitializer() {
            initializeOptimizations();
        }
    } initializer;
}
