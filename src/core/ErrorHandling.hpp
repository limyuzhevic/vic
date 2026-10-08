#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <condition_variable>

namespace nlm {

// Error categories for standardized exception handling
enum class ErrorCategory {
    None = 0,
    Memory = 1,           // Memory allocation and management errors
    Validation = 2,       // Parameter validation errors
    Configuration = 3,    // Configuration and setup errors
    Plasticity = 4,       // Plasticity rule computation errors
    Neural = 5,           // Neural computation and simulation errors
    System = 6,           // System-level integration errors
    Resource = 7,         // Resource management errors
    Thread = 8,           // Thread safety and concurrency errors
    IO = 9,               // Input/output errors
    Network = 10,         // Network communication errors
    Algorithm = 11,       // Algorithm computation errors
    Integration = 12,     // Integration and compatibility errors
    Performance = 13     // Performance and optimization errors
};

// Exception severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Resource states for resource management
enum class ResourceState {
    None = 0,
    Initializing = 1,
    Available = 2,
    InUse = 3,
    Released = 4,
    Corrupted = 5
};

// Standardized base exception class
class NLMException;

// Exception handler interface
class IExceptionHandler;

// Exception manager
class ExceptionManager;

class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                 ErrorCategory category = ErrorCategory::None,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& operation = "");
    
    virtual ~NLMException() = default;
    
    // Get error category
    ErrorCategory getCategory() const { return category_; }
    
    // Get error severity
    ErrorSeverity getSeverity() const { return severity_; }
    
    // Get operation that caused the error
    const std::string& getOperation() const { return operation_; }
    
    // Get timestamp of error occurrence
    const std::string& getTimestamp() const { return timestamp_; }
    
    // Get unique error ID
    const std::string& getErrorId() const { return error_id_; }
    
    // Get enhanced error message with context
    const std::string& getEnhancedMessage() const { return enhanced_message_; }
    
    // Check if error is recoverable
    virtual bool isRecoverable() const;
    
    // Convert error to string for logging
    virtual std::string toString() const;
    
protected:
    static std::string getCurrentTimestamp();
    static std::string generateErrorId();
    static std::string errorCategoryToString(ErrorCategory category);
    static std::string errorSeverityToString(ErrorSeverity severity);
    
private:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string operation_;
    std::string timestamp_;
    std::string error_id_;
    std::string enhanced_message_;
};

// Memory allocation exception
class MemoryAllocationException : public NLMException {
public:
    MemoryAllocationException(size_t requested_size, size_t available_size, const std::string& operation);
    
    bool isRecoverable() const override;
    
    // Get requested size
    size_t getRequestedSize() const { return requested_size_; }
    
    // Get available size
    size_t getAvailableSize() const { return available_size_; }
    
protected:
    size_t requested_size_;
    size_t available_size_;
};

// Validation exception
class ValidationException : public NLMException {
public:
    ValidationException(const std::string& parameter_name, const std::string& expected, 
                        const std::string& actual, const std::string& operation);
    
    bool isRecoverable() const override;
    
    // Get parameter name
    const std::string& getParameterName() const { return parameter_name_; }
    
    // Get expected value
    const std::string& getExpected() const { return expected_; }
    
    // Get actual value
    const std::string& getActual() const { return actual_; }
    
protected:
    std::string parameter_name_;
    std::string expected_;
    std::string actual_;
};

// Configuration exception
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& config_key, const std::string& config_value,
                           const std::string& expected_format, const std::string& operation);
    
    bool isRecoverable() const override;
    
    // Get configuration key
    const std::string& getConfigKey() const { return config_key_; }
    
    // Get configuration value
    const std::string& getConfigValue() const { return config_value_; }
    
    // Get expected format
    const std::string& getExpectedFormat() const { return expected_format_; }
    
protected:
    std::string config_key_;
    std::string config_value_;
    std::string expected_format_;
};

// Neural computation exception
class NeuralComputationException : public NLMException {
public:
    NeuralComputationException(const std::string& neural_type, const std::string& operation,
                              const std::string& details, ErrorSeverity severity = ErrorSeverity::Error);
    
    bool isRecoverable() const override;
    
    // Get neural type
    const std::string& getNeuralType() const { return neural_type_; }
    
    // Get operation
    const std::string& getOperation() const { return operation_; }
    
    // Get details
    const std::string& getDetails() const { return details_; }
    
protected:
    std::string neural_type_;
    std::string operation_;
    std::string details_;
};

// Thread safety exception
class ThreadSafetyException : public NLMException {
public:
    ThreadSafetyException(const std::string& resource, const std::string& operation,
                         const std::string& thread_info, ErrorSeverity severity = ErrorSeverity::Critical);
    
    bool isRecoverable() const override;
    
    // Get resource name
    const std::string& getResource() const { return resource_; }
    
    // Get thread information
    const std::string& getThreadInfo() const { return thread_info_; }
    
protected:
    std::string resource_;
    std::string thread_info_;
};

// Resource management exception
class ResourceManagementException : public NLMException {
public:
    ResourceManagementException(const std::string& resource_type, const std::string& operation,
                               ResourceState before_state, ResourceState after_state);
    
    bool isRecoverable() const override;
    
    // Get resource type
    const std::string& getResourceType() const { return resource_type_; }
    
    // Get before state
    ResourceState getBeforeState() const { return before_state_; }
    
    // Get after state
    ResourceState getAfterState() const { return after_state_; }
    
protected:
    std::string resource_type_;
    ResourceState before_state_;
    ResourceState after_state_;
};

// Exception handler interface
class IExceptionHandler {
public:
    virtual ~IExceptionHandler() = default;
    
    // Handle exception
    virtual void handleException(const NLMException& exception) = 0;
    
    // Check if handler can handle exception
    virtual bool canHandle(const NLMException& exception) const = 0;
    
    // Get handler name
    virtual const std::string& getName() const = 0;
};

// Default exception handler
class DefaultExceptionHandler : public IExceptionHandler {
public:
    void handleException(const NLMException& exception) override;
    
    bool canHandle(const NLMException& exception) const override;
    
    const std::string& getName() const override;
    
protected:
    virtual void logException(const NLMException& exception);
    virtual void terminateOnCriticalError(const NLMException& exception);
};

// Exception manager
class ExceptionManager {
public:
    static ExceptionManager& getInstance();
    
    // Register exception handler
    void registerHandler(std::unique_ptr<IExceptionHandler> handler);
    
    // Handle exception
    void handleException(const NLMException& exception);
    
    // Remove exception handler
    void removeHandler(const std::string& name);
    
    // Get all registered handler names
    std::vector<std::string> getHandlerNames() const;
    
    // Clear all handlers
    void clearHandlers();
    
protected:
    ExceptionManager() = default;
    ~ExceptionManager() = default;
    ExceptionManager(const ExceptionManager&) = delete;
    ExceptionManager& operator=(const ExceptionManager&) = delete;
    
    mutable std::mutex mutex_;
    std::vector<std::unique_ptr<IExceptionHandler>> handlers_;
};

// Exception macros for simplified error handling
#define NLM_THROW_MEMORY_ERROR(msg) \
    throw MemoryAllocationException(std::stoull("1024"), std::stoull("512"), msg)

#define NLM_THROW_VALIDATION_ERROR(param, expected, actual, op) \
    throw ValidationException(param, expected, actual, op)

#define NLM_THROW_CONFIG_ERROR(key, value, format, op) \
    throw ConfigurationException(key, value, format, op)

#define NLM_THROW_NEURAL_ERROR(type, operation, details, severity) \
    throw NeuralComputationException(type, operation, details, severity)

#define NLM_THROW_THREAD_ERROR(resource, operation, thread_info, severity) \
    throw ThreadSafetyException(resource, operation, thread_info, severity)

#define NLM_THROW_RESOURCE_ERROR(resource_type, operation, before, after) \
    throw ResourceManagementException(resource_type, operation, before, after)

// Graceful degradation utilities
class GracefulDegradation {
public:
    // Attempt operation with fallback
    template<typename Operation, typename Fallback>
    static auto attemptWithFallback(const Operation& operation, const Fallback& fallback,
                                   ErrorSeverity max_error_severity = ErrorSeverity::Warning);
    
    // Try multiple operations with fallbacks
    template<typename... Operations>
    static auto tryMultipleOperations(const std::tuple<Operations...>& operations);
    
    // Degrade functionality gracefully
    static std::string getDegradedFunctionality(const std::string& primary, 
                                              const std::string& fallback,
                                              double threshold = 0.5);
    
    // Create degraded service
    template<typename Service>
    static std::shared_ptr<Service> createDegradedService(
        std::function<std::shared_ptr<Service>()> primary_factory,
        std::function<std::shared_ptr<Service>()> degraded_factory);
};

// Safe resource management helper class
class SafeResource;

class CleanupGuard {
public:
    CleanupGuard(SafeResource& resource);
    ~CleanupGuard();
    
    SafeResource& getResource() { return resource_; }
    
protected:
    SafeResource& resource_;
};

class SafeResource {
public:
    SafeResource(const std::string& name, std::function<void()> cleanup_func);
    
    ~SafeResource();
    
    // Acquire resource
    void acquire();
    
    // Release resource
    void release();
    
    // Check if resource is available
    bool isAvailable() const;
    
    // Check if resource is in use
    bool isInUse() const;
    
    // Get resource state
    ResourceState getState() const;
    
    // Cleanup resource
    void cleanup();
    
    // Ensure resource is cleaned up even if exception occurs
    CleanupGuard acquireGuard();
    
protected:
    std::string name_;
    ResourceState state_;
    std::function<void()> cleanup_func_;
};

// Thread-safe resource wrapper
class ThreadSafeResource {
public:
    ThreadSafeResource(std::shared_ptr<SafeResource> resource, const std::string& name);
    
    // Acquire resource with timeout
    bool acquire(double timeout_seconds = 1.0);
    
    // Release resource
    void release();
    
    // Check if resource is available
    bool isAvailable() const;
    
    // Check if resource is in use
    bool isInUse() const;
    
    // Get resource name
    const std::string& getName() const;
    
protected:
    std::shared_ptr<SafeResource> resource_;
    std::string name_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

// Convenience functions for easy exception handling
void handleException(const NLMException& exception);

// Helper function to convert any exception to NLMException
void handleAnyException(const std::exception& e, const std::string& operation);

// Macros for safe resource usage
#define SAFE_RESOURCE_USAGE(resource_var, resource_name, cleanup_action) \
    nlm::SafeResource resource_var(resource_name, cleanup_action); \
    auto resource_guard_##__LINE__ = resource_var.acquireGuard();

// Macro for thread-safe resource usage
#define THREAD_SAFE_RESOURCE_USAGE(resource_var, resource_name) \
    nlm::ThreadSafeResource resource_var(resource_name, resource_name);

// Macro for graceful operation with fallback
#define GRACEFUL_OPERATION(operation, fallback, max_severity) \
    nlm::GracefulDegradation::attemptWithFallback(operation, fallback, max_severity)

} // namespace nlm
