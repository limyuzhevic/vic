#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <mutex>
#include <unordered_map>
#include <stacktrace>

namespace nlm {

// Enhanced log levels with additional severity levels
enum class EnhancedLogLevel {
    Trace = -1,
    Debug = 0,
    Info = 1,
    Notice = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Alert = 6,
    Emergency = 7
};

// Enhanced log entry with additional metadata
struct EnhancedLogEntry {
    EnhancedLogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::string threadId;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    std::map<std::string, std::string> metadata;
    std::string category;
    bool redacted; // For sensitive information
    
    EnhancedLogEntry() : level(EnhancedLogLevel::Info), line(0), redacted(false) {}
};

// Enhanced logger interface with advanced features
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    virtual void log(const EnhancedLogEntry& entry) = 0;
    virtual void setLevel(EnhancedLogLevel level) = 0;
    virtual EnhancedLogLevel getLevel() const = 0;
    
    // Log filtering
    virtual void addLogFilter(const std::string& pattern, EnhancedLogLevel minLevel) = 0;
    virtual void removeLogFilter(const std::string& pattern) = 0;
    
    // Log routing
    virtual void addLogRouter(const std::string& destination, std::function<bool(const EnhancedLogEntry&)> filter) = 0;
    virtual void removeLogRouter(const std::string& destination) = 0;
    
    // Log formatting
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string format(const EnhancedLogEntry& entry) const = 0;
    
    // Log redaction
    virtual void addRedactionPattern(const std::string& pattern) = 0;
    virtual void removeRedactionPattern(const std::string& pattern) = 0;
};

// Enhanced console logger with colors and formatting
class EnhancedConsoleLogger : public IEnhancedLogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(EnhancedLogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(EnhancedLogLevel level) override;
    EnhancedLogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Advanced formatting
    void setFormat(const std::string& format) override;
    std::string format(const EnhancedLogEntry& entry) const override;
    
    // Color codes
    std::string levelToColor(EnhancedLogLevel level) const;
    std::string levelToString(EnhancedLogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced file logger with rotation and compression
class EnhancedFileLogger : public IEnhancedLogger {
public:
    EnhancedFileLogger();
    explicit EnhancedFileLogger(EnhancedLogLevel level);
    ~EnhancedFileLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(EnhancedLogLevel level) override;
    EnhancedLogLevel getLevel() const override;
    
    // File rotation
    void setRotationPolicy(const std::string& policy, size_t maxSize = 0, int maxFiles = 0);
    
    // Compression
    void setCompression(bool enabled, const std::string& algorithm = "gzip");
    
    // File management
    void setLogDirectory(const std::string& directory);
    std::vector<std::string> getLogFiles() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structured logger for JSON/XML output
class StructuredLogger : public IEnhancedLogger {
public:
    StructuredLogger();
    explicit StructuredLogger(EnhancedLogLevel level);
    ~StructuredLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(EnhancedLogLevel level) override;
    EnhancedLogLevel getLevel() const override;
    
    // Output format
    void setOutputFormat(const std::string& format); // "json", "xml", "csv"
    
    // Schema definition
    void setSchema(const std::string& schemaFile);
    
    // Validation
    bool validateEntry(const EnhancedLogEntry& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced logger with correlation and tracing
class AdvancedLogger : public IEnhancedLogger {
public:
    AdvancedLogger();
    explicit AdvancedLogger(EnhancedLogLevel level);
    ~AdvancedLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(EnhancedLogLevel level) override;
    EnhancedLogLevel getLevel() const override;
    
    // Correlation ID management
    std::string generateCorrelationId();
    void setCorrelationId(const std::string& correlationId);
    std::string getCorrelationId() const;
    
    // Span management for distributed tracing
    struct Span {
        std::string spanId;
        std::string parentSpanId;
        std::string operation;
        std::chrono::system_clock::time_point startTime;
        EnhancedLogLevel level;
        std::map<std::string, std::string> tags;
    };
    
    std::string startSpan(const std::string& operation, const std::string& parentSpanId = "");
    void endSpan(const std::string& spanId);
    void addSpanTag(const std::string& spanId, const std::string& key, const std::string& value);
    
    // Performance monitoring
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        std::chrono::system_clock::time_point lastMeasurement;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Error tracking
    struct ErrorInfo {
        std::string errorMessage;
        std::string stackTrace;
        std::chrono::system_clock::time_point timestamp;
        std::string correlationId;
        std::map<std::string, std::string> context;
    };
    
    void logError(const std::string& message, const std::string& stackTrace = "");
    std::vector<ErrorInfo> getRecentErrors(size_t limit = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main enhanced logger class
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying, enable moving
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    EnhancedLogger(EnhancedLogger&&) noexcept;
    EnhancedLogger& operator=(EnhancedLogger&&) noexcept;
    
    // Logger management
    void addLogger(std::shared_ptr<IEnhancedLogger> logger);
    void removeLogger(const std::shared_ptr<IEnhancedLogger>& logger);
    void clearLoggers();
    
    // Global configuration
    void setLevel(EnhancedLogLevel level);
    void setDefaultFormat(const std::string& format);
    void enableColors(bool enabled);
    void enableRedaction(bool enabled);
    
    // Log methods with enhanced metadata
    void log(EnhancedLogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& source = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& source = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& source = "");
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& source = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0,
                const std::string& function = "", const std::string& source = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& source = "");
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& source = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& source = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& source = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0,
                   const std::string& function = "", const std::string& source = "");
    
    // Stream-style logging
    class LogStream {
    public:
        LogStream(EnhancedLogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
    private:
        EnhancedLogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
    };
    
    // Convenience macros
    #define NLM_LOG_TRACE(msg) nlm::EnhancedLogger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_DEBUG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_INFO(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_NOTICE(msg) nlm::EnhancedLogger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_WARNING(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ERROR(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_CRITICAL(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ALERT(msg) nlm::EnhancedLogger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_EMERGENCY(msg) nlm::EnhancedLogger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_STREAM(level) nlm::EnhancedLogger::LogStream(level, __FILE__, __LINE__, __func__)
    
    // Global logger management
    static EnhancedLogger& getGlobal();
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Log utilities
    static std::string getTimestamp();
    static std::string getThreadId();
    static std::string redactSensitive(const std::string& message);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Debugging utilities
class DebugUtilities {
public:
    DebugUtilities();
    ~DebugUtilities();
    
    // Memory debugging
    struct MemoryDebugInfo {
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        std::map<std::string, size_t> allocationsByType;
        std::vector<std::string> memoryLeaks;
    };
    
    MemoryDebugInfo getMemoryDebugInfo() const;
    
    // Object tracking
    struct ObjectTracker {
        size_t totalObjects;
        std::map<std::string, size_t> objectsByType;
        std::vector<std::string> liveObjects;
        std::vector<std::string> destroyedObjects;
    };
    
    ObjectTracker getObjectTracker() const;
    
    // Performance debugging
    struct PerformanceDebugInfo {
        double lastFrameTime;
        double averageFrameTime;
        double fps;
        std::vector<double> frameTimes;
        std::map<std::string, double> componentTimes;
    };
    
    PerformanceDebugInfo getPerformanceDebugInfo() const;
    
    // Thread debugging
    struct ThreadDebugInfo {
        size_t activeThreads;
        std::map<std::thread::id, std::string> threadNames;
        std::vector<std::string> deadlockCandidates;
    };
    
    ThreadDebugInfo getThreadDebugInfo() const;
    
    // File monitoring
    struct FileMonitor {
        std::string filepath;
        std::chrono::system_clock::time_point lastModified;
        size_t lastSize;
    };
    
    std::vector<FileMonitor> getFileMonitorInfo() const;
    
    // Network debugging
    struct NetworkDebugInfo {
        size_t packetsSent;
        size_t packetsReceived;
        size_t bytesSent;
        size_t bytesReceived;
        std::map<std::string, size_t> packetsByProtocol;
    };
    
    NetworkDebugInfo getNetworkDebugInfo() const;
    
    // System debugging
    struct SystemDebugInfo {
        double cpuLoad;
        double memoryLoad;
        double diskLoad;
        std::vector<std::string> runningProcesses;
    };
    
    SystemDebugInfo getSystemDebugInfo() const;
    
    // Debug breakpoints and watchpoints
    void setBreakpoint(const std::string& condition);
    void removeBreakpoint(const std::string& condition);
    bool checkBreakpoint(const std::string& condition);
    
    void setWatchpoint(const std::string& expression);
    void removeWatchpoint(const std::string& expression);
    bool checkWatchpoint(const std::string& expression);
    
    // Stack trace and crash analysis
    std::string getCurrentStackTrace() const;
    void analyzeStackTrace(const std::string& stackTrace);
    
    // Performance profiling
    class Profiler {
    public:
        Profiler(const std::string& name);
        ~Profiler();
        
        void start();
        void stop();
        double getElapsedTime() const;
        
    private:
        std::string name_;
        std::chrono::high_resolution_clock::time_point startTime_;
        std::chrono::high_resolution_clock::time_point endTime_;
        bool running_;
    };
    
    // Debugging session management
    void startDebugSession();
    void endDebugSession();
    bool isDebugSessionActive() const;
    
    // Log capture for debugging
    void startLogCapture();
    void stopLogCapture();
    std::vector<EnhancedLogEntry> getCapturedLogs() const;
    
    // Remote debugging support
    bool connectToDebugger(const std::string& host, int port);
    void disconnectFromDebugger();
    void sendDebugInfo(const std::string& info);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network debugging utilities
class NeuralDebugUtilities {
public:
    NeuralDebugUtilities();
    ~NeuralDebugUtilities();
    
    // Neuron debugging
    struct NeuronDebugInfo {
        std::string neuronId;
        std::string type;
        double membranePotential;
        double threshold;
        double firingRate;
        size_t spikeCount;
        std::vector<double> recentSpikes;
        std::map<std::string, double> internalState;
    };
    
    NeuronDebugInfo getNeuronDebugInfo(const std::string& neuronId) const;
    std::vector<NeuronDebugInfo> getAllNeuronDebugInfo() const;
    
    // Synapse debugging
    struct SynapseDebugInfo {
        std::string synapseId;
        std::string sourceNeuronId;
        std::string targetNeuronId;
        double weight;
        double delay;
        std::string type;
        bool enabled;
        std::vector<double> recentActivity;
    };
    
    SynapseDebugInfo getSynapseDebugInfo(const std::string& synapseId) const;
    std::vector<SynapseDebugInfo> getAllSynapseDebugInfo() const;
    
    // Brain debugging
    struct BrainDebugInfo {
        size_t totalNeurons;
        size_t totalSynapses;
        double overallActivity;
        double averageFiringRate;
        std::map<std::string, double> regionActivity;
        std::vector<std::string> activeRegions;
    };
    
    BrainDebugInfo getBrainDebugInfo() const;
    
    // Network topology debugging
    struct NetworkTopologyInfo {
        size_t totalNodes;
        size_t totalEdges;
        double averageDegree;
        std::map<size_t, size_t> degreeDistribution;
        std::vector<std::string> isolatedNodes;
        std::vector<std::string> hubNodes;
    };
    
    NetworkTopologyInfo getNetworkTopologyInfo() const;
    
    // Plasticity debugging
    struct PlasticityDebugInfo {
        std::string neuronId;
        std::string synapseId;
        double learningRate;
        double eligibilityTrace;
        double weightChange;
        std::vector<double> recentWeightChanges;
    };
    
    PlasticityDebugInfo getPlasticityDebugInfo(const std::string& neuronId, const std::string& synapseId) const;
    
    // Neuromodulation debugging
    struct NeuromodulationDebugInfo {
        std::string neuromodulatorId;
        std::string targetNeuronId;
        double concentration;
        double effect;
        double lastUpdate;
    };
    
    NeuromodulationDebugInfo getNeuromodulationDebugInfo(const std::string& neuromodulatorId) const;
    
    // Simulation debugging
    struct SimulationDebugInfo {
        double currentTime;
        size_t currentStep;
        double dt;
        double maxFiringRate;
        double minFiringRate;
        std::vector<std::string> activeNeurons;
        std::vector<std::string> inactiveNeurons;
    };
    
    SimulationDebugInfo getSimulationDebugInfo() const;
    
    // Debug visualization
    void visualizeNetwork(const std::string& outputFile, const std::string& format = "png");
    void visualizeActivity(const std::string& outputFile, const std::string& format = "png");
    void visualizePlasticity(const std::string& outputFile, const std::string& format = "png");
    
    // Debug session management
    void startDebugSession();
    void endDebugSession();
    bool isDebugSessionActive() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    struct RealTimeMetrics {
        double cpuUsage;
        double memoryUsage;
        double networkUsage;
        double diskUsage;
        std::chrono::system_clock::time_point timestamp;
    };
    
    RealTimeMetrics getRealTimeMetrics() const;
    
    // Debug commands
    std::string executeDebugCommand(const std::string& command);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm