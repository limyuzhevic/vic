// NetworkVisualization.hpp - Network connectivity visualization for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Network connectivity visualization for graph theory analysis and network topology
class NetworkVisualization {
public:
    NetworkVisualization(std::shared_ptr<Logger> logger);
    ~NetworkVisualization();
    
    // Initialize network visualization system
    bool initialize();
    
    // Render connectivity matrix (adjacency matrix visualization)
    void renderConnectivityMatrix(const Brain& brain,
                                  bool normalized = true,
                                  float minWeight = 0.0f,
                                  float maxWeight = 1.0f,
                                  const std::string& title = "Connectivity Matrix");
    
    // Render small-world network metrics and topology
    void renderSmallWorldMetrics(const Brain& brain,
                                 bool showClustering = true,
                                 bool showPathLength = true,
                                 bool showModularity = true,
                                 const std::string& title = "Small-World Analysis");
    
    // Render community structure and modular organization
    void renderCommunityStructure(const Brain& brain,
                                  bool showCommunities = true,
                                  bool showInterCommunity = true,
                                  bool showQuality = true,
                                  const std::string& title = "Community Structure");
    
    // Render hub and bottleneck analysis (critical nodes)
    void renderHubBottleneckAnalysis(const Brain& brain,
                                     size_t topPercent = 10,
                                     bool showBetweenness = true,
                                     bool showCloseness = true,
                                     bool showEigenvector = true,
                                     const std::string& title = "Hub and Bottleneck Analysis");
    
    // Render network evolution over time (simulation history)
    void renderNetworkEvolution(const Brain& brain,
                                size_t historySteps = 100,
                                bool showGrowth = true,
                                bool showPrunning = true,
                                bool showRewiring = true,
                                const std::string& title = "Network Evolution");
    
    // Compute comprehensive network metrics
    struct NetworkMetrics {
        // Basic topology
        float averageDegree;
        float degreeHistogram[10]; // Degree distribution
        float clusteringCoefficient;
        float characteristicPathLength;
        float averageClustering;
        
        // Network efficiency
        float transitivity;
        float reciprocity;
        float modularity;
        float smallWorldness;
        float efficiency;
        float density;
        
        // Community structure
        std::vector<std::vector<size_t>> communities;
        std::vector<float> communitySizes;
        std::vector<float> communityDensities;
        std::vector<float> communityCoherences;
        
        // Centrality measures
        std::map<size_t, float> betweennessCentrality;
        std::map<size_t, float> closenessCentrality;
        std::map<size_t, float> eigenvectorCentrality;
        std::map<size_t, float> degreeCentrality;
        
        // Resilience metrics
        float attackResistance;
        float hubAttackResistance;
        float failureThreshold;
        std::vector<float> resilienceCurve;
        
        // Temporal dynamics
        float networkFlexibility;
        float networkStability;
        float integrationLevel;
    };
    
    NetworkMetrics computeNetworkMetrics(const Brain& brain);
    
    // Network visualization modes
    enum class NetworkMode {
        MATRIX,          // Adjacency matrix
        SPHERE,          // Spherical layout
        FORCE_DIRECTED,  // Force-directed layout
        CIRCULAR,        // Circular layout
        TREEMAP,         // Tree map
        HIERARCHICAL,    // Hierarchical layout
        HEATMAP,         // Heat map visualization
        GRAPH3D,         // 3D graph visualization
        ANALYTICAL       // Analytical view with metrics
    };
    
    void setNetworkMode(NetworkMode mode);
    NetworkMode getNetworkMode() const;
    
    // Configuration methods
    void setLayoutAlgorithm(const std::string& algorithm);
    void setNodeSizeScale(float scale);
    void setEdgeWidthScale(float scale);
    void enableLabels(bool enable);
    void setColorMap(const std::string& colorMap);
    
    // Export capabilities
    bool exportNetworkData(const std::string& filename, const Brain& brain);
    bool exportGraphMetrics(const std::string& filename, const NetworkMetrics& metrics);
    bool exportCommunityData(const std::string& filename, const Brain& brain);
    bool exportCentralityData(const std::string& filename, const NetworkMetrics& metrics);
    
    // Real-time network monitoring
    void startRealTimeNetworkMonitoring(double updateInterval = 1.0);
    void stopRealTimeNetworkMonitoring();
    bool isMonitoring() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

