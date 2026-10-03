#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/STDP.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <limits>

namespace nlm {

struct AssociativeMemory::Impl {
    std::vector<Association> associations;
    size_t maxAssociations;
    float associationDecayRate;
    float inhibitionRadius;
    float learningRate;
    float contextMatchingThreshold;
    
    // Network structure
    std::unordered_map<NeuronId, std::vector<size_t>> neuronToAssocIndex;
    std::unordered_map<std::string, std::vector<size_t>> contextToAssocIndex;
    
    // Network statistics
    float networkDensity;
    float avgAssociationStrength;
    float clusteringCoefficient;
    float characteristicPathLength;
    
    // Learning parameters
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTimeConstant;
    
    // Network topology
    std::unordered_map<NeuronId, std::vector<NeuronId>> adjacencyList;
    
    Impl(size_t max) : maxAssociations(max), associationDecayRate(0.001f),
                       inhibitionRadius(0.5f), learningRate(0.01f),
                       contextMatchingThreshold(0.7f), networkDensity(0.0f),
                       avgAssociationStrength(0.0f), clusteringCoefficient(0.0f),
                       characteristicPathLength(0.0f), stdpLtpWeight(0.02f),
                       stdpLtdWeight(0.015f), stdpTimeConstant(20.0f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl(5000)) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength, const std::string& context) {
    // Remove existing reverse association
    for (auto it = pImpl->associations.begin(); it != pImpl->associations.end(); ++it) {
        if ((it->a == b && it->b == a) || (it->a == a && it->b == b)) {
            // Remove old association
            removeAssociation(it - pImpl->associations.begin(), a, b);
            break;
        }
    }
    
    // Apply decay to existing associations
    for (auto& assoc : pImpl->associations) {
        assoc.strength *= (1.0f - pImpl->associationDecayRate);
        if (assoc.context.empty()) {
            assoc.eligibility *= 0.9f;  // Decay eligibility for context-free associations
        }
    }
    
    // Add new association if within capacity
    if (pImpl->associations.size() < pImpl->maxAssociations) {
        Association newAssoc(a, b, strength, context);
        pImpl->associations.push_back(newAssoc);
        updateNetworkIndex(pImpl->associations.size() - 1);
        updateNetworkTopology();
    } else {
        // Replace lowest strength association
        size_t minIndex = std::min_element(pImpl->associations.begin(), pImpl->associations.end(),
                                         [](const Association& a, const Association& b) {
                                             return a.strength < b.strength;
                                         }) - pImpl->associations.begin();
        
        pImpl->associations[minIndex] = Association(a, b, strength, context);
        updateNetworkIndex(minIndex);
        updateNetworkTopology();
    }
}

void AssociativeMemory::updateNetworkIndex(size_t index) {
    const Association& assoc = pImpl->associations[index];
    
    // Update neuron to association index
    pImpl->neuronToAssocIndex[assoc.a].push_back(index);
    pImpl->neuronToAssocIndex[assoc.b].push_back(index);
    
    // Update context to association index
    if (!assoc.context.empty()) {
        pImpl->contextToAssocIndex[assoc.context].push_back(index);
    }
}

void AssociativeMemory::updateNetworkTopology() {
    // Rebuild adjacency list
    pImpl->adjacencyList.clear();
    
    for (const auto& assoc : pImpl->associations) {
        pImpl->adjacencyList[assoc.a].push_back(assoc.b);
        pImpl->adjacencyList[assoc.b].push_back(assoc.a);
    }
    
    // Calculate network statistics
    calculateNetworkStatistics();
}

void AssociativeMemory::calculateNetworkStatistics() {
    size_t totalNodes = pImpl->adjacencyList.size();
    size_t totalConnections = pImpl->associations.size();
    
    if (totalNodes < 2 || totalConnections < 1) {
        pImpl->networkDensity = 0.0f;
        pImpl->avgAssociationStrength = 0.0f;
        pImpl->clusteringCoefficient = 0.0f;
        pImpl->characteristicPathLength = 0.0f;
        return;
    }
    
    // Calculate density
    pImpl->networkDensity = static_cast<float>(totalConnections) / 
                           (totalNodes * (totalNodes - 1) / 2.0f);
    
    // Calculate average association strength
    float totalStrength = 0.0f;
    for (const auto& assoc : pImpl->associations) {
        totalStrength += assoc.strength;
    }
    pImpl->avgAssociationStrength = totalStrength / totalConnections;
    
    // Calculate clustering coefficient (simplified)
    float totalClustering = 0.0f;
    size_t nodeCount = 0;
    
    for (const auto& pair : pImpl->adjacencyList) {
        NeuronId node = pair.first;
        const auto& neighbors = pair.second;
        
        if (neighbors.size() < 2) continue;
        
        size_t connectedPairs = 0;
        for (size_t i = 0; i < neighbors.size(); ++i) {
            for (size_t j = i + 1; j < neighbors.size(); ++j) {
                // Check if neighbors are connected
                bool connected = false;
                for (const auto& assoc : pImpl->associations) {
                    if ((assoc.a == neighbors[i] && assoc.b == neighbors[j]) ||
                        (assoc.a == neighbors[j] && assoc.b == neighbors[i])) {
                        connected = true;
                        break;
                    }
                }
                if (connected) connectedPairs++;
            }
        }
        
        totalClustering += static_cast<float>(connectedPairs) / (neighbors.size() * (neighbors.size() - 1) / 2.0f);
        nodeCount++;
    }
    
    pImpl->clusteringCoefficient = nodeCount > 0 ? totalClustering / nodeCount : 0.0f;
    
    // Calculate characteristic path length (simplified - average shortest path)
    std::vector<float> pathLengths;
    size_t pathSamples = 0;
    
    // Sample some node pairs
    std::vector<NeuronId> nodes;
    for (const auto& pair : pImpl->adjacencyList) {
        nodes.push_back(pair.first);
    }
    
    if (nodes.size() < 2) return;
    
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<size_t> dist(0, nodes.size() - 1);
    
    for (size_t sample = 0; sample < std::min<size_t>(100, nodes.size() * 2); ++sample) {
        NeuronId start = nodes[dist(rng)];
        NeuronId end = nodes[dist(rng)];
        
        if (start == end) continue;
        
        float pathLength = calculateShortestPath(start, end);
        if (pathLength < std::numeric_limits<float>::max()) {
            pathLengths.push_back(pathLength);
            pathSamples++;
        }
    }
    
    if (!pathLengths.empty()) {
        float totalLength = std::accumulate(pathLengths.begin(), pathLengths.end(), 0.0f);
        pImpl->characteristicPathLength = totalLength / pathSamples;
    } else {
        pImpl->characteristicPathLength = 0.0f;
    }
}

float AssociativeMemory::calculateShortestPath(NeuronId start, NeuronId end) const {
    if (start == end) return 0.0f;
    
    std::unordered_map<NeuronId, float> distances;
    for (const auto& pair : pImpl->adjacencyList) {
        distances[pair.first] = std::numeric_limits<float>::max();
    }
    
    std::queue<NeuronId> queue;
    queue.push(start);
    distances[start] = 0.0f;
    
    while (!queue.empty()) {
        NeuronId current = queue.front();
        queue.pop();
        
        for (NeuronId neighbor : pImpl->adjacencyList[current]) {
            if (distances[neighbor] > distances[current] + 1.0f) {
                distances[neighbor] = distances[current] + 1.0f;
                queue.push(neighbor);
                
                if (neighbor == end) {
                    return distances[neighbor];
                }
            }
        }
    }
    
    return std::numeric_limits<float>::max();
}

void AssociativeMemory::removeAssociation(size_t index, NeuronId a, NeuronId b) {
    if (index >= pImpl->associations.size()) return;
    
    // Remove from all indexes
    const Association& assoc = pImpl->associations[index];
    
    auto& neuronIndexA = pImpl->neuronToAssocIndex[assoc.a];
    neuronIndexA.erase(std::remove(neuronIndexA.begin(), neuronIndexA.end(), index), neuronIndexA.end());
    
    auto& neuronIndexB = pImpl->neuronToAssocIndex[assoc.b];
    neuronIndexB.erase(std::remove(neuronIndexB.begin(), neuronIndexB.end(), index), neuronIndexB.end());
    
    if (!assoc.context.empty()) {
        auto& contextIndex = pImpl->contextToAssocIndex[assoc.context];
        contextIndex.erase(std::remove(contextIndex.begin(), contextIndex.end(), index), contextIndex.end());
    }
    
    // Remove from associations
    pImpl->associations.erase(pImpl->associations.begin() + index);
    
    // Update remaining indexes
    for (auto& pair : pImpl->neuronToAssocIndex) {
        for (size_t& assocIndex : pair.second) {
            if (assocIndex > index) {
                assocIndex--;
            }
        }
    }
    
    for (auto& pair : pImpl->contextToAssocIndex) {
        for (size_t& assocIndex : pair.second) {
            if (assocIndex > index) {
                assocIndex--;
            }
        }
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    auto it = pImpl->neuronToAssocIndex.find(neuron);
    if (it != pImpl->neuronToAssocIndex.end()) {
        for (size_t index : it->second) {
            if (index < pImpl->associations.size()) {
                const Association& assoc = pImpl->associations[index];
                if (assoc.a == neuron) {
                    result.push_back(assoc.b);
                } else {
                    result.push_back(assoc.a);
                }
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((assoc.a == a && assoc.b == b) || (assoc.a == b && assoc.b == a)) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

float AssociativeMemory::getAssociationStrengthWithContext(NeuronId a, NeuronId b, const std::string& context) const {
    for (const auto& assoc : pImpl->associations) {
        if (((assoc.a == a && assoc.b == b) || (assoc.a == b && assoc.b == a)) && 
            assoc.context == context) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

float AssociativeMemory::getMeanAssociationStrength(NeuronId neuron) const {
    std::vector<float> strengths;
    auto it = pImpl->neuronToAssocIndex.find(neuron);
    
    if (it != pImpl->neuronToAssocIndex.end()) {
        for (size_t index : it->second) {
            if (index < pImpl->associations.size()) {
                strengths.push_back(pImpl->associations[index].strength);
            }
        }
    }
    
    if (strengths.empty()) return 0.0f;
    
    float sum = std::accumulate(strengths.begin(), strengths.end(), 0.0f);
    return sum / strengths.size();
}

std::vector<NeuronId> AssociativeMemory::completePattern(const std::vector<NeuronId>& partialPattern) const {
    return completePatternWithContext(partialPattern, "");
}

std::vector<NeuronId> AssociativeMemory::completePatternWithContext(const std::vector<NeuronId>& partialPattern,
                                                                   const std::string& context) const {
    if (partialPattern.empty()) return {};
    
    std::vector<std::pair<float, NeuronId>> scoredCandidates;
    
    // Find all associations that match the pattern context
    for (const auto& assoc : pImpl->associations) {
        if (!context.empty() && assoc.context != context) continue;
        
        // Check if this association is relevant to the partial pattern
        bool relevant = false;
        for (NeuronId neuron : partialPattern) {
            if ((assoc.a == neuron && neuron != assoc.b) || 
                (assoc.b == neuron && neuron != assoc.a)) {
                relevant = true;
                break;
            }
        }
        
        if (relevant) {
            scoredCandidates.emplace_back(assoc.strength, assoc.a == partialPattern.back() ? assoc.b : assoc.a);
        }
    }
    
    // Sort by strength
    std::sort(scoredCandidates.begin(), scoredCandidates.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top candidates (up to 5)
    std::vector<NeuronId> result;
    for (size_t i = 0; i < std::min<size_t>(5, scoredCandidates.size()); ++i) {
        result.push_back(scoredCandidates[i].second);
    }
    
    return result;
}

std::vector<std::pair<NeuronId, NeuronId>> AssociativeMemory::getTopKAssociations(float minStrength, size_t k) const {
    std::vector<std::pair<float, std::pair<NeuronId, NeuronId>>> scoredAssociations;
    
    for (const auto& assoc : pImpl->associations) {
        if (assoc.strength >= minStrength) {
            scoredAssociations.emplace_back(assoc.strength, std::make_pair(assoc.a, assoc.b));
        }
    }
    
    // Sort by strength
    std::sort(scoredAssociations.begin(), scoredAssociations.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top k
    std::vector<std::pair<NeuronId, NeuronId>> result;
    for (size_t i = 0; i < std::min(k, scoredAssociations.size()); ++i) {
        result.push_back(scoredAssociations[i].second);
    }
    
    return result;
}

std::vector<std::vector<NeuronId>> AssociativeMemory::findAssociationChains(size_t maxChainLength) const {
    std::vector<std::vector<NeuronId>> chains;
    std::unordered_set<NeuronId> visited;
    
    for (const auto& pair : pImpl->adjacencyList) {
        NeuronId start = pair.first;
        if (visited.find(start) != visited.end()) continue;
        
        // DFS to find chain
        std::vector<NeuronId> chain;
        findChain(start, chain, visited, maxChainLength, chain);
        
        if (chain.size() >= 2) {
            chains.push_back(chain);
        }
    }
    
    return chains;
}

void AssociativeMemory::findChain(NeuronId current, std::vector<NeuronId>& chain, 
                                  std::unordered_set<NeuronId>& visited, 
                                  size_t maxChainLength, std::vector<NeuronId>& result) const {
    chain.push_back(current);
    visited.insert(current);
    
    if (chain.size() >= maxChainLength) {
        result = chain;
        chain.pop_back();
        visited.erase(current);
        return;
    }
    
    // Find next neuron in chain
    for (NeuronId neighbor : pImpl->adjacencyList[current]) {
        if (visited.find(neighbor) == visited.end()) {
            findChain(neighbor, chain, visited, maxChainLength, result);
        }
    }
    
    chain.pop_back();
    visited.erase(current);
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.a == a && assoc.b == b) || (assoc.a == b && assoc.b == a)) {
            assoc.strength = std::clamp(assoc.strength + delta * pImpl->learningRate, 0.0f, 1.0f);
            assoc.lastUpdated = 0;  // Reset eligibility
            break;
        }
    }
}

void AssociativeMemory::updateAssociationWithContext(NeuronId a, NeuronId b, float delta, const std::string& context) {
    for (auto& assoc : pImpl->associations) {
        if (((assoc.a == a && assoc.b == b) || (assoc.a == b && assoc.b == a)) && 
            assoc.context == context) {
            assoc.strength = std::clamp(assoc.strength + delta * pImpl->learningRate, 0.0f, 1.0f);
            assoc.lastUpdated = 0;
            return;
        }
    }
    
    // Create new association if it doesn't exist
    associate(a, b, std::max(0.1f, delta * pImpl->learningRate), context);
}

void AssociativeMemory::applySTDPToAssociations(const std::vector<NeuronId>& preSpikes, 
                                                const std::vector<NeuronId>& postSpikes) {
    for (const NeuronId& pre : preSpikes) {
        for (const NeuronId& post : postSpikes) {
            float dt = static_cast<float>(post - pre);
            
            // Calculate STDP delta
            float delta = 0.0f;
            if (dt > 0) {
                delta = pImpl->stdpLtpWeight * std::exp(-dt / pImpl->stdpTimeConstant);
            } else if (dt < 0) {
                delta = -pImpl->stdpLtdWeight * std::exp(dt / pImpl->stdpTimeConstant);
            }
            
            if (delta != 0.0f) {
                // Update or create association
                updateAssociation(pre, post, delta);
            }
        }
    }
}

void AssociativeMemory::applyLateralInhibition(float inhibitionStrength) {
    // Apply lateral inhibition to reduce overall network activity
    for (auto& assoc : pImpl->associations) {
        assoc.strength *= (1.0f - inhibitionStrength);
    }
}

void AssociativeMemory::resolveAssociativeInterference() {
    // Normalize association strengths to prevent runaway excitation
    if (!pImpl->associations.empty()) {
        float maxStrength = 0.0f;
        for (const auto& assoc : pImpl->associations) {
            maxStrength = std::max(maxStrength, assoc.strength);
        }
        
        if (maxStrength > 0.9f) {
            float scaleFactor = 0.9f / maxStrength;
            for (auto& assoc : pImpl->associations) {
                assoc.strength *= scaleFactor;
            }
        }
    }
}

void AssociativeMemory::normalizeAssociationNetwork() {
    // Normalize all association weights to sum to 1 for each neuron
    std::unordered_map<NeuronId, float> totalOutgoing;
    
    for (const auto& assoc : pImpl->associations) {
        totalOutgoing[assoc.a] += assoc.strength;
    }
    
    for (auto& assoc : pImpl->associations) {
        if (totalOutgoing[assoc.a] > 0.0f) {
            assoc.strength /= totalOutgoing[assoc.a];
        }
    }
}

float AssociativeMemory::getNetworkDensity() const {
    return pImpl->networkDensity;
}

float AssociativeMemory::getAverageAssociationStrength() const {
    return pImpl->avgAssociationStrength;
}

float AssociativeMemory::getClusteringCoefficient() const {
    return pImpl->clusteringCoefficient;
}

float AssociativeMemory::getCharacteristicPathLength() const {
    return pImpl->characteristicPathLength;
}

float AssociativeMemory::getNetworkIntegrity() const {
    if (pImpl->associations.empty()) return 0.0f;
    
    // Calculate integrity based on network statistics
    float densityWeight = 0.3f;
    float strengthWeight = 0.4f;
    float clusteringWeight = 0.2f;
    float pathLengthWeight = 0.1f;
    
    float densityScore = std::min(pImpl->networkDensity * 10.0f, 1.0f);  // Normalize
    float strengthScore = pImpl->avgAssociationStrength;
    float clusteringScore = std::min(pImpl->clusteringCoefficient * 10.0f, 1.0f);
    float pathScore = (pImpl->characteristicPathLength > 0) ? 
                      std::max(0.0f, 1.0f - pImpl->characteristicPathLength / 10.0f) : 0.0f;
    
    float integrity = densityWeight * densityScore +
                      strengthWeight * strengthScore +
                      clusteringWeight * clusteringScore +
                      pathLengthWeight * pathScore;
    
    return integrity;
}

std::vector<float> AssociativeMemory::getAssociationDistribution() const {
    std::vector<float> distribution(11, 0.0f);  // 0.0-1.0 in 0.1 increments
    
    for (const auto& assoc : pImpl->associations) {
        int index = static_cast<int>(assoc.strength * 10.0f);
        if (index >= 0 && index < 11) {
            distribution[index]++;
        }
    }
    
    // Normalize to probabilities
    float total = std::accumulate(distribution.begin(), distribution.end(), 0.0f);
    if (total > 0.0f) {
        for (float& count : distribution) {
            count /= total;
        }
    }
    
    return distribution;
}

std::vector<NeuronId> AssociativeMemory::getHighDegreeNodes(float threshold) const {
    std::vector<NeuronId> highDegreeNodes;
    
    for (const auto& pair : pImpl->adjacencyList) {
        NeuronId node = pair.first;
        int degree = static_cast<int>(pair.second.size());
        
        if (static_cast<float>(degree) >= threshold) {
            highDegreeNodes.push_back(node);
        }
    }
    
    return highDegreeNodes;
}

float AssociativeMemory::getNodeInfluence(NeuronId neuron) const {
    if (pImpl->adjacencyList.find(neuron) == pImpl->adjacencyList.end()) {
        return 0.0f;
    }
    
    std::vector<float> pathInfluences;
    
    // Calculate influence through network
    for (NeuronId target : pImpl->adjacencyList) {
        if (target == neuron) continue;
        
        float pathLength = calculateShortestPath(neuron, target);
        if (pathLength < std::numeric_limits<float>::max()) {
            pathInfluences.push_back(1.0f / (pathLength + 1.0f));
        }
    }
    
    if (pathInfluences.empty()) return 0.0f;
    
    float totalInfluence = std::accumulate(pathInfluences.begin(), pathInfluences.end(), 0.0f);
    return totalInfluence / pathInfluences.size();
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->neuronToAssocIndex.clear();
    pImpl->contextToAssocIndex.clear();
    pImpl->adjacencyList.clear();
    
    // Reset statistics
    pImpl->networkDensity = 0.0f;
    pImpl->avgAssociationStrength = 0.0f;
    pImpl->clusteringCoefficient = 0.0f;
    pImpl->characteristicPathLength = 0.0f;
}

size_t AssociativeMemory::getAssociationCount() const {
    return pImpl->associations.size();
}

size_t AssociativeMemory::getNeuronCount() const {
    return pImpl->adjacencyList.size();
}

float AssociativeMemory::getNetworkIntegrity() const {
    return pImpl->networkDensity * pImpl->avgAssociationStrength * 
           pImpl->clusteringCoefficient * std::max(0.0f, 1.0f - pImpl->characteristicPathLength / 10.0f);
}

void AssociativeMemory::setMaxAssociations(size_t max) {
    pImpl->maxAssociations = max;
    // Trim if over capacity
    if (pImpl->associations.size() > max) {
        pImpl->associations.erase(pImpl->associations.begin() + max, pImpl->associations.end());
        updateNetworkTopology();
    }
}

void AssociativeMemory::setAssociationDecayRate(float rate) {
    pImpl->associationDecayRate = std::max(0.0f, std::min(rate, 1.0f));
}

void AssociativeMemory::setInhibitionRadius(float radius) {
    pImpl->inhibitionRadius = std::max(0.0f, radius);
}

void AssociativeMemory::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 1.0f));
}

void AssociativeMemory::setContextMatchingThreshold(float threshold) {
    pImpl->contextMatchingThreshold = std::max(0.0f, std::min(threshold, 1.0f));
}

} // namespace nlm