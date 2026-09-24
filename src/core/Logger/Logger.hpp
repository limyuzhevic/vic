#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Trace = -1  // Additional level for very detailed logging
};

// Log domains for context-based filtering
enum class LogDomain {
    Core,          // Core system operations
    Random,        // Random number generation
    Simulation,    // Simulation clock and timing
    Brain,         // Neural components
    Neuron,        // Individual neuron dynamics
    Synapse,       // Synaptic connections
    Plasticity,    // Plasticity mechanisms (STDP, Hebbian)
    Sensory,       // Sensory processing
    Motor,         // Motor system
    Environment,   // World simulation
    Agent,         // Agent brain interface
    Development,   // Developmental processes
    Memory,        // Memory systems
    Prediction,    // Predictive systems
    Cognition,     // Cognitive processes
    Neuromodulation, // Neuromodulators (dopamine, etc.)
    Experiments,   // Experiment management
    Performance,   // Performance monitoring
    Error,         // Error conditions
    System,        // System-level operations
    All            // All domains
};

// Log entry structure with enhanced context
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced context information
    std::string experimentId;
    int simulationStep;
    double simulationTime;
    std::string threadId;
    LogDomain domain;
    std::map<std::string, std::string> context;  // Key-value context pairs
    
    // Performance metrics
    std::chrono::microseconds processingTime;
    size_t memoryUsage;
    
    // Statistics
    static int totalEntries;
    static int entriesByLevel[6];  // Index by LogLevel
    static std::map<LogDomain, int> entriesByDomain;
    
    LogEntry() : level(LogLevel::Info), simulationStep(0), simulationTime(0.0),
                 threadId(), domain(LogDomain::All), processingTime(0), memoryUsage(0) {}
    
    // Helper methods
    std::string getFormattedTimestamp() const;
    std::string getLevelString() const;
    std::string getDomainString() const;
    std::string toJSON() const;
    std::string toCSV() const;
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

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// File logger implementation with rotation
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Configure file logging
    void setFilePath(const std::string& filepath);
    void setMaxFileSize(size_t maxBytes);
    void setMaxFiles(int maxFiles);
    void setRotateOnOpen(bool rotate);
    
    // Get log statistics
    size_t getTotalEntries() const;
    size_t getEntriesByLevel(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Async logger implementation
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000);
    explicit AsyncLogger(LogLevel level, size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Wait for all log entries to be processed
    void flush();
    
    // Get queue statistics
    size_t getQueueSize() const;
    size_t getProcessedEntries() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structured logger implementation
class StructuredLogger : public ILogger {
public:
    StructuredLogger();
    explicit StructuredLogger(LogLevel level);
    ~StructuredLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Configure output format
    void setOutputFormat(const std::string& format);
    void setIndentSize(int size);
    void setPrettyPrint(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Get console statistics
    size_t getConsoleOutputCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Log filter for selective logging
class LogFilter {
public:
    LogFilter();
    
    // Add domain filter
    void addDomainFilter(LogDomain domain, bool include = true);
    void removeDomainFilter(LogDomain domain);
    bool isDomainFiltered(LogDomain domain) const;
    
    // Add level filter
    void setLevelFilter(LogLevel minLevel, LogLevel maxLevel);
    bool shouldLog(LogLevel level) const;
    
    // Add context filter
    void addContextFilter(const std::string& key, const std::string& value);
    void removeContextFilter(const std::string& key);
    bool matchesContext(const std::map<std::string, std::string>& context) const;
    
    // Check if log entry passes all filters
    bool passesFilter(const LogEntry& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log aggregator for statistics and analysis
class LogAggregator {
public:
    LogAggregator();
    ~LogAggregator();
    
    // Process log entry
    void processEntry(const LogEntry& entry);
    
    // Get statistics
    double getProcessingRate() const;  // entries per second
    size_t getTotalEntries() const;
    size_t getEntriesByLevel(LogLevel level) const;
    size_t getEntriesByDomain(LogDomain domain) const;
    
    // Get error statistics
    double getErrorRate() const;
    size_t getCriticalErrors() const;
    std::vector<LogEntry> getRecentErrors(int count = 10) const;
    
    // Performance metrics
    std::chrono::microseconds getAverageProcessingTime() const;
    size_t getMaxMemoryUsage() const;
    
    // Generate reports
    std::string generateJSONReport() const;
    std::string generateTextReport() const;
    std::string generateCSVReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Logger class with advanced features
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
    
    // Set global filter
    void setGlobalFilter(std::shared_ptr<LogFilter> filter);
    
    // Set global aggregator
    void setGlobalAggregator(std::shared_ptr<LogAggregator> aggregator);
    
    // Get global logging functions with enhanced context
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const std::map<std::string, std::string>& context = {});
    
    // Convenience methods with context
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", 
               const std::map<std::string, std::string>& context = {});
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", 
              const std::map<std::string, std::string>& context = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", 
                 const std::map<std::string, std::string>& context = {});
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", 
               const std::map<std::string, std::string>& context = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", 
                  const std::map<std::string, std::string>& context = {});
    
    // Stream-style logging with context
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, 
                  const std::string& function, const std::map<std::string, std::string>& context = {});
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Set context after stream creation
        void setContext(const std::string& key, const std::string& value);
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::map<std::string, std::string> context_;
    };
    
    #define NLM_LOG_STREAM(level) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__)
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get global filter
    static std::shared_ptr<LogFilter> getGlobalFilter();
    
    // Get global aggregator
    static std::shared_ptr<LogAggregator> getGlobalAggregator();
    
    // Flush all async loggers
    static void flushAll();
    
    // Generate comprehensive log report
    static std::string generateReport();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

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

} // namespace nlm
