#pragma once

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

namespace nlm {

/**
 * @brief Batch processing utilities for NLM simulations
 * 
 * Provides efficient batch execution of simulations with progress tracking,
 * checkpointing, and result aggregation for advanced users.
 */
class BatchProcessor {
public:
    struct BatchConfig {
        // Simulation parameters
        size_t numIterations = 100;
        std::shared_ptr<Config> config;
        
        // Progress tracking
        bool enableProgressReporting = true;
        size_t progressInterval = 10;
        
        // Checkpointing
        bool enableCheckpointing = false;
        std::string checkpointDirectory;
        size_t checkpointInterval = 100;
        
        // Parallel execution
        bool enableParallelExecution = false;
        size_t maxParallelThreads = std::thread::hardware_concurrency();
        
        // Result aggregation
        bool enableResultAggregation = true;
        bool saveIndividualResults = false;
        std::string resultsDirectory;
        
        // Performance monitoring
        bool enablePerformanceMonitoring = true;
        bool reportTimingDetails = false;
        
        // Callback functions
        std::function<void(size_t, size_t)> onIterationComplete;
        std::function<void(const std::string&)> onProgressUpdate;
        std::function<void(const BatchResult&)> onBatchComplete;
        
        // Error handling
        bool continueOnError = false;
        std::function<void(const std::exception&)> onError;
    };
    
    struct BatchResult {
        // Basic statistics
        size_t totalIterations = 0;
        size_t successfulIterations = 0;
        size_t failedIterations = 0;
        double successRate = 0.0;
        
        // Performance metrics
        double totalTimeSeconds = 0.0;
        double averageTimePerIteration = 0.0;
        double fastestIteration = 0.0;
        double slowestIteration = 0.0;
        
        // Neural activity statistics
        size_t totalSpikes = 0;
        size_t averageSpikesPerIteration = 0;
        size_t maxSpikesInIteration = 0;
        
        // Development statistics
        double averageDevelopmentStage = 0.0;
        size_t averageRegionCount = 0;
        
        // Memory usage statistics
        size_t peakMemoryUsage = 0;
        
        // Configuration used
        std::shared_ptr<Config> config;
        
        // Individual iteration results
        std::vector<IterationResult> iterationResults;
        
        // Error information
        std::vector<std::string> errors;
        
        // Batch metadata
        std::string batchId;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        
        // File paths
        std::string resultsPath;
        std::string checkpointsPath;
        
        BatchResult() : startTime(std::chrono::system_clock::now()) {}
    };
    
    struct IterationResult {
        size_t iterationNumber = 0;
        double iterationTimeSeconds = 0.0;
        size_t spikeCount = 0;
        float developmentStage = 0.0f;
        size_t regionCount = 0;
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        float avgFiringRate = 0.0f;
        float excitationInhibitionRatio = 0.0f;
        bool checkpointSaved = false;
        std::string checkpointPath;
        std::string errorMessage;
        bool success = false;
    };
    
    BatchProcessor(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                   std::shared_ptr<AgentBrain> agent);
    
    ~BatchProcessor();
    
    // Main batch processing methods
    BatchResult runBatch(const BatchConfig& config);
    BatchResult runBatch(const BatchConfig& config, const std::vector<std::shared_ptr<Brain>>& brains);
    
    // Simulation control methods
    void executeIteration(size_t iteration, std::shared_ptr<Brain> brain);
    void executeSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent, size_t numIterations);
    
    // Checkpoint management
    bool saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    bool loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath);
    std::vector<std::string> listCheckpoints(const std::string& directory);
    bool cleanupOldCheckpoints(const std::string& directory, size_t keepCount);
    
    // Progress reporting and monitoring
    void reportProgress(size_t current, size_t total, const BatchResult& result);
    void updatePerformanceMetrics(BatchResult& result, const IterationResult& iteration);
    std::string generateProgressReport(const BatchResult& result);
    
    // Result management
    void saveIndividualResults(const BatchResult& result);
    void saveBatchResults(const BatchResult& result);
    void saveIterationResults(const IterationResult& iteration, const std::string& basePath);
    
    // Configuration management
    std::shared_ptr<Config> createBatchConfig(const BatchConfig& batchConfig);
    void applyBatchConfig(std::shared_ptr<Brain> brain, const BatchConfig& config);
    
    // Parallel execution utilities
    void executeParallel(size_t numIterations, std::vector<BatchResult>& results);
    void executeParallel(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                        std::shared_ptr<AgentBrain> agent, size_t numIterations,
                        std::vector<BatchResult>& results);
    
    // Statistics and analysis
    void analyzeResults(BatchResult& result);
    void aggregateResults(const std::vector<BatchResult>& results, BatchResult& aggregated);
    double calculateStandardDeviation(const std::vector<double>& values);
    double calculateMean(const std::vector<double>& values);
    
    // Utility methods
    std::string generateTimestampedFilename(const std::string& baseName, const std::string& extension);
    void ensureDirectoryExists(const std::string& path);
    bool fileExists(const std::string& path);
    
    // Configuration templates
    static BatchConfig createExplorationConfig(size_t iterations, const std::string& checkpointDir);
    static BatchConfig createBenchmarkConfig(size_t iterations, const std::string& resultsDir);
    static BatchConfig createDevelopmentConfig(size_t iterations);
    static BatchConfig createPerformanceConfig(size_t iterations);
    
    // Result export
    std::string exportResultsToJSON(const BatchResult& result);
    bool exportResultsToCSV(const BatchResult& result, const std::string& filepath);
    
protected:
    // Callback utilities
    void executeCallbacks(const std::function<void(size_t, size_t)>& callback, 
                         size_t current, size_t total);
    void executeCallbacks(const std::function<void(const std::string&)>& callback,
                         const std::string& message);
    
    // Error handling utilities
    void handleError(const std::exception& e, BatchResult& result, bool continueOnError);
    void logError(const std::string& error, BatchResult& result);
    
    // Resource management
    void cleanupResources();
    void releaseMemory(std::shared_ptr<Brain> brain);
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    std::atomic<size_t> currentIteration_;
    std::mutex mutex_;
    std::vector<std::thread> threads_;
    bool running_;
};

/**
 * @brief Advanced debugging utilities for NLM
 * 
 * Provides comprehensive debugging tools for analyzing neural network behavior,
 * identifying performance bottlenecks, and troubleshooting issues.
 */
class Debugger {
public:
    struct DebugConfig {
        // Debug output
        bool enableVerboseLogging = false;
        bool enableDetailedTracing = false;
        bool enableMemoryTracking = true;
        bool enablePerformanceProfiling = true;
        
        // Output destinations
        std::string logFile;
        bool enableConsoleOutput = true;
        bool enableFileOutput = false;
        bool enableNetworkOutput = false;
        
        // Filtering
        std::vector<std::string> logLevelFilter;
        std::vector<std::string> componentFilter;
        bool enableStructuredLogging = false;
        
        // Performance monitoring
        size_t performanceSampleInterval = 10;
        bool enableRealTimeMetrics = false;
        std::string metricsEndpoint;
        
        // Memory debugging
        bool enableMemoryLeakDetection = true;
        size_t memoryCheckInterval = 100;
        size_t memoryAllocationThreshold = 1024 * 1024; // 1MB
        
        // Network debugging
        bool enableNetworkCapture = false;
        std::string networkInterface;
        uint16_t networkPort = 0;
    };
    
    struct DebugSession {
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::vector<std::string> logEntries;
        std::vector<std::string> memoryUsage;
        std::vector<std::string> performanceMetrics;
        std::vector<std::string> errorMessages;
        std::map<std::string, std::string> metadata;
        bool isActive = false;
    };
    
    Debugger(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
             std::shared_ptr<AgentBrain> agent);
    
    ~Debugger();
    
    // Debug control methods
    void startDebugSession(const std::string& sessionId, const DebugConfig& config);
    void stopDebugSession();
    void pauseDebugSession();
    void resumeDebugSession();
    bool isDebugSessionActive() const;
    
    // Logging utilities
    void logDebug(const std::string& message, DebugLevel level = DebugLevel::INFO);
    void logNeuralActivity(std::shared_ptr<Brain> brain, size_t iteration);
    void logWorldState(std::shared_ptr<SimpleWorld> world, size_t iteration);
    void logAgentBehavior(std::shared_ptr<AgentBrain> agent, size_t iteration);
    
    // Performance monitoring
    void startPerformanceMeasurement(const std::string& operationName);
    void stopPerformanceMeasurement(const std::string& operationName);
    double getElapsedTime(const std::string& operationName) const;
    void collectPerformanceMetrics();
    std::string getPerformanceReport() const;
    
    // Memory debugging
    void startMemoryTracking();
    void stopMemoryTracking();
    void collectMemoryStats();
    std::string getMemoryReport() const;
    bool detectMemoryLeaks();
    
    // Neural network debugging
    void debugNeuralConnectivity(std::shared_ptr<Brain> brain);
    void debugSynapticStrength(std::shared_ptr<Brain> brain);
    void debugNeuronStates(std::shared_ptr<Brain> brain);
    void debugPlasticityRules(std::shared_ptr<Brain> brain);
    
    // World debugging
    void debugWorldPhysics(std::shared_ptr<SimpleWorld> world);
    void debugAgentPerception(std::shared_ptr<AgentBrain> agent);
    void debugAgentActionSelection(std::shared_ptr<AgentBrain> agent);
    void debugRewardSignals(std::shared_ptr<AgentBrain> agent);
    
    // Error detection and analysis
    void analyzeNeuralStability(std::shared_ptr<Brain> brain);
    void analyzeLearningProgress(std::shared_ptr<Brain> brain);
    void detectAnomalies(std::shared_ptr<Brain> brain);
    void identifyPerformanceBottlenecks();
    
    // Output management
    void exportDebugLog(const std::string& filepath);
    void exportPerformanceData(const std::string& filepath);
    void exportMemoryData(const std::string& filepath);
    void exportNeuralAnalysis(const std::string& filepath);
    
    // Configuration and profiling
    void loadDebugConfig(const std::string& configFile);
    void saveDebugConfig(const std::string& configFile) const;
    DebugConfig getDebugConfig() const;
    void updateDebugConfig(const DebugConfig& config);
    
    // Profiling utilities
    void profileNeuralSimulation(size_t iterations);
    void profileAgentBehavior(size_t iterations);
    void profileWorldInteraction(size_t iterations);
    std::string generateProfilingReport() const;
    
    // Network debugging (if available)
    void startNetworkCapture();
    void stopNetworkCapture();
    void exportNetworkData(const std::string& filepath);
    
protected:
    // Debug session management
    void updateDebugSession();
    void cleanupDebugSession();
    std::string generateDebugFilename(const std::string& baseName, const std::string& extension);
    
    // Output formatting utilities
    std::string formatLogMessage(const std::string& message, DebugLevel level);
    std::string formatTimestamp() const;
    std::string formatMemoryUsage(size_t bytes) const;
    std::string formatTimeDuration(double seconds) const;
    
    // Analysis utilities
    std::vector<std::string> analyzeLogFile(const std::string& logFile);
    std::vector<std::string> extractErrorPatterns(const std::vector<std::string>& logs);
    std::vector<std::string> detectPerformanceTrends(const std::vector<double>& metrics);
    
    // Resource management
    void cleanupResources();
    void releaseTemporaryFiles();
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    DebugSession debugSession_;
    DebugConfig debugConfig_;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> performanceTimers_;
    std::map<std::string, double> performanceMetrics_;
    std::vector<std::string> memorySnapshots_;
    bool debugActive_;
    std::mutex debugMutex_;
};

/**
 * @brief Advanced visualization and analysis tools for NLM
 * 
 * Provides comprehensive visualization capabilities for neural network data,
 * simulation results, and performance analysis.
 */
class Visualizer {
public:
    struct VisualizationConfig {
        // Output formats
        bool enableImageExport = true;
        bool enableVideoExport = false;
        bool enableWebExport = false;
        bool enableInteractivePlots = false;
        
        // Image formats
        std::string imageFormat = "png";
        int imageDPI = 300;
        int imageWidth = 1920;
        int imageHeight = 1080;
        
        // Video settings
        std::string videoFormat = "mp4";
        int videoFPS = 30;
        int videoDuration = 60;
        
        // Web settings
        std::string webFormat = "html";
        bool enableWebInteractive = true;
        std::string webTitle = "NLM Visualization";
        
        // Color schemes
        std::string colormap = "viridis";
        std::string neuralColorScheme = "default";
        
        // Analysis settings
        bool enableStatisticalAnalysis = true;
        bool enablePatternDetection = true;
        bool enableAnomalyDetection = true;
        
        // Output paths
        std::string outputDirectory;
        std::string imageSubdirectory = "images";
        std::string videoSubdirectory = "videos";
        std::string webSubdirectory = "web";
    };
    
    Visualizer(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
               std::shared_ptr<AgentBrain> agent);
    
    ~Visualizer();
    
    // Neural visualization methods
    void visualizeNeuralTopology(std::shared_ptr<Brain> brain, const std::string& outputPath);
    void visualizeSynapticConnections(std::shared_ptr<Brain> brain, const std::string& outputPath);
    void visualizeNeuralActivity(std::shared_ptr<Brain> brain, size_t iteration, const std::string& outputPath);
    void visualizeBrainRegions(std::shared_ptr<Brain> brain, const std::string& outputPath);
    void visualizePopulationDynamics(std::shared_ptr<Brain> brain, const std::string& outputPath);
    
    // World visualization methods
    void visualizeWorldTopology(std::shared_ptr<SimpleWorld> world, const std::string& outputPath);
    void visualizeAgentTrajectory(std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, const std::string& outputPath);
    void visualizeWorldResources(std::shared_ptr<SimpleWorld> world, const std::string& outputPath);
    void visualizeSensoryInput(std::shared_ptr<AgentBrain> agent, const std::string& outputPath);
    
    // Time-series visualization methods
    void visualizeTrainingProgress(const std::vector<double>& metrics, const std::string& outputPath);
    void visualizeLearningCurves(const std::vector<double>& learningMetrics, const std::string& outputPath);
    void visualizePerformanceMetrics(const std::vector<double>& performanceMetrics, const std::string& outputPath);
    void visualizeSpikeActivity(const std::vector<size_t>& spikeCounts, const std::string& outputPath);
    
    // Statistical visualization methods
    void createStatisticalSummary(const BatchResult& batchResult, const std::string& outputPath);
    void createErrorAnalysis(const BatchResult& batchResult, const std::string& outputPath);
    void createPerformanceProfile(const BatchResult& batchResult, const std::string& outputPath);
    void createMemoryAnalysis(const BatchResult& batchResult, const std::string& outputPath);
    
    // Animation and video methods
    void createNeuralSimulationVideo(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent, size_t duration, const std::string& outputPath);
    void createWorldSimulationVideo(std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent,
                                   size_t duration, const std::string& outputPath);
    void createCombinedSimulationVideo(size_t iterations, const std::string& outputPath);
    
    // Web-based visualization methods
    void createWebInterface(const BatchResult& batchResult, const std::string& outputPath);
    void createInteractiveDashboard(const BatchResult& batchResult, const std::string& outputPath);
    void createNeuralNetworkVisualizer(const BatchResult& batchResult, const std::string& outputPath);
    void createWorldSimulationVisualizer(const BatchResult& batchResult, const std::string& outputPath);
    
    // Analysis and reporting methods
    void generateAnalysisReport(const BatchResult& batchResult, const std::string& outputPath);
    void generateResearchPaperFigures(const BatchResult& batchResult, const std::string& outputPath);
    void generateTechnicalDocumentation(const BatchResult& batchResult, const std::string& outputPath);
    
    // Utility methods
    void createOutputDirectoryStructure(const VisualizationConfig& config);
    std::string getFormattedOutputPath(const std::string& subdirectory, const std::string& filename);
    void exportImage(const std::string& imageData, const std::string& filepath, const VisualizationConfig& config);
    void exportVideo(const std::string& videoData, const std::string& filepath, const VisualizationConfig& config);
    
    // Configuration management
    void loadVisualizationConfig(const std::string& configFile);
    void saveVisualizationConfig(const std::string& configFile) const;
    VisualizationConfig getVisualizationConfig() const;
    void updateVisualizationConfig(const VisualizationConfig& config);
    
    // Template visualizations
    static void createDefaultNeuralTopologyVisualization(std::shared_ptr<Brain> brain, const std::string& outputPath);
    static void createDefaultWorldVisualization(std::shared_ptr<SimpleWorld> world, const std::string& outputPath);
    static void createDefaultTrainingProgressVisualization(const BatchResult& batchResult, const std::string& outputPath);
    static void createDefaultPerformanceAnalysisVisualization(const BatchResult& batchResult, const std::string& outputPath);
    
protected:
    // Image generation utilities
    void generateNeuralTopologyImage(std::shared_ptr<Brain> brain, const std::string& outputPath);
    void generateActivityHeatmap(std::shared_ptr<Brain> brain, size_t iteration, const std::string& outputPath);
    void generateConnectivityMap(std::shared_ptr<Brain> brain, const std::string& outputPath);
    void generateRegionDistributionMap(std::shared_ptr<Brain> brain, const std::string& outputPath);
    
    // Video generation utilities
    void generateFrame(std::vector<unsigned char>& frameData, size_t frameIndex, size_t totalFrames,
                      std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent);
    void encodeVideo(const std::vector<std::vector<unsigned char>>& frames, const std::string& outputPath);
    
    // Web generation utilities
    void generateHTMLStructure(const std::string& outputPath);
    void generateCSSStyles(const std::string& outputPath);
    void generateJavaScript(const std::string& outputPath);
    void generateNeuralNetworkHTML(const BatchResult& batchResult, const std::string& outputPath);
    void generateWorldSimulationHTML(const BatchResult& batchResult, const std::string& outputPath);
    
    // Analysis utilities
    std::vector<double> calculateStatisticalMeasures(const std::vector<double>& data);
    std::vector<std::string> detectPatterns(const std::vector<double>& data);
    std::vector<std::string> detectAnomalies(const std::vector<double>& data);
    
    // Resource management
    void cleanupResources();
    void releaseTemporaryFiles();
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    VisualizationConfig vizConfig_;
    bool resourcesInitialized_;
    std::mutex vizMutex_;
};

// Convenience functions for batch processing and debugging
namespace BatchUtils {
    // Batch execution utilities
    BatchResult runQuickTest(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                            std::shared_ptr<AgentBrain> agent, size_t iterations = 100);
    BatchResult runExplorationBatch(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                    std::shared_ptr<AgentBrain> agent, const std::string& checkpointDir);
    BatchResult runBenchmarkBatch(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent, const std::string& resultsDir);
    
    // Debug utilities
    void captureNeuralSnapshot(std::shared_ptr<Brain> brain, const std::string& snapshotPath);
    void restoreNeuralSnapshot(std::shared_ptr<Brain> brain, const std::string& snapshotPath);
    std::string analyzeNeuralStability(std::shared_ptr<Brain> brain);
    std::string detectPerformanceIssues(std::shared_ptr<Brain> brain);
    
    // Configuration utilities
    std::shared_ptr<Config> createOptimizedConfig(const std::string& useCase);
    std::shared_ptr<Config> createResearchConfig(size_t neuronCount, float connectionProbability);
    std::shared_ptr<Config> createProductionConfig(size_t neuronCount);
    
    // Result utilities
    std::string formatBatchResult(const BatchResult& result);
    std::string generateBatchReport(const BatchResult& result);
    std::string calculatePerformanceScore(const BatchResult& result);
    
    // System utilities
    size_t getSystemMemoryUsage();
    size_t getSystemCPUUsage();
    std::string getSystemInfo();
    
    // File utilities
    bool createDirectory(const std::string& path);
    std::vector<std::string> getFilesInDirectory(const std::string& directory, const std::string& extension);
    bool copyFile(const std::string& source, const std::string& destination);
    bool deleteFile(const std::string& path);
    bool deleteDirectory(const std::string& path);
}

} // namespace nlm