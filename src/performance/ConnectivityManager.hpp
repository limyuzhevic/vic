#pragma once

/**
 * ConnectivityManager - Sparse neural connectivity representation
 * 
 * Provides efficient sparse connectivity storage and management for neural networks.
 * Uses adjacency lists and direct lookup for O(1) access to existing connections.
 * Optimized for both memory efficiency and access speed.
 * 
 * Features:
 * - Adjacency list representation for outgoing and incoming connections
 * - Direct lookup table for O(1) connection checks
 * - Connection compression and cleanup
 * - Cache-friendly memory layout
 * - Adaptive connectivity sampling for large networks
 * - Statistics tracking for connectivity patterns
 * 
 * Biological motivation:
 * - Biological neural networks are sparsely connected
 * - Connection patterns are structured (local clusters + long-range projections)
 * - Different neuron types have different connectivity profiles
 * - Synaptic delays and types affect connectivity patterns
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class ConnectivityManager;

/**
 * Synapse connection record for adjacency lists
 */
struct SynapseConnection {
    SynapseId synapseId;       // Unique synapse identifier
    NeuronId targetNeuron;     // Destination neuron ID (for outgoing connections)
    NeuronId sourceNeuron;     // Source neuron ID (for incoming connections)
    SynapseType synapseType;   // Type of synapse (excitatory, inhibitory, electrical)
    
    SynapseConnection() : synapseId(INVALID_SYNAPSE_ID), 
                         targetNeuron(0), sourceNeuron(0),
                         synapseType(SynapseType::Excitatory) {}
    
    SynapseConnection(SynapseId id, NeuronId target, SynapseType type)
        : synapseId(id), targetNeuron(target), sourceNeuron(INVALID_NEURON_ID),
          synapseType(type) {}
    
    SynapseConnection(SynapseId id, NeuronId source, SynapseType type)
        : synapseId(id), targetNeuron(INVALID_NEURON_ID), sourceNeuron(source),
          synapseType(type) {}
};

/**
 * Statistics about the connectivity structure
 */
struct ConnectivityStats {
    size_t numNeurons;          // Total number of neurons
    size_t numSynapses;         // Total number of synapses
    size_t totalSynapseMemory;   // Memory usage in bytes
    float avgSynapsesPerNeuron; // Average connections per neuron
    float connectivityProbability; // Overall connection density
    size_t maxSynapsesFromNeuron;   // Maximum outgoing connections for any neuron
    size_t maxSynapsesToNeuron;     // Maximum incoming connections for any neuron
    float excitatoryRatio;     // Fraction of excitatory synapses
    float inhibitoryRatio;     // Fraction of inhibitory synapses
};

/**
 * ConnectivityManager - Manages sparse neural connectivity
 */
class ConnectivityManager {
public:
    /**
     * Create connectivity manager
     * @param neuronCount Total number of neurons in the network
     * @param connectionProbability Initial connection probability (0.0-1.0)
     */
    ConnectivityManager(size_t neuronCount, float connectionProbability);
    
    ~ConnectivityManager();
    
    // Disable copying
    ConnectivityManager(const ConnectivityManager&) = delete;
    ConnectivityManager& operator=(const ConnectivityManager&) = delete;
    
    // Enable move operations
    ConnectivityManager(ConnectivityManager&&) noexcept;
    ConnectivityManager& operator=(ConnectivityManager&&) noexcept;
    
    /**
     * Add a synaptic connection between neurons
     * @param source Source neuron ID
     * @param target Target neuron ID
     * @param synapseType Type of synapse
     */
    void addConnection(NeuronId source, NeuronId target, SynapseType synapseType);
    
    /**
     * Remove a synaptic connection
     * @param source Source neuron ID
     * @param target Target neuron ID
     */
    void removeConnection(NeuronId source, NeuronId target);
    
    /**
     * Check if a connection exists
     * @param source Source neuron ID
     * @param target Target neuron ID
     * @return true if connection exists
     */
    bool hasConnection(NeuronId source, NeuronId target) const;
    
    /**
     * Get synapse ID for a connection
     * @param source Source neuron ID
     * @param target Target neuron ID
     * @return Synapse ID, or INVALID_SYNAPSE_ID if not connected
     */
    SynapseId getConnectionId(NeuronId source, NeuronId target) const;
    
    /**
     * Get all outgoing connections from a source neuron
     * @param source Source neuron ID
     * @return Vector of outgoing synapse connections
     */
    std::vector<SynapseConnection> getConnectionsFrom(NeuronId source) const;
    
    /**
     * Get all incoming connections to a target neuron
     * @param target Target neuron ID
     * @return Vector of incoming synapse connections
     */
    std::vector<SynapseConnection> getConnectionsTo(NeuronId target) const;
    
    /**
     * Get all outgoing connections (non-const version)
     */
    std::vector<SynapseConnection> getConnectionsFrom(NeuronId source);
    
    /**
     * Get all incoming connections (non-const version)
     */
    std::vector<SynapseConnection> getConnectionsTo(NeuronId target);
    
    /**
     * Compress connectivity by removing isolated neurons
     * Reduces memory usage while maintaining network functionality
     */
    void compressConnections();
    
    /**
     * Optimize memory layout for better cache performance
     */
    void optimizeMemoryLayout();
    
    /**
     * Get total number of synapses
     */
    size_t getConnectionCount() const;
    
    /**
     * Get connection density (0.0 to 1.0)
     */
    float getConnectionDensity() const;
    
    /**
     * Get connectivity statistics
     */
    ConnectivityStats getStats() const;
    
    /**
     * Clear all connections
     */
    void clear();
    
    /**
     * Get estimated memory usage in bytes
     */
    size_t memoryUsage() const;
    
    /**
     * Get average connection lookup time (nanoseconds)
     */
    double getConnectionLookupTime() const;
    
    // Configuration
    static constexpr SynapseId INVALID_SYNAPSE_ID = UINT64_MAX;
    static constexpr NeuronId INVALID_NEURON_ID = UINT64_MAX;
    
private:
    // Helper classes
    
    /**
     * Connectivity sampler for generating structured connection patterns
     */
    class ConnectivitySampler {
    public:
        ConnectivitySampler(size_t neuronCount, float probability);
        
        bool hasLocalConnection(size_t source, size_t target) const;
        bool hasLongRangeConnection(size_t source, size_t target) const;
        
    private:
        size_t neuronCount_;
        float probability_;
        std::vector<std::vector<size_t>> intraClusterConnections_;
        std::vector<std::vector<size_t>> longRangeConnections_;
        std::mt19937 rng_;
        
        void generateRandomPatterns();
    };
    
    // Friend declaration for event pool iterator
    friend class EventSystem;
    
    // Internal implementation
    
    /**
     * Setup cache-optimized data structures
     */
    void setupCacheOptimizedStructures();
    
    /**
     * Find connection index in adjacency list
     */
    size_t findConnectionIndex(const std::vector<SynapseConnection>& list, NeuronId target) const;
    
    /**
     * Rebuild connectivity after neuron removal
     */
    void rebuildConnectivityAfterRemoval(const std::vector<size_t>& neuronsToRemove);
    
    /**
     * Optimize direct lookup table
     */
    void optimizeDirectLookup();
    
    /**
     * Record connection lookup timing
     */
    void recordLookupTime();
    
    // Configuration
    size_t neuronCount_;
    float connectionProbability_;
    mutable size_t totalConnections_;
    bool enableCompression_;
    bool enableDirectLookup_;
    bool enableCacheOptimizations_;
    bool enableAdaptiveConnectivity_;
    size_t compressAtSize_;
    size_t maxConnectionsPerNeuron_;
    
    // Adjacency lists
    std::vector<std::vector<SynapseConnection>> outgoing_;
    std::vector<std::vector<SynapseConnection>> incoming_;
    
    // Direct lookup table for O(1) access
    std::vector<std::vector<SynapseId>> directLookup_;
    
    // Statistics
    mutable ConnectivityStats stats_;
    
    // Performance tracking
    mutable double lookupTimeTotal_;
    mutable size_t lookupCount_;
    mutable std::chrono::high_resolution_clock::time_point lastLookupTime_;
    
    // Batch processing buffers
    std::vector<SynapseConnection> batchBuffer_;
    std::vector<SynapseConnection> sparseBatchBuffer_;
    
    // Adaptive connectivity
    std::unique_ptr<ConnectivitySampler> connectivitySampler_;
};

} // namespace nlm