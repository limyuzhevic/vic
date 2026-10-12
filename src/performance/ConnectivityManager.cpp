#include "ConnectivityManager.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cstring>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <queue>

namespace nlm {

ConnectivityManager::ConnectivityManager(size_t neuronCount, float connectionProbability)
    : neuronCount_(neuronCount)
    , connectionProbability_(connectionProbability)
    , totalConnections_(0)
    , enableCompression_(true)
    , enableDirectLookup_(true)
    , lookupTimeTotal_(0.0)
    , lookupCount_(0)
    , compressAtSize_(1000000)  // Compress when over 1M connections
    , enableCacheOptimizations_(true)
    , enableAdaptiveConnectivity_(true)
    , maxConnectionsPerNeuron_(static_cast<size_t>(neuronCount * connectionProbability))
{
    // Initialize adjacency lists
    outgoing_.resize(neuronCount_);
    incoming_.resize(neuronCount_);
    
    // Initialize direct lookup if enabled
    if (enableDirectLookup_) {
        directLookup_.resize(neuronCount_);
        for (auto& neuronLookup : directLookup_) {
            neuronLookup.resize(neuronCount_);
        }
    }
    
    // Initialize statistics
    stats_.numNeurons = neuronCount_;
    stats_.connectivityProbability = connectionProbability;
    
    // Setup cache-friendly data structures
    if (enableCacheOptimizations_) {
        setupCacheOptimizedStructures();
    }
}

ConnectivityManager::~ConnectivityManager() {
    // Clear all connections
    clear();
}

void ConnectivityManager::setupCacheOptimizedStructures() {
    // Setup cache-aligned data structures for better performance
    // Group neurons by connectivity patterns
    
    // Initialize batch processing buffers
    batchBuffer_.reserve(4096);
    sparseBatchBuffer_.reserve(4096);
    
    // Setup connection sampling structures
    if (enableAdaptiveConnectivity_) {
        connectivitySampler_ = std::make_unique<ConnectivitySampler>(neuronCount_, connectionProbability_);
    }
}

void ConnectivityManager::addConnection(NeuronId source, NeuronId target, SynapseType type) {
    if (source >= neuronCount_ || target >= neuronCount_) {
        return; // Invalid neuron IDs
    }
    
    // Check if connection already exists
    if (hasConnection(source, target)) {
        return; // Connection already exists
    }
    
    // Create synapse
    SynapseId synapseId = totalConnections_;
    totalConnections_++;
    
    // Store synapse in adjacency lists
    outgoing_[source].push_back({synapseId, target, type});
    incoming_[target].push_back({synapseId, source, type});
    
    // Update direct lookup if enabled
    if (enableDirectLookup_) {
        directLookup_[source][target] = synapseId;
    }
    
    // Update statistics
    stats_.numSynapses = totalConnections_;
    stats_.avgSynapsesPerNeuron = static_cast<float>(totalConnections_) / neuronCount_;
    
    // Check for compression
    if (enableCompression_ && totalConnections_ > compressAtSize_) {
        compressConnections();
    }
    
    // Record lookup time
    recordLookupTime();
}

void ConnectivityManager::removeConnection(NeuronId source, NeuronId target) {
    if (source >= neuronCount_ || target >= neuronCount_) {
        return;
    }
    
    auto& outList = outgoing_[source];
    auto& inList = incoming_[target];
    
    // Find and remove connection
    size_t outIndex = findConnectionIndex(outList, target);
    size_t inIndex = findConnectionIndex(inList, source);
    
    if (outIndex != SIZE_MAX && inIndex != SIZE_MAX) {
        // Update direct lookup
        if (enableDirectLookup_) {
            directLookup_[source][target] = INVALID_SYNAPSE_ID;
        }
        
        // Remove from adjacency lists
        outList.erase(outList.begin() + outIndex);
        inList.erase(inList.begin() + inIndex);
        
        totalConnections_--;
        
        // Record lookup time
        recordLookupTime();
        
        // Periodic compression
        if (enableCompression_ && totalConnections_ < compressAtSize_ / 10) {
            compressConnections();
        }
    }
}

bool ConnectivityManager::hasConnection(NeuronId source, NeuronId target) const {
    if (source >= neuronCount_ || target >= neuronCount_) {
        return false;
    }
    
    // Fast path using direct lookup
    if (enableDirectLookup_) {
        return directLookup_[source][target] != INVALID_SYNAPSE_ID;
    }
    
    // Slow path: search adjacency lists
    auto& outList = outgoing_[source];
    return std::find_if(outList.begin(), outList.end(),
                       [target](const SynapseConnection& conn) { return conn.targetNeuron == target; }) != outList.end();
}

SynapseId ConnectivityManager::getConnectionId(NeuronId source, NeuronId target) const {
    if (source >= neuronCount_ || target >= neuronCount_) {
        return INVALID_SYNAPSE_ID;
    }
    
    if (enableDirectLookup_) {
        return directLookup_[source][target];
    }
    
    // Search adjacency list
    auto& outList = outgoing_[source];
    auto it = std::find_if(outList.begin(), outList.end(),
                          [target](const SynapseConnection& conn) { return conn.targetNeuron == target; });
    
    return (it != outList.end()) ? it->synapseId : INVALID_SYNAPSE_ID;
}

std::vector<SynapseConnection> ConnectivityManager::getConnectionsFrom(NeuronId source) const {
    if (source >= neuronCount_) {
        return {};
    }
    
    return outgoing_[source];
}

std::vector<SynapseConnection> ConnectivityManager::getConnectionsTo(NeuronId target) const {
    if (target >= neuronCount_) {
        return {};
    }
    
    return incoming_[target];
}

std::vector<SynapseConnection> ConnectivityManager::getConnectionsFrom(NeuronId source) {
    if (source >= neuronCount_) {
        return {};
    }
    
    return outgoing_[source];
}

std::vector<SynapseConnection> ConnectivityManager::getConnectionsTo(NeuronId target) {
    if (target >= neuronCount_) {
        return {};
    }
    
    return incoming_[target];
}

void ConnectivityManager::compressConnections() {
    if (!enableCompression_ || totalConnections_ <= 0) {
        return;
    }
    
    // Implement connection compression
    // Remove isolated neurons and consolidate connections
    
    size_t originalConnections = totalConnections_;
    
    // Mark neurons with connections
    std::vector<bool> hasOutgoing(neuronCount_, false);
    std::vector<bool> hasIncoming(neuronCount_, false);
    
    for (size_t i = 0; i < neuronCount_; ++i) {
        hasOutgoing[i] = !outgoing_[i].empty();
        hasIncoming[i] = !incoming_[i].empty();
    }
    
    // Remove isolated neurons (neurons with no connections)
    std::vector<size_t> neuronsToRemove;
    for (size_t i = 0; i < neuronCount_; ++i) {
        if (!hasOutgoing[i] && !hasIncoming[i]) {
            neuronsToRemove.push_back(i);
        }
    }
    
    // Rebuild connectivity for remaining neurons
    if (!neuronsToRemove.empty()) {
        rebuildConnectivityAfterRemoval(neuronsToRemove);
    }
    
    // Optimize direct lookup if enabled
    if (enableDirectLookup_) {
        optimizeDirectLookup();
    }
    
    // Log compression statistics
    if (originalConnections != totalConnections_) {
        double compressionRatio = static_cast<double>(originalConnections - totalConnections_) / originalConnections;
        NLM_LOG_INFO(std::string("Connectivity compression: ") + std::to_string(originalConnections) + 
                     " -> " + std::to_string(totalConnections_) + 
                     " (" + std::to_string(compressionRatio * 100) + "% reduction)");
    }
}

void ConnectivityManager::optimizeMemoryLayout() {
    // Optimize memory layout for better cache performance
    
    // Compact adjacency lists
    for (size_t i = 0; i < neuronCount_; ++i) {
        auto& outList = outgoing_[i];
        auto& inList = incoming_[i];
        
        // Reserve appropriate capacity
        size_t expectedOutgoing = std::min(maxConnectionsPerNeuron_, outList.size() * 1.2);
        size_t expectedIncoming = std::min(maxConnectionsPerNeuron_, inList.size() * 1.2);
        
        outList.shrink_to_fit();
        inList.shrink_to_fit();
    }
    
    // Optimize direct lookup
    if (enableDirectLookup_) {
        optimizeDirectLookup();
    }
}

size_t ConnectivityManager::getConnectionCount() const {
    return totalConnections_;
}

float ConnectivityManager::getConnectionDensity() const {
    if (neuronCount_ <= 1) return 0.0f;
    
    size_t maxPossible = neuronCount_ * (neuronCount_ - 1);
    return static_cast<float>(totalConnections_) / maxPossible;
}

ConnectivityManager::Stats ConnectivityManager::getStats() const {
    Stats stats = stats_;
    
    // Update max connection counts
    size_t maxOut = 0, maxIn = 0;
    for (size_t i = 0; i < neuronCount_; ++i) {
        size_t outCount = outgoing_[i].size();
        size_t inCount = incoming_[i].size();
        maxOut = std::max(maxOut, outCount);
        maxIn = std::max(maxIn, inCount);
    }
    
    stats.maxSynapsesFromNeuron = maxOut;
    stats.maxSynapsesToNeuron = maxIn;
    
    // Calculate excitatory/inhibitory ratio (simplified)
    float totalSynapses = static_cast<float>(totalConnections_);
    if (totalSynapses > 0) {
        stats.excitatoryRatio = 0.5f; // Simplified assumption
        stats.inhibitoryRatio = 0.5f;
    }
    
    return stats;
}

void ConnectivityManager::clear() {
    // Clear all adjacency lists
    for (size_t i = 0; i < neuronCount_; ++i) {
        outgoing_[i].clear();
        incoming_[i].clear();
    }
    
    totalConnections_ = 0;
    
    // Clear direct lookup
    if (enableDirectLookup_) {
        for (size_t i = 0; i < neuronCount_; ++i) {
            directLookup_[i].assign(neuronCount_, INVALID_SYNAPSE_ID);
        }
    }
}

size_t ConnectivityManager::memoryUsage() const {
    size_t total = 0;
    
    // Adjacency lists
    for (size_t i = 0; i < neuronCount_; ++i) {
        total += outgoing_[i].capacity() * sizeof(SynapseConnection);
        total += incoming_[i].capacity() * sizeof(SynapseConnection);
    }
    
    // Direct lookup
    if (enableDirectLookup_) {
        total += directLookup_.capacity() * neuronCount_ * sizeof(SynapseId);
    }
    
    return total;
}

double ConnectivityManager::getConnectionLookupTime() const {
    if (lookupCount_ == 0) return 0.0;
    return lookupTimeTotal_ / lookupCount_;
}

void ConnectivityManager::recordLookupTime() {
    auto now = std::chrono::high_resolution_clock::now();
    if (!lastLookupTime_.count()) {
        lastLookupTime_ = now;
    } else {
        auto duration = std::chrono::duration<double, std::micro>(now - lastLookupTime_).count();
        lookupTimeTotal_ += duration;
        lookupCount_++;
        lastLookupTime_ = now;
    }
}

size_t ConnectivityManager::findConnectionIndex(const std::vector<SynapseConnection>& list, NeuronId target) const {
    auto it = std::find_if(list.begin(), list.end(),
                          [target](const SynapseConnection& conn) { return conn.targetNeuron == target; });
    return (it != list.end()) ? (it - list.begin()) : SIZE_MAX;
}

void ConnectivityManager::rebuildConnectivityAfterRemoval(const std::vector<size_t>& neuronsToRemove) {
    // Rebuild connectivity after neuron removal
    std::vector<bool> removeFlag(neuronCount_, false);
    for (size_t neuronId : neuronsToRemove) {
        removeFlag[neuronId] = true;
    }
    
    // Create new adjacency lists
    std::vector<std::vector<SynapseConnection>> newOutgoing(neuronCount_);
    std::vector<std::vector<SynapseConnection>> newIncoming(neuronCount_);
    
    // Rebuild connections for non-removed neurons
    for (size_t i = 0; i < neuronCount_; ++i) {
        if (removeFlag[i]) continue;
        
        for (const auto& conn : outgoing_[i]) {
            if (!removeFlag[conn.targetNeuron]) {
                newOutgoing[i].push_back(conn);
                newIncoming[conn.targetNeuron].push_back(conn);
            }
        }
    }
    
    // Update connectivity
    outgoing_.swap(newOutgoing);
    incoming_.swap(newIncoming);
    
    // Update total connections
    totalConnections_ = 0;
    for (size_t i = 0; i < neuronCount_; ++i) {
        totalConnections_ += outgoing_[i].size();
    }
}

void ConnectivityManager::optimizeDirectLookup() {
    if (!enableDirectLookup_) return;
    
    // Optimize direct lookup by removing entries for removed neurons
    for (size_t i = 0; i < neuronCount_; ++i) {
        for (size_t j = 0; j < neuronCount_; ++j) {
            if (hasConnection(i, j)) {
                directLookup_[i][j] = getConnectionId(i, j);
            } else {
                directLookup_[i][j] = INVALID_SYNAPSE_ID;
            }
        }
    }
}

void ConnectivityManager::setupCacheOptimizedStructures() {
    // Setup cache-aligned data structures
    batchBuffer_.reserve(4096);
    sparseBatchBuffer_.reserve(4096);
    
    // Create connectivity sampler
    connectivitySampler_ = std::make_unique<ConnectivitySampler>(neuronCount_, connectionProbability_);
}

ConnectivityManager::ConnectivitySampler::ConnectivitySampler(size_t neuronCount, float probability)
    : neuronCount_(neuronCount)
    , probability_(probability)
{
    // Initialize random number generator for connectivity sampling
    std::random_device rd;
    rng_.seed(rd());
    
    // Pre-generate random connectivity patterns
    generateRandomPatterns();
}

void ConnectivityManager::ConnectivitySampler::generateRandomPatterns() {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Generate connection patterns for different neuron groups
    // This is a simplified version - in practice, you'd have more sophisticated patterns
    
    size_t clusterSize = std::max<size_t>(10, neuronCount_ / 100);
    size_t numClusters = neuronCount_ / clusterSize;
    
    // Create local connectivity within clusters
    for (size_t cluster = 0; cluster < numClusters; ++cluster) {
        size_t clusterStart = cluster * clusterSize;
        size_t clusterEnd = std::min(clusterStart + clusterSize, neuronCount_);
        
        // Intra-cluster connections
        for (size_t i = clusterStart; i < clusterEnd; ++i) {
            for (size_t j = clusterStart; j < clusterEnd; ++j) {
                if (i != j && dist(rng_) < probability_ * 2) {  // Higher probability within cluster
                    intraClusterConnections_[i].push_back(j);
                }
            }
        }
    }
    
    // Create some long-range connections between clusters
    std::uniform_int_distribution<size_t> clusterDist(0, numClusters - 1);
    for (size_t i = 0; i < neuronCount_; ++i) {
        size_t sourceCluster = i / clusterSize;
        size_t targetCluster = clusterDist(rng_);
        if (sourceCluster != targetCluster) {
            size_t targetNeuron = targetCluster * clusterSize + (clusterDist(rng_) % clusterSize);
            if (i != targetNeuron && dist(rng_) < probability_ * 0.5) {
                longRangeConnections_[i].push_back(targetNeuron);
            }
        }
    }
}

bool ConnectivityManager::ConnectivitySampler::hasLocalConnection(size_t source, size_t target) const {
    auto it = std::find(intraClusterConnections_[source].begin(), 
                       intraClusterConnections_[source].end(), target);
    return it != intraClusterConnections_[source].end();
}

bool ConnectivityManager::ConnectivitySampler::hasLongRangeConnection(size_t source, size_t target) const {
    auto it = std::find(longRangeConnections_[source].begin(), 
                       longRangeConnections_[source].end(), target);
    return it != longRangeConnections_[source].end();
}

} // namespace nlm