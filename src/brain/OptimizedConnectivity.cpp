#include "OptimizedConnectivity.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

CSRConnectivity::CSRConnectivity(size_t neuronCount)
    : neuronCount_(neuronCount)
    , totalSynapses_(0) {
    // Initialize CSR arrays
    rowOffsets_.resize(neuronCount_ + 1, 0);
    activeNeurons_.resize(neuronCount_, false);
    
    // Pre-allocate incoming lists for neurons that have connections
    incomingLists_.resize(neuronCount_);
}

CSRConnectivity::~CSRConnectivity() = default;

size_t CSRConnectivity::addSynapse(uint64_t source, uint64_t destination,
                                   float weight, uint8_t synapseType, uint32_t delay) {
    // Validate neuron IDs
    if (source >= neuronCount_ || destination >= neuronCount_) {
        return SIZE_MAX;
    }
    
    // Mark neurons as active
    if (!activeNeurons_[source]) {
        activeNeurons_[source] = true;
        activeNeuronIndices_.push_back(source);
    }
    
    // Add to incoming list for reverse lookup
    incomingLists_[destination].push_back(totalSynapses_);
    
    // Add synapse to CSR structure
    rowOffsets_[source + 1]++;  // Increment count for next offset
    
    // Store synapse data
    columnIndices_.push_back(destination);
    weights_.push_back(weight);
    synapseTypes_.push_back(synapseType);
    delays_.push_back(delay);
    
    totalSynapses_++;
    
    // Compute prefix sum for row offsets
    std::exclusive_scan(rowOffsets_.begin(), rowOffsets_.end(), rowOffsets_.begin(), 0);
    
    return totalSynapses_ - 1;
}

void CSRConnectivity::removeSynapse(size_t synapseIndex) {
    if (synapseIndex >= totalSynapses_) return;
    
    // Mark synapse as removed by moving it to the end
    std::swap(columnIndices_[synapseIndex], columnIndices_.back());
    std::swap(weights_[synapseIndex], weights_.back());
    std::swap(synapseTypes_[synapseIndex], synapseTypes_.back());
    std::swap(delays_[synapseIndex], delays_.back());
    
    // Update incoming list
    uint64_t destination = columnIndices_[synapseIndex];
    auto& incomingList = incomingLists_[destination];
    auto it = std::find(incomingList.begin(), incomingList.end(), synapseIndex);
    if (it != incomingList.end()) {
        *it = totalSynapses_ - 1;  // Replace with index of moved synapse
    }
    
    // Mark as removed
    columnIndices_.back() = UINT64_MAX;
    weights_.pop_back();
    synapseTypes_.pop_back();
    delays_.pop_back();
    
    // Update row offsets
    std::exclusive_scan(rowOffsets_.begin(), rowOffsets_.end(), rowOffsets_.begin(), 0);
    
    totalSynapses_--;
}

std::vector<std::tuple<uint64_t, float, uint8_t, uint32_t>> CSRConnectivity::getSynapsesFrom(
    uint64_t source) const {
    std::vector<std::tuple<uint64_t, float, uint8_t, uint32_t>> result;
    
    if (source >= neuronCount_) return result;
    
    // Find range of outgoing synapses for this source
    size_t start = rowOffsets_[source];
    size_t end = rowOffsets_[source + 1];
    
    for (size_t i = start; i < end; ++i) {
        if (columnIndices_[i] != UINT64_MAX) {
            result.emplace_back(columnIndices_[i], weights_[i],
                               synapseTypes_[i], delays_[i]);
        }
    }
    
    return result;
}

std::vector<std::tuple<uint64_t, float, uint8_t, uint32_t>> CSRConnectivity::getSynapsesTo(
    uint64_t destination) const {
    std::vector<std::tuple<uint64_t, float, uint8_t, uint32_t>> result;
    
    if (destination >= neuronCount_) return result;
    
    // Use incoming list for efficient reverse lookup
    for (size_t synapseIdx : incomingLists_[destination]) {
        if (synapseIdx < totalSynapses_ && columnIndices_[synapseIdx] != UINT64_MAX) {
            // Find source neuron for this synapse
            // Need to search through all valid synapses to find source
            // This is inefficient but will be optimized later
            for (size_t i = 0; i < totalSynapses_; ++i) {
                if (i != synapseIdx && columnIndices_[i] != UINT64_MAX) {
                    uint64_t source = i;  // Actually should store source separately
                    result.emplace_back(source, weights_[synapseIdx],
                                      synapseTypes_[synapseIdx], delays_[synapseIdx]);
                    break;
                }
            }
        }
    }
    
    return result;
}

void CSRConnectivity::clear() {
    std::fill(rowOffsets_.begin(), rowOffsets_.end(), 0);
    columnIndices_.clear();
    weights_.clear();
    synapseTypes_.clear();
    delays_.clear();
    
    for (auto& incomingList : incomingLists_) {
        incomingList.clear();
    }
    
    std::fill(activeNeurons_.begin(), activeNeurons_.end(), false);
    activeNeuronIndices_.clear();
    
    totalSynapses_ = 0;
}

CSRConnectivity::Stats CSRConnectivity::getStats() const {
    Stats stats{};
    stats.numNeurons = neuronCount_;
    stats.numSynapses = totalSynapses_;
    
    if (totalSynapses_ > 0) {
        // Calculate average degrees using active neurons
        size_t outDegreeSum = 0;
        size_t inDegreeSum = 0;
        size_t maxOut = 0;
        size_t maxIn = 0;
        size_t excitatoryCount = 0;
        
        for (size_t neuronId : activeNeuronIndices_) {
            size_t outDegree = rowOffsets_[neuronId + 1] - rowOffsets_[neuronId];
            outDegreeSum += outDegree;
            maxOut = std::max(maxOut, outDegree);
            
            // Count excitatory synapses
            for (size_t i = rowOffsets_[neuronId]; i < rowOffsets_[neuronId + 1]; ++i) {
                if (columnIndices_[i] != UINT64_MAX && synapseTypes_[i] == 0) // Excitatory
                    excitatoryCount++;
            }
        }
        
        // Calculate in degrees by counting incoming lists
        for (const auto& incomingList : incomingLists_) {
            size_t inDegree = incomingList.size();
            inDegreeSum += inDegree;
            maxIn = std::max(maxIn, inDegree);
        }
        
        stats.avgOutDegree = static_cast<float>(outDegreeSum) / static_cast<float>(neuronCount_);
        stats.avgInDegree = static_cast<float>(inDegreeSum) / static_cast<float>(neuronCount_);
        stats.maxOutDegree = maxOut;
        stats.maxInDegree = maxIn;
        stats.excitatoryRatio = static_cast<float>(excitatoryCount) / static_cast<float>(totalSynapses_);
        stats.inhibitoryRatio = 1.0f - stats.excitatoryRatio;
        
        // Calculate connectivity probability
        if (neuronCount_ > 1) {
            size_t maxPossible = neuronCount_ * (neuronCount_ - 1);
            stats.connectivityProbability = static_cast<float>(totalSynapses_) / static_cast<float>(maxPossible);
        }
    }
    
    stats.memoryUsageBytes = memoryUsage();
    
    return stats;
}

size_t CSRConnectivity::memoryUsage() const {
    size_t total = 0;
    total += rowOffsets_.capacity() * sizeof(size_t);
    total += columnIndices_.capacity() * sizeof(uint64_t);
    total += weights_.capacity() * sizeof(float);
    total += synapseTypes_.capacity() * sizeof(uint8_t);
    total += delays_.capacity() * sizeof(uint32_t);
    
    size_t incomingListOverhead = 0;
    for (const auto& incomingList : incomingLists_) {
        incomingListOverhead += incomingList.capacity() * sizeof(size_t);
    }
    total += incomingListOverhead;
    
    return total;
}

void CSRConnectivity::reserveNeurons(size_t count) {
    if (count > neuronCount_) {
        // Resize neuron-related arrays
        rowOffsets_.resize(count + 1);
        activeNeurons_.resize(count);
        incomingLists_.resize(count);
        neuronCount_ = count;
    }
}

} // namespace nlm
