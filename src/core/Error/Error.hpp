#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <type_traits>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Warning = 1,
    Error = 2,
    Critical = 3
};

// Error categories
enum class ErrorCategory {
    None = 0,
    FileError = 1,
    MemoryError = 2,
    ValidationError = 3,
    NetworkError = 4,
    NeuralError = 5,
    PlasticityError = 6,
    ConfigurationError = 7,
    SystemError = 8
};

// Base error class
class NLMError {
public:
    virtual ~NLMError() = default;
    
    virtual ErrorSeverity getSeverity() const = 0;
    virtual ErrorCategory getCategory() const = 0;
    virtual const std::string& getMessage() const = 0;
    virtual const std::string& getContext() const = 0;
    virtual std::chrono::system_clock::time_point getTimestamp() const = 0;
    virtual std::vector<std::string> getStackTrace() const = 0;
    virtual std::unique_ptr<NLMError> clone() const = 0;
    
    std::string toString() const {
        return "[" + severityToString(getSeverity()) + "] " +
               categoryToString(getCategory()) + ": " +
               getMessage() + (getContext().empty() ? "" : " (Context: " + getContext() + ")");
    }
    
protected:
    static std::string severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Warning: return "WARN";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    static std::string categoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::FileError: return "FILE";
            case ErrorCategory::MemoryError: return "MEMORY";
            case ErrorCategory::ValidationError: return "VALIDATION";
            case ErrorCategory::NetworkError: return "NETWORK";
            case ErrorCategory::NeuralError: return "NEURAL";
            case ErrorCategory::PlasticityError: return "PLASTICITY";
            case ErrorCategory::ConfigurationError: return "CONFIG";
            case ErrorCategory::SystemError: return "SYSTEM";
            default: return "UNKNOWN";
        }
    }
};

// Templated error class for type-safe error messages
template<typename T>
class TypedError : public NLMError {
public:
    TypedError(ErrorSeverity severity, ErrorCategory category, const std::string& message)
        : severity_(severity), category_(category), message_(message) {}
    
    TypedError(ErrorSeverity severity, ErrorCategory category, const std::string& message, const std::string& context)
        : severity_(severity), category_(category), message_(message), context_(context) {}
    
    ErrorSeverity getSeverity() const override { return severity_; }
    ErrorCategory getCategory() const override { return category_; }
    const std::string& getMessage() const override { return message_; }
    const std::string& getContext() const override { return context_; }
    std::chrono::system_clock::time_point getTimestamp() const override { return timestamp_; }
    std::vector<std::string> getStackTrace() const override { return stackTrace_; }
    std::unique_ptr<NLMError> clone() const override {
        return std::make_unique<TypedError<T>>(*this);
    }
    
    void addToStackTrace(const std::string& trace) {
        stackTrace_.push_back(trace);
    }
    
    T getValue() const { return value_; }
    
protected:
    ErrorSeverity severity_;
    ErrorCategory category_;
    std::string message_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_ = std::chrono::system_clock::now();
    std::vector<std::string> stackTrace_;
    T value_;
};

// File error class
class FileError : public TypedError<std::string> {
public:
    FileError(ErrorSeverity severity, const std::string& message, const std::string& filepath)
        : TypedError<std::string>(severity, ErrorCategory::FileError, message, filepath) {}
};

// Memory error class
class MemoryError : public TypedError<size_t> {
public:
    MemoryError(ErrorSeverity severity, const std::string& message, size_t bytesRequested)
        : TypedError<size_t>(severity, ErrorCategory::MemoryError, message), bytesRequested_(bytesRequested) {}
    
    size_t getBytesRequested() const { return bytesRequested_; }
    
private:
    size_t bytesRequested_;
};

// Validation error class
class ValidationError : public TypedError<std::string> {
public:
    ValidationError(ErrorSeverity severity, const std::string& message, const std::string& fieldName)
        : TypedError<std::string>(severity, ErrorCategory::ValidationError, message, fieldName) {}
};

// Neural operation error class
class NeuralError : public TypedError<float> {
public:
    NeuralError(ErrorSeverity severity, const std::string& message, float value)
        : TypedError<float>(severity, ErrorCategory::NeuralError, message), value_(value) {}
    
    float getValue() const { return value_; }
    
private:
    float value_;
};

// System error class
class SystemError : public TypedError<std::string> {
public:
    SystemError(ErrorSeverity severity, const std::string& message, const std::string& operation)
        : TypedError<std::string>(severity, ErrorCategory::SystemError, message, operation) {}
};

// Exception hierarchy for standard C++ exceptions
class NLMException : public std::exception {
public:
    explicit NLMException(std::unique_ptr<NLMError> error) : error_(std::move(error)) {}
    
    const char* what() const noexcept override {
        return error_->getMessage().c_str();
    }
    
    const NLMError& getError() const noexcept { return *error_; }
    NLMError& getError() noexcept { return *error_; }
    
    ErrorSeverity getSeverity() const { return error_->getSeverity(); }
    ErrorCategory getCategory() const { return error_->getCategory(); }
    
protected:
    std::unique_ptr<NLMError> error_;
};

class RecoverableException : public NLMException {
public:
    explicit RecoverableException(std::unique_ptr<NLMError> error, bool recoverable = true)
        : NLMException(std::move(error)), recoverable_(recoverable) {}
    
    bool isRecoverable() const noexcept { return recoverable_; }
    
private:
    bool recoverable_;
};

class NonRecoverableException : public NLMException {
public:
    explicit NonRecoverableException(std::unique_ptr<NLMError> error)
        : NLMException(std::move(error)) {}
};

// Type-safe error throwing macros
#define NLM_THROW_ERROR(error) do { throw NLMException(std::make_unique<error>(__VA_ARGS__)); } while(0)

#define NLM_THROW_RECOVERABLE_ERROR(error, recoverable, ...) do { throw RecoverableException(std::make_unique<error>(__VA_ARGS__), recoverable); } while(0)

#define NLM_THROW_NON_RECOVERABLE_ERROR(error, ...) do { throw NonRecoverableException(std::make_unique<error>(__VA_ARGS__)); } while(0)

// Safe wrapper for operations that might throw
#define NLM_SAFE_OPERATION(operation, error_type, context) \
    try { \
        operation; \
    } catch (const std::bad_alloc&) { \
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true, \
            "Memory allocation failed: " #operation, context); \
    } catch (const std::invalid_argument&) { \
        NLM_THROW_RECOVERABLE_ERROR(ValidationError, true, \
            "Invalid argument: " #operation, context); \
    } catch (const NLMException&) { \
        throw; \
    } catch (...) { \
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false, \
            "Unexpected error: " #operation, context); \
    }

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void report(const NLMError& error) = 0;
    virtual void report(const std::exception& error) = 0;
};

// Console error reporter
class ConsoleErrorReporter : public IErrorReporter {
public:
    void report(const NLMError& error) override {
        std::cerr << error.toString() << std::endl;
        for (const auto& trace : error.getStackTrace()) {
            std::cerr << "  -> " << trace << std::endl;
        }
    }
    
    void report(const std::exception& error) override {
        std::cerr << "Exception: " << error.what() << std::endl;
    }
};

// Global error reporter
class ErrorHandler {
public:
    static void setReporter(std::unique_ptr<IErrorReporter> reporter) {
        reporter_ = std::move(reporter);
    }
    
    static void report(const NLMError& error) {
        if (reporter_) {
            reporter_->report(error);
        }
    }
    
    static void report(const std::exception& error) {
        if (reporter_) {
            reporter_->report(error);
        }
    }
    
    static void setGlobalErrorReporter(std::unique_ptr<IErrorReporter> reporter) {
        reporter_ = std::move(reporter);
    }
    
    static IErrorReporter& getGlobalReporter() {
        if (!reporter_) {
            reporter_ = std::make_unique<ConsoleErrorReporter>();
        }
        return *reporter_;
    }
    
    static std::unique_ptr<IErrorReporter> reporter_;
};

inline std::unique_ptr<IErrorReporter> ErrorHandler::reporter_ = std::make_unique<ConsoleErrorReporter>();

// Utility functions for error handling
namespace ErrorUtils {
    template<typename T>
    void validate(const T& value, const std::string& condition, const std::string& message) {
        if (!condition.empty()) {
            // In a real implementation, condition would be evaluated
            // For now, just log validation errors
        }
    }
    
    std::string formatMemoryError(size_t bytes) {
        if (bytes >= 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        } else if (bytes >= 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else if (bytes >= 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else {
            return std::to_string(bytes) + " bytes";
        }
    }
    
    // RAII error handler for automatic resource cleanup
    template<typename T>
    class ErrorScope {
    public:
        ErrorScope(std::function<void()> cleanup) : cleanup_(std::move(cleanup)), active_(true) {}
        
        ~ErrorScope() {
            if (active_ && cleanup_) {
                cleanup_();
            }
        }
        
        void clear() {
            active_ = false;
        }
        
    private:
        std::function<void()> cleanup_;
        bool active_;
    };
}

} // namespace nlm
