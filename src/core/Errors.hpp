// Error Hierarchy for NLM
// Comprehensive error hierarchy with base exception classes and derived types

#ifndef NLM_ERRORS_HPP
#define NLM_ERRORS_HPP

#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <exception>
#include <sstream>

// Error Categories for logging and categorization
enum class ErrorCategory {
    CORE,
    CONFIGURATION,
    MEMORY,
    VALIDATION,
    FILESYSTEM,
    NEURAL,
    WORLD,
    PYTHON,
    PERFORMANCE
};

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

enum class LogCategory {
    PERFORMANCE,
    VALIDATION,
    MEMORY,
    NEURAL,
    WORLD,
    PYTHON
};

// Source location information for errors
struct SourceLocation {
    std::string file;
    int line;
    std::string function;
    
    SourceLocation() : file("unknown"), line(0), function("unknown") {}
    SourceLocation(const std::string& f, int l, const std::string& fn) 
        : file(f), line(l), function(fn) {}
};

// Error context information for better debugging
struct ErrorContext {
    SourceLocation location;
    std::string operation;
    std::unordered_map<std::string, std::string> values;
    std::string state;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    
    ErrorContext() : operation("unknown"), state("unknown"), timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedMessage() const {
        std::stringstream ss;
        ss << "[" << operation << "] ";
        ss << "At " << location.file << ":" << location.line << " ";
        if (!location.function.empty()) {
            ss << "in " << location.function << " ";
        }
        ss << "State: " << state << ". ";
        for (const auto& pair : values) {
            ss << pair.first << "=" << pair.second << "; ";
        }
        return ss.str();
    }
};

// Base exception class
class NLMException : public std::exception {
protected:
    std::string message;
    ErrorContext context;
    ErrorCategory category;
    LogLevel severity;
    mutable char* formattedMessage = nullptr;
    
public:
    NLMException(const std::string& msg, ErrorCategory cat = ErrorCategory::CORE, 
                 LogLevel sev = LogLevel::ERROR,
                 const SourceLocation& loc = SourceLocation(),
                 const std::string& op = "unknown")
        : message(msg), category(cat), severity(sev) {
        context.location = loc;
        context.operation = op;
        context.timestamp = std::chrono::system_clock::now();
    }
    
    virtual ~NLMException() noexcept {}
    
    virtual const char* what() const noexcept override {
        if (formattedMessage == nullptr) {
            std::stringstream ss;
            ss << "[" << getCategoryString(category) << "] " << getSeverityString(severity);
            ss << " " << context.getFormattedMessage() << message;
            size_t len = ss.str().length();
            formattedMessage = new char[len + 1];
            std::strcpy(formattedMessage, ss.str().c_str());
        }
        return formattedMessage;
    }
    
    virtual const std::string& getMessage() const { return message; }
    virtual ErrorCategory getCategory() const { return category; }
    virtual LogLevel getSeverity() const { return severity; }
    virtual const ErrorContext& getContext() const { return context; }
    
    virtual std::string getCategoryString(ErrorCategory cat) const {
        static const std::map<ErrorCategory, std::string> categoryStrings = {
            {ErrorCategory::CORE, "CORE"},
            {ErrorCategory::CONFIGURATION, "CONFIGURATION"},
            {ErrorCategory::MEMORY, "MEMORY"},
            {ErrorCategory::VALIDATION, "VALIDATION"},
            {ErrorCategory::FILESYSTEM, "FILESYSTEM"},
            {ErrorCategory::NEURAL, "NEURAL"},
            {ErrorCategory::WORLD, "WORLD"},
            {ErrorCategory::PYTHON, "PYTHON"},
            {ErrorCategory::PERFORMANCE, "PERFORMANCE"}
        };
        auto it = categoryStrings.find(cat);
        return it != categoryStrings.end() ? it->second : "UNKNOWN";
    }
    
    virtual std::string getSeverityString(LogLevel sev) const {
        static const std::map<LogLevel, std::string> severityStrings = {
            {LogLevel::DEBUG, "DEBUG"},
            {LogLevel::INFO, "INFO"},
            {LogLevel::WARNING, "WARNING"},
            {LogLevel::ERROR, "ERROR"},
            {LogLevel::CRITICAL, "CRITICAL"}
        };
        auto it = severityStrings.find(sev);
        return it != severityStrings.end() ? it->second : "UNKNOWN";
    }
};

// Configuration-related errors
class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                      const std::string& op = "config_operation")
        : NLMException(msg, ErrorCategory::CONFIGURATION, LogLevel::ERROR, loc, op) {}
};

// Memory allocation errors
class MemoryError : public NLMException {
public:
    MemoryError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                const std::string& op = "memory_operation")
        : NLMException(msg, ErrorCategory::MEMORY, LogLevel::CRITICAL, loc, op) {}
};

// Input validation errors
class ValidationError : public NLMException {
public:
    ValidationError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                    const std::string& op = "validation")
        : NLMException(msg, ErrorCategory::VALIDATION, LogLevel::WARNING, loc, op) {}
};

// File I/O errors
class FileSystemError : public NLMException {
public:
    FileSystemError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                    const std::string& op = "filesystem_operation")
        : NLMException(msg, ErrorCategory::FILESYSTEM, LogLevel::ERROR, loc, op) {}
};

// Neural simulation errors
class NeuronError : public NLMException {
public:
    NeuronError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                const std::string& op = "neuron_operation")
        : NLMException(msg, ErrorCategory::NEURAL, LogLevel::ERROR, loc, op) {}
};

// World/environment errors
class WorldError : public NLMException {
public:
    WorldError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
               const std::string& op = "world_operation")
        : NLMException(msg, ErrorCategory::WORLD, LogLevel::ERROR, loc, op) {}
};

// Python binding errors
class PythonBindingError : public NLMException {
public:
    PythonBindingError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                      const std::string& op = "python_binding")
        : NLMException(msg, ErrorCategory::PYTHON, LogLevel::ERROR, loc, op) {}
};

// Performance-related errors
class PerformanceError : public NLMException {
public:
    PerformanceError(const std::string& msg, const SourceLocation& loc = SourceLocation(),
                     const std::string& op = "performance_operation")
        : NLMException(msg, ErrorCategory::PERFORMANCE, LogLevel::WARNING, loc, op) {}
};

#endif // NLM_ERRORS_HPP