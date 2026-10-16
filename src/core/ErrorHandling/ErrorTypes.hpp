// **Error Types and Exception Hierarchy**
//
// Comprehensive error handling and recovery system for the NLM framework.
// Provides structured exception hierarchy, error recovery mechanisms, and
// integration with existing APIs while maintaining backward compatibility.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include <functional>
#include <optional>
#include <typeinfo>
#include <stdexcept>
#include <sstream>

namespace nlm {

// **Error Reporting Levels**
enum class ErrorReportLevel {
    Silent = 0,    // No error reporting
    Warning = 1,    // Report warnings only
    Error = 2,      // Report errors and warnings
    Critical = 3    // Report all errors, warnings, and critical issues
};

// **Exception Handling Strategies**
enum class ExceptionStrategy {
    Throw = 0,      // Throw exceptions for errors
    Catch = 1,      // Catch and handle internally
    Ignore = 2      // Ignore errors, continue operation
};

// **Recovery Modes**
enum class RecoveryMode {
    Retry = 0,      // Retry operation
    Graceful = 1,   // Continue with reduced functionality
    Abort = 2       // Stop operation
};

// **Error Categories**
enum class ErrorCode {
    // Configuration Errors
    ConfigFileNotFound = 1000,
    MalformedConfig = 1001,
    ConfigValidationError = 1002,
    ConfigParseError = 1003,
    
    // Brain Errors
    BrainInitFailure = 2000,
    SimulationRuntimeError = 2001,
    MemoryAllocationFailure = 2002,
    SimulationStepError = 2003,
    
    // Neuron Errors
    NeuronThresholdError = 3000,
    RefractoryPeriodError = 3001,
    SynapticInputError = 3002,
    NeuronStateError = 3003,
    
    // Synapse Errors
    SynapseConnectionError = 4000,
    WeightCalculationError = 4001,
    TimingError = 4002,
    SynapseStateError = 4003,
    
    // Simulation Errors
    TimeStepError = 5000,
    StabilityError = 5001,
    ConvergenceFailure = 5002,
    SimulationResourceError = 5003,
    
    // System Errors
    ResourceAllocationError = 6000,
    MemoryError = 6001,
    ThreadingError = 6002,
    SystemError =6003
};

// **Base Exception Class**
class NLMException : public std::exception {
protected:
    ErrorCode errorCode;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> contextStack;
    
public:
    NLMException(ErrorCode code, const std::string& msg,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : errorCode(code), message(msg), file(file), line(line),
          function(function), timestamp(std::chrono::system_clock::now()) {
        // Add to context stack if we have file/function info
        if (!file.empty()) {
            contextStack.push_back("File: " + file);
        }
        if (!function.empty()) {
            contextStack.push_back("Function: " + function);
        }
        if (line > 0) {
            contextStack.push_back("Line: " + std::to_string(line));
        }
        contextStack.push_back("Timestamp: " +
            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                timestamp.time_since_epoch()).count()));
    }
    
    virtual ~NLMException() = default;
    
    ErrorCode getErrorCode() const { return errorCode; }
    const char* what() const noexcept override { return message.c_str(); }
    const std::string& getMessage() const { return message; }
    
    // Add context information
    void addContext(const std::string& context) {
        contextStack.push_back(context);
    }
    
    // Get all context information
    std::string getContext() const {
        std::ostringstream oss;
        for (size_t i = 0; i < contextStack.size(); ++i) {
            if (i > 0) oss << "; ";
            oss << contextStack[i];
        }
        return oss.str();
    }
    
    // Convert exception to string representation
    virtual std::string toString() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(errorCode) << "] " << message;
        if (!contextStack.empty()) {
            oss << " (Context: " << getContext() << ")";
        }
        return oss.str();
    }
};

// **Specific Exception Types**

class ConfigException : public NLMException {
public:
    ConfigException(ErrorCode code, const std::string& msg,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "")
        : NLMException(code, msg, file, line, function) {}
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << "[Config Error] " << static_cast<int>(getErrorCode()) << ": " << message;
        if (!contextStack.empty()) {
            oss << " (Context: " << getContext() << ")";
        }
        return oss.str();
    }
};

class BrainException : public NLMException {
public:
    BrainException(ErrorCode code, const std::string& msg,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "")
        : NLMException(code, msg, file, line, function) {}
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << "[Brain Error] " << static_cast<int>(getErrorCode()) << ": " << message;
        if (!contextStack.empty()) {
            oss << " (Context: " << getContext() << ")";
        }
        return oss.str();
    }
};

class NeuronException : public NLMException {
public:
    NeuronException(ErrorCode code, const std::string& msg,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "")
        : NLMException(code, msg, file, line, function) {}
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << "[Neuron Error] " << static_cast<int>(getErrorCode()) << ": " << message;
        if (!contextStack.empty()) {
            oss << " (Context: " << getContext() << ")";
        }
        return oss.str();
    }
};

class SynapseException : public NLMException {
public:
    SynapseException(ErrorCode code, const std::string& msg,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "")
        : NLMException(code, msg, file, line, function) {}
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << "[Synapse Error] " << static_cast<int>(getErrorCode()) << ": " << message;
        if (!contextStack.empty()) {
            oss << " (Context: " << getContext() << ")";
        }
        return oss.str();
    }
};

class SimulationException : public NLMException {
public:
    SimulationException(ErrorCode code, const std::string& msg,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "")
        : NLMException(code, msg, file, line, function) {}
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << "[Simulation Error] " << static_cast<int>(getErrorCode()) << ": " << message;
        if (!contextStack.empty()) {
            oss << " (Context: " << getContext() << ")";
        }
        return oss.str();
    }
};

// **Standard Exception Classes**

// Config Error Types
class ConfigFileNotFoundException : public ConfigException {
public:
    ConfigFileNotFoundException(const std::string& filepath,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "")
        : ConfigException(ErrorCode::ConfigFileNotFound,
                         "Configuration file not found: " + filepath,
                         file, line, function) {}
};

class MalformedConfigException : public ConfigException {
public:
    MalformedConfigException(const std::string& msg,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : ConfigException(ErrorCode::MalformedConfig, msg, file, line, function) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& msg,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "")
        : ConfigException(ErrorCode::ConfigValidationError, msg, file, line, function) {}
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& msg,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "")
        : ConfigException(ErrorCode::ConfigParseError, msg, file, line, function) {}
};

// Brain Error Types
class BrainInitException : public BrainException {
public:
    BrainInitException(const std::string& msg,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : BrainException(ErrorCode::BrainInitFailure, msg, file, line, function) {}
};

class SimulationRuntimeException : public BrainException {
public:
    SimulationRuntimeException(const std::string& msg,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "")
        : BrainException(ErrorCode::SimulationRuntimeError, msg, file, line, function) {}
};

class MemoryAllocationException : public BrainException {
public:
    MemoryAllocationException(const std::string& msg,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "")
        : BrainException(ErrorCode::MemoryAllocationFailure, msg, file, line, function) {}
};

// Neuron Error Types
class NeuronThresholdException : public NeuronException {
public:
    NeuronThresholdException(const std::string& msg,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "")
        : NeuronException(ErrorCode::NeuronThresholdError, msg, file, line, function) {}
};

class RefractoryPeriodException : public NeuronException {
public:
    RefractoryPeriodException(const std::string& msg,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "")
        : NeuronException(ErrorCode::RefractoryPeriodError, msg, file, line, function) {}
};

class SynapticInputException : public NeuronException {
public:
    SynapticInputException(const std::string& msg,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "")
        : NeuronException(ErrorCode::SynapticInputError, msg, file, line, function) {}
};

// Synapse Error Types
class SynapseConnectionException : public SynapseException {
public:
    SynapseConnectionException(const std::string& msg,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "")
        : SynapseException(ErrorCode::SynapseConnectionError, msg, file, line, function) {}
};

class WeightCalculationException : public SynapseException {
public:
    WeightCalculationException(const std::string& msg,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "")
        : SynapseException(ErrorCode::WeightCalculationError, msg, file, line, function) {}
};

// Simulation Error Types
class TimeStepException : public SimulationException {
public:
    TimeStepException(const std::string& msg,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : SimulationException(ErrorCode::TimeStepError, msg, file, line, function) {}
};

class StabilityException : public SimulationException {
public:
    StabilityException(const std::string& msg,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "")
        : SimulationException(ErrorCode::StabilityError, msg, file, line, function) {}
};

// System Error Types
class ResourceAllocationException : public NLMException {
public:
    ResourceAllocationException(const std::string& msg,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "")
        : NLMException(ErrorCode::ResourceAllocationError, msg, file, line, function) {}
};

// **Error Reporting Infrastructure**
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void report(const std::string& message, ErrorReportLevel level = ErrorReportLevel::Error,
                       const std::string& context = "") = 0;
    virtual void reportException(const NLMException& exception) = 0;
    virtual bool isEnabled(ErrorReportLevel level) const = 0;
};

class ConsoleErrorReporter : public ErrorReporter {
private:
    ErrorReportLevel minLevel;
    bool useColors;
    
public:
    ConsoleErrorReporter(ErrorReportLevel level = ErrorReportLevel::Error,
                        bool colors = true) : minLevel(level), useColors(colors) {}
    
    void report(const std::string& message, ErrorReportLevel level = ErrorReportLevel::Error,
               const std::string& context = "") override {
        if (level < minLevel) return;
        
        std::string levelStr;
        switch (level) {
            case ErrorReportLevel::Silent: levelStr = "SILENT"; break;
            case ErrorReportLevel::Warning: levelStr = "WARNING"; break;
            case ErrorReportLevel::Error: levelStr = "ERROR"; break;
            case ErrorReportLevel::Critical: levelStr = "CRITICAL"; break;
        }
        
        std::ostringstream oss;
        oss << "[" << levelStr << "]";
        if (!context.empty()) {
            oss << " [" << context << "]";
        }
        oss << " " << message;
        
        // TODO: Implement colored output when useColors is true
        std::cout << oss.str() << std::endl;
    }
    
    void reportException(const NLMException& exception) override {
        report(exception.toString(), ErrorReportLevel::Error);
    }
    
    bool isEnabled(ErrorReportLevel level) const override {
        return level >= minLevel;
    }
};

class ErrorHandler {
private:
    static std::unique_ptr<ErrorHandler> instance;
    static std::mutex instanceMutex;
    
    std::vector<std::shared_ptr<ErrorReporter>> reporters;
    ErrorReportLevel globalLevel;
    ExceptionStrategy strategy;
    std::mutex reportMutex;
    
    // Recovery state
    std::map<std::string, std::function<bool()>> recoveryActions;
    std::map<std::string, std::function<void()>> cleanupActions;
    
    // Statistics
    struct ErrorStats {
        int totalErrors = 0;
        int errorsHandled = 0;
        int errorsRecovered = 0;
        std::chrono::system_clock::time_point lastErrorTime;
        std::string lastErrorMessage;
    } stats;
    
protected:
    ErrorHandler()
        : globalLevel(ErrorReportLevel::Error),
          strategy(ExceptionStrategy::Throw),
          stats() {
        // Add default console reporter
        reporters.push_back(std::make_shared<ConsoleErrorReporter>(globalLevel));
    }
    
public:
    static ErrorHandler& getInstance() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (!instance) {
            instance = std::unique_ptr<ErrorHandler>(new ErrorHandler());
        }
        return *instance;
    }
    
    static void initialize(ErrorReportLevel level = ErrorReportLevel::Error,
                          ExceptionStrategy strategy = ExceptionStrategy::Throw) {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (!instance) {
            instance = std::unique_ptr<ErrorHandler>(new ErrorHandler());
            instance->globalLevel = level;
            instance->strategy = strategy;
            
            // Update all reporters
            for (auto& reporter : instance->reporters) {
                if (auto consoleReporter = std::dynamic_pointer_cast<ConsoleErrorReporter>(reporter)) {
                    consoleReporter->minLevel = level;
                }
            }
        }
    }
    
    static void cleanup() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        instance.reset();
    }
    
    // Error reporting
    void report(const std::string& message, ErrorReportLevel level = ErrorReportLevel::Error,
               const std::string& context = "", bool throwException = false) {
        std::lock_guard<std::mutex> lock(reportMutex);
        
        stats.totalErrors++;
        stats.lastErrorTime = std::chrono::system_clock::now();
        stats.lastErrorMessage = message;
        
        std::string fullMessage = message;
        if (!context.empty()) {
            fullMessage += " [Context: " + context + "]";
        }
        
        // Report to all enabled reporters
        for (const auto& reporter : reporters) {
            if (reporter->isEnabled(level)) {
                reporter->report(fullMessage, level, context);
            }
        }
        
        if (throwException) {
            // Create a generic exception based on message content
            ErrorCode errorCode = ErrorCode::SystemError;
            if (message.find("Config") != std::string::npos) {
                errorCode = ErrorCode::ConfigValidationError;
            } else if (message.find("Brain") != std::string::npos) {
                errorCode = ErrorCode::BrainInitFailure;
            } else if (message.find("Neuron") != std::string::npos) {
                errorCode = ErrorCode::NeuronStateError;
            } else if (message.find("Synapse") != std::string::npos) {
                errorCode = ErrorCode::SynapseConnectionError;
            } else if (message.find("Simulation") != std::string::npos) {
                errorCode = ErrorCode::SimulationRuntimeError;
            }
            
            throw NLMException(errorCode, fullMessage);
        }
    }
    
    void reportException(const NLMException& exception) {
        std::lock_guard<std::mutex> lock(reportMutex);
        
        stats.totalErrors++;
        stats.lastErrorTime = std::chrono::system_clock::now();
        stats.lastErrorMessage = exception.getMessage();
        
        // Report to all reporters
        for (const auto& reporter : reporters) {
            reporter->reportException(exception);
        }
    }
    
    // Exception handling
    template<typename TException, typename... Args>
    void handleError(const std::string& operation, const std::function<TException(Args...)>>& createException,
                    Args&&... args, RecoveryMode mode = RecoveryMode::Abort) {
        try {
            if (strategy == ExceptionStrategy::Throw) {
                throw createException(std::forward<Args>(args)...);
            } else if (strategy == ExceptionStrategy::Catch) {
                try {
                    throw createException(std::forward<Args>(args)...);
                } catch (const TException& e) {
                    reportException(e);
                    
                    if (mode == RecoveryMode::Retry) {
                        // TODO: Implement retry logic
                    } else if (mode == RecoveryMode::Graceful) {
                        // TODO: Implement graceful degradation
                    }
                    
                    stats.errorsHandled++;
                }
            }
            // Ignore strategy - just report
            else {
                report("Operation failed: " + operation, ErrorReportLevel::Warning);
                stats.errorsHandled++;
            }
        } catch (...) {
            // Unexpected exception
            std::exception_ptr ep = std::current_exception();
            try {
                std::rethrow_exception(ep);
            } catch (const NLMException& e) {
                reportException(e);
            } catch (const std::exception& e) {
                report(std::string("Unexpected error: ") + e.what(), ErrorReportLevel::Critical);
            }
            throw;
        }
    }
    
    // Recovery mechanism registration
    void registerRecoveryAction(const std::string& errorCode, const std::function<bool()>>& action) {
        recoveryActions[errorCode] = action;
    }
    
    void registerCleanupAction(const std::string& operation, const std::function<void()>>& action) {
        cleanupActions[operation] = action;
    }
    
    // Execute recovery action
    bool executeRecovery(const std::string& errorCode) {
        auto it = recoveryActions.find(errorCode);
        if (it != recoveryActions.end()) {
            bool success = it->second();
            if (success) {
                stats.errorsRecovered++;
                return true;
            }
        }
        return false;
    }
    
    // Execute cleanup action
    void executeCleanup(const std::string& operation) {
        auto it = cleanupActions.find(operation);
        if (it != cleanupActions.end()) {
            it->second();
        }
    }
    
    // Configuration
    void setGlobalLevel(ErrorReportLevel level) {
        std::lock_guard<std::mutex> lock(reportMutex);
        globalLevel = level;
        
        // Update all reporters
        for (auto& reporter : reporters) {
            if (auto consoleReporter = std::dynamic_pointer_cast<ConsoleErrorReporter>(reporter)) {
                consoleReporter->minLevel = level;
            }
        }
    }
    
    ErrorReportLevel getGlobalLevel() const { return globalLevel; }
    
    void setStrategy(ExceptionStrategy strategy) { this->strategy = strategy; }
    ExceptionStrategy getStrategy() const { return strategy; }
    
    void addReporter(std::shared_ptr<ErrorReporter> reporter) {
        reporters.push_back(reporter);
    }
    
    const std::vector<std::shared_ptr<ErrorReporter>>& getReporters() const { return reporters; }
    
    // Statistics
    const ErrorStats& getStatistics() const { return stats; }
    
    void resetStatistics() { stats = ErrorStats(); }
};

// **Recovery Utilities**
namespace Recovery {
    
    // Checkpoint recovery utilities
    class CheckpointManager {
    private:
        std::string checkpointDirectory;
        std::chrono::milliseconds checkpointInterval;
        
    public:
        CheckpointManager(const std::string& dir = "checkpoints",
                         std::chrono::milliseconds interval = std::chrono::milliseconds(5000))
            : checkpointDirectory(dir), checkpointInterval(interval) {}
        
        virtual ~CheckpointManager() = default;
        
        // Save state to checkpoint
        virtual bool save(const std::string& identifier, const std::string& data) {
            // TODO: Implement actual checkpoint saving
            std::cout << "Checkpoint saved: " << identifier << std::endl;
            return true;
        }
        
        // Load state from checkpoint
        virtual std::optional<std::string> load(const std::string& identifier) {
            // TODO: Implement actual checkpoint loading
            std::cout << "Checkpoint loaded: " << identifier << std::endl;
            return std::nullopt;
        }
        
        // Cleanup old checkpoints
        virtual void cleanup() {
            // TODO: Implement cleanup logic
        }
    };
    
    // Retry mechanism
    class RetryManager {
    public:
        struct RetryConfig {
            int maxAttempts = 3;
            std::chrono::milliseconds initialDelay = std::chrono::milliseconds(100);
            double backoffMultiplier = 2.0;
            std::vector<std::chrono::milliseconds> customDelays;
        };
        
        template<typename Func, typename... Args>
        static typename std::result_of<Func(Args...)>::type
        retry(const std::string& operation, const Func& func, Args&&... args,
              const RetryConfig& config = RetryConfig()) {
            int attempt = 0;
            std::chrono::milliseconds delay = config.initialDelay;
            
            while (attempt < config.maxAttempts) {
                try {
                    return func(std::forward<Args>(args)...);
                } catch (...) {
                    attempt++;
                    if (attempt >= config.maxAttempts) {
                        throw;
                    }
                    
                    // Exponential backoff
                    if (config.customDelays.empty()) {
                        std::this_thread::sleep_for(delay);
                        delay = std::chrono::milliseconds(static_cast<long>(
                            delay.count() * config.backoffMultiplier));
                    } else {
                        if (attempt - 1 < config.customDelays.size()) {
                            std::this_thread::sleep_for(config.customDelays[attempt - 1]);
                        } else {
                            std::this_thread::sleep_for(delay);
                            delay = std::chrono::milliseconds(static_cast<long>(
                                delay.count() * config.backoffMultiplier));
                        }
                    }
                }
            }
            
            throw std::runtime_error("Retry failed after " + std::to_string(config.maxAttempts) + 
                                   " attempts for operation: " + operation);
        }
    };
    
    // Graceful degradation utilities
    class GracefulDegradation {
    public:
        template<typename T>
        class DegradedValue {
        private:
            T value;
            bool isDegraded = false;
            std::string degradationReason;
            
        public:
            DegradedValue() : value(), isDegraded(true), degradationReason("Not initialized") {}
            explicit DegradedValue(const T& val) : value(val), isDegraded(false) {}
            
            DegradedValue(const T& val, const std::string& reason)
                : value(val), isDegraded(true), degradationReason(reason) {}
            
            operator T() const { return value; }
            T get() const { return value; }
            bool isDegradedValue() const { return isDegraded; }
            const std::string& getDegradationReason() const { return degradationReason; }
        };
        
        template<typename Func>
        static DegradedValue<typename std::result_of<Func()>::type> wrap(
            const std::string& operation, const Func& func,
            const std::string& fallbackReason = "Operation unavailable") {
            try {
                auto result = func();
                return DegradedValue<decltype(result)>(result, "");
            } catch (...) {
                return DegradedValue<decltype(result)>(DegradedValue<decltype(result)>::value_type(), fallbackReason);
            }
        }
    };
};

// **Error Handling Macros**
#define NLM_THROW_ERROR(code, message) \
    do { \
        nlm::ErrorHandler::getInstance().report(message, nlm::ErrorReportLevel::Error, "", true); \
    } while(0)

#define NLM_THROW_EXCEPTION(exception_class, message) \
    do { \
        throw exception_class(ErrorCode::##code, message, __FILE__, __LINE__, __func__); \
    } while(0)

#define NLM_TRY_RECOVER(operation, func, ...) \
    do { \
        try { \
            return func(__VA_ARGS__); \
        } catch (const nlm::NLMException& e) { \
            if (!nlm::ErrorHandler::getInstance().executeRecovery(#operation)) { \
                throw; \
            } \
        } \
    } while(0)

#define NLM_CLEANUP(operation) \
    do { \
        auto cleanupAction = [operation]() { \
            nlm::ErrorHandler::getInstance().executeCleanup(operation); \
        }; \
        cleanupAction(); \
    } while(0)

// **Integration Example Macros**
#define NLM_CONFIG_ERROR_HANDLER(operation) \
    nlm::ErrorHandler::getInstance().handleError<nlm::ConfigException>( \
        operation, []() -> nlm::ConfigException { \
            return nlm::ConfigException(nlm::ErrorCode::ConfigValidationError, "Config error in " + operation); \
        }, \
        nlm::RecoveryMode::Graceful)

#define NLM_BRAIN_ERROR_HANDLER(operation) \
    nlm::ErrorHandler::getInstance().handleError<nlm::BrainException>( \
        operation, []() -> nlm::BrainException { \
            return nlm::BrainException(nlm::ErrorCode::BrainInitFailure, "Brain error in " + operation); \
        }, \
        nlm::RecoveryMode::Graceful)

#define NLM_NEURON_ERROR_HANDLER(operation) \
    nlm::ErrorHandler::getInstance().handleError<nlm::NeuronException>( \
        operation, []() -> nlm::NeuronException { \
            return nlm::NeuronException(nlm::ErrorCode::NeuronStateError, "Neuron error in " + operation); \
        }, \
        nlm::RecoveryMode::Graceful)

// **Convenience Functions**
inline void configureErrorHandling(ErrorReportLevel level, ExceptionStrategy strategy) {
    nlm::ErrorHandler::initialize(level, strategy);
}

inline void addErrorReporter(std::shared_ptr<nlm::ErrorReporter> reporter) {
    nlm::ErrorHandler::getInstance().addReporter(reporter);
}

inline void logError(const std::string& message, const std::string& context = "") {
    nlm::ErrorHandler::getInstance().report(message, nlm::ErrorReportLevel::Error, context);
}

inline void logWarning(const std::string& message, const std::string& context = "") {
    nlm::ErrorHandler::getInstance().report(message, nlm::ErrorReportLevel::Warning, context);
}

inline void logCritical(const std::string& message, const std::string& context = "") {
    nlm::ErrorHandler::getInstance().report(message, nlm::ErrorReportLevel::Critical, context);
}

} // namespace nlm
