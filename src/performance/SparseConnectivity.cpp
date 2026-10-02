#include "SparseConnectivity.hpp"
#include <algorithm>
#include <unordered_map>
#include <set>

namespace nlm {

SparseConnectivity::SparseConnectivity(size_t maxNeurons, size_t maxSynapses) {
    neurons_.resize(maxNeurons);
    adjacencyList_.reserve(maxNeurons);
    synapseIndexMap_.reserve(maxSynapses);
}

SparseConnectivity::~SparseConnectivity() = default;

bool SparseConnectivity::addConnection(NeuronId source, NeuronId destination, float weight) {
    // Check if source neuron exists
    if (source.neuronIndex >= neurons_.size() || source.blockIndex >= neurons_[source.neuronIndex].size()) {
        return false;
    }
    
    // Check if destination neuron exists
    if (destination.neuronIndex >= neurons_.size() || destination.blockIndex >= neurons_[destination.neuronIndex].size()) {
        return false;
    }
    
    // Check if connection already exists
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt != adjacencyList_.end()) {
        for (const auto& conn : sourceIt->second) {
            if (conn.destination == destination) {
                return false;  // Connection already exists
            }
        }
    }
    
    // Add connection
    Connection conn;
    conn.destination = destination;
    conn.weight = weight;
    conn.delay = 1;  // Default delay
    conn.synapseId = synapseIndexMap_.size();
    
    adjacencyList_[source].push_back(conn);
    synapseIndexMap_[{source, destination}] = conn.synapseId;
    
    return true;
}

bool SparseConnectivity::removeConnection(NeuronId source, NeuronId destination) {
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end()) {
        return false;
    }
    
    auto destIt = std::find_if(sourceIt->second.begin(), sourceIt->second.end(),
                              [&destination](const Connection& conn) { return conn.destination == destination; });
    
    if (destIt == sourceIt->second.end()) {
        return false;
    }
    
    // Remove from adjacency list
    sourceIt->second.erase(destIt);
    
    // Remove from synapse index map
    synapseIndexMap_.erase({source, destination});
    
    return true;
}

bool SparseConnectivity::getConnection(NeuronId source, NeuronId destination, Connection& conn) const {
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end()) {
        return false;
    }
    
    auto destIt = std::find_if(sourceIt->second.begin(), sourceIt->second.end(),
                              [&destination](const Connection& c) { return c.destination == destination; });
    
    if (destIt == sourceIt->second.end()) {
        return false;
    }
    
    conn = *destIt;
    return true;
}

std::vector<SynapseId> SparseConnectivity::getOutgoingConnections(NeuronId source) const {
    std::vector<SynapseId> result;
    
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end()) {
        return result;
    }
    
    result.reserve(sourceIt->second.size());
    for (const auto& conn : sourceIt->second) {
        result.push_back(conn.synapseId);
    }
    
    return result;
}

std::vector<SynapseId> SparseConnectivity::getIncomingConnections(NeuronId destination) const {
    std::vector<SynapseId> result;
    
    for (const auto& sourceEntry : adjacencyList_) {
        for (const auto& conn : sourceEntry.second) {
            if (conn.destination == destination) {
                result.push_back(conn.synapseId);
            }
        }
    }
    
    return result;
}

float SparseConnectivity::getConnectionWeight(NeuronId source, NeuronId destination) const {
    Connection conn;
    if (getConnection(source, destination, conn)) {
        return conn.weight;
    }
    return 0.0f;
}

void SparseConnectivity::setConnectionWeight(NeuronId source, NeuronId destination, float weight) {
    Connection conn;
    if (getConnection(source, destination, conn)) {
        conn.weight = weight;
        // Update in adjacency list
        auto sourceIt = adjacencyList_.find(source);
        if (sourceIt != adjacencyList_.end()) {
            for (auto& c : sourceIt->second) {
                if (c.destination == destination) {
                    c.weight = weight;
                    break;
                }
            }
        }
    }
}

void SparseConnectivity::addNeuron(NeuronId id) {
    neurons_[id.neuronIndex].insert(id.blockIndex);
}

void SparseConnectivity::removeNeuron(NeuronId id) {
    neurons_[id.neuronIndex].erase(id.blockIndex);
    
    // Remove all outgoing connections
    auto sourceIt = adjacencyList_.find(id);
    if (sourceIt != adjacencyList_.end()) {
        adjacencyList_.erase(sourceIt);
    }
    
    // Remove all incoming connections
    for (auto it = adjacencyList_.begin(); it != adjacencyList_.end(); ) {
        auto& connections = it->second;
        connections.erase(std::remove_if(connections.begin(), connections.end(),
                                        [&id](const Connection& conn) { return conn.destination == id; }),
                         connections.end());
        if (connections.empty()) {
            it = adjacencyList_.erase(it);
        } else {
            ++it;
        }
    }
}

bool SparseConnectivity::neuronExists(NeuronId id) const {
    return neurons_[id.neuronIndex].find(id.blockIndex) != neurons_[id.neuronIndex].end();
}

bool SparseConnectivity::hasConnection(NeuronId source, NeuronId destination) const {
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end()) {
        return false;
    }
    
    return std::any_of(sourceIt->second.begin(), sourceIt->second.end(),
                      [&destination](const Connection& conn) { return conn.destination == destination; });
}

size_t SparseConnectivity::getNeuronCount() const {
    size_t count = 0;
    for (const auto& neuronBlocks : neurons_) {
        count += neuronBlocks.size();
    }
    return count;
}

size_t SparseConnectivity::getSynapseCount() const {
    return synapseIndexMap_.size();
}

size_t SparseConnectivity::getConnectionCount() const {
    size_t count = 0;
    for (const auto& sourceEntry : adjacencyList_) {
        count += sourceEntry.second.size();
    }
    return count;
}

size_t SparseConnectivity::getConnectionCount(NeuronId source) const {
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end()) {
        return 0;
    }
    return sourceIt->second.size();
}

size_t SparseConnectivity::getConnectionCount(NeuronId source, NeuronId destination) const {
    auto sourceIt = adjacencyList_.find(source);
    if (sourceIt == adjacencyList_.end()) {
        return 0;
    }
    
    return std::count_if(sourceIt->second.begin(), sourceIt->second.end(),
                        [&destination](const Connection& conn) { return conn.destination == destination; });
}

float SparseConnectivity::getAverageConnectionWeight() const {
    if (synapseIndexMap_.empty()) {
        return 0.0f;
    }
    
    float totalWeight = 0.0f;
    size_t connectionCount = 0;
    
    for (const auto& sourceEntry : adjacencyList_) {
        for (const auto& conn : sourceEntry.second) {
            totalWeight += conn.weight;
            connectionCount++;
        }
    }
    
    return totalWeight / connectionCount;
}

float SparseConnectivity::getMaxConnectionWeight() const {
    float maxWeight = 0.0f;
    
    for (const auto& sourceEntry : adjacencyList_) {
        for (const auto& conn : sourceEntry.second) {
            if (conn.weight > maxWeight) {
                maxWeight = conn.weight;
            }
        }
    }
    
    return maxWeight;
}

float SparseConnectivity::getMinConnectionWeight() const {
    float minWeight = 1.0f;
    
    for (const auto& sourceEntry : adjacencyList_) {
        for (const auto& conn : sourceEntry.second) {
            if (conn.weight < minWeight && conn.weight > 0.0f) {
                minWeight = conn.weight;
            }
        }
    }
    
    return minWeight;
}

void SparseConnectivity::clear() {
    neurons_.clear();
    adjacencyList_.clear();
    synapseIndexMap_.clear();
}

size_t SparseConnectivity::memoryUsage() const {
    size_t total = 0;
    
    // Estimate adjacency list memory
    for (const auto& sourceEntry : adjacencyList_) {
        total += sizeof(NeuronId) + sizeof(Connection) * sourceEntry.second.size();
    }
    
    // Estimate synapse index map memory
    total += sizeof(std::pair<NeuronId, NeuronId>) * synapseIndexMap_.size() + sizeof(size_t) * synapseIndexMap_.size();
    
    return total;
}

bool SparseConnectivity::isSymmetric() const {
    // Check if for every A->B connection there exists a B->A connection
    for (const auto& sourceEntry : adjacencyList_) {
        NeuronId source = sourceEntry.first;
        for (const auto& conn : sourceEntry.second) {
            if (!hasConnection(conn.destination, source)) {
                return false;
            }
        }
    }
    return true;
}

bool SparseConnectivity::hasSelfConnections() const {
    // Check for any neuron connecting to itself
    for (const auto& sourceEntry : adjacencyList_) {
        NeuronId source = sourceEntry.first;
        if (std::any_of(sourceEntry.second.begin(), sourceEntry.second.end(),
                       [&source](const Connection& conn) { return conn.destination == source; })) {
            return true;
        }
    }
    return false;
}

void SparseConnectivity::randomizeConnections(float connectionProbability) {
    // Randomize connections (placeholder)
}

void SparseConnectivity::applyWeightScaling(float scaleFactor) {
    for (auto& sourceEntry : adjacencyList_) {
        for (auto& conn : sourceEntry.second) {
            conn.weight *= scaleFactor;
        }
    }
}

void SparseConnectivity::applyWeightLimit(float maxWeight) {
    for (auto& sourceEntry : adjacencyList_) {
        for (auto& conn : sourceEntry.second) {
            if (conn.weight > maxWeight) {
                conn.weight = maxWeight;
            }
        }
    }
}

std::vector<NeuronId> SparseConnectivity::getAllSources() const {
    std::vector<NeuronId> result;
    result.reserve(adjacencyList_.size());
    for (const auto& sourceEntry : adjacencyList_) {
        result.push_back(sourceEntry.first);
    }
    return result;
}

std::vector<NeuronId> SparseConnectivity::getAllDestinations() const {
    std::vector<NeuronId> result;
    std::set<NeuronId> destinations;
    
    for (const auto& sourceEntry : adjacencyList_) {
        for (const auto& conn : sourceEntry.second) {
            destinations.insert(conn.destination);
        }
    }
    
    result.assign(destinations.begin(), destinations.end());
    return result;
}

std::vector<Connection> SparseConnectivity::getAllConnections() const {
    std::vector<Connection> result;
    
    for (const auto& sourceEntry : adjacencyList_) {
        for (const auto& conn : sourceEntry.second) {
            result.push_back(conn);
        }
    }
    
    return result;
}

} // namespace nlm