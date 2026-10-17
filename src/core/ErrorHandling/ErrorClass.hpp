#pragma once

#include <string>
#include <chrono>
#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <format>

namespace nlm {

// Error categories
enum class ErrorCategory {
    Neural = 0,
    Configuration = 1,
    I_O = 2,
    System = 3,
    Memory = 4,
    Network = 5,
    User = 6
};

// Error severity levels
enum class ErrorSeverity {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6
};

// Error codes namespace
namespace ErrorCodes {
    // Neural errors
    static const int NEURAL_NETWORK_INIT_FAILED = 1000;
    static const int NEURAL_INVALID_INPUT = 1001;
    static const int NEURAL_INVALID_OUTPUT = 1002;
    static const int NEURAL_CONVERGENCE_FAILED = 1003;
    
    // Configuration errors
    static const int CONFIG_FILE_NOT_FOUND = 2000;
    static const int CONFIG_INVALID_FORMAT = 2001;
    static const int CONFIG_VALUE_INVALID = 2002;
    static const int CONFIG_SCHEMA_MISMATCH = 2003;
    
    // I/O errors
    static const int IO_FILE_OPEN_FAILED = 3000;
    static const int IO_FILE_READ_FAILED = 3001;
    static const int IO_FILE_WRITE_FAILED = 3002;
    static const int IO_PATH_NOT_FOUND = 3003;
    
    // System errors
    static const int SYSTEM_MEMORY_EXHAUSTED = 4000;
    static const int SYSTEM_CPU_LIMIT_EXCEEDED = 4001;
    static const int SYSTEM_TIMEOUT = 4002;
    static const int SYSTEM_PERMISSION_DENIED = 4003;
    
    // Memory errors
    static const int MEMORY_ALLOCATION_FAILED = 5000;
    static const int MEMORY_ACCESS_VIOLATION = 5001;
    static const int MEMORY_OVERFLOW = 5002;
    static const int MEMORY_UNDERFLOW = 5003;
    
    // Network errors
    static const int NETWORK_CONNECTION_FAILED = 6000;
    static const int NETWORK_TIMEOUT = 6001;
    static const int NETWORK_PROTOCOL_ERROR = 6002;
    static const int NETWORK_PROTOCOL_UNSUPPORTED = 6003;
    
    // User errors
    static const int USER_INPUT_INVALID = 7000;
    static const int USER_CANCELLED = 7001;
    static const int USER_ABORTED = 7002;
};

// Error context information
struct ErrorContext {
    std::string module;
    std::string function;
    std::string file;
    int line = 0;
    std::string threadName;
    std::map<std::string, std::string> attributes;
    
    ErrorContext() : module(), function(), file(), line(0), threadName() {}
    
    ErrorContext(const std::string& mod, const std::string& func,
                 const std::string& file, int line)
        : module(mod), function(func), file(file), line(line), threadName() {}
    
    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }
    
    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        return it != attributes.end() ? it->second : "";
    }
};

// Error information
struct ErrorInfo {
    int code;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string description;
    std::string details;
    ErrorContext context;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    ErrorInfo() : code(0), category(ErrorCategory::System), 
                  severity(ErrorSeverity::Error), line(0), timestamp() {}
    
    ErrorInfo(int code, ErrorCategory category, ErrorSeverity severity,
              const std::string& message, const std::string& description = "",
              const std::string& details = "", const ErrorContext& ctx = ErrorContext())
        : code(code), category(category), severity(severity), 
          message(message), description(description), details(details),
          context(ctx), timestamp(std::chrono::system_clock::now()), 
          line(ctx.line) {}
    
    std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        return std::format("{:%Y-%m-%d %H:%M:%S}", std::tm(*std::localtime(&time)));
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity) << "] "
            << "[" << static_cast<int>(category) << "] "
            << "[" << code << "] "
            << "[" << getTimestampString() << "] "
            << message;
        if (!details.empty()) {
            oss << " - " << details;
        }
        return oss.str();
    }
};

// Error classification helper
class ErrorClassifier {
public:
    static ErrorCategory classifyError(int code);
    static ErrorSeverity classifySeverity(int code, const std::string& message);
    static std::string getErrorCategoryString(ErrorCategory category);
    static std::string getErrorSeverityString(ErrorSeverity severity);
    static std::string getErrorCodeString(int code);
    
    // Initialize error mappings
    static void initializeErrorMaps();
    
private:
    static std::map<int, ErrorCategory> errorCodeToCategoryMap;
    static std::map<int, ErrorSeverity> errorCodeToSeverityMap;
    static std::map<int, std::string> errorCodeToDescriptionMap;
    static std::map<int, std::string> errorCodeToMessageMap;
    
    static void loadNeuralErrorMappings();
    static void loadConfigErrorMappings();
    static void loadIOErrorMappings();
    static void loadSystemErrorMappings();
    static void loadMemoryErrorMappings();
    static void loadNetworkErrorMappings();
    static void loadUserErrorMappings();
};

// Exception base class for NLM
class NLMException : public std::exception {
public:
    NLMException(const ErrorInfo& errorInfo);
    virtual ~NLMException() noexcept = default;
    
    virtual const char* what() const noexcept override;
    
    // Get error information
    const ErrorInfo& getErrorInfo() const { return errorInfo_; }
    
    // Get error message
    std::string getMessage() const { return errorInfo_.message; }
    
    // Get error code
    int getCode() const { return errorInfo_.code; }
    
    // Get error category
    ErrorCategory getCategory() const { return errorInfo_.category; }
    
    // Get error severity
    ErrorSeverity getSeverity() const { return errorInfo_.severity; }
    
    // Check if error is recoverable
    virtual bool isRecoverable() const { return false; }
    
    // Get recovery suggestion
    virtual std::string getRecoverySuggestion() const { return "No recovery suggestion available"; }
    
protected:
    ErrorInfo errorInfo_;
    mutable std::string whatMessage_;
};

// Specific exception classes for different error types
class NeuralException : public NLMException {
public:
    explicit NeuralException(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return true; }
    virtual std::string getRecoverySuggestion() const override {
        return "Check neural network configuration or input data";
    }
};

class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return true; }
    virtual std::string getRecoverySuggestion() const override {
        return "Check configuration file or environment settings";
    }
};

class IOError : public NLMException {
public:
    explicit IOError(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return true; }
    virtual std::string getRecoverySuggestion() const override {
        return "Check file permissions or path validity";
    }
};

class SystemException : public NLMException {
public:
    explicit SystemException(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return false; }
    virtual std::string getRecoverySuggestion() const override {
        return "Check system resources or restart the application";
    }
};

class MemoryException : public NLMException {
public:
    explicit MemoryException(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return true; }
    virtual std::string getRecoverySuggestion() const override {
        return "Reduce memory usage or increase system resources";
    }
};

class NetworkException : public NLMException {
public:
    explicit NetworkException(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return true; }
    virtual std::string getRecoverySuggestion() const override {
        return "Check network connectivity or retry operation";
    }
};

class UserException : public NLMException {
public:
    explicit UserException(const ErrorInfo& errorInfo) : NLMException(errorInfo) {}
    virtual bool isRecoverable() const override { return true; }
    virtual std::string getRecoverySuggestion() const override {
        return "Check user input or follow application instructions";
    }
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void reportError(const ErrorInfo& error) = 0;
    virtual void reportWarning(const std::string& message, const std::string& details = "") = 0;
    virtual void reportInfo(const std::string& message, const std::string& details = "") = 0;
    virtual void reportDebug(const std::string& message, const std::string& details = "") = 0;
};

// Error class for comprehensive error management
class ErrorClass {
public:
    ErrorClass();
    ~ErrorClass();
    
    // Disable copying, enable moving
    ErrorClass(const ErrorClass&) = delete;
    ErrorClass& operator=(const ErrorClass&) = delete;
    ErrorClass(ErrorClass&&) noexcept;
    ErrorClass& operator=(ErrorClass&&) noexcept;
    
    // Get error instance
    static ErrorClass& getInstance();
    
    // Report an error
    void reportError(const ErrorInfo& error);
    
    // Report an exception
    void reportException(const std::exception& ex, const ErrorContext& context = ErrorContext());
    
    // Get recent errors
    std::vector<ErrorInfo> getRecentErrors(size_t maxCount = 100) const;
    
    // Get errors by category
    std::vector<ErrorInfo> getErrorsByCategory(ErrorCategory category, size_t maxCount = 100) const;
    
    // Get errors by severity
    std::vector<ErrorInfo> getErrorsBySeverity(ErrorSeverity severity, size_t maxCount = 100) const;
    
    // Clear errors
    void clearErrors();
    
    // Get error statistics
    struct ErrorStats {
        size_t totalErrors = 0;
        std::map<ErrorCategory, size_t> errorsByCategory;
        std::map<ErrorSeverity, size_t> errorsBySeverity;
        std::map<int, size_t> errorsByCode;
        std::chrono::system_clock::time_point firstErrorTime;
        std::chrono::system_clock::time_point lastErrorTime;
        size_t criticalErrors = 0;
        size_t fatalErrors = 0;
    };
    
    ErrorStats getErrorStats() const;
    
    // Add error reporter
    void addErrorReporter(std::shared_ptr<IErrorReporter> reporter);
    
    // Set minimum error severity to report
    void setMinimumSeverity(ErrorSeverity severity);
    
    // Set maximum errors to store
    void setMaxStoredErrors(size_t maxCount);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void addError(const ErrorInfo& error);
    bool shouldReportError(const ErrorInfo& error) const;
};

} // namespace nlm