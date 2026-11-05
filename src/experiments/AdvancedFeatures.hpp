// Advanced Features for NLM Power Users
// Phase 2+: Comprehensive tools for advanced usage and automation

#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <filesystem>

namespace advanced_features {

    // Advanced Configuration Manager
    class AdvancedConfigurationManager {
    public:
        AdvancedConfigurationManager() = default;
        ~AdvancedConfigurationManager() = default;
        
        // Load configuration from multiple sources
        bool loadConfiguration(const std::string& primaryConfig,
                              const std::vector<std::string>& overrideConfigs = {});
        
        // Validate configuration against schema
        struct ValidationResult {
            bool isValid = true;
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
            std::map<std::string, std::string> validationDetails;
        };
        
        ValidationResult validateConfiguration(const std::shared_ptr<Config>& config);
        
        // Generate configuration reports
        std::string generateConfigurationReport(const std::shared_ptr<Config>& config,
                                                bool includeDefaults = false);
        
        // Export configuration to different formats
        bool exportToJSON(const std::shared_ptr<Config>& config, 
                         const std::string& filepath);
        bool exportToYAML(const std::shared_ptr<Config>& config,
                         const std::string& filepath);
        bool exportToINI(const std::shared_ptr<Config>& config,
                        const std::string& filepath);
        
        // Import configuration from different formats
        std::shared_ptr<Config> importFromJSON(const std::string& filepath);
        std::shared_ptr<Config> importFromYAML(const std::string& filepath);
        std::shared_ptr<Config> importFromINI(const std::string& filepath);
        
        // Configuration comparison and diff
        std::string diffConfigurations(const std::shared_ptr<Config>& config1,
                                      const std::shared_ptr<Config>& config2,
                                      bool showValues = false);
        
        // Generate configuration templates
        std::string generateConfigurationTemplate(const std::string& baseConfig = "default");
        
        // Configuration optimization
        struct OptimizationResult {
            std::shared_ptr<Config> optimizedConfig;
            float performanceScore = 0.0f;
            std::vector<std::string> optimizationsApplied;
            std::vector<std::string> tradeOffs;
        };
        
        OptimizationResult optimizeConfiguration(const std::shared_ptr<Config>& config,
                                                const std::string& optimizationTarget = "balanced");
        
    private:
        std::map<std::string, std::shared_ptr<Config>> configCache_;
    };

    // Performance Profiler
    class PerformanceProfiler {
    public:
        PerformanceProfiler() = default;
        ~PerformanceProfiler() = default;
        
        // Start profiling
        void startProfile(const std::string& operationName);
        
        // Stop profiling and record results
        struct ProfileResult {
            std::string operationName;
            std::chrono::duration<double> elapsedTime;
            uint64_t memoryUsage; // bytes
            uint64_t cpuCycles;
            float cpuUtilization;
            std::map<std::string, double> customMetrics;
            std::string status;
        };
        
        ProfileResult stopProfile();
        
        // Get current profile session
        struct CurrentProfile {
            std::string operationName;
            std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
            std::vector<ProfileResult> results;
            bool isActive = false;
        };
        
        CurrentProfile getCurrentProfile() const { return currentProfile_; }
        
        // Generate performance report
        std::string generatePerformanceReport(bool includeMemory = true,
                                              bool includeCPU = true,
                                              bool includeCustom = true);
        
        // Save performance data to file
        bool savePerformanceData(const std::string& filepath,
                                const std::string& format = "csv");
        
        // Load performance data from file
        bool loadPerformanceData(const std::string& filepath);
        
        // Get performance statistics
        struct PerformanceStats {
            uint64_t totalOperations = 0;
            double totalElapsedTime = 0.0;
            double avgElapsedTime = 0.0;
            double minElapsedTime = std::numeric_limits<double>::max();
            double maxElapsedTime = 0.0;
            uint64_t totalMemoryUsage = 0;
            double avgMemoryUsage = 0.0;
            uint64_t totalCPUOperations = 0;
            float avgCPUUtilization = 0.0f;
            std::map<std::string, double> customMetricTotals;
        };
        
        PerformanceStats getPerformanceStats() const { return stats_; }
        
        // Reset profiling data
        void reset();
        
        // Set custom sampling rate
        void setSamplingRate(double samplesPerSecond) { samplingRate_ = samplesPerSecond; }
        
        // Enable/disable detailed profiling
        void setDetailedProfiling(bool enable) { detailedProfiling_ = enable; }
        
    private:
        CurrentProfile currentProfile_;
        PerformanceStats stats_;
        double samplingRate_ = 1000.0; // samples per second
        bool detailedProfiling_ = true;
    };

    // Advanced Debugger
    class AdvancedDebugger {
    public:
        AdvancedDebugger() = default;
        ~AdvancedDebugger() = default;
        
        // Set debug level
        enum class DebugLevel {
            None,
            Basic,
            Detailed,
           Verbose,
            Performance,
            Memory,
            Network,
            All
        };
        
        void setDebugLevel(DebugLevel level) { debugLevel_ = level; }
        DebugLevel getDebugLevel() const { return debugLevel_; }
        
        // Enable/disable specific debug modules
        void enableModule(const std::string& moduleName, bool enable = true);
        bool isModuleEnabled(const std::string& moduleName) const;
        
        // Set output destination
        void setOutputFile(const std::string& filepath);
        void setOutputToConsole(bool enable = true);
        void setOutputToNetwork(const std::string& host, int port);
        
        // Breakpoint management
        void addBreakpoint(const std::string& condition);
        void removeBreakpoint(const std::string& condition);
        void clearBreakpoints();
        
        // Log debug information
        void logDebug(const std::string& module, 
                     const std::string& message,
                     DebugLevel level = DebugLevel::Basic,
                     const std::map<std::string, std::string>& context = {});
        
        // Get debug log
        std::vector<std::string> getDebugLog() const { return debugLog_; }
        
        // Save debug log to file
        bool saveDebugLog(const std::string& filepath);
        
        // Load debug log from file
        bool loadDebugLog(const std::string& filepath);
        
        // Analyze debug log
        struct DebugAnalysis {
            uint64_t totalLogEntries = 0;
            uint64_t errorCount = 0;
            uint64_t warningCount = 0;
            uint64_t infoCount = 0;
            std::map<std::string, uint64_t> moduleStats;
            std::map<std::string, uint64_t> levelStats;
            std::chrono::duration<double> analysisTime = std::chrono::duration<double>::zero();
        };
        
        DebugAnalysis analyzeDebugLog();
        
        // Clear debug log
        void clearDebugLog() { debugLog_.clear(); }
        
        // Set log filter
        void setLogFilter(const std::string& filterExpression);
        
        // Get log statistics
        struct LogStatistics {
            uint64_t totalEntries = 0;
            uint64_t entriesByLevel[5] = {0}; // None, Basic, Detailed, Verbose, Performance
            uint64_t entriesByModule[10] = {0};
            double averageLogSize = 0.0;
            std::chrono::duration<double> oldestEntry = std::chrono::duration<double>::max();
            std::chrono::duration<double> newestEntry = std::chrono::duration<double>::zero();
        };
        
        LogStatistics getLogStatistics() const;
        
    private:
        DebugLevel debugLevel_ = DebugLevel::Basic;
        std::map<std::string, bool> enabledModules_;
        std::vector<std::string> debugLog_;
        std::string outputFile_;
        bool outputToConsole_ = true;
        std::string networkHost_;
        int networkPort_ = 0;
        std::vector<std::string> breakpoints_;
        std::string filterExpression_;
    };

    // Batch Processing Tools
    class BatchProcessor {
    public:
        BatchProcessor() = default;
        ~BatchProcessor() = default;
        
        // Add brain to batch
        void addBrain(const std::shared_ptr<Brain>& brain);
        
        // Add agent to batch
        void addAgent(const std::shared_ptr<AgentBrain>& agent);
        
        // Remove from batch
        void removeBrain(const std::shared_ptr<Brain>& brain);
        void removeAgent(const std::shared_ptr<AgentBrain>& agent);
        
        // Clear batch
        void clear() { brains_.clear(); agents_.clear(); }
        
        // Get batch size
        size_t getBrainCount() const { return brains_.size(); }
        size_t getAgentCount() const { return agents_.clear(); }
        
        // Run batch operations
        struct BatchOperationResult {
            std::string operationName;
            uint64_t operationsCount = 0;
            double totalTime = 0.0;
            uint64_t totalSpikes = 0;
            float avgFiringRate = 0.0f;
            std::map<std::string, double> metrics;
            std::vector<std::string> errors;
            bool success = true;
        };
        
        BatchOperationResult runBatchOperation(const std::string& operationName,
                                               const std::map<std::string, std::string>& parameters = {});
        
        // Batch training
        BatchOperationResult batchTrain(size_t stepsPerBrain,
                                       float learningRate = 0.01f,
                                       uint64_t batchSize = 10);
        
        // Batch evaluation
        BatchOperationResult batchEvaluate(const std::vector<std::vector<float>>& testInputs,
                                          const std::vector<std::vector<float>>& expectedOutputs,
                                          float tolerance = 0.1f);
        
        // Batch save/load
        bool saveBatchState(const std::string& directory);
        bool loadBatchState(const std::string& directory);
        
        // Batch comparison
        std::string compareBatchResults(const std::vector<std::shared_ptr<Brain>>& brains,
                                        const std::string& comparisonType = "performance");
        
        // Get batch statistics
        struct BatchStatistics {
            uint64_t totalBrains = 0;
            uint64_t totalAgents = 0;
            double totalSimulationTime = 0.0;
            uint64_t totalSpikesGenerated = 0;
            float avgFiringRate = 0.0f;
            std::map<std::string, double> operationStats;
        };
        
        BatchStatistics getBatchStatistics() const;
        
    private:
        std::vector<std::shared_ptr<Brain>> brains_;
        std::vector<std::shared_ptr<AgentBrain>> agents_;
    };

    // Experiment Manager
    class ExperimentManager {
    public:
        ExperimentManager() = default;
        ~ExperimentManager() = default;
        
        // Create new experiment
        std::string createExperiment(const std::string& name,
                                     const std::string& description,
                                     const std::shared_ptr<Config>& config);
        
        // Add run to experiment
        bool addExperimentRun(const std::string& experimentId,
                             const std::map<std::string, std::string>& parameters,
                             const std::string& status = "pending");
        
        // Get experiment information
        struct ExperimentInfo {
            std::string id;
            std::string name;
            std::string description;
            std::chrono::time_point<std::chrono::system_clock> creationTime;
            std::chrono::time_point<std::chrono::system_clock> lastModified;
            std::shared_ptr<Config> config;
            uint64_t runCount = 0;
            std::string status;
            std::map<std::string, std::string> metadata;
        };
        
        ExperimentInfo getExperimentInfo(const std::string& experimentId);
        
        // List all experiments
        std::vector<ExperimentInfo> listExperiments() const;
        
        // Update experiment
        bool updateExperiment(const std::string& experimentId,
                             const ExperimentInfo& info);
        
        // Delete experiment
        bool deleteExperiment(const std::string& experimentId);
        
        // Run experiment
        std::string runExperiment(const std::string& experimentId,
                                 const std::map<std::string, std::string>& overrides = {});
        
        // Get experiment runs
        std::vector<std::map<std::string, std::string>> getExperimentRuns(
            const std::string& experimentId,
            uint64_t limit = 10,
            uint64_t offset = 0);
        
        // Get experiment results
        std::string getExperimentResult(const std::string& experimentId,
                                       const std::string& runId = "latest");
        
        // Export experiment results
        bool exportExperimentResults(const std::string& experimentId,
                                    const std::string& format = "json",
                                    const std::string& filepath = "");
        
        // Import experiment results
        bool importExperimentResults(const std::string& filepath);
        
        // Compare experiment results
        std::string compareExperimentResults(const std::vector<std::string>& experimentIds);
        
        // Get experiment statistics
        struct ExperimentStatistics {
            uint64_t totalExperiments = 0;
            uint64_t totalRuns = 0;
            double totalExecutionTime = 0.0;
            float avgExecutionTime = 0.0f;
            std::map<std::string, uint64_t> experimentTypeStats;
            std::map<std::string, double> successRateStats;
        };
        
        ExperimentStatistics getExperimentStatistics() const;
        
    private:
        std::map<std::string, ExperimentInfo> experiments_;
        std::map<std::string, std::vector<std::map<std::string, std::string>>> experimentRuns_;
        std::map<std::string, std::string> experimentResults_;
    };

    // Visualization and Reporting
    class AdvancedReporter {
    public:
        AdvancedReporter() = default;
        ~AdvancedReporter() = default;
        
        // Generate performance report
        std::string generatePerformanceReport(const PerformanceProfiler& profiler,
                                              const std::string& format = "html",
                                              const std::string& template = "default");
        
        // Generate configuration report
        std::string generateConfigurationReport(const AdvancedConfigurationManager& configManager,
                                               const std::string& format = "pdf",
                                               const std::string& template = "technical");
        
        // Generate experiment report
        std::string generateExperimentReport(const ExperimentManager& experimentManager,
                                             const std::string& experimentId,
                                             const std::string& format = "markdown",
                                             const std::string& template = "scientific");
        
        // Generate memory usage report
        std::string generateMemoryReport(const std::string& reportType = "detailed",
                                         bool includeTrendAnalysis = true,
                                         bool includeComparisons = true);
        
        // Generate neural activity report
        std::string generateNeuralActivityReport(const std::shared_ptr<Brain>& brain,
                                                 const std::string& analysisType = "comprehensive",
                                                 bool includeSpikeAnalysis = true,
                                                 bool includeConnectivityAnalysis = true);
        
        // Create dashboards
        std::string createDashboard(const std::string& dashboardType = "system",
                                   const std::string& outputFormat = "html",
                                   const std::map<std::string, std::string>& options = {});
        
        // Export data to various formats
        bool exportToCSV(const std::string& filepath,
                        const std::string& dataType = "metrics",
                        const std::map<std::string, std::string>& filters = {});
        
        bool exportToJSON(const std::string& filepath,
                         const std::string& dataType = "all",
                         const std::string& compression = "none");
        
        bool exportToSQLite(const std::string& filepath,
                           const std::vector<std::string>& dataTypes = {});
        
        // Import data from various sources
        bool importFromCSV(const std::string& filepath,
                          const std::string& dataType = "metrics");
        
        bool importFromJSON(const std::string& filepath,
                           const std::string& dataType = "all");
        
        bool importFromSQLite(const std::string& filepath,
                             const std::vector<std::string>& tables = {});
        
        // Generate statistical analysis
        std::string generateStatisticalAnalysis(const std::string& dataset,
                                                const std::string& analysisType = "basic",
                                                const std::map<std::string, std::string>& options = {});
        
        // Create visualization charts
        std::string createChart(const std::string& chartType,
                               const std::string& dataSource,
                               const std::map<std::string, std::string>& chartOptions = {});
        
    private:
        std::map<std::string, std::string> reportCache_;
        std::map<std::string, std::string> dashboardCache_;
    };

    // Automation Script Engine
    class AutomationEngine {
    public:
        AutomationEngine() = default;
        ~AutomationEngine() = default;
        
        // Execute script
        bool executeScript(const std::string& scriptPath,
                          const std::map<std::string, std::string>& variables = {});
        
        // Execute Python script
        bool executePythonScript(const std::string& scriptPath,
                                const std::map<std::string, std::string>& variables = {});
        
        // Execute JavaScript script
        bool executeJavaScript(const std::string& scriptPath,
                              const std::map<std::string, std::string>& variables = {});
        
        // Execute batch commands
        bool executeBatchCommands(const std::vector<std::string>& commands,
                                 const std::map<std::string, std::string>& environment = {});
        
        // Create automation script
        std::string createAutomationScript(const std::string& scriptType,
                                           const std::map<std::string, std::string>& parameters = {});
        
        // Schedule automation task
        std::string scheduleTask(const std::string& taskName,
                                const std::string& scriptPath,
                                const std::string& schedule,
                                const std::map<std::string, std::string>& parameters = {});
        
        // Get scheduled tasks
        std::vector<std::map<std::string, std::string>> getScheduledTasks() const;
        
        // Cancel scheduled task
        bool cancelScheduledTask(const std::string& taskId);
        
        // Get task status
        std::map<std::string, std::string> getTaskStatus(const std::string& taskId) const;
        
        // Get execution log
        std::vector<std::map<std::string, std::string>> getExecutionLog(
            uint64_t limit = 100,
            const std::string& filter = "",
            const std::string& sort = "timestamp DESC");
        
        // Export automation logs
        bool exportAutomationLogs(const std::string& filepath,
                                 const std::string& format = "json");
        
        // Import automation logs
        bool importAutomationLogs(const std::string& filepath);
        
        // Analyze automation performance
        std::string analyzeAutomationPerformance(
            const std::string& timeRange = "all",
            const std::string& metric = "execution_time");
        
    private:
        std::map<std::string, std::map<std::string, std::string>> scheduledTasks_;
        std::vector<std::map<std::string, std::string>> executionLogs_;
        std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> taskStatus_;
    };

    // Main advanced features interface
    class AdvancedFeatures {
    public:
        AdvancedFeatures() = default;
        ~AdvancedFeatures() = default;
        
        // Initialize all advanced features
        bool initialize();
        
        // Shutdown all advanced features
        void shutdown();
        
        // Get advanced configuration manager
        AdvancedConfigurationManager& getConfigurationManager() { return configManager_; }
        
        // Get performance profiler
        PerformanceProfiler& getPerformanceProfiler() { return profiler_; }
        
        // Get advanced debugger
        AdvancedDebugger& getDebugger() { return debugger_; }
        
        // Get batch processor
        BatchProcessor& getBatchProcessor() { return batchProcessor_; }
        
        // Get experiment manager
        ExperimentManager& getExperimentManager() { return experimentManager_; }
        
        // Get advanced reporter
        AdvancedReporter& getReporter() { return reporter_; }
        
        // Get automation engine
        AutomationEngine& getAutomationEngine() { return automationEngine_; }
        
        // Get system status
        std::map<std::string, std::string> getSystemStatus() const;
        
        // Generate comprehensive report
        std::string generateComprehensiveReport();
        
        // Save system state
        bool saveSystemState(const std::string& filepath);
        
        // Load system state
        bool loadSystemState(const std::string& filepath);
        
        // Get feature list
        std::vector<std::string> getFeatureList() const;
        
        // Check if feature is available
        bool isFeatureAvailable(const std::string& featureName) const;
        
    private:
        AdvancedConfigurationManager configManager_;
        PerformanceProfiler profiler_;
        AdvancedDebugger debugger_;
        BatchProcessor batchProcessor_;
        ExperimentManager experimentManager_;
        AdvancedReporter reporter_;
        AutomationEngine automationEngine_;
        bool initialized_ = false;
    };

    // Global advanced features instance
    AdvancedFeatures& getAdvancedFeatures();

    // Convenience functions
    namespace utils {
        // String utilities
        std::string toLower(const std::string& str);
        std::string toUpper(const std::string& str);
        std::string trim(const std::string& str);
        bool startsWith(const std::string& str, const std::string& prefix);
        bool endsWith(const std::string& str, const std::string& suffix);
        std::vector<std::string> split(const std::string& str, char delimiter);
        std::string join(const std::vector<std::string>& strings, const std::string& separator);
        
        // File utilities
        bool fileExists(const std::string& filepath);
        std::string getFileExtension(const std::string& filepath);
        std::string getFileNameWithoutExtension(const std::string& filepath);
        std::string getFileSize(const std::string& filepath);
        std::string getCurrentTimestamp();
        std::string generateRandomString(size_t length);
        
        // JSON utilities
        std::string jsonEscape(const std::string& str);
        bool isValidJSON(const std::string& jsonStr);
        
        // Mathematical utilities
        double clamp(double value, double min, double max);
        double interpolate(double value, double inMin, double inMax, double outMin, double outMax);
        double normalize(double value, double min, double max);
        
        // String formatting
        std::string format(const std::string& format, ...);
        
        // Time utilities
        std::chrono::system_clock::time_point parseTime(const std::string& timeStr);
        std::string formatTime(const std::chrono::system_clock::time_point& timePoint);
        
        // Color utilities (for terminal output)
        std::string colorize(const std::string& text, const std::string& color);
        std::string getColorCode(const std::string& color);
        
        // Progress reporting
        void showProgressBar(int current, int total, const std::string& message = "");
        
        // Logging utilities
        void logError(const std::string& message, const std::string& errorCode = "");
        void logWarning(const std::string& message, const std::string& warningCode = "");
        void logInfo(const std::string& message, const std::string& infoCode = "");
        void logDebug(const std::string& message, const std::string& debugCode = "");
    }

} // namespace advanced_features

// Global function to initialize advanced features
bool initializeAdvancedFeatures();

// Global function to shutdown advanced features
void shutdownAdvancedFeatures();

// Global function to get advanced features instance
advanced_features::AdvancedFeatures& getAdvancedFeatures();

// Global function to check if advanced features are available
bool isAdvancedFeaturesAvailable();

// Global function to get available features
std::vector<std::string> getAvailableFeatures();