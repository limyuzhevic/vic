#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include <unordered_map>

namespace nlm {

// Advanced visualization and debugging system for NLM
// Provides comprehensive visualization, debugging, and analysis tools

class VisualizationSystem {
public:
    VisualizationSystem();
    ~VisualizationSystem();
    
    // Initialize visualization system
    bool initialize();
    
    // Update all visualizations
    void update();
    
    // Render current visualizations
    void render();
    
    // Close visualization system
    void close();
    
    // Check if visualization system is active
    bool isActive() const;
    
    // Set update rate for visualizations
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Brain visualization methods
    void visualizeBrain(const class Brain& brain);
    void visualizeNetworkGraph(const class Brain& brain);
    void visualizeActivityPatterns(const class Brain& brain);
    void visualizeSpikeRasters(const class Brain& brain);
    void visualizeSynapseWeights(const class Brain& brain);
    void visualizeBrainConnectivity(const class Brain& brain);
    
    // 3D brain visualization
    void visualize3DBrain(const class Brain& brain, bool wireframe = false);
    void visualizeLayeredCortex(const class Brain& brain);
    
    // Neural population visualization
    void visualizeNeuralPopulations(const class Brain& brain);
    void visualizeRegionActivity(const class Brain& brain);
    
    // Memory system visualization
    void visualizeMemoryTraces(const class Brain& brain);
    void visualizeEpisodicMemory(const class Brain& brain);
    void visualizeWorkingMemory(const class Brain& brain);
    void visualizeAssociativeMemory(const class Brain& brain);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const class Brain& brain);
    void visualizeDopamineSignals(const class Brain& brain);
    void visualizeCuriositySignals(const class Brain& brain);
    void visualizePredictionError(const class Brain& brain);
    
    // Development visualization
    void visualizeDevelopment(const class Brain& brain);
    void visualizePlasticityChanges(const class Brain& brain);
    
    // Time series visualization
    void visualizeTimeSeries(const std::string& title, const std::vector<double>& data);
    void visualizeSpikesOverTime(const class Brain& brain);
    void visualizeFiringRate(const class Brain& brain);
    void visualizeEIEvolution(const class Brain& brain);
    
    // Statistical visualization
    void visualizeStatistics(const class Brain& brain);
    void visualizeCorrelationMatrix(const class Brain& brain);
    void visualizeNetworkMetrics(const class Brain& brain);
    
    // Debug visualization methods
    void visualizeDebugInfo(const class Brain& brain);
    void visualizeDebugNeurons(const class Brain& brain);
    void visualizeDebugSynapses(const class Brain& brain);
    void visualizeDebugPlasticity(const class Brain& brain);
    void visualizeDebugMemory(const class Brain& brain);
    
    // Export visualization data
    bool exportVisualizationData(const std::string& filename);
    bool exportNetworkGraph(const std::string& filename);
    bool exportSpikeData(const std::string& filename);
    bool exportStatistics(const std::string& filename);
    
    // Interactive visualization controls
    void toggleNetworkHighlight(size_t regionId);
    void setNeuronHighlight(NeuronId neuron, bool highlight);
    void setSynapseHighlight(SynapseId synapse, bool highlight);
    void setTimeWindow(double startTime, double endTime);
    
    // Callback system for interactive visualization
    void setNeuronClickCallback(std::function<void(NeuronId)> callback);
    void setRegionClickCallback(std::function<void(RegionId)> callback);
    void setSynapseClickCallback(std::function<void(SynapseId)> callback);
    
    // Debug mode controls
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    
    // Performance metrics visualization
    void visualizePerformanceMetrics();
    void visualizeMemoryUsage();
    void visualizeCPUUsage();
    void visualizeNetworkLatency();
    
    // Multi-window visualization support
    void createWindow(const std::string& title, int width, int height);
    void removeWindow(const std::string& title);
    void arrangeWindows(int layout);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced debugger for NLM
class NLMDebugger {
public:
    NLMDebugger();
    ~NLMDebugger();
    
    // Initialize debugger
    bool initialize();
    
    // Main debugger update loop
    void update();
    
    // Debug brain state
    void debugBrainState(const class Brain& brain);
    
    // Debug simulation step
    void debugStep(const class Brain& brain, SimulationStep step);
    
    // Debug specific components
    void debugNeurons(const class Brain& brain);
    void debugSynapses(const class Brain& brain);
    void debugPlasticity(const class Brain& brain);
    void debugMemory(const class Brain& brain);
    void debugNeuromodulation(const class Brain& brain);
    void debugDevelopment(const class Brain& brain);
    
    // Debug visualization
    void showDebugInfo(const class Brain& brain);
    void showNeuronDebug(const class Brain& brain, NeuronId neuron);
    void showSynapseDebug(const class Brain& brain, SynapseId synapse);
    void showPlasticityDebug(const class Brain& brain);
    void showMemoryDebug(const class Brain& brain);
    
    // Debug controls
    void toggleDebugInfo(bool enable);
    void toggleNeuronDebug(bool enable);
    void toggleSynapseDebug(bool enable);
    void togglePlasticityDebug(bool enable);
    void toggleMemoryDebug(bool enable);
    
    // Debug filters
    void setNeuronFilter(const std::function<bool(NeuronId)>& filter);
    void setSynapseFilter(const std::function<bool(SynapseId)>& filter);
    void setPlasticityFilter(const std::function<bool(const Synapse*)>& filter);
    
    // Debug recording
    void startRecording();
    void stopRecording();
    bool isRecording() const;
    void saveDebugData(const std::string& filename);
    
    // Debug analysis
    void analyzeNeuronActivity(const class Brain& brain);
    void analyzeSynapseDynamics(const class Brain& brain);
    void analyzeLearningProgress(const class Brain& brain);
    void analyzeNetworkTopology(const class Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time plotter for NLM
class RealTimePlotter {
public:
    RealTimePlotter();
    ~RealTimePlotter();
    
    // Initialize plotter
    bool initialize();
    
    // Update plot data
    void update();
    
    // Render plots
    void render();
    
    // Close plotter
    void close();
    
    // Plot types
    enum class PlotType {
        Line,           // Line plot
        Scatter,        // Scatter plot
        Bar,            // Bar plot
        Histogram,      // Histogram
        Heatmap,        // Heatmap
        Network,        // Network visualization
        SpikeRaster,    // Spike raster plot
        TimeSeries,     // Time series
        NetworkMetrics  // Network metrics
    };
    
    // Create plot
    void createPlot(const std::string& title, PlotType type, int width = 800, int height = 600);
    
    // Update plot data
    void updatePlotData(const std::string& title, const std::vector<double>& data);
    void updatePlotData(const std::string& title, const std::vector<std::vector<double>>& multiSeries);
    
    // Plot specific metrics
    void plotFiringRate(const class Brain& brain, double window = 10.0);
    void plotSpikeCount(const class Brain& brain, double window = 10.0);
    void plotEIEvolution(const class Brain& brain);
    void plotMemoryUsage(const class Brain& brain);
    void plotLearningCurve(const class Brain& brain);
    void plotNetworkConnectivity(const class Brain& brain);
    
    // Plot time series
    void plotTimeSeries(const std::string& title, const std::vector<double>& timestamps, const std::vector<double>& values);
    void plotMultiTimeSeries(const std::string& title, const std::vector<std::string>& seriesNames, 
                           const std::vector<std::vector<double>>& seriesData);
    
    // Plot distribution
    void plotHistogram(const std::string& title, const std::vector<double>& data, int bins = 50);
    void plotDensity(const std::string& title, const std::vector<double>& data);
    
    // Plot network
    void plotNetworkGraph(const class Brain& brain, const std::string& layout = "spring");
    void plotBrainRegions(const class Brain& brain);
    
    // Plot controls
    void setXAxis(const std::string& label, double min, double max);
    void setYAxis(const std::string& label, double min, double max);
    void setUpdateInterval(double interval);
    void setPlotRange(double startTime, double endTime);
    
    // Export plots
    bool exportPlot(const std::string& filename);
    bool exportAllPlots(const std::string& directory);
    
    // Plot layout
    void setPlotLayout(int rows, int cols);
    void setActivePlot(const std::string& title);
    void removePlot(const std::string& title);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced brain state analyzer
class BrainStateAnalyzer {
public:
    BrainStateAnalyzer();
    ~BrainStateAnalyzer();
    
    // Analyze brain state
    struct BrainAnalysis {
        double avgFiringRate;
        double avgMembranePotential;
        double avgSynapticWeight;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        double eievolutionRatio;
        double networkComplexity;
        double stabilityScore;
        double learningRate;
        double memoryRetention;
        double coherenceScore;
        std::map<std::string, double> additionalMetrics;
    };
    
    BrainAnalysis analyzeBrain(const class Brain& brain);
    
    // Analyze network properties
    double calculateNetworkComplexity(const class Brain& brain);
    double calculateNetworkCohesion(const class Brain& brain);
    double calculateNetworkModularity(const class Brain& brain);
    double calculateEfficiency(const class Brain& brain);
    
    // Analyze neural dynamics
    double analyzeFiringDynamics(const class Brain& brain);
    double analyzeSynapticDynamics(const class Brain& brain);
    double analyzePlasticityDynamics(const class Brain& brain);
    
    // Analyze memory systems
    double analyzeWorkingMemoryQuality(const class Brain& brain);
    double analyzeEpisodicMemoryQuality(const class Brain& brain);
    double analyzeAssociativeMemoryQuality(const class Brain& brain);
    
    // Analyze development
    double analyzeDevelopmentalProgress(const class Brain& brain);
    double analyzePlasticityMaturation(const class Brain& brain);
    
    // Comparative analysis
    struct ComparisonResult {
        std::string metricName;
        double valueA;
        double valueB;
        double difference;
        double percentageChange;
        std::string interpretation;
    };
    
    std::vector<ComparisonResult> compareBrains(const class Brain& brainA, const class Brain& brainB);
    
    // Time series analysis
    struct TimeSeriesStats {
        double mean;
        double stddev;
        double min;
        double max;
        double median;
        double skew;
        double kurtosis;
        size_t length;
    };
    
    TimeSeriesStats analyzeTimeSeries(const std::vector<double>& data);
    
    // Pattern analysis
    struct PatternResult {
        std::string patternType;
        double confidence;
        std::vector<size_t> supportingIndices;
        std::string description;
    };
    
    std::vector<PatternResult> detectPatterns(const std::vector<double>& data);
    
    // Anomaly detection
    bool detectAnomaly(const std::vector<double>& data, double threshold = 3.0);
    std::vector<size_t> findAnomalies(const std::vector<double>& data, double threshold = 3.0);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm