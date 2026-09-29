#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <sstream>

namespace nlm {

namespace error {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Error categories
enum class ErrorCategory {
    Configuration,
    FileIO,
    NeuralComputation,
    MemoryManagement,
    Validation,
    ResourceManagement,
    PluginSystem,
    Simulation,
    Rendering,
    System
};

// Detailed error codes for each category
enum class ErrorCode {
    // Configuration errors
    ConfigFileNotFound = 1000,
    ConfigFileInvalidFormat,
    ConfigKeyNotFound,
    ConfigValueInvalid,
    ConfigValidationFailed,
    
    // File I/O errors
    FileNotFound = 2000,
    FileCannotOpen,
    FileCannotRead,
    FileCannotWrite,
    FileInvalidFormat,
    
    // Neural computation errors
    NeuronInvalidState = 3000,
    InvalidSynapticConnection,
    STDPUpdateFailed,
    IntegrationError,
    ConvergenceFailure,
    
    // Memory management errors
    MemoryAllocationFailed = 4000,
    MemoryCorruption,
    MemoryLeak,
    OutOfMemory,
    BufferOverflow,
    
    // Validation errors
    ValidationFailed = 5000,
    InputRangeInvalid,
    ParameterInvalid,
    ConstraintViolated,
    DataCorruption,
    
    // Resource management errors
    ResourceNotAvailable = 6000,
    ResourceAlreadyInUse,
    ResourceLimitExceeded,
    PermissionDenied,
    Timeout,
    
    // Plugin system errors
    PluginLoadFailed = 7000,
    PluginNotFound,
    PluginVersionMismatch,
    PluginDependencyMissing,
    PluginInitializationFailed,
    
    // Simulation errors
    SimulationNotRunning = 8000,
    SimulationAlreadyRunning,
    SimulationStepFailed,
    SimulationTimeout,
    SimulationInvalidState,
    
    // Rendering errors
    RenderContextLost = 9000,
    ShaderCompilationFailed,
    TextureLoadFailed,
    RenderTargetNotAvailable,
    
    // System errors
    SystemCallFailed = 10000,
    OutOfSystemResources,
    ThreadCreationFailed,
    NetworkError,
    SerializationError,
    
    // Generic errors
    UnknownError = 99999,
    NotImplemented,
    InvalidArgument,
    OperationCancelled
};

// Enhanced error information
struct ErrorInfo {
    ErrorCategory category;
    ErrorCode code;
    ErrorSeverity severity;
    std::string message;
    std::string context;
    std::string file;
    int line;
    std::string function;
    std::map<std::string, std::string> details;
    std::vector<std::string> suggestions;
    
    // Stack trace information
    std::vector<std::string> stackTrace;
    
    // Timestamp for error reporting
    std::chrono::system_clock::time_point timestamp;
    
    ErrorInfo() : category(ErrorCategory::UnknownError), code(ErrorCode::UnknownError),
                  severity(ErrorSeverity::Error), line(0) {}
    
    ErrorInfo(ErrorCategory cat, ErrorCode errCode, ErrorSeverity sev,
              const std::string& msg, const std::string& ctx = "",
              const std::string& filePath = "", int ln = 0,
              const std::string& func = "")
        : category(cat), code(errCode), severity(sev), message(msg),
          context(ctx), file(filePath), line(ln), function(func) {}
};

// Base exception class
class NLMError : public std::runtime_error {
public:
    NLMError(const ErrorInfo& info)
        : std::runtime_error(formatError(info)), info_(info) {}
    
    const ErrorInfo& getInfo() const { return info_; }
    
    ErrorCategory getCategory() const { return info_.category; }
    ErrorCode getCode() const { return info_.code; }
    ErrorSeverity getSeverity() const { return info_.severity; }
    const std::string& getContext() const { return info_.context; }
    const std::string& getMessage() const { return info_.message; }
    
    // Detailed error information
    const std::string& getFile() const { return info_.file; }
    int getLine() const { return info_.line; }
    const std::string& getFunction() const { return info_.function; }
    const std::map<std::string, std::string>& getDetails() const { return info_.details; }
    const std::vector<std::string>& getSuggestions() const { return info_.suggestions; }
    
    // Add detail information
    void addDetail(const std::string& key, const std::string& value) {
        info_.details[key] = value;
    }
    
    void addSuggestion(const std::string& suggestion) {
        info_.suggestions.push_back(suggestion);
    }
    
    // Error formatting
    static std::string formatError(const ErrorInfo& info) {
        std::ostringstream oss;
        oss << "NLM Error [" << errorCodeToString(info.code) << "]: " << info.message;
        
        if (!info.context.empty()) {
            oss << " (context: " << info.context << ")";
        }
        
        if (!info.file.empty() && info.line > 0) {
            oss << "\n  File: " << info.file << ":" << info.line;
            if (!info.function.empty()) {
                oss << " in " << info.function;
            }
        }
        
        if (!info.details.empty()) {
            oss << "\n  Details:";
            for (const auto& detail : info.details) {
                oss << "\n    " << detail.first << ": " << detail.second;
            }
        }
        
        if (!info.suggestions.empty()) {
            oss << "\n  Suggestions:";
            for (const auto& suggestion : info.suggestions) {
                oss << "\n    - " << suggestion;
            }
        }
        
        return oss.str();
    }
    
    static std::string errorCodeToString(ErrorCode code) {
        static const std::map<ErrorCode, std::string> errorCodeStrings = {
            {ErrorCode::ConfigFileNotFound, "ConfigFileNotFound"},
            {ErrorCode::ConfigFileInvalidFormat, "ConfigFileInvalidFormat"},
            {ErrorCode::ConfigKeyNotFound, "ConfigKeyNotFound"},
            {ErrorCode::ConfigValueInvalid, "ConfigValueInvalid"},
            {ErrorCode::ConfigValidationFailed, "ConfigValidationFailed"},
            {ErrorCode::FileNotFound, "FileNotFound"},
            {ErrorCode::FileCannotOpen, "FileCannotOpen"},
            {ErrorCode::FileCannotRead, "FileCannotRead"},
            {ErrorCode::FileCannotWrite, "FileCannotWrite"},
            {ErrorCode::FileInvalidFormat, "FileInvalidFormat"},
            {ErrorCode::NeuronInvalidState, "NeuronInvalidState"},
            {ErrorCode::InvalidSynapticConnection, "InvalidSynapticConnection"},
            {ErrorCode::STDPUpdateFailed, "STDPUpdateFailed"},
            {ErrorCode::IntegrationError, "IntegrationError"},
            {ErrorCode::ConvergenceFailure, "ConvergenceFailure"},
            {ErrorCode::MemoryAllocationFailed, "MemoryAllocationFailed"},
            {ErrorCode::MemoryCorruption, "MemoryCorruption"},
            {ErrorCode::MemoryLeak, "MemoryLeak"},
            {ErrorCode::OutOfMemory, "OutOfMemory"},
            {ErrorCode::BufferOverflow, "BufferOverflow"},
            {ErrorCode::ValidationFailed, "ValidationFailed"},
            {ErrorCode::InputRangeInvalid, "InputRangeInvalid"},
            {ErrorCode::ParameterInvalid, "ParameterInvalid"},
            {ErrorCode::ConstraintViolated, "ConstraintViolated"},
            {ErrorCode::DataCorruption, "DataCorruption"},
            {ErrorCode::ResourceNotAvailable, "ResourceNotAvailable"},
            {ErrorCode::ResourceAlreadyInUse, "ResourceAlreadyInUse"},
            {ErrorCode::ResourceLimitExceeded, "ResourceLimitExceeded"},
            {ErrorCode::PermissionDenied, "PermissionDenied"},
            {ErrorCode::Timeout, "Timeout"},
            {ErrorCode::PluginLoadFailed, "PluginLoadFailed"},
            {ErrorCode::PluginNotFound, "PluginNotFound"},
            {ErrorCode::PluginVersionMismatch, "PluginVersionMismatch"},
            {ErrorCode::PluginDependencyMissing, "PluginDependencyMissing"},
            {ErrorCode::PluginInitializationFailed, "PluginInitializationFailed"},
            {ErrorCode::SimulationNotRunning, "SimulationNotRunning"},
            {ErrorCode::SimulationAlreadyRunning, "SimulationAlreadyRunning"},
            {ErrorCode::SimulationStepFailed, "SimulationStepFailed"},
            {ErrorCode::SimulationTimeout, "SimulationTimeout"},
            {ErrorCode::SimulationInvalidState, "SimulationInvalidState"},
            {ErrorCode::RenderContextLost, "RenderContextLost"},
            {ErrorCode::ShaderCompilationFailed, "ShaderCompilationFailed"},
            {ErrorCode::TextureLoadFailed, "TextureLoadFailed"},
            {ErrorCode::RenderTargetNotAvailable, "RenderTargetNotAvailable"},
            {ErrorCode::SystemCallFailed, "SystemCallFailed"},
            {ErrorCode::OutOfSystemResources, "OutOfSystemResources"},
            {ErrorCode::ThreadCreationFailed, "ThreadCreationFailed"},
            {ErrorCode::NetworkError, "NetworkError"},
            {ErrorCode::SerializationError, "SerializationError"},
            {ErrorCode::UnknownError, "UnknownError"},
            {ErrorCode::NotImplemented, "NotImplemented"},
            {ErrorCode::InvalidArgument, "InvalidArgument"},
            {ErrorCode::OperationCancelled, "OperationCancelled"}
        };
        
        auto it = errorCodeStrings.find(code);
        return (it != errorCodeStrings.end()) ? it->second : "UnknownError";
    }
    
private:
    ErrorInfo info_;
};

// Specialized exception classes for different error types
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message, const std::string& context = "",
                const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMError(createErrorInfo(ErrorCategory::Configuration, ErrorCode::ConfigValidationFailed,
                                   ErrorSeverity::Error, message, context, file, line, function)) {}
    
private:
    static ErrorInfo createErrorInfo(ErrorCategory cat, ErrorCode code, ErrorSeverity sev,
                                     const std::string& message, const std::string& context,
                                     const std::string& file, int line, const std::string& function) {
        return ErrorInfo(cat, code, sev, message, context, file, line, function);
    }
};

class FileIOError : public NLMError {
public:
    FileIOError(const std::string& message, const std::string& context = "",
                const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMError(createErrorInfo(ErrorCategory::FileIO, ErrorCode::FileCannotRead,
                                   ErrorSeverity::Error, message, context, file, line, function)) {}
    
private:
    static ErrorInfo createErrorInfo(ErrorCategory cat, ErrorCode code, ErrorSeverity sev,
                                     const std::string& message, const std::string& context,
                                     const std::string& file, int line, const std::string& function) {
        return ErrorInfo(cat, code, sev, message, context, file, line, function);
    }
};

class NeuralComputationError : public NLMError {
public:
    NeuralComputationError(const std::string& message, const std::string& context = "",
                          const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMError(createErrorInfo(ErrorCategory::NeuralComputation, ErrorCode::IntegrationError,
                                   ErrorSeverity::Error, message, context, file, line, function)) {}
    
private:
    static ErrorInfo createErrorInfo(ErrorCategory cat, ErrorCode code, ErrorSeverity sev,
                                     const std::string& message, const std::string& context,
                                     const std::string& file, int line, const std::string& function) {
        return ErrorInfo(cat, code, sev, message, context, file, line, function);
    }
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, const std::string& context = "",
                const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMError(createErrorInfo(ErrorCategory::MemoryManagement, ErrorCode::MemoryAllocationFailed,
                                   ErrorSeverity::Critical, message, context, file, line, function)) {}
    
private:
    static ErrorInfo createErrorInfo(ErrorCategory cat, ErrorCode code, ErrorSeverity sev,
                                     const std::string& message, const std::string& context,
                                     const std::string& file, int line, const std::string& function) {
        return ErrorInfo(cat, code, sev, message, context, file, line, function);
    }
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message, const std::string& context = "",
                    const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMError(createErrorInfo(ErrorCategory::Validation, ErrorCode::ValidationFailed,
                                   ErrorSeverity::Warning, message, context, file, line, function)) {}
    
private:
    static ErrorInfo createErrorInfo(ErrorCategory cat, ErrorCode code, ErrorSeverity sev,
                                     const std::string& message, const std::string& context,
                                     const std::string& file, int line, const std::string& function) {
        return ErrorInfo(cat, code, sev, message, context, file, line, function);
    }
};

// Error reporting and handling interface
class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const NLMError&)>
    
    static void registerErrorHandler(ErrorCallback callback) {
        errorCallback_ = callback;
    }
    
    static void unregisterErrorHandler() {
        errorCallback_ = nullptr;
    }
    
    static void handleError(const NLMError& error) {
        if (errorCallback_) {
            errorCallback_(error);
        }
        
        // Log error
        logError(error);
        
        // Check if error can be recovered
        if (canRecover(error)) {
            attemptRecovery(error);
        }
    }
    
    static void logError(const NLMError& error) {
        std::cerr << error.what() << std::endl;
        
        // Additional logging based on severity
        switch (error.getSeverity()) {
            case ErrorSeverity::Debug:
                // Debug logging
                break;
            case ErrorSeverity::Info:
                // Info logging
                break;
            case ErrorSeverity::Warning:
                std::cerr << "[WARNING] " << error.what() << std::endl;
                break;
            case ErrorSeverity::Error:
                std::cerr << "[ERROR] " << error.what() << std::endl;
                break;
            case ErrorSeverity::Critical:
                std::cerr << "[CRITICAL] " << error.what() << std::endl;
                break;
        }
    }
    
    static bool canRecover(const NLMError& error) {
        // Check if error can be recovered based on error type and state
        switch (error.getCode()) {
            case ErrorCode::MemoryAllocationFailed:
            case ErrorCode::OutOfMemory:
                return false; // Cannot recover from memory errors
            case ErrorCode::ConfigFileNotFound:
            case ErrorCode::FileNotFound:
                return true; // Can recover by using defaults
            case ErrorCode::ValidationFailed:
                return true; // Can recover by fixing inputs
            default:
                return true; // Default to recoverable
        }
    }
    
    static bool attemptRecovery(const NLMError& error) {
        // Attempt to recover from error
        switch (error.getCode()) {
            case ErrorCode::ConfigFileNotFound:
                // Load default configuration
                std::cerr << "Attempting recovery: Loading default configuration..." << std::endl;
                return true;
            case ErrorCode::FileNotFound:
                // Try alternative file paths
                std::cerr << "Attempting recovery: Trying alternative file paths..." << std::endl;
                return true;
            case ErrorCode::ValidationFailed:
                // Log warning and continue with default values
                std::cerr << "Attempting recovery: Using default values..." << std::endl;
                return true;
            default:
                return false;
        }
    }
    
    static void setErrorReportingLevel(ErrorSeverity level) {
        errorReportingLevel_ = level;
    }
    
    static ErrorSeverity getErrorReportingLevel() {
        return errorReportingLevel_;
    }
    
    static std::string getErrorSummary(const std::vector<NLMError>& errors) {
        std::ostringstream oss;
        oss << "Error Summary: " << errors.size() << " error(s) reported\n";
        
        for (const auto& error : errors) {
            oss << "  - " << error.getInfo().message << " (" << errorCodeToString(error.getCode()) << ")\n";
        }
        
        return oss.str();
    }
    
private:
    static ErrorCallback errorCallback_;
    static ErrorSeverity errorReportingLevel_;
};

// Global error handler instance
ErrorHandler::ErrorCallback ErrorHandler::errorCallback_ = nullptr;
ErrorSeverity ErrorHandler::errorReportingLevel_ = ErrorSeverity::Error;

// Error macros for easy error reporting
#define NLM_THROW_ERROR(category, code, severity, message, context) \
    do { \
        ErrorInfo info(category, code, severity, message, context, __FILE__, __LINE__, __func__); \
        NLMError error(info); \
        ErrorHandler::handleError(error); \
        throw error; \
    } while(0)

#define NLM_THROW_CONFIG_ERROR(message, context) \
    NLM_THROW_ERROR(ErrorCategory::Configuration, ErrorCode::ConfigValidationFailed, \
                    ErrorSeverity::Error, message, context)

#define NLM_THROW_FILE_IO_ERROR(message, context) \
    NLM_THROW_ERROR(ErrorCategory::FileIO, ErrorCode::FileCannotRead, \
                    ErrorSeverity::Error, message, context)

#define NLM_THROW_NEURAL_COMPUTATION_ERROR(message, context) \
    NLM_THROW_ERROR(ErrorCategory::NeuralComputation, ErrorCode::IntegrationError, \
                    ErrorSeverity::Error, message, context)

#define NLM_THROW_MEMORY_ERROR(message, context) \
    NLM_THROW_ERROR(ErrorCategory::MemoryManagement, ErrorCode::MemoryAllocationFailed, \
                    ErrorSeverity::Critical, message, context)

#define NLM_THROW_VALIDATION_ERROR(message, context) \
    NLM_THROW_ERROR(ErrorCategory::Validation, ErrorCode::ValidationFailed, \
                    ErrorSeverity::Warning, message, context)

// Safe execution wrapper with error handling
template<typename Func>
auto safeExecute(Func&& func, const std::string& context = "") -> decltype(func()) {
    try {
        return func();
    } catch (const NLMError& e) {
        ErrorHandler::handleError(e);
        throw; // Re-throw after handling
    } catch (const std::exception& e) {
        NLMError error(ErrorInfo(ErrorCategory::System, ErrorCode::UnknownError,
                                ErrorSeverity::Error, std::string("Unexpected exception: ") + e.what(),
                                context, __FILE__, __LINE__, __func__));
        ErrorHandler::handleError(error);
        throw error;
    }
}

// Error recovery context
class ErrorRecoveryContext {
public:
    ErrorRecoveryContext(const std::string& operation) : operation_(operation), recovered_(false) {}
    
    template<typename Func>
    auto executeWithRecovery(Func&& func) -> decltype(func()) {
        try {
            result_ = func();
            recovered_ = true;
            return result_;
        } catch (const NLMError& e) {
            if (!canRecover(e)) {
                throw; // Cannot recover, re-throw
            }
            
            // Attempt recovery
            if (attemptRecovery(e)) {
                recovered_ = true;
                // Retry the operation
                return executeWithRecovery(func);
            } else {
                throw; // Recovery failed, re-throw
            }
        }
    }
    
    bool wasRecovered() const { return recovered_; }
    bool isSuccessful() const { return recovered_; }
    
    const auto& getResult() const { return result_; }
    
private:
    std::string operation_;
    bool recovered_;
    
    template<typename Func>
    bool canRecover(const NLMError& error) {
        // Implement recovery logic based on error type
        switch (error.getCode()) {
            case ErrorCode::ConfigFileNotFound:
            case ErrorCode::FileNotFound:
            case ErrorCode::ValidationFailed:
                return true;
            default:
                return false;
        }
    }
    
    bool attemptRecovery(const NLMError& error) {
        // Implement recovery actions
        switch (error.getCode()) {
            case ErrorCode::ConfigFileNotFound:
                std::cerr << "Recovering from config file not found..." << std::endl;
                break;
            case ErrorCode::FileNotFound:
                std::cerr << "Recovering from file not found..." << std::endl;
                break;
            case ErrorCode::ValidationFailed:
                std::cerr << "Recovering from validation failure..." << std::endl;
                break;
            default:
                return false;
        }
        return true;
    }
    
    decltype(func()) result_;
};

} // namespace error

} // namespace nlm
