// ErrorHandling.hpp - Comprehensive error handling infrastructure for NLM
#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <map>
#include <chrono>
#include <vector>
#include <sstream>
#include <optional>
#include <functional>

namespace nlm {

// Error codes for different types of failures
enum class ErrorCode {
    // Success
    Success = 0,
    
    // Configuration errors
    ConfigInvalidParameter = 1000,
    ConfigMissingRequiredField = 1001,
    ConfigFileNotFound = 1002,
    ConfigParseError = 1003,
    
    // Memory errors
    MemoryAllocationFailed = 2000,
    MemoryCorruptionDetected =2001,
    OutOfMemory =2002,
    
    // Brain/Neural errors
    BrainInitializationFailed =3000,
    BrainValidationFailed =3001,
    NeuronCreationFailed =3002,
    SynapseCreationFailed =3003,
    RegionCreationFailed =3004,
    
    // Simulation errors
    SimulationStepFailed =4000,
    SimulationNotInitialized =4001,
    InvalidSimulationParameters =4002,
    
    // Memory system errors
    MemorySystemNotAvailable =5000,
    MemoryIntegrationFailed =5001,
    EpisodeStorageFailed =5002,
    
    // Neuromodulation errors
    NeuromodulationSystemNotAvailable =6000,
    NeuromodulationUpdateFailed =6001,
    
    // Prediction system errors
    PredictionSystemNotAvailable =7000,
    PredictionUpdateFailed =7001,
    
    // Cognition system errors
    CognitionSystemNotAvailable =8000,
    CognitionUpdateFailed =8001,
    
    // Development errors
    DevelopmentSystemNotAvailable =9000,
    DevelopmentUpdateFailed =9001,
    
    // Checkpoint errors
    CheckpointSaveFailed =10000,
    CheckpointLoadFailed =10001,
    CheckpointValidationFailed =10002,
    
    // Replay errors
    ReplaySystemNotAvailable =11000,
    ReplayExecutionFailed =11001,
    
    // Resource errors
    ResourceNotAvailable =12000,
    ResourceInitializationFailed =12001,
    
    // Generic errors
    GeneralError =13000,
    NotImplemented =13001,
    InvalidOperation =13002,
    Timeout =13003,
    
    // External system errors
    ExternalSystemError =14000,
    FileSystemError =14001,
    NetworkError =14002,
    
    // Security errors
    SecurityViolation =15000,
    AccessDenied =15001,
    
    // System errors
    SystemError =16000,
    ThreadError =16001,
    ConcurrencyError =16002,
    
    // Unknown errors
    UnknownError =17000,
    InternalError =17001
};

// Error category for system error codes
class NLMErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "NLM";
    }
    
    std::string message(int ev) const override {
        auto it = errorMessages.find(static_cast<ErrorCode>(ev));
        if (it != errorMessages.end()) {
            return it->second;
        }
        return "Unknown NLM error";
    }
    
    static std::string getMessage(ErrorCode code) {
        auto it = errorMessages.find(code);
        if (it != errorMessages.end()) {
            return it->second;
        }
        return "Unknown error";
    }
    
private:
    static std::map<ErrorCode, std::string> errorMessages;
};

// Exception classes for different error types
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCode code, const std::string& message, 
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : std::runtime_error(formatMessage(code, message, file, line, function)),
          code_(code),
          file_(file),
          line_(line),
          function_(function),
          timestamp_(std::chrono::system_clock::now()) {
    }
    
    ErrorCode getCode() const noexcept { return code_; }
    const std::string& getFile() const noexcept { return file_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFunction() const noexcept { return function_; }
    const auto& getTimestamp() const noexcept { return timestamp_; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[" << getTimestampString() << "] "
            << "Error Code: " << static_cast<int>(code_) << ": "
            << std::runtime_error::what();
        if (!file_.empty()) {
            oss << " (at " << file_;
            if (line_ > 0) oss << ":" << line_;
            if (!function_.empty()) oss << " in " << function_;
            oss << ")";
        }
        return oss.str();
    }
    
protected:
    static std::string formatMessage(ErrorCode code, const std::string& message,
                                     const std::string& file, int line,
                                     const std::string& function) {
        std::ostringstream oss;
        oss << NLMErrorCategory::getMessage(code);
        if (!message.empty()) {
            oss << ": " << message;
        }
        return oss.str();
    }
    
    static std::string getTimestampString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
private:
    ErrorCode code_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
};

class NLMConfigException : public NLMException {
public:
    NLMConfigException(ErrorCode code, const std::string& message,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "")
        : NLMException(code, message, file, line, function) {
    }
};

class NLMBrainException : public NLMException {
public:
    NLMBrainException(ErrorCode code, const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "")
        : NLMException(code, message, file, line, function) {
    }
};

class NLMMemoryException : public NLMException {
public:
    NLMMemoryException(ErrorCode code, const std::string& message,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "")
        : NLMException(code, message, file, line, function) {
    }
};

class NLMNeuromodulationException : public NLMException {
public:
    NLMNeuromodulationException(ErrorCode code, const std::string& message,
                                 const std::string& file = "", int line = 0,
                                 const std::string& function = "")
        : NLMException(code, message, file, line, function) {
    }
};

// Result type for functions that can fail
template<typename T>
class Result {
public:
    Result() : success_(false), error_(ErrorCode::UnknownError) {}
    Result(const T& value) : success_(true), value_(value) {}
    Result(ErrorCode code, const std::string& message, 
           const std::string& file = "", int line = 0,
           const std::string& function = "")
        : success_(false), error_(code), errorMessage_(message), 
          file_(file), line_(line), function_(function) {}
    
    bool success() const { return success_; }
    const T& value() const { return value_; }
    const T& value() { return value_; }
    
    ErrorCode errorCode() const { return error_; }
    const std::string& errorMessage() const { return errorMessage_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    std::string getFullError() const {
        std::ostringstream oss;
        oss << "Error Code: " << static_cast<int>(error_) << ": "
            << errorMessage_;
        if (!file_.empty()) {
            oss << " (at " << file_;
            if (line_ > 0) oss << ":" << line_;
            if (!function_.empty()) oss << " in " << function_;
            oss << ")";
        }
        return oss.str();
    }
    
    // Convert to exception
    void throwIfFailed() const {
        if (!success_) {
            throw NLMException(error_, errorMessage_, file_, line_, function_);
        }
    }
    
    // Get reference to underlying value (unsafe if not success)
    T& getValueRef() { return value_; }
    const T& getValueRef() const { return value_; }
    
    // Chaining for better error handling
    template<typename U>
    Result<U> map(std::function<U(const T&)> f) const {
        if (success_) {
            return Result<U>(f(value_));
        } else {
            return Result<U>(error_, errorMessage_, file_, line_, function_);
        }
    }
    
private:
    bool success_;
    T value_;
    ErrorCode error_;
    std::string errorMessage_;
    std::string file_;
    int line_;
    std::string function_;
};

// Error handler class with context management
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Set the current error context
    void setContext(const std::string& context, const std::string& operation = "") {
        currentContext_ = context;
        currentOperation_ = operation;
    }
    
    // Add additional context information
    void addContext(const std::string& key, const std::string& value) {
        contextData_[key] = value;
    }
    
    // Clear all context
    void clearContext() {
        currentContext_.clear();
        currentOperation_.clear();
        contextData_.clear();
    }
    
    // Handle an error with enhanced context
    template<typename... Args>
    void handleError(ErrorCode code, const std::string& message, Args&&... args) {
        std::ostringstream oss;
        oss << message;
        (void)std::initializer_list<int>{ (oss << " " << std::forward<Args>(args), 0)... };
        
        std::string fullMessage = oss.str();
        
        // Add context information
        if (!currentContext_.empty()) {
            fullMessage = "[" + currentContext_ + "] " + fullMessage;
        }
        if (!currentOperation_.empty()) {
            fullMessage += " (Operation: " + currentOperation_ + ")";
        }
        for (const auto& pair : contextData_) {
            fullMessage += " [" + pair.first + ": " + pair.second + "]";
        }
        
        // Log the error
        if (logger_) {
            logger_->error(fullMessage, "", 0, "");
        }
        
        // Throw exception for critical errors
        if (code >= ErrorCode::GeneralError && code < ErrorCode::ExternalSystemError) {
            throw NLMException(code, fullMessage, "", 0, "");
        }
    }
    
    // Log error without throwing
    template<typename... Args>
    void logError(ErrorCode code, const std::string& message, Args&&... args) {
        std::ostringstream oss;
        oss << message;
        (void)std::initializer_list<int>{ (oss << " " << std::forward<Args>(args), 0)... };
        
        std::string fullMessage = oss.str();
        
        // Add context information
        if (!currentContext_.empty()) {
            fullMessage = "[" + currentContext_ + "] " + fullMessage;
        }
        if (!currentOperation_.empty()) {
            fullMessage += " (Operation: " + currentOperation_ + ")";
        }
        for (const auto& pair : contextData_) {
            fullMessage += " [" + pair.first + ": " + pair.second + "]";
        }
        
        // Log the error
        if (logger_) {
            logger_->error(fullMessage, "", 0, "");
        }
    }
    
    // Set logger for error reporting
    void setLogger(std::shared_ptr<class ILogger> logger) {
        logger_ = logger;
    }
    
    // Get current logger
    std::shared_ptr<class ILogger> getLogger() const {
        return logger_;
    }
    
    // Check if errors should be thrown
    bool shouldThrow(ErrorCode code) const {
        return code >= ErrorCode::GeneralError && code < ErrorCode::ExternalSystemError;
    }
    
private:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    std::string currentContext_;
    std::string currentOperation_;
    std::map<std::string, std::string> contextData_;
    std::shared_ptr<class ILogger> logger_;
};

// Global error handler instance
inline ErrorHandler& GlobalErrorHandler() {
    return ErrorHandler::getInstance();
}

// Macros for enhanced error handling with context
#define NLM_THROW_ERROR(code, message) \
    do { \
        nlm::GlobalErrorHandler().handleError(code, message, __FILE__, __LINE__, __func__); \
    } while (0)

#define NLM_THROW_ERROR_CTX(code, message, context, ...) \
    do { \
        nlm::GlobalErrorHandler().setContext(context); \
        nlm::GlobalErrorHandler().handleError(code, message, __VA_ARGS__); \
        nlm::GlobalErrorHandler().clearContext(); \
    } while (0)

#define NLM_LOG_ERROR_CTX(code, message, context, ...) \
    do { \
        nlm::GlobalErrorHandler().setContext(context); \
        nlm::GlobalErrorHandler().logError(code, message, __VA_ARGS__); \
        nlm::GlobalErrorHandler().clearContext(); \
    } while (0)

#define NLM_TRY_CATCH_ALL \
    try \
    do \

#define NLM_CATCH_ALL \
    } while (0); \
    catch (const std::exception& e) \
    { \
        NLM_THROW_ERROR(nlm::ErrorCode::SystemError, std::string("Exception caught: ") + e.what()); \
    } \
    catch (...) \
    { \
        NLM_THROW_ERROR(nlm::ErrorCode::UnknownError, "Unknown exception caught"); \
    }

// Helper function for creating error results
template<typename T>
Result<T> makeErrorResult(ErrorCode code, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "") {
    return Result<T>(code, message, file, line, function);
}

// Helper function for successful results
template<typename T>
Result<T> makeSuccessResult(const T& value) {
    return Result<T>(value);
}

} // namespace nlm

// Initialize static error messages
inline std::map<nlm::ErrorCode, std::string> nlm::NLMErrorCategory::errorMessages = {
    {nlm::ErrorCode::Success, "Operation completed successfully"},
    
    // Configuration errors
    {nlm::ErrorCode::ConfigInvalidParameter, "Invalid configuration parameter"},
    {nlm::ErrorCode::ConfigMissingRequiredField, "Missing required configuration field"},
    {nlm::ErrorCode::ConfigFileNotFound, "Configuration file not found"},
    {nlm::ErrorCode::ConfigParseError, "Failed to parse configuration file"},
    
    // Memory errors
    {nlm::ErrorCode::MemoryAllocationFailed, "Memory allocation failed"},
    {nlm::ErrorCode::MemoryCorruptionDetected, "Memory corruption detected"},
    {nlm::ErrorCode::OutOfMemory, "Out of memory"},
    
    // Brain/Neural errors
    {nlm::ErrorCode::BrainInitializationFailed, "Brain initialization failed"},
    {nlm::ErrorCode::BrainValidationFailed, "Brain validation failed"},
    {nlm::ErrorCode::NeuronCreationFailed, "Neuron creation failed"},
    {nlm::ErrorCode::SynapseCreationFailed, "Synapse creation failed"},
    {nlm::ErrorCode::RegionCreationFailed, "Region creation failed"},
    
    // Simulation errors
    {nlm::ErrorCode::SimulationStepFailed, "Simulation step failed"},
    {nlm::ErrorCode::SimulationNotInitialized, "Simulation not initialized"},
    {nlm::ErrorCode::InvalidSimulationParameters, "Invalid simulation parameters"},
    
    // Memory system errors
    {nlm::ErrorCode::MemorySystemNotAvailable, "Memory system not available"},
    {nlm::ErrorCode::MemoryIntegrationFailed, "Memory integration failed"},
    {nlm::ErrorCode::EpisodeStorageFailed, "Episode storage failed"},
    
    // Neuromodulation errors
    {nlm::ErrorCode::NeuromodulationSystemNotAvailable, "Neuromodulation system not available"},
    {nlm::ErrorCode::NeuromodulationUpdateFailed, "Neuromodulation update failed"},
    
    // Prediction system errors
    {nlm::ErrorCode::PredictionSystemNotAvailable, "Prediction system not available"},
    {nlm::ErrorCode::PredictionUpdateFailed, "Prediction update failed"},
    
    // Cognition system errors
    {nlm::ErrorCode::CognitionSystemNotAvailable, "Cognition system not available"},
    {nlm::ErrorCode::CognitionUpdateFailed, "Cognition update failed"},
    
    // Development errors
    {nlm::ErrorCode::DevelopmentSystemNotAvailable, "Development system not available"},
    {nlm::ErrorCode::DevelopmentUpdateFailed, "Development update failed"},
    
    // Checkpoint errors
    {nlm::ErrorCode::CheckpointSaveFailed, "Checkpoint save failed"},
    {nlm::ErrorCode::CheckpointLoadFailed, "Checkpoint load failed"},
    {nlm::ErrorCode::CheckpointValidationFailed, "Checkpoint validation failed"},
    
    // Replay errors
    {nlm::ErrorCode::ReplaySystemNotAvailable, "Replay system not available"},
    {nlm::ErrorCode::ReplayExecutionFailed, "Replay execution failed"},
    
    // Resource errors
    {nlm::ErrorCode::ResourceNotAvailable, "Resource not available"},
    {nlm::ErrorCode::ResourceInitializationFailed, "Resource initialization failed"},
    
    // Generic errors
    {nlm::ErrorCode::GeneralError, "General error occurred"},
    {nlm::ErrorCode::NotImplemented, "Operation not implemented"},
    {nlm::ErrorCode::InvalidOperation, "Invalid operation"},
    {nlm::ErrorCode::Timeout, "Operation timed out"},
    
    // External system errors
    {nlm::ErrorCode::ExternalSystemError, "External system error"},
    {nlm::ErrorCode::FileSystemError, "File system error"},
    {nlm::ErrorCode::NetworkError, "Network error"},
    
    // Security errors
    {nlm::ErrorCode::SecurityViolation, "Security violation"},
    {nlm::ErrorCode::AccessDenied, "Access denied"},
    
    // System errors
    {nlm::ErrorCode::SystemError, "System error occurred"},
    {nlm::ErrorCode::ThreadError, "Thread error occurred"},
    {nlm::ErrorCode::ConcurrencyError, "Concurrency error occurred"},
    
    // Unknown errors
    {nlm::ErrorCode::UnknownError, "Unknown error occurred"},
    {nlm::ErrorCode::InternalError, "Internal error occurred"}
};

#include "ErrorHandling.cpp"
