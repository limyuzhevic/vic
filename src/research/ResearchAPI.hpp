// Research and Development Utilities for NLM

/**
 * NLM Research Module
 * 
 * Advanced utilities for researchers and power users:
 * - Performance profiling and benchmarking
 * - Detailed statistics collection and analysis
 * - Data export for research analysis
 * - Advanced configuration tools
 * - Debug and development utilities
 * - Research-specific metrics and observations
 */

#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Config;

// =============================================================================
// PERFORMANCE PROFILING
// =============================================================================

/**
 * High-resolution performance profiler for neural simulation
 */
class PerformanceProfiler {
public:
    struct ProfilingStats {
        double cpuTime = 0.0;           // CPU time in seconds
        double wallTime = 0.0;          // Wall clock time in seconds
        uint64_t memoryUsage = 0;       // Memory usage in bytes
        uint64_t spikeEvents = 0;       // Number of spike events processed
        uint64_t synapticEvents = 0;    // Number of synaptic events
        uint64_t plasticityEvents = 0;  // Number of plasticity updates
        uint64_t neuronUpdates = 0;     // Number of neuron state updates
        double averageFiringRate = 0.0; // Average firing rate across network
        double spikeStandardDeviation = 0.0; // Spike timing variability
        size_t stepCount = 0;           // Number of simulation steps
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start/stop profiling
    void start(const std::string& operation = "simulation");
    void stop();
    void reset();
    
    // Record events
    void recordSpikeEvent();
    void recordSynapticEvent();
    void recordPlasticityEvent();
    void recordNeuronUpdate();
    
    // Configuration
    void setSamplingRate(double samplesPerSecond);
    void enableMemoryTracking(bool enable);
    void enableDetailedMetrics(bool enable);
    
    // Statistics
    ProfilingStats getStats() const;
    std::string getDetailedReport() const;
    double getPerformanceScore() const; // Composite performance metric
    
    // Comparison
    double compareTo(const PerformanceProfiler& other) const;
    
    // Export
    bool exportToJson(const std::string& filepath) const;
    bool exportToCsv(const std::string& filepath) const;
    bool exportToProfileFormat(const std::string& filepath) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// =============================================================================
// STATISTICS COLLECTOR
// =============================================================================

/**
 * Comprehensive statistics collector for neural networks
 */
class StatisticsCollector {
public:
    struct NetworkStatistics {
        // Topology
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t regionCount = 0;
        size_t activeNeuronCount = 0;
        
        // Dynamics
        double averageFiringRate = 0.0;
        double firingRateVariance = 0.0;
        double firingRateStdDev = 0.0;
        double coefficientOfVariation = 0.0; // Firing regularity
        
        // Connectivity
        double averageSynapticWeight = 0.0;
        double weightStandardDeviation = 0.0;
        double excitationInhibitionRatio = 0.0;
        double connectionDensity = 0.0; // Synapses per neuron
        
        // Temporal dynamics
        double averageInterSpikeInterval = 0.0;
        double isiCoefficientOfVariation = 0.0; // Burstiness measure
        double networkBursting = 0.0;
        
        // Plasticity
        double totalPlasticityChange = 0.0;
        double stdpLTPContribution = 0.0;
        double stdpLTDContribution = 0.0;
        double hebbianContribution = 0.0;
        
        // Memory
        size_t workingMemoryActiveTraces = 0;
        size_t episodicMemoryEpisodeCount = 0;
        double memoryConsolidationRate = 0.0;
        
        // Neuromodulation
        double averageDopamineLevel = 0.0;
        double averageCuriosityLevel = 0.0;
        double averageNoveltyLevel = 0.0;
        double averagePredictionError = 0.0;
        
        // Development
        double developmentalStage = 0.0;
        double plasticityRate = 0.0;
        double structuralPlasticityRate = 0.0;
        
        // Network-level metrics
        double avalancheExponent = 0.0; // Fractal dimension
        double criticality = 0.0; // Distance from critical point
        double integrationTime = 0.0; // Memory integration time
        double replayEfficiency = 0.0; // Memory replay quality
    };
    
    StatisticsCollector();
    ~StatisticsCollector();
    
    // Update with current brain state
    void update(std::shared_ptr<Brain> brain);
    
    // Configuration
    void enableTopologyMetrics(bool enable);
    void enableDynamicMetrics(bool enable);
    void enablePlasticityMetrics(bool enable);
    void enableMemoryMetrics(bool enable);
    void enableNeuromodulationMetrics(bool enable);
    void enableDevelopmentMetrics(bool enable);
    void enableNetworkMetrics(bool enable);
    
    // Time-series data
    void recordTimeSeriesData(const std::string& metric, double value);
    std::vector<std::pair<double, double>> getTimeSeries(const std::string& metric) const;
    
    // Statistics
    NetworkStatistics getCurrentStats() const;
    std::string getSummaryReport() const;
    std::string getDetailedReport() const;
    
    // Analysis
    std::vector<std::string> getAnomalyDetections() const;
    std::vector<std::pair<std::string, double>> getCorrelationAnalysis() const;
    double getStatisticalSignificance(const std::string& metric1, const std::string& metric2) const;
    
    // Export
    bool exportToJson(const std::string& filepath) const;
    bool exportToCsv(const std::string& filepath) const;
    bool exportToAnalysisFormat(const std::string& filepath) const;
    
    // Reset and clearing
    void reset();
    void clearTimeSeriesData();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// =============================================================================
// RESEARCH DATA EXPORT
// =============================================================================

/**
 * Comprehensive data export utilities for research analysis
 */
class ResearchExporter {
public:
    enum ExportFormat {
        TEXT,
        JSON,
        CSV,
        BINARY,
        HDF5,
        MATLAB,
        PYTHON,
        NEUROBENCH,
        NEURALDATA
    };
    
    enum ExportScope {
        CURRENT_STEP,
        EPISODE,
        SESSION,
        DEVELOPMENT_STAGE,
        PLOT_DATA,
        ANALYSIS_DATA
    };
    
    ResearchExporter();
    ~ResearchExporter();
    
    // Configuration
    void setExportFormat(ExportFormat format);
    void setExportScope(ExportScope scope);
    void setOutputDirectory(const std::string& directory);
    void setFileNamingPattern(const std::string& pattern);
    void enableCompression(bool enable);
    void setCompressionLevel(int level);
    
    // Export methods
    bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportNetworkTopology(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportSpikeData(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportPlasticityData(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportMemoryData(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportNeuromodulationData(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportDevelopmentData(std::shared_ptr<Brain> brain, const std::string& label = "");
    bool exportPerformanceData(const PerformanceProfiler& profiler, const std::string& label = "");
    bool exportStatisticsData(const StatisticsCollector& stats, const std::string& label = "");
    
    // Analysis exports
    bool exportConnectivityMatrix(std::shared_ptr<Brain> brain);
    bool exportWeightDistribution(std::shared_ptr<Brain> brain);
    bool exportTemporalDynamics(std::shared_ptr<Brain> brain);
    bool exportCrossRegionalCoupling(std::shared_ptr<Brain> brain);
    
    // Batch operations
    bool exportAllData(std::shared_ptr<Brain> brain, const PerformanceProfiler& profiler,
                       const StatisticsCollector& stats, const std::string& sessionLabel = "");
    bool exportTimeSeriesData(std::shared_ptr<Brain> brain, size_t startStep, size_t endStep);
    
    // Research-specific formats
    bool exportNeuroBenchFormat(std::shared_ptr<Brain> brain, const std::string& benchmarkName);
    bool exportNeuralDataFormat(std::shared_ptr<Brain> brain, const std::string& datasetName);
    bool exportForMachineLearning(std::shared_ptr<Brain> brain, const std::string& mlPipeline);
    
    // Validation and quality control
    bool validateExportData(std::shared_ptr<Brain> brain) const;
    std::vector<std::string> getExportWarnings() const;
    bool performDataIntegrityCheck() const;
    
    // Utility functions
    static std::string generateTimestampedFilename(const std::string& baseName,
                                                   const std::string& extension = "");
    static std::string sanitizeFilename(const std::string& filename);
    static bool createOutputDirectory(const std::string& directory);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// =============================================================================
// ADVANCED CONFIGURATION
// =============================================================================

/**
 * Advanced configuration management for research settings
 */
class AdvancedConfig {
public:
    struct ResearchParameters {
        // Performance optimization
        double targetSimulationSpeed = 1.0;      // Target steps per second
        size_t maxMemoryUsage = 1024 * 1024 * 1024; // Max memory in bytes
        size_t maxNetworkSize = 1000000;      // Max neurons for this session
        bool enableRealTimeProfiling = false;
        bool enableMemoryProfiling = true;
        
        // Data collection
        size_t statsSamplingInterval = 100;    // Steps between stat collection
        bool enableTimeSeriesRecording = true;
        size_t timeSeriesBufferSize = 10000;
        bool enableEventLogging = true;
        std::string logFile = "nlm_research.log";
        
        // Analysis
        bool enableAutomaticAnalysis = false;
        double anomalyDetectionThreshold = 3.0;
        bool enableCorrelationAnalysis = true;
        size_t correlationWindow = 1000;
        
        // Export
        bool autoExportOnSessionEnd = true;
        std::string defaultExportFormat = "json";
        std::string exportPath = "./exports/";
        
        // Research-specific
        bool enableHypothesisTesting = false;
        bool enableStatisticalTesting = true;
        bool enableModelComparison = false;
        bool enableReproducibilityMode = true;
    };
    
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Configuration management
    void loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    void mergeWithDefaultConfig();
    void validateConfiguration() const;
    
    // Parameter access
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const;
    
    template<typename T>
    void setParameter(const std::string& key, const T& value);
    
    // Research parameter access
    const ResearchParameters& getResearchParameters() const;
    void setResearchParameters(const ResearchParameters& params);
    
    // Configuration presets
    void loadPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    void createCustomPreset(const std::string& name, const std::string& description);
    
    // Configuration summary
    std::string getConfigurationSummary() const;
    std::string getParameterDocumentation(const std::string& parameter) const;
    
    // Configuration validation
    bool validateParameter(const std::string& key, const std::string& value) const;
    std::vector<std::string> getValidationErrors() const;
    void clearValidationErrors();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// =============================================================================
// DEBUG AND DEVELOPMENT TOOLS
// =============================================================================

/**
 * Comprehensive debug and development utilities
 */
class DebugTools {
public:
    DebugTools();
    ~DebugTools();
    
    // Brain debugging
    static void debugBrain(std::shared_ptr<Brain> brain, const std::string& outputFile = "");
    static void validateBrainIntegrity(std::shared_ptr<Brain> brain);
    static void analyzeBrainConnectivity(std::shared_ptr<Brain> brain);
    static void profileMemoryUsage(std::shared_ptr<Brain> brain);
    static void checkForMemoryLeaks(std::shared_ptr<Brain> brain);
    
    // Performance debugging
    static void debugPerformance(const PerformanceProfiler& profiler);
    static void profileStepTiming(std::shared_ptr<Brain> brain, size_t steps = 100);
    static void analyzeCacheEfficiency(std::shared_ptr<Brain> brain);
    static void debugThreadingIssues(std::shared_ptr<Brain> brain);
    
    // Configuration debugging
    static void debugConfiguration(std::shared_ptr<Config> config);
    static void validateConfigurationFile(const std::string& filepath);
    static void analyzeConfigurationConflicts(const std::string& filepath);
    
    // Data debugging
    static void debugDataConsistency(std::shared_ptr<Brain> brain);
    static void analyzeDataQuality(std::shared_ptr<Brain> brain);
    static void validateExportIntegrity(const std::string& filepath);
    
    // Development utilities
    static void enableDebugLogging(bool enable);
    static void setDebugLevel(int level);
    static void enableMemoryDebugInfo(bool enable);
    static void enablePerformanceDebugInfo(bool enable);
    
    // Interactive debugging
    static void startInteractiveDebugger(std::shared_ptr<Brain> brain);
    static void launchBrowserBasedDebugger(const std::string& port = "8080");
    static void generateDebugReport(std::shared_ptr<Brain> brain, const std::string& outputFile = "debug_report.txt");
    
    // Helper functions
    static std::string getSystemInfo();
    static std::string getBrainInfo(std::shared_ptr<Brain> brain);
    static std::string getPerformanceInfo(const PerformanceProfiler& profiler);
    static std::string getConfigurationInfo(std::shared_ptr<Config> config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// =============================================================================
// RESEARCH WORKFLOW UTILITIES
// =============================================================================

/**
 * High-level research workflow utilities
 */
namespace ResearchUtils {
    
    // Session management
    struct ResearchSession {
        std::string sessionId;
        std::string description;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string configurationFile;
        std::string outputDirectory;
        size_t neuronCount;
        size_t stepCount;
        std::string experimentType;
        std::map<std::string, std::string> metadata;
    };
    
    // Session lifecycle
    static ResearchSession createSession(const std::string& description,
                                         const std::string& experimentType);
    static bool saveSession(const ResearchSession& session);
    static ResearchSession loadSession(const std::string& sessionId);
    static bool endSession(const std::string& sessionId);
    
    // Data management
    static std::string exportBrainData(std::shared_ptr<Brain> brain,
                                       const ResearchSession& session,
                                       const std::string& dataType = "all");
    static bool importBrainData(std::shared_ptr<Brain> brain,
                                const std::string& filepath);
    static bool validateDataQuality(const std::string& filepath);
    
    // Analysis workflow
    static std::vector<std::string> runAutomatedAnalysis(std::shared_ptr<Brain> brain,
                                                         const PerformanceProfiler& profiler,
                                                         const StatisticsCollector& stats);
    static bool saveAnalysisResults(const std::vector<std::string>& results,
                                    const std::string& outputDirectory);
    static bool generateResearchReport(std::shared_ptr<Brain> brain,
                                       const ResearchSession& session,
                                       const std::string& reportTemplate = "standard");
    
    // Reproducibility
    static std::string generateReproducibilityHash(std::shared_ptr<Brain> brain,
                                                   const AdvancedConfig& config);
    static bool createReproducibilityPackage(std::shared_ptr<Brain> brain,
                                             const AdvancedConfig& config,
                                             const std::string& outputDirectory);
    
    // Collaboration
    static bool shareResults(const std::string& resultsDirectory,
                             const std::string& remoteServer = "",
                             const std::string& apiKey = "");
    static std::string createDatasetDescription(const ResearchSession& session,
                                               std::shared_ptr<Brain> brain);
}

} // namespace nlm