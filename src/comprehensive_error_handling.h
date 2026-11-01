// comprehensive_error_handling.h - Comprehensive error handling and logging for NLM
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Error categories
enum class ErrorCategory {
    None,
    Configuration,
    Initialization,
    Simulation,
    Memory,
    Plasticity,
    Neuromodulation,
    Prediction,
    Cognition,
    Development,
    IO,
    System,
    Network,
    Performance,
    Security
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

// Error codes for specific error conditions
enum class ErrorCode {
    // Configuration errors
    ConfigFileNotFound = 1000,
    ConfigParseError,
    ConfigValidationError,
    InvalidParameter,
    
    // Initialization errors
    BrainInitializationFailed,
    MemorySystemNotAvailable,
    PlasticitySystemNotAvailable,
    NeuromodulationSystemNotAvailable,
    PredictionSystemNotAvailable,
    CognitionSystemNotAvailable,
    DevelopmentSystemNotAvailable,
    
    // Simulation errors
    SimulationStepFailed,
    InvalidNeuronState,
    SynapseError,
    SpikePropagationError,
    NetworkStabilityError,
    
    // Memory errors
    MemoryAllocationFailed,
    MemoryCorruption,
    MemoryLeakDetected,
    MemoryCapacityExceeded,
    MemoryConsistencyError,
    
    // Plasticity errors
    STDPError,
    HebbianError,
    StructuralPlasticityError,
    PlasticityLearningRateError,
    
    // Neuromodulation errors
    DopamineSystemError,
    CuriositySystemError,
    NoveltySystemError,
    PredictionErrorSystem,
    NeuromodulatorInteractionError,
    
    // Prediction errors
    PredictionSystemFailure,
    PatternRecognitionError,
    ConfidenceCalculationError,
    PredictionAccuracyError,
    
    // Cognition errors
    PlanningError,
    ConceptFormationError,
    AttentionError,
    DecisionMakingError,
    CognitiveLoadExceeded,
    
    // Development errors
    DevelopmentalStageError,
    MaturationError,
    PlasticityRateError,
    StructuralDevelopmentError,
    
    // IO errors
    FileReadError,
    FileWriteError,
    CheckpointSaveError,
    CheckpointLoadError,
    SerializationError,
    DeserializationError,
    
    // System errors
    OutOfMemory,
    DivisionByZero,
    InvalidPointer,
    ThreadCreationError,
    ResourceExhaustion,
    
    // Network errors
    NetworkPartition,
    ConnectivityError,
    LatencyError,
    BandwidthError,
    
    // Performance errors
    PerformanceDegradation,
    ThroughputError,
    LatencyError,
    MemoryEfficiencyError,
    
    // Security errors
    UnauthorizedAccess,
    DataCorruption,
    IntegrityViolation,
    
    // Generic errors
    UnknownError,
    NotImplemented,
    FeatureNotAvailable,
    CompatibilityError,
    ConfigurationError
};

// Error context information
struct ErrorContext {
    std::string component;
    std::string method;
    std::string file;
    int line;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    
    ErrorContext() : component(""), method(""), file(""), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(const std::string& comp, const std::string& meth, 
                 const std::string& filePath, int lineNum)
        : component(comp), method(meth), file(filePath), line(lineNum) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCode code = ErrorCode::UnknownError,
                 ErrorCategory category = ErrorCategory::None,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 const ErrorContext& context = ErrorContext())
        : std::runtime_error(message), errorCode_(code), errorCategory_(category),
          errorSeverity_(severity), context_(context) {}
    
    ErrorCode getErrorCode() const { return errorCode_; }
    ErrorCategory getErrorCategory() const { return errorCategory_; }
    ErrorSeverity getErrorSeverity() const { return errorSeverity_; }
    const ErrorContext& getContext() const { return context_; }
    
    std::string getFullMessage() const {
        std::stringstream ss;
        ss << "[" << static_cast<int>(errorSeverity_) << "] " << what() << "\n";
        ss << "  Error Code: " << static_cast<int>(errorCode_) << "\n";
        ss << "  Category: " << static_cast<int>(errorCategory_) << "\n";
        ss << "  Component: " << context_.component << "\n";
        ss << "  Method: " << context_.method << "\n";
        ss << "  Location: " << context_.file << ":" << context_.line << "\n";
        if (!context_.stackTrace.empty()) {
            ss << "  Stack Trace:\n" << context_.stackTrace;
        }
        return ss.str();
    }
    
    std::string getCategoryString() const;
    std::string getSeverityString() const;
    
private:
    ErrorCode errorCode_;
    ErrorCategory errorCategory_;
    ErrorSeverity errorSeverity_;
    ErrorContext context_;
};

// Error handling strategy
enum class ErrorHandlingStrategy {
    None,
    Continue,
    Retry,
    Recover,
    Warn,
    Error,
    Abort,
    GracefulShutdown
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual bool handleError(const NLMException& error, const ErrorContext& context) = 0;
    virtual void setErrorHandlingStrategy(ErrorHandlingStrategy strategy) = 0;
    virtual ErrorHandlingStrategy getErrorHandlingStrategy() const = 0;
    virtual void addErrorCallback(std::function<void(const NLMException&)> callback) = 0;
    virtual void removeErrorCallback(std::function<void(const NLMException&)> callback) = 0;
    virtual size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const = 0;
    virtual std::vector<NLMException> getRecentErrors(size_t count = 10) const = 0;
    virtual void clearErrors() = 0;
    virtual void exportErrors(const std::string& filepath, const std::string& format = "json") const = 0;
};

// Global error handler
class GlobalErrorHandler : public ErrorHandler {
public:
    static GlobalErrorHandler& getInstance() {
        static GlobalErrorHandler instance;
        return instance;
    }
    
    void initialize();
    void shutdown();
    
    bool handleError(const NLMException& error, const ErrorContext& context) override;
    void setErrorHandlingStrategy(ErrorHandlingStrategy strategy) override;
    ErrorHandlingStrategy getErrorHandlingStrategy() const override;
    
    void addErrorCallback(std::function<void(const NLMException&)> callback) override;
    void removeErrorCallback(std::function<void(const NLMException&)> callback) override;
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override;
    std::vector<NLMException> getRecentErrors(size_t count = 10) const override;
    void clearErrors() override;
    void exportErrors(const std::string& filepath, const std::string& format = "json") const override;
    
    void setMaxErrors(size_t maxErrors);
    void setErrorLogFile(const std::string& filepath);
    void setConsoleOutput(bool enabled);
    void setStackTraceEnabled(bool enabled);
    
    std::string generateErrorReport() const;
    std::string generateErrorSummary() const;
    
private:
    GlobalErrorHandler() : errorHandlingStrategy_(ErrorHandlingStrategy::Error),
                          maxErrors_(100), consoleOutput_(true), stackTrace_(true) {}
    
    bool processError(const NLMException& error, const ErrorContext& context);
    void logError(const NLMException& error, const ErrorContext& context);
    void callErrorCallbacks(const NLMException& error);
    
    std::vector<NLMException> errors_;
    std::vector<std::function<void(const NLMException&)>> errorCallbacks_;
    ErrorHandlingStrategy errorHandlingStrategy_;
    size_t maxErrors_;
    std::string errorLogFile_;
    bool consoleOutput_;
    bool stackTrace_;
    
    std::chrono::system_clock::time_point lastErrorTime_;
    size_t errorsInLastMinute_;
    double errorRate_;
};

// Error manager for component-specific error handling
class ErrorManager {
public:
    ErrorManager(const std::string& componentName);
    ~ErrorManager();
    
    void setComponentName(const std::string& name);
    std::string getComponentName() const;
    
    void setErrorHandler(ErrorHandler* handler);
    ErrorHandler* getErrorHandler() const;
    
    void handleError(const NLMException& error);
    void handleError(ErrorCode code, const std::string& message,
                    ErrorCategory category = ErrorCategory::System,
                    ErrorSeverity severity = ErrorSeverity::Error);
    
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message, ErrorCode code = ErrorCode::UnknownError,
                  ErrorCategory category = ErrorCategory::System);
    void logCritical(const std::string& message);
    void logFatal(const std::string& message);
    
    bool isErrorToleranceEnabled() const;
    void setErrorTolerance(bool enabled);
    
    std::string getErrorSummary() const;
    
private:
    std::string componentName_;
    ErrorHandler* errorHandler_;
    bool errorToleranceEnabled_;
    std::chrono::system_clock::time_point componentStartTime_;
    size_t componentErrorCount_;
};

// Macro for easy error handling
#define NLM_ERROR(category, code, severity, message) \
    nlm::NLMException(__FILE__, __LINE__, category, code, severity, message)

#define NLM_THROW(category, code, severity, message) \
    throw nlm::NLMException(__FILE__, __LINE__, category, code, severity, message)

#define NLM_THROW_WITH_CONTEXT(component, method, category, code, severity, message) \
    throw nlm::NLMException(__FILE__, __LINE__, component, method, category, code, severity, message)

#define NLM_LOG_ERROR(component, method, message, code) \
    do { \
        std::stringstream ss; \
        ss << "[" << component << "] " << method << ": " << message; \
        nlm::GlobalErrorHandler::getInstance().handleError( \
            nlm::NLMException(ss.str(), code, nlm::ErrorCategory::System, nlm::ErrorSeverity::Error), \
            nlm::ErrorContext(component, method, __FILE__, __LINE__) \
        ); \
    } while(0)

// Utility functions for error handling
namespace error_utils {
    std::string getErrorCategoryName(ErrorCategory category);
    std::string getErrorSeverityName(ErrorSeverity severity);
    std::string getErrorCodeName(ErrorCode code);
    
    std::string formatError(const NLMException& error);
    std::string formatErrorContext(const ErrorContext& context);
    
    bool isRecoverable(ErrorCode code);
    ErrorHandlingStrategy getDefaultErrorHandling(ErrorCode code);
    
    std::string generateErrorReport(const std::vector<NLMException>& errors);
    std::string generateErrorStatistics(const std::vector<NLMException>& errors);
    
    template<typename T>
    T safe_operation(const std::string& operation, std::function<T()> func,
                    ErrorCode errorCode = ErrorCode::UnknownError,
                    ErrorHandlingStrategy strategy = ErrorHandlingStrategy::Error);
    
    template<typename T>
    std::optional<T> safe_operation_optional(const std::string& operation, 
                                             std::function<T()> func,
                                             ErrorCode errorCode = ErrorCode::UnknownError,
                                             ErrorHandlingStrategy strategy = ErrorHandlingStrategy::Error);
    
    template<typename T>
    std::vector<T> safe_operation_vector(const std::string& operation,
                                         std::function<std::vector<T>()> func,
                                         ErrorCode errorCode = ErrorCode::UnknownError,
                                         ErrorHandlingStrategy strategy = ErrorHandlingStrategy::Error);
}

// Error handling for different components
class ConfigErrorHandler : public ErrorHandler {
public:
    bool handleError(const NLMException& error, const ErrorContext& context) override;
    void setErrorHandlingStrategy(ErrorHandlingStrategy strategy) override;
    ErrorHandlingStrategy getErrorHandlingStrategy() const override;
    
    void addErrorCallback(std::function<void(const NLMException&)> callback) override;
    void removeErrorCallback(std::function<void(const NLMException&)> callback) override;
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override;
    std::vector<NLMException> getRecentErrors(size_t count = 10) const override;
    void clearErrors() override;
    void exportErrors(const std::string& filepath, const std::string& format = "json") const override;
    
private:
    std::vector<NLMException> errors_;
    ErrorHandlingStrategy strategy_;
    std::vector<std::function<void(const NLMException&)>> callbacks_;
};

class BrainErrorHandler : public ErrorHandler {
public:
    bool handleError(const NLMException& error, const ErrorContext& context) override;
    void setErrorHandlingStrategy(ErrorHandlingStrategy strategy) override;
    ErrorHandlingStrategy getErrorHandlingStrategy() const override;
    
    void addErrorCallback(std::function<void(const NLMException&)> callback) override;
    void removeErrorCallback(std::function<void(const NLMException&)> callback) override;
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override;
    std::vector<NLMException> getRecentErrors(size_t count = 10) const override;
    void clearErrors() override;
    void exportErrors(const std::string& filepath, const std::string& format = "json") const override;
    
private:
    std::vector<NLMException> errors_;
    ErrorHandlingStrategy strategy_;
    std::vector<std::function<void(const NLMException&)>> callbacks_;
};

} // namespace nlm

// Convenience functions for error handling
inline void nlm_log_info(const std::string& message) {
    nlm::GlobalErrorHandler::getInstance().handleError(
        nlm::NLMException(message, nlm::ErrorCode::UnknownError, 
                          nlm::ErrorCategory::System, nlm::ErrorSeverity::Info),
        nlm::ErrorContext("global", "log_info", __FILE__, __LINE__)
    );
}

inline void nlm_log_warning(const std::string& message) {
    nlm::GlobalErrorHandler::getInstance().handleError(
        nlm::NLMException(message, nlm::ErrorCode::UnknownError,
                          nlm::ErrorCategory::System, nlm::ErrorSeverity::Warning),
        nlm::ErrorContext("global", "log_warning", __FILE__, __LINE__)
    );
}

inline void nlm_log_error(const std::string& message, nlm::ErrorCode code = nlm::ErrorCode::UnknownError) {
    nlm::GlobalErrorHandler::getInstance().handleError(
        nlm::NLMException(message, code, nlm::ErrorCategory::System, nlm::ErrorSeverity::Error),
        nlm::ErrorContext("global", "log_error", __FILE__, __LINE__)
    );
}

inline void nlm_log_critical(const std::string& message) {
    nlm::GlobalErrorHandler::getInstance().handleError(
        nlm::NLMException(message, nlm::ErrorCode::UnknownError,
                          nlm::ErrorCategory::System, nlm::ErrorSeverity::Critical),
        nlm::ErrorContext("global", "log_critical", __FILE__, __LINE__)
    );
}

inline void nlm_log_fatal(const std::string& message) {
    nlm::GlobalErrorHandler::getInstance().handleError(
        nlm::NLMException(message, nlm::ErrorCode::UnknownError,
                          nlm::ErrorCategory::System, nlm::ErrorSeverity::Fatal),
        nlm::ErrorContext("global", "log_fatal", __FILE__, __LINE__)
    );
}