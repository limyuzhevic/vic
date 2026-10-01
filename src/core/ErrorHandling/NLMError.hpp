#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <map>
#include "Logger.hpp"

namespace nlm {

// Error codes for standardized error identification
enum class ErrorCode : int {
    // Success
    Success = 0,
    
    // General errors
    Unknown = 1,
    NotImplemented = 2,
    InvalidArgument = 3,
    OutOfRange = 4,
    NotFound = 5,
    AlreadyExists = 6,
    PermissionDenied = 7,
    ResourceExhausted = 8,
    TimedOut = 9,
    Interrupted = 10,
    
    // File system errors
    FileNotFound = 100,
    FileAccessDenied = 101,
    FileCorrupted = 102,
    FileReadError = 103,
    FileWriteError = 104,
    
    // Neural network errors
    NeuronNotFound = 200,
    SynapseNotFound = 201,
    RegionNotFound = 202,
    InvalidConnection = 203,
    NetworkNotInitialized = 204,
    InvalidNetworkState = 205,
    
    // Memory errors
    MemoryAllocationFailed = 300,
    MemoryCorruption = 301,
    
    // Configuration errors
    ConfigInvalid = 400,
    ConfigFileError = 401,
    
    // Simulation errors
    SimulationNotRunning = 500,
    InvalidTimeStep = 501,
    StepExecutionFailed = 502,
    
    // Integration errors
    IntegrationFailed = 600,
    ComponentNotAvailable = 601,
    
    // Performance errors
    PerformanceThresholdExceeded = 700
};

// Error context for detailed error reporting
struct ErrorContext {
    std::string operation;
    std::string component;
    std::string details;
    std::string suggestion;
    std::map<std::string, std::string> metadata;
    
    ErrorContext(const std::string& op = "", const std::string& comp = "",
                 const std::string& det = "", const std::string& sugg = "")
        : operation(op), component(comp), details(det), suggestion(sugg) {}
};

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                 const ErrorContext& context = ErrorContext());
    
    NLMException(ErrorCode code, const ErrorContext& context = ErrorContext());
    
    ErrorCode getCode() const { return code_; }
    const ErrorContext& getContext() const { return context_; }
    
    std::string getFullMessage() const;
    
protected:
    ErrorCode code_;
    ErrorContext context_;
};

// Specialized exception classes
class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message, const ErrorContext& context = ErrorContext());
};

class FileSystemException : public NLMException {
public:
    FileSystemException(const std::string& message, ErrorCode code, const ErrorContext& context = ErrorContext());
};

class NetworkException : public NLMException {
public:
    NetworkException(const std::string& message, ErrorCode code, const ErrorContext& context = ErrorContext());
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, const ErrorContext& context = ErrorContext());
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, ErrorCode code, const ErrorContext& context = ErrorContext());
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message, ErrorCode code, const ErrorContext& context = ErrorContext());
};

// Error handling utilities
class ErrorHandler {
public:
    static void handle(const NLMException& error);
    static void handle(const std::exception& error);
    static void handle(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext());
    
    static std::string errorCodeToString(ErrorCode code);
    static ErrorCode getLastErrorCode() { return lastErrorCode_; }
    static const std::string& getLastErrorMessage() { return lastErrorMessage_; }
    
    // Error recovery mechanisms
    static bool canRecover(ErrorCode code);
    static bool attemptRecovery(NLMException& error);
    
private:
    static ErrorCode lastErrorCode_;
    static std::string lastErrorMessage_;
};

// Smart error result types
enum class ResultType { Success, Failure, PartialSuccess };

class ResultBase {
public:
    virtual ~ResultBase() = default;
    virtual ResultType getType() const = 0;
    virtual bool isSuccess() const = 0;
    virtual bool isFailure() const = 0;
    virtual bool isPartial() const = 0;
    virtual const NLMException* getError() const = 0;
    virtual const NLMException* getSuccess() const = 0;
};

template<typename T>
class Result : public ResultBase {
public:
    Result(const T& value) : value_(value), type_(ResultType::Success) {}
    Result(T&& value) : value_(std::move(value)), type_(ResultType::Success) {}
    
    Result(const NLMException& error) : error_(error), type_(ResultType::Failure) {}
    
    ResultType getType() const override { return type_; }
    bool isSuccess() const override { return type_ == ResultType::Success; }
    bool isFailure() const override { return type_ == ResultType::Failure; }
    bool isPartial() const override { return type_ == ResultType::PartialSuccess; }
    const NLMException* getError() const override { return isFailure() ? &error_ : nullptr; }
    const NLMException* getSuccess() const override { return isSuccess() ? &value_ : nullptr; }
    
    const T& get() const { return value_; }
    T& get() { return value_; }
    const T& value() const { return value_; }
    T& value() { return value_; }
    
    explicit operator bool() const { return isSuccess(); }
    
private:
    T value_;
    NLMException error_;
    ResultType type_;
};

} // namespace nlm

// Error macros for consistent error reporting
#define NLM_THROW_ERROR(code, message) \
    do { \
        nlm::ErrorContext ctx("THROW_ERROR", "USER", message); \
        nlm::ErrorHandler::handle(nlm::NLMException(code, ctx)); \
        throw nlm::NLMException(code, ctx); \
    } while(0)

#define NLM_THROW_VALIDATION(message) \
    do { \
        nlm::ErrorContext ctx("VALIDATION", "USER", message); \
        nlm::ErrorHandler::handle(nlm::ValidationException(message, ctx)); \
        throw nlm::ValidationException(message, ctx); \
    } while(0)

#define NLM_THROW_FILE_ERROR(code, message, file) \
    do { \
        nlm::ErrorContext ctx("FILE_OP", "FS", message, std::string("Check file: ") + file); \
        nlm::ErrorHandler::handle(nlm::FileSystemException(message, code, ctx)); \
        throw nlm::FileSystemException(message, code, ctx); \
    } while(0)

#define NLM_RETURN_ERROR(code, message) \
    { \
        nlm::ErrorContext ctx("RETURN_ERROR", "USER", message); \
        nlm::ErrorHandler::handle(nlm::NLMException(code, ctx)); \
        return nlm::Result<void>(nlm::NLMException(code, ctx)); \
    }

// Result-style macros
#define NLM_TRY_EXPR(expr) \
    do { \
        auto result = (expr); \
        if (!result.isSuccess()) { \
            nlm::ErrorHandler::handle(*result.getError()); \
            return result; \
        } \
        return result.value(); \
    } while(0)

#define NLM_TRY_BLOCK try
#define NLM_CATCH_ERROR(code, ...) \
    } catch (const nlm::NLMException& e) { \
        return nlm::Result<T>(e); \
    } catch (const std::exception& e) { \
        nlm::ErrorContext ctx("EXCEPT_CATCH", "SYSTEM", e.what()); \
        nlm::ErrorHandler::handle(e); \
        return nlm::Result<T>(nlm::NLMException(code, ctx)); \
    }\
    }}