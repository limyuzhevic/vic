#include "NeuromodulatorNetwork.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <random>

namespace nlm {

struct NeuromodulatorNetwork::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    NetworkTopology topology;
    DiffusionState diffusionState;
    NetworkMetrics metrics;
    
    // Network configuration
    int numNodes;
    float connectionDensity;
    float diffusionRate;
    float propagationSpeed;
    
    // Network state
    std::vector<float> nodeActivity;
    std::vector<float> node excitability;
    std::vector<float> nodeInhibition;
    std::vector<std::vector<float>> concentrationGrid;
    
    // Hub detection
    std::vector<size_t> hubNodes;
    std::vector<float> hubStrength;
    
    // Community detection
    std::vector<size_t> communityAssignments;
    std::vector<std::vector<size_t>> communityMatrix;
    
    // Performance tracking
    std::vector<float> efficiencyHistory;
    std::vector<float> pathLengthHistory;
    std::vector<float> clusteringHistory;
    
    // Safety bounds
    float minEfficiency;
    float maxEfficiency;
    float minPathLength;
    float maxPathLength;
    float minClustering;
    float maxClustering;
    
    Impl() : numNodes(100), connectionDensity(0.1f), diffusionRate(0.1f),
             propagationSpeed(1.0f), minEfficiency(0.1f), maxEfficiency(1.0f),
             minPathLength(1.0f), maxPathLength(10.0f), minClustering(0.0f),
             maxClustering(1.0f) {
        
        // Initialize node vectors
        nodeActivity.resize(numNodes, 0.0f);
        nodeExcitability.resize(numNodes, 1.0f);
        nodeInhibition.resize(numNodes, 0.0f);
        concentrationGrid.resize(numNodes);
        for (auto& row : concentrationGrid) {
            row.resize(numNodes, 0.0f);
        }
        
        // Initialize hub detection
        hubNodes.reserve(numNodes / 10);
        hubStrength.reserve(numNodes / 10);
        
        // Initialize community assignments
        communityAssignments.resize(numNodes, 0);
        
        // Initialize topology
        topology.adjacencyMatrix.resize(numNodes);
        for (auto& row : topology.adjacencyMatrix) {
            row.resize(numNodes, 0);
        }
    }
};

NeuromodulatorNetwork::NeuromodulatorNetwork() : pImpl(new Impl) {
    NLM_LOG_INFO("NeuromodulatorNetwork created");
}

NeuromodulatorNetwork::~NeuromodulatorNetwork() {
    NLM_LOG_INFO("NeuromodulatorNetwork destroyed");
}

NeuromodulatorNetwork::NeuromodulatorNetwork(NeuromodulatorNetwork&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

NeuromodulatorNetwork& NeuromodulatorNetwork::operator=(NeuromodulatorNetwork&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
        if (pImpl) {
            pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
            pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
        }
    }
    return *this;
}

bool NeuromodulatorNetwork::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("NeuromodulatorNetwork initialization failed: invalid dependencies");
            return false;
        }
        
        // Initialize network topology
        computeTopology();
        
        // Initialize diffusion state
        for (size_t i = 0; i < pImpl->diffusionState.concentration.size(); ++i) {
            pImpl->diffusionState.concentration[i] = 0.1f;  // Baseline concentration
        }
        
        NLM_LOG_INFO("NeuromodulatorNetwork initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuromodulatorNetwork initialization failed: " + std::string(e.what()));
        return false;
    }
}

void NeuromodulatorNetwork::update(TimestepDuration dt, SimulationStep step) {
    // Propagate neuromodulator diffusion
    propagateDiffusion({});
    
    // Update local modulation
    updateLocalModulation();
    
    // Update global modulation
    updateGlobalModulation();
    
    // Update network state
    updateState(dt);
    
    // Analyze network metrics
    analyzeNetworkMetrics();
}

void NeuromodulatorNetwork::addNode(const NetworkNode& node) {
    topology.nodes.push_back(node);
    
    // Update adjacency matrix
    size_t nodeIndex = topology.nodes.size() - 1;
    topology.adjacencyMatrix.resize(topology.nodes.size());
    for (auto& row : topology.adjacencyMatrix) {
        row.resize(topology.nodes.size(), 0);
    }
    
    // Update other node vectors
    pImpl->nodeActivity.push_back(0.0f);
    pImpl->nodeExcitability.push_back(1.0f);
    pImpl->nodeInhibition.push_back(0.0f);
    pImpl->concentrationGrid.push_back(std::vector<float>(topology.nodes.size(), 0.0f));
    
    // Update diffusion state arrays
    pImpl->diffusionState.concentration.push_back(0.1f);
    pImpl->diffusionState.gradient.push_back(0.0f);
    pImpl->diffusionState.flux.push_back(0.0f);
    pImpl->diffusionState.potential.push_back(0.0f);
    pImpl->diffusionState.sourceTerm.push_back(0.0f);
}

void NeuromodulatorNetwork::removeNode(NeuronId neuronId) {
    // Find node index
    auto it = std::find_if(topology.nodes.begin(), topology.nodes.end(),
                          [neuronId](const NetworkNode& node) { return node.neuronId == neuronId; });
    
    if (it != topology.nodes.end()) {
        size_t index = std::distance(topology.nodes.begin(), it);
        
        // Remove node from all arrays
        topology.nodes.erase(it);
        topology.adjacencyMatrix.erase(topology.adjacencyMatrix.begin() + index);
        pImpl->nodeActivity.erase(pImpl->nodeActivity.begin() + index);
        pImpl->nodeExcitability.erase(pImpl->nodeExcitability.begin() + index);
        pImpl->nodeInhibition.erase(pImpl->nodeInhibition.begin() + index);
        pImpl->concentrationGrid.erase(pImpl->concentrationGrid.begin() + index);
        pImpl->diffusionState.concentration.erase(pImpl->diffusionState.concentration.begin() + index);
        pImpl->diffusionState.gradient.erase(pImpl->diffusionState.gradient.begin() + index);
        pImpl->diffusionState.flux.erase(pImpl->diffusionState.flux.begin() + index);
        pImpl->diffusionState.potential.erase(pImpl->diffusionState.potential.begin() + index);
        pImpl->diffusionState.sourceTerm.erase(pImpl->diffusionState.sourceTerm.begin() + index);
        
        // Update adjacency matrix for remaining nodes
        for (auto& row : topology.adjacencyMatrix) {
            if (index < row.size()) {
                row.erase(row.begin() + index);
            }
        }
    }
}

void NeuromodulatorNetwork::addConnection(const NetworkConnection& connection) {
    topology.connections.push_back(connection);
    
    // Update adjacency matrix
    size_t sourceIndex = std::numeric_limits<size_t>::max();
    size_t targetIndex = std::numeric_limits<size_t>::max();
    
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        if (topology.nodes[i].neuronId == connection.sourceNode.neuronId) {
            sourceIndex = i;
        }
        if (topology.nodes[i].neuronId == connection.targetNode.neuronId) {
            targetIndex = i;
        }
    }
    
    if (sourceIndex != std::numeric_limits<size_t>::max() && 
        targetIndex != std::numeric_limits<size_t>::max() &&
        sourceIndex < topology.adjacencyMatrix.size() &&
        targetIndex < topology.adjacencyMatrix[sourceIndex].size()) {
        
        topology.adjacencyMatrix[sourceIndex][targetIndex] = 1;
    }
}

void NeuromodulatorNetwork::removeConnection(NeuronId sourceId, NeuronId targetId) {
    // Find connection index
    auto it = std::find_if(topology.connections.begin(), topology.connections.end(),
                          [sourceId, targetId](const NetworkConnection& conn) {
                              return conn.sourceNode.neuronId == sourceId && 
                                     conn.targetNode.neuronId == targetId;
                          });
    
    if (it != topology.connections.end()) {
        // Update adjacency matrix
        size_t sourceIndex = std::numeric_limits<size_t>::max();
        size_t targetIndex = std::numeric_limits<size_t>::max();
        
        for (size_t i = 0; i < topology.nodes.size(); ++i) {
            if (topology.nodes[i].neuronId == sourceId) {
                sourceIndex = i;
            }
            if (topology.nodes[i].neuronId == targetId) {
                targetIndex = i;
            }
        }
        
        if (sourceIndex != std::numeric_limits<size_t>::max() && 
            targetIndex != std::numeric_limits<size_t>::max() &&
            sourceIndex < topology.adjacencyMatrix.size() &&
            targetIndex < topology.adjacencyMatrix[sourceIndex].size()) {
            
            topology.adjacencyMatrix[sourceIndex][targetIndex] = 0;
        }
        
        // Remove connection
        topology.connections.erase(it);
    }
}

void NeuromodulatorNetwork::distributeNeuromodulator(const std::string& neuromodulatorName, 
                                                   float concentration, NeuronId sourceId) {
    // Find source node
    auto it = std::find_if(topology.nodes.begin(), topology.nodes.end(),
                          [sourceId](const NetworkNode& node) { return node.neuronId == sourceId; });n
    if (it == topology.nodes.end()) {
        return;
    }
    
    size_t sourceIndex = std::distance(topology.nodes.begin(), it);
    
    // Distribute concentration to nearby nodes
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        if (i != sourceIndex) {
            // Calculate distance based on position
            float dx = topology.nodes[i].positionX - it->positionX;
            float dy = topology.nodes[i].positionY - it->positionY;
            float dz = topology.nodes[i].positionZ - it->positionZ;
            float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            
            // Apply diffusion based on distance
            float influence = std::exp(-distance / pImpl->diffusionRate);
            pImpl->diffusionState.concentration[i] += concentration * influence;
            
            // Update concentration grid
            pImpl->concentrationGrid[sourceIndex][i] += concentration * influence;
        }
    }
}

void NeuromodulatorNetwork::propagateDiffusion(const std::vector<float>& sourceConcentrations) {
    // Reset diffusion state
    std::fill(pImpl->diffusionState.gradient.begin(), pImpl->diffusionState.gradient.end(), 0.0f);
    std::fill(pImpl->diffusionState.flux.begin(), pImpl->diffusionState.flux.end(), 0.0f);
    
    // Compute gradients (finite difference)
    for (size_t i = 0; i < pImpl->diffusionState.concentration.size(); ++i) {
        // Find neighbors in adjacency matrix
        for (size_t j = 0; j < pImpl->topology.adjacencyMatrix.size(); ++j) {
            if (pImpl->topology.adjacencyMatrix[i][j] > 0) {
                float distance = computeNodeDistance(i, j);
                if (distance > 0.0f) {
                    float gradient = (pImpl->diffusionState.concentration[j] - 
                                    pImpl->diffusionState.concentration[i]) / distance;
                    pImpl->diffusionState.gradient[i] += gradient;
                }
            }
        }
    }
    
    // Compute flux (Fick's law)
    for (size_t i = 0; i < pImpl->diffusionState.concentration.size(); ++i) {
        pImpl->diffusionState.flux[i] = -pImpl->diffusionState.gradient[i];
    }
    
    // Update concentrations based on flux
    for (size_t i = 0; i < pImpl->diffusionState.concentration.size(); ++i) {
        float netFlux = 0.0f;
        for (size_t j = 0; j < pImpl->topology.adjacencyMatrix.size(); ++j) {
            if (pImpl->topology.adjacencyMatrix[j][i] > 0) {
                netFlux += pImpl->diffusionState.flux[j];
            }
        }
        
        pImpl->diffusionState.concentration[i] += netFlux * pImpl->propagationSpeed * 0.01f;
        
        // Apply bounds
        pImpl->diffusionState.concentration[i] = 
            std::clamp(pImpl->diffusionState.concentration[i], 0.0f, 1.0f);
    }
}

void NeuromodulatorNetwork::updateLocalModulation() {
    // Update local modulation based on concentration
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        float localConcentration = pImpl->diffusionState.concentration[i];
        
        // Update node excitability and inhibition
        pImpl->nodeExcitability[i] = 1.0f + localConcentration * 2.0f;
        pImpl->nodeInhibition[i] = localConcentration * 0.5f;
        
        // Apply bounds
        pImpl->nodeExcitability[i] = std::clamp(pImpl->nodeExcitability[i], 0.1f, 10.0f);
        pImpl->nodeInhibition[i] = std::clamp(pImpl->nodeInhibition[i], 0.0f, 1.0f);
    }
}

void NeuromodulatorNetwork::updateGlobalModulation() {
    // Update global modulation based on average concentration
    float totalConcentration = 0.0f;
    for (float conc : pImpl->diffusionState.concentration) {
        totalConcentration += conc;
    }
    
    float averageConcentration = totalConcentration / pImpl->diffusionState.concentration.size();
    
    // Apply global modulation to all nodes
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        // Global modulation scales with average concentration
        float globalModulation = averageConcentration * 0.5f;
        
        // Apply to node activity
        pImpl->nodeActivity[i] = 
            pImpl->nodeActivity[i] * (1.0f + globalModulation * 0.1f);
        
        // Apply bounds
        pImpl->nodeActivity[i] = std::clamp(pImpl->nodeActivity[i], 0.0f, 1.0f);
    }
}

void NeuromodulatorNetwork::computeTopology() {
    // Generate random network topology
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Clear existing topology
    topology.nodes.clear();
    topology.connections.clear();
    topology.adjacencyMatrix.clear();
    
    // Generate nodes
    for (int i = 0; i < pImpl->numNodes; ++i) {
        NetworkNode node;
        node.neuronId = NeuronId(static_cast<uint64_t>(i));
        node.positionX = dis(gen) * 10.0f - 5.0f;  // Range -5 to 5
        node.positionY = dis(gen) * 10.0f - 5.0f;
        node.positionZ = dis(gen) * 10.0f - 5.0f;
        node.connectivityStrength = 1.0f;
        
        topology.nodes.push_back(node);
    }
    
    // Generate connections based on density
    int maxConnections = static_cast<int>(topology.nodes.size() * pImpl->connectionDensity);
    for (int i = 0; i < maxConnections; ++i) {
        NetworkConnection conn;
        
        // Random source and target
        int source = static_cast<int>(dis(gen) * topology.nodes.size());
        int target = static_cast<int>(dis(gen) * topology.nodes.size());
        
        if (source != target) {
            conn.sourceNode = topology.nodes[source];
            conn.targetNode = topology.nodes[target];
            conn.connectionStrength = 1.0f;
            
            topology.connections.push_back(conn);
        }
    }
    
    // Update adjacency matrix
    updateAdjacencyMatrix();
    
    // Compute topology metrics
    computeTopologyMetrics();
}

void NeuromodulatorNetwork::updateAdjacencyMatrix() {
    topology.adjacencyMatrix.resize(topology.nodes.size());
    for (auto& row : topology.adjacencyMatrix) {
        row.resize(topology.nodes.size(), 0);
    }
    
    // Fill adjacency matrix
    for (const auto& conn : topology.connections) {
        size_t sourceIndex = std::numeric_limits<size_t>::max();
        size_t targetIndex = std::numeric_limits<size_t>::max();
        
        for (size_t i = 0; i < topology.nodes.size(); ++i) {
            if (topology.nodes[i].neuronId == conn.sourceNode.neuronId) {
                sourceIndex = i;
            }
            if (topology.nodes[i].neuronId == conn.targetNode.neuronId) {
                targetIndex = i;
            }
        }
        
        if (sourceIndex != std::numeric_limits<size_t>::max() && 
            targetIndex != std::numeric_limits<size_t>::max() &&
            sourceIndex < topology.adjacencyMatrix.size() &&
            targetIndex < topology.adjacencyMatrix[sourceIndex].size()) {
            
            topology.adjacencyMatrix[sourceIndex][targetIndex] = 1;
        }
    }
}

void NeuromodulatorNetwork::optimizeNetworkLayout() {
    if (!pImpl->enableSmallWorldTopology_) {
        return;
    }
    
    // Apply small-world network optimization (Watts-Strogatz algorithm)
    // This is a simplified implementation
    
    // Rewire some connections to create small-world properties
    float rewiringProbability = 0.3f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    for (auto& conn : topology.connections) {
        if (dis(gen) < rewiringProbability) {
            // Remove original connection
            removeConnection(conn.sourceNode.neuronId, conn.targetNode.neuronId);
            
            // Add new random connection
            size_t newTarget = static_cast<size_t>(dis(gen) * topology.nodes.size());
            
            // Find target node
            for (const auto& node : topology.nodes) {
                if (static_cast<size_t>(node.neuronId.value) == newTarget) {
                    conn.targetNode = node;
                    addConnection(conn);
                    break;
                }
            }
        }
    }
    
    // Update adjacency matrix after rewiring
    updateAdjacencyMatrix();
}

void NeuromodulatorNetwork::analyzeNetworkMetrics() {
    // Reset metrics
    pImpl->metrics.networkEfficiency = 0.0f;
    pImpl->metrics.characteristicPathLength = 0.0f;
    pImpl->metrics.averageClustering = 0.0f;
    pImpl->metrics.betweennessCentrality = 0.0f;
    pImpl->metrics.closenessCentrality = 0.0f;
    pImpl->metrics.modularityQuality = 0.0f;
    pImpl->metrics.synchronizability = 0.0f;
    pImpl->metrics.signalPropagationSpeed = 0.0f;
    pImpl->metrics.informationFlow = 0.0f;
    pImpl->metrics.resourceUtilization = 0.0f;
    
    if (topology.nodes.size() < 2) return;
    
    // Compute characteristic path length
    pImpl->metrics.characteristicPathLength = computeCharacteristicPathLength();
    
    // Compute clustering coefficient
    pImpl->metrics.averageClustering = computeClusteringCoefficient();
    
    // Compute network efficiency
    pImpl->metrics.networkEfficiency = computeNetworkEfficiency();
    
    // Compute betweenness centrality
    pImpl->metrics.betweennessCentrality = computeBetweennessCentrality();
    
    // Compute closeness centrality
    pImpl->metrics.closenessCentrality = computeClosenessCentrality();
    
    // Compute signal propagation speed
    pImpl->metrics.signalPropagationSpeed = 
        pImpl->propagationSpeed * static_cast<float>(topology.connections.size()) / 
        static_cast<float>(topology.nodes.size());
    
    // Compute information flow
    pImpl->metrics.informationFlow = 
        pImpl->metrics.networkEfficiency * pImpl->metrics.signalPropagationSpeed;
    
    // Compute resource utilization
    float totalConnections = static_cast<float>(topology.connections.size());
    float maxConnections = static_cast<float>(topology.nodes.size() * (topology.nodes.size() - 1));
    pImpl->metrics.resourceUtilization = std::min(1.0f, totalConnections / maxConnections);
    
    // Store history
    pImpl->efficiencyHistory.push_back(pImpl->metrics.networkEfficiency);
    pImpl->pathLengthHistory.push_back(pImpl->metrics.characteristicPathLength);
    pImpl->clusteringHistory.push_back(pImpl->metrics.averageClustering);
    
    if (pImpl->efficiencyHistory.size() > 1000) {
        pImpl->efficiencyHistory.erase(pImpl->efficiencyHistory.begin());
        pImpl->pathLengthHistory.erase(pImpl->pathLengthHistory.begin());
        pImpl->clusteringHistory.erase(pImpl->clusteringHistory.begin());
    }
}

void NeuromodulatorNetwork::detectHubNodes() {
    pImpl->hubNodes.clear();
    pImpl->hubStrength.clear();
    
    if (topology.nodes.empty()) return;
    
    // Compute degree for each node
    std::vector<size_t> degrees(topology.nodes.size(), 0);
    
    for (size_t i = 0; i < topology.adjacencyMatrix.size(); ++i) {
        for (size_t j = 0; j < topology.adjacencyMatrix[i].size(); ++j) {
            if (topology.adjacencyMatrix[i][j] > 0) {
                degrees[i]++;
            }
        }
    }
    
    // Identify hubs (nodes with degree > average + 2*stddev)
    size_t avgDegree = 0;
    if (!degrees.empty()) {
        avgDegree = std::accumulate(degrees.begin(), degrees.end(), 0) / degrees.size();
        
        float stddev = 0.0f;
        for (size_t degree : degrees) {
            stddev += std::pow(degree - avgDegree, 2);
        }
        stddev = std::sqrt(stddev / degrees.size());
        
        float threshold = avgDegree + 2.0f * stddev;
        
        for (size_t i = 0; i < degrees.size(); ++i) {
            if (degrees[i] > threshold) {
                pImpl->hubNodes.push_back(i);
                pImpl->hubStrength.push_back(static_cast<float>(degrees[i]));
            }
        }
    }
}

void NeuromodulatorNetwork::identifyCommunities() {
    // Simplified community detection using connected components
    pImpl->communityAssignments.clear();
    pImpl->communityMatrix.clear();
    
    if (topology.nodes.empty()) return;
    
    // Initialize communities
    pImpl->communityAssignments.resize(topology.nodes.size(), 0);
    pImpl->communityMatrix.resize(topology.nodes.size());
    for (auto& row : pImpl->communityMatrix) {
        row.resize(topology.nodes.size(), 0);
    }
    
    // Simple connected components (placeholder for Louvain/Lancichinetti algorithm)
    std::vector<bool> visited(topology.nodes.size(), false);
    size_t communityId = 0;
    
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        if (!visited[i]) {
            // Perform BFS from node i
            std::vector<size_t> component;
            std::vector<size_t> queue;
            queue.push_back(i);
            visited[i] = true;
            
            while (!queue.empty()) {
                size_t current = queue.back();
                queue.pop_back();
                component.push_back(current);
                
                for (size_t j = 0; j < topology.adjacencyMatrix[current].size(); ++j) {
                    if (topology.adjacencyMatrix[current][j] > 0 && !visited[j]) {
                        visited[j] = true;
                        queue.push_back(j);
                    }
                }
            }
            
            // Assign all nodes in component to same community
            for (size_t nodeId : component) {
                pImpl->communityAssignments[nodeId] = communityId;
            }
            
            communityId++;
        }
    }
}

void NeuromodulatorNetwork::computeInformationFlow() {
    // Compute information flow based on concentration differences and connectivity
    float totalFlow = 0.0f;
    
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        for (size_t j = 0; j < topology.adjacencyMatrix[i].size(); ++j) {
            if (topology.adjacencyMatrix[i][j] > 0) {
                float concentrationDiff = std::abs(
                    pImpl->diffusionState.concentration[i] - 
                    pImpl->diffusionState.concentration[j]);
                
                float flow = concentrationDiff * topology.adjacencyMatrix[i][j] * 
                           pImpl->propagationSpeed;
                
                totalFlow += flow;
            }
        }
    }
    
    pImpl->metrics.informationFlow = totalFlow / 
                                    std::max(1.0f, static_cast<float>(topology.nodes.size() * 
                                                                      topology.nodes.size()));
}

void NeuromodulatorNetwork::updateState(TimestepDuration dt) {
    // Update node activity based on excitability and inhibition
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        float activityChange = pImpl->nodeExcitability[i] - pImpl->nodeInhibition[i];
        pImpl->nodeActivity[i] += activityChange * static_cast<float>(dt);
        
        // Apply bounds
        pImpl->nodeActivity[i] = std::clamp(pImpl->nodeActivity[i], 0.0f, 1.0f);
    }
}

void NeuromodulatorNetwork::integrateWithBrainRegions() {
    if (!pImpl->brain) return;
    
    // Integrate with brain regions based on neuron IDs
    for (const auto& node : topology.nodes) {
        // Find corresponding brain region
        // This would involve mapping neuron IDs to brain regions
        // For now, it's a placeholder
    }
}

void NeuromodulatorNetwork::integrateWithNeuralDynamics() {
    // Integrate with neural dynamics
    if (!pImpl->engine) return;
    
    // Update based on neural activity patterns
    // This would involve reading neural activity and updating
    // neuromodulator distribution accordingly
}

void NeuromodulatorNetwork::integrateWithBehaviorOutput() {
    // Integrate with behavior output
    // This would involve reading behavior output and adjusting
    // network parameters accordingly
}

void NeuromodulatorNetwork::stimulateNode(NeuronId neuronId, float intensity) {
    // Find and stimulate node
    auto it = std::find_if(topology.nodes.begin(), topology.nodes.end(),
                          [neuronId](const NetworkNode& node) { return node.neuronId == neuronId; });
    
    if (it != topology.nodes.end()) {
        size_t index = std::distance(topology.nodes.begin(), it);
        
        // Increase node activity
        pImpl->nodeActivity[index] = std::min(1.0f, 
                                            pImpl->nodeActivity[index] + intensity);
    }
}

void NeuromodulatorNetwork::inhibitNode(NeuronId neuronId, float strength) {
    // Find and inhibit node
    auto it = std::find_if(topology.nodes.begin(), topology.nodes.end(),
                          [neuronId](const NetworkNode& node) { return node.neuronId == neuronId; });
    
    if (it != topology.nodes.end()) {
        size_t index = std::distance(topology.nodes.begin(), it);
        
        // Increase inhibition
        pImpl->nodeInhibition[index] = std::min(1.0f, 
                                              pImpl->nodeInhibition[index] + strength);
    }
}

void NeuromodulatorNetwork::modulateConnection(NeuronId sourceId, NeuronId targetId, float factor) {
    // Find and modulate connection
    auto it = std::find_if(topology.connections.begin(), topology.connections.end(),
                          [sourceId, targetId](const NetworkConnection& conn) {
                              return conn.sourceNode.neuronId == sourceId && 
                                     conn.targetNode.neuronId == targetId;
                          });
    
    if (it != topology.connections.end()) {
        it->connectionStrength *= factor;
        
        // Update adjacency matrix
        updateAdjacencyMatrix();
    }
}

void NeuromodulatorNetwork::exportNetwork(std::ostream& stream) const {
    // Export topology
    size_t nodeCount = topology.nodes.size();
    stream.write(reinterpret_cast<const char*>(&nodeCount), sizeof(size_t));
    
    for (const auto& node : topology.nodes) {
        stream.write(reinterpret_cast<const char*>(&node), sizeof(NetworkNode));
    }
    
    size_t connectionCount = topology.connections.size();
    stream.write(reinterpret_cast<const char*>(&connectionCount), sizeof(size_t));
    
    for (const auto& conn : topology.connections) {
        stream.write(reinterpret_cast<const char*>(&conn), sizeof(NetworkConnection));
    }
}

void NeuromodulatorNetwork::importNetwork(std::istream& stream) {
    // Import topology
    size_t nodeCount;
    stream.read(reinterpret_cast<char*>(&nodeCount), sizeof(size_t));
    
    topology.nodes.resize(nodeCount);
    for (auto& node : topology.nodes) {
        stream.read(reinterpret_cast<char*>(&node), sizeof(NetworkNode));
    }
    
    size_t connectionCount;
    stream.read(reinterpret_cast<char*>(&connectionCount), sizeof(size_t));
    
    topology.connections.resize(connectionCount);
    for (auto& conn : topology.connections) {
        stream.read(reinterpret_cast<char*>(&conn), sizeof(NetworkConnection));
    }
    
    // Update adjacency matrix
    updateAdjacencyMatrix();
}

float NeuromodulatorNetwork::computeNodeDistance(size_t nodeIndex1, size_t nodeIndex2) const {
    if (nodeIndex1 >= topology.nodes.size() || nodeIndex2 >= topology.nodes.size()) {
        return 0.0f;
    }
    
    float dx = topology.nodes[nodeIndex1].positionX - topology.nodes[nodeIndex2].positionX;
    float dy = topology.nodes[nodeIndex1].positionY - topology.nodes[nodeIndex2].positionY;
    float dz = topology.nodes[nodeIndex1].positionZ - topology.nodes[nodeIndex2].positionZ;
    
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void NeuromodulatorNetwork::computeTopologyMetrics() {
    // Compute topology metrics
    topology.averageDegree = 0.0f;
    topology.clusteringCoefficient = 0.0f;
    topology.smallWorldness = 0.0f;
    topology.modularity = 0.0f;
    
    if (topology.nodes.size() < 2) return;
    
    // Compute average degree
    size_t totalConnections = 0;
    for (const auto& row : topology.adjacencyMatrix) {
        for (size_t j = 0; j < row.size(); ++j) {
            if (row[j] > 0) {
                totalConnections++;
            }
        }
    }
    
    topology.averageDegree = static_cast<float>(totalConnections) / 
                             static_cast<float>(topology.nodes.size() * topology.nodes.size());
    
    // Compute clustering coefficient
    topology.clusteringCoefficient = computeClusteringCoefficient();
    
    // Compute small-worldness (placeholder)
    topology.smallWorldness = 1.0f;  // Simplified
    
    // Compute modularity (placeholder)
    topology.modularity = 0.5f;  // Simplified
}

float NeuromodulatorNetwork::computeCharacteristicPathLength() const {
    if (topology.nodes.size() < 2) return 0.0f;
    
    // Compute shortest paths using BFS
    float totalPathLength = 0.0f;
    size_t pathCount = 0;
    
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        for (size_t j = 0; j < topology.nodes.size(); ++j) {
            if (i != j) {
                float pathLength = computeShortestPathLength(i, j);
                totalPathLength += pathLength;
                pathCount++;
            }
        }
    }
    
    if (pathCount == 0) return 0.0f;
    
    return totalPathLength / static_cast<float>(pathCount);
}

float NeuromodulatorNetwork::computeShortestPathLength(size_t start, size_t end) const {
    if (start == end) return 0.0f;
    
    // BFS to find shortest path
    std::vector<size_t> distances(topology.nodes.size(), std::numeric_limits<size_t>::max());
    std::vector<bool> visited(topology.nodes.size(), false);
    std::vector<size_t> queue;
    
    distances[start] = 0;
    visited[start] = true;
    queue.push_back(start);
    
    while (!queue.empty()) {
        size_t current = queue.front();
        queue.erase(queue.begin());
        
        for (size_t j = 0; j < topology.adjacencyMatrix[current].size(); ++j) {
            if (topology.adjacencyMatrix[current][j] > 0 && !visited[j]) {
                distances[j] = distances[current] + 1;
                visited[j] = true;
                queue.push_back(j);
                
                if (j == end) {
                    return static_cast<float>(distances[j]);
                }
            }
        }
    }
    
    return std::numeric_limits<float>::max();  // No path found
}

float NeuromodulatorNetwork::computeClusteringCoefficient() const {
    if (topology.nodes.size() < 3) return 0.0f;
    
    float totalClustering = 0.0f;
    size_t triangleCount = 0;
    size_t possibleTriangles = 0;
    
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        for (size_t j = 0; j < topology.adjacencyMatrix[i].size(); ++j) {
            if (topology.adjacencyMatrix[i][j] > 0) {
                for (size_t k = j + 1; k < topology.adjacencyMatrix[i].size(); ++k) {
                    if (topology.adjacencyMatrix[i][k] > 0) {
                        triangleCount++;
                        possibleTriangles++;
                    }
                    
                    if (topology.adjacencyMatrix[j][k] > 0) {
                        totalClustering++;
                        possibleTriangles++;
                    }
                }
            }
        }
    }
    
    if (possibleTriangles == 0) return 0.0f;
    
    return totalClustering / static_cast<float>(possibleTriangles);
}

float NeuromodulatorNetwork::computeNetworkEfficiency() const {
    if (topology.nodes.size() < 2) return 0.0f;
    
    float totalInverseDistance = 0.0f;
    size_t validPairs = 0;
    
    for (size_t i = 0; i < topology.nodes.size(); ++i) {
        for (size_t j = 0; j < topology.nodes.size(); ++j) {
            if (i != j) {
                float distance = computeShortestPathLength(i, j);
                if (distance < std::numeric_limits<float>::max()) {
                    totalInverseDistance += 1.0f / distance;
                    validPairs++;
                }
            }
        }
    }
    
    if (validPairs == 0) return 0.0f;
    
    return totalInverseDistance / static_cast<float>(validPairs);
}

float NeuromodulatorNetwork::computeBetweennessCentrality() const {
    if (topology.nodes.size() < 3) return 0.0f;
    
    // Simplified betweenness centrality (placeholder for Brandes algorithm)
    std::vector<float> betweenness(topology.nodes.size(), 0.0f);
    
    for (size_t s = 0; s < topology.nodes.size(); ++s) {
        for (size_t t = 0; t < topology.nodes.size(); ++t) {
            if (s != t) {
                float distance = computeShortestPathLength(s, t);
                if (distance < std::numeric_limits<float>::max()) {
                    betweenness[s] += 1.0f / distance;
                }
            }
        }
    }
    
    // Normalize
    float maxBetweenness = *std::max_element(betweenness.begin(), betweenness.end());
    if (maxBetweenness > 0.0f) {
        for (size_t i = 0; i < betweenness.size(); ++i) {
            betweenness[i] /= maxBetweenness;
        }
    }
    
    return *std::max_element(betweenness.begin(), betweenness.end());
}

float NeuromodulatorNetwork::computeClosenessCentrality() const {
    if (topology.nodes.size() < 2) return 0.0f;
    
    std::vector<float> distances(topology.nodes.size(), 0.0f);
    float totalDistance = 0.0f;
    size_t validTargets = 0;
    
    for (size_t s = 0; s < topology.nodes.size(); ++s) {
        for (size_t t = 0; t < topology.nodes.size(); ++t) {
            if (s != t) {
                float distance = computeShortestPathLength(s, t);
                if (distance < std::numeric_limits<float>::max()) {
                    totalDistance += distance;
                    validTargets++;
                }
            }
        }
    }
    
    if (validTargets == 0) return 0.0f;
    
    float avgDistance = totalDistance / static_cast<float>(validTargets);
    
    if (avgDistance > 0.0f) {
        return static_cast<float>(topology.nodes.size() - 1) / avgDistance;
    }
    
    return 0.0f;
}

void NeuromodulatorNetwork::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm