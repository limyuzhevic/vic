// NLM Advanced Features - Command Line Interface
// Enhanced multi-mode CLI with advanced options and scripting support

#ifndef NLM_ADVANCED_CLI_HPP
#define NLM_ADVANCED_CLI_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace advanced {

enum class CLIMode {
    Run,           // Normal simulation mode
    Benchmark,     // Performance benchmarking mode
    Profile,       // CPU/memory profiling mode
    Visualize,     // Real-time visualization mode
    Batch,         // Batch processing mode
    Monitor,       // System monitoring mode
    Help,          // Help/usage information
    Version        // Version information
};

enum class OutputFormat {
    Text,          // Human-readable text output
    JSON,          // Structured JSON output
    CSV,           // Comma-separated values
    XML,           // XML format
    YAML,          // YAML format
    Custom        // Custom format (user-defined)
};

struct AdvancedOptions {
    // Core options
    CLIMode mode = CLIMode::Run;
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    OutputFormat outputFormat = OutputFormat::Text;
    
    // Advanced features
    bool verbose = false;
    int verbosityLevel = 1;
    std::vector<std::string> logDestinations; // console, file, network, database
    
    // Performance modes
    bool benchmark = false;
    bool profile = false;
    bool visualize = false;
    bool batch = false;
    bool monitor = false;
    
    // Progress tracking
    bool progressBar = false;
    bool eta = false;
    bool checkpoints = false;
    std::string checkpointPrefix = "checkpoint_";
    
    // Error handling
    bool errorRecovery = true;
    std::vector<std::string> errorCategories; // fatal, warning, recoverable, info
    
    // Parallel processing
    int parallelThreads = 1;
    bool loadBalancing = true;
    std::string distributionStrategy; // dynamic, static, adaptive
    
    // Scripting and automation
    std::string scriptFile;
    std::string batchFile;
    bool dryRun = false;
    bool skipExisting = false;
    
    // Monitoring and metrics
    bool monitoring = false;
    std::vector<std::string> metrics; // cpu, memory, network, neural
    double monitoringInterval = 1.0; // seconds
    
    // Visualization
    std::string visualizationType; // neurons, spikes, learning, network
    std::string visualizationPort; // network port for web UI
    bool realTime = false;
    
    // Command line parameters
    std::vector<std::string> args;
    std::map<std::string, std::string> namedArgs;
    std::vector<std::string> positionalArgs;
};

class CLIAdvanced {
public:
    static AdvancedOptions parseCommandLine(int argc, char** argv);
    static void printHelp(CLIMode mode = CLIMode::Help);
    static void printVersion();
    static bool validateOptions(const AdvancedOptions& options);
    static std::string optionsToString(const AdvancedOptions& options);
    static void setupLogging(const AdvancedOptions& options);
    static void executeMode(const AdvancedOptions& options);
    
    // Mode execution methods
    static void executeRunMode(const AdvancedOptions& options);
    static void executeBenchmarkMode(const AdvancedOptions& options);
    static void executeProfileMode(const AdvancedOptions& options);
    static void executeVisualizeMode(const AdvancedOptions& options);
    static void executeBatchMode(const AdvancedOptions& options);
    static void executeMonitorMode(const AdvancedOptions& options);
    
    // Utility methods
    static std::string modeToString(CLIMode mode);
    static std::string outputFormatToString(OutputFormat format);
    static CLIMode stringToMode(const std::string& modeStr);
    static OutputFormat stringToOutputFormat(const std::string& formatStr);
    static bool isValidMode(const std::string& modeStr);
    static bool isValidOutputFormat(const std::string& formatStr);
    
    // Scripting support
    static bool loadScript(const std::string& scriptFile, AdvancedOptions& options);
    static bool executeScript(const std::string& scriptFile);
    static bool executeBatchFile(const std::string& batchFile);
    static std::vector<std::string> getAvailableModes();
    static std::vector<std::string> getAvailableOutputFormats();
    
    // Progress tracking utilities
    static double calculateETA(int current, int total, double elapsedTime);
    static std::string formatProgress(int current, int total, double percentage);
    static void setupProgressTracking(const AdvancedOptions& options);
    static void updateProgress(int current, int total, const std::string& status = "");
    
    // Error handling utilities
    static void setupErrorHandling(const AdvancedOptions& options);
    static bool handleError(const std::string& errorType, const std::string& errorMessage, bool fatal = false);
    static void logError(const std::string& category, const std::string& message, const std::string& context = "");
    static std::vector<std::string> getErrorHistory();
    
    // Monitoring utilities
    static void setupMonitoring(const AdvancedOptions& options);
    static void collectMetrics();
    static void displayMetrics(const AdvancedOptions& options);
    static double getSystemMetric(const std::string& metric);
    static void setupAlerting(const std::string& condition, const std::string& action);
    
    // Visualization utilities
    static void setupVisualization(const AdvancedOptions& options);
    static void startVisualizationServer(const std::string& port);
    static void updateVisualizationData(const std::string& dataType, const std::string& data);
    static void displayVisualization(const std::string& type, const std::string& output);
    
    // Parallel processing utilities
    static void setupParallelProcessing(const AdvancedOptions& options);
    static void distributeWork(const std::vector<std::function<void()>>& tasks);
    static size_t getOptimalThreadCount();
    static void balanceLoad(const std::vector<size_t>& workloads);
    
    // Checkpoint utilities
    static void setupCheckpointing(const AdvancedOptions& options);
    static bool saveCheckpoint(const std::string& prefix = "checkpoint");
    static bool loadCheckpoint(const std::string& prefix = "checkpoint");
    static std::vector<std::string> getAvailableCheckpoints();
    static void cleanupOldCheckpoints(int keepCount = 10);
    
    // Batch processing utilities
    static bool parseBatchFile(const std::string& batchFile, std::vector<AdvancedOptions>& optionsList);
    static void processBatch(const std::vector<AdvancedOptions>& optionsList);
    static void setupBatchLogging(const std::string& logFile);
    static std::string generateBatchReport(const std::vector<AdvancedOptions>& optionsList);
    
    // Logging system utilities
    static void setupAdvancedLogging(const AdvancedOptions& options);
    static void logAdvanced(const std::string& level, const std::string& message, const std::string& module = "");
    static void configureLogRotation(const std::string& logFile, size_t maxSize, int maxFiles);
    static void setupLogDestinations(const std::vector<std::string>& destinations);
    static std::string getLogFilePath(const std::string& baseName);
    
private:
    static std::map<CLIMode, std::string> modeStrings;
    static std::map<OutputFormat, std::string> formatStrings;
    static std::vector<std::string> availableModes;
    static std::vector<std::string> availableFormats;
    static std::vector<std::string> errorHistory;
    static double lastProgressUpdate;
    static size_t currentProgress;
    static size_t totalProgress;
    static std::chrono::steady_clock::time_point startTime;
};

} // namespace advanced
} // namespace nlm

#endif // NLM_ADVANCED_CLI_HPP
