#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <functional>
#include <chrono>
#include <iomanip>

namespace nlm {

// Error types for different failure scenarios
enum class ErrorType {
    None = 0,
    ConfigParseError,
    FileIOError,
    MemoryError,
    ValidationError,
    NetworkError,
    TimeoutError,
    PermissionError,
    ConfigurationError,
    SimulationError,
    PlasticityError,
    DevelopmentError,
    UserInputError,
    SystemError,
    UnknownError
};

// Error severity levels
enum class ErrorSeverity {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3,
    Fatal = 4
};

// Detailed error information
struct ErrorInfo {
    ErrorType type;
    ErrorSeverity severity;
    std::string message;
    std::string code;
    std::string details;
    std::string suggestion;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    
    ErrorInfo() : type(ErrorType::UnknownError), severity(ErrorSeverity::Error),
                  message("Unknown error"), code("UNKNOWN"), details(""),
                  suggestion("No suggestion available"), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorInfo(ErrorType t, const std::string& msg, ErrorSeverity sev = ErrorSeverity::Error)
        : type(t), severity(sev), message(msg), code("UNKNOWN"), details(""),
          suggestion("No suggestion available"), timestamp(std::chrono::system_clock::now()) {}
    
    std::string format() const {
        std::ostringstream oss;
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
        
        // Add severity
        oss << "[" << severityToString(severity) << "] ";
        
        // Add error type
        oss << "[" << errorTypeToString(type) << "] ";
        
        // Add message
        oss << message;
        
        // Add error code if present
        if (code != "UNKNOWN") {
            oss << " (Code: " << code << ")";
        }
        
        return oss.str();
    }
    
    std::string formatHumanReadable() const {
        std::ostringstream oss;
        
        oss << "Error: " << message;
        
        if (!code.empty() && code != "UNKNOWN") {
            oss << " (Code: " << code << ")";
        }
        
        oss << "\n";
        
        if (!details.empty()) {
            oss << "Details: " << details << "\n";
        }
        
        if (!suggestion.empty() && suggestion != "No suggestion available") {
            oss << "Suggestion: " << suggestion << "\n";
        }
        
        if (!context.empty()) {
            oss << "Context:";
            for (const auto& pair : context) {
                oss << "\n  " << pair.first << " = " << pair.second;
            }
        }
        
        return oss.str();
    }
    
    static std::string severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    static std::string errorTypeToString(ErrorType type) {
        switch (type) {
            case ErrorType::None: return "NONE";
            case ErrorType::ConfigParseError: return "CONFIG_PARSE";
            case ErrorType::FileIOError: return "FILE_IO";
            case ErrorType::MemoryError: return "MEMORY";
            case ErrorType::ValidationError: return "VALIDATION";
            case ErrorType::NetworkError: return "NETWORK";
            case ErrorType::TimeoutError: return "TIMEOUT";
            case ErrorType::PermissionError: return "PERMISSION";
            case ErrorType::ConfigurationError: return "CONFIGURATION";
            case ErrorType::SimulationError: return "SIMULATION";
            case ErrorType::PlasticityError: return "PLASTICITY";
            case ErrorType::DevelopmentError: return "DEVELOPMENT";
            case ErrorType::UserInputError: return "USER_INPUT";
            case ErrorType::SystemError: return "SYSTEM";
            case ErrorType::UnknownError: return "UNKNOWN";
            default: return "UNKNOWN";
        }
    }
};

// Exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    ErrorInfo info;
    
    NLMError(const ErrorInfo& errorInfo) 
        : std::runtime_error(errorInfo.format()), info(errorInfo) {}
    
    NLMError(ErrorType type, const std::string& message, ErrorSeverity severity = ErrorSeverity::Error)
        : std::runtime_error(ErrorInfo(type, message, severity).format()) {
        info.type = type;
        info.severity = severity;
        info.message = message;
        info.code = "UNKNOWN";
        info.timestamp = std::chrono::system_clock::now();
    }
    
    NLMError(ErrorType type, const std::string& message, const std::string& code, ErrorSeverity severity = ErrorSeverity::Error)
        : std::runtime_error(ErrorInfo(type, message, severity).format()) {
        info.type = type;
        info.severity = severity;
        info.message = message;
        info.code = code;
        info.timestamp = std::chrono::system_clock::now();
    }
    
    // Get formatted error message
    std::string getFormattedMessage() const {
        return info.format();
    }
    
    // Get human readable error message
    std::string getHumanReadable() const {
        return info.formatHumanReadable();
    }
    
    // Check if error is recoverable
    bool isRecoverable() const {
        return info.severity <= ErrorSeverity::Warning;
    }
    
    // Get error severity as string
    std::string getSeverityString() const {
        return ErrorInfo::severityToString(info.severity);
    }
    
    // Get error type as string
    std::string getTypeString() const {
        return ErrorInfo::errorTypeToString(info.type);
    }
};

// Error handling utility
class ErrorHandler {
public:
    // Error callback function type
    using ErrorCallback = std::function<void(const ErrorInfo&)>;
    
    // Add error callback
    static void addErrorCallback(ErrorCallback callback) {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        callbacks.push_back(callback);
    }
    
    // Remove all error callbacks
    static void clearErrorCallbacks() {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        callbacks.clear();
    }
    
    // Handle an error
    static void handleError(const ErrorInfo& error) {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        
        // Call all registered callbacks
        for (const auto& callback : callbacks) {
            try {
                callback(error);
            } catch (const std::exception& e) {
                // If callback fails, log a warning
                std::cerr << "Warning: Error callback failed: " << e.what() << std::endl;
            }
        }
        
        // Print to stderr if severity is Error or higher
        if (error.severity >= ErrorSeverity::Error) {
            std::cerr << error.format() << std::endl;
        }
    }
    
    // Handle an error synchronously
    static void handleError(const NLMError& error) {
        handleError(error.info);
    }
    
    // Create and handle an error
    static void raiseError(ErrorType type, const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        ErrorInfo error(type, message, severity);
        handleError(error);
        throw NLMError(error);
    }
    
    // Create and handle an error with code
    static void raiseError(ErrorType type, const std::string& message, const std::string& code, ErrorSeverity severity = ErrorSeverity::Error) {
        ErrorInfo error(type, message, severity);
        error.code = code;
        handleError(error);
        throw NLMError(error);
    }
    
    // Log a warning without throwing
    static void logWarning(ErrorType type, const std::string& message) {
        ErrorInfo error(type, message, ErrorSeverity::Warning);
        handleError(error);
    }
    
    // Log an info message
    static void logInfo(ErrorType type, const std::string& message) {
        ErrorInfo error(type, message, ErrorSeverity::Info);
        handleError(error);
    }
    
    // Get all recent errors
    static std::vector<ErrorInfo> getRecentErrors() {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        return recentErrors;
    }
    
    // Clear recent errors
    static void clearRecentErrors() {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        recentErrors.clear();
    }
    
    // Check if there are any errors
    static bool hasErrors() {
        std::lock_guard<std::mutex> lock(callbacksMutex);
        return !recentErrors.empty();
    }
    
    // Set up default error callbacks
    static void setupDefaultCallbacks() {
        // Add console output callback
        addErrorCallback([](const ErrorInfo& error) {
            std::cout << error.format() << std::endl;
        });
        
        // Add error counting callback
        addErrorCallback([](const ErrorInfo& error) {
            std::lock_guard<std::mutex> lock(callbacksMutex);
            recentErrors.push_back(error);
            
            // Keep only recent errors (last 100)
            if (recentErrors.size() > 100) {
                recentErrors.erase(recentErrors.begin());
            }
        });
    }
    
private:
    static std::vector<ErrorCallback> callbacks;
    static std::vector<ErrorInfo> recentErrors;
    static std::mutex callbacksMutex;
};

// Error context manager for adding context to errors
class ErrorContext {
public:
    ErrorContext(const std::string& key, const std::string& value) {
        ErrorHandler::ErrorInfo temp;
        temp.context[key] = value;
        contextStack.push_back(temp.context);
    }
    
    ~ErrorContext() {
        if (!contextStack.empty()) {
            contextStack.pop_back();
        }
    }
    
    // Get current context
    static std::map<std::string, std::string> getCurrentContext() {
        if (contextStack.empty()) {
            return std::map<std::string, std::string>();
        }
        return contextStack.back();
    }
    
    // Add context
    static void addContext(const std::string& key, const std::string& value) {
        ErrorInfo temp;
        temp.context[key] = value;
        contextStack.push_back(temp.context);
    }
    
    // Remove context
    static void removeContext(const std::string& key) {
        if (!contextStack.empty()) {
            contextStack.back().erase(key);
        }
    }
    
private:
    static std::vector<std::map<std::string, std::string>> contextStack;
};

// CLI error handler
class CLIErrorHandler {
public:
    static void handleCLIError(const ErrorInfo& error, bool isInteractive = false) {
        if (isInteractive) {
            // Interactive mode: provide detailed error and suggestions
            std::cout << "\n" << "=" << std::string(60, "=") << std::endl;
            std::cout << "ERROR OCCURRED" << std::endl;
            std::cout << "=" << std::string(60, "=") << std::endl;
            std::cout << error.formatHumanReadable() << std::endl;
            
            // Offer help
            std::cout << "\n" << "Do you want help? (y/n): ";
            std::string response;
            std::getline(std::cin, response);
            
            if (response == "y" || response == "Y") {
                showHelpForError(error);
            }
        } else {
            // Non-interactive mode: just output error
            std::cerr << error.format() << std::endl;
        }
    }
    
    static void handleCLIError(const NLMError& error, bool isInteractive = false) {
        handleCLIError(error.info, isInteractive);
    }
    
private:
    static void showHelpForError(const ErrorInfo& error) {
        std::cout << "\n" << "Available help options:" << std::endl;
        
        if (error.type == ErrorType::ConfigParseError) {
            std::cout << "  - Check your config file syntax" << std::endl;
            std::cout << "  - Use 'nlm config template' for a JSON template" << std::endl;
            std::cout << "  - Use 'nlm config validate <file>' to validate configs" << std::endl;
        } else if (error.type == ErrorType::FileIOError) {
            std::cout << "  - Check if file exists and is readable" << std::endl;
            std::cout << "  - Verify file permissions" << std::endl;
            std::cout << "  - Check available disk space" << std::endl;
        } else if (error.type == ErrorType::MemoryError) {
            std::cout << "  - Try running with less data" << std::endl;
            std::cout << "  - Check system memory usage" << std::endl;
        } else if (error.type == ErrorType::ValidationError) {
            std::cout << "  - Check input values against constraints" << std::endl;
            std::cout << "  - Use 'nlm brain analyze' for validation" << std::endl;
        } else {
            std::cout << "  - Check logs for more details" << std::endl;
            std::cout << "  - Run with '--verbose' flag for more information" << std::endl;
        }
        
        std::cout << "\nGeneral help:" << std::endl;
        std::cout << "  - 'nlm --help' for general CLI help" << std::endl;
        std::cout << "  - 'nlm config --help' for config help" << std::endl;
        std::cout << "  - 'nlm test --help' for test help" << std::endl;
    }
};

// Common error factory methods
namespace Errors {
    inline ErrorInfo configParseError(const std::string& message, const std::string& details = "") {
        return ErrorInfo(ErrorType::ConfigParseError, message + ". " + details,
                        ErrorSeverity::Error);
    }
    
    inline ErrorInfo fileIOError(const std::string& message, const std::string& filepath = "") {
        ErrorInfo error(ErrorType::FileIOError, message + ". File: " + filepath);
        error.suggestion = "Check file permissions and existence";
        return error;
    }
    
    inline ErrorInfo memoryError(const std::string& message) {
        ErrorInfo error(ErrorType::MemoryError, message);
        error.suggestion = "Try reducing the simulation size or closing other applications";
        return error;
    }
    
    inline ErrorInfo validationError(const std::string& message, const std::string& field = "") {
        ErrorInfo error(ErrorType::ValidationError, "Validation error");
        error.details = "Field: " + field + " - " + message;
        error.suggestion = "Check input values against configuration constraints";
        return error;
    }
    
    inline ErrorInfo warning(const std::string& message) {
        return ErrorInfo(ErrorType::UnknownError, message, ErrorSeverity::Warning);
    }
}

// Exception translation for standard exceptions
class ExceptionTranslator {
public:
    static void setupDefaultTranslation() {
        // Add standard exception translators
        ErrorHandler::addErrorCallback([](const ErrorInfo& error) {
            // Check if error message matches known patterns and provide better messages
            std::string msg = error.message;
            if (msg.find("file not found") != std::string::npos) {
                std::string filepath = extractFilepath(msg);
                ErrorHandler::handleError(Errors::fileIOError("File not found", filepath));
            } else if (msg.find("permission denied") != std::string::npos) {
                std::string filepath = extractFilepath(msg);
                ErrorInfo err(ErrorType::PermissionError, "Permission denied accessing file");
                err.details = "File: " + filepath;
                err.suggestion = "Check file permissions or run with appropriate privileges";
                ErrorHandler::handleError(err);
            }
        });
    }
    
private:
    static std::string extractFilepath(const std::string& message) {
        // Simple extraction of filepath from error message
        size_t pos = message.find_last_of('/');
        if (pos != std::string::npos) {
            return message.substr(pos + 1);
        }
        return "unknown";
    }
};

// Initialize static members
std::vector<ErrorHandler::ErrorCallback> ErrorHandler::callbacks;
std::vector<ErrorInfo> ErrorHandler::recentErrors;
std::mutex ErrorHandler::callbacksMutex;

std::vector<std::map<std::string, std::string>> ErrorContext::contextStack;

} // namespace nlm