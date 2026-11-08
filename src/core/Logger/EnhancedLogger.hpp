#include "Logger.hpp"
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <cstdint>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t correlationId; // For tracking related log messages
    std::string threadId;
    std::vector<std::string> context; // Key-value pairs for structured logging
};

// Enhanced console logger with additional features
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setOutputFile(const std::string& filepath);
    void setMaxFileSize(size_t maxSizeBytes);
    void setMaxFiles(int maxFiles);
    void rotateLogFile();
    void closeOutputFile();
    
    // Correlation ID management
    uint64_t generateCorrelationId();
    void setCorrelationId(uint64_t id);
    uint64_t getCurrentCorrelationId() const;
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message, 
                      const std::vector<std::string>& context);
    
    // Performance metrics
    void startPerformanceTimer(const std::string& operation);
    void stopPerformanceTimer(const std::string& operation);
    void logPerformanceMetric(const std::string& name, double value, 
                             const std::string& unit = "ms");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatStructured(const EnhancedLogEntry& entry) const;
    uint64_t getCurrentThreadId() const;
};

// Performance metrics collector
class PerformanceMetrics {
public:
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    void recordMetric(const std::string& name, double value, 
                     const std::string& unit = "ms", 
                     const std::string& category = "general");
    
    void recordMemoryUsage(size_t memoryBytes);
    void recordCpuUsage(double cpuPercent);
    
    std::vector<std::pair<std::string, double>> getMetrics() const;
    void clearMetrics();
    
    static PerformanceMetrics& getGlobalMetrics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logger with performance monitoring
class EnhancedLogger : public Logger {
public:
    EnhancedLogger();
    ~EnhancedLogger() override;
    
    // Performance monitoring
    void startOperation(const std::string& name);
    void endOperation(const std::string& name);
    void logPerformance(const std::string& name, double duration,
                       const std::string& unit = "ms");
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message,
                      const std::vector<std::string>& context);
    
    // Error tracking
    void trackError(const std::string& errorCode, const std::string& message,
                   const std::string& context = "");
    
    // Performance metrics access
    static PerformanceMetrics& getMetrics();
};

// Error handling classes
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

enum class ErrorCategory {
    None = 0,
    Configuration = 1,
    Validation = 2,
    Memory = 3,
    Plasticity = 4,
    Development = 5,
    Neuromodulation = 6,
    Perception = 7,
    Action = 8,
    Environment = 9,
    Experiment = 10,
    Performance = 11,
    System = 12
};

class NLMError {
public:
    NLMError() = default;
    NLMError(ErrorCategory category, ErrorSeverity severity,
            const std::string& message, const std::string& file = "",
            int line = 0, const std::string& function = "",
            const std::string& errorCode = "", uint64_t correlationId = 0);
    
    virtual ~NLMError() = default;
    
    virtual void log() const;
    virtual bool isRecoverable() const;
    virtual std::string getDescription() const;
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getErrorCode() const { return errorCode_; }
    uint64_t getCorrelationId() const { return correlationId_; }
    
protected:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::string errorCode_;
    uint64_t correlationId_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error types
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, 
                      const std::string& file = "", int line = 0,
                      const std::string& function = "",
                      const std::string& errorCode = "",
                      uint64_t correlationId = 0);
    
    void log() const override;
    bool isRecoverable() const override;
    std::string getDescription() const override;
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "",
                   const std::string& errorCode = "",
                   uint64_t correlationId = 0);
    
    void log() const override;
    bool isRecoverable() const override;
    std::string getDescription() const override;
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "",
               const std::string& errorCode = "",
               uint64_t correlationId = 0);
    
    void log() const override;
    bool isRecoverable() const override;
    std::string getDescription() const override;
};

// Error handling utilities
class ErrorHandler {
public:
    static void logError(const std::shared_ptr<NLMError> error);
    static void logError(ErrorCategory category, ErrorSeverity severity,
                        const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::string& errorCode = "");
    
    static bool isRecoverable(const std::shared_ptr<NLMError> error);
    static std::string getFormattedError(const std::shared_ptr<NLMError> error);
    
    static void setErrorReportingCallback(std::function<void(const std::shared_ptr<NLMError>)> callback);
    
    static uint64_t generateCorrelationId();
    static void setGlobalCorrelationId(uint64_t id);
    static uint64_t getGlobalCorrelationId();
};