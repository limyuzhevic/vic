#include "SparseConnectivity.hpp"

namespace nlm {

struct SparseConnectivity::Impl {
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, float>> connections;
    size_t maxConnectionsPerNeuron;
    size_t totalConnectionCount;
};

SparseConnectivity::SparseConnectivity() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxConnectionsPerNeuron = 1000;
    pImpl->totalConnectionCount = 0;
}

SparseConnectivity::~SparseConnectivity() = default;

void SparseConnectivity::addConnection(uint64_t fromNeuron, uint64_t toNeuron, float weight) {
    if (pImpl->connections[fromNeuron].size() >= pImpl->maxConnectionsPerNeuron) {
        return; // Would exceed limit
    }
    
    if (pImpl->connections.find(fromNeuron) == pImpl->connections.end()) {
        pImpl->connections[fromNeuron] = std::unordered_map<uint64_t, float>();
    }
    
    if (pImpl->connections[fromNeuron].find(toNeuron) == pImpl->connections[fromNeuron].end()) {
        pImpl->totalConnectionCount++;
    }
    
    pImpl->connections[fromNeuron][toNeuron] = weight;
}

void SparseConnectivity::removeConnection(uint64_t fromNeuron, uint64_t toNeuron) {
    auto fromIt = pImpl->connections.find(fromNeuron);
    if (fromIt != pImpl->connections.end()) {
        if (fromIt->second.erase(toNeuron)) {
            pImpl->totalConnectionCount--;
        }
        if (fromIt->second.empty()) {
            pImpl->connections.erase(fromIt);
        }
    }
}

bool SparseConnectivity::hasConnection(uint64_t fromNeuron, uint64_t toNeuron) const {
    auto fromIt = pImpl->connections.find(fromNeuron);
    return fromIt != pImpl->connections.end() && 
           fromIt->second.find(toNeuron) != fromIt->second.end();
}

float SparseConnectivity::getConnectionWeight(uint64_t fromNeuron, uint64_t toNeuron) const {
    auto fromIt = pImpl->connections.find(fromNeuron);
    if (fromIt != pImpl->connections.end()) {
        auto toIt = fromIt->second.find(toNeuron);
        if (toIt != fromIt->second.end()) {
            return toIt->second;
        }
    }
    return 0.0f;
}

void SparseConnectivity::setConnectionWeight(uint64_t fromNeuron, uint64_t toNeuron, float weight) {
    addConnection(fromNeuron, toNeuron, weight);
}

std::vector<uint64_t> SparseConnectivity::getOutgoingConnections(uint64_t neuron) const {
    std::vector<uint64_t> connections;
    auto it = pImpl->connections.find(neuron);
    if (it != pImpl->connections.end()) {
        connections.reserve(it->second.size());
        for (const auto& pair : it->second) {
            connections.push_back(pair.first);
        }
    }
    return connections;
}

std::vector<uint64_t> SparseConnectivity::getIncomingConnections(uint64_t neuron) const {
    std::vector<uint64_t> connections;
    for (const auto& fromPair : pImpl->connections) {
        for (const auto& toPair : fromPair.second) {
            if (toPair.first == neuron) {
                connections.push_back(fromPair.first);
            }
        }
    }
    return connections;
}

size_t SparseConnectivity::getConnectionCount(uint64_t neuron) const {
    auto it = pImpl->connections.find(neuron);
    return (it != pImpl->connections.end()) ? it->second.size() : 0;
}

void SparseConnectivity::clear() {
    pImpl->connections.clear();
    pImpl->totalConnectionCount = 0;
}

size_t SparseConnectivity::getTotalConnectionCount() const {
    return pImpl->totalConnectionCount;
}

std::vector<std::pair<uint64_t, uint64_t>> SparseConnectivity::getAllConnections() const {
    std::vector<std::pair<uint64_t, uint64_t>> connections;
    for (const auto& fromPair : pImpl->connections) {
        for (const auto& toPair : fromPair.second) {
            connections.emplace_back(fromPair.first, toPair.first);
        }
    }
    return connections;
}

float SparseConnectivity::getSparsity() const {
    if (pImpl->totalConnectionCount == 0) return 1.0f;
    
    size_t totalPossible = pImpl->connections.size() * pImpl->maxConnectionsPerNeuron;
    return 1.0f - static_cast<float>(pImpl->totalConnectionCount) / totalPossible;
}

float SparseConnectivity::getDensity() const {
    return 1.0f - getSparsity();
}

float SparseConnectivity::getConnectionRatio() const {
    if (pImpl->connections.empty()) return 0.0f;
    size_t totalNeurons = pImpl->connections.size();
    size_t totalConnections = pImpl->totalConnectionCount;
    return static_cast<float>(totalConnections) / (totalNeurons * pImpl->maxConnectionsPerNeuron);
}

void SparseConnectivity::setMaxConnectionsPerNeuron(size_t maxConnections) {
    pImpl->maxConnectionsPerNeuron = maxConnections;
}

size_t SparseConnectivity::getMaxConnectionsPerNeuron() const {
    return pImpl->maxConnectionsPerNeuron;
}

std::vector<uint64_t> SparseConnectivity::findConnectedComponents() const {
    std::vector<uint64_t> allNeurons;
    for (const auto& pair : pImpl->connections) {
        allNeurons.push_back(pair.first);
    }
    
    std::vector<std::vector<uint64_t>> components;
    std::vector<bool> visited(allNeurons.size(), false);
    
    for (size_t i = 0; i < allNeurons.size(); ++i) {
        if (!visited[i]) {
            std::vector<uint64_t> component;
            // BFS would go here
            components.push_back(component);
            visited[i] = true;
        }
    }
    
    return allNeurons; // Simplified - would return component assignments
}

bool SparseConnectivity::isConnected(uint64_t neuron1, uint64_t neuron2) const {
    // Check if there's a path between neurons
    auto fromIt1 = pImpl->connections.find(neuron1);
    auto fromIt2 = pImpl->connections.find(neuron2);
    
    if (fromIt1 == pImpl->connections.end() || fromIt2 == pImpl->connections.end()) {
        return false;
    }
    
    // Simple check - direct connection only
    // A proper implementation would use graph traversal
    return fromIt1->second.find(neuron2) != fromIt1->second.end();
}

std::vector<uint64_t> SparseConnectivity::getShortestPath(uint64_t from, uint64_t to) const {
    std::vector<uint64_t> path;
    
    if (isConnected(from, to)) {
        path.push_back(from);
        path.push_back(to);
    }
    
    return path;
}

} // namespace nlm
