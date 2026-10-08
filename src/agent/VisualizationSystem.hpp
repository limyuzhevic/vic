#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <functional>
#include "AgentBrain.hpp"
#include "AdvancedAgentBrain.hpp"
#include "ErrorHandling.hpp"
#include <nlohmann/json.hpp>
#include <Eigen/Dense>

namespace nlm {

// Forward declarations for visualization components
class BrainVisualizer;
class DataPlotter;
class SimulationMonitor;
class PerformanceAnalyzer;
class ResultExporter;

// Visualization data structures
struct NeuralActivityData {
    std::vector<float> neuronActivities;
    std::vector<float> synapticWeights;
    std::vector<std::vector<float>> connections;
    double timestamp;
    size_t stepNumber;
};

struct SimulationMetrics {
    double elapsedTime;
    size_t stepsCompleted;
    double averageFiringRate;
    double totalReward;
    double predictionError;
    double curiosityLevel;
    double noveltyLevel;
    double dopamineLevel;
    float energyLevel;
    float healthLevel;
    
    // Performance metrics
    double cpuUsage;
    double memoryUsage;
    std::vector<double> simulationSpeed;
};

struct AnalysisResult {
    std::string analysisType;
    std::string description;
    std::map<std::string, double> metrics;
    std::map<std::string, std::vector<double>> timeSeries;
    std::vector<std::string> insights;
    std::vector<std::string> recommendations;
    double confidence;
};

struct VisualizationConfig {
    std::string backend;  // "opengl", "vulkan", "webgl", "terminal"
    std::string theme;    // "light", "dark", "neural"
    int updateIntervalMs;
    bool realTimeUpdates;
    bool showConnections;
    bool showNeurons;
    bool showLabels;
    float neuronSize;
    std::string colorScheme;
    std::map<std::string, std::string> customStyles;
};

struct ExportConfig {
    std::string format;  // "json", "csv", "parquet", "protobuf"
    std::string compression;  // "none", "gzip", "zstd"
    std::string outputPath;
    bool includeMetadata;
    bool includeTimestamps;
    std::vector<std::string> includeOnly;
    std::vector<std::string> exclude;
    nlohmann::json customMetadata;
};

// Advanced visualization backend interface
class VisualizationBackend {
public:
    virtual ~VisualizationBackend() = default;
    
    virtual bool initialize(const VisualizationConfig& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isReady() const = 0;
    
    virtual void render(const NeuralActivityData& neuralData) = 0;
    virtual void update() = 0;
    virtual void resize(int width, int height) = 0;
    
    virtual void addOverlay(const std::string& id, const std::string& content) = 0;
    virtual void removeOverlay(const std::string& id) = 0;
    
    virtual bool saveScreenshot(const std::string& filename) = 0;
    virtual bool startRecording(const std::string& filename) = 0;
    virtual bool stopRecording() = 0;
};

// Data plotting backend interface
class PlotBackend {
public:
    virtual ~PlotBackend() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual void plot(const std::string& name, const std::vector<double>& data,
                     const std::vector<double>& timeAxis = {}) = 0;
    
    virtual void plot2D(const std::string& name,
                       const std::vector<std::pair<double, double>>& points,
                       const std::string& xLabel = "", const std::string& yLabel = "") = 0;
    
    virtual void plot3D(const std::string& name,
                       const std::vector<std::vector<double>>& data) = 0;
    
    virtual void savePlot(const std::string& filename, const std::string& format = "png") = 0;
    virtual void clear() = 0;
};

// Neural network visualization backend
class NeuralVisualizer {
public:
    virtual ~NeuralVisualizer() = default;
    
    virtual bool initialize(const VisualizationConfig& config) = 0;
    virtual void shutdown() = 0;
    
    virtual void visualizeNetwork(const NeuralActivityData& neuralData) = 0;
    virtual void highlightActiveNeurons(const std::vector<size_t>& activeNeurons) = 0;
    virtual void highlightActiveConnections(const std::vector<std::pair<size_t, size_t>>& activeConnections) = 0;
    
    virtual void setCameraPosition(double x, double y, double z) = 0;
    virtual void zoom(double factor) = 0;
    virtual void rotate(double dx, double dy) = 0;
    
    virtual bool exportToFile(const std::string& filename) = 0;
};

// Brain activity monitor for real-time visualization
class BrainActivityMonitor {
public:
    BrainActivityMonitor(std::shared_ptr<AgentBrain> agent);
    ~BrainActivityMonitor();
    
    // Start/stop monitoring
    void startMonitoring(double intervalSeconds = 0.1);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Get current neural activity data
    NeuralActivityData getCurrentNeuralActivity() const;
    SimulationMetrics getCurrentMetrics() const;
    
    // Data access
    std::vector<NeuralActivityData> getNeuralActivityHistory(size_t maxPoints = 1000) const;
    std::vector<SimulationMetrics> getMetricsHistory(size_t maxPoints = 1000) const;
    
    // Callbacks
    void setActivityCallback(std::function<void(const NeuralActivityData&)> callback);
    void setMetricsCallback(std::function<void(const SimulationMetrics&)> callback);
    
    // Filtering and processing
    void setActivityFilter(std::function<bool(const NeuralActivityData&)> filter);
    void setMetricsFilter(std::function<bool(const SimulationMetrics&)> filter);
    
    // Statistical analysis
    std::map<std::string, double> computeStatistics(const NeuralActivityData& data) const;
    std::vector<double> detectPatterns(const NeuralActivityData& data) const;
    
    // Advanced analysis
    AnalysisResult performDeepAnalysis(size_t windowSize = 100) const;
    
private:
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<AdvancedAgentBrain> advancedAgent_;
    
    mutable std::mutex dataMutex_;
    
    std::vector<NeuralActivityData> neuralActivityHistory_;
    std::vector<SimulationMetrics> metricsHistory_;
    
    std::atomic<bool> monitoring_;
    std::thread monitoringThread_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    
    std::function<void(const NeuralActivityData&)> activityCallback_;
    std::function<void(const SimulationMetrics&)> metricsCallback_;
    std::function<bool(const NeuralActivityData&)> activityFilter_;
    std::function<bool(const SimulationMetrics&)> metricsFilter_;
    
    double monitoringInterval_;
    size_t maxHistorySize_;
    
    void monitoringLoop();
    void updateNeuralActivity();
    void updateMetrics();
};

// Advanced data plotter
class AdvancedDataPlotter {
public:
    AdvancedDataPlotter();
    ~AdvancedDataPlotter();
    
    // Initialize with specific backend
    bool initialize(const std::string& backend = "terminal");
    
    // Plotting functions
    void plotNeuronsActivity(const std::vector<float>& activities, const std::string& title = "Neural Activity");
    void plotSynapticWeights(const std::vector<float>& weights, const std::string& title = "Synaptic Weights");
    void plotConnections(const std::vector<std::vector<float>>& connections, const std::string& title = "Connections");
    
    void plotMetrics(const SimulationMetrics& metrics, const std::string& title = "Simulation Metrics");
    void plotTimeSeries(const std::map<std::string, std::vector<double>>& timeSeries, const std::string& title = "Time Series");
    
    void plot3DNeuralNetwork(const NeuralActivityData& neuralData, const std::string& title = "3D Neural Network");
    
    // Multi-panel displays
    void createDashboard(const std::map<std::string, std::vector<std::pair<double, double>>>& plots);
    void createTimeSeriesDashboard(const std::vector<std::string>& metrics, double duration = 60.0);
    
    // Export functions
    bool exportPlots(const std::string& directory, const std::string& format = "png");
    bool exportDashboard(const std::string& filename, const std::string& format = "pdf");
    
    // Interactive features
    void enableZoom(bool enable = true);
    void enablePan(bool enable = true);
    void enableSelection(bool enable = true);
    
    // Callbacks
    void setPlotClickCallback(std::function<void(const std::string&, size_t)> callback);
    void setPlotHoverCallback(std::function<void(const std::string&, double)> callback);
    
private:
    std::unique_ptr<PlotBackend> plotBackend_;
    std::map<std::string, std::vector<std::pair<double, double>>> plotData_;
    
    std::function<void(const std::string&, size_t)> clickCallback_;
    std::function<void(const std::string&, double)> hoverCallback_;
    
    bool zoomEnabled_;
    bool panEnabled_;
    bool selectionEnabled_;
};

// Performance analyzer
class AdvancedPerformanceAnalyzer {
public:
    AdvancedPerformanceAnalyzer();
    ~AdvancedPerformanceAnalyzer();
    
    // Analysis functions
    AnalysisResult analyzeNeuralDynamics(const NeuralActivityData& data) const;
    AnalysisResult analyzeLearningProgress(const std::vector<SimulationMetrics>& metrics) const;
    AnalysisResult analyzePlasticity(const NeuralActivityData& data) const;
    AnalysisResult analyzeConnectivity(const NeuralActivityData& data) const;
    
    // Statistical analysis
    std::map<std::string, double> computeBasicStats(const std::vector<double>& data) const;
    std::vector<double> computeMovingAverage(const std::vector<double>& data, size_t windowSize = 10) const;
    double computeCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    std::vector<double> detectAnomalies(const std::vector<double>& data, double threshold = 3.0) const;
    
    // Pattern recognition
    std::vector<std::vector<size_t>> findClusters(const std::vector<std::vector<double>>& data, 
                                                  double threshold = 0.7) const;
    std::vector<std::string> classifyActivity(const NeuralActivityData& data) const;
    double predictNextState(const std::vector<double>& currentState) const;
    
    // Machine learning integration
    void trainModel(const std::vector<NeuralActivityData>& trainingData);
    std::vector<double> predict(const NeuralActivityData& data) const;
    double evaluateModel(const std::vector<NeuralActivityData>& testData) const;
    
    // Export analysis results
    bool exportAnalysis(const AnalysisResult& result, const std::string& filename);
    bool exportReport(const std::vector<AnalysisResult>& results, const std::string& filename);
    
    // Configuration
    void setModelPath(const std::string& path);
    void setHyperparameters(const nlohmann::json& params);
    
private:
    std::unique_ptr<class MLModel> model_;
    nlohmann::json hyperparameters_;
    std::string modelPath_;
    
    // Statistical computations
    static double mean(const std::vector<double>& data);
    static double variance(const std::vector<double>& data);
    static double standardDeviation(const std::vector<double>& data);
    static double covariance(const std::vector<double>& x, const std::vector<double>& y);
    static double correlationCoefficient(const std::vector<double>& x, const std::vector<double>& y);
};

// Simulation monitor for real-time system monitoring
class AdvancedSimulationMonitor {
public:
    AdvancedSimulationMonitor(std::shared_ptr<AgentBrain> agent);
    ~AdvancedSimulationMonitor();
    
    // Start/stop monitoring
    void startMonitoring(double intervalSeconds = 1.0);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Get system metrics
    struct SystemMetrics {
        double cpuUsage;
        double memoryUsage;
        double memoryPressure;
        std::vector<double> cpuCores;
        std::map<std::string, double> diskUsage;
        std::map<std::string, double> networkUsage;
        double temperature;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    SystemMetrics getSystemMetrics() const;
    
    // Brain-specific metrics
    struct BrainMetrics {
        size_t totalNeurons;
        size_t activeNeurons;
        double averageFiringRate;
        double totalSpikeCount;
        double averageSynapticWeight;
        double plasticityIndex;
        double developmentProgress;
        double curiosityLevel;
        double noveltyLevel;
        double predictionError;
        double rewardRate;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    BrainMetrics getBrainMetrics() const;
    
    // Performance monitoring
    void setPerformanceThresholds(const std::map<std::string, double>& thresholds);
    std::vector<std::string> checkPerformance() const;
    
    // Alert system
    void setAlertCallback(std::function<void(const std::string&, const std::string&)> callback);
    void addAlertRule(const std::string& condition, const std::string& message);
    
    // Export monitoring data
    bool exportMetrics(const std::string& filename, const std::string& format = "csv");
    bool exportSystemReport(const std::string& filename);
    
private:
    std::shared_ptr<AgentBrain> agent_;
    
    mutable std::mutex metricsMutex_;
    
    std::atomic<bool> monitoring_;
    std::thread monitoringThread_;
    
    std::chrono::steady_clock::time_point lastUpdateTime_;
    
    std::function<void(const std::string&, const std::string&)> alertCallback_;
    std::vector<std::pair<std::string, std::string>> alertRules_;
    
    double monitoringInterval_;
    
    void monitoringLoop();
    SystemMetrics computeSystemMetrics() const;
    BrainMetrics computeBrainMetrics() const;
    bool checkAlertConditions() const;
};

// Results exporter
class AdvancedResultsExporter {
public:
    AdvancedResultsExporter();
    ~AdvancedResultsExporter();
    
    // Export brain state
    bool exportBrainState(const std::string& filename, 
                         std::shared_ptr<AgentBrain> agent,
                         const ExportConfig& config = ExportConfig());
    
    // Export simulation results
    bool exportSimulationResults(const std::string& filename,
                                const std::vector<SimulationMetrics>& metrics,
                                const std::vector<NeuralActivityData>& neuralData,
                                const ExportConfig& config = ExportConfig());
    
    // Export analysis results
    bool exportAnalysisResults(const std::string& filename,
                              const std::vector<AnalysisResult>& results,
                              const ExportConfig& config = ExportConfig());
    
    // Export complete report
    bool exportCompleteReport(const std::string& filename,
                             std::shared_ptr<AgentBrain> agent,
                             const std::vector<AnalysisResult>& analyses,
                             const ExportConfig& config = ExportConfig());
    
    // Import functions
    bool importBrainState(const std::string& filename, std::shared_ptr<AgentBrain> agent);
    bool importSimulationResults(const std::string& filename, std::vector<SimulationMetrics>& metrics);
    
    // Configuration management
    void setDefaultExportFormat(const std::string& format);
    void setCompressionLevel(int level);
    void setIncludeMetadata(bool include);
    
    // Post-processing
    void setPostProcessor(std::function<nlohmann::json(const nlohmann::json&)> processor);
    
private:
    std::string defaultFormat_;
    int compressionLevel_;
    bool includeMetadata_;
    
    std::function<nlohmann::json(const nlohmann::json&)> postProcessor_;
    
    // Format-specific exporters
    bool exportToJSON(const nlohmann::json& data, const std::string& filename);
    bool exportToCSV(const nlohmann::json& data, const std::string& filename);
    bool exportToParquet(const nlohmann::json& data, const std::string& filename);
    bool exportToProtobuf(const nlohmann::json& data, const std::string& filename);
    
    // Data serialization
    nlohmann::json serializeBrainState(std::shared_ptr<AgentBrain> agent);
    nlohmann::json serializeMetrics(const std::vector<SimulationMetrics>& metrics);
    nlohmann::json serializeNeuralData(const std::vector<NeuralActivityData>& neuralData);
    nlohmann::json serializeAnalysisResults(const std::vector<AnalysisResult>& results);
};

// Integrated visualization system
class VisualizationSystem {
public:
    VisualizationSystem(std::shared_ptr<AgentBrain> agent);
    ~VisualizationSystem();
    
    // Initialize with configuration
    bool initialize(const VisualizationConfig& config);
    void shutdown();
    
    // Start/stop all visualization components
    void startAll();
    void stopAll();
    bool isRunning() const;
    
    // Component access
    std::shared_ptr<BrainVisualizer> getBrainVisualizer() const { return brainVisualizer_; }
    std::shared_ptr<DataPlotter> getDataPlotter() const { return dataPlotter_; }
    std::shared_ptr<SimulationMonitor> getSimulationMonitor() const { return simulationMonitor_; }
    std::shared_ptr<PerformanceAnalyzer> getPerformanceAnalyzer() const { return performanceAnalyzer_; }
    std::shared_ptr<ResultExporter> getResultExporter() const { return resultExporter_; }
    
    // Configuration updates
    void updateVisualizationConfig(const VisualizationConfig& config);
    void updateExportConfig(const ExportConfig& config);
    
    // System control
    void pauseVisualization(bool pause);
    void captureScreenshot(const std::string& filename);
    void startRecording(const std::string& filename);
    void stopRecording();
    
    // Status and monitoring
    std::map<std::string, std::string> getSystemStatus() const;
    std::vector<std::string> getActiveComponents() const;
    double getSystemUptime() const;
    
private:
    std::shared_ptr<AgentBrain> agent_;
    
    std::shared_ptr<BrainVisualizer> brainVisualizer_;
    std::shared_ptr<DataPlotter> dataPlotter_;
    std::shared_ptr<SimulationMonitor> simulationMonitor_;
    std::shared_ptr<PerformanceAnalyzer> performanceAnalyzer_;
    std::shared_ptr<ResultExporter> resultExporter_;
    
    bool running_;
    std::chrono::steady_clock::time_point startTime_;
    
    VisualizationConfig visConfig_;
    ExportConfig exportConfig_;
    
    bool initializeComponents();
    void shutdownComponents();
};

} // namespace nlm
