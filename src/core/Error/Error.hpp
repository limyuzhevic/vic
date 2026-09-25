#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <map>
#include <vector>
#include <sstream>

namespace nlm {

// Error categories for better organization
enum class ErrorCategory {
    None,
    Memory,
    Configuration,
    Simulation,
    Input,
    Output,
    Network,
    Validation,
    Plasticity,
    Neuromodulation,
    Cognitive,
    MemorySystem,
    Developmental,
    Performance,
    FileSystem,
    Threading
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

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message,
                 ErrorCategory category = ErrorCategory::None,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& source = "")
        : std::runtime_error(message)
        , category_(category)
        , severity_(severity)
        , source_(source)
        , timestamp_(std::chrono::system_clock::now()) {}
    
    // Getters
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getSource() const { return source_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    // Format error with all details
    std::string formatError() const {
        std::ostringstream oss;
        oss << "[" << timestampToString(timestamp_) << "] ";
        oss << "[Category: " << errorCategoryToString(category_) << "] ";
        oss << "[Severity: " << errorSeverityToString(severity_) << "] ";
        if (!source_.empty()) {
            oss << "[Source: " << source_ << "] ";
        }
        oss << "Message: " << what();
        return oss.str();
    }
    
protected:
    static std::string timestampToString(std::chrono::system_clock::time_point tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()) % 1000;
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::map<ErrorCategory, std::string> categoryNames = {
            {ErrorCategory::None, "None"},
            {ErrorCategory::Memory, "Memory"},
            {ErrorCategory::Configuration, "Configuration"},
            {ErrorCategory::Simulation, "Simulation"},
            {ErrorCategory::Input, "Input"},
            {ErrorCategory::Output, "Output"},
            {ErrorCategory::Network, "Network"},
            {ErrorCategory::Validation, "Validation"},
            {ErrorCategory::Plasticity, "Plasticity"},
            {ErrorCategory::Neuromodulation, "Neuromodulation"},
            {ErrorCategory::Cognitive, "Cognitive"},
            {ErrorCategory::MemorySystem, "MemorySystem"},
            {ErrorCategory::Developmental, "Developmental"},
            {ErrorCategory::Performance, "Performance"},
            {ErrorCategory::FileSystem, "FileSystem"},
            {ErrorCategory::Threading, "Threading"}
        };
        auto it = categoryNames.find(category);
        return (it != categoryNames.end()) ? it->second : "Unknown";
    }
    
    static std::string errorSeverityToString(ErrorSeverity severity) {
        static const std::map<ErrorSeverity, std::string> severityNames = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARNING"},
            {ErrorSeverity::Error, "ERROR"},
            {ErrorSeverity::Critical, "CRITICAL"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        auto it = severityNames.find(severity);
        return (it != severityNames.end()) ? it->second : "UNKNOWN";
    }
    
private:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string source_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific exception types for common error scenarios
class MemoryAllocationError : public NLMException {
public:
    MemoryAllocationError(const std::string& message, const std::string& source = "")
        : NLMException(message, ErrorCategory::Memory, ErrorSeverity::Error, source) {}
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message, const std::string& source = "")
        : NLMException(message, ErrorCategory::Configuration, ErrorSeverity::Warning, source) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message, const std::string& source = "")
        : NLMException(message, ErrorCategory::Simulation, ErrorSeverity::Error, source) {}
};

class ValidationError : public NLMException {
public:
    ValidationError(const std::string& message, const std::string& source = "")
        : NLMException(message, ErrorCategory::Validation, ErrorSeverity::Warning, source) {}
};

class PlasticityError : public NLMException {
public:
    PlasticityError(const std::string& message, const std::string& source = "")
        : NLMException(message, ErrorCategory::Plasticity, ErrorSeverity::Error, source) {}
};

class NeuromodulationError : public NLMException {
public:
    NeuromodulationError(const std::string& message, const std::string& source = "")
        : NLMException(message, ErrorCategory::Neuromodulation, ErrorSeverity::Warning, source) {}
};

// Error context for better debugging
struct ErrorContext {
    std::string operation;
    std::string file;
    int line;
    std::string function;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> state;
    
    ErrorContext(const std::string& op, const std::string& f = "", int l = 0, const std::string& func = "")
        : operation(op), file(f), line(l), function(func) {}
    
    void addParameter(const std::string& key, const std::string& value) {
        parameters[key] = value;
    }
    
    void addState(const std::string& key, const std::string& value) {
        state[key] = value;
    }
    
    std::string formatContext() const {
        std::ostringstream oss;
        oss << "Operation: " << operation;
        if (!file.empty()) {
            oss << " File: " << file;
            oss << " Line: " << line;
        }
        if (!function.empty()) {
            oss << " Function: " << function;
        }
        if (!parameters.empty()) {
            oss << " Parameters: ";
            for (const auto& [key, value] : parameters) {
                oss << " " << key << "=" << value;
            }
        }
        if (!state.empty()) {
            oss << " State: ";
            for (const auto& [key, value] : state) {
                oss << " " << key << "=" << value;
            }
        }
        return oss.str();
    }
};

// Error handling utilities
class ErrorHandler {
public:
    static std::vector<NLMException> getLastErrors() { return lastErrors_; }
    static void clearLastErrors() { lastErrors_.clear(); }
    static size_t getErrorCount() { return lastErrors_.size(); }
    static size_t getErrorCount(ErrorSeverity severity) {
        return std::count_if(lastErrors_.begin(), lastErrors_.end(),
            [severity](const NLMException& e) { return e.getSeverity() == severity; });
    }
    
    static void addError(const NLMException& error, const ErrorContext& context = ErrorContext("")) {
        // Format error with context
        std::string formattedMessage = error.what();
        if (!context.operation.empty()) {
            formattedMessage += " Context: " + context.formatContext();
        }
        
        NLMException errorWithContext(formattedMessage, error.getCategory(), error.getSeverity(), error.getSource());
        lastErrors_.push_back(errorWithContext);
        
        // Log based on severity
        switch (error.getSeverity()) {
            case ErrorSeverity::Debug:
                logMessage(formattedMessage, "DEBUG");
                break;
            case ErrorSeverity::Info:
                logMessage(formattedMessage, "INFO");
                break;
            case ErrorSeverity::Warning:
                logMessage(formattedMessage, "WARNING");
                break;
            case ErrorSeverity::Error:
                logMessage(formattedMessage, "ERROR");
                break;
            case ErrorSeverity::Critical:
                logMessage(formattedMessage, "CRITICAL");
                break;
            case ErrorSeverity::Fatal:
                logMessage(formattedMessage, "FATAL");
                break;
        }
    }
    
    static void setErrorHandler(std::function<void(const NLMException&)> handler) {
        errorHandler_ = std::move(handler);
    }
    
    static bool hasErrors() { return !lastErrors_.empty(); }
    static bool hasFatalErrors() {
        return std::any_of(lastErrors_.begin(), lastErrors_.end(),
            [](const NLMException& e) { return e.getSeverity() == ErrorSeverity::Fatal; });
    }
    
protected:
    static void logMessage(const std::string& message, const std::string& level) {
        // Default logging to stderr
        std::cerr << "[" << level << "] " << message << std::endl;
        
        if (errorHandler_) {
            // Create a basic NLMException for the handler
            NLMException error(message, ErrorCategory::None, ErrorSeverity::Error);
            errorHandler_(error);
        }
    }
    
private:
    static std::vector<NLMException> lastErrors_;
    static std::function<void(const NLMException&)> errorHandler_;
};

// Helper macro for error checking
#define NLM_CHECK_CONDITION(condition, error_message, error_category) \
    do { \
        if (!(condition)) { \
            nlm::ErrorHandler::addError( \
                nlm::NLMException(error_message, error_category, nlm::ErrorSeverity::Error, __func__), \
                nlm::ErrorContext(#condition, __FILE__, __LINE__, __func__) \
            ); \
            return false; \
        } \
    } while(0)

#define NLM_THROW_ERROR(error_type, message) throw error_type(message, __func__)

} // namespace nlm
