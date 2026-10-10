#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Forward declarations
class Neuromodulator;
class NeuromodulationEngine;

// Network node for neuromodulator distribution
struct NetworkNode {
    NeuronId neuronId;              // Associated neuron
    float positionX;                // X position in network space
    float positionY;                // Y position in network space
    float positionZ;                // Z position in network space
    std::vector<float> receptiveFields;  // Receptive fields for each neuromodulator
    float connectivityStrength;     // Overall connectivity strength
    float localModulationRadius;    // Radius of local modulation influence
    float globalModulationRadius;   // Radius of global modulation influence
    
    NetworkNode()
        : neuronId(INVALID_NEURON_ID), positionX(0.0f), positionY(0.0f), positionZ(0.0f),
          connectivityStrength(1.0f), localModulationRadius(1.0f), globalModulationRadius(10.0f) {
        receptiveFields.resize(10, 0.0f);  // Default 10 neuromodulators
    }
};

// Network connection for neuromodulator propagation
struct NetworkConnection {
    NetworkNode sourceNode;         // Source node
    NetworkNode targetNode;         // Target node
    float connectionStrength;       // Connection strength
    float connectionDelay;          // Delay in connection propagation
    float connectionWeight;         // Weight in network flow
    float diffusionRate;            // Rate of diffusion through network
    float propagationSpeed;         // Speed of signal propagation
    
    NetworkConnection()
        : connectionStrength(1.0f), connectionDelay(1.0f), connectionWeight(1.0f),
          diffusionRate(0.1f), propagationSpeed(1.0f) {}
};

// Neuromodulator diffusion state
struct DiffusionState {
    std::vector<float> concentration;      // Concentration at each node
    std::vector<float> gradient;           // Gradient at each node
    std::vector<float> flux;               // Flux at each node
    std::vector<float> potential;          // Potential at each node
    std::vector<float> sourceTerm;         // Source term at each node
    
    DiffusionState() {
        concentration.resize(100, 0.0f);  // Default 100 nodes
        gradient.resize(100, 0.0f);
        flux.resize(100, 0.0f);
        potential.resize(100, 0.0f);
        sourceTerm.resize(100, 0.0f);
    }
};

// Network topology information
struct NetworkTopology {
    std::vector<NetworkNode> nodes;              // All network nodes
    std::vector<NetworkConnection> connections;  // All network connections
    std::vector<std::vector<size_t>> adjacencyMatrix;  // Adjacency matrix
    float averageDegree;                        // Average node degree
    float clusteringCoefficient;                 // Clustering coefficient
    float smallWorldness;                        // Small-worldness measure
    float modularity;                            // Modularity measure
    
    NetworkTopology()
        : averageDegree(0.0f), clusteringCoefficient(0.0f),
          smallWorldness(0.0f), modularity(0.0f) {}
};

// Neuromodulator network performance metrics
struct NetworkMetrics {
    float networkEfficiency;           // Global efficiency
    float characteristicPathLength;    // Characteristic path length
    float averageClustering;           // Average clustering coefficient
    float betweennessCentrality;       // Betweenness centrality
    float closenessCentrality;         // Closeness centrality
    float modularityQuality;           // Modularity quality
    float synchronizability;           // Network synchronizability
    float signalPropagationSpeed;      // Signal propagation speed
    float informationFlow;             // Information flow rate
    float resourceUtilization;         // Resource utilization efficiency
    
    NetworkMetrics()
        : networkEfficiency(0.0f), characteristicPathLength(0.0f),
          averageClustering(0.0f), betweennessCentrality(0.0f),
          closenessCentrality(0.0f), modularityQuality(0.0f),
          synchronizability(0.0f), signalPropagationSpeed(0.0f),
          informationFlow(0.0f), resourceUtilization(0.0f) {}
};

// Neuromodulator network: Integrated distribution and propagation system
class NeuromodulatorNetwork {
public:
    NeuromodulatorNetwork();
    ~NeuromodulatorNetwork();
    
    // Disable copying, enable moving
    NeuromodulatorNetwork(const NeuromodulatorNetwork&) = delete;
    NeuromodulatorNetwork& operator=(const NeuromodulatorNetwork&) = delete;
    NeuromodulatorNetwork(NeuromodulatorNetwork&&) noexcept;
    NeuromodulatorNetwork& operator=(NeuromodulatorNetwork&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Network management
    void addNode(const NetworkNode& node);
    void removeNode(NeuronId neuronId);
    void addConnection(const NetworkConnection& connection);
    void removeConnection(NeuronId sourceId, NeuronId targetId);
    
    // Neuromodulator distribution
    void distributeNeuromodulator(const std::string& neuromodulatorName, 
                                 float concentration, NeuronId sourceId);
    void propagateDiffusion(const std::vector<float>& sourceConcentrations);
    void updateLocalModulation();
    void updateGlobalModulation();
    
    // Network topology management
    void computeTopology();
    void updateAdjacencyMatrix();
    void optimizeNetworkLayout();
    
    // Neuromodulator network analysis
    void analyzeNetworkMetrics();
    void detectHubNodes();
    void identifyCommunities();
    void computeInformationFlow();
    
    // State management
    void updateState(TimestepDuration dt);
    void reset();
    
    // Accessors
    const NetworkTopology& getTopology() const { return pImpl->topology; }
    const DiffusionState& getDiffusionState() const { return pImpl->diffusionState; }
    const NetworkMetrics& getMetrics() const { return pImpl->metrics; }
    float getNetworkEfficiency() const { return pImpl->metrics.networkEfficiency; }
    float getSignalPropagationSpeed() const { return pImpl->metrics.signalPropagationSpeed; }
    
    // Configuration
    void setNetworkSize(int numNodes);
    void setConnectionDensity(float density);
    void setDiffusionRate(float rate) { pImpl->diffusionRate = rate; }
    void setPropagationSpeed(float speed) { pImpl->propagationSpeed = speed; }
    void enableSmallWorldTopology(bool enable) { pImpl->enableSmallWorldTopology_ = enable; }
    void enableModularityOptimization(bool enable) { pImpl->enableModularityOptimization_ = enable; }
    
    // Integration with other systems
    void integrateWithBrainRegions();
    void integrateWithNeuralDynamics();
    void integrateWithBehaviorOutput();
    
    // Network control
    void stimulateNode(NeuronId neuronId, float intensity);
    void inhibitNode(NeuronId neuronId, float strength);
    void modulateConnection(NeuronId sourceId, NeuronId targetId, float factor);
    
    // Export/import
    void exportNetwork(std::ostream& stream) const;
    void importNetwork(std::istream& stream);
    
    // Testing interface
    size_t getNodeCount() const { return pImpl->topology.nodes.size(); }
    size_t getConnectionCount() const { return pImpl->topology.connections.size(); }
    float getAverageDegree() const { return pImpl->topology.averageDegree; }
    float getClusteringCoefficient() const { return pImpl->topology.clusteringCoefficient; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    float diffusionRate;
    float propagationSpeed;
    
    bool enableSmallWorldTopology_;
    bool enableModularityOptimization_;
};

} // namespace nlm