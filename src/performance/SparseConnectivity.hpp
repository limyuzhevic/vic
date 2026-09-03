#pragma once

/**
 * SparseConnectivity - Memory-efficient neural connectivity representation
 * 
 * Provides sparse adjacency list storage for neural connections.
 * Avoids the O(N^2) memory requirement of dense connectivity matrices.
 * 
 * Features:
 * - Adjacency list representation
 * - Efficient synapse lookup by source or destination neuron
 * - Compact storage for large networks
 * - Cache-friendly iteration patterns
 * 
 * Biological motivation:
 * - Biological neural networks are sparsely connected (10-20% connectivity typical)
 * - Each neuron connects to thousands of others, not all neurons
 * - Connection patterns are structured (local clusters, long-range projections)
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <memory>
#include <iterator>
#include <limits>

namespace nlm {

/**
 * SynapseHandle - Lightweight reference to a synapse in the sparse connectivity
 * 
 * Instead of storing Synapse objects directly, we use compact records
 * stored in contiguous arrays, with handles referencing them by index.
 */
struct SynapseRecord {
    uint64_t sourceNeuron;    // Source neuron ID
    uint64_t destNeuron;     // Destination neuron ID
    float weight;            // Synaptic weight
    uint32_t delay;         // Synaptic delay (steps)
    uint8_t synapseType;     // SynapseType enum (1 byte)
    uint8_t flags;          // Packed plasticity flags (1 byte)
    uint16_t padding;       // Alignment padding
    
    SynapseRecord() : sourceNeuron(0), destNeuron(0), weight(0.0f), 
                      delay(1), synapseType(0), flags(0), padding(0) {}
    
    SynapseRecord(uint64_t src, uint64_t dst, float w, uint32_t d, 
                  uint8_t type, uint8_t f)
        : sourceNeuron(src), destNeuron(dst), weight(w), delay(d),
          synapseType(type), flags(f), padding(0) {}
};

/**
 * SparseConnectivity - Adjacency list based neural connectivity
 * 
 * Stores synapses in a compact format with multiple index structures:
 * - Outgoing index: source neuron -> list of synapse record indices
 * - Incoming index: destination neuron -> list of synapse record indices
 * - Synapse records: contiguous array of all synapses
 * 
 * This enables O(k) lookup where k is the number of synapses for a neuron,
 * instead of O(N) scanning all neurons.
 */
class SparseConnectivity {
public:
    /**
     * Statistics about the connectivity
     */
    struct Stats {
        size_t numNeurons;
        size_t numSynapses;
        size_t totalSynapseMemory;  // bytes
        float avgSynapsesPerNeuron;
        float connectivityProbability;
        size_t maxSynapsesFromNeuron;
        size_t maxSynapsesToNeuron;
        float excitatoryRatio;  // Fraction of excitatory synapses
        float inhibitoryRatio;
    };
    
    SparseConnectivity();
    ~SparseConnectivity();
    
    // Disable copying (use move instead)
    SparseConnectivity(const SparseConnectivity&) = delete;
    SparseConnectivity& operator=(const SparseConnectivity&) = delete;
    SparseConnectivity(SparseConnectivity&&) noexcept;
    SparseConnectivity& operator=(SparseConnectivity&&) noexcept;
    
    /**
     * Reserve space for neurons
     */
    void reserveNeurons(size_t count);
    
    /**
     * Add a synapse
     * @return Synapse record index (handle)
     */
    size_t addSynapse(uint64_t sourceNeuron, uint64_t destNeuron,
                      float weight, uint32_t delay, uint8_t synapseType);
    
    /**
     * Remove a synapse by record index
     */
    void removeSynapse(size_t synapseIndex);
    
    /**
     * Get synapse record
     */
    const SynapseRecord& getSynapse(size_t index) const;
    SynapseRecord& getSynapse(size_t index);
    
    /**
     * Get all synapses from a source neuron
     * @return Vector of (synapseIndex, synapseRecord) pairs
     */
    std::vector<std::pair<size_t, SynapseRecord*>> getSynapsesFrom(uint64_t sourceNeuron);
    std::vector<std::pair<size_t, const SynapseRecord*>> getSynapsesFrom(uint64_t sourceNeuron) const;
    
    /**
     * Get all synapses to a destination neuron
     * @return Vector of (synapseIndex, synapseRecord) pairs
     */
    std::vector<std::pair<size_t, SynapseRecord*>> getSynapsesTo(uint64_t destNeuron);
    std::vector<std::pair<size_t, const SynapseRecord*>> getSynapsesTo(uint64_t destNeuron) const;
    
    /**
     * Get synapse count from a specific neuron
     */
    size_t getSynapseCountFrom(uint64_t sourceNeuron) const;
    size_t getSynapseCountTo(uint64_t destNeuron) const;
    
    /**
     * Get total synapse count
     */
    size_t getSynapseCount() const { return synapses_.size(); }
    
    /**
     * Get neuron count
     */
    size_t getNeuronCount() const { return numNeurons_; }
    
    /**
     * Get synapses with delay <= maxDelay (for efficient event processing)
     */
    std::vector<size_t> getSynapsesWithDelay(uint32_t maxDelay) const;
    
    /**
     * Get statistics
     */
    Stats getStats() const;
    
    /**
     * Clear all synapses
     */
    void clear();
    
    /**
     * Memory usage in bytes
     */
    size_t memoryUsage() const;
    
    /**
     * Iterate over all synapses (for bulk processing)
     */
    template<typename Func>
    void forEachSynapse(Func&& func);
    
    template<typename Func>
    void forEachSynapse(Func&& func) const;
    
    /**
     * Get raw synapse array (for SoA processing)
     */
    const std::vector<SynapseRecord>& getSynapses() const { return synapses_; }
    std::vector<SynapseRecord>& getSynapses() { return synapses_; }

private:
    // Outgoing adjacency: source neuron -> vector of synapse indices
    std::unordered_map<uint64_t, std::vector<size_t>> outgoing_;
    
    // Incoming adjacency: destination neuron -> vector of synapse indices
    std::unordered_map<uint64_t, std::vector<size_t>> incoming_;
    
    // Contiguous synapse storage
    std::vector<SynapseRecord> synapses_;
    
    // Free list for removed synapses (keeps indices stable)
    std::vector<size_t> freeList_;
    
    // Track neuron count
    size_t numNeurons_;
    
    // Track max neuron ID seen
    uint64_t maxNeuronId_;
};

// Inline implementations

inline SparseConnectivity::SparseConnectivity()
    : numNeurons_(0), maxNeuronId_(0) {}

inline SparseConnectivity::~SparseConnectivity() = default;

inline SparseConnectivity::SparseConnectivity(SparseConnectivity&& other) noexcept
    : outgoing_(std::move(other.outgoing_))
    , incoming_(std::move(other.incoming_))
    , synapses_(std::move(other.synapses_))
    , freeList_(std::move(other.freeList_))
    , numNeurons_(other.numNeurons_)
    , maxNeuronId_(other.maxNeuronId_)
{
    other.numNeurons_ = 0;
    other.maxNeuronId_ = 0;
}

inline SparseConnectivity& SparseConnectivity::operator=(SparseConnectivity&& other) noexcept {
    if (this != &other) {
        outgoing_ = std::move(other.outgoing_);
        incoming_ = std::move(other.incoming_);
        synapses_ = std::move(other.synapses_);
        freeList_ = std::move(other.freeList_);
        numNeurons_ = other.numNeurons_;
        maxNeuronId_ = other.maxNeuronId_;
        other.numNeurons_ = 0;
        other.maxNeuronId_ = 0;
    }
    return *this;
}

inline void SparseConnectivity::reserveNeurons(size_t count) {
    outgoing_.reserve(count);
    incoming_.reserve(count);
    // Pre-reserve for expected synapse density
    synapses_.reserve(count * 100);  // Assume ~100 synapses per neuron average
}

inline size_t SparseConnectivity::addSynapse(uint64_t sourceNeuron, uint64_t destNeuron,
                                            float weight, uint32_t delay,
                                            uint8_t synapseType) {
    // Update neuron tracking
    numNeurons_ = std::max(numNeurons_, sourceNeuron + 1);
    numNeurons_ = std::max(numNeurons_, destNeuron + 1);
    maxNeuronId_ = std::max(maxNeuronId_, std::max(sourceNeuron, destNeuron));
    
    // Reuse free'd index if available
    size_t index;
    if (!freeList_.empty()) {
        index = freeList_.back();
        freeList_.pop_back();
        synapses_[index] = SynapseRecord(sourceNeuron, destNeuron, weight, delay, synapseType, 0);
    } else {
        index = synapses_.size();
        synapses_.emplace_back(sourceNeuron, destNeuron, weight, delay, synapseType, 0);
    }
    
    // Add to adjacency lists
    outgoing_[sourceNeuron].push_back(index);
    incoming_[destNeuron].push_back(index);
    
    return index;
}

inline void SparseConnectivity::removeSynapse(size_t synapseIndex) {
    if (synapseIndex >= synapses_.size()) return;
    
    const auto& syn = synapses_[synapseIndex];
    
    // Remove from outgoing list
    auto& outList = outgoing_[syn.sourceNeuron];
    outList.erase(std::remove(outList.begin(), outList.end(), synapseIndex), outList.end());
    
    // Remove from incoming list
    auto& inList = incoming_[syn.destNeuron];
    inList.erase(std::remove(inList.begin(), inList.end(), synapseIndex), inList.end());
    
    // Mark as free (don't actually erase to keep indices stable)
    freeList_.push_back(synapseIndex);
    synapses_[synapseIndex].sourceNeuron = UINT64_MAX;  // Mark as invalid
}

inline const SynapseRecord& SparseConnectivity::getSynapse(size_t index) const {
    return synapses_[index];
}

inline SynapseRecord& SparseConnectivity::getSynapse(size_t index) {
    return synapses_[index];
}

inline std::vector<std::pair<size_t, SynapseRecord*>> 
SparseConnectivity::getSynapsesFrom(uint64_t sourceNeuron) {
    std::vector<std::pair<size_t, SynapseRecord*>> result;
    auto it = outgoing_.find(sourceNeuron);
    if (it != outgoing_.end()) {
        result.reserve(it->second.size());
        for (size_t idx : it->second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) {  // Skip freed
                result.emplace_back(idx, &synapses_[idx]);
            }
        }
    }
    return result;
}

inline std::vector<std::pair<size_t, const SynapseRecord*>> 
SparseConnectivity::getSynapsesFrom(uint64_t sourceNeuron) const {
    std::vector<std::pair<size_t, const SynapseRecord*>> result;
    auto it = outgoing_.find(sourceNeuron);
    if (it != outgoing_.end()) {
        result.reserve(it->second.size());
        for (size_t idx : it->second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) {
                result.emplace_back(idx, &synapses_[idx]);
            }
        }
    }
    return result;
}

inline std::vector<std::pair<size_t, SynapseRecord*>> 
SparseConnectivity::getSynapsesTo(uint64_t destNeuron) {
    std::vector<std::pair<size_t, SynapseRecord*>> result;
    auto it = incoming_.find(destNeuron);
    if (it != incoming_.end()) {
        result.reserve(it->second.size());
        for (size_t idx : it->second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) {
                result.emplace_back(idx, &synapses_[idx]);
            }
        }
    }
    return result;
}

inline std::vector<std::pair<size_t, const SynapseRecord*>> 
SparseConnectivity::getSynapsesTo(uint64_t destNeuron) const {
    std::vector<std::pair<size_t, const SynapseRecord*>> result;
    auto it = incoming_.find(destNeuron);
    if (it != incoming_.end()) {
        result.reserve(it->second.size());
        for (size_t idx : it->second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) {
                result.emplace_back(idx, &synapses_[idx]);
            }
        }
    }
    return result;
}

inline size_t SparseConnectivity::getSynapseCountFrom(uint64_t sourceNeuron) const {
    auto it = outgoing_.find(sourceNeuron);
    if (it != outgoing_.end()) {
        size_t count = 0;
        for (size_t idx : it->second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) ++count;
        }
        return count;
    }
    return 0;
}

inline size_t SparseConnectivity::getSynapseCountTo(uint64_t destNeuron) const {
    auto it = incoming_.find(destNeuron);
    if (it != incoming_.end()) {
        size_t count = 0;
        for (size_t idx : it->second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) ++count;
        }
        return count;
    }
    return 0;
}

inline std::vector<size_t> SparseConnectivity::getSynapsesWithDelay(uint32_t maxDelay) const {
    std::vector<size_t> result;
    for (size_t i = 0; i < synapses_.size(); ++i) {
        if (synapses_[i].sourceNeuron != UINT64_MAX && synapses_[i].delay <= maxDelay) {
            result.push_back(i);
        }
    }
    return result;
}

inline SparseConnectivity::Stats SparseConnectivity::getStats() const {
    Stats stats{};
    stats.numNeurons = numNeurons_;
    stats.numSynapses = synapses_.size() - freeList_.size();
    
    size_t totalSynapses = 0;
    size_t maxOut = 0;
    size_t maxIn = 0;
    size_t excitatoryCount = 0;
    
    for (const auto& pair : outgoing_) {
        size_t validCount = 0;
        for (size_t idx : pair.second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) {
                ++validCount;
                if (synapses_[idx].synapseType == 0) {  // Excitatory
                    ++excitatoryCount;
                }
            }
        }
        totalSynapses += validCount;
        maxOut = std::max(maxOut, validCount);
    }
    
    for (const auto& pair : incoming_) {
        size_t validCount = 0;
        for (size_t idx : pair.second) {
            if (synapses_[idx].sourceNeuron != UINT64_MAX) {
                ++validCount;
            }
        }
        maxIn = std::max(maxIn, validCount);
    }
    
    stats.totalSynapseMemory = sizeof(SynapseRecord) * synapses_.size();
    stats.avgSynapsesPerNeuron = numNeurons_ > 0 ? 
        static_cast<float>(totalSynapses) / numNeurons_ : 0.0f;
    stats.maxSynapsesFromNeuron = maxOut;
    stats.maxSynapsesToNeuron = maxIn;
    stats.excitatoryRatio = totalSynapses > 0 ? 
        static_cast<float>(excitatoryCount) / totalSynapses : 0.0f;
    stats.inhibitoryRatio = 1.0f - stats.excitatoryRatio;
    
    // Calculate connectivity probability
    if (numNeurons_ > 1) {
        size_t maxPossible = numNeurons_ * (numNeurons_ - 1);
        stats.connectivityProbability = static_cast<float>(totalSynapses) / maxPossible;
    } else {
        stats.connectivityProbability = 0.0f;
    }
    
    return stats;
}

inline void SparseConnectivity::clear() {
    outgoing_.clear();
    incoming_.clear();
    synapses_.clear();
    freeList_.clear();
    numNeurons_ = 0;
    maxNeuronId_ = 0;
}

inline size_t SparseConnectivity::memoryUsage() const {
    size_t total = sizeof(SynapseRecord) * synapses_.capacity();
    total += sizeof(std::vector<size_t>) * (outgoing_.size() + incoming_.size());
    for (const auto& pair : outgoing_) {
        total += sizeof(size_t) * pair.second.capacity();
    }
    for (const auto& pair : incoming_) {
        total += sizeof(size_t) * pair.second.capacity();
    }
    total += sizeof(size_t) * freeList_.capacity();
    return total;
}

template<typename Func>
inline void SparseConnectivity::forEachSynapse(Func&& func) {
    for (size_t i = 0; i < synapses_.size(); ++i) {
        if (synapses_[i].sourceNeuron != UINT64_MAX) {
            func(i, synapses_[i]);
        }
    }
}

template<typename Func>
inline void SparseConnectivity::forEachSynapse(Func&& func) const {
    for (size_t i = 0; i < synapses_.size(); ++i) {
        if (synapses_[i].sourceNeuron != UINT64_MAX) {
            func(i, synapses_[i]);
        }
    }
}

} // namespace nlm