#pragma once

#include "ConnectionGraph.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <mutex>
#include <cmath>

namespace nlm {

class ConnectionGraph {
public:
    struct GraphNode {
        NeuronId id;
        float activation;
        float threshold;
        float refractoryTime;
        bool active;
        bool firing;
        
        GraphNode() : id(), activation(0.0f), threshold(0.0f), refractoryTime(0.0f), 
                     active(false), firing(false) {}
    };

    struct GraphEdge {
        NeuronId source;
        NeuronId target;
        float weight;
        Delay delay;
        bool isExcitatory;
        bool enabled;
        
        GraphEdge() : source(), target(), weight(0.0f), delay(1), 
                     isExcitatory(true), enabled(true) {}
    };

    ConnectionGraph();
    ~ConnectionGraph();

    // Disable copying, enable moving
    ConnectionGraph(const ConnectionGraph&) = delete;
    ConnectionGraph& operator=(const ConnectionGraph&) = delete;
    ConnectionGraph(ConnectionGraph&&) noexcept;
    ConnectionGraph& operator=(ConnectionGraph&&) noexcept;

    // Graph construction
    void addNode(const NeuronId& id);
    void removeNode(const NeuronId& id);
    void addEdge(const NeuronId& source, const NeuronId& target, float weight,
                 Delay delay, bool isExcitatory);
    void removeEdge(const NeuronId& source, const NeuronId& target);
    void clear();

    // Graph operations
    void updateNodeActivation(const NeuronId& id, float activation);
    void updateNodeThreshold(const NeuronId& id, float threshold);
    void updateNodeRefractory(const NeuronId& id, float refractoryTime);

    // Optimized spike propagation
    void propagateSpikes(SimulationStep currentStep, Timestamp currentTime);
    void batchPropagateSpikes(const std::vector<NeuronId>& sources,
                              SimulationStep currentStep, Timestamp currentTime);
    void processDelayedEdges(SimulationStep currentStep, Timestamp currentTime);

    // Graph queries
    GraphNode* getNode(const NeuronId& id);
    const GraphNode* getNode(const NeuronId& id) const;
    GraphEdge* getEdge(const NeuronId& source, const NeuronId& target);
    const GraphEdge* getEdge(const NeuronId& source, const NeuronId& target) const;

    // Optimized lookups
    std::vector<GraphEdge*> getOutgoingEdges(const NeuronId& neuron);
    std::vector<GraphEdge*> getIncomingEdges(const NeuronId& neuron);
    std::vector<GraphNode*> getActiveNodes();
    std::vector<GraphNode*> getFiringNodes();

    // Graph analysis
    size_t getNodeCount() const;
    size_t getEdgeCount() const;
    size_t getActiveNodeCount() const;
    float getGraphDensity() const;
    float getAverageEdgeWeight() const;

    // Optimization
    void optimizeConnectivity(float threshold);
    void pruneWeakEdges(float threshold);
    void compressGraph();

    // Statistics and monitoring
    std::string getStatistics() const;
    void resetStatistics();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
