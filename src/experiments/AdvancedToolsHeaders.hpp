#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace nlm {

// Advanced Configuration Manager - CLI tool for complex configurations
class AdvancedConfigManager {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    // Load and save configurations
    virtual bool loadConfig(const std::string& filepath);
    virtual bool saveConfig(const std::string& filepath) const;
    
    // Create and manage configuration profiles
    virtual void createProfile(const std::string& name, const std::map<std::string, std::string>& config);
    virtual std::map<std::string, std::string> getProfile(const std::string& name) const;
    virtual void deleteProfile(const std::string& name);
    virtual std::vector<std::string> listProfiles() const;
    
    // Configuration validation and optimization
    virtual bool validateConfig(const std::map<std::string, std::string>& config) const;
    virtual std::map<std::string, std::string> optimizeConfig(const std::map<std::string, std::string>& config) const;
    virtual std::map<std::string, double> compareConfigs(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2) const;
    
    // Configuration scenarios
    virtual void applyScenario(std::map<std::string, std::string>& config, const std::string& scenario) const;
    virtual std::vector<std::string> listScenarios() const;
    
    // Configuration statistics and analysis
    virtual std::map<std::string, double> getStats(const std::map<std::string, std::string>& config) const;
    
    // Configuration export/import
    virtual bool exportConfig(const std::map<std::string, std::string>& config, const std::string& filepath) const;
    virtual bool importConfig(const std::string& filepath, std::map<std::string, std::string>& config) const;
};

// Performance Profiler - Benchmark and profile brain simulation performance
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Benchmarking
    virtual void startBenchmark(const std::string& name, const std::map<std::string, std::string>& config);
    virtual PerformanceResult stopBenchmark();
    virtual std::vector<PerformanceResult> getBenchmarks() const;
    
    // Profiling specific operations
    virtual void profileMemoryAllocation(const std::string& operation, std::function<void()> func);
    virtual void profileSynapticOperations(const std::string& operation, std::function<void()> func);
    virtual void profilePlasticityOperations(const std::string& operation, std::function<void()> func);
    virtual void profileMemoryOperations(const std::string& operation, std::function<void()> func);
    
    // Performance analysis
    virtual PerformanceProfile getProfile() const;
    virtual std::vector<std::string> findBottlenecks() const;
    virtual std::map<std::string, double> calculateRatios() const;
    
    // Performance reporting
    virtual std::string generateReport() const;
    virtual bool saveProfile(const std::string& filepath) const;
    virtual bool loadProfile(const std::string& filepath);
};

// Data Export Tool - Export simulation data for analysis
class DataExportTool {
public:
    DataExportTool();
    ~DataExportTool();
    
    // Export different data types
    virtual bool exportRawData(const std::vector<ExperimentRun>& runs, const std::string& filepath);
    virtual bool exportMetrics(const Experiment& experiment, const std::string& filepath);
    virtual bool exportNeuralState(const std::vector<ExperimentRun>& runs, const std::string& filepath);
    virtual bool exportBehavioralData(const std::vector<ExperimentRun>& runs, const std::string& filepath);
    virtual bool exportConfig(const std::map<std::string, std::string>& config, const std::string& filepath);
    
    // Export configurations
    virtual bool exportToMultipleFormats(const std::vector<ExperimentRun>& runs, const std::vector<std::pair<std::string, std::string>>& formats);
    virtual bool mergeDataSets(const std::vector<ExperimentRun>& runs, const std::string& filepath);
    
    // Import and validate data
    virtual bool importData(const std::string& filepath, Experiment& experiment);
    virtual std::vector<std::string> checkCompatibility(const std::vector<std::string>& files) const;
    
    // Specialized exports
    virtual bool exportForMachineLearning(const std::vector<ExperimentRun>& runs, const std::string& filepath, const MLExportConfig& config);
    virtual bool generateMetadata(const std::vector<ExperimentRun>& runs, const std::string& filepath);
    virtual bool getCompressionStats() const;
};

// Memory Analysis Tools - Analyze memory usage and optimize performance
class MemoryAnalysisTools {
public:
    MemoryAnalysisTools();
    ~MemoryAnalysisTools();
    
    // Memory analysis
    virtual MemoryStats analyzeWorkingMemory(WorkingMemory* memory);
    virtual MemoryStats analyzeEpisodicMemory(EpisodicMemory* memory);
    virtual MemoryStats analyzeConsolidation(const std::vector<double>& metrics);
    virtual MemoryStats analyzeReplay(const std::vector<EpisodicMemory::Episode>& episodes);
    
    // Memory profiling
    virtual MemoryProfile profileMemoryAllocation(const std::string& operation, std::function<void()> func);
    virtual UsagePattern analyzeUsagePatterns(WorkingMemory* wm, EpisodicMemory* em);
    virtual LeakAnalysis detectMemoryLeaks(const std::vector<uint64_t>& created, const std::vector<uint64_t>& deleted);
    virtual FragmentationAnalysis analyzeFragmentation(WorkingMemory* memory);
    
    // Memory optimization
    virtual std::string generateOptimizationReport() const;
    virtual bool exportAnalysis(const std::string& filepath);
    
    // Memory prediction and benchmarking
    virtual MemoryPrediction predictMemoryUsage(int neurons, int regions, float connectionProb);
    virtual MemoryBenchmark benchmarkMemoryAllocation(const std::map<std::string, std::string>& config);
    virtual bool analyzeMemoryUsage(const std::map<std::string, std::string>& config);
};

// Visualization Tools - Generate visualizations and reports
class VisualizationTools {
public:
    VisualizationTools();
    ~VisualizationTools();
    
    // Visualization generation
    virtual bool generateBrainNetworkVisualization(NeuralRegion* region, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateNeuralActivityHeatmap(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateRewardLearningCurve(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateMemoryUsagePlot(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateNeuromodulationDynamics(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateDevelopmentProgression(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateMultiPanelDashboard(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generate3DVisualization(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    
    // Advanced visualization
    virtual bool generateComparisonPlots(const std::vector<ExperimentRun>& runs1, const std::vector<ExperimentRun>& runs2, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateStatisticalVisualization(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateTemporalEvolutionPlots(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    
    // Export visualization data
    virtual bool exportVisualizationParameters(const std::string& filepath);
    virtual bool generateReportWithVisualizations(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool createPresentationSlides(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateEmbeddingVisualization(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateInteractiveDashboard(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateVideoSummary(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateVRVisualization(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generateARMarkers(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    virtual bool generate3DPrintableModel(const std::vector<ExperimentRun>& runs, const std::string& filepath, const VisualizationConfig& config);
    
    // API and plugin generation
    virtual bool generateAPIDocumentation(const std::string& filepath, const VisualizationConfig& config);
    virtual bool generatePluginCode(const std::string& filepath, const VisualizationConfig& config);
};

// Debugging Tools - Advanced debugging and tracing capabilities
class DebuggingTools {
public:
    DebuggingTools();
    ~DebuggingTools();
    
    // Debugging configuration
    virtual void setLogLevel(DebugLevel level);
    virtual DebugLevel getLogLevel() const;
    virtual void enableStackTrace(bool enable);
    virtual void enableMemoryLeakDetection(bool enable);
    virtual void enablePerformanceProfiling(bool enable);
    virtual void enableStateValidation(bool enable);
    
    // Debugging operations
    virtual void logDebug(const std::string& message, DebugLevel level);
    virtual std::vector<std::string> captureStackTrace();
    virtual MemoryMonitor startMemoryMonitoring();
    virtual void stopMemoryMonitoring();
    virtual PerformanceProfiler startPerformanceProfiling();
    virtual PerformanceResult stopPerformanceProfiling();
    
    // State debugging
    virtual bool validateBrainState(Brain* brain);
    virtual std::vector<std::string> getValidationErrors() const;
    virtual std::map<std::string, double> compareState(const std::map<std::string, double>& expected);
    
    // Memory debugging
    virtual LeakAnalysis checkMemoryLeaks();
    
    // Export debugging data
    virtual std::string generateDebugReport() const;
    virtual bool exportDebugLogs(const std::string& filepath);
    virtual bool exportPerformanceProfile(const std::string& filepath);
    virtual bool exportMemoryAnalysis(const std::string& filepath);
    virtual bool exportStateValidation(const std::string& filepath);
    
    // Script and documentation generation
    virtual bool generateDebuggingScript(const std::string& filepath, const DebuggingConfig& config);
    virtual bool generateDebugVisualization(const std::string& filepath, const DebuggingConfig& config);
    virtual bool createDebugSnapshot(const std::string& filepath);
    virtual bool exportDebuggingArtifacts(const std::string& filepath);
    virtual bool restoreDebugState(const std::string& filepath);
    virtual bool generateDebuggingChecklist(const std::string& filepath, const DebuggingConfig& config);
    virtual bool generateDebuggingGuidelines(const std::string& filepath, const DebuggingConfig& config);
    virtual bool createDebuggingTemplate(const std::string& filepath, const DebuggingConfig& config);
    virtual bool exportDebuggingConfig(const std::string& filepath);
    virtual bool generateDebuggingHelp(const std::string& filepath);
};

// Plugin System Framework - Extensible plugin architecture
class PluginSystemFramework {
public:
    PluginSystemFramework();
    ~PluginSystemFramework();
    
    // Plugin management
    virtual bool loadPlugin(const std::string& filepath);
    virtual bool unloadPlugin(const std::string& name);
    virtual bool enablePlugin(const std::string& name);
    virtual bool disablePlugin(const std::string& name);
    virtual std::vector<std::string> listPlugins() const;
    virtual PluginInfo getPluginInfo(const std::string& name) const;
    
    // Plugin execution
    virtual bool executePlugin(const std::string& name, const std::vector<std::string>& args);
    virtual bool broadcastEvent(const std::string& event, const std::map<std::string, std::string>& data);
    
    // Plugin development
    virtual bool createPluginTemplate(const std::string& filepath, const PluginConfig& config);
    virtual std::string generatePluginCode(const PluginTemplate& template, const PluginConfig& config);
    virtual bool compilePlugin(const std::string& filepath);
    
    // Plugin services
    virtual bool registerService(const std::string& name, std::function<void()> service);
    virtual bool callService(const std::string& name, const std::vector<std::string>& args);
    virtual std::vector<std::string> listServices() const;
};

// Advanced Experiment Framework - Framework for complex experimental designs
class AdvancedExperimentFramework {
public:
    AdvancedExperimentFramework();
    ~AdvancedExperimentFramework();
    
    // Experiment management
    virtual std::shared_ptr<AdvancedExperiment> createExperiment(const std::string& type, const std::map<std::string, std::string>& params);
    virtual bool runExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    virtual bool stopExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    virtual bool pauseExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    virtual bool resumeExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    
    // Experiment management
    virtual std::vector<std::shared_ptr<AdvancedExperiment>> listExperiments() const;
    virtual bool deleteExperiment(const std::string& name);
    virtual bool saveExperiment(std::shared_ptr<AdvancedExperiment> experiment, const std::string& filepath);
    virtual std::shared_ptr<AdvancedExperiment> loadExperiment(const std::string& filepath);
    
    // Experiment execution control
    virtual bool startBatch(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
    virtual bool stopBatch();
    virtual bool pauseBatch();
    virtual bool resumeBatch();
    
    // Experiment monitoring
    virtual ExperimentStatus getExperimentStatus(std::shared_ptr<AdvancedExperiment> experiment) const;
    virtual std::map<std::string, double> getExperimentMetrics(std::shared_ptr<AdvancedExperiment> experiment) const;
    virtual std::vector<std::string> getExperimentLogs(std::shared_ptr<AdvancedExperiment> experiment) const;
    
    // Experiment configuration
    virtual bool configureExperiment(std::shared_ptr<AdvancedExperiment> experiment, const std::map<std::string, std::string>& config);
    virtual std::map<std::string, std::string> getExperimentConfig(std::shared_ptr<AdvancedExperiment> experiment) const;
    
    // Experiment reporting
    virtual std::string generateExperimentReport(std::shared_ptr<AdvancedExperiment> experiment) const;
    virtual bool exportExperimentResults(std::shared_ptr<AdvancedExperiment> experiment, const std::string& filepath);
    virtual bool importExperimentResults(const std::string& filepath, std::shared_ptr<AdvancedExperiment> experiment);
};

// Supporting data structures and types
struct PerformanceResult {
    std::string name;
    double totalTime;
    double averageFps;
    double averageMemoryUsage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

struct PerformanceProfile {
    std::vector<PerformanceResult> benchmarks;
    std::vector<MemoryAllocationProfile> memoryAllocations;
    std::vector<SynapticOperationProfile> synapticOperations;
    std::vector<PlasticityOperationProfile> plasticityOperations;
    std::vector<MemoryOperationProfile> memoryOperations;
    std::string mostTimeConsumingOperation;
    double highestTime;
    std::string highestMemoryOperation;
    double highestMemory;
};

struct MemoryStats {
    size_t activeTraces;
    size_t capacity;
    double utilizationRatio;
    double averageTraceStrength;
    size_t competitionCount;
    double conflictRate;
    size_t episodeCount;
    double averageEpisodeLength;
    double memoryDensity;
    double replayEfficiency;
    double consolidationRate;
    double retrievalSuccessRate;
    double peakConsolidation;
    size_t timeToPeak;
    double halfLife;
    double finalConsolidation;
    size_t allocationCount;
    double averageAllocationTime;
    size_t peakMemoryUsage;
    double memoryGrowthRate;
    size_t peakUsageTime;
    double periodicity;
    double predictiveCompressionRatio;
    double compressionEfficiency;
    bool leakDetected;
    double leakRate;
    std::string growthTrend;
    std::string suggestedCleanup;
    double fragmentationLevel;
    bool needsDefragmentation;
    size_t wastedMemory;
    double optimizationPotential;
};

struct ExperimentRun {
    std::string runId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<Experiment::StepData> stepData;
    std::vector<std::pair<std::string, double>> metrics;
    double totalReward;
    double averageFiringRate;
    size_t totalSpikes;
};

struct CompressionStats {
    size_t originalSize;
    size_t compressedSize;
    double compressionRatio;
};

struct MLExportConfig {
    std::vector<std::string> features;
    std::string target;
    std::string format;
    bool normalize;
    bool splitTrainTest;
};

struct MemoryPrediction {
    double predictedMemoryUsage;
    double confidence;
    double uncertainty;
};

struct MemoryBenchmark {
    double allocationsPerSecond;
    double averageAllocationTime;
    double peakThroughput;
};

struct VisualizationConfig {
    std::map<std::string, std::string> options;
    std::string theme;
    bool interactive;
    double animationDuration;
};

struct DebuggingConfig {
    DebugLevel logLevel;
    bool enableStackTrace;
    bool enableMemoryLeakDetection;
    bool enablePerformanceProfiling;
    bool enableStateValidation;
    std::string outputFormat;
};

struct DebugLevel {
    enum Level { Trace, Debug, Info, Warning, Error, Critical };
    Level level;
};

struct PluginInfo {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::vector<std::string> dependencies;
    bool enabled;
};

struct PluginConfig {
    std::string name;
    std::string version;
    std::string author;
    std::string language;
    std::string framework;
    std::map<std::string, std::string> parameters;
};

struct PluginTemplate {
    std::string name;
    std::string description;
    std::string codeTemplate;
    std::vector<std::string> requiredFiles;
    std::vector<std::string> optionalFiles;
};

struct AdvancedExperiment {
    virtual ~AdvancedExperiment() {}
    virtual std::string getName() const = 0;
    virtual ExperimentStatus getStatus() const = 0;
    virtual std::map<std::string, double> getMetrics() const = 0;
    virtual std::vector<std::string> getLogs() const = 0;
    virtual bool run() = 0;
    virtual bool stop() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
};

enum ExperimentStatus { NotStarted, Running, Paused, Stopped, Completed, Error };

} // namespace nlm
