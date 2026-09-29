// error_handling.hpp - Comprehensive error handling for NLM

#pragma once

#include <string>
#include <stdexcept>
#include <system_error>
#include <map>
#include <memory>
#include <vector>

namespace nlm {

// Error categories for NLM
enum class NLMErrorCategory {
    Configuration,
    Memory,
    Simulation,
    Input,
    IO,
    Network,
    Plugin,
    System,
    User,
    Unknown
};

// NLM-specific error codes
enum class NLMErrorCode {
    // Configuration errors
    ConfigFileNotFound = 1000,
    ConfigParseError,
    ConfigValidationError,
    ConfigMissingRequired,
    ConfigInvalidValue,
    
    // Memory errors
    MemoryAllocationFailed,
    MemoryCorruption,
    MemoryLimitExceeded,
    
    // Simulation errors
    SimulationNotInitialized,
    SimulationStepFailed,
    NeuronInvalid,
    SynapseInvalid,
    RegionInvalid,
    
    // Input errors
    InvalidArgument,
    ArgumentOutOfRange,
    InputFormatError,
    
    // IO errors
    FileOpenFailed,
    FileReadError,
    FileWriteError,
    
    // System errors
    SystemCallFailed,
    ThreadError,
    
    // User errors
    UserInterrupt,
    UserExit,
    
    // General errors
    UnknownError,
    NotImplemented,
    FeatureNotAvailable,
    ResourceUnavailable
};

// NLM exception class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             NLMErrorCode code = NLMErrorCode::UnknownError,
             NLMErrorCategory category = NLMErrorCategory::Unknown,
             const std::string& file = "",
             int line = 0,
             const std::string& function = "");
    
    NLMErrorCode getCode() const { return code_; }
    NLMErrorCategory getCategory() const { return category_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    // Get human-readable error category name
    static const char* getCategoryName(NLMErrorCategory category);
    
    // Get human-readable error code name
    static const char* getCodeName(NLMErrorCode code);
    
    // Format the error with full details
    std::string formatFullError() const;
    
    // Check if error is recoverable
    bool isRecoverable() const;
    
    // Check if error is user error
    bool isUserError() const;

private:
    NLMErrorCode code_;
    NLMErrorCategory category_;
    std::string file_;
    int line_;
    std::string function_;
};

// Exception factory functions
class NLMExceptionFactory {
public:
    static std::runtime_error create(const std::string& message,
                                    NLMErrorCode code = NLMErrorCode::UnknownError,
                                    NLMErrorCategory category = NLMErrorCategory::Unknown,
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "");
    
    static std::runtime_error configFileError(const std::string& filepath, const std::string& reason);
    static std::runtime_error configValidationError(const std::string& key, const std::string& expected, const std::string& actual);
    static std::runtime_error memoryError(const std::string& operation, size_t size);
    static std::runtime_error simulationError(const std::string& context, const std::string& reason);
    static std::runtime_error inputError(const std::string& parameter, const std::string& value);
    static std::runtime_error ioError(const std::string& operation, const std::string& filepath);
    static std::runtime_error systemError(const std::string& operation);
};

// Error recovery utilities
class ErrorRecovery {
public:
    enum class RecoveryAction {
        Retry,
        Skip,
        Fallback,
        Exit,
        Abort,
        Continue
    };
    
    static RecoveryAction determineRecoveryAction(const NLMError& error);
    static bool canRecover(const NLMError& error);
    static void logError(const NLMError& error, bool verbose = false);
    static void logWarning(const std::string& message);
    static void logInfo(const std::string& message);
    static void logDebug(const std::string& message);
};

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const NLMError& error) = 0;
    virtual bool shouldExit(const NLMError& error) const = 0;
    virtual RecoveryAction getRecoveryAction(const NLMError& error) = 0;
};

// Default error handler
class DefaultErrorHandler : public IErrorHandler {
public:
    void handleError(const NLMError& error) override;
    bool shouldExit(const NLMError& error) const override;
    RecoveryAction getRecoveryAction(const NLMError& error) override;
};

// Global error handler management
class ErrorHandlerManager {
public:
    static ErrorHandlerManager& getInstance();
    
    void setErrorHandler(std::unique_ptr<IErrorHandler> handler);
    IErrorHandler& getErrorHandler();
    
    void handleError(const NLMError& error);
    bool shouldExit(const NLMError& error);
    RecoveryAction getRecoveryAction(const NLMError& error);
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    bool isVerbose() const { return verbose_; }
    
    // Error summary and statistics
    struct ErrorStats {
        int totalErrors = 0;
        int recoverableErrors = 0;
        int fatalErrors = 0;
        std::map<NLMErrorCode, int> errorCounts;
        std::map<NLMErrorCategory, int> categoryCounts;
    };
    
    ErrorStats getErrorStats() const { return stats_; }
    void resetStats() { stats_ = ErrorStats(); }
    
private:
    ErrorHandlerManager() : verbose_(false) {}
    
    std::unique_ptr<IErrorHandler> errorHandler_;
    bool verbose_;
    ErrorStats stats_;
};

// Error checking macros
#define NLM_CHECK(condition, error_code, message) \
    do { \
        if (!(condition)) { \
            throw NLMExceptionFactory::create(message, error_code, NLMErrorCategory::User, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define NLM_CHECK_PTR(ptr, error_code, message) \
    do { \
        if ((ptr) == nullptr) { \
            throw NLMExceptionFactory::create(message, error_code, NLMErrorCategory::Memory, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define NLM_CHECK_RANGE(value, min, max, error_code, message) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            throw NLMExceptionFactory::create(message, error_code, NLMErrorCategory::Input, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define NLM_THROW(error_code, message) \
    do { \
        throw NLMExceptionFactory::create(message, error_code, NLMErrorCategory::Unknown, __FILE__, __LINE__, __func__); \
    } while(0)

#define NLM_TRY_CATCH(retry_on, action) \
    try { \
        action; \
    } catch (const NLMError& e) { \
        if (ErrorHandlerManager::getInstance().shouldExit(e)) { \
            throw; \
        } \
        RecoveryAction recovery = ErrorHandlerManager::getInstance().getRecoveryAction(e); \
        if (recovery == RecoveryAction::Retry) { \
            /* Implement retry logic here */ \
        } else if (recovery == RecoveryAction::Fallback) { \
            /* Implement fallback logic here */ \
        } else if (recovery == RecoveryAction::Skip) { \
            /* Skip operation and continue */ \
        } else { \
            throw; \
        } \
    }

// Error context manager for better error reporting
class ErrorContext {
public:
    ErrorContext(const std::string& operation, const std::string& details = "");
    ~ErrorContext();
    
    void addDetail(const std::string& key, const std::string& value);
    std::string getContextSummary() const;
    
    // Stack of active error contexts
    static std::vector<ErrorContext*> getContextStack();
    
private:
    std::string operation_;
    std::string details_;
    std::map<std::string, std::string> detailsMap_;
    static thread_local std::vector<ErrorContext*> contextStack_;
};

// Error result type for operations that can fail
enum class ErrorResult {
    Success,
    Warning,
    Error,
    Fatal
};

template<typename T>
class Result {
public:
    Result(T value, ErrorResult status = ErrorResult::Success, const std::string& message = "")
        : value_(std::move(value)), status_(status), message_(message) {}
    
    static Result success(T value) { return Result(std::move(value), ErrorResult::Success, ""); }
    static Result warning(T value, const std::string& message) { return Result(std::move(value), ErrorResult::Warning, message); }
    static Result error(T value, const std::string& message) { return Result(std::move(value), ErrorResult::Error, message); }
    static Result fatal(T value, const std::string& message) { return Result(std::move(value), ErrorResult::Fatal, message); }
    
    bool isSuccess() const { return status_ == ErrorResult::Success; }
    bool isWarning() const { return status_ == ErrorResult::Warning; }
    bool isError() const { return status_ == ErrorResult::Error; }
    bool isFatal() const { return status_ == ErrorResult::Fatal; }
    
    const T& value() const { return value_; }
    T& value() { return value_; }
    
    const std::string& message() const { return message_; }
    
    // Exception conversion
    void throwIfError() const;
    void throwIfFatal() const;
    
private:
    T value_;
    ErrorResult status_;
    std::string message_;
};

// Result utilities
namespace ResultUtils {
    template<typename T>
    Result<T> combineResults(const Result<T>& a, const Result<T>& b, const std::string& operation);
    
    template<typename T>
    Result<T> safeOperation(const std::function<Result<T>()>& operation, const std::string& fallbackMessage = "");
    
    template<typename T, typename U>
    Result<U> transformResult(const Result<T>& result, const std::function<U(const T&)>& transform);
}

} // namespace nlm