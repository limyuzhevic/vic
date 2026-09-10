#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <system_error>
#include <chrono>
#include <exception>
#include <optional>
#include <sstream>

namespace nlm {

// Error codes for comprehensive error handling
enum class ErrorCode {
    // Success
    Success = 0,
    
    // Configuration errors
    ConfigFileNotFound = 100,
    ConfigFileInvalidFormat,
    ConfigKeyNotFound,
    ConfigTypeMismatch,
    ConfigValueOutOfRange,
    ConfigValidationFailed,
    ConfigParseError,
    
    // File I/O errors
    FileOpenFailed = 200,
    FileWriteFailed,
    FileReadFailed,
    FilePermissionDenied,
    FileCorrupted,
    FileNotFound,
    
    // Neural errors
    NeuralInitializationFailed = 300,
    MemoryAllocationFailed,
    NeuronNotFound,
    SynapseNotFound,
    RegionNotFound,
    InvalidNeuronState,
    InvalidSynapseState,
    
    // System errors
    OutOfMemory = 400,
    InvalidArgument,
    NullPointerDereference,
    DivisionByZero,
    Timeout,
    ResourceUnavailable,
    
    // Checkpoint errors
    CheckpointSaveFailed = 500,
    CheckpointLoadFailed,
    CheckpointValidationFailed,
    CheckpointCorrupted,
    
    // Integration errors
    IntegrationFailed = 600,
    SystemNotReady,
    InvalidConfiguration,
    VersionMismatch,
    
    // General errors
    UnknownError = 999,
    NotImplemented,
    OperationCancelled,
    InternalError
};

// Base exception class with error context and chaining
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCode code, const std::string& message)
        : std::runtime_error(buildMessage(code, message)),
          code_(code),
          timestamp_(std::chrono::system_clock::now()),
          file_(""),
          line_(0),
          function_(""),
          chained_(false) {
    }
    
    NLMException(ErrorCode code, const std::string& message, 
                 const std::string& file, int line, const std::string& function)
        : std::runtime_error(buildMessage(code, message)),
          code_(code),
          timestamp_(std::chrono::system_clock::now()),
          file_(file),
          line_(line),
          function_(function),
          chained_(false) {
    }
    
    NLMException(const NLMException& other) noexcept
        : std::runtime_error(other),
          code_(other.code_),
          timestamp_(other.timestamp_),
          file_(other.file_),
          line_(other.line_),
          function_(other.function_),
          chained_(other.chained_),
          cause_(other.cause_) {
    }
    
    NLMException& operator=(const NLMException& other) noexcept {
        if (this != &other) {
            std::runtime_error::operator=(other);
            code_ = other.code_;
            timestamp_ = other.timestamp_;
            file_ = other.file_;
            line_ = other.line_;
            function_ = other.function_;
            chained_ = other.chained_;
            cause_ = other.cause_;
        }
        return *this;
    }
    
    // Get the error code
    ErrorCode getCode() const noexcept { return code_; }
    
    // Get the timestamp when the exception was created
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return timestamp_; }
    
    // Get the source file where the exception was thrown
    const std::string& getFile() const noexcept { return file_; }
    
    // Get the line number in the source file
    int getLine() const noexcept { return line_; }
    
    // Get the function name where the exception was thrown
    const std::string& getFunction() const noexcept { return function_; }
    
    // Add a chained exception (for debugging)
    void chain(const NLMException& cause) noexcept {
        cause_ = std::make_shared<const NLMException>(cause);
        chained_ = true;
    }
    
    // Check if there's a chained exception
    bool hasCause() const noexcept { return chained_ && cause_ != nullptr; }
    
    // Get the chained exception
    const NLMException* getCause() const noexcept { return cause_.get(); }
    
    // Get detailed error information
    std::string getDetails() const {
        std::ostringstream oss;
        oss << "Error Code: " << static_cast<int>(code_) << "\n";
        oss << "Message: " << what() << "\n";
        if (!file_.empty()) {
            oss << "Location: " << file_;
            if (line_ > 0) oss << ":" << line_;
            oss << " in " << function_ << "\n";
        }
        oss << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp_);
        if (hasCause()) {
            oss << "\nCaused by: " << cause_->what();
            if (!cause_->getFile().empty()) {
                oss << " at " << cause_->getFile();
                if (cause_->getLine() > 0) oss << ":" << cause_->getLine();
            }
        }
        return oss.str();
    }
    
    // Convert to string representation
    std::string toString() const {
        std::string result = what();
        if (!file_.empty()) {
            result += " (" + file_;
            if (line_ > 0) result += ":" + std::to_string(line_);
            result += ")";
        }
        if (hasCause()) {
            result += " -> " + cause_->toString();
        }
        return result;
    }
    
    // Static convenience constructor for macro usage
    static NLMException Create(ErrorCode code, const std::string& message, 
                               const std::string& file = "", 
                               int line = 0, 
                               const std::string& function = "") {
        return NLMException(code, message, file, line, function);
    }

private:
    static std::string buildMessage(ErrorCode code, const std::string& message) {
        std::ostringstream oss;
        oss << "[Error " << static_cast<int>(code) << "] " << message;
        return oss.str();
    }
    
    ErrorCode code_;
    std::chrono::system_clock::time_point timestamp_;
    std::string file_;
    int line_;
    std::string function_;
    bool chained_;
    std::shared_ptr<const NLMException> cause_;
};

// Specialized exceptions for common error cases
class ConfigException : public NLMException {
public:
    ConfigException(ErrorCode code, const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "")
        : NLMException(code, message, file, line, function) {}
};

class FileIOException : public NLMException {
public:
    FileIOException(ErrorCode code, const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "")
        : NLMException(code, message, file, line, function) {}
};

class NeuralException : public NLMException {
public:
    NeuralException(ErrorCode code, const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "")
        : NLMException(code, message, file, line, function) {}
};

// Helper macros for error handling
#define NLM_THROW_ERROR(code, msg) throw nlm::NLMException::Create(code, msg, __FILE__, __LINE__, __func__)

#define NLM_THROW_ERROR_WITH_CAUSE(code, msg, cause) \
    { \
        auto exc = nlm::NLMException::Create(code, msg, __FILE__, __LINE__, __func__); \
        exc.chain(cause); \
        throw exc; \
    }

#define NLM_THROW_IF(condition, code, msg) \
    if (condition) { NLM_THROW_ERROR(code, msg); }

// Exception helper for throwing with context
inline nlm::NLMException throw_with_context(ErrorCode code, const std::string& message) {
    return nlm::NLMException(code, message, __FILE__, __LINE__, __func__);
}

// Function to convert ErrorCode to string for logging
std::string errorCodeToString(ErrorCode code);

// Function to get error message from ErrorCode
std::string getErrorMessage(ErrorCode code);

} // namespace nlm
