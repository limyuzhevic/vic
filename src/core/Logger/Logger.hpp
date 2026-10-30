#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Enhanced log entry with additional metadata
struct EnhancedLogEntry : public LogEntry {
    std::string module;
    std::string threadId;
    std::map<std::string, std::string> metadata;
    
    EnhancedLogEntry() : LogEntry() {
        timestamp = std::chrono::system_clock::now();
        threadId = getCurrentThreadId();
    }
    
    static std::string getCurrentThreadId() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
};

// Log rotation configuration
struct LogRotationConfig {
    size_t maxSizeMB = 100;
    size_t maxFiles = 5;
    bool rotateDaily = false;
    bool compressOldLogs = true;
};

// Python logging integration configuration
struct PythonLogConfig {
    std::string module;
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::string outputFile;
    bool enableRotation = false;
    size_t maxFileSizeMB = 100;
    size_t maxFiles = 5;
    bool rotateDaily = false;
    bool compressLogs = true;
    
    // Performance and monitoring
    bool enablePerformanceLogging = true;
    bool enableMemoryLogging = true;
    bool enableNetworkLogging = false;
    std::string networkEndpoint;
    
    // Debug and development
    bool enableDebugLogging = false;
    bool enableTraceLogging = false;
    std::map<std::string, std::string> customMetadata;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Enhanced ConsoleLogger with log rotation and filtering
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file with rotation
    void setOutputFile(const std::string& filepath, const LogRotationConfig& config = LogRotationConfig());
    void closeOutputFile();
    
    // Filter by module
    void addModuleFilter(const std::string& module);
    void removeModuleFilter(const std::string& module);
    bool isModuleEnabled(const std::string& module) const;
    
    // Set minimum level per module
    void setModuleLevel(const std::string& module, LogLevel level);
    LogLevel getModuleLevel(const std::string& module) const;
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message, 
                      const std::map<std::string, std::string>& metadata,
                      const std::string& module = "");
    
    // Helper functions
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    bool shouldLog(const EnhancedLogEntry& entry) const;
    void rotateFileIfNeeded();
    std::string getRotatedFilename(const std::string& basePath, int index) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class
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
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Logger with enhanced features
class AdvancedLogger : public Logger {
public:
    AdvancedLogger();
    ~AdvancedLogger() override;
    
    // Component-based logging
    void setComponentEnabled(const std::string& component, bool enabled);
    bool isComponentEnabled(const std::string& component) const;
    
    // Performance monitoring
    void logPerformance(const std::string& operation, double duration,
                       const std::map<std::string, std::string>& metadata = {});
    
    // Memory tracking
    void logMemoryUsage(size_t allocated, size_t available);
    
    // Network logging
    void setNetworkLogEndpoint(const std::string& endpoint);
    void sendLogToNetwork(const EnhancedLogEntry& entry);
    
    // Log statistics
    struct LogStats {
        std::atomic<size_t> totalLogs{0};
        std::atomic<size_t> logsByLevel[5]{0}; // DEBUG, INFO, WARNING, ERROR, CRITICAL
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    };
    
    LogStats getLogStats() const;
    void resetLogStats();
    
    // Log level filtering with thresholds
    void setLogLevelThreshold(LogLevel level);
    bool isLogLevelEnabled(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Python logging integration
namespace python {
    // Python-friendly logging configuration
    struct PythonLogConfig {
        std::string module;
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        std::string outputFile;
        bool enableRotation = false;
        size_t maxFileSizeMB = 100;
        size_t maxFiles = 5;
        bool rotateDaily = false;
        bool compressLogs = true;
        
        // Performance and monitoring
        bool enablePerformanceLogging = true;
        bool enableMemoryLogging = true;
        bool enableNetworkLogging = false;
        std::string networkEndpoint;
        
        // Debug and development
        bool enableDebugLogging = false;
        bool enableTraceLogging = false;
        std::map<std::string, std::string> customMetadata;
    };
    
    // Helper functions for Python logging
    class PythonLogger {
    public:
        PythonLogger();
        
        // Configure logging from Python dictionary
        static void configure(const py::dict& config);
        
        // Get current log statistics
        static py::dict getLogStatistics();
        
        // Set log level for specific module
        static void setModuleLevel(const std::string& module, LogLevel level);
        
        // Enable/disable module
        static void setModuleEnabled(const std::string& module, bool enabled);
        
        // Get module status
        static py::dict getModuleStatus(const std::string& module);
        
        // Log performance event
        static void logPerformance(const std::string& operation, double duration,
                                  const py::dict& metadata = py::dict());
        
        // Log memory usage
        static void logMemoryUsage(size_t allocated, size_t available);
        
        // Set network logging endpoint
        static void setNetworkLogEndpoint(const std::string& endpoint);
        
        // Get all module statuses
        static py::list getAllModuleStatuses();
        
        // Clear logs
        static void clearLogs();
        
        // Reset log statistics
        static void resetStatistics();
        
        // Get current log level threshold
        static LogLevel getLogLevelThreshold();
        
        // Set log level threshold
        static void setLogLevelThreshold(LogLevel level);
        
        // Check if log level is enabled
        static bool isLogLevelEnabled(LogLevel level);
        
    private:
        static void applyConfiguration(const PythonLogConfig& config);
    };
    
    // Parse Python configuration
    LogRotationConfig parseLogRotationConfig(const std::map<std::string, std::string>& config);
    
    // Get Python-friendly log statistics
    py::dict getPythonLogStats(const AdvancedLogger::LogStats& stats);
    
    // Register Python logging module
    void registerPythonLogging(py::module& m);
}

// Global advanced logger instance
inline AdvancedLogger& getGlobalAdvancedLogger() {
    static AdvancedLogger logger;
    return logger;
}

// Convenience macros for advanced logging
#define NLM_LOG_PERFORMANCE(operation, duration) \
    do { \
        auto& logger = nlm::getGlobalAdvancedLogger(); \
        logger.logPerformance(operation, duration); \
    } while(0)

#define NLM_LOG_MEMORY(allocated, available) \
    do { \
        auto& logger = nlm::getGlobalAdvancedLogger(); \
        logger.logMemoryUsage(allocated, available); \
    } while(0)

#define NLM_LOG_EVENT(event, metadata) \
    do { \
        auto& logger = nlm::getGlobalAdvancedLogger(); \
        logger.logStructured(nlm::LogLevel::Info, event, metadata, "APPLICATION"); \
    } while(0)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

} // namespace nlm

// Python module integration
PYBIND11_MODULE(pynlm_logging, m) {
    m.doc() = R"pbdoc(
        NLM Python Logging Integration Module
        -------------------------------------
        Enhanced logging infrastructure for NLM with Python integration.
        
        Features:
        - Log rotation and file management
        - Module-level filtering
        - Performance and memory monitoring
        - Structured logging
        - Python-friendly configuration
        - Log statistics and analysis
    )pbdoc";
    
    // Register Python logging integration
    nlm::python::registerPythonLogging(m);
    
    // Export main logging classes
    py::class_<nlm::LogLevel>(m, "LogLevel")
        .value("DEBUG", nlm::LogLevel::Debug)
        .value("INFO", nlm::LogLevel::Info)
        .value("WARNING", nlm::LogLevel::Warning)
        .value("ERROR", nlm::LogLevel::Error)
        .value("CRITICAL", nlm::LogLevel::Critical);
    
    py::class_<nlm::LogRotationConfig>(m, "LogRotationConfig")
        .def(py::init<>())
        .def_readwrite("max_size_mb", &nlm::LogRotationConfig::maxSizeMB)
        .def_readwrite("max_files", &nlm::LogRotationConfig::maxFiles)
        .def_readwrite("rotate_daily", &nlm::LogRotationConfig::rotateDaily)
        .def_readwrite("compress_old_logs", &nlm::LogRotationConfig::compressOldLogs);
    
    // Global logger access
    m.def("get_global_advanced_logger", &nlm::getGlobalAdvancedLogger, "Get the global advanced logger instance");
    
    // Convenience logging functions
    m.def("configure_logging", &nlm::python::PythonLogger::configure, "Configure logging from Python dictionary");
    m.def("get_logging_stats", &nlm::python::PythonLogger::getLogStatistics, "Get current logging statistics");
    m.def("log_performance", &nlm::python::PythonLogger::logPerformance, "Log performance metrics");
    m.def("log_memory", &nlm::python::PythonLogger::logMemoryUsage, "Log memory usage statistics");
    m.def("log_event", [](const std::string& event, const py::dict& metadata) {
        std::map<std::string, std::string> metaMap;
        for (auto item : metadata) {
            metaMap[py::cast<std::string>(item.first)] = py::cast<std::string>(item.second);
        }
        
        auto& logger = nlm::getGlobalAdvancedLogger();
        logger.logStructured(nlm::LogLevel::Info, event, metaMap, "APPLICATION");
    }, "Log an event with metadata");
}
