// NLM Enhanced Error Handling and Logging System
//
// Comprehensive error handling and logging system for NLM
// Provides enhanced error reporting, recovery mechanisms, and system monitoring
// Improves reliability, debugging, and user experience

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <set>
#include <chrono>
#include <ctime>
#include <functional>
#include <stdexcept>

namespace nlm {

// Error types
enum class ErrorType {
    None,
    ConfigurationError,
    FileError,
    MemoryError,
    NetworkError,
    PerformanceError,
    ValidationError,
    PermissionError,
    TimeoutError,
    ResourceError,
    SystemError,
    UserError,
    CustomError
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error category
enum class ErrorCategory {
    System,
    Application,
    Network,
    FileSystem,
    Configuration,
    Memory,
    Performance,
    UserInput,
    Security,
    ExternalService
};

// Enhanced error information
struct EnhancedError {
    ErrorType type;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string message;
    std::string description;
    std::string code;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    std::vector<std::string> stackTrace;
    std::vector<std::string> suggestions;
    std::shared_ptr<EnhancedError> innerError;
    bool recoveryAttempted;
    bool recoverySuccessful;
    std::string recoveryAction;
    
    EnhancedError(ErrorType t, ErrorSeverity s, const std::string& m, 
                  const std::string& desc = "", const std::string& srcFile = "",
                  int srcLine = 0, const std::string& funcName = "")
        : type(t), severity(s), category(ErrorCategory::Application), message(m),
          description(desc), code(""), sourceFile(srcFile), sourceLine(srcLine),
          functionName(funcName), recoveryAttempted(false), recoverySuccessful(false) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    // Error handling
    virtual void handleError(const EnhancedError& error) = 0;
    virtual void handleWarning(const EnhancedError& error) = 0;
    virtual void handleInfo(const EnhancedError& error) = 0;
    virtual void handleDebug(const EnhancedError& error) = 0;
    
    // Error reporting
    virtual void reportError(const EnhancedError& error) = 0;
    virtual void reportWarning(const EnhancedError& error) = 0;
    virtual void reportInfo(const EnhancedError& error) = 0;
    virtual void reportDebug(const EnhancedError& error) = 0;
    
    // Error recovery
    virtual bool canRecover(const EnhancedError& error) const = 0;
    virtual bool recover(const EnhancedError& error) = 0;
    virtual std::vector<std::string> getRecoveryActions(const EnhancedError& error) const = 0;
    
    // Error filtering
    virtual bool shouldReport(const EnhancedError& error) const = 0;
    virtual bool shouldHandle(const EnhancedError& error) const = 0;
    
    // Error configuration
    virtual void setErrorLevel(ErrorSeverity level) = 0;
    virtual void setErrorCategories(const std::set<ErrorCategory>& categories) = 0;
    virtual void setFilterPattern(const std::string& pattern) = 0;
    
    // Statistics
    virtual size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const = 0;
    virtual size_t getWarningCount() const = 0;
    virtual size_t getInfoCount() const = 0;
    virtual double getErrorRate() const = 0;
    virtual std::map<ErrorSeverity, size_t> getErrorStatistics() const = 0;
};

// Simple logging system
class Logger {
public:
    Logger();
    ~Logger();
    
    // Log levels
    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    // Logging methods
    void log(LogLevel level, const std::string& message, const std::string& category = "general");
    void debug(const std::string& message, const std::string& category = "debug");
    void info(const std::string& message, const std::string& category = "info");
    void warning(const std::string& message, const std::string& category = "warning");
    void error(const std::string& message, const std::string& category = "error");
    void critical(const std::string& message, const std::string& category = "critical");
    
    // Log filtering and configuration
    void setLogLevel(LogLevel level);
    void setLogCategories(const std::set<std::string>& categories);
    void setLogFile(const std::string& filename);
    
    // Log management
    void clearLog();
    size_t getLogCount(LogLevel level = LogLevel::Debug) const;
    std::string getLogSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comprehensive error and logging service
class ErrorHandlingService {
public:
    static ErrorHandlingService& getInstance();
    static void initialize();
    static void shutdown();
    
    // Error handling
    static void registerErrorHandler(std::unique_ptr<ErrorHandler> handler);
    static std::unique_ptr<ErrorHandler>& getErrorHandler();
    static void handleError(const EnhancedError& error);
    static void handleWarning(const EnhancedError& error);
    static void handleInfo(const EnhancedError& error);
    static void handleDebug(const EnhancedError& error);
    
    // Logging
    static void setLogger(std::unique_ptr<Logger> logger);
    static std::unique_ptr<Logger>& getLogger();
    static void log(LogLevel level, const std::string& message, const std::string& category = "general");
    static void debug(const std::string& message, const std::string& category = "debug");
    static void info(const std::string& message, const std::string& category = "info");
    static void warning(const std::string& message, const std::string& category = "warning");
    static void error(const std::string& message, const std::string& category = "error");
    static void critical(const std::string& message, const std::string& category = "critical");
    
    // Statistics
    static std::map<std::string, size_t> getSystemStatistics();
    static void resetStatistics();
    
private:
    ErrorHandlingService();
    ~ErrorHandlingService();
    ErrorHandlingService(const ErrorHandlingService&) = delete;
    ErrorHandlingService& operator=(const ErrorHandlingService&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions
namespace ErrorUtils {
    static std::string getErrorTypeString(ErrorType type);
    static std::string getErrorSeverityString(ErrorSeverity severity);
    static std::string getErrorCategoryString(ErrorCategory category);
    static ErrorSeverity errorToSeverity(ErrorType type);
    static std::string formatError(const EnhancedError& error);
    static std::string getCurrentFunctionName();
    static std::string getCurrentFileName();
    static int getCurrentLineNumber();
    static std::string getStackTrace();
    
    // Error creation utilities
    static EnhancedError createError(ErrorType type, ErrorSeverity severity, const std::string& message,
                                    const std::string& description = "", const std::string& sourceFile = "",
                                    int sourceLine = 0, const std::string& functionName = "");
    static EnhancedError createConfigurationError(const std::string& message, const std::string& description = "");
    static EnhancedError createFileError(const std::string& message, const std::string& filePath = "");
    static EnhancedError createMemoryError(const std::string& message);
    static EnhancedError createNetworkError(const std::string& message);
    static EnhancedError createPerformanceError(const std::string& message);
    static EnhancedError createValidationError(const std::string& message, const std::string& field = "");
    static EnhancedError createSystemError(const std::string& message);
    static EnhancedError createUserError(const std::string& message);
    
    // Error analysis
    static std::string analyzeError(const EnhancedError& error);
    static std::vector<std::string> suggestRecovery(const EnhancedError& error);
    static bool isRecoverable(const EnhancedError& error);
    static std::string getErrorContext(const EnhancedError& error);
    static std::chrono::duration<double> getErrorAge(const EnhancedError& error);
    
    // Error filtering
    static bool shouldReportError(const EnhancedError& error, ErrorSeverity minSeverity = ErrorSeverity::Warning);
    
    // Error logging
    static void logError(const EnhancedError& error);
    static void logWarning(const EnhancedError& error);
    static void logInfo(const EnhancedError& error);
    static void logDebug(const EnhancedError& error);
    
    // System utilities
    static std::string getSystemInfo();
}}

} // namespace nlm