// Enhanced Error Handling System for NLM
// Provides consistent and comprehensive error handling across the codebase

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <functional>

namespace nlm {

/**
 * Error categories for NLM
 */
enum class ErrorCategory {
    // Configuration errors
    CONFIG_FILE_NOT_FOUND,
    CONFIG_FILE_PARSE_ERROR,
    CONFIG_VALIDATION_ERROR,
    CONFIG_MISSING_FIELD,
    CONFIG_INVALID_VALUE,
    CONFIG_DUPLICATE_FIELD,
    
    // Neural network errors
    NEURON_NOT_FOUND,
    SYNAPSE_NOT_FOUND,
    REGION_NOT_FOUND,
    BRAIN_NOT_INITIALIZED,
    INVALID_NEURON_TYPE,
    INVALID_SYNAPSE_TYPE,
    CONNECTION_ERROR,
    
    // Memory errors
    MEMORY_NOT_INITIALIZED,
    WORKING_MEMORY_FULL,
    EPISODIC_MEMORY_FULL,
    MEMORY_TRAIL_NOT_FOUND,
    MEMORY_CORRUPTION,
    
    // Development errors
    DEVELOPMENT_NOT_STARTED,
    DEVELOPMENT_NOT_COMPLETE,
    CRITICAL_PERIOD_NOT_ENTERED,
    DEVELOPMENT_ERROR,
    
    // Neuromodulation errors
    NEUROMODULATOR_NOT_INITIALIZED,
    INVALID_NEUROMODULATOR_TYPE,
    NEUROMODULATION_ERROR,
    
    // Prediction errors
    PREDICTION_NOT_INITIALIZED,
    PREDICTION_MODEL_NOT_LOADED,
    PREDICTION_ERROR,
    
    // Motor errors
    MOTOR_SYSTEM_NOT_INITIALIZED,
    INVALID_MOTOR_COMMAND,
    MOTOR_EXECUTION_ERROR,
    
    // Environment errors
    ENVIRONMENT_NOT_INITIALIZED,
    INVALID_ENVIRONMENT_TYPE,
    ENVIRONMENT_ERROR,
    
    // Cognition errors
    CONCEPT_NOT_FOUND,
    PLANNER_NOT_INITIALIZED,
    COGNITION_ERROR,
    
    // Agent errors
    AGENT_NOT_INITIALIZED,
    SENSOR_ERROR,
    AGENT_ERROR,
    
    // Performance errors
    PERFORMANCE_ERROR,
    TIMEOUT_ERROR,
    MEMORY_ERROR,
    COMPUTATION_ERROR,
    
    // System errors
    INITIALIZATION_ERROR,
    SHUTDOWN_ERROR,
    RESOURCE_NOT_AVAILABLE,
    SYSTEM_ERROR,
    
    // External interface errors
    EXTERNAL_INTERFACE_ERROR,
    PYTHON_BINDING_ERROR,
    GUI_ERROR,
    
    // Generic errors
    UNKNOWN_ERROR,
    UNEXPECTED_ERROR,
    VALIDATION_ERROR,
    NOT_IMPLEMENTED_ERROR,
    PERMISSION_ERROR,
    OUT_OF_BOUNDS_ERROR
};

/**
 * Error severity levels
 */
enum class ErrorSeverity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};

/**
 * Enhanced error info structure
 */
struct ErrorInfo {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string context;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> suggestions;
    std::shared_ptr<std::exception> originalException;
    
    ErrorInfo(ErrorCategory cat, ErrorSeverity sev, const std::string& msg)
        : category(cat), severity(sev), message(msg), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorInfo(ErrorCategory cat, ErrorSeverity sev, const std::string& msg,
             const std::string& file, int line, const std::string& function)
        : category(cat), severity(sev), message(msg), file(file), line(line), function(function) {
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string getFormattedMessage() const {
        std::stringstream ss;
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;
        
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count()
           << " [" << errorCategoryToString(category) << "]"
           << " [" << errorSeverityToString(severity) << "]";
        
        if (!file.empty()) {
            ss << " in " << file;
            if (line > 0) {
                ss << ":" << line;
            }
        }
        
        if (!function.empty()) {
            ss << " (" << function << ")";
        }
        
        ss << ": " << message;
        
        return ss.str();
    }
    
    std::string errorCategoryToString(ErrorCategory cat) const {
        static const std::map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::CONFIG_FILE_NOT_FOUND, "CONFIG_FILE_NOT_FOUND"},
            {ErrorCategory::CONFIG_FILE_PARSE_ERROR, "CONFIG_FILE_PARSE_ERROR"},
            {ErrorCategory::CONFIG_VALIDATION_ERROR, "CONFIG_VALIDATION_ERROR"},
            {ErrorCategory::CONFIG_MISSING_FIELD, "CONFIG_MISSING_FIELD"},
            {ErrorCategory::CONFIG_INVALID_VALUE, "CONFIG_INVALID_VALUE"},
            {ErrorCategory::CONFIG_DUPLICATE_FIELD, "CONFIG_DUPLICATE_FIELD"},
            {ErrorCategory::NEURON_NOT_FOUND, "NEURON_NOT_FOUND"},
            {ErrorCategory::SYNAPSE_NOT_FOUND, "SYNAPSE_NOT_FOUND"},
            {ErrorCategory::REGION_NOT_FOUND, "REGION_NOT_FOUND"},
            {ErrorCategory::BRAIN_NOT_INITIALIZED, "BRAIN_NOT_INITIALIZED"},
            {ErrorCategory::INVALID_NEURON_TYPE, "INVALID_NEURON_TYPE"},
            {ErrorCategory::INVALID_SYNAPSE_TYPE, "INVALID_SYNAPSE_TYPE"},
            {ErrorCategory::CONNECTION_ERROR, "CONNECTION_ERROR"},
            {ErrorCategory::MEMORY_NOT_INITIALIZED, "MEMORY_NOT_INITIALIZED"},
            {ErrorCategory::WORKING_MEMORY_FULL, "WORKING_MEMORY_FULL"},
            {ErrorCategory::EPISODIC_MEMORY_FULL, "EPISODIC_MEMORY_FULL"},
            {ErrorCategory::MEMORY_TRAIL_NOT_FOUND, "MEMORY_TRAIL_NOT_FOUND"},
            {ErrorCategory::MEMORY_CORRUPTION, "MEMORY_CORRUPTION"},
            {ErrorCategory::DEVELOPMENT_NOT_STARTED, "DEVELOPMENT_NOT_STARTED"},
            {ErrorCategory::DEVELOPMENT_NOT_COMPLETE, "DEVELOPMENT_NOT_COMPLETE"},
            {ErrorCategory::CRITICAL_PERIOD_NOT_ENTERED, "CRITICAL_PERIOD_NOT_ENTERED"},
            {ErrorCategory::DEVELOPMENT_ERROR, "DEVELOPMENT_ERROR"},
            {ErrorCategory::NEUROMODULATOR_NOT_INITIALIZED, "NEUROMODULATOR_NOT_INITIALIZED"},
            {ErrorCategory::INVALID_NEUROMODULATOR_TYPE, "INVALID_NEUROMODULATOR_TYPE"},
            {ErrorCategory::NEUROMODULATION_ERROR, "NEUROMODULATION_ERROR"},
            {ErrorCategory::PREDICTION_NOT_INITIALIZED, "PREDICTION_NOT_INITIALIZED"},
            {ErrorCategory::PREDICTION_MODEL_NOT_LOADED, "PREDICTION_MODEL_NOT_LOADED"},
            {ErrorCategory::PREDICTION_ERROR, "PREDICTION_ERROR"},
            {ErrorCategory::MOTOR_SYSTEM_NOT_INITIALIZED, "MOTOR_SYSTEM_NOT_INITIALIZED"},
            {ErrorCategory::INVALID_MOTOR_COMMAND, "INVALID_MOTOR_COMMAND"},
            {ErrorCategory::MOTOR_EXECUTION_ERROR, "MOTOR_EXECUTION_ERROR"},
            {ErrorCategory::ENVIRONMENT_NOT_INITIALIZED, "ENVIRONMENT_NOT_INITIALIZED"},
            {ErrorCategory::INVALID_ENVIRONMENT_TYPE, "INVALID_ENVIRONMENT_TYPE"},
            {ErrorCategory::ENVIRONMENT_ERROR, "ENVIRONMENT_ERROR"},
            {ErrorCategory::CONCEPT_NOT_FOUND, "CONCEPT_NOT_FOUND"},
            {ErrorCategory::PLANNER_NOT_INITIALIZED, "PLANNER_NOT_INITIALIZED"},
            {ErrorCategory::COGNITION_ERROR, "COGNITION_ERROR"},
            {ErrorCategory::AGENT_NOT_INITIALIZED, "AGENT_NOT_INITIALIZED"},
            {ErrorCategory::SENSOR_ERROR, "SENSOR_ERROR"},
            {ErrorCategory::AGENT_ERROR, "AGENT_ERROR"},
            {ErrorCategory::PERFORMANCE_ERROR, "PERFORMANCE_ERROR"},
            {ErrorCategory::TIMEOUT_ERROR, "TIMEOUT_ERROR"},
            {ErrorCategory::MEMORY_ERROR, "MEMORY_ERROR"},
            {ErrorCategory::COMPUTATION_ERROR, "COMPUTATION_ERROR"},
            {ErrorCategory::INITIALIZATION_ERROR, "INITIALIZATION_ERROR"},
            {ErrorCategory::SHUTDOWN_ERROR, "SHUTDOWN_ERROR"},
            {ErrorCategory::RESOURCE_NOT_AVAILABLE, "RESOURCE_NOT_AVAILABLE"},
            {ErrorCategory::SYSTEM_ERROR, "SYSTEM_ERROR"},
            {ErrorCategory::EXTERNAL_INTERFACE_ERROR, "EXTERNAL_INTERFACE_ERROR"},
            {ErrorCategory::PYTHON_BINDING_ERROR, "PYTHON_BINDING_ERROR"},
            {ErrorCategory::GUI_ERROR, "GUI_ERROR"},
            {ErrorCategory::UNKNOWN_ERROR, "UNKNOWN_ERROR"},
            {ErrorCategory::UNEXPECTED_ERROR, "UNEXPECTED_ERROR"},
            {ErrorCategory::VALIDATION_ERROR, "VALIDATION_ERROR"},
            {ErrorCategory::NOT_IMPLEMENTED_ERROR, "NOT_IMPLEMENTED_ERROR"},
            {ErrorCategory::PERMISSION_ERROR, "PERMISSION_ERROR"},
            {ErrorCategory::OUT_OF_BOUNDS_ERROR, "OUT_OF_BOUNDS_ERROR"}
        };
        
        auto it = categoryStrings.find(cat);
        return (it != categoryStrings.end()) ? it->second : "UNKNOWN";
    }
    
    std::string errorSeverityToString(ErrorSeverity sev) const {
        static const std::map<ErrorSeverity, std::string> severityStrings = {
            {ErrorSeverity::DEBUG, "DEBUG"},
            {ErrorSeverity::INFO, "INFO"},
            {ErrorSeverity::WARNING, "WARNING"},
            {ErrorSeverity::ERROR, "ERROR"},
            {ErrorSeverity::CRITICAL, "CRITICAL"},
            {ErrorSeverity::FATAL, "FATAL"}
        };
        
        auto it = severityStrings.find(sev);
        return (it != severityStrings.end()) ? it->second : "UNKNOWN";
    }
};

/**
 * Error handler interface
 */
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const ErrorInfo& error) = 0;
    virtual void handleError(const std::exception& error) = 0;
    virtual bool shouldRethrow() const = 0;
    virtual ErrorSeverity getMinSeverity() const = 0;
};

/**
 * Default error handler
 */
class DefaultErrorHandler : public IErrorHandler {
private:
    ErrorSeverity minSeverity;
    std::vector<std::shared_ptr<IErrorHandler>> chainedHandlers;
    bool outputToConsole;
    bool outputToFile;
    std::string errorLogFile;
    
public:
    DefaultErrorHandler(ErrorSeverity severity = ErrorSeverity::WARNING,
                       bool consoleOutput = true,
                       bool fileOutput = false,
                       const std::string& logFile = "nlm_errors.log")
        : minSeverity(severity), outputToConsole(consoleOutput),
          outputToFile(fileOutput), errorLogFile(logFile) {}
    
    void handleError(const ErrorInfo& error) override {
        if (error.severity < minSeverity) {
            return; // Ignore error below minimum severity
        }
        
        // Handle through chained handlers
        for (const auto& handler : chainedHandlers) {
            handler->handleError(error);
        }
        
        // Console output
        if (outputToConsole) {
            std::cerr << error.getFormattedMessage() << std::endl;
        }
        
        // File output
        if (outputToFile) {
            std::ofstream file(errorLogFile, std::ios::app);
            if (file.is_open()) {
                file << error.getFormattedMessage() << std::endl;
            }
        }
        
        // Rethrow if severity is FATAL or CRITICAL
        if (error.severity >= ErrorSeverity::ERROR) {
            throw std::runtime_error(error.message);
        }
    }
    
    void handleError(const std::exception& error) override {
        ErrorInfo errorInfo(
            ErrorCategory::UNEXPECTED_ERROR,
            ErrorSeverity::ERROR,
            error.what(),
            "", 0, ""
        );
        errorInfo.originalException = std::current_exception();
        handleError(errorInfo);
    }
    
    bool shouldRethrow() const override {
        return false;
    }
    
    ErrorSeverity getMinSeverity() const override {
        return minSeverity;
    }
    
    // Add chained error handler
    void addChainedHandler(std::shared_ptr<IErrorHandler> handler) {
        chainedHandlers.push_back(handler);
    }
    
    // Set output options
    void setOutputToConsole(bool enabled) {
        outputToConsole = enabled;
    }
    
    void setOutputToFile(bool enabled, const std::string& filename = "") {
        outputToFile = enabled;
        if (!filename.empty()) {
            errorLogFile = filename;
        }
    }
};

/**
 * Error context manager
 */
class ErrorContext {
private:
    std::string contextName;
    std::map<std::string, std::string> contextData;
    bool isActive = false;
    
public:
    ErrorContext(const std::string& name) : contextName(name) {}
    
    void addData(const std::string& key, const std::string& value) {
        contextData[key] = value;
    }
    
    std::string getData(const std::string& key) const {
        auto it = contextData.find(key);
        return (it != contextData.end()) ? it->second : "";
    }
    
    void activate() {
        isActive = true;
    }
    
    void deactivate() {
        isActive = false;
    }
    
    bool isActive() const {
        return isActive;
    }
    
    std::string getContextName() const {
        return contextName;
    }
    
    std::map<std::string, std::string> getAllData() const {
        return contextData;
    }
};

/**
 * Global error handler
 */
class ErrorManager {
private:
    static std::shared_ptr<IErrorHandler> currentHandler;
    static std::vector<ErrorInfo> errorHistory;
    static bool isEnabled;
    
public:
    static void setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        currentHandler = handler;
        isEnabled = true;
    }
    
    static std::shared_ptr<IErrorHandler> getErrorHandler() {
        return currentHandler;
    }
    
    static void resetToDefault() {
        currentHandler = std::make_shared<DefaultErrorHandler>();
        isEnabled = true;
    }
    
    static void logError(ErrorCategory category, ErrorSeverity severity,
                         const std::string& message,
                         const std::string& file = "",
                         int line = 0,
                         const std::string& function = "") {
        if (!isEnabled || !currentHandler) {
            return;
        }
        
        ErrorInfo error(category, severity, message, file, line, function);
        currentHandler->handleError(error);
        
        if (currentHandler->shouldRethrow()) {
            throw std::runtime_error(message);
        }
    }
    
    static void logError(const std::exception& error) {
        if (!isEnabled || !currentHandler) {
            return;
        }
        
        currentHandler->handleError(error);
    }
    
    static std::vector<ErrorInfo> getErrorHistory() {
        return errorHistory;
    }
    
    static void clearErrorHistory() {
        errorHistory.clear();
    }
    
    static bool hasErrors() {
        return !errorHistory.empty();
    }
    
    static size_t getErrorCount(ErrorCategory category = ErrorCategory::UNKNOWN_ERROR) {
        size_t count = 0;
        for (const auto& error : errorHistory) {
            if (category == ErrorCategory::UNKNOWN_ERROR || error.category == category) {
                count++;
            }
        }
        return count;
    }
};

/**
 * Enhanced exception class
 */
class NLMException : public std::runtime_error {
private:
    ErrorCategory category;
    ErrorSeverity severity;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    
public:
    NLMException(ErrorCategory cat, ErrorSeverity sev, const std::string& message)
        : std::runtime_error(message), category(cat), severity(sev) {
        timestamp = std::chrono::system_clock::now();
    }
    
    NLMException(ErrorCategory cat, ErrorSeverity sev, const std::string& message,
                const std::string& file, int line, const std::string& function)
        : std::runtime_error(formatMessage(message, file, line, function)),
          category(cat), severity(sev) {
        timestamp = std::chrono::system_clock::now();
        context = file + ":" + std::to_string(line) + " (" + function + ")";
    }
    
    ErrorCategory getCategory() const {
        return category;
    }
    
    ErrorSeverity getSeverity() const {
        return severity;
    }
    
    const std::string& getContext() const {
        return context;
    }
    
    std::chrono::system_clock::time_point getTimestamp() const {
        return timestamp;
    }
    
private:
    static std::string formatMessage(const std::string& message,
                                     const std::string& file, int line,
                                     const std::string& function) {
        std::stringstream ss;
        ss << message;
        if (!file.empty()) {
            ss << " [in " << file;
            if (line > 0) ss << ":" << line;
            ss << "]";
        }
        if (!function.empty()) {
            ss << " (" << function << ")";
        }
        return ss.str();
    }
};

/**
 * Macro for enhanced error handling
 */
#define NLM_THROW_ERROR(category, severity, message) \
    do { \
        nlm::ErrorManager::logError( \
            category, severity, message, __FILE__, __LINE__, __FUNCTION__ \
        ); \
        throw nlm::NLMException(category, severity, message, __FILE__, __LINE__, __FUNCTION__); \
    } while(0)

#define NLM_THROW_ERROR_CONTEXT(category, severity, message, context) \
    do { \
        nlm::ErrorManager::logError( \
            category, severity, message, __FILE__, __LINE__, __FUNCTION__ \
        ); \
        nlm::ErrorContext ctx(context); \
        throw nlm::NLMException(category, severity, message, __FILE__, __LINE__, __FUNCTION__); \
    } while(0)

#define NLM_SAFE_CALL(operation) \
    try { \
        operation; \
    } catch (const std::exception& e) { \
        nlm::ErrorManager::logError(e); \
        throw; \
    }

/**
 * Error handling utilities
 */
namespace ErrorUtils {
    // Validate input parameters
    template<typename T>
    void validateNotNull(const T* ptr, const std::string& paramName = "parameter") {
        if (!ptr) {
            NLM_THROW_ERROR(
                ErrorCategory::INVALID_VALUE,
                ErrorSeverity::ERROR,
                "Null pointer provided for parameter: " + paramName
            );
        }
    }
    
    // Validate range
    template<typename T>
    void validateRange(const T& value, const T& min, const T& max, const std::string& paramName = "parameter") {
        if (value < min || value > max) {
            NLM_THROW_ERROR(
                ErrorCategory::OUT_OF_BOUNDS_ERROR,
                ErrorSeverity::ERROR,
                "Value " + std::to_string(value) + " is out of range for parameter " + paramName +
                " (must be between " + std::to_string(min) + " and " + std::to_string(max) + ")"
            );
        }
    }
    
    // Validate string
    void validateNotEmpty(const std::string& value, const std::string& paramName = "parameter") {
        if (value.empty()) {
            NLM_THROW_ERROR(
                ErrorCategory::INVALID_VALUE,
                ErrorSeverity::ERROR,
                "Empty string provided for parameter: " + paramName
            );
        }
    }
    
    // Validate file exists
    bool validateFileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
    
    // Get error suggestions
    std::vector<std::string> getErrorSuggestions(ErrorCategory category) {
        static const std::map<ErrorCategory, std::vector<std::string>> suggestions = {
            {ErrorCategory::CONFIG_FILE_NOT_FOUND, {
                "Check if the configuration file exists",
                "Verify file permissions",
                "Check if the file path is correct"
            }},
            {ErrorCategory::CONFIG_FILE_PARSE_ERROR, {
                "Check syntax of configuration file",
                "Verify file format (JSON/YAML/INI)",
                "Check for missing commas or brackets"
            }},
            {ErrorCategory::CONFIG_VALIDATION_ERROR, {
                "Check configuration schema",
                "Verify required fields are present",
                "Check data types match configuration schema"
            }},
            {ErrorCategory::NEURON_NOT_FOUND, {
                "Check neuron ID or name",
                "Verify neuron has been created",
                "Check if neuron is still active"
            }},
            {ErrorCategory::MEMORY_NOT_INITIALIZED, {
                "Initialize memory system first",
                "Check memory allocation parameters",
                "Verify system resources are available"
            }},
            {ErrorCategory::PERFORMANCE_ERROR, {
                "Check system resources (CPU, memory)",
                "Consider reducing simulation size",
                "Check for infinite loops or inefficient code"
            }},
            {ErrorCategory::OUT_OF_BOUNDS_ERROR, {
                "Check array indices",
                "Verify numeric ranges",
                "Check boundary conditions"
            }},
            {ErrorCategory::UNEXPECTED_ERROR, {
                "Check system state",
                "Verify input parameters",
                "Check for resource exhaustion"
            }}
        };
        
        auto it = suggestions.find(category);
        return (it != suggestions.end()) ? it->second : std::vector<std::string>{"Check implementation"};
    }
}

// Initialize static members
std::shared_ptr<IErrorHandler> ErrorManager::currentHandler = std::make_shared<DefaultErrorHandler>();
std::vector<ErrorInfo> ErrorManager::errorHistory;
bool ErrorManager::isEnabled = true;

} // namespace nlm
