#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    Configuration,
    FileSystem,
    Memory,
    Neural,
    Integration,
    Performance,
    System,
    Validation
};

// Error severity levels
enum class ErrorSeverity {
    Warning,
    Error,
    Critical,
    Fatal
};

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCategory category = ErrorCategory::System, 
                 ErrorSeverity severity = ErrorSeverity::Error)
        : std::runtime_error(message), category_(category), severity_(severity), 
          timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMException() = default;
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    virtual std::string getDetails() const { return what(); }
    
    // Convert exception to string for logging
    virtual std::string toString() const {
        std::string severityStr;
        switch (severity_) {
            case ErrorSeverity::Warning: severityStr = "WARNING"; break;
            case ErrorSeverity::Error: severityStr = "ERROR"; break;
            case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
            case ErrorSeverity::Fatal: severityStr = "FATAL"; break;
        }
        
        std::string categoryStr;
        switch (category_) {
            case ErrorCategory::Configuration: categoryStr = "CONFIGURATION"; break;
            case ErrorCategory::FileSystem: categoryStr = "FILESYSTEM"; break;
            case ErrorCategory::Memory: categoryStr = "MEMORY"; break;
            case ErrorCategory::Neural: categoryStr = "NEURAL"; break;
            case ErrorCategory::Integration: categoryStr = "INTEGRATION"; break;
            case ErrorCategory::Performance: categoryStr = "PERFORMANCE"; break;
            case ErrorCategory::System: categoryStr = "SYSTEM"; break;
            case ErrorCategory::Validation: categoryStr = "VALIDATION"; break;
        }
        
        return "[" + severityStr + "] [" + categoryStr + "] " + what();
    }
    
protected:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::chrono::system_clock::time_point timestamp_;
};

// Configuration-specific exceptions
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::Configuration, severity) {}
};

class FileSystemException : public NLMException {
public:
    FileSystemException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::FileSystem, severity) {}
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical)
        : NLMException(message, ErrorCategory::Memory, severity) {}
};

class NeuralException : public NLMException {
public:
    NeuralException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::Neural, severity) {}
};

class IntegrationException : public NLMException {
public:
    IntegrationException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, ErrorCategory::Integration, severity) {}
};

class PerformanceException : public NLMException {
public:
    PerformanceException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, ErrorCategory::Performance, severity) {}
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, ErrorCategory::Validation, severity) {}
};

// Error handler class for collecting and reporting errors
class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();
    
    // Singleton pattern
    static ErrorHandler& getInstance();
    
    // Add an error
    void addError(std::shared_ptr<NLMException> error);
    
    // Check if any errors have occurred
    bool hasErrors() const { return !errors_.empty(); }
    
    // Get all errors
    const std::vector<std::shared_ptr<NLMException>>& getErrors() const { return errors_; }
    
    // Clear all errors
    void clearErrors();
    
    // Get errors by category
    std::vector<std::shared_ptr<NLMException>> getErrorsByCategory(ErrorCategory category) const;
    
    // Get errors by severity
    std::vector<std::shared_ptr<NLMException>> getErrorsBySeverity(ErrorSeverity severity) const;
    
    // Report all errors to logger
    void reportErrors();
    
    // Check for specific error type
    template<typename T>
    bool hasErrorOfType() const {
        for (const auto& error : errors_) {
            if (std::dynamic_pointer_cast<T>(error)) {
                return true;
            }
        }
        return false;
    }
    
    // Get count of errors by category
    size_t getErrorCount(ErrorCategory category) const;
    
    // Get count of errors by severity
    size_t getErrorCount(ErrorSeverity severity) const;
    
    // Set error reporting callback
    void setErrorReportCallback(std::function<void(std::shared_ptr<NLMException>)> callback);
    
    // Check if system should continue on errors
    bool shouldContinueOnError() const { return continue_on_error_; }
    void setShouldContinueOnError(bool continue) { continue_on_error_ = continue; }
    
    // Get summary of errors
    std::string getErrorSummary() const;
    
private:
    std::vector<std::shared_ptr<NLMException>> errors_;
    std::function<void(std::shared_ptr<NLMException>)> errorReportCallback_;
    bool continue_on_error_;
};}
