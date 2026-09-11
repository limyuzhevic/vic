// SparseConnectivity.cpp - Implementation of sparse connectivity optimizations

#include "SparseConnectivity.hpp"
#include <random>
#include <algorithm>
#include <immintrin.h>

namespace nlm {

// Factory function for creating sparse connectivity
std::unique_ptr<SparseConnectivityManager> createSparseConnectivity(
    size_t neuronCount, size_t maxConnectionsPerNeuron,
    float connectionProbability, RandomGenerator& rng) {
    return std::make_unique<SparseConnectivityManager>(neuronCount, maxConnectionsPerNeuron,
                                                        connectionProbability, rng);
}

// Default constructor implementation
SparseConnectivityManager::SparseConnectivityManager()
    : neuronCount_(0), maxConnectionsPerNeuron_(0), connectionProbability_(0.0f), 
      rng_(std::random_device{}()) {
}

// Full constructor implementation
SparseConnectivityManager::SparseConnectivityManager(
    size_t neuronCount, size_t maxConnectionsPerNeuron,
    float connectionProbability, RandomGenerator& rng)
    : neuronCount_(neuronCount), maxConnectionsPerNeuron_(maxConnectionsPerNeuron),
      connectionProbability_(connectionProbability), rng_(rng) {
    // Generate initial sparse connectivity
    generateRandomConnectivity();
}

// Generate random sparse connectivity matrix
void SparseConnectivityManager::generateRandomConnectivity() {
    // Clear existing connections
    connections_.clear();
    
    // Pre-allocate memory for efficiency
    connections_.reserve(neuronCount_ * maxConnectionsPerNeuron_);
    
    // Generate connections using vectorized operations when available
#if defined(NLM_SIMD_AVX2) || defined(NLM_SIMD_AVX512)
    generateVectorizedConnectivity();
#else
    generateScalarConnectivity();
#endif
}

// Vectorized connectivity generation (SIMD)
void SparseConnectivityManager::generateVectorizedConnectivity() {
    // Implementation using SIMD instructions for better performance
    // This generates random connections efficiently using vector operations
    
    // For simplicity, we'll implement a scalar version that can be optimized later
    generateScalarConnectivity();
}

// Scalar connectivity generation
void SparseConnectivityManager::generateScalarConnectivity() {
    for (size_t source = 0; source < neuronCount_; ++source) {
        // Determine number of connections for this source neuron
        size_t numConnections = maxConnectionsPerNeuron_;
        
        // For very sparse connectivity, use probabilistic approach
        if (connectionProbability_ < 0.5f) {
            // Use Bernoulli trials to determine actual number of connections
            numConnections = 0;
            for (size_t attempt = 0; attempt < maxConnectionsPerNeuron_; ++attempt) {
                if (rng_.bernoulli(connectionProbability_)) {
                    numConnections++;
                }
            }
        }
        
        // Generate unique target neurons
        std::vector<size_t> targets;
        targets.reserve(numConnections);
        
        // Use reservoir sampling for efficiency
        for (size_t i = 0; i < neuronCount_; ++i) {
            if (i == source) continue;  // No self-connections
            
            // Insert with probability proportional to remaining slots
            if (targets.size() < numConnections) {
                targets.push_back(i);
            } else if (rng_.bernoulli((float)(numConnections) / (i + 1))) {
                // Replace random existing target
                size_t replaceIdx = rng_.uniformInt(0, numConnections - 1);
                targets[replaceIdx] = i;
            }
        }
        
        // Add connections
        for (size_t target : targets) {
            addConnection(source, target);
        }
    }
}

// Add a single connection with weight
void SparseConnectivityManager::addConnection(size_t source, size_t target, float weight) {
    if (source >= neuronCount_ || target >= neuronCount_) {
        return;  // Invalid neuron indices
    }
    
    // Create connection object
    SynapseConnection conn;
    conn.sourceNeuron = source;
    conn.targetNeuron = target;
    conn.weight = weight;
    conn.active = true;
    conn.createdStep = currentStep_;
    
    // Add to adjacency lists
    connections_[source].push_back(conn);
    
    // Update statistics
    totalConnections_++;
}

// Remove a connection
void SparseConnectivityManager::removeConnection(size_t source, size_t target) {
    if (source >= connections_.size()) return;
    
    auto& conns = connections_[source];
    auto it = std::find_if(conns.begin(), conns.end(),
                          [target](const SynapseConnection& conn) {
                              return conn.targetNeuron == target && conn.active;
                          });
    
    if (it != conns.end()) {
        it->active = false;
        totalConnections_--;
    }
}

// Get all active connections from a source neuron
const std::vector<SynapseConnection>& SparseConnectivityManager::getConnections(size_t source) const {
    if (source >= connections_.size()) {
        static const std::vector<SynapseConnection> empty;
        return empty;
    }
    return connections_[source];
}

// Get connection count for a source neuron
// Get total number of active connections
size_t SparseConnectivityManager::getConnectionCount(size_t source) const {
    if (source >= connections_.size()) return 0;
    
    size_t count = 0;
    for (const auto& conn : connections_[source]) {
        if (conn.active) count++;
    }
    return count;
}

// Get total connections across all neurons
size_t SparseConnectivityManager::getTotalConnections() const {
    return totalConnections_;
}

// Check if connection exists and is active
bool SparseConnectivityManager::hasConnection(size_t source, size_t target) const {
    if (source >= connections_.size()) return false;
    
    for (const auto& conn : connections_[source]) {
        if (conn.targetNeuron == target && conn.active) {
            return true;
        }
    }
    return false;
}

// Update connection weights (plasticity)
void SparseConnectivityManager::updateWeights(float learningRate, float weightDecay) {
    for (auto& sourceConns : connections_) {
        for (auto& conn : sourceConns) {
            if (!conn.active) continue;
            
            // Apply learning rate
            conn.weight *= (1.0f - learningRate);
            
            // Apply weight decay
            conn.weight *= (1.0f - weightDecay);
            
            // Clamp weight to reasonable range
            conn.weight = std::max(-1.0f, std::min(1.0f, conn.weight));
        }
    }
}

// Simulate synaptic transmission
double SparseConnectivityManager::transmitSpikes(size_t source, double currentStep) {
    if (source >= connections_.size()) return 0.0;
    
    double totalInput = 0.0;
    
    // Process each outgoing connection
    for (const auto& conn : connections_[source]) {
        if (!conn.active) continue;
        
        // Calculate synaptic delay
        double delay = calculateSynapticDelay(conn);
        
        // Check if spike should be delivered at current step
        if (currentStep - conn.createdStep >= delay) {
            double weight = conn.weight;
            
            // Apply spike transmission
            totalInput += weight;
            
            // Update post-synaptic neuron
            // Note: This would call into the actual neural simulation
            // For now, we just return the total input
        }
    }
    
    return totalInput;
}

// Calculate synaptic delay based on biological principles
double SparseConnectivityManager::calculateSynapticDelay(const SynapseConnection& conn) const {
    // Base delay between 1-20ms (0.001-0.02 in simulation time)
    // Can be affected by synapse type, neuromodulation, etc.
    
    double baseDelay = 0.005;  // 5ms default
    
    // Add some randomness for biological variability
    double randomFactor = 0.8f + 0.4f * rng_.uniformReal(0.0f, 1.0f);
    
    return baseDelay * randomFactor;
}

// Clean up inactive connections
void SparseConnectivityManager::cleanupConnections() {
    size_t cleanedCount = 0;
    
    for (auto& sourceConns : connections_) {
        auto& conns = sourceConns;
        auto newEnd = std::remove_if(conns.begin(), conns.end(),
                                    [](const SynapseConnection& conn) {
                                        return !conn.active;
                                    });
        cleanedCount += std::distance(newEnd, conns.end());
        conns.erase(newEnd, conns.end());
    }
    
    totalConnections_ -= cleanedCount;
}

// Get connection statistics
SparseConnectivityManager::ConnectivityStats SparseConnectivityManager::getStats() const {
    ConnectivityStats stats;
    
    // Calculate basic statistics
    stats.totalConnections = totalConnections_;
    if (neuronCount_ > 0) {
        stats.connectionsPerNeuron = static_cast<float>(totalConnections_) / neuronCount_;
    }
    
    // Count active and inactive connections
    size_t activeConns = 0;
    for (const auto& sourceConns : connections_) {
        for (const auto& conn : sourceConns) {
            if (conn.active) activeConns++;
        }
    }
    stats.activeConnections = activeConns;
    stats.inactiveConnections = totalConnections_ - activeConns;
    
    return stats;
}

// Print connectivity statistics
void SparseConnectivityManager::printStats() const {
    auto stats = getStats();
    
    std::cout << "=== Sparse Connectivity Statistics ===" << std::endl;
    std::cout << "Total connections: " << stats.totalConnections << std::endl;
    std::cout << "Active connections: " << stats.activeConnections << std::endl;
    std::cout << "Inactive connections: " << stats.inactiveConnections << std::endl;
    std::cout << "Connections per neuron: " << stats.connectionsPerNeuron << std::endl;
    std::cout << "Neuron count: " << neuronCount_ << std::endl;
    std::cout << "=====================================" << std::endl;
}

// Advance simulation step
void SparseConnectivityManager::advanceStep() {
    currentStep_++;
}

} // namespace nlm