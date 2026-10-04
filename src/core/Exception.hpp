// Comprehensive error class with context and recovery information
// Enhanced error handling framework for NLM neural simulation project

#ifndef NLM_ERROR_HPP
#define NLM_ERROR_HPP

#include <string>
#include <map>
#include <chrono>
#include <unordered_map>
#include <any>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include "ErrorTypes.hpp"
#include "ErrorReporter.hpp"
#include "ErrorHandlingUtils.hpp"

namespace nlm {

// Base error class with comprehensive context and recovery information
class Error {
public:
    Error(const std::string& message, ErrorSeverity severity = ErrorSeverity::ERROR,
          const std::string& context = "", ErrorCategory category = ErrorCategory::SYSTEM,
          const ErrorMetadata& metadata = ErrorMetadata(),
          RecoveryAction recovery = RecoveryAction::NONE)
        : message_(message), severity_(severity), context_(context),
          category_(category), metadata_(metadata), 
          timestamp_(std::chrono::system_clock::now()),
          recoveryAction_(recovery) {
        updateHash();
    }
    
    virtual ~Error() = default;
    
    // Error identification
    const std::string& what() const { return message_; }
    const std::string& id() const { return errorId_; }
    ErrorSeverity severity() const { return severity_; }
    const std::string& context() const { return context_; }
    ErrorCategory category() const { return category_; }
    const ErrorMetadata& metadata() const { return metadata_; }
    const auto& timestamp() const { return timestamp_; }
    
    // Error hash for deduplication
    const std::string& hash() const { return errorHash_; }
    
    // Severity and category strings
    static std::string severityToString(ErrorSeverity severity);
    static std::string categoryToString(ErrorCategory category);
    
    // Formatted error message with full context
    virtual std::string format() const {
        std::ostringstream oss;
        oss << "[" << severityToString(severity_) << "]";
        oss << " [" << categoryToString(category_) << "]";
        oss << " [" << id() << "]";
        
        if (!context_.empty()) {
            oss << " [" << context_ << "]";
        }
        
        oss << " " << message_;
        
        if (!metadata_.operation.empty()) {
            oss << " (" << metadata_.operation << ")";
        }
        
        if (!metadata_.component.empty()) {
            oss << " [" << metadata_.component << "]";
        }
        
        if (!metadata_.file.empty()) {
            oss << " File: " << metadata_.file;
            if (metadata_.line > 0) {
                oss << ":" << metadata_.line;
            }
        }
        
        return oss.str();
    }
    
    // Check if error is recoverable
    virtual bool isRecoverable() const {
        return recoveryAction_ != RecoveryAction::NONE && 
               severity_ <= ErrorSeverity::ERROR;
    }
    
    // Get recovery action
    RecoveryAction getRecoveryAction() const { return recoveryAction_; }
    
    // Perform recovery with error handling
    virtual bool recover() {
        if (!isRecoverable()) {
            ErrorReporter::getInstance().log(this, ErrorLevel::WARNING, 
                                            "Attempted recovery on non-recoverable error");
            return false;
        }
        
        ErrorReporter::getInstance().log(this, ErrorLevel::INFO, 
                                        "Starting recovery for error");
        
        bool success = performRecovery();
        
        if (success) {
            ErrorReporter::getInstance().log(this, ErrorLevel::INFO, 
                                            "Recovery completed successfully");
        } else {
            ErrorReporter::getInstance().log(this, ErrorLevel::ERROR, 
                                            "Recovery failed");
        }
        
        return success;
    }
    
    // Get error statistics
    static size_t getTotalErrorCount() { return totalErrorCount_.load(); }
    static size_t getRecoverableErrorCount() { return recoverableErrorCount_.load(); }
    
    // Get error summary for reporting
    static std::string getGlobalErrorSummary() {
        std::lock_guard<std::mutex> lock(summaryMutex_);
        return ErrorReporter::getInstance().getGlobalSummary();
    }
    
    // Set error reporter instance
    static void setErrorReporter(ErrorReporter* reporter) {
        ErrorReporter::setInstance(reporter);
    }
    
protected:
    virtual bool performRecovery() {
        // Default implementation - override in derived classes
        return false;
    }
    
    void updateHash() {
        std::hash<std::string> hasher;
        std::string hashInput = message_ + context_ + std::to_string(static_cast<int>(severity_));
        errorHash_ = std::to_string(hasher(hashInput));
        errorId_ = generateErrorId();
    }
    
    std::string generateErrorId() const {
        // Generate unique error ID
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        
        std::hash<std::string> hasher;
        std::string seed = std::to_string(timestamp) + context_ + message_;
        return "ERR_" + std::to_string(hasher(seed));
    }
    
    std::string message_;
    std::string errorId_;
    std::string errorHash_;
    ErrorSeverity severity_;
    std::string context_;
    ErrorCategory category_;
    ErrorMetadata metadata_;
    std::chrono::system_clock::time_point timestamp_;
    RecoveryAction recoveryAction_;
    
    // Global error statistics
    static std::atomic<size_t> totalErrorCount_;
    static std::atomic<size_t> recoverableErrorCount_;
    static std::mutex summaryMutex_;
};

// Specialized error types for neural simulation components

// Null pointer error with neural context
class NeuralNullPointerError : public Error {
public:
    NeuralNullPointerError(const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Null pointer dereference in neural system", ErrorSeverity::CRITICAL, context, 
                ErrorCategory::NULL_POINTER, metadata, RecoveryAction::REINITIALIZE) {}
    
    bool performRecovery() override {
        // Reset neural state, reinitialize components
        return true;
    }
};

// Memory allocation error for neural systems
class NeuralMemoryAllocationError : public Error {
public:
    NeuralMemoryAllocationError(size_t requestedSize, const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Memory allocation failed: " + std::to_string(requestedSize) + " bytes", 
                ErrorSeverity::ERROR, context, ErrorCategory::MEMORY_ALLOCATION, metadata, 
                RecoveryAction::FALLBACK) {}
}; 

// Neural computation error with retry capability
class NeuralComputationError : public Error {
public:
    NeuralComputationError(const std::string& message, const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Neural computation error: " + message, ErrorSeverity::ERROR, context,
                ErrorCategory::NEURAL, metadata, RecoveryAction::RETRY) {}
    
    bool performRecovery() override {
        // Reduce computation complexity, use simpler model
        return true;
    }
};

// Integration error between neural subsystems
class NeuralIntegrationError : public Error {
public:
    NeuralIntegrationError(const std::string& component, const std::string& message, const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Integration error in " + component + ": " + message, ErrorSeverity::CRITICAL, context,
                ErrorCategory::INTEGRATION_FAILURE, metadata, RecoveryAction::SHUTDOWN) {}
};

// System state validation error
class NeuralSystemStateError : public Error {
public:
    NeuralSystemStateError(const std::string& component, const std::string& expected, const std::string& actual, 
                          const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("System state mismatch in " + component + ": expected '" + expected + "', got '" + actual + "'", 
                ErrorSeverity::WARNING, context, ErrorCategory::SYSTEM_STATE, metadata, 
                RecoveryAction::CORRECTION) {}
    
    bool performRecovery() override {
        // Attempt to correct the state
        return true;
    }
};

// Plasticity rule error with correction
class NeuralPlasticityError : public Error {
public:
    NeuralPlasticityError(const std::string& rule, const std::string& message, const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Plasticity rule error in " + rule + ": " + message, ErrorSeverity::WARNING, context,
                ErrorCategory::PLASTICITY, metadata, RecoveryAction::CORRECTION) {}
    
    bool performRecovery() override {
        // Clamp plasticity values to valid ranges
        return true;
    }
};

// Neuromodulation error with fallback
class NeuralNeuromodulationError : public Error {
public:
    NeuralNeuromodulationError(const std::string& system, const std::string& message, const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Neuromodulation error in " + system + ": " + message, ErrorSeverity::ERROR, context,
                ErrorCategory::NEUROMODULATION, metadata, RecoveryAction::FALLBACK) {}
};

// Memory management error with retry
class NeuralMemoryManagementError : public Error {
public:
    NeuralMemoryManagementError(const std::string& operation, const std::string& message, const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Memory management error in " + operation + ": " + message, ErrorSeverity::ERROR, context,
                ErrorCategory::MEMORY, metadata, RecoveryAction::RETRY) {}
};

// Neural simulation specific error for boundary violations
class NeuralBoundaryError : public Error {
public:
    NeuralBoundaryError(const std::string& component, const std::string& operation, const std::string& message, 
                       const std::string& context = "", const ErrorMetadata& metadata = ErrorMetadata())
        : Error("Boundary violation in " + component + "::" + operation + ": " + message, 
                ErrorSeverity::ERROR, context, ErrorCategory::BOUNDARY_VIOLATION, metadata,
                RecoveryAction::CORRECTION) {}
    
    bool performRecovery() override {
        // Clamp values to valid ranges
        return true;
    }
};

// Error aggregator for batch processing and reporting
class ErrorAggregator {
public:
    void addError(std::shared_ptr<Error> error) {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.push_back(error);
        
        if (error->severity() >= ErrorSeverity::ERROR) {
            criticalErrors_.push_back(error);
        }
        
        // Update category counts
        categoryCounts_[error->category()]++;
        
        // Update statistics
        Error::getTotalErrorCount()++;
        if (error->isRecoverable()) {
            Error::getRecoverableErrorCount()++;
        }
        
        // Add to error reporter for logging
        if (ErrorReporter::getInstance()) {
            ErrorReporter::getInstance()->addError(error);
        }
    }
    
    size_t getTotalErrorCount() const { return errors_.size(); }
    size_t getCriticalErrorCount() const { return criticalErrors_.size(); }
    
    std::vector<std::shared_ptr<Error>> getErrors() const { return errors_; }
    std::vector<std::shared_ptr<Error>> getCriticalErrors() const { return criticalErrors_; }
    
    std::map<ErrorCategory, size_t> getCategoryCounts() const { return categoryCounts_; }
    
    bool hasErrors() const { return !errors_.empty(); }
    bool hasCriticalErrors() const { return !criticalErrors_.empty(); }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        errors_.clear();
        criticalErrors_.clear();
        categoryCounts_.clear();
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Error Summary:" << std::endl;
        oss << "Total errors: " << getTotalErrorCount() << std::endl;
        oss << "Critical errors: " << getCriticalErrorCount() << std::endl;
        
        oss << "By category:" << std::endl;
        for (const auto& pair : categoryCounts_) {
            oss << "  " << Error::categoryToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        return oss.str();
    }
    
    // Process all recoverable errors
    std::vector<std::shared_ptr<Error>> processRecoverableErrors() {
        std::vector<std::shared_ptr<Error>> recoveredErrors;
        
        for (auto& error : errors_) {
            if (error->isRecoverable()) {
                if (error->recover()) {
                    recoveredErrors.push_back(error);
                }
            }
        }
        
        // Remove processed errors
        for (const auto& recovered : recoveredErrors) {
            errors_.erase(std::remove(errors_.begin(), errors_.end(), recovered), errors_.end());
            criticalErrors_.erase(std::remove(criticalErrors_.begin(), criticalErrors_.end(), recovered), criticalErrors_.end());
            categoryCounts_[recovered->category()]--;
        }
        
        return recoveredErrors;
    }
    
private:
    std::vector<std::shared_ptr<Error>> errors_;
    std::vector<std::shared_ptr<Error>> criticalErrors_;
    std::map<ErrorCategory, size_t> categoryCounts_;
    mutable std::mutex mutex_;
};

} // namespace nlm

#endif // NLM_ERROR_HPP
