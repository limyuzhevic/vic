#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <system_error>
#include <sstream>
#include <chrono>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error category enumeration
enum class ErrorCategory {
    None = 0,
    Memory,
    Parameter,
    Configuration,
    Network,
    IO,
    Resource,
    System,
    User,
    Internal
};

// Enhanced exception base class with metadata
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
             ErrorCategory category = ErrorCategory::Internal,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    virtual ~NLMError() noexcept = default;
    
    // Get error details
    const std::string& getMessage() const noexcept { return what(); }
    ErrorSeverity getSeverity() const noexcept { return severity_; }
    ErrorCategory getCategory() const noexcept { return category_; }
    const std::string& getFile() const noexcept { return file_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFunction() const noexcept { return function_; }
    const std::string& getTimestamp() const noexcept { return timestamp_; }
    
    // Stack trace (if available)
    const std::string& getStackTrace() const noexcept { return stackTrace_; }
    void setStackTrace(const std::string& trace) { stackTrace_ = trace; }
    
    // Error context
    const std::string& getContext() const noexcept { return context_; }
    void setContext(const std::string& ctx) { context_ = ctx; }
    
    // Cause (for chained exceptions)
    const std::string& getCause() const noexcept { return cause_; }
    void setCause(const std::string& c) { cause_ = c; }
    
    // Additional details
    template<typename T>
    void addDetail(const std::string& key, const T& value) {
        details_[key] = std::to_string(value);
    }
    
    const std::unordered_map<std::string, std::string>& getDetails() const noexcept { return details_; }
    
    // Formatted error report
    std::string getReport() const;
    
    // Exception type identifier
    virtual const char* what() const noexcept override { return message_.c_str(); }
    
protected:
    std::string message_;
    ErrorSeverity severity_;
    ErrorCategory category_;
    std::string file_;
    int line_;
    std::string function_;
    std::string timestamp_;
    std::string stackTrace_;
    std::string context_;
    std::string cause_;
    std::unordered_map<std::string, std::string> details_;
};

// Parameter validation result
class ValidationResult {
public:
    ValidationResult() : isValid_(true) {}
    ValidationResult(bool isValid, const std::string& message = "") 
        : isValid_(isValid), message_(message) {}
    
    bool isValid() const noexcept { return isValid_; }
    const std::string& getMessage() const noexcept { return message_; }
    
    // Chain validation results
    ValidationResult& addError(const std::string& error) {
        if (isValid_) isValid_ = false;
        if (!message_.empty()) message_ += "; ";
        message_ += error;
        return *this;
    }
    
    ValidationResult& addWarning(const std::string& warning) {
        if (!warnings_.empty()) warnings_ += "; ";
        warnings_ += warning;
        return *this;
    }
    
    const std::string& getWarnings() const noexcept { return warnings_; }
    
private:
    bool isValid_;
    std::string message_;
    std::string warnings_;
};

// Parameter validation utilities
class ParameterValidator {
public:
    // Basic type validation
    static ValidationResult validateRange(double value, double min, double max, const std::string& name = "");
    static ValidationResult validatePositive(double value, const std::string& name = "");
    static ValidationResult validateNonNegative(double value, const std::string& name = "");
    static ValidationResult validateIntegerRange(int64_t value, int64_t min, int64_t max, const std::string& name = "");
    static ValidationResult validateString(const std::string& value, size_t minLen = 0, size_t maxLen = SIZE_MAX, const std::string& name = "");
    static ValidationResult validatePointer(const void* ptr, const std::string& name = "");
    static ValidationResult validateNotNull(const std::nullptr_t ptr, const std::string& name = "");
    
    // Complex validations
    static ValidationResult validateConfigKey(const std::string& key);
    static ValidationResult validateNeuronId(uint64_t id);
    static ValidationResult validateSynapseId(uint64_t id);
    static ValidationResult validateRegionId(uint64_t id);
    
    // Helper for generic validation
    template<typename T, typename Validator>
    static ValidationResult validate(const T& value, Validator validator, const std::string& name = "") {
        return validator(value, name);
    }
    
    // Create validation chain
    static ValidationResult validateAll(std::initializer_list<ValidationResult> results);
};

// Error handling utilities
class ErrorHandler {
public:
    // Error reporting and logging
    static void reportError(const NLMError& error);
    static void reportWarning(const std::string& message, const std::string& context = "");
    static void reportInfo(const std::string& message, const std::string& context = "");
    
    // Error suppression and handling
    static bool shouldSuppress(const NLMError& error);
    static void handleError(const NLMError& error, std::function<void(const NLMError&)> handler = nullptr);
    
    // Error recovery
    static bool recoverFromError(const std::string& context, std::function<bool()> recoveryAction = nullptr);
    
    // Cleanup on error
    template<typename T>
    static T* safeAllocate(size_t count, const std::string& context = "") {
        try {
            T* ptr = new T[count];
            if (!ptr) {
                ErrorHandler::reportError(NLMError("Memory allocation failed", ErrorSeverity::Critical, ErrorCategory::Memory, context));
            }
            return ptr;
        } catch (const std::bad_alloc&) {
            ErrorHandler::reportError(NLMError("Memory allocation failed", ErrorSeverity::Critical, ErrorCategory::Memory, context));
            return nullptr;
        }
    }
    
    template<typename T>
    static void safeDeallocate(T* ptr, const std::string& context = "") {
        if (ptr) {
            try {
                delete[] ptr;
            } catch (...) {
                ErrorHandler::reportError(NLMError("Memory deallocation failed", ErrorSeverity::Error, ErrorCategory::Memory, context));
            }
        }
    }
    
    // RAII wrapper for resource management
    template<typename T>
    class SafeResource {
    public:
        SafeResource(T* ptr, const std::string& context = "") : ptr_(ptr), context_(context) {}
        ~SafeResource() {
            if (ptr_) {
                try {
                    delete ptr_;
                    ErrorHandler::reportInfo("Resource safely released", context_);
                } catch (...) {
                    ErrorHandler::reportError(NLMError("Resource cleanup failed", ErrorSeverity::Error, ErrorCategory::Resource, context_));
                }
            }
        }
        
        T* get() const { return ptr_; }
        T* release() { T* temp = ptr_; ptr_ = nullptr; return temp; }
        
        // No copying
        SafeResource(const SafeResource&) = delete;
        SafeResource& operator=(const SafeResource&) = delete;
        
    private:
        T* ptr_;
        std::string context_;
    };
    
    // Smart pointer with error handling
    template<typename T>
    static std::unique_ptr<T> makeSafePtr(T* ptr, const std::string& context = "") {
        if (!ptr) {
            ErrorHandler::reportError(NLMError("Null pointer provided to makeSafePtr", ErrorSeverity::Error, ErrorCategory::Parameter, context));
        }
        return std::unique_ptr<T>(ptr);
    }
    
    // Error context management
    class ErrorContext {
    public:
        ErrorContext(const std::string& operation, const std::string& component = "")
            : operation_(operation), component_(component) {
            startTime_ = std::chrono::steady_clock::now();
        }
        
        ~ErrorContext() {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
            ErrorHandler::reportInfo("Operation '" + operation_ + "' completed in " + std::to_string(duration.count()) + "ms", component_);
        }
        
        void addStep(const std::string& step) {
            steps_.push_back(step);
        }
        
        void addError(const std::string& error) {
            errors_.push_back(error);
        }
        
        const std::vector<std::string>& getSteps() const { return steps_; }
        const std::vector<std::string>& getErrors() const { return errors_; }
        
    private:
        std::string operation_;
        std::string component_;
        std::chrono::steady_clock::time_point startTime_;
        std::vector<std::string> steps_;
        std::vector<std::string> errors_;
    };
    
    // Exception wrapping
    template<typename Func, typename... Args>
    static auto wrapExceptions(Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const NLMError& e) {
            throw; // Re-throw NLMError as-is
        } catch (const std::exception& e) {
            throw NLMError(std::string("Wrapped exception: ") + e.what(), ErrorSeverity::Error, ErrorCategory::Internal);
        } catch (...) {
            throw NLMError("Unknown exception occurred", ErrorSeverity::Critical, ErrorCategory::Internal);
        }
    }
    
    // Template parameter validation
    template<typename T, typename U>
    static ValidationResult validateType(const T& value, U expectedType) {
        if (std::is_same<T, U>::value) {
            return ValidationResult(true);
        }
        return ValidationResult(false, "Type mismatch: expected " + typeid(U).name() + ", got " + typeid(T).name());
    }
};

// Error reporting helper functions
void logError(const NLMError& error);
void logWarning(const std::string& message, const std::string& context = "");
void logInfo(const std::string& message, const std::string& context = "");

// Common error instances for quick access
const NLMError& getMemoryAllocationError(const std::string& context = "");
const NLMError& getParameterError(const std::string& paramName, const std::string& expected, const std::string& actual);
const NLMError& getConfigurationError(const std::string& key, const std::string& reason);
const NLMError& getNullPointerError(const std::string& objectType, const std::string& context = "");

// Macros for common error handling patterns
#define VALIDATE_PARAM(param, validator) \
    do { \
        auto __validation_result = validator(param, #param); \
        if (!__validation_result.isValid()) { \
            throw NLMError(__validation_result.getMessage(), ErrorSeverity::Error, ErrorCategory::Parameter, __FILE__, __LINE__, __func__); \
        } \
        if (!__validation_result.getWarnings().empty()) { \
            ErrorHandler::reportWarning(__validation_result.getWarnings(), #param); \
        } \
    } while(0)

#define SAFE_CALL(func) \
    do { \
        try { func; } \
        catch (const NLMError& e) { throw; } \
        catch (const std::exception& e) { throw NLMError(std::string("Error in " #func ": ") + e.what(), ErrorSeverity::Error, ErrorCategory::Internal, __FILE__, __LINE__, __func__); } \
    } while(0)

#define LOG_THROW_ERROR(severity, category, message) \
    do { \
        NLMError error(message, severity, category, __FILE__, __LINE__, __func__); \
        ErrorHandler::reportError(error); \
        throw error; \
    } while(0)

// Forward declaration
class ErrorHandler;

// Global error handler instance
inline ErrorHandler& getGlobalErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

} // namespace nlm
