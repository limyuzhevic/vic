// Visualization and debugging module for NLM
// Provides comprehensive visualization, debugging, and analysis tools

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <queue>
#include <iostream>
#include <iomanip>

using namespace std;

namespace nlm {

// Visualization data structures
struct NeuronData {
    int id;
    float x, y, z;
    float membranePotential;
    float firingRate;
    float adaptationVariable;
    int regionId;
    int populationId;
    std::string type;
    bool isActive;
    bool isFiring;
    float lastSpikeTime;
};

struct SynapseData {
    int id;
    int sourceNeuronId;
    int targetNeuronId;
    float weight;
    float delay;
    std::string type;
    float plasticityFactor;
    bool isPlastic;
    float eligibilityTrace;
};

struct SpikeData {
    int neuronId;
    double timestamp;
    int simulationStep;
    int regionId;
    bool is excitatory;
};

struct ActivityData {
    double timestamp;
    size_t totalSpikes;
    size_t firingNeurons;
    size_t activeNeurons;
    float excitationInhibitionRatio;
    float averageFiringRate;
    std::map<int, float> neuronActivations; // neuronId -> activation level
    std::map<int, float> synapticWeights; // synapseId -> weight
};

// Advanced visualization modes
enum class VisualizationMode {
    NETWORK_GRAPH,
    SPIKE_RASTER,
    ACTIVITY_SURFACE,
    WEIGHT_MATRIX,
    CONNECTIVITY_MAP,
    TEMPORAL_EVOLUTION,
    3D_REPRESENTATION,
    HEATMAP,
    STATISTICAL_DISTRIBUTION,
    REAL_TIME_STREAM
};

// Debugging levels
enum class DebugLevel {
    BASIC,
    INTERMEDIATE,
    ADVANCED,
    EXPERT,
    DEVELOPMENTAL
};

// === Core Visualization System ===
class VisualizationSystem {
public:
    VisualizationSystem();
    ~VisualizationSystem();
    
    // Initialize with configuration
    bool initialize(const std::string& configFile = "");
    
    // Set visualization mode and parameters
    void setMode(VisualizationMode mode);
    void setUpdateRate(double fps);
    void setOutputFormat(const std::string& format);
    void setOutputPath(const std::string& path);
    
    // Core visualization methods
    void visualizeNetwork(const std::vector<NeuronData>& neurons, 
                        const std::vector<SynapseData>& synapses);
    void visualizeSpikes(const std::vector<SpikeData>& spikes);
    void visualizeActivity(const std::vector<ActivityData>& activities);
    void visualizeWeights(const std::vector<SynapseData>& synapses);
    void visualizeConnectivity(const std::vector<NeuronData>& neurons);
    
    // Real-time streaming visualization
    void startStreaming(double updateInterval = 0.1);
    void stopStreaming();
    bool isStreaming() const;
    
    // Export functionality
    bool exportToFile(const std::string& filepath, VisualizationMode mode);
    bool exportToAnimation(const std::string& filepath, double fps = 30.0);
    
    // Configuration and settings
    void enableAutoRefresh(bool enable);
    void setMaxHistory(size_t maxHistory);
    void setFilter(const std::string& pattern);
    void setColorScheme(const std::string& scheme);
    
    // Statistics and analysis
    std::string getStatistics() const;
    double getFramerate() const;
    size_t getDataPoints() const;
    
    // Configuration management
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// === Advanced Debugging System ===
class DebugSystem {
public:
    DebugSystem();
    ~DebugSystem();
    
    // Initialize with brain reference and configuration
    bool initialize(std::shared_ptr<class Brain> brain, const std::string& configFile = "");
    
    // Debugging modes
    void setDebugLevel(DebugLevel level);
    void enableFeature(const std::string& feature, bool enable);
    void setBreakpoint(const std::string& condition);
    void addCustomAnalyzer(const std::string& name, std::function<void()> analyzer);
    
    // Real-time monitoring
    void startMonitoring(double interval = 0.01);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Core debugging tools
    void analyzeNeuronBehavior(int neuronId);
    void analyzeSynapseDynamics(int synapseId);
    void analyzeNetworkTopology();
    void analyzePlasticityDynamics();
    void analyzeMemorySystemState();
    void analyzeNeuromodulationState();
    
    // Data collection and analysis
    void collectData(const std::string& category, const std::string& key, double value);
    void collectNeuronData(const NeuronData& neuron);
    void collectSynapseData(const SynapseData& synapse);
    void collectSpikeData(const SpikeData& spike);
    
    // Analysis methods
    void runPatternAnalysis();
    void runTemporalAnalysis();
    void runStatisticalAnalysis();
    void runAnomalyDetection();
    
    // Reporting and output
    std::string generateReport() const;
    void exportReport(const std::string& filepath, const std::string& format = "text");
    void showConsoleOutput(bool show);
    
    // Configuration and settings
    void setLogLevel(const std::string& level);
    void setOutputDirectory(const std::string& dir);
    void setReportFormat(const std::string& format);
    
    // Advanced debugging features
    void enableMemoryProfiling(bool enable);
    void enablePerformanceProfiling(bool enable);
    void enableEventTracing(bool enable);
    void enableCustomMetrics(const std::string& name, std::function<double()> metric);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// === Performance Profiling System ===
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Initialize profiler
    bool initialize(const std::string& configFile = "");
    
    // Profiling modes
    void startProfiling(const std::string& mode = "detailed");
    void stopProfiling();
    bool isProfiling() const;
    
    // CPU profiling
    void profileCPU(std::function<void()> func, const std::string& label);
    void profileMemory(std::function<void()> func, const std::string& label);
    void profileIOL(std::function<void()> func, const std::string& label);
    
    // System-wide profiling
    void profileSystem(double duration);
    void profileNeuralNetwork(std::shared_ptr<class Brain> brain, double duration);
    void profileVisualization(VisualizationSystem& viz, double duration);
    
    // Metrics collection
    struct PerformanceMetrics {
        double cpuTime;
        double memoryUsage;
        double ioTime;
        double networkTime;
        double totalTime;
        size_t memoryAllocated;
        size_t memoryDeallocated;
        std::map<std::string, double> customMetrics;
        std::vector<double> timeSeries;
        std::chrono::system_clock::time_point timestamp;
    };
    
    // Metrics management
    void recordMetrics(const PerformanceMetrics& metrics);
    std::vector<PerformanceMetrics> getMetricsHistory() const;
    PerformanceMetrics getLatestMetrics() const;
    
    // Analysis and reporting
    std::string analyzePerformance() const;
    double calculateEfficiency() const;
    double calculateBottleneckRatio() const;
    std::string generatePerformanceReport() const;
    
    // Export functionality
    bool exportMetrics(const std::string& filepath, const std::string& format = "csv");
    bool exportProfile(const std::string& filepath);
    void setExportPath(const std::string& path);
    
    // Configuration
    void setSamplingRate(double rate);
    void setMetricThresholds(const std::string& category, double threshold);
    void enableRealTimeProfiling(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// === Batch Processing and Experiment Management ===
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Initialize batch processor
    bool initialize(const std::string& configFile = "");
    
    // Experiment management
    bool addExperiment(const std::string& name, std::map<std::string, std::string> parameters);
    bool removeExperiment(const std::string& name);
    bool modifyExperiment(const std::string& name, const std::string& parameter, const std::string& value);
    
    // Batch execution
    bool runBatch(const std::vector<std::string>& experimentNames);
    bool runParameterSweep(const std::string& baseExperiment, 
                          const std::map<std::string, std::vector<std::string>>& parameters);
    bool runMonteCarloSimulation(size_t iterations, const std::string& experimentName);
    bool runEvolutionaryOptimization(const std::string& baseExperiment, 
                                     size_t generations, size_t populationSize);
    
    // Results management
    struct ExperimentResult {
        std::string experimentName;
        double startTime;
        double endTime;
        double duration;
        std::map<std::string, double> metrics;
        std::vector<std::string> logs;
        std::map<std::string, std::string> parameters;
        bool success;
        std::string errorMessage;
    };
    
    std::vector<ExperimentResult> getResults(const std::string& experimentName = "") const;
    ExperimentResult getLatestResult(const std::string& experimentName) const;
    
    // Results analysis
    std::string analyzeResults(const std::string& experimentName) const;
    std::string compareExperiments(const std::vector<std::string>& experimentNames) const;
    bool exportResults(const std::string& experimentName, const std::string& filepath, 
                      const std::string& format = "csv");
    
    // Configuration and settings
    void setOutputDirectory(const std::string& dir);
    void setResultFormat(const std::string& format);
    void setParallelExecution(bool enable);
    void setMaxParallelExperiments(size_t max);
    void enableAutoSave(bool enable);
    
    // Monitoring and control
    bool pauseExecution();
    bool resumeExecution();
    bool cancelExecution();
    bool isExecuting() const;
    std::string getExecutionStatus() const;
    
    // Progress tracking
    double getExecutionProgress(const std::string& experimentName) const;
    std::string getCurrentExperiment() const;
    std::string getEstimatedTimeRemaining(const std::string& experimentName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// === Plugin System for Extensibility ===
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& filepath);
    bool unloadPlugin(const std::string& name);
    bool reloadPlugin(const std::string& name);
    bool isPluginLoaded(const std::string& name) const;
    
    // Plugin information
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::vector<std::string> dependencies;
        std::map<std::string, std::string> metadata;
    };
    
    std::vector<PluginInfo> getLoadedPlugins() const;
    PluginInfo getPluginInfo(const std::string& name) const;
    
    // Plugin functionality
    bool executePluginFunction(const std::string& pluginName, 
                              const std::string& functionName,
                              const std::vector<std::string>& arguments);
    bool registerPluginFunction(const std::string& pluginName,
                               const std::string& functionName,
                               std::function<bool(const std::vector<std::string>&)> func);
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginName, 
                        const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    
    // Plugin system management
    bool savePluginState(const std::string& filepath) const;
    bool loadPluginState(const std::string& filepath);
    bool enableAutoReload(bool enable);
    void setPluginDirectory(const std::string& directory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// === Enhanced Documentation System ===
class DocumentationSystem {
public:
    DocumentationSystem();
    ~DocumentationSystem();
    
    // Documentation generation
    bool generateAPIReference(const std::string& outputPath);
    bool generateUserGuide(const std::string& outputPath);
    bool generateDeveloperGuide(const std::string& outputPath);
    bool generateTutorial(const std::string& outputPath);
    
    // Interactive help system
    std::string getHelp(const std::string& topic, const std::string& subtopic = "") const;
    std::string getQuickReference() const;
    std::vector<std::string> getAvailableTopics() const;
    
    // Documentation management
    bool addDocumentation(const std::string& topic, const std::string& content, 
                         const std::string& format = "markdown");
    bool removeDocumentation(const std::string& topic);
    bool updateDocumentation(const std::string& topic, const std::string& content);
    
    // Configuration
    void setDocumentationPath(const std::string& path);
    void setDefaultFormat(const std::string& format);
    void enableSyntaxHighlighting(bool enable);
    
    // Interactive features
    bool openDocumentation(const std::string& topic) const;
    std::string searchDocumentation(const std::string& query) const;
    std::vector<std::string> getRelatedTopics(const std::string& topic) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// === Main integration class ===
class NLMAdvancedTools {
public:
    NLMAdvancedTools();
    ~NLMAdvancedTools();
    
    // Initialize all tools
    bool initialize(std::shared_ptr<class Brain> brain, const std::string& configFile = "");
    
    // Access to individual tools
    VisualizationSystem& getVisualization() { return *visualization; }
    DebugSystem& getDebug() { return *debug; }
    PerformanceProfiler& getProfiler() { return *profiler; }
    BatchProcessor& getBatchProcessor() { return *batchProcessor; }
    PluginManager& getPluginManager() { return *pluginManager; }
    DocumentationSystem& getDocumentation() { return *documentation; }
    
    // Coordinated operations
    bool runFullAnalysis();
    bool exportCompleteResults(const std::string& outputPath);
    bool runBenchmarkSuite();
    bool runDebugAnalysis();
    
    // Configuration
    void setWorkingDirectory(const std::string& dir);
    void setConfigFile(const std::string& configFile);
    bool loadConfiguration(const std::string& configFile);
    bool saveConfiguration(const std::string& configFile) const;
    
    // Status and control
    bool isInitialized() const;
    bool isRunning() const;
    std::string getStatus() const;
    void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
