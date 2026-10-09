#pragma once

#include <string>
#include <stdexcept>
#include <map>
#include <chrono>
#include <sstream>
#include <memory>
#include <vector>
#include <format>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include <set>
#include <mutex>
#include <unordered_map>
#include "../core/Logger/Logger.hpp"

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

// Error codes for programmatic handling
enum class ErrorCode {
    // Configuration errors
    ConfigFileNotFound = 1000,
    ConfigParseError = 1001,
    ConfigValidationError = 1002,
    ConfigMissingRequired = 1003,
    ConfigInvalidType = 1004,
    ConfigValueOutOfRange = 1005,
    ConfigCircularReference = 1006,
    
    // File system errors
    FileNotFound = 2000,
    FilePermissionDenied = 2001,
    FileReadError = 2002,
    FileWriteError = 2003,
    FileCorrupted =2004,
    FileTooLarge =2005,
    DirectoryNotEmpty =2006,
    
    // Memory allocation errors
    MemoryAllocationFailed = 3000,
    MemoryOutOfMemory =3001,
    MemoryBlockCorrupted =3002,
    MemoryAlignmentError =3003,
    MemoryLeakDetected =3004,
    
    // Simulation errors
    SimulationNotInitialized = 4000,
    SimulationStepFailed =4001,
    InvalidSimulationParameters =4002,
    NeuronCountExceeded =4003,
    SynapseCountExceeded =4004,
    RegionCountExceeded =4005,
    InvalidConnection =4006,
    SimulationTimeStepInvalid =4007,
    
    // Validation errors
    ParameterValidationFailed =5000,
    InputValidationFailed =5001,
    RangeValidationFailed =5002,
    FormatValidationFailed =5003,
    SchemaValidationFailed =5004,
    CustomValidationFailed =5005,
    
    // Network/communication errors
    ConnectionFailed =6000,
    Timeout =6001,
    ProtocolError =6002,
    NetworkUnreachable =6003,
    
    // Resource exhaustion errors
    ResourceLimitExceeded =7000,
    CPUUsageTooHigh =7001,
    MemoryUsageTooHigh =7002,
    FileDescriptorLimit =7003,
    ThreadLimitExceeded =7004,
    
    // General system errors
    GeneralSystemError = 8000,
    NotImplemented = 8001,
    OperationCancelled = 8002,
    OperationTimeout = 8003,
    ResourceBusy = 8004,
    InvalidState = 8005,
    
    // Neuromodulation errors
    NoveltyCalculationFailed =9000,
    DopamineCalculationFailed =9001,
    CuriosityCalculationFailed =9002,
    PredictionErrorCalculationFailed =9003,
    
    // Memory system errors
    WorkingMemoryFull =9100,
    EpisodicMemoryFull =9101,
    AssociativeMemoryCollision =9102,
    MemoryIndexError =9103,
    
    // Cognition errors
    PlanningFailed =9200,
    ConceptFormationFailed =9201,
    AttentionAllocationFailed =9202,
    
    // Development errors
    DevelopmentStageInvalid =9300,
    CriticalPeriodViolated =9301,
    MaturationFailed =9302,
    
    // Performance errors
    PerformanceMonitorError =9400,
    CheckpointError =9401,
    OptimizationError =9402,
    
    // Success
    Success = 0
};

// Base exception class
class NLMError : public std::exception {
protected:
    ErrorSeverity severity_;
    ErrorCode code_;
    std::string message_;
    std::string context_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::unordered_map<std::string, std::string> details_;
    std::vector<std::string> stackTrace_;
    std::shared_ptr<NLMError> cause_;
    mutable std::string formattedMessage_;
    
public:
    NLMError(ErrorSeverity severity, ErrorCode code, const std::string& message,
             const std::string& context = "", const std::string& file = "", 
             int line = 0, const std::string& function = "")
        : severity_(severity), code_(code), message_(message), context_(context),
          file_(file), line_(line), function_(function), timestamp_(std::chrono::system_clock::now()) {
        addStackTrace();
    }
    
    virtual ~NLMError() noexcept = default;
    
    // Get formatted message
    virtual const char* what() const noexcept override {
        if (formattedMessage_.empty()) {
            formatMessage();
        }
        return formattedMessage_.c_str();
    }
    
    // Error information getters
    ErrorSeverity getSeverity() const { return severity_; }
    ErrorCode getCode() const { return code_; }
    const std::string& getMessage() const { return message_;} 
    const std::string& getContext() const { return context_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    // Details management
    void addDetail(const std::string& key, const std::string& value) {
        details_[key] = value;
    }
    
    std::string getDetail(const std::string& key) const {
        auto it = details_.find(key);
        return it != details_.end() ? it->second : "";
    }
    
    // Stack trace management
    void addStackTrace() {
        std::ostringstream oss;
        oss << "Error at " << file_ << ":" << line_ << " in " << function_;
        stackTrace_.push_back(oss.str());
    }
    
    const std::vector<std::string>& getStackTrace() const { return stackTrace_; }
    
    // Cause chain
    void setCause(std::shared_ptr<NLMError> cause) { cause_ = cause; }
    std::shared_ptr<NLMError> getCause() const { return cause_; }
    
    // Logging
    void log() const {
        Logger& logger = Logger::getGlobal();
        std::string severityStr;
        switch (severity_) {
            case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
            case ErrorSeverity::Info: severityStr = "INFO"; break;
            case ErrorSeverity::Warning: severityStr = "WARNING"; break;
            case ErrorSeverity::Error: severityStr = "ERROR"; break;
            case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
            case ErrorSeverity::Fatal: severityStr = "FATAL"; break;
        }
        
        logger.error(std::format("[{0}] Code {1}: {2}", severityStr, 
                                static_cast<int>(code_), what()));
        if (!context_.empty()) {
            logger.debug(std::format("Context: {0}", context_));
        }
        for (const auto& detail : details_) {
            logger.debug(std::format("Detail [{0}]: {1}", detail.first, detail.second));
        }
    }
    
    // Format message
    virtual void formatMessage() const {
        std::ostringstream oss;
        oss << "NLM Error: Code " << static_cast<int>(code_) << ": " << message_;
        if (!context_.empty()) {
            oss << " (Context: " << context_ << ")";
        }
        if (!file_.empty()) {
            oss << " at " << file_ << ":" << line_;
        }
        formattedMessage_ = oss.str();
    }
    
    // Static factory methods for different error types
    static std::shared_ptr<NLMError> createConfigurationError(ErrorCode code, 
        const std::string& message, const std::string& context = "", 
        const std::string& file = "", int line = 0, 
        const std::string& function = "") {
        auto error = std::make_shared<ConfigurationError>(code, message, context, file, line, function);
        error->log();
        return error;
    }
    
    static std::shared_ptr<NLMError> createFileSystemError(ErrorCode code,
        const std::string& message, const std::string& context = "",
        const std::string& file = "", int line = 0,
        const std::string& function = "") {
        auto error = std::make_shared<FileSystemError>(code, message, context, file, line, function);
        error->log();
        return error;
    }
    
    static std::shared_ptr<NLMError> createMemoryError(ErrorCode code,
        const std::string& message, const std::string& context = "",
        const std::string& file = "", int line = 0,
        const std::string& function = "") {
        auto error = std::make_shared<MemoryAllocationError>(code, message, context, file, line, function);
        error->log();
        return error;
    }
    
    static std::shared_ptr<NLMError> createSimulationError(ErrorCode code,
        const std::string& message, const std::string& context = "",
        const std::string& file = "", int line = 0,
        const std::string& function = "") {
        auto error = std::make_shared<SimulationError>(code, message, context, file, line, function);
        error->log();
        return error;
    }
    
    static std::shared_ptr<NLMError> createValidationError(ErrorCode code,
        const std::string& message, const std::string& context = "",
        const std::string& file = "", int line = 0,
        const std::string& function = "") {
        auto error = std::make_shared<ValidationError>(code, message, context, file, line, function);
        error->log();
        return error;
    }
    
    static std::shared_ptr<NLMError> createResourceError(ErrorCode code,
        const std::string& message, const std::string& context = "",
        const std::string& file = "", int line = 0,
        const std::string& function = "") {
        auto error = std::make_shared<ResourceExhaustionError>(code, message, context, file, line, function);
        error->log();
        return error;
    }
    
    static std::shared_ptr<NLMError> createSuccess() {
        return std::make_shared<SuccessError>();
    }
};

// Specific error classes
class ConfigurationError : public NLMError {
public:
    ConfigurationError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Error, code, message, context, file, line, function) {}
};

class FileSystemError : public NLMError {
public:
    FileSystemError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Error, code, message, context, file, line, function) {}
};

class MemoryAllocationError : public NLMError {
public:
    MemoryAllocationError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Critical, code, message, context, file, line, function) {}
};

class SimulationError : public NLMError {
public:
    SimulationError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Error, code, message, context, file, line, function) {}
};

class ValidationError : public NLMError {
public:
    ValidationError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Warning, code, message, context, file, line, function) {}
};

class ResourceExhaustionError : public NLMError {
public:
    ResourceExhaustionError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Critical, code, message, context, file, line, function) {}
};

class NeuromodulationError : public NLMError {
public:
    NeuromodulationError(ErrorCode code, const std::string& message,
        const std::string& context = "", const std::string& file = "",
        int line = 0, const std::string& function = "")
        : NLMError(ErrorSeverity::Error, code, message, context, file, line, function) {}
};

class SuccessError : public NLMError {
public:
    SuccessError() 
        : NLMError(ErrorSeverity::Info, ErrorCode::Success, "Operation completed successfully") {}
};

// Error manager for tracking and reporting
class ErrorManager {
private:
    std::vector<std::shared_ptr<NLMError>> errorHistory_;
    std::mutex mutex_;
    size_t maxHistorySize_ = 1000;
    
public:
    ErrorManager() = default;
    
    // Add error to history
    void addError(std::shared_ptr<NLMError> error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorHistory_.push_back(error);
        
        // Trim history if too large
        if (errorHistory_.size() > maxHistorySize_) {
            errorHistory_.erase(errorHistory_.begin(),
                             errorHistory_.begin() + (errorHistory_.size() - maxHistorySize_));
        }
        
        // Log error
        error->log();
    }
    
    // Get error statistics
    struct ErrorStats {
        size_t totalErrors = 0;
        std::unordered_map<ErrorCode, size_t> errorsByCode;
        std::unordered_map<ErrorSeverity, size_t> errorsBySeverity;
        std::unordered_map<std::string, size_t> errorsByContext;
        size_t recentErrors = 0; // Last 100 errors
        std::chrono::system_clock::time_point lastErrorTime;
    };
    
    ErrorStats getErrorStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        ErrorStats stats;
        
        for (const auto& error : errorHistory_) {
            stats.totalErrors++;
            stats.errorsByCode[error->getCode()]++;
            stats.errorsBySeverity[error->getSeverity()]++;
            if (!error->getContext().empty()) {
                stats.errorsByContext[error->getContext()]++;
            }
            stats.lastErrorTime = error->getTimestamp();
        }
        
        // Calculate recent errors (last 100)
        if (errorHistory_.size() > 100) {
            stats.recentErrors = 100;
        } else {
            stats.recentErrors = errorHistory_.size();
        }
        
        return stats;
    }
    
    // Clear error history
    void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        errorHistory_.clear();
    }
    
    // Get error history
    const std::vector<std::shared_ptr<NLMError>>& getErrorHistory() const {
        return errorHistory_;
    }
    
    // Check if there are any errors
    bool hasErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !errorHistory_.empty();
    }
    
    // Get latest error
    std::shared_ptr<NLMError> getLatestError() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (errorHistory_.empty()) {
            return nullptr;
        }
        return errorHistory_.back();
    }
    
    // Filter errors by severity
    std::vector<std::shared_ptr<NLMError>> getErrorsBySeverity(ErrorSeverity severity) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<NLMError>> filtered;
        for (const auto& error : errorHistory_) {
            if (error->getSeverity() == severity) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    // Filter errors by code
    std::vector<std::shared_ptr<NLMError>> getErrorsByCode(ErrorCode code) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<NLMError>> filtered;
        for (const auto& error : errorHistory_) {
            if (error->getCode() == code) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    // Get errors by time range
    std::vector<std::shared_ptr<NLMError>> getErrorsByTimeRange(
        std::chrono::system_clock::time_point start, 
        std::chrono::system_clock::time_point end) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<NLMError>> filtered;
        for (const auto& error : errorHistory_) {
            if (error->getTimestamp() >= start && error->getTimestamp() <= end) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
};

// Global error manager instance
inline ErrorManager& getGlobalErrorManager() {
    static ErrorManager manager;
    return manager;
}

// Validation helper functions
class ValidationHelper {
public:
    // Parameter validation
    static bool validateParameter(const std::string& name, double value,
        double min, double max, const std::string& context = "") {
        if (value < min || value > max) {
            auto error = NLMError::createValidationError(
                ErrorCode::RangeValidationFailed,
                std::format("Parameter '{}' out of range: {} (expected between {} and {})",
                           name, value, min, max),
                context
            );
            getGlobalErrorManager().addError(error);
            return false;
        }
        return true;
    }
    
    static bool validateParameter(const std::string& name, int value,
        int min, int max, const std::string& context = "") {
        if (value < min || value > max) {
            auto error = NLMError::createValidationError(
                ErrorCode::RangeValidationFailed,
                std::format("Parameter '{}' out of range: {} (expected between {} and {})",
                           name, value, min, max),
                context
            );
            getGlobalErrorManager().addError(error);
            return false;
        }
        return true;
    }
    
    static bool validateParameter(const std::string& name, size_t value,
        size_t min, size_t max, const std::string& context = "") {
        if (value < min || value > max) {
            auto error = NLMError::createValidationError(
                ErrorCode::RangeValidationFailed,
                std::format("Parameter '{}' out of range: {} (expected between {} and {})",
                           name, value, min, max),
                context
            );
            getGlobalErrorManager().addError(error);
            return false;
        }
        return true;
    }
    
    // File existence validation
    static bool validateFileExists(const std::string& filepath, 
        const std::string& context = "") {
        std::ifstream file(filepath);
        if (!file.good()) {
            auto error = NLMError::createFileSystemError(
                ErrorCode::FileNotFound,
                std::format("File not found: {}", filepath),
                context
            );
            getGlobalErrorManager().addError(error);
            return false;
        }
        return true;
    }
    
    // Memory allocation validation
    static bool validateMemoryAllocation(size_t size, 
        const std::string& context = "") {
        if (size == 0) {
            auto error = NLMError::createMemoryError(
                ErrorCode::MemoryAllocationFailed,
                std::format("Invalid memory allocation size: {}", size),
                context
            );
            getGlobalErrorManager().addError(error);
            return false;
        }
        return true;
    }
    
    // Configuration validation
    static bool validateConfig(const std::string& key, 
        const ConfigValue& value, const std::string& context = "") {
        // Basic type validation
        if (key == "neuron_count" && std::holds_alternative<size_t>(value)) {
            size_t count = std::get<size_t>(value);
            if (!validateParameter("neuron_count", count, 1, 1000000, context)) {
                return false;
            }
        } else if (key == "region_count" && std::holds_alternative<size_t>(value)) {
            size_t count = std::get<size_t>(value);
            if (!validateParameter("region_count", count, 1, 1000, context)) {
                return false;
            }
        } else if (key == "connection_probability" && std::holds_alternative<double>(value)) {
            double prob = std::get<double>(value);
            if (!validateParameter("connection_probability", prob, 0.0, 1.0, context)) {
                return false;
            }
        } else if (key == "simulation_timestep" && std::holds_alternative<double>(value)) {
            double timestep = std::get<double>(value);
            if (!validateParameter("simulation_timestep", timestep, 1e-6, 1.0, context)) {
                return false;
            }
        }
        return true;
    }
    
    // Error recovery helpers
    static bool canRecover(const std::shared_ptr<NLMError>& error) {
        if (!error) return true;
        
        switch (error->getCode()) {
            case ErrorCode::FileNotFound:
            case ErrorCode::FilePermissionDenied:
                return false; // Cannot recover from file system errors
                
            case ErrorCode::MemoryAllocationFailed:
            case ErrorCode::MemoryOutOfMemory:
                return false; // Cannot recover from memory errors
                
            case ErrorCode::Timeout:
            case ErrorCode::NetworkUnreachable:
                return true; // Can retry after a delay
                
            case ErrorCode::ResourceLimitExceeded:
                return true; // Can try to reduce resource usage
                
            case ErrorCode::ConfigurationError:
            case ErrorCode::ValidationError:
                return false; // Cannot recover from configuration/validation errors
                
            case ErrorCode::SimulationStepFailed:
                return true; // Can try to reset and continue
                
            default:
                return true; // Try to recover from unknown errors
        }
    }
    
    static bool attemptRecovery(const std::shared_ptr<NLMError>& error) {
        if (!canRecover(error)) {
            return false;
        }
        
        // Attempt recovery based on error type
        switch (error->getCode()) {
            case ErrorCode::Timeout:
            case ErrorCode::NetworkUnreachable:
                // Wait and retry (simulated)
                return true;
                
            case ErrorCode::ResourceLimitExceeded:
                // Try to reduce resource usage
                return true;
                
            case ErrorCode::SimulationStepFailed:
                // Reset simulation state and try again
                return true;
                
            default:
                return true;
        }
    }
    
    // Error reporting
    static void reportErrors() {
        ErrorManager& manager = getGlobalErrorManager();
        auto stats = manager.getErrorStats();
        
        std::cout << "=== NLM Error Report ===" << std::endl;
        std::cout << "Total Errors: " << stats.totalErrors << std::endl;
        std::cout << "Recent Errors (last 100): " << stats.recentErrors << std::endl;
        
        if (stats.totalErrors > 0) {
            std::cout << "\nErrors by Code:" << std::endl;
            for (const auto& pair : stats.errorsByCode) {
                std::cout << "  Code " << static_cast<int>(pair.first) 
                         << ": " << pair.second << std::endl;
            }
            
            std::cout << "\nErrors by Severity:" << std::endl;
            for (const auto& pair : stats.errorsBySeverity) {
                std::string severityStr;
                switch (pair.first) {
                    case ErrorSeverity::Debug: severityStr = "Debug"; break;
                    case ErrorSeverity::Info: severityStr = "Info"; break;
                    case ErrorSeverity::Warning: severityStr = "Warning"; break;
                    case ErrorSeverity::Error: severityStr = "Error"; break;
                    case ErrorSeverity::Critical: severityStr = "Critical"; break;
                    case ErrorSeverity::Fatal: severityStr = "Fatal"; break;
                }
                std::cout << "  " << severityStr << ": " << pair.second << std::endl;
            }
        }
    }
    
    // Generate error report file
    static bool generateErrorReport(const std::string& filename) {
        ErrorManager& manager = getGlobalErrorManager();
        auto stats = manager.getErrorStats();
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            auto error = NLMError::createFileSystemError(
                ErrorCode::FileWriteError,
                std::format("Failed to create error report file: {}", filename)
            );
            manager.addError(error);
            return false;
        }
        
        file << "=== NLM Error Report ===" << std::endl;
        file << "Generated at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
             << std::endl;
        file << "Total Errors: " << stats.totalErrors << std::endl;
        file << "Recent Errors (last 100): " << stats.recentErrors << std::endl;
        
        if (stats.totalErrors > 0) {
            file << "\nDetailed Error History:" << std::endl;
            for (const auto& error : manager.getErrorHistory()) {
                file << std::format("  [{0}] Code {1}: {2}",
                                   static_cast<int>(error->getSeverity()),
                                   static_cast<int>(error->getCode()),
                                   error->what()) << std::endl;
                if (!error->getContext().empty()) {
                    file << "      Context: " << error->getContext() << std::endl;
                }
            }
        }
        
        file.close();
        return true;
    }
};

// Error recovery system with retry logic
class ErrorRecoverySystem {
private:
    std::unordered_map<ErrorCode, int> retryCounts_;
    std::chrono::system_clock::time_point lastRetryTime_;
    std::mutex mutex_;
    
public:
    ErrorRecoverySystem() = default;
    
    // Attempt to recover from error with retry logic
    bool attemptRecovery(std::shared_ptr<NLMError> error) {
        if (!ValidationHelper::canRecover(error)) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check retry count
        int maxRetries = getMaxRetries(error->getCode());
        int currentRetry = ++retryCounts_[error->getCode()];
        
        if (currentRetry > maxRetries) {
            return false;
        }
        
        // Check time since last retry
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastRetryTime_);
        
        if (duration.count() < getRetryDelay(error->getCode())) {
            return false;
        }
        
        // Update last retry time
        lastRetryTime_ = now;
        
        // Attempt recovery based on error type
        return attemptSpecificRecovery(error, currentRetry);
    }
    
    // Get maximum retry count for error code
    int getMaxRetries(ErrorCode code) const {
        static const std::unordered_map<ErrorCode, int> maxRetries = {
            {ErrorCode::Timeout, 3},
            {ErrorCode::NetworkUnreachable, 5},
            {ErrorCode::FileNotFound, 2},
            {ErrorCode::ResourceLimitExceeded, 3},
            {ErrorCode::SimulationStepFailed, 2},
            {ErrorCode::FileReadError, 2},
            {ErrorCode::FileWriteError, 2},
            {ErrorCode::MemoryAllocationFailed, 1},
            {ErrorCode::MemoryOutOfMemory, 0},
            {ErrorCode::ConfigurationError, 0},
            {ErrorCode::ValidationError, 0},
            {ErrorCode::ConnectionFailed, 4},
            {ErrorCode::ProtocolError, 2},
        };
        
        auto it = maxRetries.find(code);
        return it != maxRetries.end() ? it->second : 1;
    }
    
    // Get retry delay in seconds for error code
    int getRetryDelay(ErrorCode code) const {
        static const std::unordered_map<ErrorCode, int> retryDelays = {
            {ErrorCode::Timeout, 1},
            {ErrorCode::NetworkUnreachable, 2},
            {ErrorCode::FileNotFound, 0},
            {ErrorCode::ResourceLimitExceeded, 5},
            {ErrorCode::SimulationStepFailed, 1},
            {ErrorCode::FileReadError, 1},
            {ErrorCode::FileWriteError, 1},
            {ErrorCode::MemoryAllocationFailed, 0},
            {ErrorCode::MemoryOutOfMemory, 0},
            {ErrorCode::ConfigurationError, 0},
            {ErrorCode::ValidationError, 0},
            {ErrorCode::ConnectionFailed, 1},
            {ErrorCode::ProtocolError, 1},
        };
        
        auto it = retryDelays.find(code);
        return it != retryDelays.end() ? it->second : 1;
    }
    
    // Attempt specific recovery for error
    bool attemptSpecificRecovery(std::shared_ptr<NLMError> error, int retryCount) {
        switch (error->getCode()) {
            case ErrorCode::Timeout:
            case ErrorCode::NetworkUnreachable:
                // Simulate network recovery
                return true;
                
            case ErrorCode::FileNotFound:
                // Simulate file recreation
                return true;
                
            case ErrorCode::ResourceLimitExceeded:
                // Simulate resource cleanup
                return true;
                
            case ErrorCode::SimulationStepFailed:
                // Simulate simulation reset
                return true;
                
            case ErrorCode::FileReadError:
                // Simulate file retry
                return true;
                
            case ErrorCode::FileWriteError:
                // Simulate write retry
                return true;
                
            case ErrorCode::MemoryAllocationFailed:
                // Simulate memory cleanup and retry
                return true;
                
            case ErrorCode::ConnectionFailed:
                // Simulate connection retry
                return true;
                
            default:
                return true;
        }
    }
    
    // Reset retry counts for error
    void resetRetryCount(ErrorCode code) {
        std::lock_guard<std::mutex> lock(mutex_);
        retryCounts_[code] = 0;
    }
    
    // Reset all retry counts
    void resetAllRetryCounts() {
        std::lock_guard<std::mutex> lock(mutex_);
        retryCounts_.clear();
    }
};

// Global error recovery system instance
inline ErrorRecoverySystem& getGlobalErrorRecoverySystem() {
    static ErrorRecoverySystem recoverySystem;
    return recoverySystem;
}

// Error handling macros for common operations
#define NLM_TRY_BLOCK try

#define NLM_CATCH_BLOCK(handleErrorFunc) catch (const std::exception& e) { \
    auto error = NLMError::createErrorFromException(e); \
    if (handleErrorFunc(error)) { \
        return false; \
    } \
    throw; \
}

#define NLM_VALIDATE(expr, errorCode, message) \
    if (!(expr)) { \
        auto error = NLMError::createValidationError(errorCode, message); \
        getGlobalErrorManager().addError(error); \
        return false; \
    }

#define NLM_CHECK_CONDITION(condition, errorCode, message) \
    if (!(condition)) { \
        auto error = NLMError::createError(errorCode, message); \
        getGlobalErrorManager().addError(error); \
        return false; \
    }

#define NLM_THROW_ERROR(errorCode, message) \
    throw NLMError::createError(errorCode, message)

#define NLM_THROW_CONFIGURATION_ERROR(errorCode, message) \
    throw NLMError::createConfigurationError(errorCode, message)

#define NLM_THROW_FILESYSTEM_ERROR(errorCode, message) \
    throw NLMError::createFileSystemError(errorCode, message)

#define NLM_THROW_MEMORY_ERROR(errorCode, message) \
    throw NLMError::createMemoryError(errorCode, message)

#define NLM_THROW_SIMULATION_ERROR(errorCode, message) \
    throw NLMError::createSimulationError(errorCode, message)

#define NLM_THROW_VALIDATION_ERROR(errorCode, message) \
    throw NLMError::createValidationError(errorCode, message)

// Exception to error conversion
inline std::shared_ptr<NLMError> NLMError::createErrorFromException(const std::exception& e) {
    auto error = NLMError::createGeneralError(ErrorCode::GeneralSystemError, 
        std::format("Exception: {}", e.what()));
    error->addDetail("exception_type", typeid(e).name());
    return error;
}

inline std::shared_ptr<NLMError> NLMError::createError(ErrorCode code, 
    const std::string& message, const std::string& context,
    const std::string& file, int line, const std::string& function) {
    auto error = std::make_shared<NLMError>(ErrorSeverity::Error, code, message, context, file, line, function);
    error->log();
    getGlobalErrorManager().addError(error);
    return error;
}

inline std::shared_ptr<NLMError> NLMError::createGeneralError(ErrorCode code,
    const std::string& message, const std::string& context,
    const std::string& file, int line, const std::string& function) {
    auto error = std::make_shared<NLMError>(ErrorSeverity::Error, code, message, context, file, line, function);
    error->log();
    getGlobalErrorManager().addError(error);
    return error;
}

// Enhanced logging integration
class ErrorAwareLogger : public ILogger {
private:
    std::shared_ptr<ILogger> baseLogger_;
    std::mutex mutex_;
    
public:
    ErrorAwareLogger(std::shared_ptr<ILogger> logger) : baseLogger_(logger) {}
    
    void log(const LogEntry& entry) override {
        std::lock_guard<std::mutex> lock(mutex_);
        baseLogger_->log(entry);
    }
    
    void setLevel(LogLevel level) override {
        baseLogger_->setLevel(level);
    }
    
    LogLevel getLevel() const override {
        return baseLogger_->getLevel();
    }
    
    // Log error with context
    void logError(const std::shared_ptr<NLMError>& error) {
        if (!error) return;
        
        LogEntry entry;
        entry.level = LogLevel::Error;
        entry.message = std::format("[Code {}] {}", static_cast<int>(error->getCode()), error->what());
        entry.file = error->getFile();
        entry.line = error->getLine();
        entry.function = error->getFunction();
        entry.timestamp = error->getTimestamp();
        
        log(entry);
    }
};

} // namespace nlm