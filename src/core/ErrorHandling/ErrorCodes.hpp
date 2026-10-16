// Error codes for the NLM brain system
#pragma once

#include <string>
#include <system_error>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <functional>
#include <utility>

namespace nlm {

// Error codes for the NLM brain system
enum class ErrorCode : int {
    // General errors (0-99)
    Success = 0,
    UnknownError = 1,
    NotImplemented = 2,
    InvalidArgument = 3,
    OutOfRange = 4,
    Timeout = 5,
    NotFound = 6,
    AlreadyExists = 7,
    PermissionDenied = 8,
    ResourceExhausted = 9,
    
    // File system errors (100-199)
    FileNotFound = 100,
    FileAccessDenied = 101,
    FileNotReadable = 102,
    FileNotWritable = 103,
    FileCorrupted = 104,
    DirectoryNotFound = 105,
    DirectoryNotEmpty = 106,
    
    // Configuration errors (200-299)
    ConfigFileNotFound = 200,
    ConfigParseError = 201,
    ConfigInvalidValue = 202,
    ConfigMissingRequired = 203,
    ConfigTypeMismatch = 204,
    ConfigCorrupted = 205,
    ConfigValidationFailed = 206,
    
    // Memory errors (300-399)
    MemoryAllocationFailed = 300,
    MemoryAccessViolation = 301,
    MemoryNotInitialized = 302,
    MemoryPoolExhausted = 303,
    OutOfMemory = 304,
    
    // Neural computation errors (400-499)
    NeuronNotFound = 400,
    SynapseNotFound = 401,
    ConnectionFailed = 402,
    PlasticityUpdateFailed = 403,
    SimulationStepFailed = 404,
    NetworkSynchronizationFailed = 405,
    
    // Logger errors (500-599)
    LoggerNotInitialized = 500,
    LogFileError = 501,
    LogInvalidLevel = 502,
    LoggerAlreadyInitialized = 503,
    
    // IO/Network errors (600-699)
    ProtocolError = 601,
    NetworkTimeout = 602,
    
    // Additional errors (700-999)
    OperationAborted = 700,
    ConcurrencyError = 701,
    SerializationError = 702,
    DeserializationError = 703,
    CheckpointFailed = 704,
    RecoveryFailed = 705,
    LoggerError = 506
};

// Error details structure for comprehensive error reporting
struct ErrorDetails {
    ErrorCode code;
    std::string message;
    std::string context;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    ErrorDetails() : code(ErrorCode::UnknownError), line(0) {}
    
    ErrorDetails(ErrorCode c, const std::string& msg, const std::string& ctx = "")
        : code(c), message(msg), context(ctx), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    // Additional constructor with file information
    ErrorDetails(ErrorCode c, const std::string& msg, const std::string& file, 
                 int line, const std::string& function, const std::string& ctx = "")
        : code(c), message(msg), context(ctx), file(file), line(line), function(function) {
        timestamp = std::chrono::system_clock::now();
    }
    
    // Convert to string for logging
    std::string toString() const {
        std::stringstream ss;
        ss << "[Error " << static_cast<int>(code) << "] " << message;
        if (!context.empty()) ss << " (Context: " << context << ")";
        if (!file.empty()) ss << " at " << file << ":" << line << " in " << function;
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        ss << " at " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void report(const ErrorDetails& error) = 0;
    virtual void setErrorLevel(ErrorCode level) = 0;
    virtual bool shouldReport(ErrorCode code) const = 0;
};

// Result type for better error handling
template<typename T>
class Result {
public:
    enum class State { Success, Error };
    
    Result() : state_(State::Error), value_() {}
    
    Result(const T& value) : state_(State::Success), value_(value) {}
    
    Result(const std::string& error) : state_(State::Error), error_(error) {}
    
    bool isSuccess() const { return state_ == State::Success; }
    bool isError() const { return state_ == State::Error; }
    
    const T& value() const { return value_; }
    const std::string& error() const { return error_; }
    
private:
    State state_;
    T value_;
    std::string error_;
};

// Exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const ErrorDetails& details);
    
    const ErrorDetails& getDetails() const { return details_; }
    ErrorCode getCode() const { return details_.code; }
    const std::string& getMessage() const { return details_.message; }
    const std::string& getContext() const { return details_.context; }
    
    // Convert exception to error details
    static ErrorDetails fromException(const std::exception& e);
    
private:
    ErrorDetails details_;
};

// Macros for error handling
#define NLM_THROW_ERROR(code, msg) throw nlm::NLMException(nlm::ErrorDetails(code, msg, __func__))
#define NLM_THROW_ERROR_WITH_CONTEXT(code, msg, context) throw nlm::NLMException(nlm::ErrorDetails(code, msg, context + std::string(" at ") + std::string(__func__)))

#define NLM_REPORT_ERROR(code, msg) nlm::ErrorReporter::getInstance().report(nlm::ErrorDetails(code, msg, __func__))

#define NLM_CHECK_ERROR(cond, code, msg) if (!(cond)) { NLM_THROW_ERROR(code, msg); }

#define NLM_CHECK_ERROR_REPORT(cond, code, msg) if (!(cond)) { NLM_REPORT_ERROR(code, msg); }

} // namespace nlm
