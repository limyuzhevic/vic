#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <fstream>
#include <mutex>
#include <algorithm>
#include <iomanip>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    // New enhanced levels
    PerfTrace = 5,      // Performance trace
    MemoryDebug = 6,    // Memory debugging
    NeuralDebug = 7,    // Neural network debugging
    EventDebug = 8,     // Event system debugging
    ConfigDebug = 9,    // Configuration debugging
    PluginDebug = 10    // Plugin system debugging
};

// Log entry structure (enhanced)
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadName;
    std::map<std::string, std::string> metadata;
    
    LogEntry()
        : level(LogLevel::Info), line(0), threadName("Main") {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "",
             int line = 0, const std::string& function = "", 
             const std::string& thread = "Main")
        : level(lvl), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()),
          threadName(thread) {}
};

// Performance monitoring types
struct PerformanceSample {
    std::string operation;
    std::chrono::duration<double> duration;
    std::string threadName;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    
    PerformanceSample(const std::string& op, std::chrono::duration<double> dur,
                      const std::string& thread = "Main")
        : operation(op), duration(dur), threadName(thread), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Memory tracking types
struct MemoryInfo {
    std::string operation;  // alloc, free, new, delete, etc.
    size_t size;           // size in bytes
    void* pointer;         // memory address
    std::string file;      // source file
    int line;              // line number
    std::chrono::system_clock::time_point timestamp;
    std::string threadName;
    
    MemoryInfo(const std::string& op, size_t sz, void* ptr,
               const std::string& file = "", int line = 0,
               const std::string& thread = "Main")
        : operation(op), size(sz), pointer(ptr), file(file), line(line),
          timestamp(std::chrono::system_clock::now()), threadName(thread) {}
};

// Neural debugging types
struct NeuralDebugInfo {
    enum class Type {
        NeuronCreated, NeuronDestroyed, NeuronFired, NeuronUpdated,
        SynapseCreated, SynapseDestroyed, SynapseUpdated, WeightChanged,
        RegionCreated, RegionDestroyed, PopulationCreated, PopulationUpdated,
        SpikeEvent, ConnectionChanged, PlasticityApplied, DevelopmentStage
    };
    
    Type type;
    std::string details;
    int neuronId;          // -1 for non-neuron events
    int populationId;      // -1 for non-population events
    int regionId;          // -1 for non-region events
    float value;           // numerical value if applicable
    std::string threadName;
    std::chrono::system_clock::time_point timestamp;
    
    NeuralDebugInfo(Type t, const std::string& det = "", int nid = -1, 
                   int pid = -1, int rid = -1, float val = 0.0f,
                   const std::string& thread = "Main")
        : type(t), details(det), neuronId(nid), populationId(pid), 
          regionId(rid), value(val), threadName(thread),
          timestamp(std::chrono::system_clock::now()) {}
};

// Event system debugging types
struct EventDebugInfo {
    enum class Type {
        EventReceived, EventProcessed, EventDispatched, EventCompleted,
        EventError, EventQueued, EventCancelled
    };
    
    Type type;
    std::string eventName;
    std::string source;
    std::string target;
    std::string details;
    std::string threadName;
    std::chrono::system_clock::time_point timestamp;
    
    EventDebugInfo(Type t, const std::string& name, const std::string& source = "",
                  const std::string& target = "", const std::string& details = "",
                  const std::string& thread = "Main")
        : type(t), eventName(name), source(source), target(target), 
          details(details), threadName(thread),
          timestamp(std::chrono::system_clock::now()) {}
};

// Configuration debugging types
struct ConfigDebugInfo {
    enum class Type {
        ParameterSet, ParameterGet, ParameterRemoved, ParameterValidated,
        DefaultLoaded, FileLoaded, CommandLineParsed
    };
    
    Type type;
    std::string parameterName;
    std::string oldValue;
    std::string newValue;
    std::string source;
    bool validationPassed;
    std::string errorMessage;
    std::string threadName;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigDebugInfo(Type t, const std::string& name, const std::string& oldVal = "",
                   const std::string& newVal = "", const std::string& src = "",
                   bool passed = true, const std::string& error = "",
                   const std::string& thread = "Main")
        : type(t), parameterName(name), oldValue(oldVal), newValue(newVal),
          source(src), validationPassed(passed), errorMessage(error),
          threadName(thread), timestamp(std::chrono::system_clock::now()) {}
};

// Plugin debugging types
struct PluginDebugInfo {
    enum class Type {
        PluginLoaded, PluginUnloaded, PluginError, PluginCalled,
        PluginRegistered, PluginUnregistered, PluginEnabled, PluginDisabled
    };
    
    Type type;
    std::string pluginName;
    std::string pluginVersion;
    std::string details;
    std::string threadName;
    std::chrono::system_clock::time_point timestamp;
    
    PluginDebugInfo(Type t, const std::string& name, const std::string& version = "",
                  const std::string& details = "", const std::string& thread = "Main")
        : type(t), pluginName(name), pluginVersion(version), 
          details(details), threadName(thread),
          timestamp(std::chrono::system_clock::now()) {}
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Performance monitoring
    virtual void logPerformance(const PerformanceSample& sample) = 0;
    virtual void logMemory(const MemoryInfo& info) = 0;
    virtual void logNeural(const NeuralDebugInfo& info) = 0;
    virtual void logEvent(const EventDebugInfo& info) = 0;
    virtual void logConfig(const ConfigDebugInfo& info) = 0;
    virtual void logPlugin(const PluginDebugInfo& info) = 0;
    
    // Configuration and filtering
    virtual void setPerformanceThreshold(double threshold) = 0;
    virtual void setMemoryThreshold(size_t threshold) = 0;
    virtual void enablePerformanceTracking(bool enable) = 0;
    virtual void enableMemoryTracking(bool enable) = 0;
    
    // Export functionality
    virtual std::vector<LogEntry> getLogEntries(size_t limit = 1000) const = 0;
    virtual std::vector<PerformanceSample> getPerformanceSamples(size_t limit = 1000) const = 0;
    virtual std::vector<MemoryInfo> getMemoryInfo(size_t limit = 1000) const = 0;
    
    // Filtering
    virtual void addLogFilter(std::function<bool(const LogEntry&)> filter) = 0;
    virtual void clearLogFilters() = 0;
    
    // Export to file
    virtual bool exportLogs(const std::string& filepath, 
                          const std::string& format = "json") = 0;
    virtual bool exportPerformance(const std::string& filepath) = 0;
    virtual bool exportMemory(const std::string& filepath) = 0;
    
    // Advanced features
    virtual void startTimer(const std::string& operation) = 0;
    virtual void stopTimer(const std::string& operation) = 0;
    virtual void memoryAlloc(size_t size, const std::string& file = "", int line = 0) = 0;
    virtual void memoryFree(void* ptr) = 0;
    virtual void neuralActivity(const std::string& details, int neuronId = -1) = 0;
    virtual void eventActivity(const std::string& eventName, const std::string& source = "") = 0;
    virtual void configChange(const std::string& paramName, const std::string& oldValue, 
                            const std::string& newValue, const std::string& source = "") = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance monitoring
    void logPerformance(const PerformanceSample& sample) override;
    void logMemory(const MemoryInfo& info) override;
    void logNeural(const NeuralDebugInfo& info) override;
    void logEvent(const EventDebugInfo& info) override;
    void logConfig(const ConfigDebugInfo& info) override;
    void logPlugin(const PluginDebugInfo& info) override;
    
    // Configuration and filtering
    void setPerformanceThreshold(double threshold) override;
    void setMemoryThreshold(size_t threshold) override;
    void enablePerformanceTracking(bool enable) override;
    void enableMemoryTracking(bool enable) override;
    
    // Export functionality
    std::vector<LogEntry> getLogEntries(size_t limit = 1000) const override;
    std::vector<PerformanceSample> getPerformanceSamples(size_t limit = 1000) const override;
    std::vector<MemoryInfo> getMemoryInfo(size_t limit = 1000) const override;
    
    // Filtering
    void addLogFilter(std::function<bool(const LogEntry&)> filter) override;
    void clearLogFilters() override;
    
    // Export to file
    bool exportLogs(const std::string& filepath, 
                  const std::string& format = "json") override;
    bool exportPerformance(const std::string& filepath) override;
    bool exportMemory(const std::string& filepath) override;
    
    // Advanced features
    void startTimer(const std::string& operation) override;
    void stopTimer(const std::string& operation) override;
    void memoryAlloc(size_t size, const std::string& file = "", int line = 0) override;
    void memoryFree(void* ptr) override;
    void neuralActivity(const std::string& details, int neuronId = -1) override;
    void eventActivity(const std::string& eventName, const std::string& source = "") override;
    void configChange(const std::string& paramName, const std::string& oldValue, 
                    const std::string& newValue, const std::string& source = "") override;
    
    // Enable/disable features
    void enablePerformanceTracking(bool enable) { pImpl->trackPerformance = enable; }
    void enableMemoryTracking(bool enable) { pImpl->trackMemory = enable; }
    void enableNeuralDebug(bool enable) { pImpl->trackNeural = enable; }
    void enableEventDebug(bool enable) { pImpl->trackEvents = enable; }
    void enableConfigDebug(bool enable) { pImpl->trackConfig = enable; }
    void enablePluginDebug(bool enable) { pImpl->trackPlugins = enable; }
    
    // Get current settings
    bool isPerformanceTrackingEnabled() const { return pImpl->trackPerformance; }
    bool isMemoryTrackingEnabled() const { return pImpl->trackMemory; }
    bool isNeuralDebugEnabled() const { return pImpl->trackNeural; }
    bool isEventDebugEnabled() const { return pImpl->trackEvents; }
    bool isConfigDebugEnabled() const { return pImpl->trackConfig; }
    bool isPluginDebugEnabled() const { return pImpl->trackPlugins; }
    
    // Set output file
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Performance formatting
    static std::string performanceToString(const PerformanceSample& sample);
    static std::string memoryToString(const MemoryInfo& info);
    static std::string neuralToString(const NeuralDebugInfo& info);
    static std::string eventToString(const EventDebugInfo& info);
    static std::string configToString(const ConfigDebugInfo& info);
    static std::string pluginToString(const PluginDebugInfo& info);
    
    // Timer formatting
    static std::string timerToString(const std::string& operation, 
                                   std::chrono::duration<double> duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Enhanced Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& thread = "Main");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& thread = "Main");
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", const std::string& thread = "Main");
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& thread = "Main");
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& thread = "Main");
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& thread = "Main");
    
    // Enhanced logging methods
    void performance(const std::string& operation, std::chrono::duration<double> duration,
                     const std::string& thread = "Main");
    void memoryAlloc(size_t size, const std::string& file = "", int line = 0);
    void memoryFree(void* ptr);
    void neuralActivity(const std::string& details, int neuronId = -1);
    void eventActivity(const std::string& eventName, const std::string& source = "");
    void configChange(const std::string& paramName, const std::string& oldValue,
                    const std::string& newValue, const std::string& source = "");
    void pluginActivity(const std::string& pluginName, const std::string& version = "",
                       const std::string& details = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Timer utilities
    static void startTimer(const std::string& operation);
    static void stopTimer(const std::string& operation);
    static std::chrono::duration<double> getLastTimerDuration(const std::string& operation);
    
    // Memory tracking
    static void trackMemoryAlloc(size_t size, const std::string& file = "", int line = 0);
    static void trackMemoryFree(void* ptr);
    
    // Performance analysis
    static void analyzePerformance(const std::string& outputFile = "performance_report.txt");
    static void analyzeMemory(const std::string& outputFile = "memory_report.txt");
    static void analyzeNeural(const std::string& outputFile = "neural_report.txt");
    
    // Advanced features
    static void enableAllDebugModes();
    static void disableAllDebugModes();
    static void setDebugLevel(LogLevel level);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Global timer management
    static std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> timers_;
    static std::mutex timer_mutex_;
    
    // Global memory tracking
    static std::unordered_map<void*, MemoryInfo> memoryAllocations_;
    static std::mutex memory_mutex_;
    
    // Global performance samples
    static std::vector<PerformanceSample> performanceSamples_;
    static std::mutex performance_mutex_;
    
    // Global neural debug info
    static std::vector<NeuralDebugInfo> neuralDebugInfo_;
    static std::mutex neural_mutex_;
    
    // Global event debug info
    static std::vector<EventDebugInfo> eventDebugInfo_;
    static std::mutex event_mutex_;
    
    // Global config debug info
    static std::vector<ConfigDebugInfo> configDebugInfo_;
    static std::mutex config_mutex_;
    
    // Global plugin debug info
    static std::vector<PluginDebugInfo> pluginDebugInfo_;
    static std::mutex plugin_mutex_;
};

// Convenience macros for enhanced logging
#define NLM_LOG_PERFORMANCE(operation, duration) nlm::Logger::performance(operation, duration, "Main")
#define NLM_LOG_MEMORY_ALLOC(size, file, line) nlm::Logger::trackMemoryAlloc(size, file, line)
#define NLM_LOG_MEMORY_FREE(ptr) nlm::Logger::trackMemoryFree(ptr)
#define NLM_LOG_NEURAL(details, neuronId) nlm::Logger::neuralActivity(details, neuronId)
#define NLM_LOG_EVENT(eventName, source) nlm::Logger::eventActivity(eventName, source)
#define NLM_LOG_CONFIG(paramName, oldValue, newValue, source) nlm::Logger::configChange(paramName, oldValue, newValue, source)
#define NLM_LOG_PLUGIN(pluginName, version, details) nlm::Logger::pluginActivity(pluginName, version, details)

// Timer macros
#define NLM_START_TIMER(operation) nlm::Logger::startTimer(operation)
#define NLM_STOP_TIMER(operation) nlm::Logger::stopTimer(operation)
#define NLM_GET_TIMER_DURATION(operation) nlm::Logger::getLastTimerDuration(operation)

// Memory macros
#define NLM_TRACK_ALLOC(size, file, line) nlm::Logger::trackMemoryAlloc(size, file, line)
#define NLM_TRACK_FREE(ptr) nlm::Logger::trackMemoryFree(ptr)

// Stream-style enhanced logging
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, 
              const std::string& function, const std::string& thread = "Main");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Performance logging
    LogStream& operator<<(const PerformanceSample& sample);
    LogStream& operator<<(const MemoryInfo& info);
    LogStream& operator<<(const NeuralDebugInfo& info);
    LogStream& operator<<(const EventDebugInfo& info);
    LogStream& operator<<(const ConfigDebugInfo& info);
    LogStream& operator<<(const PluginDebugInfo& info);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string thread_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Helper functions for getting debug information
namespace Debug {
    // Get all log entries
    std::vector<LogEntry> getLogEntries(size_t limit = 1000);
    
    // Get performance samples
    std::vector<PerformanceSample> getPerformanceSamples(size_t limit = 1000);
    
    // Get memory info
    std::vector<MemoryInfo> getMemoryInfo(size_t limit = 1000);
    
    // Get neural debug info
    std::vector<NeuralDebugInfo> getNeuralDebugInfo(size_t limit = 1000);
    
    // Get event debug info
    std::vector<EventDebugInfo> getEventDebugInfo(size_t limit = 1000);
    
    // Get config debug info
    std::vector<ConfigDebugInfo> getConfigDebugInfo(size_t limit = 1000);
    
    // Get plugin debug info
    std::vector<PluginDebugInfo> getPluginDebugInfo(size_t limit = 1000);
    
    // Clear all debug information
    void clearAllDebugInfo();
    
    // Export debug information
    bool exportAllDebug(const std::string& directory);
    
    // Analyze and report
    void generateDebugReport(const std::string& outputFile = "debug_report.txt");
    
    // Filter functions
    void addLogFilter(std::function<bool(const LogEntry&)> filter);
    void clearLogFilters();
}

} // namespace nlm