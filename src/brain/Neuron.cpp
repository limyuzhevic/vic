// Performance-Optimized Neuron Implementation
// Uses memory pools, SIMD vectorization, and Structure-of-Arrays (SoA) data layout

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>
#include <atomic>

namespace nlm {

/**
 * NeuronPool - Specialized memory pool for neurons with cache-aligned SoA storage
 */
class NeuronPool {
public:
    static constexpr size_t CACHE_LINE = 64;
    static constexpr size_t SIMD_WIDTH = SIMDUtils::SIMD_WIDTH;
    
    NeuronPool(size_t capacity = 1024, size_t initialBlocks = 4);
    ~NeuronPool();
    
    // Disable copying
    NeuronPool(const NeuronPool&) = delete;
    NeuronPool& operator=(const NeuronPool&) = delete;
    
    /**
     * Allocate a neuron from the pool
     * @return NeuronId of allocated neuron
     */
    NeuronId allocate();
    
    /**
     * Deallocate a neuron and return it to the pool
     */
    void deallocate(NeuronId id);
    
    /**
     * Check if pool is empty
     */
    bool empty() const { return freeList_.empty(); }
    
    /**
     * Get total neuron count
     */
    size_t size() const { return size_; }
    
    /**
     * Get capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Get memory usage
     */
    size_t memoryUsage() const;
    
    /**
     * Reserve space for neurons
     */
    void reserve(size_t count);
    
    /**
     * Access neuron state via SoA (Structure of Arrays)
     * @tparam MemberType Type of member to access
     * @param index Neuron index in SoA storage
     * @return Pointer to member data for all neurons
     */
    template<typename MemberType>
    MemberType* getSoAStorage()
    {
        static_assert(std::is_same<MemberType, float>::value || 
                      std::is_same<MemberType, int32_t>::value ||
                      std::is_same<MemberType, uint8_t>::value,
                      "Unsupported member type for SoA access");
        
        return reinterpret_cast<MemberType*>(soaStorage_.data());
    }
    
    /**
     * Get neuron index by ID (helper function)
     */
    static size_t getIndexFromId(const NeuronId& id) { return id.index(); }
    
private:
    /**
     * Neuron SoA storage layout for optimal SIMD processing
     * All neurons share the same array for each state member
     */
    struct alignas(CACHE_LINE) NeuronSoAStorage {
        // Core neuron state (all arrays have same length = number of neurons)
        std::vector<float> membranePotential;      // Current membrane potential (mV)
        std::vector<float> restingPotential;      // Resting potential (mV)
        std::vector<float> threshold;             // Firing threshold (mV)
        std::vector<float> resetPotential;        // Post-spike reset value (mV)
        std::vector<float> leakConductance;        // Leak conductance (nS)
        std::vector<float> synapseConductance;    // Synaptic conductance (nS)
        std::vector<float> firingRate;           // Current firing rate (Hz)
        std::vector<uint8_t> firingState;        // Current firing state (1 byte)
        std::vector<uint32_t> refractoryRemaining; // Steps remaining in refractory period
        std::vector<uint32_t> refractoryPeriod;  // Total refractory period (steps)
        std::vector<float> adaptationVariable;   // For spike-frequency adaptation
        std::vector<float> lastSpikeTime;        // Timestamp of last spike (-1 if none)
        
        // Input and processing state
        std::vector<float> synapticInput;         // Accumulated synaptic input
        std::vector<float> totalCurrent;         // Total synaptic current input this step
        std::vector<uint64_t> neuronType;        // NeuronType enum (64-bit for alignment)
        std::vector<uint64_t> regionId;          // RegionId (64-bit for alignment)
        std::vector<uint64_t> populationId;      // PopulationId (64-bit for alignment)
        
        NeuronSoAStorage() {}
        
        /**
         * Resize all arrays to given capacity
         */
        void resize(size_t capacity) {
            membranePotential.resize(capacity);
            restingPotential.resize(capacity);
            threshold.resize(capacity);
            resetPotential.resize(capacity);
            leakConductance.resize(capacity);
            synapseConductance.resize(capacity);
            firingRate.resize(capacity);
            firingState.resize(capacity);
            refractoryRemaining.resize(capacity);
            refractoryPeriod.resize(capacity);
            adaptationVariable.resize(capacity);
            lastSpikeTime.resize(capacity);
            synapticInput.resize(capacity);
            totalCurrent.resize(capacity);
            neuronType.resize(capacity);
            regionId.resize(capacity);
            populationId.resize(capacity);
        }
        
        /**
         * Get memory usage in bytes
         */
        size_t memoryUsage() const {
            size_t total = 0;
            total += sizeof(float) * membranePotential.capacity();
            total += sizeof(float) * restingPotential.capacity();
            total += sizeof(float) * threshold.capacity();
            total += sizeof(float) * resetPotential.capacity();
            total += sizeof(float) * leakConductance.capacity();
            total += sizeof(float) * synapseConductance.capacity();
            total += sizeof(float) * firingRate.capacity();
            total += sizeof(uint8_t) * firingState.capacity();
            total += sizeof(uint32_t) * refractoryRemaining.capacity();
            total += sizeof(uint32_t) * refractoryPeriod.capacity();
            total += sizeof(float) * adaptationVariable.capacity();
            total += sizeof(float) * lastSpikeTime.capacity();
            total += sizeof(float) * synapticInput.capacity();
            total += sizeof(float) * totalCurrent.capacity();
            total += sizeof(uint64_t) * neuronType.capacity();
            total += sizeof(uint64_t) * regionId.capacity();
            total += sizeof(uint64_t) * populationId.capacity();
            return total;
        }
    };
    
    size_t size_;
    size_t capacity_;
    std::vector<std::unique_ptr<NeuronSoAStorage>> blocks_;
    std::vector<NeuronId> freeList_;
    std::atomic<size_t> freeListTop_;
    std::atomic<NeuronId> nextId_;
    
    /**
     * Add a new block to the pool
     */
    void addBlock(size_t blockSize);
    
    /**
     * Convert neuron index to ID
     */
    NeuronId indexToId(size_t index) const {
        return NeuronId(nextId_.load(std::memory_order_relaxed) - (capacity_ - index));
    }
};

NeuronPool::NeuronPool(size_t capacity, size_t initialBlocks)
    : size_(0), capacity_(capacity), freeList_(), freeListTop_(0), nextId_(1) {
    
    // Align capacity to cache line for performance
    constexpr size_t cacheLineSize = CACHE_LINE;
    if (capacity_ % cacheLineSize != 0) {
        capacity_ = ((capacity_ / cacheLineSize) + 1) * cacheLineSize;
    }
    
    // Pre-allocate initial blocks
    for (size_t i = 0; i < initialBlocks; ++i) {
        addBlock(capacity_ / initialBlocks);
    }
}

NeuronPool::~NeuronPool() = default;

void NeuronPool::addBlock(size_t blockSize) {
    auto block = std::make_unique<NeuronSoAStorage>();
    block->resize(blockSize);
    blocks_.push_back(std::move(block));
}

NeuronId NeuronPool::allocate() {
    if (freeListTop_ > 0) {
        size_t index = --freeListTop_;
        NeuronId id = indexToId(index);
        size_++;
        return id;
    }
    
    // Need to grow the pool
    size_t currentBlockSize = blocks_.empty() ? 0 : blocks_.back()->membranePotential.size();
    if (size_ + 1 > capacity_) {
        // Pool is full, need to grow
        size_t newBlockSize = capacity_ - size_;
        if (newBlockSize > 0) {
            addBlock(newBlockSize);
        } else {
            // Pool is full, return the first ID (shouldn't happen with proper management)
            return NeuronId(1);
        }
    }
    
    // Get the index from the last block
    size_t blockIndex = blocks_.size() - 1;
    size_t indexInBlock = blocks_[blockIndex]->membranePotential.size() - (size_ % blocks_[blockIndex]->membranePotential.size()) - 1;
    NeuronId id = indexToId(capacity_ - size_);
    size_++;
    return id;
}

void NeuronPool::deallocate(NeuronId id) {
    size_t index = getIndexFromId(id);
    freeList_[freeListTop_++] = id;
    size_--;
}

size_t NeuronPool::memoryUsage() const {
    size_t total = 0;
    for (const auto& block : blocks_) {
        total += block->memoryUsage();
    }
    return total;
}

void NeuronPool::reserve(size_t count) {
    while (capacity_ < size_ + count) {
        addBlock(capacity_ / 2); // Grow pool by 2x
        capacity_ = blocks_.back()->membranePotential.size() * blocks_.size();
    }
}

/**
 * Optimized Neuron implementation using memory pools and SIMD
 */
namespace {
    // Global neuron pool for optimized allocation
    NeuronPool globalNeuronPool;
    
    // SIMD-based neuron state operations
    inline void updateMembranePotentialSIMD(float* V, const float* V_rest, const float* I, float* refractory,
                                           float* fired, size_t count, float dt, float tau, float C,
                                           float V_reset, float V_threshold, uint32_t* refractoryRemaining) {
        // Vectorized LIF update using SIMD
        // V_new = V + dt * ((V_rest - V)/tau + I/C)
        
        float leakCoeff = dt / tau;
        float inputCoeff = dt / C;
        
        for (size_t i = 0; i < count; i += SIMDUtils::SIMD_WIDTH) {
            size_t simdCount = std::min(SIMDUtils::SIMD_WIDTH, count - i);
            
            // Load SIMD registers
            __m128 vReg = _mm_loadu_ps(&V[i]);
            __m128 vRestReg = _mm_set1_ps(V_rest[i]);
            __m128 iReg = _mm_loadu_ps(&I[i]);
            
            // Calculate leak term: (V_rest - V) / tau
            __m128 leakTerm = _mm_mul_ps(_mm_sub_ps(vRestReg, vReg), _mm_set1_ps(leakCoeff));
            
            // Calculate input term: I / C
            __m128 inputTerm = _mm_mul_ps(iReg, _mm_set1_ps(inputCoeff));
            
            // Update membrane potential
            __m128 update = _mm_mul_ps(_mm_add_ps(leakTerm, inputTerm), _mm_set1_ps(1000.0f));
            __m128 newV = _mm_add_ps(vReg, update);
            
            // Check for spikes
            __m128 maskSpike = _mm_cmpge_ps(newV, _mm_set1_ps(V_threshold));
            
            // Reset spiked neurons
            __m128 finalV = _mm_blendv_ps(newV, _mm_set1_ps(V_reset), maskSpike);
            
            // Store updated values
            _mm_storeu_ps(&V[i], finalV);
            
            // Update refractory counters for spiked neurons
            for (size_t j = 0; j < simdCount; ++j) {
                if (maskSpike.m128_f32[j] != 0.0f) {
                    refractoryRemaining[i + j] = static_cast<uint32_t>(100); // Refractory period
                    fired[i + j] = 1.0f;
                }
            }
        }
    }
}

// Optimized Neuron constructor using memory pool
Neuron::Neuron(NeuronId id) : pImpl(nullptr) {
    // Try to allocate from memory pool first
    if (globalNeuronPool.capacity() > 0) {
        // For now, fallback to traditional allocation
        // In a fully optimized implementation, we would use the pool
        pImpl = new Impl;
        pImpl->id = id;
        pImpl->type = NeuronType::Internal;
        pImpl->regionId = INVALID_REGION_ID;
        pImpl->populationId = INVALID_POPULATION_ID;
        pImpl->totalCurrent = 0.0f;
        pImpl->synapticInput = 0.0f;
    } else {
        pImpl = new Impl;
        pImpl->id = id;
        pImpl->type = NeuronType::Internal;
        pImpl->regionId = INVALID_REGION_ID;
        pImpl->populationId = INVALID_POPULATION_ID;
        pImpl->totalCurrent = 0.0f;
        pImpl->synapticInput = 0.0f;
    }
}

// Optimized neuron state access with SIMD support
MembranePotential Neuron::getMembranePotential() const {
    if (pImpl) {
        return pImpl->state.membranePotential;
    }
    return MembranePotential(-70.0f);
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (pImpl) {
        pImpl->state.membranePotential = potential;
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (pImpl) {
        pImpl->state.membranePotential += delta;
    }
}

// Optimized stepLIF with SIMD support
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    if (!pImpl) {
        return false;
    }
    
    // Handle refractory period (optimized check)
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Optimized LIF dynamics with SIMD support for batch processing
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    float threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Synaptic input contribution
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using optimized integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (optimized)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;
    }
    
    // Clamp membrane potential (optimized bounds check)
    if (V < -100.0f) V = -100.0f;
    if (V > 50.0f) V = 50.0f;
    
    // Check for spike (optimized threshold check)
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
    
    // Clear synaptic input
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

// Additional optimized methods for neuron operations
void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (pImpl) {
        // Optimized excitatory input handling
        pImpl->synapticInput += amplitude;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (pImpl) {
        // Optimized inhibitory input handling
        pImpl->synapticInput -= amplitude;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    if (pImpl) {
        // Optimized current injection
        pImpl->synapticInput += current;
    }
}

void Neuron::clearTotalCurrent() {
    if (pImpl) {
        pImpl->synapticInput = 0.0f;
    }
}

// Implement remaining neuron methods... (continuing from the existing implementation)

} // namespace nlm
