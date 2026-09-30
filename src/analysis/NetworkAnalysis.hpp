// NetworkAnalysis.hpp - Network connectivity analysis for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Network analysis framework for graph theory and connectivity analysis
class NetworkAnalysis {
public:
    NetworkAnalysis(std::shared_ptr<Logger> logger);
    ~NetworkAnalysis();
    
    // Initialize network analysis system
    bool initialize();
    
    // Comprehensive graph theory analysis
    struct GraphMetrics {
        // Basic topology metrics
        float averageDegree;
        float degreeHistogram[10]; // Degree distribution (bins)
        float clusteringCoefficient;
        float characteristicPathLength;
        float averageClustering;
        float transitivity;
        float reciprocity;
        
        // Network efficiency
        float modularity;
        float smallWorldness;
        float efficiency;
        float density;
        float diameter;
        
        // Community structure metrics
        float communityQuality;
        float communityStability;
        float nestedness;
        float hierarchy;
        
        // Temporal dynamics
        float networkFlexibility;
        float networkStability;
        float integrationLevel;
        float segregationLevel;
    };
    
    GraphMetrics computeGraphMetrics(const Brain& brain);
    
    // Centrality analysis - identification of critical nodes
    struct CentralityMetrics {
        // Different types of centrality
        std::map<size_t, float> betweennessCentrality; // how often a node lies on shortest paths
        std::map<size_t, float> closenessCentrality; // average shortest path distance to all other nodes
        std::map<size_t, float> eigenvectorCentrality; // influence through connections to other influential nodes
        std::map<size_t, float> degreeCentrality; // number of direct connections
        std::map<size_t, float> contactEigenvectorCentrality; // importance of neighbors
        
        // Derived measures
        std::vector<size_t> hubs; // top 10% most central nodes
        std::vector<size_t> bottlenecks; // top 10% most critical nodes
        std::vector<size_t> influentialNodes; // most influential neurons
        std::vector<size_t> keyConnectors; // nodes connecting different communities
        
        // Statistical measures
        float hubProminence;
        float bottleneckEffectiveness;
        float networkCentralization;
    };
    
    CentralityMetrics computeCentralityMetrics(const Brain& brain);
    
    // Community detection and modular analysis
    struct CommunityMetrics {
        // Community structure
        std::vector<std::vector<size_t>> communities; // actual communities (lists of neuron IDs)
        std::vector<size_t> communitySizes; // size of each community
        std::vector<float> communityDensities; // internal connectivity density
        std::vector<float> communityCoherences; // functional coherence within communities
        
        // Quality measures
        float modularityQ; // quality of community partition
        float communityModularity; // average modularity of communities
        float resolutionParameter; // parameter used for detection
        
        // Inter-community relationships
        std::vector<std::vector<float>> interCommunityConnectivity; // connections between communities
        std::vector<size_t> dominantCommunities; // most significant communities
        std::vector<float> communityInfluence; // influence of each community
        
        // Dynamics
        std::vector<float> communityStability; // how stable each community is
        std::vector<float> communityDynamism; // how much communities change over time
    };
    
    CommunityMetrics detectCommunities(const Brain& brain);
    CommunityMetrics findCommunityHierarchy(const Brain& brain);
    
    // Network resilience and robustness analysis
    struct ResilienceMetrics {
        // Random failures
        float randomFailureResistance; // how network behaves when nodes fail randomly
        float randomFailureThreshold; // fraction of nodes for 50% performance loss
        std::vector<float> randomFailureCurve; // performance vs. fraction of failed nodes
        
        // Targeted attacks
        float targetedAttackResistance; // how network behaves when critical nodes are targeted
        float hubAttackResistance; // resistance to hub removal
        float bottleneckAttackResistance; // resistance to bottleneck removal
        std::vector<float> targetedAttackCurve; // performance vs. critical nodes removed
        
        // Recovery analysis
        float recoveryRate; // how quickly network recovers from damage
        float damageThreshold; // threshold for catastrophic failure
        std::vector<float> recoveryTrajectory; // recovery over time
    };
    
    ResilienceMetrics computeResilienceMetrics(const Brain& brain);
    
    // Export network analysis results
    bool exportGraphMetrics(const std::string& filename, const GraphMetrics& metrics);
    bool exportCentralityMetrics(const std::string& filename, const CentralityMetrics& metrics);
    bool exportCommunityMetrics(const std::string& filename, const CommunityMetrics& metrics);
    bool exportResilienceMetrics(const std::string& filename, const ResilienceMetrics& metrics);
    
    // Configuration methods
    void setGraphAlgorithm(const std::string& algorithm);
    void setCommunityDetectionMethod(const std::string& method);
    void setCentralityMeasures(const std::vector<std::string>& measures);
    void setResilienceAnalysis(bool enableRandomFailures, bool enableTargetedAttacks);
    void setThreshold(float threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

