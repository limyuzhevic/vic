// NLM Error Handling System
// Comprehensive error handling for NLM (熙然) neural simulation framework

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <functional>
#include "Logger.hpp"

namespace nlm {

// Error categories for NLM
enum class ErrorCategory {
    Configuration,
    Memory,
    NeuralDynamics,
    Plasticity,
    MemorySystem,
    PredictionSystem,
    CognitiveSystem,
    DevelopmentSystem,
    Neuromodulation,
    WorldSimulation,
    AgentInterface,
    FileIO,
    Integration,
    Performance,
    Unknown
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Detailed error information
struct ErrorInfo {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> stackTrace;
    std::map<std::string, std::string> context;
    
    ErrorInfo(ErrorCategory cat, ErrorSeverity sev, const std::string& msg,
              const std::string& file = "", int line = 0,
              const std::string& function = "")
        : category(cat), severity(sev), message(msg), file(file), line(line),
          function(function), timestamp(std::chrono::system_clock::now()) {}
};

// Custom exception classes
class NLMError : public std::runtime_error {
public:
    NLMError(ErrorCategory category, ErrorSeverity severity,
             const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : std::runtime_error(message), category_(category), severity_(severity),
          file_(file), line_(line), function_(function) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "NLM Error: " << what();
        if (!file_.empty()) {
            oss << " (" << file_;
            if (line_ > 0) oss << ":" << line_;
            oss << ")";
        }
        if (!function_.empty()) {
            oss << " in " << function_;
        }
        oss << " [" << static_cast<int>(category_) << "]";
        return oss.str();
    }
    
private:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual void handleWarning(const ErrorInfo& warning) = 0;
    virtual void setSeverityThreshold(ErrorSeverity threshold) = 0;
    virtual ErrorSeverity getSeverityThreshold() const = 0;
    virtual void addContext(const std::string& key, const std::string& value) = 0;
    virtual std::map<std::string, std::string> getContext() const = 0;
};

// Default error handler implementation
class DefaultErrorHandler : public IErrorHandler {
public:
    DefaultErrorHandler() : severityThreshold_(ErrorSeverity::Error) {}
    
    void handleError(const ErrorInfo& error) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string logMessage = error.message;
        if (!error.file.empty()) {
            logMessage += " (" + error.file;
            if (error.line > 0) logMessage += ":" + std::to_string(error.line);
            logMessage += ")";
        }
        
        if (error.severity >= severityThreshold_) {
            Logger::getGlobal().log(error.severity == ErrorSeverity::Critical || 
                                  error.severity == ErrorSeverity::Fatal ? 
                                  LogLevel::Critical : LogLevel::Error,
                                  "[" + errorCategoryToString(error.category) + "] " + logMessage,
                                  error.file, error.line, error.function);
        }
    }
    
    void handleWarning(const ErrorInfo& warning) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string logMessage = warning.message;
        if (!warning.file.empty()) {
            logMessage += " (" + warning.file;
            if (warning.line > 0) logMessage += ":" + std::to_string(warning.line);
            logMessage += ")";
        }
        
        Logger::getGlobal().log(LogLevel::Warning, 
                               "[WARN] " + logMessage,
                               warning.file, warning.line, warning.function);
    }
    
    void setSeverityThreshold(ErrorSeverity threshold) override {
        std::lock_guard<std::mutex> lock(mutex_);
        severityThreshold_ = threshold;
    }
    
    ErrorSeverity getSeverityThreshold() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return severityThreshold_;
    }
    
    void addContext(const std::string& key, const std::string& value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        context_[key] = value;
    }
    
    std::map<std::string, std::string> getContext() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return context_;
    }
    
    void clearContext() {
        std::lock_guard<std::mutex> lock(mutex_);
        context_.clear();
    }
    
private:
    std::string errorCategoryToString(ErrorCategory category) const {
        switch (category) {
            case ErrorCategory::Configuration: return "CONFIG";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::NeuralDynamics: return "NEURAL";
            case ErrorCategory::Plasticity: return "PLASTICITY";
            case ErrorCategory::MemorySystem: return "MEM_SYS";
            case ErrorCategory::PredictionSystem: return "PREDICT";
            case ErrorCategory::CognitiveSystem: return "COGNITIVE";
            case ErrorCategory::DevelopmentSystem: return "DEVELOPMENT";
            case ErrorCategory::Neuromodulation: return "NEUROMOD";
            case ErrorCategory::WorldSimulation: return "WORLD";
            case ErrorCategory::AgentInterface: return "AGENT";
            case ErrorCategory::FileIO: return "FILEIO";
            case ErrorCategory::Integration: return "INTEGRATION";
            case ErrorCategory::Performance: return "PERFORMANCE";
            case ErrorCategory::Unknown: return "UNKNOWN";
        }
        return "UNKNOWN";
    }
    
    mutable std::mutex mutex_;
    std::map<std::string, std::string> context_;
    ErrorSeverity severityThreshold_;
};

// Global error handling functions
namespace ErrorHandling {
    // Global error handler instance
    std::shared_ptr<IErrorHandler> globalErrorHandler = std::make_shared<DefaultErrorHandler>();
    
    // Error convenience functions
    template<typename... Args>
    void throwError(ErrorCategory category, ErrorSeverity severity,
                   const std::string& message, Args&&... args) {
        std::ostringstream oss;
        oss << message;
        // Format additional arguments
        std::vector<std::string> formattedArgs = {std::forward<Args>(args)...};
        for (const auto& arg : formattedArgs) {
            oss << " " << arg;
        }
        
        ErrorInfo error(category, severity, oss.str(), __FILE__, __LINE__, __func__);
        globalErrorHandler->handleError(error);
        throw NLMError(category, severity, oss.str(), __FILE__, __LINE__, __func__);
    }
    
    template<typename... Args>
    void reportError(ErrorCategory category, ErrorSeverity severity,
                    const std::string& message, Args&&... args) {
        std::ostringstream oss;
        oss << message;
        std::vector<std::string> formattedArgs = {std::forward<Args>(args)...};
        for (const auto& arg : formattedArgs) {
            oss << " " << arg;
        }
        
        ErrorInfo error(category, severity, oss.str(), __FILE__, __LINE__, __func__);
        globalErrorHandler->handleError(error);
    }
    
    template<typename... Args>
    void reportWarning(ErrorCategory category, const std::string& message, Args&&... args) {
        std::ostringstream oss;
        oss << message;
        std::vector<std::string> formattedArgs = {std::forward<Args>(args)...};
        for (const auto& arg : formattedArgs) {
            oss << " " << arg;
        }
        
        ErrorInfo warning(category, ErrorSeverity::Warning, oss.str(), __FILE__, __LINE__, __func__);
        globalErrorHandler->handleWarning(warning);
    }
    
    // Set global error handler
    void setGlobalErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        globalErrorHandler = handler;
    }
    
    // Get global error handler
    std::shared_ptr<IErrorHandler> getGlobalErrorHandler() {
        return globalErrorHandler;
    }
    
    // Helper functions for common error scenarios
    void validateConfig(const std::string& key, const std::string& value);
    void validateNeuralState(const std::string& context);
    void validateMemoryAccess(size_t index, size_t size);
    void validateSynapseConnection(const std::string& pre, const std::string& post);
    
    // Error validation functions
    template<typename T>
    bool validateValue(const T& value, T min, T max, const std::string& description) {
        if (value < min || value > max) {
            reportError(ErrorCategory::Configuration, ErrorSeverity::Error,
                       "Invalid " + description + ": " + std::to_string(value) +
                       ". Must be between " + std::to_string(min) + 
                       " and " + std::to_string(max));
            return false;
        }
        return true;
    }
    
    template<typename T>
    bool validatePointer(const T* ptr, const std::string& name) {
        if (!ptr) {
            reportError(ErrorCategory::Memory, ErrorSeverity::Error,
                       "Null pointer access: " + name);
            return false;
        }
        return true;
    }
    
    bool validateRange(size_t index, size_t size, const std::string& context);
    bool validatePositive(const std::string& name, double value);
    bool validateNonNegative(const std::string& name, double value);
    bool validateString(const std::string& name, const std::string& value, 
                       size_t minLength = 0, size_t maxLength = std::string::max_size);
}

// Convenience macros for error handling
#define NLM_THROW_ERROR(category, severity, message) \
    nlm::ErrorHandling::throwError(category, severity, message, __FILE__, __LINE__, __func__)

#define NLM_REPORT_ERROR(category, severity, message) \
    nlm::ErrorHandling::reportError(category, severity, message, __FILE__, __LINE__, __func__)

#define NLM_REPORT_WARNING(category, message) \
    nlm::ErrorHandling::reportWarning(category, message, __FILE__, __LINE__, __func__)

#define NLM_VALIDATE_PTR(ptr, name) \
    if (!nlm::ErrorHandling::validatePointer(ptr, name)) { \
        throw NLMError(nlm::ErrorCategory::Memory, nlm::ErrorSeverity::Error, \
                      std::string("Null pointer: ") + name, __FILE__, __LINE__, __func__); \
    }

#define NLM_VALIDATE_RANGE(index, size, context) \
    if (!nlm::ErrorHandling::validateRange(index, size, context)) { \
        throw NLMError(nlm::ErrorCategory::Memory, nlm::ErrorSeverity::Error, \
                      std::string("Invalid range in ") + context, __FILE__, __LINE__, __func__); \
    }

#define NLM_VALIDATE_CONFIG(key, value) \
    nlm::ErrorHandling::validateConfig(key, value)

// Error category helper functions
inline std::string errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::Configuration: return "Configuration";
        case ErrorCategory::Memory: return "Memory";
        case ErrorCategory::NeuralDynamics: return "Neural Dynamics";
        case ErrorCategory::Plasticity: return "Plasticity";
        case ErrorCategory::MemorySystem: return "Memory System";
        case ErrorCategory::PredictionSystem: return "Prediction System";
        case ErrorCategory::CognitiveSystem: return "Cognitive System";
        case ErrorCategory::DevelopmentSystem: return "Development System";
        case ErrorCategory::Neuromodulation: return "Neuromodulation";
        case ErrorCategory::WorldSimulation: return "World Simulation";
        case ErrorCategory::AgentInterface: return "Agent Interface";
        case ErrorCategory::FileIO: return "File I/O";
        case ErrorCategory::Integration: return "Integration";
        case ErrorCategory::Performance: return "Performance";
        case ErrorCategory::Unknown: return "Unknown";
    }
    return "Unknown";
}

// Error severity helper functions
inline std::string errorSeverityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return "Debug";
        case ErrorSeverity::Info: return "Info";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error: return "Error";
        case ErrorSeverity::Critical: return "Critical";
        case ErrorSeverity::Fatal: return "Fatal";
    }
    return "Unknown";
}

} // namespace nlm
