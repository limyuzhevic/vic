#include "SparseConnectivity.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace nlm {

// SparseConnectivity implementation - constructor

SparseConnectivity::SparseConnectivity()
    : numNeurons_(0), maxNeuronId_(0) {
}

SparseConnectivity::~SparseConnectivity() = default;

// SparseConnectivity implementation - reserveNeurons()

inline void SparseConnectivity::reserveNeurons(size_t count) {
    outgoing_.reserve(count);
    incoming_.reserve(count);
    // Pre-reserve for expected synapse density
    synapses_.reserve(count * 100);  // Assume ~100 synapses per neuron average
}

// SparseConnectivity implementation - addSynapse()

size_t SparseConnectivity::addSynapse(uint64_t sourceNeuron, uint64_t destNeuron,
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

// SparseConnectivity implementation - removeSynapse()

void SparseConnectivity::removeSynapse(size_t synapseIndex) {
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

// SparseConnectivity implementation - getSynapse()

const SynapseRecord& SparseConnectivity::getSynapse(size_t index) const {
    return synapses_[index];
}

SynapseRecord& SparseConnectivity::getSynapse(size_t index) {
    return synapses_[index];
}

// SparseConnectivity implementation - getSynapsesFrom()

std::vector<std::pair<size_t, SynapseRecord*>> 
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

std::vector<std::pair<size_t, const SynapseRecord*>> 
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

// SparseConnectivity implementation - getSynapsesTo()

std::vector<std::pair<size_t, SynapseRecord*>> 
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

std::vector<std::pair<size_t, const SynapseRecord*>> 
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

// SparseConnectivity implementation - getSynapseCountFrom/To()

size_t SparseConnectivity::getSynapseCountFrom(uint64_t sourceNeuron) const {
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

size_t SparseConnectivity::getSynapseCountTo(uint64_t destNeuron) const {
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

// SparseConnectivity implementation - getSynapsesWithDelay()

std::vector<size_t> SparseConnectivity::getSynapsesWithDelay(uint32_t maxDelay) const {
    std::vector<size_t> result;
    for (size_t i = 0; i < synapses_.size(); ++i) {
        if (synapses_[i].sourceNeuron != UINT64_MAX && synapses_[i].delay <= maxDelay) {
            result.push_back(i);
        }
    }
    return result;
}

// SparseConnectivity implementation - getStats()

SparseConnectivity::Stats SparseConnectivity::getStats() const {
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

// SparseConnectivity implementation - clear()

void SparseConnectivity::clear() {
    outgoing_.clear();
    incoming_.clear();
    synapses_.clear();
    freeList_.clear();
    numNeurons_ = 0;
    maxNeuronId_ = 0;
}

// SparseConnectivity implementation - memoryUsage()

size_t SparseConnectivity::memoryUsage() const {
    size_t total = 0;
    total += sizeof(SynapseRecord) * synapses_.capacity();
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

// SparseConnectivity implementation - forEachSynapse()

template<typename Func>
void SparseConnectivity::forEachSynapse(Func&& func) {
    for (size_t i = 0; i < synapses_.size(); ++i) {
        if (synapses_[i].sourceNeuron != UINT64_MAX) {
            func(i, synapses_[i]);
        }
    }
}

template<typename Func>
void SparseConnectivity::forEachSynapse(Func&& func) const {
    for (size_t i = 0; i < synapses_.size(); ++i) {
        if (synapses_[i].sourceNeuron != UINT64_MAX) {
            func(i, synapses_[i]);
        }
    }
}

// Explicit template instantiations for forEachSynapse

template void SparseConnectivity::forEachSynapse<void(*)(size_t, SynapseRecord&)>(void(*)(size_t, SynapseRecord&));
template void SparseConnectivity::forEachSynapse<void(*)(size_t, const SynapseRecord&)>(void(*)(size_t, const SynapseRecord&));

} // namespace nlm

// Benchmarking and logging

inline void benchmarkSparseConnectivity() {
    std::cout << "SparseConnectivity benchmark not yet implemented" << std::endl;
}

inline void logSparseConnectivityStats(const SparseConnectivity& conn) {
    auto stats = conn.getStats();
    std::cout << "SparseConnectivity Stats:" << std::endl;
    std::cout << "  Neurons: " << stats.numNeurons << std::endl;
    std::cout << "  Synapses: " << stats.numSynapses << std::endl;
    std::cout << "  Memory usage: " << stats.totalSynapseMemory << " bytes" << std::endl;
    std::cout << "  Avg synapses per neuron: " << std::fixed << std::setprecision(2) 
              << stats.avgSynapsesPerNeuron << std::endl;
    std::cout << "  Excitatory ratio: " << std::fixed << std::setprecision(2) 
              << stats.excitatoryRatio << std::endl;
}

} // namespace nlm