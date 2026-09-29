// advanced_features.hpp - Advanced features for experienced users

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <thread>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "UnifiedCLI.hpp"

namespace nlm {

// Plugin system for extending NLM functionality
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getCommands() const = 0;
    
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    virtual void shutdown() = 0;
    
    virtual bool handleCommand(const std::string& command, 
                              const std::vector<std::string>& args,
                              std::string& output) = 0;
    
    virtual void onSimulationStep(std::shared_ptr<Brain> brain, 
                                  double timestep) = 0;
    
    virtual void onMemoryEvent(const std::string& eventType,
                              const std::map<std::string, std::string>& data) = 0;
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    
    std::vector<std::string> getLoadedPlugins() const;
    std::shared_ptr<NLMPlugin> getPlugin(const std::string& pluginName);
    
    bool initializeAllPlugins(const std::shared_ptr<Config>& config);
    void shutdownAllPlugins();
    
    bool executePluginCommand(const std::string& pluginName,
                             const std::string& command,
                             const std::vector<std::string>& args,
                             std::string& output);
    
    void broadcastSimulationStep(std::shared_ptr<Brain> brain, double timestep);
    void broadcastMemoryEvent(const std::string& eventType,
                              const std::map<std::string, std::string>& data);
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory) const;
    
private:
    std::map<std::string, std::shared_ptr<NLMPlugin>> plugins_;
    std::map<std::string, std::string> pluginPaths_;
};

// Advanced configuration management
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Configuration profiles
    bool saveProfile(const std::string& profileName);
    bool loadProfile(const std::string& profileName);
    bool deleteProfile(const std::string& profileName);
    std::vector<std::string> getAvailableProfiles() const;
    
    // Configuration comparison
    struct ConfigDiff {
        std::string key;
        std::string value1;
        std::string value2;
        std::string changeType; // "added", "removed", "modified"
    };
    
    std::vector<ConfigDiff> compareConfigs(const std::shared_ptr<Config>& config1,
                                          const std::shared_ptr<Config>& config2) const;
    
    // Configuration validation
    class ValidationResult {
    public:
        bool isValid() const { return errors_.empty(); }
        const std::vector<std::string>& getErrors() const { return errors_; }
        const std::vector<std::string>& getWarnings() const { return warnings_; }
        
        void addError(const std::string& error) { errors_.push_back(error); }
        void addWarning(const std::string& warning) { warnings_.push_back(warning); }
        
    private:
        std::vector<std::string> errors_;
        std::vector<std::string> warnings_;
    };
    
    ValidationResult validateConfig(const std::shared_ptr<Config>& config) const;
    
    // Configuration templates
    struct ConfigTemplate {
        std::string name;
        std::string description;
        std::shared_ptr<Config> config;
        std::vector<std::string> tags;
    };
    
    bool saveTemplate(const ConfigTemplate& template);
    ConfigTemplate getTemplate(const std::string& name) const;
    std::vector<ConfigTemplate> getAllTemplates() const;
    
    // Configuration export/import
    bool exportConfig(const std::shared_ptr<Config>& config, 
                     const std::string& filepath,
                     const std::string& format = "json");
    std::shared_ptr<Config> importConfig(const std::string& filepath,
                                         const std::string& format = "auto");
};

// Performance monitoring and profiling
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;           // Percentage
        double memoryUsage;        // MB
        double memoryUsagePercent; // Percentage
        double diskIO;             // MB/s
        double networkIO;          // MB/s
        uint64_t activeThreads;
        uint64_t totalMemory;      // MB
        double simulationTime;     // Seconds
        double realTime;           // Seconds
        uint64_t stepsPerSecond;
        uint64_t spikeCount;
        double averageFiringRate;
        double peakFiringRate;
        
        PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), 
                               memoryUsagePercent(0.0), diskIO(0.0),
                               networkIO(0.0), activeThreads(0), totalMemory(0),
                               simulationTime(0.0), realTime(0.0),
                               stepsPerSecond(0), spikeCount(0),
                               averageFiringRate(0.0), peakFiringRate(0.0) {}
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void startMonitoring(std::shared_ptr<Brain> brain);
    void stopMonitoring();
    bool isMonitoring() const { return isMonitoring_; }
    
    PerformanceMetrics getCurrentMetrics() const { return currentMetrics_; }
    std::vector<PerformanceMetrics> getHistory() const { return metricsHistory_; }
    
    // Performance analysis
    struct PerformanceAnalysis {
        double averageCpuUsage;
        double averageMemoryUsage;
        double peakMemoryUsage;
        double averageStepsPerSecond;
        double simulationEfficiency; // Steps per real time
        std::string performanceGrade; // "A", "B", "C", "D", "F"
        std::vector<std::string> recommendations;
    };
    
    PerformanceAnalysis analyzePerformance(const std::string& timeRange = "last_10min") const;
    
    // Profiling
    class Profiler {
    public:
        void startProfile(const std::string& name);
        void stopProfile(const std::string& name);
        bool isProfiling(const std::string& name) const;
        std::chrono::microseconds getProfileTime(const std::string& name) const;
        std::map<std::string, std::chrono::microseconds> getAllProfiles() const;
        void resetProfiles();
        
        std::string getProfileReport() const;
        
    private:
        std::map<std::string, std::chrono::steady_clock::time_point> startTimes_;
        std::map<std::string, std::chrono::microseconds> accumulatedTimes_;
    };
    
    Profiler& getProfiler() { return profiler_; }
    
    // Resource monitoring
    struct ResourceLimits {
        double maxMemoryMB;      // Maximum memory usage in MB
        double maxCpuPercent;    // Maximum CPU usage percentage
        uint64_t maxThreads;     // Maximum number of threads
        double maxDiskIO;        // Maximum disk I/O in MB/s
        double maxNetworkIO;     // Maximum network I/O in MB/s
        uint64_t maxSteps;       // Maximum simulation steps
        std::chrono::duration<double> maxSimulationTime; // Maximum simulation time
        
        ResourceLimits() : maxMemoryMB(1024.0), maxCpuPercent(100.0),
                          maxThreads(8), maxDiskIO(100.0), maxNetworkIO(100.0),
                          maxSteps(1000000), maxSimulationTime(std::chrono::hours(1)) {}
    };
    
    ResourceLimits getResourceLimits() const { return resourceLimits_; }
    void setResourceLimits(const ResourceLimits& limits) { resourceLimits_ = limits; }
    
    bool checkResourceLimits(const PerformanceMetrics& metrics) const;
    
    // Benchmarking
    class Benchmark {
    public:
        struct BenchmarkResult {
            std::string name;
            std::chrono::duration<double> elapsedTime;
            PerformanceMetrics metrics;
            uint64_t iterations;
            double itemsPerSecond;
            std::string status; // "passed", "failed", "skipped"
        };
        
        void addBenchmark(const std::string& name, 
                         const std::function<void()>& benchmarkFunc,
                         uint64_t iterations = 1);
        
        BenchmarkResult runBenchmark(const std::string& name);
        std::vector<BenchmarkResult> runAllBenchmarks();
        
        std::string getBenchmarkReport() const;
        
        void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }
        
    private:
        std::map<std::string, std::function<void()>> benchmarks_;
        std::map<std::string, uint64_t> iterations_;
        std::shared_ptr<Config> config_;
    };
    
    Benchmark& getBenchmark() { return benchmark_; }
    
    // Alert system
    class Alert {
    public:
        enum class AlertType {
            Performance,
            Resource,
            Error,
            Warning,
            Info
        };
        
        Alert(AlertType type, const std::string& message, 
              double severity = 1.0, 
              const std::map<std::string, std::string>& context = {});
        
        AlertType getType() const { return type_; }
        const std::string& getMessage() const { return message_; }
        double getSeverity() const { return severity_; }
        const std::map<std::string, std::string>& getContext() const { return context_; }
        
        std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
        
    private:
        AlertType type_;
        std::string message_;
        double severity_;
        std::map<std::string, std::string> context_;
        std::chrono::system_clock::time_point timestamp_;
    };
    
    void addAlert(const Alert& alert);
    std::vector<Alert> getAlerts(Alert::AlertType type = Alert::AlertType::Info) const;
    void clearAlerts() { alerts_.clear(); }
    
    std::string getAlertReport() const;
    
    // Visualization
    class Visualization {
    public:
        virtual ~Visualization() = default;
        
        virtual std::string getName() const = 0;
        virtual std::string getType() const = 0;
        
        virtual void update(const PerformanceMetrics& metrics) = 0;
        virtual std::string generateOutput() const = 0;
        
        virtual void setConfig(const std::shared_ptr<Config>& config) {}
    };
    
    void addVisualization(std::unique_ptr<Visualization> viz);
    void removeVisualization(const std::string& name);
    std::vector<std::string> getVisualizationNames() const;
    
    void updateVisualizations(const PerformanceMetrics& metrics);
    std::string generateVisualizationOutput() const;
    
    // Export
    bool exportMetrics(const std::string& filepath, 
                      const std::string& format = "csv",
                      const std::vector<std::string>& fields = {});
    bool exportProfiles(const std::string& filepath,
                       const std::string& format = "json");
    
private:
    bool isMonitoring_;
    PerformanceMetrics currentMetrics_;
    std::vector<PerformanceMetrics> metricsHistory_;
    Profiler profiler_;
    Benchmark benchmark_;
    ResourceLimits resourceLimits_;
    
    std::vector<Alert> alerts_;
    std::vector<std::unique_ptr<Visualization>> visualizations_;
    
    mutable std::mutex metricsMutex_;
    
    std::shared_ptr<Brain> monitoredBrain_;
    std::chrono::steady_clock::time_point monitoringStart_;
};

// Advanced scripting and extension interface
class AdvancedScripting {
public:
    AdvancedScripting();
    ~AdvancedScripting();
    
    // Scripting languages
    enum class Language {
        Lua,
        Python,
        JavaScript,
        Custom
    };
    
    // Script execution context
    struct ScriptContext {
        std::string name;
        Language language;
        std::string code;
        std::map<std::string, std::string> variables;
        std::map<std::string, std::function<std::string(const std::vector<std::string>&)>> functions;
        bool enabled;
        
        ScriptContext(const std::string& name, Language lang)
            : name(name), language(lang), enabled(true) {}
    };
    
    // Script execution result
    struct ScriptResult {
        bool success;
        std::string output;
        std::string error;
        std::chrono::duration<double> executionTime;
        std::map<std::string, std::string> variables;
        
        ScriptResult() : success(false), executionTime(0) {}
    };
    
    // Script management
    bool loadScript(const ScriptContext& script);
    bool unloadScript(const std::string& name);
    bool isScriptLoaded(const std::string& name) const;
    std::vector<std::string> getLoadedScripts() const;
    
    // Script execution
    ScriptResult executeScript(const std::string& name,
                               const std::map<std::string, std::string>& variables = {});
    ScriptResult executeScript(const std::string& name,
                               const std::vector<std::string>& args);
    
    // Script event handling
    void registerEventHandler(const std::string& eventType,
                              const std::string& scriptName,
                              const std::string& functionName);
    void unregisterEventHandler(const std::string& eventType,
                                const std::string& scriptName);
    void triggerEvent(const std::string& eventType,
                      const std::map<std::string, std::string>& data);
    
    // Script integration with brain
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Script debugging
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }
    
    std::string getScriptDebugInfo(const std::string& name) const;
    
    // Script validation
    struct ScriptValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ScriptValidationResult() : isValid(true) {}
    };
    
    ScriptValidationResult validateScript(const ScriptContext& script);
    
    // Script documentation
    struct ScriptDocumentation {
        std::string name;
        std::string description;
        std::string author;
        std::string version;
        std::vector<std::string> tags;
        std::map<std::string, std::string> parameters;
        std::string usage;
        std::string examples;
        
        ScriptDocumentation(const std::string& name, const std::string& desc)
            : name(name), description(desc), version("1.0.0") {}
    };
    
    void addScriptDocumentation(const ScriptDocumentation& doc);
    ScriptDocumentation getScriptDocumentation(const std::string& name) const;
    std::vector<ScriptDocumentation> getAllScriptDocumentation() const;
    
private:
    std::map<std::string, ScriptContext> scripts_;
    std::map<std::string, std::string> eventHandlers_;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> registeredEvents_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    bool debugMode_;
    
    mutable std::mutex scriptsMutex_;
};

// Extension API for third-party developers
class ExtensionAPI {
public:
    ExtensionAPI();
    ~ExtensionAPI();
    
    // Extension registration
    bool registerExtension(const std::string& name,
                           const std::string& version,
                           const std::string& description,
                           const std::function<void()>& init,
                           const std::function<void()>& shutdown);
    
    bool unregisterExtension(const std::string& name);
    bool isExtensionRegistered(const std::string& name) const;
    
    std::vector<std::string> getRegisteredExtensions() const;
    
    // Extension services
    template<typename T>
    bool provideService(const std::string& name, T service);
    
    template<typename T>
    std::optional<T> getService(const std::string& name);
    
    bool removeService(const std::string& name);
    
    // Extension communication
    void sendMessage(const std::string& extensionName,
                     const std::string& messageType,
                     const std::map<std::string, std::string>& data);
    
    std::vector<std::map<std::string, std::string>> receiveMessages(
        const std::string& extensionName,
        const std::string& messageType = "",
        int timeout = 0);
    
    // Extension lifecycle
    void initializeAllExtensions();
    void shutdownAllExtensions();
    
    // Extension discovery
    struct ExtensionInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string contact;
        std::vector<std::string> dependencies;
        std::chrono::system_clock::time_point lastUpdated;
        bool isActive;
        
        ExtensionInfo(const std::string& name, const std::string& version,
                     const std::string& desc, const std::string& auth)
            : name(name), version(version), description(desc), author(auth),
              isActive(false) {}
    };
    
    std::vector<ExtensionInfo> discoverExtensions() const;
    ExtensionInfo getExtensionInfo(const std::string& name) const;
    
    // Extension security
    void setExtensionTrusted(const std::string& name, bool trusted);
    bool isExtensionTrusted(const std::string& name) const;
    
    bool validateExtension(const std::string& name,
                           const std::map<std::string, std::string>& manifest) const;
    
private:
    std::map<std::string, ExtensionInfo> extensions_;
    std::map<std::string, std::function<void()>> initFunctions_;
    std::map<std::string, std::function<void()>> shutdownFunctions_;
    std::map<std::string, bool> trustedExtensions_;
    
    std::map<std::string, std::any> services_;
    std::vector<std::map<std::string, std::string>> messageQueue_;
    
    mutable std::mutex extensionsMutex_;
    mutable std::mutex servicesMutex_;
    mutable std::mutex messageQueueMutex_;
};

// Advanced debugging and development tools
class DevelopmentTools {
public:
    DevelopmentTools();
    ~DevelopmentTools();
    
    // Brain state inspection
    struct BrainState {
        uint64_t neuronCount;
        uint64_t synapseCount;
        uint64_t regionCount;
        uint64_t totalSpikeCount;
        uint64_t firingNeuronCount;
        double averageFiringRate;
        std::chrono::system_clock::time_point lastUpdate;
        
        // Network statistics
        double averageConnectionStrength;
        double maxConnectionStrength;
        double minConnectionStrength;
        
        // Activity metrics
        double totalActivity;
        double peakActivity;
        double averageActivity;
        
        // Development state
        std::string developmentalStage;
        double plasticityRate;
        
        BrainState() : neuronCount(0), synapseCount(0), regionCount(0),
                      totalSpikeCount(0), firingNeuronCount(0),
                      averageFiringRate(0.0), totalActivity(0.0),
                      peakActivity(0.0), averageActivity(0.0),
                      plasticityRate(0.0) {}
    };
    
    BrainState getBrainState(std::shared_ptr<Brain> brain) const;
    std::string getBrainStateString(std::shared_ptr<Brain> brain, int indent = 0) const;
    
    // Memory inspection
    struct MemoryStats {
        size_t totalMemoryAllocated;
        size_t memoryUsage;
        size_t memoryLimit;
        size_t peakMemoryUsage;
        double memoryUsagePercent;
        
        // Object counts
        size_t neuronObjectCount;
        size_t synapseObjectCount;
        size_t regionObjectCount;
        size_t otherObjectCount;
        
        // Garbage collection
        uint64_t garbageCollectionCount;
        double totalGarbageCollectionTime;
        
        MemoryStats() : totalMemoryAllocated(0), memoryUsage(0), 
                       memoryLimit(0), peakMemoryUsage(0),
                       memoryUsagePercent(0.0), garbageCollectionCount(0),
                       totalGarbageCollectionTime(0.0) {}
    };
    
    MemoryStats getMemoryStats() const;
    
    // Performance profiling
    class Profiler {
    public:
        Profiler();
        ~Profiler();
        
        void startProfiling(const std::string& category = "default");
        void stopProfiling(const std::string& category = "default");
        bool isProfiling(const std::string& category) const;
        
        std::chrono::microseconds getProfilingTime(const std::string& category) const;
        std::map<std::string, std::chrono::microseconds> getAllProfilingTimes() const;
        
        void resetProfiling();
        
        std::string getProfilingReport() const;
        
        // Performance comparison
        struct PerformanceProfile {
            std::string name;
            std::map<std::string, std::chrono::microseconds> times;
            PerformanceMetrics metrics;
            std::chrono::system_clock::time_point timestamp;
        };
        
        void saveProfile(const PerformanceProfile& profile);
        PerformanceProfile loadProfile(const std::string& name);
        std::vector<std::string> getProfileNames() const;
        
    private:
        std::map<std::string, std::chrono::steady_clock::time_point> startTimes_;
        std::map<std::string, std::chrono::microseconds> accumulatedTimes_;
        std::map<std::string, std::chrono::microseconds> currentTimes_;
    };
    
    Profiler& getProfiler() { return profiler_; }
    
    // Debug mode
    void enableDebugMode(bool enable);
    bool isDebugMode() const { return debugMode_; }
    
    // Breakpoint debugging
    class Debugger {
    public:
        Debugger();
        ~Debugger();
        
        void setBreakpoint(const std::string& condition);
        void removeBreakpoint(const std::string& condition);
        bool hasBreakpoint(const std::string& condition) const;
        
        void triggerBreakpoint(const std::string& condition);
        void continueExecution();
        bool isPaused() const { return isPaused_; }
        
        std::string getDebugState() const;
        
        // Stack trace
        struct StackFrame {
            std::string functionName;
            std::string fileName;
            int lineNumber;
            std::chrono::microseconds executionTime;
            std::map<std::string, std::string> localVariables;
        };
        
        std::vector<StackFrame> getCurrentStackTrace() const;
        
        // Watch variables
        void addWatchVariable(const std::string& expression);
        void removeWatchVariable(const std::string& expression);
        std::map<std::string, std::string> evaluateWatchVariables() const;
        
    private:
        bool isPaused_;
        std::vector<std::string> breakpoints_;
        std::vector<std::string> watchVariables_;
        std::vector<StackFrame> currentStackTrace_;
    };
    
    Debugger& getDebugger() { return debugger_; }
    
    // Simulation control
    class SimulationController {
    public:
        SimulationController();
        ~SimulationController();
        
        enum class ControlMode {
            Normal,
            FastForward,
            SlowMotion,
            StepByStep,
            Pause
        };
        
        void setControlMode(ControlMode mode);
        ControlMode getControlMode() const { return controlMode_; }
        
        void setSpeedMultiplier(double multiplier);
        double getSpeedMultiplier() const { return speedMultiplier_; }
        
        void setStepDelay(std::chrono::milliseconds delay);
        std::chrono::milliseconds getStepDelay() const { return stepDelay_; }
        
        // Simulation control commands
        void pauseSimulation();
        void resumeSimulation();
        void stepForward();
        void stepBackward();
        
        bool isSimulationPaused() const { return controlMode_ == ControlMode::Pause; }
        bool isSimulationStepping() const { return controlMode_ == ControlMode::StepByStep; }
        
        // Simulation recording
        void startRecording(const std::string& filepath);
        void stopRecording();
        bool isRecording() const { return isRecording_; }
        
        // Simulation replay
        void startReplay(const std::string& filepath);
        void stopReplay();
        bool isReplaying() const { return isReplaying_; }
        
        double getReplayPosition() const { return replayPosition_; }
        void setReplayPosition(double position);
        
        // Simulation snapshots
        void createSnapshot(const std::string& name);
        std::shared_ptr<Brain> restoreSnapshot(const std::string& name);
        std::vector<std::string> getSnapshotNames() const;
        
    private:
        ControlMode controlMode_;
        double speedMultiplier_;
        std::chrono::milliseconds stepDelay_;
        
        bool isRecording_;
        std::string recordingFilepath_;
        
        bool isReplaying_;
        std::string replayFilepath_;
        double replayPosition_;
        
        std::map<std::string, std::shared_ptr<Brain>> snapshots_;
    };
    
    SimulationController& getSimulationController() { return simulationController_; }
    
    // Export/Import
    bool exportBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    std::shared_ptr<Brain> importBrainState(const std::string& filepath);
    
    bool exportConfig(std::shared_ptr<Config> config, const std::string& filepath);
    std::shared_ptr<Config> importConfig(const std::string& filepath);
    
    // Development utilities
    class DevelopmentUtils {
    public:
        // String utilities
        static std::string generateUUID();
        static std::string generateRandomString(size_t length);
        static std::string generateHash(const std::string& input);
        
        // File utilities
        static bool createDirectory(const std::string& path);
        static bool deleteDirectory(const std::string& path);
        static std::vector<std::string> listFiles(const std::string& directory);
        static bool copyFile(const std::string& source, const std::string& destination);
        static bool moveFile(const std::string& source, const std::string& destination);
        
        // Math utilities
        static double clamp(double value, double min, double max);
        static double lerp(double a, double b, double t);
        static double easeInOut(double t);
        
        // Time utilities
        static std::string formatTime(std::chrono::duration<double> duration);
        static std::string getCurrentTimestamp();
        
        // Memory utilities
        static std::string formatMemorySize(size_t bytes);
        
        // Validation utilities
        static bool isValidIdentifier(const std::string& identifier);
        static bool isValidFilepath(const std::string& path);
        static bool isValidName(const std::string& name);
        
        // Logging utilities
        static void setLogLevel(int level);
        static int getLogLevel();
        static void logMessage(int level, const std::string& message);
        
        // Configuration utilities
        static std::string getDefaultConfigPath();
        static std::string getUserHomeDirectory();
        static std::string getApplicationDataDirectory();
    };
    
    DevelopmentUtils& getUtils() { return utils_; }
    
private:
    bool debugMode_;
    
    Profiler profiler_;
    Debugger debugger_;
    SimulationController simulationController_;
    DevelopmentUtils utils_;
};