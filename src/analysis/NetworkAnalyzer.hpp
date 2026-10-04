#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include "brain/Brain.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

class NetworkAnalyzer {
public:
    struct GraphMetrics {
        double averageDegree;
        double clusteringCoefficient;
        double diameter;
        double density;
        double transitivity;
        size_t connectedComponents;
        double modularity;
    };
    
    struct CentralityMetrics {
        std::map<NeuronId, double> betweennessCentrality;
        std::map<NeuronId, double> closenessCentrality;
        std::map<NeuronId, double> eigenvectorCentrality;
        std::map<NeuronId, double> degreeCentrality;
    };
    
    struct CommunityMetrics {
        std::vector<std::vector<NeuronId>> communities;
        std::vector<size_t> communitySizes;
        double modularityScore;
    };
    
    static std::unique_ptr<NetworkAnalyzer> create() {
        return std::make_unique<NetworkAnalyzer>();
    }
    
    // Network topology analysis
    GraphMetrics analyzeGraphMetrics(const Brain& brain) const;
    CentralityMetrics calculateCentralityMeasures(const Brain& brain) const;
    CommunityMetrics detectCommunities(const Brain& brain) const;
    std::vector<std::vector<NeuronId>> findConnectedComponents(const Brain& brain) const;
    
    // Network structure analysis
    double calculateClusteringCoefficient(const Brain& brain, NeuronId neuronId) const;
    double calculatePathLength(const Brain& brain) const;
    double calculateNetworkDensity(const Brain& brain) const;
    
    // Network analysis results export
    bool exportNetworkMetrics(const std::string& filepath, const GraphMetrics& metrics) const;
    bool exportCentralityAnalysis(const std::string& filepath, const CentralityMetrics& centrality) const;
    bool exportCommunityStructure(const std::string& filepath, const CommunityMetrics& communities) const;
    
    // Network visualization data
    std::vector<std::pair<NeuronId, NeuronId>> getTopConnections(const Brain& brain, size_t limit) const;
    std::vector<std::pair<NeuronId, NeuronId>> getWeakConnections(const Brain& brain, double threshold) const;
    
private:
    NetworkAnalyzer() = default;
    
    // Helper methods for network analysis
    void buildAdjacencyMatrix(const Brain& brain, std::vector<std::vector<double>>& adjMatrix) const;
    void powerIteration(const std::vector<std::vector<double>>& matrix, 
                       std::vector<double>& eigenvector, int iterations = 100) const;
    double calculateModularity(const Brain& brain, const std::vector<std::vector<NeuronId>>& communities) const;
};

} // namespace nlm