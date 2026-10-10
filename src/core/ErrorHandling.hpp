#pragma once

#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace nlm {

// Error codes enum - comprehensive error categorization
enum class ErrorCode {
    // Success codes
    Success = 0,
    
    // System errors
    System_MemoryAllocationFailed,
    System_FileNotFound,
    System_PermissionDenied,
    System_InvalidFileFormat,
    System_IOError,
    System_Timeout,
    System_OutOfMemory,
    System_ThreadError,
    System_InvalidConfiguration,
    
    // Component-specific errors
    Component_InvalidParameter,
    Component_InitializationFailed,
    Component_OperationFailed,
    Component_NotInitialized,
    Component_InvalidState,
    Component_NotFound,
    Component_AlreadyExists,
    Component_OutOfRange,
    Component_FormatError,
    Component_CompatibilityError,
    
    // Neural network errors
    Neural_InvalidNeuronId,
    Neural_InvalidSynapseId,
    Neural_ConnectionFailed,
    Neural_ComputationError,
    Neural_PropagationError,
    Neural_PlasticityError,
    Neural_MemoryError,
    
    // Simulation errors
    Simulation_InvalidStep,
    Simulation_InsufficientInputs,
    Simulation_TooManySpikes,
    Simulation_StabilityError,
    
    // Memory errors
    Memory_AllocationFailed,
    Memory_InitializationError,
    Memory_ConsolidationError,
    Memory_CompressionError,
    
    // Prediction/ cognition errors
    Prediction_InvalidPrediction,
    Prediction_ErrorComputationFailed,
    Cognition_PlanningError,
    Cognition_ConceptError,
    
    // Neuromodulation errors
    Neuromodulation_InvalidSignal,
    Neuromodulation_ComputationError,
    
    // Agent errors
    Agent_InvalidAction,
    Agent_PlanningError,
    Agent_ExecutionError,
    
    // Environment errors
    Environment_InvalidInput,
    Environment_CollisionError,
    Environment_SimulationError,
    
    // Generic errors
    UnknownError,
    NotImplementedError,
    ValidationError,
    SecurityError
};

// Base exception class with error code and context
class NLMError : public std::exception {
protected:
    ErrorCode code_;
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::map<std::string, std::string> context_;
    
public:
    explicit NLMError(ErrorCode code, const std::string& message,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : code_(code), message_(message), file_(file), line_(line),
          function_(function), timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMError() noexcept = default;
    
    // Error code accessor
    ErrorCode code() const noexcept { return code_; }
    
    // Error message accessor
    const char* what() const noexcept override {
        static std::string formattedMessage;
        std::stringstream ss;
        
        ss << "Error Code: " << static_cast<int>(code_) << " - " << message_;
        if (!file_.empty()) ss << "\nFile: " << file_;
        if (line_ > 0) ss << ":" << line_;
        if (!function_.empty()) ss << "\nFunction: " << function_;
        
        formattedMessage = ss.str();
        return formattedMessage.c_str();
    }
    
    // Additional context
    void addContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }
    
    const std::map<std::string, std::string>& getContext() const { return context_; }
    
    // Error type for switching
    virtual const char* type() const noexcept { return "NLMError"; }
};

// Specific error types for better error handling
class NLMSystemError : public NLMError {
public:
    explicit NLMSystemError(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : NLMError(code, message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "SystemError"; }
};

class NLMComponentError : public NLMError {
public:
    explicit NLMComponentError(ErrorCode code, const std::string& message,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "")
        : NLMError(code, message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "ComponentError"; }
};

class NLMNeuralError : public NLMError {
public:
    explicit NLMNeuralError(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : NLMError(code, message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "NeuralError"; }
};

class NLMMemoryError : public NLMError {
public:
    explicit NLMMemoryError(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : NLMError(code, message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "MemoryError"; }
};

// Error handling utilities
class ErrorHandler {
public:
    // Global error callback function type
    using ErrorCallback = std::function<void(const NLMError&)>;
    
    // Set global error callback
    static void setErrorCallback(ErrorCallback callback) {
        errorCallback_ = std::move(callback);
    }
    
    // Log and throw error
    static void throwError(ErrorCode code, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "") {
        auto error = std::make_unique<NLMError>(code, message, file, line, function);
        logError(*error);
        throw std::move(error);
    }
    
    // Log error without throwing
    static void logError(ErrorCode code, const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "") {
        auto error = std::make_unique<NLMError>(code, message, file, line, function);
        logError(*error);
    }
    
    // Safe error handling macro
    #define NLM_THROW_ERROR(code, message) \
        nlm::ErrorHandler::throwError(code, message, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_ERROR(code, message) \
        nlm::ErrorHandler::logError(code, message, __FILE__, __LINE__, __func__)
    
    // Handle operations with error checking
    template<typename T, typename F>
    static T executeWithErrorCheck(const std::string& operationName, F&& func) {
        try {
            return func();
        } catch (const NLMError&) {
            throw;
        } catch (const std::exception& e) {
            logError(ErrorCode::UnknownError, 
                    "Operation " + operationName + " failed: " + e.what());
            throw NLMSystemError(ErrorCode::System_IOError,
                               "Operation " + operationName + " failed");
        }
    }
    
    // Check condition and throw if false
    static void checkCondition(bool condition, ErrorCode code,
                              const std::string& message,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "") {
        if (!condition) {
            throwError(code, message, file, line, function);
        }
    }
    
    // Safe file operations
    static std::string readFile(const std::string& filepath);
    static bool writeFile(const std::string& filepath, const std::string& content);
    static bool fileExists(const std::string& filepath);
    
    // Safe memory operations
    template<typename T>
    static std::unique_ptr<T> allocateMemory(size_t count) {
        try {
            auto ptr = std::make_unique<T[]>(count);
            return ptr;
        } catch (const std::bad_alloc&) {
            throw NLMMemoryError(ErrorCode::System_MemoryAllocationFailed,
                               "Failed to allocate memory for " + 
                               std::to_string(count) + " objects");
        }
    }
    
private:
    static void logError(const NLMError& error) {
        if (errorCallback_) {
            errorCallback_(error);
        }
        // Also use the existing logger if available
        // Logger::getGlobal().error(error.what(), error.file(), error.line(), error.function());
    }
    
    static ErrorCallback errorCallback_;
};

// Error reporting utilities
class ErrorReporter {
public:
    // Generate error report
    static std::string generateReport(const NLMError& error);
    
    // Generate system-wide error summary
    static std::string generateSystemSummary();
    
    // Export errors to file
    static bool exportErrors(const std::string& filepath,
                            const std::vector<std::reference_wrapper<const NLMError>>& errors);
    
    // Create error context from exception chain
    static std::string extractErrorContext(const std::exception& e);
};

// Error handling macros for different levels
#define NLM_CHECK_THROW(condition, code, message) \
    nlm::ErrorHandler::checkCondition(condition, code, message, __FILE__, __LINE__, __func__)

#define NLM_TRY_CATCH(operation, errorCode) \
    try { \
        operation; \
    } catch (const nlm::NLMError&) { \
        throw; \
    } catch (const std::exception& e) { \
        nlm::ErrorHandler::logError(errorCode, "Operation failed: " + std::string(e.what())); \
        throw nlm::NLMSystemError(errorCode, "Operation failed: " + std::string(e.what())); \
    }

// Error mapping utilities for backward compatibility
class ErrorCompatibility {
public:
    // Map standard C++ exceptions to NLM errors
    static ErrorCode mapException(const std::exception& e);
    
    // Get error code for file operation
    static ErrorCode getFileErrorCode(int errnum);
    
    // Get error code for memory operation
    static ErrorCode getMemoryErrorCode(int errnum);
    
    // Convert NLM error to standard exception
    static std::unique_ptr<NLMError> toNLMError(const std::exception& e);
};

} // namespace nlm