#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace error {

// Error codes for different error types
enum class ErrorCode {
    // Success
    Success = 0,
    
    // Configuration errors
    ConfigFileNotFound = 100,
    ConfigParseError,
    ConfigValidationError,
    ConfigMissingRequiredField,
    ConfigInvalidValue,
    ConfigDuplicateKey,
    ConfigSchemaMismatch,
    
    // Brain/Neural system errors
    BrainInitializationError,
    NeuronCreationError,
    SynapseCreationError,
    RegionError,
    PopulationError,
    SpikeSystemError,
    PlasticityError,
    
    // Experiment errors
    ExperimentNotFound,
    ExperimentLoadError,
    ExperimentRunError,
    ExperimentSaveError,
    ExperimentInvalidConfig,
    
    // Memory system errors
    MemoryAllocationError,
    MemoryPoolError,
    WorkingMemoryError,
    EpisodicMemoryError,
    AssociativeMemoryError,
    
    // File system errors
    FileNotFound = 200,
    FileReadError,
    FileWriteError,
    DirectoryError,
    PermissionError,
    
    // Network/communication errors
    ConnectionError = 300,
    NetworkTimeout,
    ProtocolError,
    
    // Resource errors
    MemoryError = 400,
    CpuError,
    GpuError,
    DiskError,
    
    // System errors
    SystemError = 500,
    InitializationError,
    RuntimeError,
    TerminationError,
    
    // CLI errors
    CliArgumentError = 600,
    CliCommandNotFound,
    CliSubcommandError,
    CliParseError,
    
    // General errors
    UnknownError = 999,
    NotImplemented,
    OutOfRange,
    InvalidArgument,
    Timeout,
    Interrupted
};

// Error categories
enum class ErrorCategory {
    Configuration,
    NeuralSystem,
    Experiment,
    Memory,
    FileSystem,
    Network,
    Resource,
    System,
    Cli,
    General
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

// Error context information
struct ErrorContext {
    std::string file;
    int line;
    int column;
    std::string function;
    std::map<std::string, std::string> attributes;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext() : file(), line(0), column(0), function() {
        timestamp = std::chrono::system_clock::now();
    }
};

// Error recovery action
enum class RecoveryAction {
    None,
    Retry,
    Fallback,
    Ignore,
    Abort,
    Shutdown,
    Reset,
    Cleanup
};

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCode code, const std::string& message, 
                 const ErrorContext& context = ErrorContext(),
                 const std::string& suggestion = "",
                 RecoveryAction recovery = RecoveryAction::None);
    
    virtual ~NLMException() = default;
    
    ErrorCode getCode() const { return code_; }
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const ErrorContext& getContext() const { return context_; }
    const std::string& getSuggestion() const { return suggestion_; }
    RecoveryAction getRecoveryAction() const { return recovery_; }
    
    std::string getFullMessage() const;
    std::string getLocationInfo() const;
    
protected:
    static ErrorCategory mapCodeToCategory(ErrorCode code);
    static ErrorSeverity mapCodeToSeverity(ErrorCode code);
    
private:
    ErrorCode code_;
    ErrorCategory category_;
    ErrorSeverity severity_;
    ErrorContext context_;
    std::string suggestion_;
    RecoveryAction recovery_;
};

// Specialized exception classes
class ConfigException : public NLMException {
public:
    ConfigException(ErrorCode code, const std::string& message,
                   const ErrorContext& context = ErrorContext(),
                   const std::string& suggestion = "");
};

class BrainException : public NLMException {
public:
    BrainException(ErrorCode code, const std::string& message,
                  const ErrorContext& context = ErrorContext(),
                  const std::string& suggestion = "");
};

class ExperimentException : public NLMException {
public:
    ExperimentException(ErrorCode code, const std::string& message,
                       const ErrorContext& context = ErrorContext(),
                       const std::string& suggestion = "");
};

class MemoryException : public NLMException {
public:
    MemoryException(ErrorCode code, const std::string& message,
                   const ErrorContext& context = ErrorContext(),
                   const std::string& suggestion = "");
};

class FileException : public NLMException {
public:
    FileException(ErrorCode code, const std::string& message,
                 const ErrorContext& context = ErrorContext(),
                 const std::string& suggestion = "");
};

class CliException : public NLMException {
public:
    CliException(ErrorCode code, const std::string& message,
                const ErrorContext& context = ErrorContext(),
                const std::string& suggestion = "");
};

// Error reporting and logging interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    
    virtual void report(const NLMException& error) = 0;
    virtual void log(const std::string& message, ErrorSeverity severity = ErrorSeverity::Info) = 0;
    virtual void clear() = 0;
    
    static ErrorReporter& getDefault();
    static void setDefault(std::unique_ptr<ErrorReporter> reporter);
};

// Console error reporter
class ConsoleErrorReporter : public ErrorReporter {
public:
    void report(const NLMException& error) override;
    void log(const std::string& message, ErrorSeverity severity = ErrorSeverity::Info) override;
    void clear() override;
    
private:
    std::vector<std::string> errorBuffer_;
    std::vector<std::string> logBuffer_;
    
    std::string severityToString(ErrorSeverity severity);
    std::string formatError(const NLMException& error);
};

// File error reporter
class FileErrorReporter : public ErrorReporter {
public:
    FileErrorReporter(const std::string& filepath);
    ~FileErrorReporter() override;
    
    void report(const NLMException& error) override;
    void log(const std::string& message, ErrorSeverity severity = ErrorSeverity::Info) override;
    void clear() override;
    
private:
    std::string filepath_;
    std::ofstream logFile_;
};

// Error recovery manager
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager();
    ~ErrorRecoveryManager() = default;
    
    void registerHandler(ErrorCode code, std::function<bool(const NLMException&)> handler);
    bool handleError(const NLMException& error);
    bool shouldRecover(const NLMException& error) const;
    RecoveryAction getRecoveryAction(const NLMException& error) const;
    
private:
    std::map<ErrorCode, std::function<bool(const NLMException&)>> errorHandlers_;
};

// Exception factory
class ExceptionFactory {
public:
    template<typename T, typename... Args>
    static T create(ErrorCode code, const std::string& message, Args&&... args) {
        ErrorContext context;
        return T(code, message, context, std::forward<Args>(args)...);
    }
    
    static NLMException fromCode(ErrorCode code, const std::string& message,
                                const ErrorContext& context = ErrorContext(),
                                const std::string& suggestion = "");
    
    static std::string errorCodeToString(ErrorCode code);
    static std::string errorCategoryToString(ErrorCategory category);
    static std::string errorSeverityToString(ErrorSeverity severity);
};

// Error utilities
namespace utils {
    std::string formatError(const NLMException& error);
    std::string formatError(const std::exception& error);
    std::string getErrorSummary(const std::vector<NLMException>& errors);
    
    bool shouldRetry(const NLMException& error);
    bool shouldIgnore(const NLMException& error);
    RecoveryAction getRecommendedRecovery(const NLMException& error);
    
    std::string generateErrorId();
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    
    template<typename T>
    T safeExecute(std::function<T()> func, ErrorCode errorCode, const std::string& errorMessage) {
        try {
            return func();
        } catch (const NLMException& e) {
            throw;
        } catch (const std::exception& e) {
            throw ExceptionFactory::create<T>(errorCode, errorMessage + ": " + std::string(e.what()));
        }
    }
    
    template<typename T>
    T safeExecute(std::function<T()> func, const std::string& errorMessage = "Operation failed") {
        try {
            return func();
        } catch (const NLMException& e) {
            throw;
        } catch (const std::exception& e) {
            throw ExceptionFactory::create<T>(ErrorCode::UnknownError, errorMessage + ": " + std::string(e.what()));
        }
    }
}

} // namespace error
} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP