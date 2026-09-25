// Enhanced Error Handling System for NLM Phase 6
// Comprehensive error handling with proper hierarchy, recovery mechanisms, and user-friendly messages

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <chrono>
#include <memory>
#include <sstream>
#include <iomanip>

namespace nlm {

// Error Severity Levels
enum class ErrorSeverity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};

// Error Categories
enum class ErrorCategory {
    None,
    Configuration,
    FileSystem,
    Resource,
    Network,
    Memory,
    InputValidation,
    System,
    Runtime,
    Plugin,
    Visualization,
    Integration
};

// Base NLM Error Class
class NLMError : public std::runtime_error {
protected:
    std::string errorCode;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> context;
    std::vector<std::string> suggestions;
    int lineNumber;
    std::string fileName;
    
public:
    NLMError(const std::string& message, ErrorSeverity sev = ErrorSeverity::ERROR, 
             ErrorCategory cat = ErrorCategory::Runtime,
             const std::string& component = "",
             const std::string& errorCode = "ERR_GENERIC",
             int lineNumber = 0, const std::string& fileName = "")
        : std::runtime_error(message), errorCode(errorCode), severity(sev), 
          category(cat), component(component), timestamp(std::chrono::system_clock::now()),
          lineNumber(lineNumber), fileName(fileName) {}
    
    virtual ~NLMError() = default;
    
    // Getters
    const std::string& getErrorCode() const { return errorCode; }
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    const std::string& getComponent() const { return component; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    int getLineNumber() const { return lineNumber; }
    const std::string& getFileName() const { return fileName; }
    
    // Context and suggestions
    void addContext(const std::string& ctx) { context.push_back(ctx); }
    void addSuggestion(const std::string& suggestion) { suggestions.push_back(suggestion); }
    const std::vector<std::string>& getContext() const { return context; }
    const std::vector<std::string>& getSuggestions() const { return suggestions; }
    
    // Format error message for users
    virtual std::string formatMessage() const {
        std::stringstream ss;
        ss << "[" << errorCode << "] " << what();
        if (!component.empty()) {
            ss << " (Component: " << component << ")";
        }
        if (lineNumber > 0) {
            ss << " at line " << lineNumber;
        }
        if (!fileName.empty()) {
            ss << " in file " << fileName;
        }
        return ss.str();
    }
    
    // Format detailed error for debugging
    virtual std::string formatDetails() const {
        std::stringstream ss;
        ss << "=== NLM Error Details ===" << std::endl;
        ss << "Code: " << errorCode << std::endl;
        ss << "Severity: " << static_cast<int>(severity) << std::endl;
        ss << "Category: " << static_cast<int>(category) << std::endl;
        ss << "Component: " << component << std::endl;
        ss << "Time: " << std::chrono::system_clock::to_time_t(timestamp) << std::endl;
        if (!fileName.empty()) {
            ss << "Location: " << fileName;
            if (lineNumber > 0) ss << ":" << lineNumber;
            ss << std::endl;
        }
        ss << "Message: " << what() << std::endl;
        
        if (!context.empty()) {
            ss << "Context:" << std::endl;
            for (const auto& ctx : context) {
                ss << "  - " << ctx << std::endl;
            }
        }
        
        if (!suggestions.empty()) {
            ss << "Suggestions:" << std::endl;
            for (const auto& suggestion : suggestions) {
                ss << "  - " << suggestion << std::endl;
            }
        }
        
        return ss.str();
    }
};

// Specific NLM Error Types
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message, ErrorSeverity sev = ErrorSeverity::ERROR,
                const std::string& component = "Config",
                const std::string& errorCode = "CONFIG_ERR",
                int lineNumber = 0, const std::string& fileName = "")
        : NLMError(message, sev, ErrorCategory::Configuration, component, errorCode, lineNumber, fileName) {}
};

class FileError : public NLMError {
public:
    FileError(const std::string& message, ErrorSeverity sev = ErrorSeverity::ERROR,
              const std::string& component = "FileSystem",
              const std::string& errorCode = "FILE_ERR",
              int lineNumber = 0, const std::string& fileName = "")
        : NLMError(message, sev, ErrorCategory::FileSystem, component, errorCode, lineNumber, fileName) {}
};

class ResourceError : public NLMError {
public:
    ResourceError(const std::string& message, ErrorSeverity sev = ErrorSeverity::ERROR,
                  const std::string& component = "Resource",
                  const std::string& errorCode = "RESOURCE_ERR",
                  int lineNumber = 0, const std::string& fileName = "")
        : NLMError(message, sev, ErrorCategory::Resource, component, errorCode, lineNumber, fileName) {}
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message, ErrorSeverity sev = ErrorSeverity::WARNING,
                    const std::string& component = "Validation",
                    const std::string& errorCode = "VALIDATION_ERR",
                    int lineNumber = 0, const std::string& fileName = "")
        : NLMError(message, sev, ErrorCategory::InputValidation, component, errorCode, lineNumber, fileName) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, ErrorSeverity sev = ErrorSeverity::CRITICAL,
                const std::string& component = "Memory",
                const std::string& errorCode = "MEMORY_ERR",
                int lineNumber = 0, const std::string& fileName = "")
        : NLMError(message, sev, ErrorCategory::Memory, component, errorCode, lineNumber, fileName) {}
};

class VisualizationError : public NLMError {
public:
    VisualizationError(const std::string& message, ErrorSeverity sev = ErrorSeverity::ERROR,
                       const std::string& component = "Visualization",
                       const std::string& errorCode = "VIZ_ERR",
                       int lineNumber = 0, const std::string& fileName = "")
        : NLMError(message, sev, ErrorCategory::Visualization, component, errorCode, lineNumber, fileName) {}
};

// Exception handling utilities
class ErrorHandler {
public:
    // Convert exception to string
    static std::string exceptionToString(const std::exception& e) {
        std::stringstream ss;
        ss << "Exception: " << e.what();
        return ss.str();
    }
    
    // Safe execution with error catching
    template<typename Func>
    static bool executeSafely(Func&& func, const std::string& errorContext = "") {
        try {
            func();
            return true;
        } catch (const NLMError& e) {
            logError(e, errorContext);
            return false;
        } catch (const std::exception& e) {
            NLMError nlme(std::string("Standard exception: ") + e.what(), 
                         ErrorSeverity::ERROR, ErrorCategory::Runtime, "ErrorHandler", "STD_EXC");
            nlme.addContext(errorContext);
            logError(nlme, errorContext);
            return false;
        } catch (...) {
            NLMError nlme("Unknown exception occurred", 
                         ErrorSeverity::FATAL, ErrorCategory::System, "ErrorHandler", "UNKNOWN_EXC");
            nlme.addContext(errorContext);
            logError(nlme, errorContext);
            return false;
        }
    }
    
    // Log error with enhanced formatting
    static void logError(const NLMError& error, const std::string& context = "") {
        std::stringstream ss;
        ss << "[" << getSeverityString(error.getSeverity()) << "] " 
           << error.formatMessage();
        if (!context.empty()) {
            ss << " | Context: " << context;
        }
        
        // Output to stderr for errors
        std::cerr << ss.str() << std::endl;
        
        // Store in error log for debugging
        errorLog.push_back(error);
    }
    
    // Get all logged errors
    static const std::vector<NLMError>& getErrorLog() { return errorLog; }
    
    // Clear error log
    static void clearErrorLog() { errorLog.clear(); }
    
    // Get severity string
    static std::string getSeverityString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::DEBUG: return "DEBUG";
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARNING";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::CRITICAL: return "CRITICAL";
            case ErrorSeverity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    // Get category string
    static std::string getCategoryString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::None: return "None";
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::FileSystem: return "FileSystem";
            case ErrorCategory::Resource: return "Resource";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::InputValidation: return "InputValidation";
            case ErrorCategory::System: return "System";
            case ErrorCategory::Plugin: return "Plugin";
            case ErrorCategory::Visualization: return "Visualization";
            case ErrorCategory::Integration: return "Integration";
            default: return "Unknown";
        }
    }
    
private:
    static std::vector<NLMError> errorLog;
};

// NLM-style logging macros (maintain backward compatibility)
#define NLM_THROW_ERROR(error) throw error

#define NLM_LOG_ERROR(msg) do { \
    nlm::NLMError error(msg, nlm::ErrorSeverity::ERROR); \
    nlm::ErrorHandler::logError(error, ""); \
} while(0)

#define NLM_LOG_WARNING(msg) do { \
    nlm::NLMError error(msg, nlm::ErrorSeverity::WARNING); \
    nlm::ErrorHandler::logError(error, ""); \
} while(0)

#define NLM_LOG_INFO(msg) do { \
    nlm::NLMError error(msg, nlm::ErrorSeverity::INFO); \
    nlm::ErrorHandler::logError(error, ""); \
} while(0)

#define NLM_LOG_DEBUG(msg) do { \
    nlm::NLMError error(msg, nlm::ErrorSeverity::DEBUG); \
    nlm::ErrorHandler::logError(error, ""); \
} while(0)

#define NLM_CHECK(condition, error_msg, error_code) do { \
    if (!(condition)) { \
        throw nlm::ValidationError(error_msg, nlm::ErrorSeverity::ERROR, "Validation", error_code); \
    } \
} while(0)

#define NLM_CHECK_FILE_EXISTS(path) do { \
    if (!std::filesystem::exists(path)) { \
        throw nlm::FileError("File does not exist: " + path, nlm::ErrorSeverity::ERROR, "FileSystem", "FILE_NOT_FOUND"); \
    } \
} while(0)

#define NLM_CHECK_RESOURCE_AVAILABLE(resource) do { \
    if (!resource) { \
        throw nlm::ResourceError("Resource not available: " + std::string(#resource), \
                                nlm::ErrorSeverity::CRITICAL, "Resource", "RESOURCE_UNAVAILABLE"); \
    } \
} while(0)

// User-friendly error message helpers
inline std::string getBeginnerFriendlyError(const NLMError& error) {
    std::string friendlyMsg;
    
    if (error.getCategory() == ErrorCategory::Configuration) {
        friendlyMsg = "Configuration Error: ";
        if (error.getErrorCode() == "CONFIG_FILE_NOT_FOUND") {
            friendlyMsg += "Configuration file not found. Please ensure your config file exists and is accessible.";
        } else if (error.getErrorCode() == "CONFIG_INVALID_FORMAT") {
            friendlyMsg += "Configuration file has invalid format. Please check the file syntax and structure.";
        } else {
            friendlyMsg += error.what();
        }
    }
    else if (error.getCategory() == ErrorCategory::FileSystem) {
        friendlyMsg = "File Error: ";
        if (error.getErrorCode() == "FILE_NOT_FOUND") {
            friendlyMsg += "The required file cannot be found. Please check the file path and permissions.";
        } else if (error.getErrorCode() == "FILE_PERMISSION_DENIED") {
            friendlyMsg += "Permission denied when accessing the file. Please check file permissions.";
        } else {
            friendlyMsg += error.what();
        }
    }
    else if (error.getCategory() == ErrorCategory::InputValidation) {
        friendlyMsg = "Input Validation Error: ";
        friendlyMsg += error.what();
        friendlyMsg += "\nPlease check your input values and try again.";
    }
    else if (error.getCategory() == ErrorCategory::Memory) {
        friendlyMsg = "Memory Error: ";
        friendlyMsg += "Not enough memory to complete the operation. ";
        friendlyMsg += "Try reducing the simulation size or closing other applications.";
    }
    else {
        friendlyMsg = error.what();
    }
    
    return friendlyMsg;
}

inline std::string getPowerUserError(const NLMError& error) {
    std::string detailedMsg;
    detailedMsg += "Error Code: " + error.getErrorCode() + "\n";
    detailedMsg += "Severity: " + ErrorHandler::getSeverityString(error.getSeverity()) + "\n";
    detailedMsg += "Category: " + ErrorHandler::getCategoryString(error.getCategory()) + "\n";
    detailedMsg += "Component: " + error.getComponent() + "\n";
    detailedMsg += "Timestamp: " + std::to_string(std::chrono::system_clock::to_time_t(error.getTimestamp())) + "\n";
    detailedMsg += "Location: ";
    if (!error.getFileName().empty()) {
        detailedMsg += error.getFileName();
        if (error.getLineNumber() > 0) {
            detailedMsg += ":" + std::to_string(error.getLineNumber());
        }
        detailedMsg += "\n";
    }
    detailedMsg += "Message: " + std::string(error.what()) + "\n";
    
    if (!error.getContext().empty()) {
        detailedMsg += "Context:\n";
        for (const auto& ctx : error.getContext()) {
            detailedMsg += "  " + ctx + "\n";
        }
    }
    
    if (!error.getSuggestions().empty()) {
        detailedMsg += "Suggestions:\n";
        for (const auto& suggestion : error.getSuggestions()) {
            detailedMsg += "  " + suggestion + "\n";
        }
    }
    
    return detailedMsg;
}
}