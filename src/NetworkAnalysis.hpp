// NLM Network Analysis
// Network analysis and connectivity metrics for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declarations
class Brain;

// Graph representation for neural network
struct GraphNode {
    size_t id;
    std::string type; // "neuron", "region", "population"
    float x, y, z; // Position in space (for visualization)
    std::map<size_t, float> connections; // Target node ID -> weight
    std::vector<size_t> incomingConnections; // Source node IDs
};

struct GraphEdge {
    size_t sourceId;
    size_t targetId;
    float weight;
    float delay;
    float plasticityFlag;
    bool isDirected;
};

// Network metrics
struct NetworkMetrics {
    size_t nodeCount;
    size_t edgeCount;
    float density;
    float averageDegree;
    float clusteringCoefficient;
    float smallWorldCoefficient;
    float efficiency;
    float diameter;
    float averagePathLength;
    float modularity;
    float transitivity;
    float centralization;
    std::map<std::string, float> centralityMetrics;
    std::map<std::string, float> communityMetrics;
};

// Community detection result
struct CommunityResult {
    size_t communityId;
    std::vector<size_t> memberNodes;
    float cohesion;
    float density;
    std::vector<size_t> boundaryNodes;
};

// Network analyzer class
class NetworkAnalyzer {
public:
    NetworkAnalyzer();
    ~NetworkAnalyzer();
    
    // Initialize analyzer
    void initialize();
    
    // Extract network graph from brain
    std::vector<GraphNode> extractGraph(std::shared_ptr<Brain> brain);
    
    // Compute network metrics
    NetworkMetrics computeMetrics(const std::vector<GraphNode>& graph);
    
    // Detect communities in network
    std::vector<CommunityResult> detectCommunities(const std::vector<GraphNode>& graph);
    
    // Compute shortest paths between all nodes
    std::vector<std::vector<float>> computeShortestPaths(const std::vector<GraphNode>& graph);
    
    // Visualize network (generate simple text-based visualization)
    std::string visualizeNetwork(const std::vector<GraphNode>& graph, 
                                size_t maxNodes = 50) const;
    
    // Generate network statistics report
    std::string generateReport(const std::vector<GraphNode>& graph,
                              const NetworkMetrics& metrics) const;
    
    // Get network statistics
    std::string getNetworkStatistics(const std::vector<GraphNode>& graph) const;
    
    // Calculate centrality measures
    std::map<std::string, float> calculateCentrality(const std::vector<GraphNode>& graph);
    
    // Calculate community structure metrics
    std::map<std::string, float> calculateCommunityMetrics(const std::vector<CommunityResult>& communities);
    
    // Detect network motifs
    std::vector<std::vector<size_t>> detectMotifs(const std::vector<GraphNode>& graph);
    
    // Compare networks
    std::vector<std::string> compareNetworks(const std::vector<GraphNode>& graph1,
                                            const std::vector<GraphNode>& graph2);
    
    // Export network data
    bool exportNetwork(const std::vector<GraphNode>& graph, 
                      const std::string& filepath,
                      const std::string& format = "graphml") const;
    
    // Import network data
    bool importNetwork(std::vector<GraphNode>& graph,
                      const std::string& filepath,
                      const std::string& format = "graphml");
    
    // Reset analyzer
    void reset();
    
    // Shutdown analyzer
    void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
