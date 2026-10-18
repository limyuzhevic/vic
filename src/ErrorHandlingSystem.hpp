// Enhanced Error Handling and User-Friendly Messages System
// 
// This file provides:
// 1. Comprehensive error categorization and classification
// 2. User-friendly error messages with context
// 3. Error recovery mechanisms
// 4. Logging and reporting
// 5. Integration with experiment framework
// 6. Context-aware error messages
// 7. Multiple error output formats
// 8. Error aggregation and summary reporting
// 9. Interactive error recovery when possible
// 10. Comprehensive error documentation

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <functional>
#include <stdexcept>

namespace nlm {

// Error categories for enhanced classification
enum class ErrorCategory {
    // Core system errors
    SystemInitialization,
    ConfigurationError,
    MemoryAllocation,
    FileSystemError,
    NetworkError,
    
    // Neural computation errors
    NeuronDynamicsError,
    SynapseError,
    BrainError,
    PlasticityError,
    STDPError,
    HebbianError,
    
    // Development and learning errors
    DevelopmentError,
    LearningError,
    NeuromodulationError,
    RewardError,
    CuriosityError,
    NoveltyError,
    
    // Environment and perception errors
    EnvironmentError,
    SensoryError,
    PerceptionError,
    MotorError,
    
    // Memory system errors
    MemoryError,
    WorkingMemoryError,
    EpisodicMemoryError,
    SemanticMemoryError,
    
    // Cognitive errors
    PlanningError,
    ConceptFormationError,
    PredictionError,
    DecisionError,
    
    // Integration and system errors
    IntegrationError,
    SerializationError,
    ValidationError,
    DependencyError,
    
    // User interface errors
    UserInterfaceError,
    InputError,
    OutputError,
    VisualizationError,
    
    // Performance errors
    PerformanceError,
    TimeoutError,
    ResourceExhaustion,
    
    // Compatibility and migration errors
    CompatibilityError,
    MigrationError,
    VersionError,
    
    // Unknown or unexpected errors
    UnknownError,
    UnexpectedError,
    CustomError
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

// Error context for enhanced reporting
struct ErrorContext {
    std::string errorCode;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string description;
    std::string suggestion;
    std::string remediation;
    
    // Technical details
    std::string file;
    int line;
    std::string function;
    
    // System state
    std::string systemState;
    std::string configuration;
    std::string environment;
    
    // Timing information
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    double duration;
    
    // Related components
    std::vector<std::string> relatedComponents;
    std::vector<std::string> affectedComponents;
    
    // Error metadata
    std::unordered_map<std::string, std::string> metadata;
    
    ErrorContext() : category(ErrorCategory::UnknownError), severity(ErrorSeverity::Error),
                    line(0), duration(0.0) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Enhanced exception class with full context
class EnhancedException : public std::exception {
public:
    EnhancedException(const std::string& message, ErrorCategory category = ErrorCategory::UnknownError,
                      ErrorSeverity severity = ErrorSeverity::Error)
        : message_(message), category_(category), severity_(severity) {}
    
    EnhancedException(const ErrorContext& context)
        : message_(context.message), category_(context.category), severity_(context.severity),
          context_(context) {}
    
    // Get error message
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    // Get error details
    std::string getMessage() const { return message_; }
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    
    // Get context
    bool hasContext() const { return !context_.message.empty(); }
    ErrorContext getContext() const { return context_; }
    
    // Add context
    void addContext(const ErrorContext& ctx) {
        context_ = ctx;
        if (message_.empty()) {
            message_ = ctx.message;
        }
        category_ = ctx.category;
        severity_ = ctx.severity;
    }
    
    // Check if recoverable
    virtual bool isRecoverable() const {
        return severity_ != ErrorSeverity::Fatal && severity_ != ErrorSeverity::Critical;
    }
    
    // Get user-friendly error message
    virtual std::string getUserFriendlyMessage() const {
        std::stringstream ss;
        
        switch (severity_) {
            case ErrorSeverity::Debug:
                ss << "Debug: ";
                break;
            case ErrorSeverity::Info:
                ss << "Info: ";
                break;
            case ErrorSeverity::Warning:
                ss << "Warning: ";
                break;
            case ErrorSeverity::Error:
                ss << "Error: ";
                break;
            case ErrorSeverity::Critical:
                ss << "Critical Error: ";
                break;
            case ErrorSeverity::Fatal:
                ss << "Fatal Error: ";
                break;
        }
        
        ss << message_;
        
        if (!context_.suggestion.empty()) {
            ss << "\n\nSuggestion: " << context_.suggestion;
        }
        
        if (!context_.remediation.empty()) {
            ss << "\n\nRemediation: " << context_.remediation;
        }
        
        return ss.str();
    }
    
    // Get technical details
    virtual std::string getTechnicalDetails() const {
        std::stringstream ss;
        ss << "=== Technical Details ===\n";
        ss << "Error Category: " << static_cast<int>(category_) << "\n";
        ss << "Error Severity: " << static_cast<int>(severity_) << "\n";
        ss << "Error Code: " << context_.errorCode << "\n";
        
        if (!context_.file.empty()) {
            ss << "Location: " << context_.file << ":" << context_.line << "\n";
        }
        
        if (!context_.function.empty()) {
            ss << "Function: " << context_.function << "\n";
        }
        
        ss << "Description: " << context_.description << "\n";
        ss << "System State: " << context_.systemState << "\n";
        ss << "Configuration: " << context_.configuration << "\n";
        
        if (!context_.environment.empty()) {
            ss << "Environment: " << context_.environment << "\n";
        }
        
        ss << "Timestamp: " << std::chrono::system_clock::to_time_t(context_.timestamp) << "\n";
        ss << "Duration: " << context_.duration << " ms\n";
        
        if (!context_.relatedComponents.empty()) {
            ss << "Related Components: ";
            for (size_t i = 0; i < context_.relatedComponents.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << context_.relatedComponents[i];
            }
            ss << "\n";
        }
        
        if (!context_.affectedComponents.empty()) {
            ss << "Affected Components: ";
            for (size_t i = 0; i < context_.affectedComponents.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << context_.affectedComponents[i];
            }
            ss << "\n";
        }
        
        if (!context_.metadata.empty()) {
            ss << "Metadata:\n";
            for (const auto& pair : context_.metadata) {
                ss << "  " << pair.first << ": " << pair.second << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Get error summary
    virtual std::string getSummary() const {
        std::stringstream ss;
        ss << "Error Summary:";
        ss << "\n  Category: " << static_cast<int>(category_);
        ss << "\n  Severity: " << static_cast<int>(severity_);
        ss << "\n  Message: " << message_;
        if (hasContext()) {
            ss << "\n  Context available: Yes";
        }
        return ss.str();
    }
    
protected:
    std::string message_;
    ErrorCategory category_;
    ErrorSeverity severity_;
    ErrorContext context_;
};

// Specialized error types for different domains
class SystemError : public EnhancedException {
public:
    SystemError(const std::string& message, const std::string& component = "System")
        : EnhancedException(message, ErrorCategory::UnknownError, ErrorSeverity::Error) {
        context_.description = "System error in component: " + component;
        context_.suggestion = "Check system logs and restart if necessary.";
        context_.remediation = "Contact system administrator.";
    }
};

class ConfigurationError : public EnhancedException {
public:
    ConfigurationError(const std::string& message, const std::string& key = "", const std::string& expected = "")
        : EnhancedException("Configuration error: " + message, ErrorCategory::ConfigurationError, ErrorSeverity::Error) {
        context_.errorCode = "CONFIG_ERROR";
        if (!key.empty()) {
            context_.description = "Configuration error for key '" + key + "': " + message;
            if (!expected.empty()) {
                context_.suggestion = "Expected value of type '" + expected + "'.";
            }
        }
        context_.remediation = "Check configuration file and correct the value.";
    }
};

class NeuronError : public EnhancedException {
public:
    NeuronError(const std::string& message, const std::string& neuronId = "")
        : EnhancedException("Neuron error: " + message, ErrorCategory::NeuronDynamicsError, ErrorSeverity::Error) {
        context_.errorCode = "NEURON_ERROR";
        if (!neuronId.empty()) {
            context_.description = "Error in neuron '" + neuronId + "': " + message;
        }
        context_.suggestion = "Check neuron parameters and initial conditions.";
        context_.remediation = "Reset or recreate the neuron.";
    }
};

class SynapseError : public EnhancedException {
public:
    SynapseError(const std::string& message, const std::string& synapseId = "")
        : EnhancedException("Synapse error: " + message, ErrorCategory::SynapseError, ErrorSeverity::Error) {
        context_.errorCode = "SYNAPSE_ERROR";
        if (!synapseId.empty()) {
            context_.description = "Error in synapse '" + synapseId + "': " + message;
        }
        context_.suggestion = "Check synapse weight and plasticity parameters.";
        context_.remediation = "Reset or recreate the synapse.";
    }
};

class BrainError : public EnhancedException {
public:
    BrainError(const std::string& message, const std::string& brainId = "")
        : EnhancedException("Brain error: " + message, ErrorCategory::BrainError, ErrorSeverity::Critical) {
        context_.errorCode = "BRAIN_ERROR";
        if (!brainId.empty()) {
            context_.description = "Error in brain '" + brainId + "': " + message;
        }
        context_.suggestion = "Check brain initialization and configuration.";
        context_.remediation = "Restart the simulation or check configuration files.";
    }
    
    bool isRecoverable() const override {
        return false;  // Brain errors are generally not recoverable
    }
};

class PlasticityError : public EnhancedException {
public:
    PlasticityError(const std::string& message, const std::string& ruleType = "")
        : EnhancedException("Plasticity error: " + message, ErrorCategory::PlasticityError, ErrorSeverity::Warning) {
        context_.errorCode = "PLASTICITY_ERROR";
        if (!ruleType.empty()) {
            context_.description = "Error in plasticity rule '" + ruleType + "': " + message;
        }
        context_.suggestion = "Check plasticity parameters and learning rates.";
        context_.remediation = "Adjust plasticity parameters or disable the rule.";
    }
};

class EnvironmentError : public EnhancedException {
public:
    EnvironmentError(const std::string& message, const std::string& environmentId = "")
        : EnhancedException("Environment error: " + message, ErrorCategory::EnvironmentError, ErrorSeverity::Error) {
        context_.errorCode = "ENVIRONMENT_ERROR";
        if (!environmentId.empty()) {
            context_.description = "Error in environment '" + environmentId + "': " + message;
        }
        context_.suggestion = "Check environment configuration and boundaries.";
        context_.remediation = "Reset environment or check configuration.";
    }
};

class MemoryError : public EnhancedException {
public:
    MemoryError(const std::string& message, const std::string& memoryType = "")
        : EnhancedException("Memory error: " + message, ErrorCategory::MemoryError, ErrorSeverity::Error) {
        context_.errorCode = "MEMORY_ERROR";
        if (!memoryType.empty()) {
            context_.description = "Error in " + memoryType + " system: " + message;
        }
        context_.suggestion = "Check memory capacity and allocation.";
        context_.remediation = "Free memory or increase capacity.";
    }
};

class DevelopmentError : public EnhancedException {
public:
    DevelopmentError(const std::string& message, const std::string& stage = "")
        : EnhancedException("Development error: " + message, ErrorCategory::DevelopmentError, ErrorSeverity::Warning) {
        context_.errorCode = "DEVELOPMENT_ERROR";
        if (!stage.empty()) {
            context_.description = "Error in developmental stage '" + stage + "': " + message;
        }
        context_.suggestion = "Check developmental parameters and timing.";
        context_.remediation = "Adjust developmental stage parameters or skip the stage.";
    }
};

class LearningError : public EnhancedException {
public:
    LearningError(const std::string& message, const std::string& algorithm = "")
        : EnhancedException("Learning error: " + message, ErrorCategory::LearningError, ErrorSeverity::Warning) {
        context_.errorCode = "LEARNING_ERROR";
        if (!algorithm.empty()) {
            context_.description = "Error in learning algorithm '" + algorithm + "': " + message;
        }
        context_.suggestion = "Check learning parameters and training data.";
        context_.remediation = "Adjust learning parameters or use a different algorithm.";
    }
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual void handleError(const EnhancedException& error) = 0;
    virtual void handleWarning(const std::string& message) = 0;
    virtual void handleInfo(const std::string& message) = 0;
};

// Console error handler for user-friendly output
class ConsoleErrorHandler : public ErrorHandler {
public:
    void handleError(const EnhancedException& error) override {
        std::cerr << "\n" << "=" << std::string(60, '=') << std::endl;
        std::cerr << "ERROR OCCURRED" << std::endl;
        std::cerr << "=" << std::string(60, '=') << std::endl;
        
        std::cerr << error.getUserFriendlyMessage() << std::endl;
        
        if (error.getContext().severity == ErrorSeverity::Critical) {
            std::cerr << "\n" << "=" << std::string(60, '=') << std::endl;
            std::cerr << "CRITICAL ERROR - SYSTEM MAY BE UNSTABLE" << std::endl;
            std::cerr << "=" << std::string(60, '=') << std::endl;
        }
        
        std::cerr << "\n" << error.getTechnicalDetails() << std::endl;
    }
    
    void handleWarning(const std::string& message) override {
        std::cerr << "Warning: " << message << std::endl;
    }
    
    void handleInfo(const std::string& message) override {
        std::cout << "Info: " << message << std::endl;
    }
};

// File error handler for logging to file
class FileErrorHandler : public ErrorHandler {
public:
    FileErrorHandler(const std::string& filename) : logFile_(filename) {}
    
    void handleError(const EnhancedException& error) override {
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << "\n" << std::chrono::system_clock::now() << " | ERROR | " << error.getMessage() << std::endl;
            file << "Technical Details:" << error.getTechnicalDetails() << std::endl;
            file.close();
        }
    }
    
    void handleWarning(const std::string& message) override {
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << "\n" << std::chrono::system_clock::now() << " | WARNING | " << message << std::endl;
            file.close();
        }
    }
    
    void handleInfo(const std::string& message) override {
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << "\n" << std::chrono::system_clock::now() << " | INFO | " << message << std::endl;
            file.close();
        }
    }
    
private:
    std::string logFile_;
};

// Error aggregator for collecting and summarizing errors
class ErrorAggregator {
public:
    ErrorAggregator() : errorCount_(0), warningCount_(0), infoCount_(0) {}
    
    void addError(const EnhancedException& error) {
        errors_.push_back(error);
        errorCount_++;
        
        switch (error.getContext().severity) {
            case ErrorSeverity::Warning:
                warningCount_++;
                break;
            case ErrorSeverity::Info:
                infoCount_++;
                break;
            case ErrorSeverity::Error:
            case ErrorSeverity::Critical:
            case ErrorSeverity::Fatal:
                errorCount_++;
                break;
            default:
                break;
        }
    }
    
    void addWarning(const std::string& message) {
        warnings_.push_back(message);
        warningCount_++;
    }
    
    void addInfo(const std::string& message) {
        infos_.push_back(message);
        infoCount_++;
    }
    
    // Get error summary
    std::string getSummary() const {
        std::stringstream ss;
        ss << "=== Error Summary ===\n";
        ss << "Total Errors: " << errorCount_ << "\n";
        ss << "Total Warnings: " << warningCount_ << "\n";
        ss << "Total Info Messages: " << infoCount_ << "\n";
        ss << "Total Unique Error Types: " << getUniqueErrorTypes() << "\n";
        return ss.str();
    }
    
    // Get error report
    std::string getErrorReport() const {
        std::stringstream ss;
        ss << "=== Error Report ===\n\n";
        
        if (!errors_.empty()) {
            ss << "Errors:" << std::endl;
            for (size_t i = 0; i < errors_.size(); ++i) {
                ss << "[" << (i + 1) << "] " << errors_[i].getUserFriendlyMessage() << "\n\n";
            }
        }
        
        if (!warnings_.empty()) {
            ss << "Warnings:" << std::endl;
            for (size_t i = 0; i < warnings_.size(); ++i) {
                ss << "[" << (i + 1) << "] " << warnings_[i] << "\n";
            }
            ss << "\n";
        }
        
        if (!infos_.empty()) {
            ss << "Info Messages:" << std::endl;
            for (size_t i = 0; i < infos_.size(); ++i) {
                ss << "[" << (i + 1) << "] " << infos_[i] << "\n";
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    // Get unique error types
    size_t getUniqueErrorTypes() const {
        std::unordered_set<ErrorCategory> categories;
        for (const auto& error : errors_) {
            categories.insert(error.getCategory());
        }
        return categories.size();
    }
    
    // Clear all errors
    void clear() {
        errors_.clear();
        warnings_.clear();
        infos_.clear();
        errorCount_ = 0;
        warningCount_ = 0;
        infoCount_ = 0;
    }
    
    // Get most recent error
    EnhancedException getMostRecentError() const {
        if (!errors_.empty()) {
            return errors_.back();
        }
        return EnhancedException("No errors occurred.");
    }
    
private:
    std::vector<EnhancedException> errors_;
    std::vector<std::string> warnings_;
    std::vector<std::string> infos_;
    size_t errorCount_;
    size_t warningCount_;
    size_t infoCount_;
};

// Global error handler and aggregator
class GlobalErrorManager {
public:
    static GlobalErrorManager& getInstance() {
        static GlobalErrorManager instance;
        return instance;
    }
    
    // Set error handler
    void setErrorHandler(std::shared_ptr<ErrorHandler> handler) {
        errorHandler_ = handler;
    }
    
    // Get error handler
    std::shared_ptr<ErrorHandler> getErrorHandler() const {
        return errorHandler_;
    }
    
    // Add error
    void addError(const EnhancedException& error) {
        if (errorHandler_) {
            errorHandler_->handleError(error);
        }
        errorAggregator_.addError(error);
    }
    
    // Add warning
    void addWarning(const std::string& message) {
        if (errorHandler_) {
            errorHandler_->handleWarning(message);
        }
        errorAggregator_.addWarning(message);
    }
    
    // Add info
    void addInfo(const std::string& message) {
        if (errorHandler_) {
            errorHandler_->handleInfo(message);
        }
        errorAggregator_.addInfo(message);
    }
    
    // Get error aggregator
    ErrorAggregator getErrorAggregator() const {
        return errorAggregator_;
    }
    
    // Get error summary
    std::string getErrorSummary() const {
        return errorAggregator_.getSummary();
    }
    
    // Get error report
    std::string getErrorReport() const {
        return errorAggregator_.getErrorReport();
    }
    
    // Clear errors
    void clear() {
        errorAggregator_.clear();
    }
    
    // Check if there are errors
    bool hasErrors() const {
        return errorAggregator_.getErrorCount() > 0;
    }
    
    // Check if there are critical errors
    bool hasCriticalErrors() const {
        for (const auto& error : errorAggregator_.getErrors()) {
            if (error.getSeverity() == ErrorSeverity::Critical || error.getSeverity() == ErrorSeverity::Fatal) {
                return true;
            }
        }
        return false;
    }
    
    // Setup default error handling
    static void setupDefaultErrorHandling() {
        auto consoleHandler = std::make_shared<ConsoleErrorHandler>();
        setErrorHandler(consoleHandler);
    }
    
    // Setup file error logging
    static void setupFileErrorLogging(const std::string& filename) {
        auto fileHandler = std::make_shared<FileErrorHandler>(filename);
        setErrorHandler(fileHandler);
    }
    
private:
    GlobalErrorManager() : errorHandler_(std::make_shared<ConsoleErrorHandler>()) {}
    
    std::shared_ptr<ErrorHandler> errorHandler_;
    ErrorAggregator errorAggregator_;
    
    // Helper method for getErrorCount
    size_t getErrorCount() const {
        return errorAggregator_.getErrorCount();
    }
    
    // Helper method to get errors
    std::vector<EnhancedException> getErrors() const {
        return errorAggregator_.getErrors();
    }
};

// Utility functions for error reporting
namespace ErrorUtils {
    // Create error context with current system state
    ErrorContext createErrorContext(const std::string& errorCode, const std::string& message,
                                   ErrorCategory category, ErrorSeverity severity,
                                   const std::string& file = "", int line = 0,
                                   const std::string& function = "") {
        ErrorContext context;
        context.errorCode = errorCode;
        context.message = message;
        context.category = category;
        context.severity = severity;
        context.description = message;
        context.file = file;
        context.line = line;
        context.function = function;
        context.timestamp = std::chrono::high_resolution_clock::now();
        
        // Add system information
        context.systemState = "NLM Phase 2 Implementation";
        context.configuration = "Default configuration";
        context.environment = "Python environment";
        
        return context;
    }
    
    // Create user-friendly error message
    std::string createUserFriendlyMessage(const std::string& errorType, const std::string& details,
                                         const std::string& suggestion = "", const std::string& remediation = "") {
        std::stringstream ss;
        ss << errorType << ": " << details;
        
        if (!suggestion.empty()) {
            ss << "\n\nSuggestion: " << suggestion;
        }
        
        if (!remediation.empty()) {
            ss << "\n\nRemediation: " << remediation;
        }
        
        return ss.str();
    }
    
    // Get error icon for severity
    std::string getErrorIcon(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug:
                return "🔍";
            case ErrorSeverity::Info:
                return "ℹ";
            case ErrorSeverity::Warning:
                return "⚠";
            case ErrorSeverity::Error:
                return "❌";
            case ErrorSeverity::Critical:
                return "🚨";
            case ErrorSeverity::Fatal:
                return "💥";
            default:
                return "❓";
        }
    }
    
    // Get error color code for terminal output
    std::string getErrorColor(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug:
                return "\033[36m";  // Cyan
            case ErrorSeverity::Info:
                return "\033[32m";  // Green
            case ErrorSeverity::Warning:
                return "\033[33m";  // Yellow
            case ErrorSeverity::Error:
                return "\033[31m";  // Red
            case ErrorSeverity::Critical:
                return "\033[1;31m";  // Bold Red
            case ErrorSeverity::Fatal:
                return "\033[1;31;40m";  // Bold Red on Black
            default:
                return "\033[0m";   // Reset
        }
    }
    
    // Reset terminal color
    std::string resetColor() {
        return "\033[0m";
    }
}

} // namespace nlm

// Convenience macros for error handling
#define NLM_THROW_ERROR(error_type, message) \
    throw nlm::error_type(message)

#define NLM_THROW_ERROR_WITH_CONTEXT(error_type, message, category, severity) \
    nlm::ErrorContext context = nlm::ErrorUtils::createErrorContext("CUSTOM", message, category, severity); \
    throw nlm::error_type(context)

#define NLM_THROW_SYSTEM_ERROR(message) \
    throw nlm::SystemError(message)

#define NLM_THROW_CONFIGURATION_ERROR(message, key, expected) \
    throw nlm::ConfigurationError(message, key, expected)

#define NLM_THROW_NEURON_ERROR(message, neuron_id) \
    throw nlm::NeuronError(message, neuron_id)

#define NLM_THROW_SYNAPSE_ERROR(message, synapse_id) \
    throw nlm::SynapseError(message, synapse_id)

#define NLM_THROW_BRAIN_ERROR(message, brain_id) \
    throw nlm::BrainError(message, brain_id)

#define NLM_THROW_PLASTICITY_ERROR(message, rule_type) \
    throw nlm::PlasticityError(message, rule_type)

#define NLM_THROW_ENVIRONMENT_ERROR(message, environment_id) \
    throw nlm::EnvironmentError(message, environment_id)

#define NLM_THROW_MEMORY_ERROR(message, memory_type) \
    throw nlm::MemoryError(message, memory_type)

#define NLM_THROW_DEVELOPMENT_ERROR(message, stage) \
    throw nlm::DevelopmentError(message, stage)

#define NLM_THROW_LEARNING_ERROR(message, algorithm) \
    throw nlm::LearningError(message, algorithm)

// Usage examples:
// NLM_THROW_ERROR_WITH_CONTEXT(
//     nlm::BrainError,
//     "Failed to initialize brain with configuration",
//     nlm::ErrorCategory::BrainError,
//     nlm::ErrorSeverity::Critical,
//     __FILE__, __LINE__, __FUNCTION__
// );

// NLM_THROW_CONFIGURATION_ERROR(
//     "Invalid value for key 'random_seed'",
//     "random_seed",
//     "integer"
// );

// NLM_THROW_NEURON_ERROR(
//     "Membrane potential exceeds threshold",
//     "neuron_123"
// );