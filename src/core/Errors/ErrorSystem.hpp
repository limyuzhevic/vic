// Error hierarchy and exception system for NLM
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <chrono>
#include <exception>
#include <stdexcept>

namespace nlm {

// Error severity levels (same as LogLevel for consistency)
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Base exception class for all NLM errors
class NLMException : public std::exception {
protected:
    std::string message_;
    std::string details_;
    ErrorSeverity severity_;
    std::string errorCode_;
    std::chrono::system_clock::time_point timestamp_;
    std::vector<std::string> recoverySteps_;
    std::vector<std::string> context_;
    
public:
    NLMException(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& errorCode = "",
                 const std::string& details = "")
        : message_(message), details_(details), severity_(severity), 
          errorCode_(errorCode), timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMException() noexcept = default;
    
    // Get the error message
    virtual const char* what() const noexcept override {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(severity_) << "] " << message_;
        if (!details_.empty()) {
            oss << " - " << details_;
        }
        return oss.str().c_str();
    }
    
    // Get formatted error details
    virtual std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "Error: " << message_ << std::endl;
        if (!details_.empty()) {
            oss << "Details: " << details_ << std::endl;
        }
        if (!errorCode_.empty()) {
            oss << "Error Code: " << errorCode_ << std::endl;
        }
        if (!recoverySteps_.empty()) {
            oss << "Recovery Steps:" << std::endl;
            for (const auto& step : recoverySteps_) {
                oss << "  - " << step << std::endl;
            }
        }
        return oss.str();
    }
    
    // Getters
    const std::string& getMessage() const { return message_; }
    const std::string& getDetails() const { return details_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getErrorCode() const { return errorCode_; }
    const std::vector<std::string>& getRecoverySteps() const { return recoverySteps_; }
    const std::vector<std::string>& getContext() const { return context_; }
    
    // Setters
    void addRecoveryStep(const std::string& step) { recoverySteps_.push_back(step); }
    void addContext(const std::string& ctx) { context_.push_back(ctx); }
    
    // Virtual destructor for inheritance
    virtual ~NLMException() = default;
};

// System-level errors (infrastructure issues)
class SystemError : public NLMException {
public:
    SystemError(const std::string& message, const std::string& errorCode = "SYS_001",
                const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Critical)
        : NLMException(message, severity, errorCode, details) {}
};

class MemoryError : public SystemError {
public:
    MemoryError(const std::string& message, const std::string& errorCode = "MEM_001",
                const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Critical)
        : SystemError(message, errorCode, details, severity) {
        addRecoveryStep("Check system memory availability");
        addRecoveryStep("Restart the simulation");
    }
};

class ResourceError : public SystemError {
public:
    ResourceError(const std::string& message, const std::string& errorCode = "RES_001",
                  const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Critical)
        : SystemError(message, errorCode, details, severity) {
        addRecoveryStep("Check system resources");
        addRecoveryStep("Close unnecessary applications");
    }
};

// Neural-specific errors
class NeuralError : public NLMException {
public:
    NeuralError(const std::string& message, const std::string& errorCode = "NRL_001",
                const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check neural network configuration");
        addRecoveryStep("Reset neural network");
    }
};

class NeuronError : public NeuralError {
public:
    NeuronError(const std::string& message, const std::string& errorCode = "NRN_001",
                const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NeuralError(message, errorCode, details, severity) {
        addRecoveryStep("Check neuron parameters");
        addRecoveryStep("Reset neuron state");
        addRecoveryStep("Adjust neuron dynamics");
    }
};

class SynapseError : public NeuralError {
public:
    SynapseError(const std::string& message, const std::string& errorCode = "SYN_001",
                 const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NeuralError(message, errorCode, details, severity) {
        addRecoveryStep("Check synapse parameters");
        addRecoveryStep("Reset synapse state");
        addRecoveryStep("Adjust synaptic dynamics");
    }
};

class PlasticityError : public NeuralError {
public:
    PlasticityError(const std::string& message, const std::string& errorCode = "PLT_001",
                   const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NeuralError(message, errorCode, details, severity) {
        addRecoveryStep("Check plasticity parameters");
        addRecoveryStep("Disable affected plasticity rule");
        addRecoveryStep("Continue with default plasticity");
    }
};

// Configuration errors
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message, const std::string& errorCode = "CFG_001",
                const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check configuration file syntax");
        addRecoveryStep("Use default configuration");
        addRecoveryStep("Fix configuration values");
    }
};

class ValidationError : public ConfigError {
public:
    ValidationError(const std::string& message, const std::string& errorCode = "VAL_001",
                    const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : ConfigError(message, errorCode, details, severity) {
        addRecoveryStep("Fix configuration validation");
        addRecoveryStep("Adjust schema validation rules");
    }
};

// Memory system errors
class MemorySystemError : public NLMException {
public:
    MemorySystemError(const std::string& message, const std::string& errorCode = "MEMSYS_001",
                      const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check memory system state");
        addRecoveryStep("Reset working memory");
        addRecoveryStep("Reinitialize episodic memory");
    }
};

class WorkingMemoryError : public MemorySystemError {
public:
    WorkingMemoryError(const std::string& message, const std::string& errorCode = "WM_001",
                      const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : MemorySystemError(message, errorCode, details, severity) {
        addRecoveryStep("Clear working memory");
        addRecoveryStep("Reduce working memory capacity");
    }
};

class EpisodicMemoryError : public MemorySystemError {
public:
    EpisodicMemoryError(const std::string& message, const std::string& errorCode = "EM_001",
                        const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : MemorySystemError(message, errorCode, details, severity) {
        addRecoveryStep("Disable episodic memory replay");
        addRecoveryStep("Clear old episodic memories");
    }
};

// Neuromodulation errors
class NeuromodulationError : public NLMException {
public:
    NeuromodulationError(const std::string& message, const std::string& errorCode = "NM_001",
                         const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check neuromodulator parameters");
        addRecoveryStep("Disable affected neuromodulator");
        addRecoveryStep("Continue without neuromodulation");
    }
};

class DopamineError : public NeuromodulationError {
public:
    DopamineError(const std::string& message, const std::string& errorCode = "DA_001",
                  const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Warning)
        : NeuromodulationError(message, errorCode, details, severity) {
        addRecoveryStep("Check reward prediction error calculation");
        addRecoveryStep("Disable reward modulation");
    }
};

// Development errors
class DevelopmentError : public NLMException {
public:
    DevelopmentError(const std::string& message, const std::string& errorCode = "DEV_001",
                     const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check developmental stage");
        addRecoveryStep("Use default development parameters");
        addRecoveryStep("Skip development phase");
    }
};

// Prediction system errors
class PredictionError : public NLMException {
public:
    PredictionError(const std::string& message, const std::string& errorCode = "PRD_001",
                    const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check prediction model");
        addRecoveryStep("Disable prediction system");
        addRecoveryStep("Continue without predictions");
    }
};

// Agent system errors
class AgentError : public NLMException {
public:
    AgentError(const std::string& message, const std::string& errorCode = "AGT_001",
               const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Error)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check agent configuration");
        addRecoveryStep("Reset agent state");
        addRecoveryStep("Use default agent parameters");
    }
};

// World interaction errors
class WorldError : public NLMException {
public:
    WorldError(const std::string& message, const std::string& errorCode = "WRL_001",
               const std::string& details = "", ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMException(message, severity, errorCode, details) {
        addRecoveryStep("Check world model");
        addRecoveryStep("Use default world parameters");
        addRecoveryStep("Continue without world interaction");
    }
};

// Exception utilities for error handling
class ErrorUtils {
public:
    // Convert exception to string representation
    static std::string toString(const std::exception& e) {
        return e.what();
    }
    
    // Check if exception can be recovered from
    static bool isRecoverable(const std::exception& e) {
        // Check severity level
        return dynamic_cast<const NLMException*>(&e) == nullptr || 
               dynamic_cast<const NLMException*>(&e)->getSeverity() < ErrorSeverity::Critical;
    }
    
    // Get error severity from exception
    static ErrorSeverity getSeverity(const std::exception& e) {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&e);
        if (nlmEx) {
            return nlmEx->getSeverity();
        }
        return ErrorSeverity::Error; // Default to Error for non-NLM exceptions
    }
    
    // Create a human-readable error summary
    static std::string getErrorSummary(const std::exception& e) {
        std::ostringstream oss;
        oss << "Exception: " << e.what() << std::endl;
        
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&e);
        if (nlmEx) {
            oss << "Severity: " << static_cast<int>(nlmEx->getSeverity()) << std::endl;
            if (!nlmEx->getErrorCode().empty()) {
                oss << "Error Code: " << nlmEx->getErrorCode() << std::endl;
            }
            if (!nlmEx->getContext().empty()) {
                oss << "Context:" << std::endl;
                for (const auto& ctx : nlmEx->getContext()) {
                    oss << "  - " << ctx << std::endl;
                }
            }
        }
        
        return oss.str();
    }
};

// Error reporting interface
class IErrorReporter {
public:
    virtual ~IErrorReporter() = default;
    virtual void report(const std::exception& error) = 0;
    virtual void report(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) = 0;
    virtual void reportError(const NLMException& error) = 0;
    virtual void setErrorThreshold(ErrorSeverity threshold) = 0;
    virtual ErrorSeverity getErrorThreshold() const = 0;
    virtual size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const = 0;
    virtual void resetErrorCounts() = 0;
};

// Console error reporter implementation
class ConsoleErrorReporter : public IErrorReporter {
private:
    ErrorSeverity threshold_;
    size_t errorCount_;
    size_t warningCount_;
    size_t criticalCount_;
    
public:
    ConsoleErrorReporter(ErrorSeverity threshold = ErrorSeverity::Warning)
        : threshold_(threshold), errorCount_(0), warningCount_(0), criticalCount_(0) {}
    
    void report(const std::exception& error) override {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        if (nlmEx) {
            reportError(*nlmEx);
        } else {
            // Handle non-NLM exceptions
            std::string msg = ErrorUtils::toString(error);
            report(msg, ErrorSeverity::Error);
        }
    }
    
    void report(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) override {
        if (severity < threshold_) {
            return; // Below threshold, don't report
        }
        
        // Print to console
        std::string severityStr;
        switch (severity) {
            case ErrorSeverity::Debug: severityStr = "DEBUG"; break;
            case ErrorSeverity::Info: severityStr = "INFO"; break;
            case ErrorSeverity::Warning: severityStr = "WARNING"; break;
            case ErrorSeverity::Error: severityStr = "ERROR"; break;
            case ErrorSeverity::Critical: severityStr = "CRITICAL"; break;
        }
        
        std::cout << "[" << severityStr << "] " << message << std::endl;
        
        // Update counters
        switch (severity) {
            case ErrorSeverity::Debug: break;
            case ErrorSeverity::Info: break;
            case ErrorSeverity::Warning: warningCount_++; break;
            case ErrorSeverity::Error: errorCount_++; break;
            case ErrorSeverity::Critical: criticalCount_++; break;
        }
    }
    
    void reportError(const NLMException& error) override {
        report(error.getFormattedMessage(), error.getSeverity());
    }
    
    void setErrorThreshold(ErrorSeverity threshold) override {
        threshold_ = threshold;
    }
    
    ErrorSeverity getErrorThreshold() const override {
        return threshold_;
    }
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override {
        switch (severity) {
            case ErrorSeverity::Debug: return 0;
            case ErrorSeverity::Info: return 0;
            case ErrorSeverity::Warning: return warningCount_;
            case ErrorSeverity::Error: return errorCount_;
            case ErrorSeverity::Critical: return criticalCount_;
            default: return 0;
        }
    }
    
    void resetErrorCounts() override {
        errorCount_ = 0;
        warningCount_ = 0;
        criticalCount_ = 0;
    }
    
    // Get error statistics
    std::string getErrorStats() const {
        std::ostringstream oss;
        oss << "Error Statistics:" << std::endl;
        oss << "  Warnings: " << warningCount_ << std::endl;
        oss << "  Errors: " << errorCount_ << std::endl;
        oss << "  Critical: " << criticalCount_ << std::endl;
        return oss.str();
    }
};

// Error recovery strategies
class IErrorRecoveryStrategy {
public:
    virtual ~IErrorRecoveryStrategy() = default;
    virtual bool canRecover(const std::exception& error) const = 0;
    virtual void recover(std::exception& error) = 0;
    virtual std::string getStrategyName() const = 0;
};

// Automatic error recovery strategy (default - attempt recovery, fall back to defaults)
class AutomaticErrorRecovery : public IErrorRecoveryStrategy {
public:
    bool canRecover(const std::exception& error) const override {
        return ErrorUtils::isRecoverable(error) && 
               dynamic_cast<const NLMException*>(&error) != nullptr;
    }
    
    void recover(std::exception& error) override {
        // Get recovery steps from NLMException
        NLMException* nlmEx = dynamic_cast<NLMException*>(&error);
        if (!nlmEx) {
            return; // Can't recover non-NLM exceptions
        }
        
        // For demonstration, just log the recovery attempt
        // In practice, this would implement the actual recovery logic
        for (const auto& step : nlmEx->getRecoverySteps()) {
            std::cout << "Attempting recovery: " << step << std::endl;
            // Here would be the actual recovery implementation
        }
    }
    
    std::string getStrategyName() const override {
        return "Automatic Error Recovery";
    }
};

// Error recovery manager
class ErrorRecoveryManager {
private:
    std::vector<std::shared_ptr<IErrorRecoveryStrategy>> strategies_;
    ErrorSeverity maxSeverityToRecover_;
    bool enableAutoRecovery_;
    
public:
    ErrorRecoveryManager() : maxSeverityToRecover_(ErrorSeverity::Error), enableAutoRecovery_(true) {
        strategies_.push_back(std::make_shared<AutomaticErrorRecovery>());
    }
    
    void addStrategy(std::shared_ptr<IErrorRecoveryStrategy> strategy) {
        strategies_.push_back(strategy);
    }
    
    bool attemptRecovery(std::exception& error) {
        if (!enableAutoRecovery_) {
            return false;
        }
        
        // Check if we should even attempt recovery for this error severity
        ErrorSeverity severity = ErrorUtils::getSeverity(error);
        if (severity >= maxSeverityToRecover_) {
            return false; // Too critical to recover from
        }
        
        // Try each strategy
        for (const auto& strategy : strategies_) {
            if (strategy->canRecover(error)) {
                strategy->recover(error);
                return true;
            }
        }
        
        return false;
    }
    
    void setMaxSeverityToRecover(ErrorSeverity severity) {
        maxSeverityToRecover_ = severity;
    }
    
    void setAutoRecoveryEnabled(bool enabled) {
        enableAutoRecovery_ = enabled;
    }
    
    std::vector<std::string> getAvailableStrategies() const {
        std::vector<std::string> names;
        for (const auto& strategy : strategies_) {
            names.push_back(strategy->getStrategyName());
        }
        return names;
    }
};

// Main error handler class (wrapper for Logger integration)
class ErrorHandler {
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<IErrorReporter> errorReporter_;
    std::shared_ptr<ErrorRecoveryManager> recoveryManager_;
    ErrorSeverity errorThreshold_;
    bool enableErrorHandling_;
    
public:
    ErrorHandler()
        : errorThreshold_(ErrorSeverity::Warning), enableErrorHandling_(true) {
        logger_ = &Logger::getGlobal();
        errorReporter_ = std::make_shared<ConsoleErrorReporter>(errorThreshold_);
        recoveryManager_ = std::make_shared<ErrorRecoveryManager>();
    }
    
    // Handle an exception
    void handle(const std::exception& error) {
        if (!enableErrorHandling_) {
            return;
        }
        
        // Log the error
        logError(error);
        
        // Report to error reporter
        errorReporter_->report(error);
        
        // Attempt recovery if possible
        if (attemptRecovery(error)) {
            logRecoverySuccess(error);
        }
    }
    
    // Handle an error with message only (no exception)
    void handle(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
               const std::string& details = "", const std::string& errorCode = "") {
        if (!enableErrorHandling_) {
            return;
        }
        
        // Log the error
        logError(message, severity, details, errorCode);
        
        // Report to error reporter
        errorReporter_->report(message, severity);
        
        // Create NLMException for recovery
        NLMException nlmError(message, severity, errorCode, details);
        attemptRecovery(nlmError);
    }
    
    // Helper to create NLMException and handle it
    template<typename TException, typename... Args>
    void handle(TException&& error) {
        handle(static_cast<const std::exception&>(error));
    }
    
    // Log error to logger
    void logError(const std::exception& error) {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        if (nlmEx) {
            logger_->error(nlmEx->getFormattedMessage());
        } else {
            logger_->error(ErrorUtils::toString(error));
        }
    }
    
    // Log error with message
    void logError(const std::string& message, ErrorSeverity severity,
                  const std::string& details = "", const std::string& errorCode = "") {
        std::ostringstream oss;
        oss << message;
        if (!details.empty()) {
            oss << " - " << details;
        }
        if (!errorCode.empty()) {
            oss << " [" << errorCode << "]";
        }
        
        switch (severity) {
            case ErrorSeverity::Debug: logger_->debug(oss.str()); break;
            case ErrorSeverity::Info: logger_->info(oss.str()); break;
            case ErrorSeverity::Warning: logger_->warning(oss.str()); break;
            case ErrorSeverity::Error: logger_->error(oss.str()); break;
            case ErrorSeverity::Critical: logger_->critical(oss.str()); break;
        }
    }
    
    // Attempt recovery
    bool attemptRecovery(const std::exception& error) {
        if (!enableErrorHandling_) {
            return false;
        }
        
        // Create a mutable copy for recovery
        std::exception* mutableError = const_cast<std::exception*>(&error);
        return recoveryManager_->attemptRecovery(*mutableError);
    }
    
    // Set error threshold
    void setErrorThreshold(ErrorSeverity threshold) {
        errorThreshold_ = threshold;
        errorReporter_->setErrorThreshold(threshold_);
        recoveryManager_->setMaxSeverityToRecover(threshold_);
    }
    
    // Enable/disable error handling
    void setErrorHandlingEnabled(bool enabled) {
        enableErrorHandling_ = enabled;
    }
    
    // Get error statistics
    std::string getErrorStats() const {
        if (errorReporter_) {
            return errorReporter_->getErrorStats();
        }
        return "Error reporting not available";
    }
    
    // Add recovery strategy
    void addRecoveryStrategy(std::shared_ptr<IErrorRecoveryStrategy> strategy) {
        recoveryManager_->addStrategy(strategy);
    }
};

// Global error handler instance
inline ErrorHandler& getErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

// Convenience functions for error handling
inline void logError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                    const std::string& details = "", const std::string& errorCode = "") {
    getErrorHandler().handle(message, severity, details, errorCode);
}

inline void handleError(const std::exception& error) {
    getErrorHandler().handle(error);
}

inline void handleError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                       const std::string& details = "", const std::string& errorCode = "") {
    getErrorHandler().handle(message, severity, details, errorCode);
}

// Exception throwing macros
#define NLM_THROW_ERROR(msg, ...) \
    nlm::handleError(msg, nlm::ErrorSeverity::Error, ##__VA_ARGS__)

#define NLM_THROW_WARNING(msg, ...) \
    nlm::handleError(msg, nlm::ErrorSeverity::Warning, ##__VA_ARGS__)

#define NLM_THROW_CRITICAL(msg, ...) \
    nlm::handleError(msg, nlm::ErrorSeverity::Critical, ##__VA_ARGS__)

// Exception checking macros
#define NLM_CHECK(condition, errorMsg, ...) \
    do { \
        if (!(condition)) { \
            nlm::handleError(errorMsg, nlm::ErrorSeverity::Error, ##__VA_ARGS__); \
            return false; \
        } \
    } while(0)

#define NLM_CHECK_PTR(ptr, errorMsg, ...) \
    do { \
        if ((ptr) == nullptr) { \
            nlm::handleError(errorMsg, nlm::ErrorSeverity::Error, ##__VA_ARGS__); \
            return nullptr; \
        } \
    } while(0)

// Safe operation macros with recovery
#define NLM_SAFE_OP(operation, errorMsg) \
    [&]() -> auto { \
        try { \
            auto result = (operation); \
            return result; \
        } catch (const std::exception& e) { \
            nlm::handleError(errorMsg + std::string(" - Caught exception: ") + e.what()); \
            return nullptr; \
        } \
    }()

} // namespace nlm
