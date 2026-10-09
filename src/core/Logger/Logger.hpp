#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace nlm {

// Enhanced Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    NeuralDebug = 5,    // Neural system detailed logging
    Performance = 6,    // Performance metrics
    Memory = 7,        // Memory usage
    Plasticity = 8,     // Plasticity metrics
    Learning = 9,      // Learning progress
    System = 10        // System statistics
};

// Enhanced Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional metadata for enhanced logging
    std::string threadId;
    std::string processId;
    std::unordered_map<std::string, std::string> metadata;
    double executionTime; // microseconds
    
    LogEntry() : level(LogLevel::Info), line(0), executionTime(0.0) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& thread = "", 
             const std::string& process = "")
        : level(lvl), message(msg), file(file), line(line), function(function),
          threadId(thread), processId(process), timestamp(std::chrono::system_clock::now()),
          executionTime(0.0) {}
};

// Enhanced Logger interface with statistics
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Statistics methods
    virtual size_t getMessageCount(LogLevel level) const = 0;
    virtual double getAverageExecutionTime(LogLevel level) const = 0;
    virtual std::string getStatistics() const = 0;
    virtual void resetStatistics() = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public IEnhancedLogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // File output with rotation
    void setOutputFile(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024, // 10MB
                      int maxFiles = 5);
    void closeOutputFile();
    
    // Statistics methods
    size_t getMessageCount(LogLevel level) const override;
    double getAverageExecutionTime(LogLevel level) const override;
    std::string getStatistics() const override;
    void resetStatistics() override;
    
    // Neural-specific logging methods
    void logNeuralActivity(const std::string& message, size_t neuronId = 0, 
                          float firingRate = 0.0f, float membranePotential = 0.0f);
    void logPlasticityChange(const std::string& message, float weightChange,
                           float learningRate, size_t synapseId = 0);
    void logLearningProgress(const std::string& message, float progress, 
                            size_t totalSteps, double duration);
    void logMemoryUsage(const std::string& message, size_t memoryUsage, 
                       size_t peakMemory, double usageRatio);
    void logPerformanceMetrics(const std::string& message, double cpuTime,
                              double wallClockTime, size_t stepsProcessed);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    // Helper methods for enhanced logging
    void logWithTimestamp(const LogEntry& entry);
    void logToFile(const std::string& message);
};

// Enhanced Main Logger class with comprehensive statistics
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced logging methods
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& thread = "", 
             const std::string& process = "", const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Convenience methods for different log levels
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& thread = "", 
               const std::string& process = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", const std::string& thread = "", 
              const std::string& process = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& thread = "", 
                 const std::string& process = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& thread = "", 
               const std::string& process = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& thread = "", 
                  const std::string& process = "", const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Neural system logging methods
    void logNeuralActivity(const std::string& message, size_t neuronId = 0, 
                          float firingRate = 0.0f, float membranePotential = 0.0f,
                          const std::string& thread = "", const std::string& process = "");
    void logPlasticityChange(const std::string& message, float weightChange,
                           float learningRate, size_t synapseId = 0,
                           const std::string& thread = "", const std::string& process = "");
    void logLearningProgress(const std::string& message, float progress, 
                            size_t totalSteps, double duration,
                            const std::string& thread = "", const std::string& process = "");
    void logMemoryUsage(const std::string& message, size_t memoryUsage, 
                       size_t peakMemory, double usageRatio,
                       const std::string& thread = "", const std::string& process = "");
    void logPerformanceMetrics(const std::string& message, double cpuTime,
                              double wallClockTime, size_t stepsProcessed,
                              const std::string& thread = "", const std::string& process = "");
    
    // Statistics and monitoring methods
    std::string getGlobalStatistics() const;
    std::string getSystemStatistics() const;
    std::string getNeuralStatistics() const;
    std::string getPerformanceStatistics() const;
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration
    static void configureLogging(LogLevel level, bool useColors = true,
                                 const std::string& logFile = "", 
                                 size_t maxFileSize = 10 * 1024 * 1024);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced macros for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced macros with metadata
#define NLM_LOG_DEBUG_WITH_METADATA(msg, metadata) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "", "", metadata)
#define NLM_LOG_INFO_WITH_METADATA(msg, metadata) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "", "", metadata)
#define NLM_LOG_WARNING_WITH_METADATA(msg, metadata) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, "", "", metadata)
#define NLM_LOG_ERROR_WITH_METADATA(msg, metadata) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, "", "", metadata)

// Neural system macros
#define NLM_LOG_NEURAL(msg, neuronId, firingRate, membranePotential) nlm::Logger::getGlobal().logNeuralActivity(msg, neuronId, firingRate, membranePotential)
#define NLM_LOG_PLASTICITY(msg, weightChange, learningRate, synapseId) nlm::Logger::getGlobal().logPlasticityChange(msg, weightChange, learningRate, synapseId)
#define NLM_LOG_LEARNING(msg, progress, totalSteps, duration) nlm::Logger::getGlobal().logLearningProgress(msg, progress, totalSteps, duration)
#define NLM_LOG_MEMORY(msg, usage, peak, ratio) nlm::Logger::getGlobal().logMemoryUsage(msg, usage, peak, ratio)
#define NLM_LOG_PERFORMANCE(msg, cpuTime, wallClockTime, steps) nlm::Logger::getGlobal().logPerformanceMetrics(msg, cpuTime, wallClockTime, steps)

// Stream-style logging helper with metadata
class EnhancedLogStream {
public:
    EnhancedLogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                      const std::string& thread = "", const std::string& process = "",
                      const std::unordered_map<std::string, std::string>& metadata = {});
    ~EnhancedLogStream();
    
    template<typename T>
    EnhancedLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Helper methods for adding metadata
    EnhancedLogStream& withMetadata(const std::string& key, const std::string& value);
    EnhancedLogStream& withThread(const std::string& threadId);
    EnhancedLogStream& withProcess(const std::string& processId);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string thread_;
    std::string process_;
    std::unordered_map<std::string, std::string> metadata_;
};

#define NLM_LOG_STREAM(level, ...) nlm::EnhancedLogStream(level, __FILE__, __LINE__, __func__)

// Global logging helper functions
namespace LogHelpers {
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    std::string getThreadId();
    std::string getProcessId();
    std::string sanitizeMessage(const std::string& message);
    
    // Performance monitoring helpers
    class PerformanceTimer {
    public:
        PerformanceTimer();
        ~PerformanceTimer();
        double getElapsedTime() const;
        void reset();
    
    private:
        std::chrono::high_resolution_clock::time_point startTime_;
    };
}

} // namespace nlm
