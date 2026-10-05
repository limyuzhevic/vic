// Development tools for NLM - Debugging, profiling, and visualization
// Provides comprehensive development tools for neural simulation analysis and debugging

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

// Neural network debugger for tracing execution and analyzing behavior
class NeuralDebugger {
public:
    struct DebugOptions {
        bool enableStepByStep = false;
        bool enableBreakpoint = false;
        bool enableLogNeuronStates = false;
        bool enableLogSynapseChanges = false;
        bool enableMemoryTracking = false;
        bool enablePerformanceTracking = false;
        std::string logFile = "nlm_debug.log";
        int maxStepsToLog = 1000;
        bool enableVisualOutput = false;
        bool enableDetailedErrorReporting = true;
    };
    
    NeuralDebugger(std::shared_ptr<Brain> brain, const DebugOptions& options = DebugOptions()) 
        : brain_(brain), options_(options) {}
    
    // Control debugging
    void startDebugSession(const std::string& sessionName);
    void stopDebugSession();
    bool isDebugActive() const { return debugActive_; }
    
    // Set breakpoints
    void setBreakpoint(SimulationStep step, const std::string& description = "");
    void removeBreakpoint(SimulationStep step);
    void clearBreakpoints();
    
    // Log specific events
    void logNeuronState(NeuronId neuronId, const std::string& reason = "state_change");
    void logSynapseChange(SynapseId synapseId, float oldWeight, float newWeight);
    void logSpikeEvent(NeuronId neuronId, SimulationStep step);
    void logInputInjection(NeuronId neuronId, float current, SimulationStep step);
    
    // Memory tracking
    void enableMemoryTracking(bool enable);
    void logMemoryUsage();
    bool analyzeMemoryLeaks();
    
    // Performance tracking
    void enablePerformanceTracking(bool enable);
    void logPerformanceMetrics();
    void generatePerformanceReport(const std::string& outputFile);
    
    // Visualization
    bool enableVisualOutput(bool enable);
    bool generateNetworkVisualization(const std::string& outputFile);
    bool generateActivityHeatmap(const std::string& outputFile);
    bool generateSpikeRaster(const std::string& outputFile);
    
    // Error analysis
    void analyzeError(const std::string& errorMessage, const std::string& context = "");
    bool generateErrorReport(const std::string& outputFile);
    std::vector<std::string> getRecentErrors() const { return recentErrors_; }
    
    // Export data
    bool exportDebugData(const std::string& outputDir);
    bool importDebugSession(const std::string& sessionFile);
    
    // Analysis tools
    void analyzeLearningPatterns();
    void analyzeConnectivityChanges();
    void analyzePlasticityDynamics();
    void generateAnalysisReport(const std::string& outputFile);
    
private:
    std::shared_ptr<Brain> brain_;
    DebugOptions options_;
    bool debugActive_ = false;
    std::string currentSession_;
    std::chrono::system_clock::time_point sessionStart_;
    
    std::map<SimulationStep, std::string> breakpoints_;
    std::vector<std::string> recentErrors_;
    std::vector<std::string> logMessages_;
    
    void logMessage(const std::string& message, const std::string& level = "INFO");
    void writeToLogFile(const std::string& message);
    bool shouldLogStep(SimulationStep step) const;
};

// Performance profiler for timing and resource analysis
class PerformanceProfiler {
public:
    struct ProfileOptions {
        bool enableTiming = true;
        bool enableMemory = true;
        bool enableCpu = true;
        bool enableNetwork = false;
        bool enableDetailedMetrics = true;
        std::string profileFile = "nlm_profile.json";
        double samplingInterval = 0.1; // seconds
        bool enableRealTimeReporting = false;
        bool enableBottleneckDetection = true;
        bool enableOptimizationSuggestions = true;
    };
    
    PerformanceProfiler(std::shared_ptr<Brain> brain, const ProfileOptions& options = ProfileOptions())
        : brain_(brain), options_(options) {}
    
    // Start/stop profiling
    void startProfiling(const std::string& profileName);
    void stopProfiling();
    bool isProfiling() const { return profilingActive_; }
    
    // Collect metrics
    void collectMetrics();
    void logMetrics();
    
    // Analysis
    void analyzePerformance();
    bool detectBottlenecks();
    std::vector<std::string> getOptimizationSuggestions() const;
    
    // Reports
    void generatePerformanceReport(const std::string& outputFile);
    void generateMemoryReport(const std::string& outputFile);
    void generateCpuReport(const std::string& outputFile);
    
    // Query functions
    double getAverageStepTime() const { return averageStepTime_; }
    double getMemoryUsage() const { return memoryUsage_; }
    double getCpuUsage() const { return cpuUsage_; }
    size_t getActiveNeurons() const { return activeNeurons_; }
    size_t getTotalSpikes() const { return totalSpikes_; }
    
    // Configuration
    void setOptions(const ProfileOptions& options) { options_ = options; }
    ProfileOptions getOptions() const { return options_; }
    
private:
    std::shared_ptr<Brain> brain_;
    ProfileOptions options_;
    bool profilingActive_ = false;
    std::string currentProfileName_;
    std::chrono::system_clock::time_point profileStart_;
    
    // Metrics collection
    double averageStepTime_ = 0.0;
    double memoryUsage_ = 0.0;
    double cpuUsage_ = 0.0;
    size_t activeNeurons_ = 0;
    size_t totalSpikes_ = 0;
    size_t stepCount_ = 0;
    
    std::vector<double> stepTimes_;
    std::vector<double> memoryUsages_;
    std::vector<double> cpuUsages_;
    
    void sampleMetrics();
    void calculateMetrics();
};

// Visualization tools for neural network analysis
class VisualizationTools {
public:
    struct VisualizationOptions {
        bool enableNetworkGraphs = true;
        bool enableActivityPlots = true;
        bool enableSpikeRasters = true;
        bool enableHeatmaps = true;
        bool enableTimeSeries = true;
        std::string outputFormat = "svg"; // "png", "svg", "pdf", "html"
        std::string outputDirectory = "./visualizations";
        bool enableAnimation = false;
        double animationInterval = 0.1; // seconds
        bool enableInteractive = false;
        std::string theme = "default";
        bool generateReports = true;
    };
    
    VisualizationTools(std::shared_ptr<Brain> brain, const VisualizationOptions& options = VisualizationOptions())
        : brain_(brain), options_(options) {}
    
    // Network visualization
    bool generateNetworkGraph(const std::string& outputFile = "network_graph.svg");
    bool generateRegionConnectivity(const std::string& outputFile = "region_connectivity.svg");
    bool generateSynapticTopology(const std::string& outputFile = "synaptic_topology.svg");
    
    // Activity visualization
    bool generateActivityHeatmap(const std::string& outputFile = "activity_heatmap.svg");
    bool generateSpikeRasterPlot(const std::string& outputFile = "spike_raster.svg");
    bool generateFiringRatePlots(const std::string& outputFile = "firing_rates.svg");
    bool generateTimeSeriesCharts(const std::string& outputFile = "time_series.svg");
    
    // Comprehensive visualizations
    bool generateComprehensiveDashboard(const std::string& outputFile = "dashboard.html");
    bool generateLearningCurves(const std::string& outputFile = "learning_curves.svg");
    bool generatePlasticityEvolution(const std::string& outputFile = "plasticity_evolution.svg");
    
    // Animation and interactive visualizations
    bool generateAnimation(const std::string& outputFile = "animation.gif");
    bool createInteractiveDashboard(const std::string& outputDir);
    
    // Data export for external tools
    bool exportForPython(const std::string& outputDir);
    bool exportForMATLAB(const std::string& outputDir);
    bool exportForR(const std::string& outputDir);
    
    // Analysis visualizations
    bool generateStatisticalAnalysis(const std::string& outputFile = "statistical_analysis.svg");
    bool generateComparativePlots(const std::string& outputFile = "comparative_plots.svg");
    
    // Configuration
    void setOptions(const VisualizationOptions& options) { options_ = options; }
    VisualizationOptions getOptions() const { return options_; }
    
private:
    std::shared_ptr<Brain> brain_;
    VisualizationOptions options_;
    
    bool ensureOutputDirectory();
    std::string getTimestampFilename(const std::string& baseName);
};

// Experimental analysis tools
class ExperimentalAnalysis {
public:
    struct AnalysisOptions {
        bool enableStatisticalAnalysis = true;
        bool enableHypothesisTesting = false;
        bool enablePatternRecognition = true;
        bool enableComparativeAnalysis = false;
        bool enableLongitudinalAnalysis = true;
        std::string analysisFile = "nlm_analysis.json";
        bool generateDetailedReports = true;
        bool enableConfidenceIntervals = true;
        bool enableEffectSizeCalculation = true;
        std::string analysisMethod = "comprehensive";
    };
    
    ExperimentalAnalysis(std::shared_ptr<Brain> brain, const AnalysisOptions& options = AnalysisOptions())
        : brain_(brain), options_(options) {}
    
    // Statistical analysis
    bool analyzeLearningTrajectories();
    bool analyzeSynapticWeightChanges();
    bool analyzeActivityPatterns();
    bool analyzeConnectivityDynamics();
    
    // Hypothesis testing
    bool performTTests(const std::string& condition1, const std::string& condition2);
    bool performANOVAs(const std::vector<std::string>& conditions);
    bool performNonParametricTests(const std::string& data);
    
    // Pattern recognition
    bool identifyLearningPhases();
    bool detectStabilityPeriods();
    bool recognizeBehavioralPatterns();
    
    // Comparative analysis
    bool compareWithBaseline(const std::string& baselineData);
    bool analyzeParameterEffects(const std::map<std::string, double>& parameters);
    bool performCrossValidation(const std::string& foldSpec);
    
    // Longitudinal analysis
    bool analyzeTimeSeriesData();
    bool detectRegimeChanges();
    bool projectFutureTrends();
    
    // Reports
    bool generateAnalysisReport(const std::string& outputFile);
    bool generateStatisticalTables(const std::string& outputFile);
    bool generateVisualizationSummary(const std::string& outputFile);
    
    // Configuration
    void setOptions(const AnalysisOptions& options) { options_ = options; }
    AnalysisOptions getOptions() const { return options_; }
    
private:
    std::shared_ptr<Brain> brain_;
    AnalysisOptions options_;
    
    std::vector<double> calculateStatistics(const std::vector<double>& data) const;
    double calculateEffectSize(const std::vector<double>& group1, const std::vector<double>& group2) const;
    bool performBootstrap(const std::vector<double>& data, int iterations = 1000);
};

// System monitoring and health checks
class SystemMonitor {
public:
    struct MonitorOptions {
        bool enableResourceMonitoring = true;
        bool enableProcessMonitoring = true;
        bool enableNetworkMonitoring = true;
        bool enableMemoryMonitoring = true;
        bool enablePerformanceMonitoring = true;
        double monitorInterval = 5.0; // seconds
        bool enableAlerts = true;
        std::string alertFile = "system_alerts.log";
        bool enableHealthChecks = true;
        bool enablePerformanceThresholds = true;
        std::string healthCheckFile = "health_report.json";
    };
    
    SystemMonitor(std::shared_ptr<Brain> brain, const MonitorOptions& options = MonitorOptions())
        : brain_(brain), options_(options) {}
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoringActive_; }
    
    // Resource monitoring
    void monitorResources();
    bool checkMemoryUsage();
    bool checkCpuUsage();
    bool checkNetworkUsage();
    bool checkDiskSpace();
    
    // Process monitoring
    void monitorProcesses();
    bool checkBrainProcesses();
    bool checkMemoryProcesses();
    bool checkPerformanceProcesses();
    
    // Health checks
    bool performHealthCheck();
    std::vector<std::string> getHealthIssues() const { return healthIssues_; }
    bool resolveHealthIssues(const std::string& issue);
    
    // Alerts and notifications
    bool sendAlert(const std::string& message, const std::string& severity = "WARNING");
    bool logAlert(const std::string& message, const std::string& context = "");
    std::vector<std::string> getAlerts() const { return alerts_; }
    void clearAlerts() { alerts_.clear(); }
    
    // Reports
    bool generateHealthReport(const std::string& outputFile);
    bool generatePerformanceReport(const std::string& outputFile);
    bool generateResourceReport(const std::string& outputFile);
    
    // Configuration
    void setOptions(const MonitorOptions& options) { options_ = options; }
    MonitorOptions getOptions() const { return options_; }
    
private:
    std::shared_ptr<Brain> brain_;
    MonitorOptions options_;
    bool monitoringActive_ = false;
    std::chrono::system_clock::time_point monitorStart_;
    
    std::vector<std::string> healthIssues_;
    std::vector<std::string> alerts_;
    
    double getCurrentMemoryUsage() const;
    double getCurrentCpuUsage() const;
    double getCurrentNetworkUsage() const;
    bool checkThresholds(const std::string& metric, double value);
    void generateAlert(const std::string& message, const std::string& severity);
};

} // namespace nlm