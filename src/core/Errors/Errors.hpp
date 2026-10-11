#pragma once

#include <string>
#include <stdexcept>
#include <system_error>
#include <chrono>
#include <map>
#include <memory>

namespace nlm {

// Error codes for different error types
enum class ErrorCode {
    // Configuration errors
    CONFIG_FILE_NOT_FOUND = 1000,
    CONFIG_INVALID_FORMAT = 1001,
    CONFIG_VALIDATION_FAILED = 1002,
    CONFIG_KEY_NOT_FOUND = 1003,
    CONFIG_INVALID_VALUE = 1004,
    
    // File I/O errors
    FILE_OPEN_FAILED = 2000,
    FILE_READ_FAILED = 2001,
    FILE_WRITE_FAILED = 2002,
    FILE_PARSE_FAILED = 2003,
    FILE_CORRUPTED = 2004,
    
    // Neural simulation errors
    NEURAL_SIMULATION_FAILED = 3000,
    NEURAL_INVALID_STATE = 3001,
    NEURAL_MEMORY_EXHAUSTED = 3002,
    NEURAL_INVALID_PARAMETERS = 3003,
    
    // Memory errors
    MEMORY_ALLOCATION_FAILED = 4000,
    MEMORY_OUT_OF_BOUNDS = 4001,
    MEMORY_CORRUPTION = 4002,
    
    // Validation errors
    VALIDATION_FAILED = 5000,
    VALIDATION_CONSTRAINT_VIOLATED = 5001,
    VALIDATION_SCHEMA_MISMATCH = 5002,
    
    // Plasticity errors
    PLASTICITY_RULE_FAILED = 6000,
    PLASTICITY_INVALID_SYNAPSE = 6001,
    PLASTICITY_CONSISTENCY_ERROR = 6002,
    
    // General errors
    UNKNOWN_ERROR = 9999,
    NOT_IMPLEMENTED = 9998,
    TIMEOUT = 9997,
    CANCELLED = 9996
};

// Exception class for error codes
class NLMErrorBase : public std::runtime_error {
protected:
    ErrorCode code_;
    std::string context_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    
public:
    NLMErrorBase(ErrorCode code, const std::string& message) 
        : std::runtime_error(message), code_(code), timestamp_(std::chrono::system_clock::now()) {}
    
    NLMErrorBase(ErrorCode code, const std::string& message, 
                 const std::string& context,
                 const std::string& file = "", int line = 0, 
                 const std::string& function = "")
        : std::runtime_error(message), 
          code_(code),
          context_(context),
          file_(file),
          line_(line),
          function_(function),
          timestamp_(std::chrono::system_clock::now()) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getContext() const { return context_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    std::string getFullMessage() const {
        std::string msg = std::runtime_error::what();
        if (!context_.empty()) {
            msg += " [Context: " + context_ + "]";
        }
        if (!file_.empty()) {
            msg += " [File: " + file_ + ":" + std::to_string(line_) + "]";
        }
        if (!function_.empty()) {
            msg += " [Function: " + function_ + "]";
        }
        return msg;
    }
};

// Base NLMException class
class NLMException : public NLMErrorBase {
public:
    NLMException(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN_ERROR)
        : NLMErrorBase(code, message) {}
    
    NLMException(const std::string& message, const std::string& context,
                 ErrorCode code = ErrorCode::UNKNOWN_ERROR,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : NLMErrorBase(code, message, context, file, line, function) {}
};

// ConfigurationException
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, ErrorCode code = ErrorCode::CONFIG_INVALID_VALUE)
        : NLMException(message, code) {}
    
    ConfigurationException(const std::string& message, const std::string& context,
                           ErrorCode code = ErrorCode::CONFIG_INVALID_VALUE,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : NLMException(message, context, code, file, line, function) {}
};

// FileIOError
class FileIOError : public NLMException {
public:
    FileIOError(const std::string& message, ErrorCode code = ErrorCode::FILE_OPEN_FAILED)
        : NLMException(message, code) {}
    
    FileIOError(const std::string& message, const std::string& context,
                ErrorCode code = ErrorCode::FILE_OPEN_FAILED,
                const std::string& file = "", int line = 0,
                const std::string& function = "")
        : NLMException(message, context, code, file, line, function) {}
};

// NeuralSimulationError
class NeuralSimulationError : public NLMException {
public:
    NeuralSimulationError(const std::string& message, ErrorCode code = ErrorCode::NEURAL_SIMULATION_FAILED)
        : NLMException(message, code) {}
    
    NeuralSimulationError(const std::string& message, const std::string& context,
                          ErrorCode code = ErrorCode::NEURAL_SIMULATION_FAILED,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "")
        : NLMException(message, context, code, file, line, function) {}
};

// MemoryError
class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message, ErrorCode code = ErrorCode::MEMORY_ALLOCATION_FAILED)
        : NLMException(message, code) {}
    
    MemoryError(const std::string& message, const std::string& context,
                ErrorCode code = ErrorCode::MEMORY_ALLOCATION_FAILED,
                const std::string& file = "", int line = 0,
                const std::string& function = "")
        : NLMException(message, context, code, file, line, function) {}
};

// ValidationError
class ValidationError : public NLMException {
public:
    ValidationError(const std::string& message, ErrorCode code = ErrorCode::VALIDATION_FAILED)
        : NLMException(message, code) {}
    
    ValidationError(const std::string& message, const std::string& context,
                    ErrorCode code = ErrorCode::VALIDATION_FAILED,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMException(message, context, code, file, line, function) {}
};

// PlasticityError
class PlasticityError : public NLMException {
public:
    PlasticityError(const std::string& message, ErrorCode code = ErrorCode::PLASTICITY_RULE_FAILED)
        : NLMException(message, code) {}
    
    PlasticityError(const std::string& message, const std::string& context,
                    ErrorCode code = ErrorCode::PLASTICITY_RULE_FAILED,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMException(message, context, code, file, line, function) {}
};

// Error context information structure
struct ErrorContext {
    std::string operation;
    std::string entity;
    std::map<std::string, std::string> metadata;
    
    ErrorContext() = default;
    
    ErrorContext(const std::string& op, const std::string& ent = "")
        : operation(op), entity(ent) {}
    
    void addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    std::string getMetadataSummary() const {
        std::string summary;
        for (const auto& [key, value] : metadata) {
            if (!summary.empty()) summary += ", ";
            summary += key + "=" + value;
        }
        return summary;
    }
};

// Error recovery callback type
using ErrorRecoveryCallback = std::function<bool(const NLMException&)>;

// Error reporting interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void report(const NLMException& error) = 0;
    virtual void reportWarning(const std::string& message) = 0;
};

// Global error reporting registry
class ErrorReporterRegistry {
private:
    static std::vector<std::shared_ptr<ErrorReporter>> reporters_;
    
public:
    static void addReporter(std::shared_ptr<ErrorReporter> reporter) {
        reporters_.push_back(reporter);
    }
    
    static void reportError(const NLMException& error) {
        for (const auto& reporter : reporters_) {
            reporter->report(error);
        }
    }
    
    static void reportWarning(const std::string& message) {
        for (const auto& reporter : reporters_) {
            reporter->reportWarning(message);
        }
    }
    
    static void clearReporters() {
        reporters_.clear();
    }
};

// Enhanced try-catch macro with error reporting
#define NLM_TRY_CATCH(operation, context, errorType) \
    try { \
        operation; \
    } catch (const NLMException& e) { \
        ErrorReporterRegistry::reportError(e); \
        throw; \
    } catch (const std::exception& e) { \
        ErrorContext ctx(context); \
        ctx.addMetadata("original_exception", e.what()); \
        errorType(#operation " failed", ctx.getMetadataSummary(), ErrorCode::UNKNOWN_ERROR); \
        ErrorReporterRegistry::reportError(e); \
        throw; \
    }

// Error recovery macro
#define NLM_TRY_CATCH_WITH_RECOVERY(operation, context, errorType, callback) \
    try { \
        operation; \
    } catch (const NLMException& e) { \
        if (callback && callback(e)) { \
            return; \
        } \
        ErrorReporterRegistry::reportError(e); \
        throw; \
    } catch (const std::exception& e) { \
        ErrorContext ctx(context); \
        ctx.addMetadata("original_exception", e.what()); \
        errorType(#operation " failed", ctx.getMetadataSummary(), ErrorCode::UNKNOWN_ERROR); \
        if (callback && callback(e)) { \
            return; \
        } \
        ErrorReporterRegistry::reportError(e); \
        throw; \
    }

} // namespace nlm
