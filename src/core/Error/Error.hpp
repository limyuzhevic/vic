#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <stdexcept>
#include <functional>
#include <map>

namespace nlm {

// Forward declarations
class Error;
class ValidationEngine;
class ErrorReporter;

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Error types for classification
enum class ErrorType {
    SystemError,
    ConfigurationError,
    MemoryError,
    NeuralError,
    FileError,
    NetworkError,
    ValidationError,
    PlasticityError,
    DevelopmentError,
    PredictionError,
    NeuromodulationError,
    MemoryErrorCore,
    AttentionError,
    MotorError,
    CognitiveError,
    DevelopmentalError,
    Unknown
};

// Error category for context
enum class ErrorCategory {
    Runtime,
    CompileTime,
    Validation,
    System,
    User,
    Integration,
    Performance,
    Configuration,
    Resource
};

// Base Error class with exception safety
class Error : public std::exception {
public:
    Error(const std::string& message, ErrorType type = ErrorType::Unknown,
          ErrorSeverity severity = ErrorSeverity::Error,
          const std::string& file = "", int line = 0,
          const std::string& function = "",
          const std::string& category = "")
        : message_(message)
        , type_(type)
        , severity_(severity)
        , file_(file)
        , line_(line)
        , function_(function)
        , category_(category)
        , timestamp_(std::chrono::system_clock::now())
    {}

    virtual ~Error() noexcept = default;

    // Exception interface
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }

    // Error information
    const std::string& getMessage() const { return message_; }
    ErrorType getType() const { return type_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::string& getCategory() const { return category_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }

    // Error formatting
    virtual std::string format() const;
    virtual std::string formatStackTrace() const;

    // Error comparison
    virtual bool isRecoverable() const { return severity_ < ErrorSeverity::Critical; }
    virtual bool isSystemError() const { return category_ == "System"; }

protected:
    std::string message_;
    ErrorType type_;
    ErrorSeverity severity_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error subclasses
class SystemError : public Error {
public:
    SystemError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical,
                const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::SystemError, severity, file, line, function, "System") {}
};

class ConfigurationError : public Error {
public:
    ConfigurationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                       const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::ConfigurationError, severity, file, line, function, "Configuration") {}
};

class MemoryError : public Error {
public:
    MemoryError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical,
                const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::MemoryError, severity, file, line, function, "Resource") {}
};

class NeuralError : public Error {
public:
    NeuralError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::NeuralError, severity, file, line, function, "Runtime") {}
};

class FileError : public Error {
public:
    FileError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
              const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::FileError, severity, file, line, function, "System") {}
};

class NetworkError : public Error {
public:
    NetworkError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::NetworkError, severity, file, line, function, "Runtime") {}
};

class ValidationError : public Error {
public:
    ValidationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning,
                    const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::ValidationError, severity, file, line, function, "Validation") {}
};

class PlasticityError : public Error {
public:
    PlasticityError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                   const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::PlasticityError, severity, file, line, function, "Runtime") {}
};

class DevelopmentError : public Error {
public:
    DevelopmentError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning,
                     const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::DevelopmentError, severity, file, line, function, "Development") {}
};

class AttentionError : public Error {
public:
    AttentionError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                   const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::AttentionError, severity, file, line, function, "Runtime") {}
};

class MotorError : public Error {
public:
    MotorError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
               const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::MotorError, severity, file, line, function, "Runtime") {}
};

class CognitiveError : public Error {
public:
    CognitiveError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                   const std::string& file = "", int line = 0, const std::string& function = "")
        : Error(message, ErrorType::CognitiveError, severity, file, line, function, "Runtime") {}
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void report(const std::shared_ptr<Error> error) = 0;
    virtual void setErrorLevel(ErrorSeverity level) = 0;
    virtual ErrorSeverity getErrorLevel() const = 0;
    virtual void clear() = 0;
    virtual size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const = 0;
    virtual std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity = ErrorSeverity::Error) const = 0;
    virtual void addErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) = 0;
    virtual void removeErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) = 0;
};

// Console error reporter implementation
class ConsoleErrorReporter : public IErrorReporter {
public:
    ConsoleErrorReporter();
    explicit ConsoleErrorReporter(ErrorSeverity level);
    ~ConsoleErrorReporter() override;

    void report(const std::shared_ptr<Error> error) override;
    void setErrorLevel(ErrorSeverity level) override;
    ErrorSeverity getErrorLevel() const override;
    void clear() override;
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override;
    std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity = ErrorSeverity::Error) const override;
    void addErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) override;
    void removeErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    std::string severityToString(ErrorSeverity severity) const;
    std::string errorTypeToString(ErrorType type) const;
};

// File error reporter implementation
class FileErrorReporter : public IErrorReporter {
public:
    FileErrorReporter(const std::string& filepath, bool append = true);
    ~FileErrorReporter() override;

    void report(const std::shared_ptr<Error> error) override;
    void setErrorLevel(ErrorSeverity level) override;
    ErrorSeverity getErrorLevel() const override;
    void clear() override;
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override;
    std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity = ErrorSeverity::Error) const override;
    void addErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) override;
    void removeErrorHandler(std::function<void(const std::shared_ptr<Error>)> handler) override;

    void setMaxFileSize(size_t maxBytes);
    void setMaxFileCount(size_t maxFiles);
    void rotate();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Centralized error reporting system
class ErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter();

    // Disable copying
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;

    // Add error reporter sink
    void addReporter(std::shared_ptr<IErrorReporter> reporter);

    // Remove all reporters
    void clearReporters();

    // Set minimum error level
    void setErrorLevel(ErrorSeverity level);

    // Report an error
    void report(std::shared_ptr<Error> error);
    void report(const std::string& message, ErrorType type = ErrorType::Unknown,
                ErrorSeverity severity = ErrorSeverity::Error,
                const std::string& file = "", int line = 0,
                const std::string& function = "",
                const std::string& category = "");

    // Convenience methods
    void logDebug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logInfo(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logWarning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logError(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logCritical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");

    // Error management
    void clearAllErrors();
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const;
    std::vector<std::shared_ptr<Error>> getErrors(ErrorSeverity severity = ErrorSeverity::Error) const;

    // Global error reporter instance
    static ErrorReporter& getGlobal();
    static void setGlobal(std::shared_ptr<ErrorReporter> reporter);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Validation rule structure
struct ValidationRule {
    std::string name;
    std::string description;
    std::function<bool(const std::vector<std::string>&)> validator;
    ErrorType errorType;
    ErrorSeverity severity;
    bool required;

    ValidationRule(const std::string& n, const std::string& desc,
                   std::function<bool(const std::vector<std::string>&)> val,
                   ErrorType type = ErrorType::ValidationError,
                   ErrorSeverity sev = ErrorSeverity::Warning,
                   bool req = true)
        : name(n)
        , description(desc)
        , validator(val)
        , errorType(type)
        , severity(sev)
        , required(req) {}
};

// Validation result structure
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> messages;

    ValidationResult() : valid(true) {}
    ValidationResult(bool v) : valid(v) {}
};

// Validation engine for comprehensive validation
class ValidationEngine {
public:
    ValidationEngine();
    ~ValidationEngine();

    // Add validation rule
    void addRule(const ValidationRule& rule);
    void addRule(const std::string& name, const std::string& description,
                 std::function<bool(const std::vector<std::string>&)> validator,
                 ErrorType errorType = ErrorType::ValidationError,
                 ErrorSeverity severity = ErrorSeverity::Warning,
                 bool required = true);

    // Validate data
    ValidationResult validate(const std::vector<std::string>& data, const std::string& context = "") const;
    ValidationResult validateConfiguration(const std::string& key, const std::string& value) const;
    ValidationResult validateSystemState() const;

    // Get validation statistics
    size_t getRuleCount() const { return rules_.size(); }
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const;
    size_t getWarningCount(ErrorSeverity severity = ErrorSeverity::Warning) const;

    // Error recovery
    bool attemptRecovery(const std::shared_ptr<Error> error);

    // Configuration validation
    void configureValidation(const std::string& section, const std::map<std::string, std::string>& settings);

private:
    std::vector<ValidationRule> rules_;
    std::map<std::string, std::map<std::string, std::string>> config_;

    ValidationResult validateAgainstRules(const std::vector<std::string>& data, const std::string& context) const;
};

// Error handling utilities
namespace ErrorHandling {
    // Exception safety helpers
    template<typename T>
    class SafeResource {
    public:
        explicit SafeResource(std::function<T*()()> alloc) : allocator_(alloc), resource_(nullptr), valid_(false) {}
        ~SafeResource() { cleanup(); }

        T* get() const { return resource_; }
        bool isValid() const { return valid_; }

        void acquire() {
            cleanup();
            resource_ = allocator_();
            valid_ = (resource_ != nullptr);
        }

        void release() {
            cleanup();
            valid_ = false;
        }

        T* operator->() { return resource_; }
        T& operator*() { return *resource_; }

    private:
        void cleanup() {
            if (resource_ && valid_) {
                delete resource_;
                resource_ = nullptr;
                valid_ = false;
            }
        }

        std::function<T*()()> allocator_;
        T* resource_;
        bool valid_;
    };

    // Smart error handling
    template<typename T>
    T safeExecute(std::function<T()> operation,
                  std::function<void(const std::shared_ptr<Error>)> errorHandler = nullptr,
                  ErrorSeverity defaultSeverity = ErrorSeverity::Error) {
        try {
            return operation();
        } catch (const Error& e) {
            if (errorHandler) {
                errorHandler(std::make_shared<Error>(e));
            } else {
                ErrorReporter::getGlobal().report(std::make_shared<Error>(e));
            }
            throw;
        } catch (const std::exception& e) {
            std::string message = "Unexpected error: " + std::string(e.what());
            auto error = std::make_shared<Error>(message, ErrorType::SystemError, defaultSeverity);
            if (errorHandler) {
                errorHandler(error);
            } else {
                ErrorReporter::getGlobal().report(error);
            }
            throw;
        }
    }

    // Error localization
    std::string getCurrentLocation(const std::string& file = "", int line = 0, const std::string& function = "");
    std::string getBacktrace(size_t maxDepth = 20);

    // Memory allocation with error handling
    void* safeMalloc(size_t size);
    void safeFree(void* ptr);

    // Resource validation
    bool validateResourceAvailability(const std::string& resource, float minAvailable = 0.0f);
    bool validateMemoryLimit(size_t maxBytes);
    bool validateFileAccess(const std::string& path, bool writeAccess = false);
}

// Error reporting macros
#define NLM_THROW_ERROR(type, severity, message) \
    do { \
        auto error = std::make_shared<type>(message, severity, __FILE__, __LINE__, __func__); \
        ErrorReporter::getGlobal().report(error); \
        throw error; \
    } while(0)

#define NLM_LOG_ERROR_SEVERITY(severity, message) \
    do { \
        ErrorReporter::getGlobal().report( \
            std::make_shared<Error>(message, ErrorType::Unknown, severity, __FILE__, __LINE__, __func__) \
        ); \
    } while(0)

#define NLM_VALIDATE_CONDITION(condition, message, errorType, severity) \
    do { \
        if (!(condition)) { \
            NLM_THROW_ERROR(errorType, severity, message); \
        } \
    } while(0)

#define NLM_LOG_ERROR(message) NLM_LOG_ERROR_SEVERITY(ErrorSeverity::Error, message)
#define NLM_LOG_WARNING(message) NLM_LOG_ERROR_SEVERITY(ErrorSeverity::Warning, message)
#define NLM_LOG_INFO(message) NLM_LOG_ERROR_SEVERITY(ErrorSeverity::Info, message)
#define NLM_LOG_DEBUG(message) NLM_LOG_ERROR_SEVERITY(ErrorSeverity::Debug, message)
#define NLM_LOG_CRITICAL(message) NLM_LOG_ERROR_SEVERITY(ErrorSeverity::Critical, message)

} // namespace nlm
