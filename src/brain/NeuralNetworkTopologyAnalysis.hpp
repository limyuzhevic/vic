#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "brain/Brain.hpp"

namespace nlm {

/**
 * Neural network topology analysis results
 */
struct NeuralTopologyAnalysis {
    // Network structure metrics
    float connectivityDensity;
    float clusteringCoefficient;
    float averagePathLength;
    float smallWorldness;
    
    // Population metrics
    std::unordered_map<NeuronType, size_t> populationSizes;
    std::unordered_map<NeuronType, float> populationExcitatoryRatio;
    
    // Regional metrics
    struct RegionMetrics {
        std::string name;
        size_t regionId;
        size_t neuronCount;
        size_t synapseCount;
        float synapticDensity;
        float averageFiringRate;
        float activityLevel;
        float clusteringCoefficient;
    };
    std::vector<RegionMetrics> regionMetrics;
    
    // Degree metrics
    struct DegreeMetrics {
        size_t inDegree;           // Number of incoming synapses per neuron
        size_t outDegree;          // Number of outgoing synapses per neuron
        size_t maxInDegree;        // Maximum incoming synapses
        size_t maxOutDegree;       // Maximum outgoing synapses
        float averageDegree;       // Average total degree
        float hubDegree;           // Degree of most connected neuron
    } degreeMetrics;
    
    // Functional metrics
    float globalExcitatoryInhibitionRatio;
    float strongCouplingRatio;   // Proportion of strong connections (> 0.5 weight)
    float weakCouplingRatio;     // Proportion of weak connections (< 0.1 weight)
    
    // Network efficiency
    float networkEfficiency;
    float synchronizability;
    
    // Complexity metrics
    float entropy;
    size_t betweennessCentrality;
    
    // Quality metrics
    float topologyQuality;      // How "good" the network topology is for information processing
    float robustness;           // Resistance to random failures
    
    // Analysis metadata
    double analysisTime;        // Time taken to perform analysis (seconds)
    size_t sampleSize;          // Number of snapshots if this was iterative
    
    NeuralTopologyAnalysis() : connectivityDensity(0.0f), clusteringCoefficient(0.0f),
                              averagePathLength(0.0f), smallWorldness(0.0f),
                              globalExcitatoryInhibitionRatio(0.0f),
                              strongCouplingRatio(0.0f), weakCouplingRatio(0.0f),
                              networkEfficiency(0.0f), synchronizability(0.0f),
                              entropy(0.0f), betweennessCentrality(0), 
                              topologyQuality(0.0f), robustness(0.0f),
                              analysisTime(0.0), sampleSize(0) {}
};

/**
 * Neural network topology analyzer
 * 
 * Performs comprehensive analysis of neural network topology including:
 * - Graph theoretical metrics
 * - Regional organization
 * - Degree distribution
 * - Functional connectivity patterns
 * - Network efficiency measures
 * - Quality and robustness assessments
 */
class NeuralNetworkTopologyAnalyzer {
public:
    NeuralNetworkTopologyAnalyzer();
    ~NeuralNetworkTopologyAnalyzer();
    
    /**
     * Perform complete topology analysis on a brain
     */
    NeuralTopologyAnalysis analyze(const Brain& brain, double timestep = 0.001);
    
    /**
     * Calculate clustering coefficient for a region
     */
    static float calculateClusteringCoefficient(const Brain& brain, RegionId regionId);
    
    /**
     * Calculate average path length for a connected component
     */
    static float calculateAveragePathLength(const Brain& brain, RegionId regionId);
    
    /**
     * Calculate small-worldness coefficient
     */
    static float calculateSmallWorldness(const Brain& brain, RegionId regionId);
    
    /**
     * Analyze degree distribution across the network
     */
    static NeuralTopologyAnalysis::DegreeMetrics analyzeDegreeDistribution(const Brain& brain);
    
    /**
     * Calculate network efficiency
     */
    static float calculateNetworkEfficiency(const Brain& brain);
    
    /**
     * Calculate synchronizability metric
     */
    static float calculateSynchronizability(const Brain& brain);
    
    /**
     * Calculate entropy of the network topology
     */
    static float calculateTopologyEntropy(const Brain& brain);
    
    /**
     * Analyze robustness to random failures
     */
    static float analyzeRobustness(const Brain& brain, size_t failurePercentage = 10);
    
    /**
     * Generate a topology quality score
     */
    static float generateTopologyQualityScore(const NeuralTopologyAnalysis& analysis);
    
    /**
     * Save topology analysis to file for visualization
     */
    static bool saveToFile(const NeuralTopologyAnalysis& analysis, const std::string& filename);
    
    /**
     * Generate human-readable report of analysis results
     */
    static std::string generateReport(const NeuralTopologyAnalysis& analysis);
    
    /**
     * Check if topology analysis is performed
     */
    bool isInitialized() const;
    
    /**
     * Set analysis parameters
     */
    void setParameters(double timestep = 0.001, bool includeDynamics = true);
    
    /**
     * Get analysis parameters
     */
    struct AnalysisParameters {
        double timestep;           // Simulation timestep for dynamic analysis
        bool includeDynamics;      // Whether to include dynamic metrics
        float clusteringThreshold; // Threshold for clustering coefficient calculation
        bool computePathLength;    // Whether to compute path length
        size_t maxIterations;      // Max iterations for path length calculations
    };
    
    AnalysisParameters getParameters() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * Helper: Calculate local clustering coefficient for a single neuron
     */
    static float calculateLocalClusteringCoefficient(
        const Brain& brain, 
        NeuronId neuronId,
        const std::vector<std::vector<float>>& adjacencyMatrix
    );
    
    /**
     * Helper: Build adjacency matrix for a region
     */
    static std::vector<std::vector<float>> buildAdjacencyMatrix(
        const Brain& brain,
        RegionId regionId
    );
    
    /**
     * Helper: Calculate distance matrix between neurons
     */
    static std::vector<std::vector<float>> calculateDistanceMatrix(
        const Brain& brain,
        RegionId regionId
    );
    
    /**
     * Helper: Extract adjacency matrix for whole brain
     */
    static std::vector<std::vector<float>> buildGlobalAdjacencyMatrix(const Brain& brain);
    
    /**
     * Helper: Calculate betweenness centrality
     */
    static std::unordered_map<NeuronId, size_t> calculateBetweennessCentrality(
        const Brain& brain,
        const std::vector<std::vector<float>>& adjacencyMatrix
    );
    
    /**
     * Helper: Calculate synchronization function for synchronizability
     */
    static float calculateSynchronizationFunction(
        const Brain& brain,
        const std::vector<std::vector<float>>& adjacencyMatrix,
        float omega
    );
};

} // namespace nlm
