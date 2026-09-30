#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

namespace nlm {

// Neural Network Visualization
// ============================

class NeuralVisualization {
public:
    NeuralVisualization(std::shared_ptr<Logger> logger);
    ~NeuralVisualization();
    
    // Initialize visualization system
    bool initialize();
    
    // Render neural network structure
    void renderNetwork(const Brain& brain, double scale = 1.0);
    
    // Render neurons with activity levels
    void renderNeurons(const Brain& brain, const std::string& colorScheme = "default");
    
    // Render synaptic connections
    void renderSynapses(const Brain& brain, float minWeight = 0.0f, float maxWeight = 1.0f);
    
    // Render neural regions
    void renderRegions(const Brain& brain, bool showRegionIds = false);
    
    // Highlight active neurons
    void highlightActiveNeurons(const Brain& brain, const std::vector<NeuronId>& neuronIds);
    
    // Color map for neuron properties
    struct Color {
        float r, g, b, a;
        Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
            : r(r), g(g), b(b), a(a) {}
    };
    
    Color getNeuronColor(const Neuron& neuron, const std::string& colorScheme = "activity");
    Color getSynapseColor(float weight, float minWeight = 0.0f, float maxWeight = 1.0f);
    
    // Performance and streaming
    void setFrameRate(double fps);
    void enableStreaming(bool enable);
    bool isStreaming() const;
    
    // Export capabilities
    bool exportToPNG(const std::string& filename, int width = 1920, int height = 1080);
    bool exportToJSON(const std::string& filename, const Brain& brain);
    bool exportToCSV(const std::string& filename, const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spike Train Visualization
// =========================

class SpikeVisualization {
public:
    SpikeVisualization(std::shared_ptr<Logger> logger);
    ~SpikeVisualization();
    
    // Initialize spike visualization system
    bool initialize();
    
    // Render spike raster plot
    void renderSpikeRaster(const Brain& brain, double timeWindow = 10.0,
                          double binWidth = 0.001, bool normalize = true);
    
    // Render spike frequency plot
    void renderSpikeFrequency(const Brain& brain, double timeWindow = 60.0,
                             double binWidth = 0.01);
    
    // Render population activity
    void renderPopulationActivity(const Brain& brain, NeuronType type = NeuronType::Interneuron);
    
    // Render spike times for specific neurons
    void renderSpikeTimes(const std::vector<SpikeEvent>& spikes, 
                         const std::string& title = "Spike Times");
    
    // Set spike visualization parameters
    void setTimeWindow(double window);
    void setBinWidth(double binWidth);
    void enableNormalize(bool normalize);
    
    // Export spike data
    bool exportSpikeData(const std::string& filename, const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory System Visualization
// ============================

class MemoryVisualization {
public:
    MemoryVisualization(std::shared_ptr<Logger> logger);
    ~MemoryVisualization();
    
    // Initialize memory visualization system
    bool initialize();
    
    // Render working memory contents
    void renderWorkingMemory(const Brain& brain, bool showDetails = true);
    
    // Render episodic memory traces
    void renderEpisodicMemory(const Brain& brain, size_t maxEntries = 100);
    
    // Render associative memory network
    void renderAssociativeMemory(const Brain& brain, float activationThreshold = 0.5f);
    
    // Render prediction system
    void renderPredictionSystem(const Brain& brain, bool showPredictions = true);
    
    // Render cognitive system state
    void renderCognitiveSystem(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plasticity Visualization
// ========================

class PlasticityVisualization {
public:
    PlasticityVisualization(std::shared_ptr<Logger> logger);
    ~PlasticityVisualization();
    
    // Initialize plasticity visualization system
    bool initialize();
    
    // Render synaptic weight changes
    void renderWeightChanges(const Brain& brain, double timeWindow = 60.0,
                            bool showSTDP = true, bool showHebbian = true);
    
    // Render plasticity rule activity
    void renderPlasticityActivity(const Brain& brain, const std::string& rule = "stdp");
    
    // Render structural plasticity
    void renderStructuralPlasticity(const Brain& brain);
    
    // Render learning trajectories
    void renderLearningTrajectories(const Brain& brain, size_t numTrajectories = 10);
    
    // Set visualization parameters
    void setWeightChangeThreshold(float threshold);
    void enableShowTraces(bool show);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network Connectivity Visualization
// ==================================

class NetworkVisualization {
public:
    NetworkVisualization(std::shared_ptr<Logger> logger);
    ~NetworkVisualization();
    
    // Initialize network visualization system
    bool initialize();
    
    // Render connectivity matrix
    void renderConnectivityMatrix(const Brain& brain, bool normalized = true);
    
    // Render small-world metrics
    void renderSmallWorldMetrics(const Brain& brain);
    
    // Render community structure
    void renderCommunityStructure(const Brain& brain);
    
    // Render hub and bottleneck analysis
    void renderHubBottleneckAnalysis(const Brain& brain);
    
    // Render network evolution
    void renderNetworkEvolution(const Brain& brain, size_t historySteps = 100);
    
    // Compute network metrics
    struct NetworkMetrics {
        float clusteringCoefficient;
        float characteristicPathLength;
        float modularity;
        float efficiency;
        float smallWorldness;
        std::vector<size_t> communityIds;
        std::map<size_t, float> nodeBetweenness;
        std::map<size_t, float> nodeCloseness;
    };
    
    NetworkMetrics computeNetworkMetrics(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Statistical Analysis Framework
// ================================

class StatisticalAnalysis {
public:
    StatisticalAnalysis(std::shared_ptr<Logger> logger);
    ~StatisticalAnalysis();
    
    // Initialize analysis system
    bool initialize();
    
    // Neural activity statistics
    struct NeuralStats {
        float meanFiringRate;
        float stdFiringRate;
        float maxFiringRate;
        float minFiringRate;
        std::vector<float> firingRateHistogram;
        std::vector<float> crossCorrelationMatrix;
        std::vector<float> autocovariance;
        std::vector<float> autocorrelation;
        float synchronyIndex;
        float networkBursting;
    };
    
    NeuralStats computeNeuralStatistics(const Brain& brain, double window = 1.0);
    
    // Population statistics
    struct PopulationStats {
        std::vector<float> meanActivity;
        std::vector<float> activityVariance;
        std::vector<float> crossCorrelation;
        std::vector<float> coherence;
        float synchrony;
        std::vector<size_t> dominantPopulations;
    };
    
    PopulationStats computePopulationStatistics(const Brain& brain);
    
    // Time series analysis
    struct TimeSeriesStats {
        std::vector<float> mean;
        std::vector<float> variance;
        std::vector<float> skewness;
        std::vector<float> kurtosis;
        std::vector<float> peaks;
        std::vector<size_t> peakPositions;
        float dominantFrequency;
        float spectralPower;
    };
    
    TimeSeriesStats computeTimeSeriesAnalysis(const std::vector<float>& timeSeries,
                                             float samplingRate = 1000.0f);
    
    // Statistical tests
    float computeKSStatistic(const std::vector<float>& data1,
                            const std::vector<float>& data2);
    float computePearsonCorrelation(const std::vector<float>& data1,
                                   const std::vector<float>& data2);
    float computeSpearmanCorrelation(const std::vector<float>& data1,
                                    const std::vector<float>& data2);
    
    // Export analysis results
    bool exportStatistics(const std::string& filename, const NeuralStats& stats);
    bool exportTimeSeriesData(const std::string& filename, const TimeSeriesStats& stats);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Pattern Analysis
// =================

class PatternAnalysis {
public:
    PatternAnalysis(std::shared_ptr<Logger> logger);
    ~PatternAnalysis();
    
    // Initialize pattern analysis system
    bool initialize();
    
    // Pattern detection
    struct Pattern {
        std::string type; // "burst", "oscillation", "synchronization", "formation"
        float confidence; // 0-1
        std::vector<size_t> neuronIds; // neurons involved
        float frequency; // in Hz
        float duration; // in seconds
        std::map<size_t, float> weights; // importance of neurons
    };
    
    std::vector<Pattern> detectPatterns(const Brain& brain, double window = 1.0);
    
    // Sequence analysis
    struct SequenceAnalysis {
        std::vector<std::vector<size_t>> sequences; // sequences of neuron activations
        std::vector<float> sequenceProbabilities;
        std::vector<size_t> transitionMatrix;
        float sequenceComplexity;
        std::vector<size_t> dominantSequences;
    };
    
    SequenceAnalysis analyzeSequences(const Brain& brain, size_t sequenceLength = 5);
    
    // Clustering analysis
    struct ClusteringResult {
        std::vector<size_t> clusterAssignments;
        float silhouetteScore;
        float DaviesBouldinIndex;
        std::vector<std::vector<size_t>> clusters;
        std::vector<float> clusterCoherences;
    };
    
    ClusteringResult clusterNeurons(const Brain& brain, size_t numClusters = 3);
    
    // Feature extraction
    struct NeuralFeatures {
        std::vector<float> activityFeatures;
        std::vector<float> temporalFeatures;
        std::vector<float> connectivityFeatures;
        std::vector<float> plasticityFeatures;
        float complexityMeasure;
        float entropy;
        float sparsity;
    };
    
    NeuralFeatures extractFeatures(const Brain& brain);
    
    // Pattern matching
    float matchPattern(const NeuralFeatures& features, const std::string& patternType);
    
    // Export pattern analysis
    bool exportPatterns(const std::string& filename, const std::vector<Pattern>& patterns);
    bool exportFeatures(const std::string& filename, const NeuralFeatures& features);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network Analysis
// =================

class NetworkAnalysis {
public:
    NetworkAnalysis(std::shared_ptr<Logger> logger);
    ~NetworkAnalysis();
    
    // Initialize network analysis system
    bool initialize();
    
    // Graph theory metrics
    struct GraphMetrics {
        float averageDegree;
        float degreeHistogram[10]; // binned degree distribution
        float clusteringCoefficient;
        float characteristicPathLength;
        float averageClustering;
        float transitivity;
        float reciprocity;
        float modularity;
        float smallWorldness;
        float efficiency;
        float density;
    };
    
    GraphMetrics computeGraphMetrics(const Brain& brain);
    
    // Centrality measures
    struct CentralityMetrics {
        std::map<size_t, float> betweennessCentrality;
        std::map<size_t, float> closenessCentrality;
        std::map<size_t, float> eigenvectorCentrality;
        std::map<size_t, float> degreeCentrality;
        std::vector<size_t> hubs; // top 10% most central nodes
        std::vector<size_t> bottlenecks; // top 10% most critical nodes
    };
    
    CentralityMetrics computeCentralityMetrics(const Brain& brain);
    
    // Community detection
    struct CommunityMetrics {
        std::vector<std::vector<size_t>> communities;
        float modularityQ;
        std::vector<float> communitySizes;
        std::vector<float> communityDensities;
        std::vector<float> communityCoherences;
        float communityModularity;
    };
    
    CommunityMetrics detectCommunities(const Brain& brain);
    
    // Network resilience
    struct ResilienceMetrics {
        float attackResistance; // random failure resilience
        float hubAttackResistance; // targeted hub removal resilience
        float failureThreshold; // fraction of nodes for 50% performance loss
        std::vector<float> resilienceCurve; // performance vs. node removal
    };
    
    ResilienceMetrics computeResilienceMetrics(const Brain& brain);
    
    // Export network analysis
    bool exportGraphMetrics(const std::string& filename, const GraphMetrics& metrics);
    bool exportCentralityMetrics(const std::string& filename, const CentralityMetrics& metrics);
    bool exportCommunityMetrics(const std::string& filename, const CommunityMetrics& metrics);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Learning Analysis
// =================

class LearningAnalysis {
public:
    LearningAnalysis(std::shared_ptr<Logger> logger);
    ~LearningAnalysis();
    
    // Initialize learning analysis system
    bool initialize();
    
    // Learning efficiency metrics
    struct LearningMetrics {
        float learningRate; // rate of synaptic weight change
        float retention; // ability to retain learned information
        float generalization; // ability to apply learning to new contexts
        float stability; // resistance to forgetting
        float transfer; // ability to transfer learning across tasks
        float adaptability; // ability to adapt to new environments
        std::vector<float> learningTrajectory; // learning progress over time
        std::vector<float> performanceCurve; // performance over time
        float asymptoticPerformance; // final performance level
        float sampleEfficiency; // samples needed to reach target performance
    };
    
    LearningMetrics computeLearningMetrics(const Brain& brain, 
                                          double learningWindow = 60.0);
    
    // Memory consolidation
    struct ConsolidationMetrics {
        float consolidationRate; // rate of memory strengthening
        float integrationScore; // integration of new memories
        float interferenceLevel; // memory interference
        std::vector<float> memoryStrengths; // strength of memory traces
        std::vector<float> memoryLatencies; // memory access times
        float workingMemoryCapacity; // current working memory load
        float episodicMemoryCapacity; // current episodic memory load
    };
    
    ConsolidationMetrics computeConsolidationMetrics(const Brain& brain);
    
    // Decision making analysis
    struct DecisionMetrics {
        float choiceConsistency; // consistency of choices across trials
        float reactionTime; // average reaction time
        float accuracy; // decision accuracy
        float explorationExploitationBalance; // balance between exploration and exploitation
        std::vector<size_t> choiceSequences; // sequences of choices
        std::vector<float> rewardHistory; // reward history
        float rewardPredictionError; // difference between expected and actual reward
    };
    
    DecisionMetrics computeDecisionMetrics(const Brain& brain);
    
    // Learning phases
    enum LearningPhase {
        EXPLORATION,
        ACQUISITION,
        CONSOLIDATION,
        RETENTION,
        TRANSFER,
        FORGETTING
    };
    
    std::vector<std::pair<LearningPhase, float>> identifyLearningPhases(
        const Brain& brain, float threshold = 0.1f);
    
    // Export learning analysis
    bool exportLearningMetrics(const std::string& filename, const LearningMetrics& metrics);
    bool exportConsolidationMetrics(const std::string& filename, const ConsolidationMetrics& metrics);
    bool exportDecisionMetrics(const std::string& filename, const DecisionMetrics& metrics);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

