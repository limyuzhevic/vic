#pragma once

#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include "../brain/Brain.hpp"

namespace nlm {

// Performance monitoring and profiling infrastructure
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Performance metrics
    struct PerformanceMetrics {
        double cpuUsage;          // CPU utilization (0-1)
        double memoryUsage;       // Memory utilization (0-1)
        double diskIO;            // Disk I/O rate (MB/s)
        double networkIO;         // Network I/O rate (MB/s)
        uint64_t totalMemory;    // Total system memory (MB)
        uint64_t usedMemory;     // Used memory (MB)
        double simulationTime;   // Simulation time per step
        double cpuTime;          // CPU time for current step
        double realTime;         // Real time for current step
        size_t memoryAllocations; // Memory allocation count
        size_t memoryDeallocations; // Memory deallocation count
    };
    
    // Get current metrics
    PerformanceMetrics getMetrics() const;
    
    // Reset counters
    void reset();
    
    // Check system health
    bool checkSystemHealth(double threshold = 0.8) const;
    std::string getHealthReport() const;
    
    // Performance profiling
    void startProfile(const std::string& operation);
    void stopProfile(const std::string& operation);
    std::map<std::string, double> getProfileResults() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simulation replay and analysis system
class ReplaySystem {
public:
    ReplaySystem();
    ~ReplaySystem();
    
    // Load replay data from file
    bool loadReplayData(const std::string& filepath);
    
    // Replay data structure
    struct ReplayData {
        std::vector<SimulationStep> steps;
        std::vector<Timestamp> times;
        std::vector<std::shared_ptr<SensoryInput>> sensoryInputs;
        std::vector<std::shared_ptr<Action>> actions;
        std::vector<float> rewards;
        std::vector<float> brainStates; // Flattened brain state vector
        std::map<std::string, std::vector<float>> additionalData;
        
        // Metadata
        uint64_t brainSize;        // Size of brain state vector
        uint64_t maxNeurons;       // Maximum neuron count
        uint64_t maxSynapses;      // Maximum synapse count
        std::string simulationConfig; // Configuration used
        std::chrono::system_clock::time_point timestamp; // Creation time
    };
    
    // Play replay in brain
    bool playReplay(std::shared_ptr<Brain> brain);
    
    // Set replay speed
    void setSpeed(double speed); // 1.0 = real-time, 2.0 = 2x speed, 0.5 = 0.5x speed
    double getSpeed() const;
    
    // Get replay information
    ReplayData getReplayData() const;
    size_t getReplayStepCount() const;
    Timestamp getReplayDuration() const;
    
    // Analyze replay
    struct ReplayAnalysis {
        double averageFiringRate;
        double firingRateVariability;
        double rewardDistributionMean;
        double rewardDistributionStdDev;
        size_t totalSpikes;
        float energyEfficiency;
        float behavioralConsistency;
        std::vector<float> actionFrequency;
        std::map<std::string, double> statisticalMetrics;
    };
    
    ReplayAnalysis analyzeReplay() const;
    
    // Extract features for machine learning
    std::vector<std::vector<float>> extractFeatures(const std::string& featureType = "basic") const;
    std::vector<float> extractTimeSeriesFeatures(size_t windowSize = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch processing system for running multiple simulations
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Add simulation to batch
    struct BatchSimulation {
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Environment> environment;
        std::string name;
        std::map<std::string, std::string> parameters;
        size_t steps;
        bool enabled;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double executionTime;
        float finalReward;
        std::map<std::string, float> metrics;
    };
    
    void addSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
                      size_t steps, const std::string& name = "");
    
    // Run batch processing
    bool runBatch(const std::string& outputDir = "batch_results");
    void stopBatch();
    bool isRunning() const;
    
    // Get batch results
    std::vector<BatchSimulation> getResults() const;
    BatchSimulation getResult(size_t index) const;
    size_t getSimulationCount() const;
    
    // Configuration
    void setParallelProcessing(bool enable, size_t threadCount = 0);
    void setCheckpointInterval(size_t interval);
    void setOutputFormat(const std::string& format);
    void setMetricsCollection(const std::vector<std::string>& metrics);
    
    // Progress tracking
    double getProgress() const;
    std::string getStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Scientific data export system
class ScientificExporter {
public:
    ScientificExporter();
    ~ScientificExporter();
    
    // Export data structures
    struct ExportData {
        std::string name;
        std::vector<std::vector<float>> data;  // Multi-dimensional data
        std::vector<std::string> labels;
        std::map<std::string, std::string> metadata;
        std::chrono::system_clock::time_point timestamp;
    };
    
    // Export to different formats
    bool exportToNumPy(const ExportData& data, const std::string& filename);
    bool exportToCSV(const ExportData& data, const std::string& filename);
    bool exportToHDF5(const ExportData& data, const std::string& filename);
    bool exportToMAT(const ExportData& data, const std::string& filename);
    bool exportToJSON(const ExportData& data, const std::string& filename);
    
    // Specialized export functions
    bool exportBrainState(const std::shared_ptr<Brain>& brain, const std::string& filename);
    bool exportSimulationResults(const std::vector<BatchSimulation>& simulations, 
                                const std::string& outputDir);
    bool exportPerformanceMetrics(const PerformanceMonitor::PerformanceMetrics& metrics,
                                 const std::string& filename);
    
    // Get available export formats
    std::vector<std::string> getAvailableFormats() const;
    std::map<std::string, std::string> getFormatInfo(const std::string& format) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced analysis tools
class AdvancedAnalyzer {
public:
    AdvancedAnalyzer();
    ~AdvancedAnalyzer();
    
    // Connectivity analysis
    struct ConnectivityAnalysis {
        double globalEfficiency;          // Network global efficiency
        double localEfficiency;           // Network local efficiency
        double clusteringCoefficient;      // Graph clustering coefficient
        double characteristicPathLength;   // Average path length
        double modularity;                 // Community detection modularity
        std::vector<std::vector<float>> adjacencyMatrix;
        std::vector<std::vector<float>> laplacianMatrix;
        std::vector<std::vector<float>> eigenVectors;
    };
    
    ConnectivityAnalysis analyzeConnectivity(const std::shared_ptr<Brain>& brain) const;
    
    // Critical point detection
    struct CriticalPoint {
        SimulationStep step;               // Step where critical point occurs
        std::string type;                   // Type: "bifurcation", "phase_transition", "critical_point"
        float value;                        // Measured value at critical point
        float threshold;                    // Threshold that was crossed
        std::string description;            // Human-readable description
        std::map<std::string, float> parameters; // Parameters of the critical point
    };
    
    std::vector<CriticalPoint> detectCriticalPoints(const std::shared_ptr<Brain>& brain,
                                                   double sensitivity = 0.01) const;
    
    // Plasticity dynamics analysis
    struct PlasticityAnalysis {
        double totalPlasticityChange;       // Total weight change over simulation
        double averageLearningRate;        // Average learning rate across synapses
        double weightDistributionVariance;  // Variance in weight distribution
        double stabilityIndex;             // Measure of network stability
        std::vector<float> weightHistory;  // Weight change history over time
        std::map<std::string, double> plasticityMetrics; // Various plasticity measures
    };
    
    PlasticityAnalysis analyzePlasticity(const std::shared_ptr<Brain>& brain) const;
    
    // Development trajectory analysis
    struct DevelopmentAnalysis {
        float developmentalStage;           // Current developmental stage
        float maturationProgress;           // Progress through developmental stages
        float specializationLevel;          // Degree of specialization
        std::vector<float> developmentalTrajectory; // Values over developmental time
        std::map<std::string, float> systemMaturity; // Maturity of different systems
        float adaptationRate;               // Rate of adaptation to environment
    };
    
    DevelopmentAnalysis analyzeDevelopment(const std::shared_ptr<Brain>& brain) const;
    
    // System-level analysis
    struct SystemAnalysis {
        double systemComplexity;             // Measure of system complexity
        double informationProcessing;       // Information processing capacity
        double behavioralFlexibility;       // Behavioral flexibility measure
        double cognitiveLoad;               // Cognitive load estimate
        std::vector<float> systemStates;    // System state time series
        std::map<std::string, double> emergentProperties; // Emergent properties
    };
    
    SystemAnalysis analyzeSystem(const std::shared_ptr<Brain>& brain) const;
    
    // Multi-scale analysis
    struct MultiScaleAnalysis {
        struct ScaleLevel {
            size_t scale;                     // Scale level (1 = micro, 2 = meso, 3 = macro)
            double scaleSpecificMetric;       // Metric specific to this scale
            std::vector<float> scaleData;     // Data at this scale
        };
        
        std::vector<ScaleLevel> scaleAnalysis; // Analysis at different scales
        double scaleCoupling;                // Coupling between scales
        std::vector<std::vector<float>> crossScaleCorrelations; // Correlations between scales
    };
    
    MultiScaleAnalysis multiScaleAnalysis(const std::shared_ptr<Brain>& brain,
                                         const std::vector<size_t>& scales) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network visualization system
class NetworkVisualizer {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // Network visualization methods
    struct NetworkLayout {
        std::vector<std::vector<float>> positions;      // Node positions (x, y)
        std::vector<std::vector<uint64_t>> adjacency;   // Adjacency matrix
        std::vector<std::string> nodeLabels;            // Node labels
        std::vector<std::string> edgeLabels;            // Edge labels
        std::map<std::string, std::vector<float>> nodeAttributes; // Node attributes
        std::map<std::string, std::vector<float>> edgeAttributes; // Edge attributes
    };
    
    NetworkLayout createLayout(const std::shared_ptr<Brain>& brain,
                              const std::string& layoutType = "spring") const;
    
    void drawNetwork(const std::shared_ptr<Brain>& brain,
                    const NetworkLayout& layout,
                    const std::string& outputFile = "network.png");
    
    void drawActivity(const std::shared_ptr<Brain>& brain,
                     const NetworkLayout& layout,
                     double timeWindow = 1.0,
                     const std::string& outputFile = "activity.png");
    
    // Spike visualization
    struct SpikeVisualization {
        std::vector<std::vector<float>> spikeRaster;     // Raster plot data
        std::vector<float> spikeTimes;                  // Spike times
        std::vector<uint64_t> neuronIds;                 // Neuron IDs
        float timeScale;                                 // Time scale for display
        float heightScale;                               // Height scale for display
        std::map<std::string, std::vector<float>> styling; // Styling information
    };
    
    SpikeVisualization createSpikeRaster(const std::vector<SpikeEvent>& spikes,
                                        size_t maxSpikes = 1000) const;
    
    void drawSpikeRaster(const SpikeVisualization& spikes,
                        const std::string& outputFile = "spike_raster.png");
    
    // Weight matrix visualization
    struct WeightVisualization {
        std::vector<std::vector<float>> weightMatrix;    // Weight matrix
        float minWeight;                                 // Minimum weight for coloring
        float maxWeight;                                 // Maximum weight for coloring
        std::string colormap;                            // Colormap name
        bool showWeights;                                // Whether to show weight values
        bool showThreshold;                              // Whether to show threshold crossings
        std::map<std::string, std::vector<float>> weightAttributes;
    };
    
    WeightVisualization createWeightMatrix(const std::shared_ptr<Brain>& brain,
                                          float threshold = 0.1) const;
    
    void drawWeights(const WeightVisualization& weights,
                    const NetworkLayout& layout,
                    const std::string& outputFile = "weights.png");
    
    // Advanced visualization features
    void saveVisualization(const std::string& filename,
                          const std::string& format = "png",
                          const std::map<std::string, std::string>& options = {});
    
    void createAnimation(const std::vector<NetworkLayout>& frames,
                        double fps = 10.0,
                        const std::string& outputFile = "animation.gif");
    
    void createInteractivePlot(const std::shared_ptr<Brain>& brain,
                              const std::string& title = "Neural Network");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
