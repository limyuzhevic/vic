#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <system_error>
#include <format>
#include "Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Error handling hierarchy for the NLM project
 * 
 * This error handling system provides:
 * - Proper exception hierarchy with specific error types
 * - Error context preservation for debugging
 * - Graceful failure modes and recovery mechanisms
 * - Comprehensive error logging and reporting
 */

// Base error class for all NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error(message), file_(""), line_(0), timestamp_(std::chrono::system_clock::now()) {}
    
    NLMError(const std::string& message, const std::string& file, int line)
        : std::runtime_error(message), file_(file), line_(line), timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMError() = default;
    
    // Get error category
    virtual const char* category() const noexcept { return "NLM"; }
    
    // Get file where error occurred
    const std::string& getFile() const noexcept { return file_; }
    
    // Get line number where error occurred
    int getLine() const noexcept { return line_; }
    
    // Get timestamp
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return timestamp_; }
    
    // Get full error report with context
    virtual std::string report() const {
        std::ostringstream oss;
        oss << "[" << category() << "] " << what();
        if (!file_.empty()) {
            oss << " (at " << file_ << ":" << line_ << ")";
        }
        return oss.str();
    }
    
protected:
    std::string file_;
    int line_;
    std::chrono::system_clock::time_point timestamp_;
};

// Configuration-related errors
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError(message) {}
    ConfigError(const std::string& message, const std::string& file, int line) 
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Config"; }
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError(message) {}
    ConfigValidationError(const std::string& message, const std::string& file, int line)
        : ConfigError(message, file, line) {}
    
    const char* category() const noexcept override { return "ConfigValidation"; }
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message) : ConfigError(message) {}
    ConfigParseError(const std::string& message, const std::string& file, int line)
        : ConfigError(message, file, line) {}
    
    const char* category() const noexcept override { return "ConfigParse"; }
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& message) : ConfigError(message) {}
    ConfigFileError(const std::string& message, const std::string& file, int line)
        : ConfigError(message, file, line) {}
    
    const char* category() const noexcept override { return "ConfigFile"; }
};

// Brain-related errors
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError(message) {}
    BrainError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Brain"; }
};

class BrainInitializationError : public BrainError {
public:
    explicit BrainInitializationError(const std::string& message) : BrainError(message) {}
    BrainInitializationError(const std::string& message, const std::string& file, int line)
        : BrainError(message, file, line) {}
    
    const char* category() const noexcept override { return "BrainInitialization"; }
};

class BrainStepError : public BrainError {
public:
    explicit BrainStepError(const std::string& message) : BrainError(message) {}
    BrainStepError(const std::string& message, const std::string& file, int line)
        : BrainError(message, file, line) {}
    
    const char* category() const noexcept override { return "BrainStep"; }
};

// Memory system errors
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError(message) {}
    MemoryError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Memory"; }
};

class MemoryValidationError : public MemoryError {
public:
    explicit MemoryValidationError(const std::string& message) : MemoryError(message) {}
    MemoryValidationError(const std::string& message, const std::string& file, int line)
        : MemoryError(message, file, line) {}
    
    const char* category() const noexcept override { return "MemoryValidation"; }
};

// Neuromodulator errors
class NeuromodulatorError : public NLMError {
public:
    explicit NeuromodulatorError(const std::string& message) : NLMError(message) {}
    NeuromodulatorError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Neuromodulator"; }
};

// Network/interface errors
class NetworkError : public NLMError {
public:
    explicit NetworkError(const std::string& message) : NLMError(message) {}
    NetworkError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Network"; }
};

// Input validation errors
class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError(message) {}
    ValidationError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Validation"; }
};

// Recovery failure errors
class RecoveryError : public NLMError {
public:
    explicit RecoveryError(const std::string& message) : NLMError(message) {}
    RecoveryError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "Recovery"; }
};

// General system errors
class SystemError : public NLMError {
public:
    explicit SystemError(const std::string& message) : NLMError(message) {}
    SystemError(const std::string& message, const std::string& file, int line)
        : NLMError(message, file, line) {}
    
    const char* category() const noexcept override { return "System"; }
};

// Error context for debugging
struct ErrorContext {
    std::string operation;
    std::string component;
    std::string details;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    
    ErrorContext() : timestamp(std::chrono::system_clock::now()) {}
};

// Error reporting and debugging utilities
class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    // Report an error with full context
    void report(const NLMError& error, const ErrorContext& context = ErrorContext()) {
        // Log the error with full context
        NLM_LOG_ERROR(error.report());
        
        if (!context.operation.empty()) {
            NLM_LOG_ERROR("Operation: " + context.operation);
        }
        if (!context.component.empty()) {
            NLM_LOG_ERROR("Component: " + context.component);
        }
        if (!context.details.empty()) {
            NLM_LOG_ERROR("Details: " + context.details);
        }
        
        // Add to error history for debugging
        errorHistory_.push_back(error);
    }
    
    // Get recent errors
    std::vector<NLMError> getRecentErrors(size_t count = 10) const {
        if (errorHistory_.size() <= count) {
            return errorHistory_;
        }
        auto start = errorHistory_.end() - count;
        return std::vector<NLMError>(start, errorHistory_.end());
    }
    
    // Clear error history
    void clearHistory() { errorHistory_.clear(); }
    
    // Get total error count
    size_t getErrorCount() const { return errorHistory_.size(); }
    
private:
    ErrorReporter() = default;
    ~ErrorReporter() = default;
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;
    
    std::vector<NLMError> errorHistory_;
};

// Macro for convenient error handling
#define THROW_NLM_ERROR(msg) throw nlm::NLMError(msg, __FILE__, __LINE__)
#define THROW_CONFIG_ERROR(msg) throw nlm::ConfigError(msg, __FILE__, __LINE__)
#define THROW_VALIDATION_ERROR(msg) throw nlm::ValidationError(msg, __FILE__, __LINE__)
#define THROW_MEMORY_ERROR(msg) throw nlm::MemoryError(msg, __FILE__, __LINE__)
#define CATCH_AND_LOG() \
    catch (const nlm::NLMError& e) { \
        NLM_LOG_ERROR(e.report()); \
        throw; \
    } \
    catch (const std::exception& e) { \
        NLM_LOG_ERROR(std::string("System exception: ") + e.what()); \
        throw nlm::SystemError(std::string("System exception: ") + e.what(), __FILE__, __LINE__); \
    }

} // namespace nlm
