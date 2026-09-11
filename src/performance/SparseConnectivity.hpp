// Sparse connectivity system for NLM
// Manages sparse neural connectivity for improved memory efficiency

#pragma once

#include <unordered_map>
#include <vector>
#include <set>

namespace nlm {

/**
 * Sparse connectivity system for neural networks
 * Manages sparse connectivity patterns for memory efficiency
 */
class SparseConnectivity {
public:
    SparseConnectivity();
    ~SparseConnectivity();
    
    // Connectivity management
    void addConnection(uint64_t fromNeuron, uint64_t toNeuron, float weight);
    void removeConnection(uint64_t fromNeuron, uint64_t toNeuron);
    bool hasConnection(uint64_t fromNeuron, uint64_t toNeuron) const;
    float getConnectionWeight(uint64_t fromNeuron, uint64_t toNeuron) const;
    void setConnectionWeight(uint64_t fromNeuron, uint64_t toNeuron, float weight);
    
    // Neuron connectivity queries
    std::vector<uint64_t> getOutgoingConnections(uint64_t neuron) const;
    std::vector<uint64_t> getIncomingConnections(uint64_t neuron) const;
    size_t getConnectionCount(uint64_t neuron) const;
    
    // Network-wide operations
    void clear();
    size_t getTotalConnectionCount() const;
    std::vector<std::pair<uint64_t, uint64_t>> getAllConnections() const;
    
    // Sparsity metrics
    float getSparsity() const;
    float getDensity() const;
    float getConnectionRatio() const;
    
    // Configuration
    void setMaxConnectionsPerNeuron(size_t maxConnections);
    size_t getMaxConnectionsPerNeuron() const;
    
    // Graph algorithms
    std::vector<uint64_t> findConnectedComponents() const;
    bool isConnected(uint64_t neuron1, uint64_t neuron2) const;
    std::vector<uint64_t> getShortestPath(uint64_t from, uint64_t to) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
