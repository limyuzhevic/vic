#ifndef NLM_ADVANCED_FEATURES_HPP
#define NLM_ADVANCED_FEATURES_HPP

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <chrono>
#include <map>
#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>

namespace nlm {

// Performance profiling data structure
struct PerformanceMetrics {
    std::chrono::high_resolution_clock::duration totalTime;
    std::chrono::high_resolution_clock::duration spikeProcessingTime;
    std::chrono::high_resolution_clock::duration memoryUpdateTime;
    std::chrono::high_resolution_clock::duration plasticityUpdateTime;
    std::chrono::high_resolution_clock::duration totalStepTime;
    
    size_t spikeCount;
    size_t memoryUpdateCount;
    size_t plasticityUpdateCount;
    size_t checkpointCount;
    
    float averageStepTime;  // in milliseconds
    float memoryUsage;      // in MB
    float cpuUsage;         // percentage
    
    PerformanceMetrics()
        : totalTime(0), spikeProcessingTime(0), memoryUpdateTime(0)
        , plasticityUpdateTime(0), totalStepTime(0)
        , spikeCount(0), memoryUpdateCount(0), plasticityUpdateCount(0)
        , checkpointCount(0), averageStepTime(0.0f), memoryUsage(0.0f), cpuUsage(0.0f) {}
};

// Advanced checkpoint management API
class CheckpointManagerAdvanced {
public:
    struct CheckpointOptions {
        std::string directory;
        size_t maxCheckpoints;
        size_t retentionDays;
        bool compress;
        CompressionLevel compressionLevel;
        std::string namingPattern;
        std::map<std::string, std::string> metadata;
        bool includeDiagnostics;
        bool includeNeuralConnections;
        bool includeMemoryState;
        bool includePlasticityState;
    };
    
    struct CompressionStats {
        size_t originalSize;
        size_t compressedSize;
        float compressionRatio;
        std::chrono::high_resolution_clock::duration compressionTime;
    };
    
    struct RestoreResult {
        bool success;
        std::chrono::high_resolution_clock::duration restoreTime;
        size_t neuronsRestored;
        size_t synapsesRestored;
        std::string errorMessage;
    };
    
    CheckpointManagerAdvanced() = default;
    ~CheckpointManagerAdvanced() = default;
    
    // Configure advanced checkpoint manager
    bool configure(const CheckpointOptions& options);
    
    // Create checkpoint with full diagnostics
    bool createCheckpoint(const std::string& name, 
                         const Brain& brain,
                         const std::string& description = "",
                         const std::map<std::string, std::string>& metadata = {});
    
    // Create incremental checkpoint
    bool createIncrementalCheckpoint(const Brain& brain);
    
    // Restore brain from checkpoint with diagnostics
    RestoreResult restoreCheckpoint(const std::string& name, Brain& brain);
    
    // Get checkpoint list with metadata
    std::vector<std::string> listCheckpoints(bool includeHidden = false) const;
    
    // Get checkpoint metadata
    std::map<std::string, std::string> getCheckpointMetadata(const std::string& name) const;
    
    // Delete checkpoint
    bool deleteCheckpoint(const std::string& name);
    
    // Get compression statistics
    CompressionStats getCompressionStats() const;
    
    // Get diagnostics report
    std::string getDiagnosticsReport() const;
    
    // Get performance profile
    PerformanceMetrics getPerformanceProfile() const;
    
    // Clean up old checkpoints
    size_t cleanupOldCheckpoints(size_t daysToKeep);
    
    // Get checkpoint directory
    std::string getCheckpointDirectory() const;
    
    // Set performance monitoring
    void enablePerformanceMonitoring(bool enable);
    
    // Get recent performance data
    std::vector<PerformanceMetrics> getRecentPerformance(size_t count = 10) const;

private:
    CheckpointOptions currentOptions_;
    mutable PerformanceMetrics performanceMetrics_;
    std::chrono::high_resolution_clock::time_point lastCheckpointTime_;
    std::chrono::high_resolution_clock::time_point startTime_;
    bool performanceMonitoringEnabled_ = false;
    std::vector<PerformanceMetrics> performanceHistory_;
    CompressionStats compressionStats_;
};

// Performance profiler
class PerformanceProfiler {
public:
    struct ProfileResult {
        std::string operationName;
        std::chrono::high_resolution_clock::duration duration;
        size_t iterations;
        std::chrono::high_resolution_clock::duration averageTime;
        float memoryUsage;
        float cpuUsage;
        std::string details;
    };
    
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    // Start profiling an operation
    void startProfiling(const std::string& operationName);
    
    // Stop profiling and record result
    ProfileResult stopProfiling();
    
    // Profile a function call
    template<typename Func, typename... Args>
    auto profileFunction(const std::string& operationName, Func&& func, Args&&... args) {
        startProfiling(operationName);
        auto result = func(std::forward<Args>(args)...);
        return stopProfiling();
    }
    
    // Get all profile results
    std::vector<ProfileResult> getProfileResults() const;
    
    // Clear profile results
    void clearProfileResults();
    
    // Enable real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    
    // Get system resource usage
    float getCurrentCPUUsage() const;
    float getCurrentMemoryUsage() const;  // in MB
    
    // Export profile to file
    bool exportProfile(const std::string& filepath) const;
    
    // Import profile from file
    bool importProfile(const std::string& filepath);

private:
    struct ProfileSession {
        std::string operationName;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::duration duration;
        bool completed = false;
    };
    
    std::vector<ProfileSession> activeSessions_;
    std::vector<ProfileResult> profileResults_;
    bool realTimeMonitoringEnabled_ = false;
    std::chrono::high_resolution_clock::time_point sessionStartTime_;
};

// Advanced command-line interface for NLM
class NLMCommandLineAdvanced {
public:
    enum class CommandType {
        RunSimulation,
        SaveCheckpoint,
        LoadCheckpoint,
        ProfilePerformance,
        ShowStatus,
        GenerateDiagnostics,
        ConfigureBrain,
        ExportBrainState,
        ImportBrainState,
        ListPlugins,
        EnablePlugin,
        DisablePlugin,
        SetPluginParameter,
        GetSystemInfo,
        PerformanceBenchmark,
        MemoryAnalysis,
        SpikeAnalysis,
        PlasticityAnalysis,
        ExportMetrics,
        ImportMetrics,
        CompareBrains,
        BatchProcess,
        AdvancedVisualization
    };
    
    struct CommandOptions {
        CommandType type;
        std::map<std::string, std::string> parameters;
        std::map<std::string, double> floatParameters;
        std::map<std::string, size_t> intParameters;
        std::map<std::string, bool> boolParameters;
        std::string outputFile;
        std::string inputFile;
        bool verbose = false;
        bool quiet = false;
        std::string description;
    };
    
    struct CommandResult {
        bool success;
        std::string output;
        std::string errorMessage;
        std::chrono::high_resolution_clock::duration executionTime;
        std::map<std::string, std::string> metadata;
    };
    
    NLMCommandLineAdvanced() = default;
    ~NLMCommandLineAdvanced() = default;
    
    // Execute a command
    CommandResult executeCommand(const CommandOptions& options);
    
    // Parse command line arguments
    std::vector<CommandOptions> parseCommandLine(int argc, char* argv[]);
    
    // Get available commands
    std::vector<CommandType> getAvailableCommands() const;
    
    // Check if command is available
    bool isCommandAvailable(CommandType type) const;
    
    // Get command description
    std::string getCommandDescription(CommandType type) const;
    
    // Get command usage
    std::string getCommandUsage(CommandType type) const;
    
    // Validate command parameters
    bool validateCommand(const CommandOptions& options, std::string& errorMessage) const;
    
    // Get command parameter schema
    std::map<std::string, std::string> getCommandParameterSchema(CommandType type) const;
    
    // Execute batch of commands from file
    std::vector<CommandResult> executeBatch(const std::string& batchFile);
    
    // Export command history
    bool exportCommandHistory(const std::string& filepath) const;
    
    // Import command history
    bool importCommandHistory(const std::string& filepath);
    
    // Get command execution statistics
    std::map<CommandType, size_t> getCommandStatistics() const;
    
    // Get recent command executions
    std::vector<CommandResult> getRecentCommands(size_t count = 10) const;

private:
    CommandResult executeRunSimulation(const CommandOptions& options);
    CommandResult executeSaveCheckpoint(const CommandOptions& options);
    CommandResult executeLoadCheckpoint(const CommandOptions& options);
    CommandResult executeProfilePerformance(const CommandOptions& options);
    CommandResult executeShowStatus(const CommandOptions& options);
    CommandResult executeGenerateDiagnostics(const CommandOptions& options);
    CommandResult executeConfigureBrain(const CommandOptions& options);
    // ... more command implementations
    
    std::map<CommandType, size_t> commandStatistics_;
    std::vector<CommandResult> commandHistory_;
};

// Neural network visualization interface
class VisualizationInterfaceAdvanced {
public:
    struct VisualizationOptions {
        std::string outputFormat;  // png, svg, json, html
        std::string outputFile;
        bool saveIntermediateFrames;
        std::string frameRate;  // fps
        std::map<std::string, std::string> customStyles;
        std::map<std::string, std::vector<std::string>> annotations;
        bool includeColorCoding;
        bool includeTimeLabels;
        bool includeStatistics;
        bool includeConnectivity;
        bool interactive;
        std::string interactionMode;  // hover, click, both
    };
    
    struct NeuralNetworkSnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string description;
        size_t totalNeurons;
        size_t activeNeurons;
        size_t totalSynapses;
        std::vector<float> neuronActivities;
        std::vector<std::pair<size_t, size_t>> activeConnections;
        std::map<std::string, float> metrics;
    };
    
    VisualizationInterfaceAdvanced() = default;
    ~VisualizationInterfaceAdvanced() = default;
    
    // Create visualization of current brain state
    bool visualize(const Brain& brain, const VisualizationOptions& options);
    
    // Create animated visualization
    bool createAnimation(const Brain& brain, const VisualizationOptions& options, 
                       size_t durationSteps);
    
    // Create connectivity matrix visualization
    bool visualizeConnectivity(const Brain& brain, const VisualizationOptions& options);
    
    // Create activity heatmap visualization
    bool visualizeActivityHeatmap(const Brain& brain, const VisualizationOptions& options);
    
    // Create time series visualization
    bool visualizeTimeSeries(const std::vector<NeuralNetworkSnapshot>& snapshots, 
                          const VisualizationOptions& options);
    
    // Get visualization capabilities
    std::vector<std::string> getAvailableFormats() const;
    
    // Check if format is supported
    bool isFormatSupported(const std::string& format) const;
    
    // Get default visualization options
    VisualizationOptions getDefaultOptions() const;
    
    // Save visualization configuration
    bool saveConfiguration(const std::string& filepath) const;
    
    // Load visualization configuration
    bool loadConfiguration(const std::string& filepath);
    
    // Get supported output formats
    std::map<std::string, std::string> getFormatExtensions() const;
    
    // Get format capabilities
    std::map<std::string, std::vector<std::string>> getFormatCapabilities() const;

private:
    bool generatePNG(const Brain& brain, const VisualizationOptions& options);
    bool generateSVG(const Brain& brain, const VisualizationOptions& options);
    bool generateJSON(const Brain& brain, const VisualizationOptions& options);
    bool generateHTML(const Brain& brain, const VisualizationOptions& options);
    
    std::map<std::string, std::string> formatExtensions_;
    std::map<std::string, std::vector<std::string>> formatCapabilities_;
};

// Plugin architecture for custom plasticity rules
class PlasticityPluginManagerAdvanced {
public:
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string author;
        std::string description;
        std::string filename;
        bool enabled;
        std::map<std::string, std::string> parameters;
        std::chrono::high_resolution_clock::duration loadTime;
        std::chrono::high_resolution_clock::duration lastUpdateTime;
        std::vector<std::string> dependencies;
    };
    
    struct PluginResult {
        bool success;
        std::string message;
        std::chrono::high_resolution_clock::duration executionTime;
        std::map<std::string, std::string> output;
    };
    
    PlasticityPluginManagerAdvanced() = default;
    ~PlasticityPluginManagerAdvanced() = default;
    
    // Load plugin from file
    bool loadPlugin(const std::string& filepath);
    
    // Unload plugin
    bool unloadPlugin(const std::string& name);
    
    // Enable plugin
    bool enablePlugin(const std::string& name);
    
    // Disable plugin
    bool disablePlugin(const std::string& name);
    
    // Get list of loaded plugins
    std::vector<std::string> getLoadedPlugins() const;
    
    // Get plugin info
    PluginInfo getPluginInfo(const std::string& name) const;
    
    // Get all plugin info
    std::vector<PluginInfo> getAllPluginInfo() const;
    
    // Create custom plasticity rule plugin
    bool createPlugin(const std::string& name, const std::string& description,
                    const std::string& sourceCode);
    
    // Export plugin
    bool exportPlugin(const std::string& name, const std::string& filepath);
    
    // Execute plugin
    PluginResult executePlugin(const std::string& name,
                             const std::map<std::string, std::string>& parameters);
    
    // Get plugin dependencies
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    
    // Check if plugin is installed
    bool isPluginInstalled(const std::string& name) const;
    
    // Check if plugin is enabled
    bool isPluginEnabled(const std::string& name) const;
    
    // Get plugin parameter schema
    std::map<std::string, std::string> getPluginParameterSchema(const std::string& name) const;
    
    // Set plugin parameter
    bool setPluginParameter(const std::string& name, const std::string& key,
                          const std::string& value);
    
    // Get plugin parameter
    std::string getPluginParameter(const std::string& name, const std::string& key) const;
    
    // Validate plugin
    bool validatePlugin(const std::string& name, std::string& errorMessage) const;
    
    // Get plugin compatibility info
    std::map<std::string, std::string> getPluginCompatibility(const std::string& name) const;
    
    // Get plugin update info
    std::chrono::high_resolution_clock::duration getTimeSinceLastUpdate(const std::string& name) const;
    
    // Force plugin reload
    bool reloadPlugin(const std::string& name);
    
    // Get plugin statistics
    std::map<std::string, std::string> getPluginStatistics(const std::string& name) const;

private:
    std::unordered_map<std::string, PluginInfo> plugins_;
    std::unordered_map<std::string, void*> pluginHandles_;
    std::string pluginDirectory_;
    bool autoLoadPlugins_ = false;
    std::chrono::high_resolution_clock::time_point lastPluginScan_;
};

// Advanced utility functions
namespace utils {
    
    // Get system information
    std::map<std::string, std::string> getSystemInfo();
    
    // Get memory usage information
    struct MemoryInfo {
        size_t totalMemory;
        size_t availableMemory;
        size_t usedMemory;
        float usagePercentage;
    };
    MemoryInfo getMemoryInfo();
    
    // Get CPU information
    struct CPUInfo {
        size_t logicalCores;
        float currentUsage;
        float maxUsage;
        std::vector<std::string> features;
    };
    CPUInfo getCPUInfo();
    
    // Get disk information
    struct DiskInfo {
        std::string mountPoint;
        size_t totalSpace;
        size_t usedSpace;
        size_t availableSpace;
        float usagePercentage;
    };
    std::vector<DiskInfo> getDiskInfo();
    
    // Generate unique ID
    std::string generateUniqueId();
    
    // Format number with thousands separator
    std::string formatNumber(size_t number);
    
    // Format time duration
    std::string formatDuration(const std::chrono::high_resolution_clock::duration& duration);
    
    // Get current timestamp
    std::string getCurrentTimestamp();
    
    // Create directory if it doesn't exist
    bool ensureDirectoryExists(const std::string& path);
    
    // Get file size
    size_t getFileSize(const std::string& filepath);
    
    // Check if file exists
    bool fileExists(const std::string& filepath);
    
    // List directory contents
    std::vector<std::string> listDirectory(const std::string& path);
    
    // Convert string to lowercase
    std::string toLowercase(const std::string& str);
    
    // Convert string to uppercase
    std::string toUppercase(const std::string& str);
    
    // Trim whitespace from string
    std::string trim(const std::string& str);
    
    // Split string by delimiter
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Join vector of strings with delimiter
    std::string join(const std::vector<std::string>& strings, char delimiter);
    
    // Check if string contains substring
    bool contains(const std::string& str, const std::string& substring);
    
    // Replace all occurrences of substring in string
    std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    
    // Escape special characters for JSON
    std::string escapeJSON(const std::string& str);
    
    // Unescape JSON string
    std::string unescapeJSON(const std::string& str);
    
    // Calculate hash of string
    std::string calculateHash(const std::string& str);
    
    // Get relative path
    std::string getRelativePath(const std::string& path, const std::string& base);
    
    // Expand user home directory
    std::string expandUser(const std::string& path);
    
    // Get temporary directory
    std::string getTempDirectory();
    
    // Generate random string
    std::string generateRandomString(size_t length);
    
    // Parse JSON string (simplified)
    std::map<std::string, std::string> parseJSON(const std::string& json);
    
    // Format JSON string (simplified)
    std::string formatJSON(const std::map<std::string, std::string>& data);
    
    // Get current working directory
    std::string getCurrentWorkingDirectory();
    
    // Change current working directory
    bool changeWorkingDirectory(const std::string& path);
    
    // Execute system command
    std::string executeCommand(const std::string& command);
    
    // Get environment variable
    std::string getEnvironmentVariable(const std::string& name);
    
    // Set environment variable
    bool setEnvironmentVariable(const std::string& name, const std::string& value);
    
    // Get hostname
    std::string getHostname();
    
    // Get username
    std::string getUsername();
    
    // Get process ID
    pid_t getProcessId();
    
    // Get thread ID
    std::thread::id getThreadId();
    
} // namespace utils

} // namespace nlm

#endif // NLM_ADVANCED_FEATURES_HPP
