#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <complex>
#include <functional>
#include <unordered_set>
#include <eigen3/Eigen/Dense>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

/**
 * Network topology metrics
 */
struct NetworkTopologyMetrics {
    double clusteringCoefficient = 0.0;
    double smallWorldness = 0.0;
    double characteristicPathLength = 0.0;
    double averageDegree = 0.0;
    double modularity = 0.0;
    double efficiency = 0.0;
    size_t diameter = 0;
    double assortativityCoefficient = 0.0;
    double transitivity = 0.0;
    double expansion = 0.0;
    double entropy = 0.0;
    double networkRobustness = 0.0;
    double fragility = 0.0;
    std::map<std::string, double> additionalMetrics;
};

/**
 * Graph structure for network analysis
 */
struct GraphNode {
    NeuronId id;
    NeuronType type;
    float x, y, z;  // Spatial coordinates
    std::map<NeuronId, float> attributes;
};

struct GraphEdge {
    NeuronId source;
    NeuronId target;
    float weight;
    float delay;
    bool isDirected;
    bool isExcitatory;
    float plasticityState;
};

/**
 * Advanced analysis results
 */
struct ActivityPattern {
    size_t timeStep;
    std::vector<std::pair<NeuronId, float>> neuronActivities;
    float overallActivity;
    std::map<NeuronId, float> firingRates;
    float entropy;
    float synchrony;
};

struct NetworkDynamics {
    double stability = 0.0;
    double attractorsCount = 0;
    double chaosLevel = 0.0;
    double bifurcationPoints = 0;
    double limitCycles = 0;
    double fixedPoints = 0;
    std::vector<std::complex<double>> eigenvalues;
    double spectralRadius = 0.0;
    std::map<std::string, double> dynamicalProperties;
};

/**
 * Advanced network analysis and visualization engine
 */
class AdvancedAnalysis {
public:
    AdvancedAnalysis();
    ~AdvancedAnalysis();
    
    // Initialize with brain reference
    bool initialize(std::shared_ptr<Brain> brain);
    
    // Topology analysis methods
    NetworkTopologyMetrics analyzeNetworkTopology(const std::vector<NeuronId>& neuronIds = {});
    NetworkTopologyMetrics analyzeCommunityStructure(const std::vector<NeuronId>& neuronIds = {});
    NetworkTopologyMetrics analyzeLayeredStructure(const std::vector<NeuronId>& neuronIds = {});
    
    // Activity pattern analysis
    std::vector<ActivityPattern> analyzeActivityPatterns(size_t windowSize = 100);
    double analyzeFiringRateDistribution(const std::vector<ActivityPattern>& patterns);
    double analyzeCorrelations(const std::vector<ActivityPattern>& patterns, 
                              float correlationThreshold = 0.1);
    double analyzeSynchrony(const std::vector<ActivityPattern>& patterns);
    
    // Network dynamics analysis
    NetworkDynamics analyzeNetworkDynamics(const std::vector<ActivityPattern>& patterns,
                                          const std::map<size_t, double>& timescales = {});
    std::vector<double> detectAttractors(const std::vector<ActivityPattern>& patterns,
                                        double convergenceThreshold = 0.01);
    double analyzeStability(const std::vector<ActivityPattern>& patterns);
    
    // Graph construction from neural network
    void buildGraphFromNetwork(std::map<NeuronId, GraphNode>& nodes,
                              std::vector<GraphEdge>& edges,
                              const std::vector<NeuronId>& neuronIds = {});
    
    // Visualization data generation
    struct VisualizationData {
        std::vector<float> nodePositions;
        std::vector<float> nodeColors;
        std::vector<float> edgeWeights;
        std::vector<int> nodeTypes;
        std::string topologyType;
        double zoomLevel;
        float nodeSize;
        std::map<std::string, std::string> metadata;
    };
    
    VisualizationData generateTopologyVisualizationData(const NetworkTopologyMetrics& metrics,
                                                        const std::map<NeuronId, GraphNode>& nodes,
                                                        const std::vector<GraphEdge>& edges);
    
    VisualizationData generateActivityVisualizationData(const std::vector<ActivityPattern>& patterns,
                                                        const std::map<NeuronId, GraphNode>& nodes);
    
    // Statistical analysis and hypothesis testing
    struct StatisticalResult {
        double pValue;
        double effectSize;
        double confidenceInterval;
        std::string hypothesis;
        bool significant;
        std::string testType;
        std::string interpretation;
    };
    
    StatisticalResult performKSTest(const std::vector<double>& data1, 
                                   const std::vector<double>& data2,
                                   const std::string& hypothesis = "distributions_are_equal");
    
    StatisticalResult performANOVA(const std::vector<std::vector<double>>& datasets,
                                   const std::string& hypothesis = "all_means_equal");
    
    StatisticalResult performCorrelationAnalysis(const std::vector<double>& x,
                                                const std::vector<double>& y,
                                                const std::string& hypothesis = "no_correlation");
    
    // Export capabilities
    bool exportNetworkGraph(const std::string& filename, 
                           const std::map<NeuronId, GraphNode>& nodes,
                           const std::vector<GraphEdge>& edges,
                           const std::string& format = "graphml");
    
    bool exportAnalysisResults(const std::string& filename,
                              const NetworkTopologyMetrics& topologyMetrics,
                              const NetworkDynamics& dynamicsMetrics,
                              const std::vector<ActivityPattern>& patterns,
                              const std::string& format = "json");
    
    bool exportVisualizationData(const std::string& filename,
                                const VisualizationData& data,
                                const std::string& format = "csv");
    
    // Time-series analysis
    struct TimeSeriesData {
        std::vector<size_t> timestamps;
        std::vector<std::vector<double>> multidimensionalData;
        std::vector<std::string> dimensionNames;
    };
    
    TimeSeriesData extractTimeSeriesData(const std::vector<ActivityPattern>& patterns,
                                        const std::vector<NeuronId>& neuronIds = {});
    
    // Machine learning integration
    struct MLModel {
        std::string modelType;
        std::string architecture;
        std::map<std::string, double> hyperparameters;
        double accuracy;
        double loss;
        std::string status;
    };
    
    MLModel trainPredictionModel(const std::vector<ActivityPattern>& patterns,
                               const std::vector<double>& targets,
                               const std::string& modelType = "neural_network");
    
    std::vector<double> predictFutureActivity(const MLModel& model,
                                             const std::vector<ActivityPattern>& patterns,
                                             size_t predictionSteps = 100);
    
    // Parallel processing support
    void setParallelProcessing(bool enable, size_t numThreads = 0);
    
    // Configuration
    void configure(const std::map<std::string, std::string>& options);
    
    // Performance monitoring
    double getAnalysisTime() const { return analysisTime; }
    
    // Error handling
    std::string getLastError() const { return lastError; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Brain> brainRef;
    bool parallelProcessingEnabled;
    size_t numThreads;
    double analysisTime;
    std::string lastError;
    
    // Helper functions
    static double calculateClusteringCoefficient(const std::map<NeuronId, GraphNode>& nodes,
                                                 const std::vector<GraphEdge>& edges);
    static double calculateSmallWorldness(const std::map<NeuronId, GraphNode>& nodes,
                                         const std::vector<GraphEdge>& edges);
    static double calculateCharacteristicPathLength(const std::map<NeuronId, GraphNode>& nodes,
                                                    const std::vector<GraphEdge>& edges);
    static Eigen::MatrixXd computeAdjacencyMatrix(const std::map<NeuronId, GraphNode>& nodes,
                                                const std::vector<GraphEdge>& edges);
    static std::vector<std::complex<double>> computeEigenvalues(const Eigen::MatrixXd& matrix);
};

} // namespace nlm
