#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <functional>

namespace nlm {

// Enterprise-grade visualization and monitoring system
class VisualizationManager {
public:
    // Visualization backend types
    enum class BackendType {
        None,
        CLI,
        GUI,
        WebSocket,
        RESTAPI,
        OpenGL,
        Vulkan
    };

    // Export formats
    enum class ExportFormat {
        JSON,
        CSV,
        HDF5,
        Parquet,
        ApacheArrow,
        Custom
    };

    // Performance monitoring types
    enum class MetricType {
        CPU,
        Memory,
        GPU,
        Network,
        Simulation,
        Visualization
    };

    struct SystemMetrics {
        double cpuUsage;
        double memoryUsage;
        double gpuUsage;
        uint64_t memoryAllocated;
        uint64_t memoryPeak;
        double networkThroughput;
        double simulationFPS;
        double visualizationFPS;
        std::chrono::system_clock::time_point timestamp;
    };

    struct ExportOptions {
        std::vector<ExportFormat> formats;
        std::string outputPath;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        bool includeMetadata;
        bool compressOutput;
        std::string compressionAlgorithm;
    };

    struct AnalysisOptions {
        std::vector<std::string> metricsToAnalyze;
        std::chrono::system_clock::time_point analysisStart;
        std::chrono::system_clock::time_point analysisEnd;
        bool realTimeAnalysis;
        double smoothingFactor;
        std::string outputFormat;
    };

    struct CollaborationConfig {
        std::string serverUrl;
        std::string apiKey;
        std::string teamId;
        std::vector<std::string> collaborators;
        bool enableVersionControl;
        bool enableLiveSharing;
        std::string repositoryPath;
    };

    struct CloudConfig {
        std::string provider; // AWS, GCP, Azure, etc.
        std::string region;
        std::string bucket;
        std::string endpoint;
        bool useS3Compatible;
        std::chrono::system_clock::time_point lastSync;
        bool enableStreaming;
        uint32_t chunkSize;
    };

    // Main interface for comprehensive visualization system
    VisualizationManager(std::shared_ptr<Config> config);
    ~VisualizationManager();

    // Disable copying, enable moving
    VisualizationManager(const VisualizationManager&) = delete;
    VisualizationManager& operator=(const VisualizationManager&) = delete;
    VisualizationManager(VisualizationManager&&) noexcept;
    VisualizationManager& operator=(VisualizationManager&&) noexcept;

    // Initialize the entire visualization and monitoring system
    bool initialize(const std::string& configFile = "");

    // Main processing loop - called from brain simulation step
    void update(const Brain& brain, SimulationStep step, Timestamp time);

    // Core visualization methods
    void renderAll();
    void exportData(const ExportOptions& options);
    void analyzeData(const AnalysisOptions& options);
    void monitorPerformance();
    void generateReports(const std::string& reportPath);

    // Real-time framework management
    bool startRealTimeVisualization(BackendType backend);
    void stopRealTimeVisualization();
    bool isRealTimeActive() const;

    // Multi-format support
    bool startWebServer(uint16_t port = 8080);
    bool startCLIInterface();
    bool startGUIInterface();

    // Data analytics
    void collectNeuralData(const Brain& brain, SimulationStep step);
    void performStatisticalAnalysis(const std::string& analysisType);
    void applyMachineLearningModels(const std::string& modelPath);
    void performTimeSeriesAnalysis(const std::string& timeSeriesData);

    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    SystemMetrics getCurrentMetrics() const;
    std::vector<SystemMetrics> getMetricsHistory(size_t limit = 1000) const;

    // Export capabilities
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToHDF5(const std::string& filepath) const;
    bool exportToParquet(const std::string& filepath) const;

    // Advanced analysis
    void patternRecognition(const std::string& patternType);
    void networkTopologyAnalysis();
    void learningCurveVisualization();
    void comparativeAnalysis(const std::vector<std::string>& simulationIds);

    // Collaboration features
    bool enableCollaboration(const CollaborationConfig& config);
    void shareSimulation(const std::string& simulationId, const std::string& path);
    std::vector<std::string> getSharedSimulations() const;

    // Cloud integration
    bool configureCloudStorage(const CloudConfig& config);
    void syncToCloud(const std::string& simulationId);
    std::vector<std::string> listCloudSimulations();

    // Interactive features
    void startWebDashboard(uint16_t port = 8080);
    void startNeuralNetworkExplorer();
    void startSimulationControlInterface();
    void startDataFilteringTools();

    // Developer tools
    void enablePluginSystem();
    void startCustomVisualizerBuilder();
    void startAnimationRecorder();
    void startPerformanceOptimizer();

    // Configuration and setup
    bool configureBackend(BackendType type, const std::unordered_map<std::string, std::string>& options);
    void setUpdateRate(double hz);
    double getUpdateRate() const;

    // Status and control
    bool isInitialized() const;
    void close();
    std::string getSystemStatus() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
