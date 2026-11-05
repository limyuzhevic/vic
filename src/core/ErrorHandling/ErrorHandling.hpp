#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <stdexcept>
#include <map>

namespace nlm {

// Advanced error handling framework

class NLMError : public std::runtime_error {
public:
    enum class ErrorCategory {
        CONFIGURATION,
        MEMORY,
        NEURAL,
        SIMULATION,
        IO,
        VALIDATION,
        NETWORK,
        PLUGIN,
        SYSTEM
    };

    NLMError(ErrorCategory category, const std::string& message,
             const std::string& context = "",
             const std::string& suggestion = "",
             int errorCode = 0);

    virtual ~NLMError() = default;

    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    const std::string& getSuggestion() const { return suggestion_; }
    int getErrorCode() const { return errorCode_; }
    
    std::string getFullMessage() const;
    virtual std::string getErrorTypeName() const;

protected:
    ErrorCategory category_;
    std::string context_;
    std::string suggestion_;
    int errorCode_;
};

class ConfigError : public NLMError {
public:
    enum class ConfigErrorType {
        INVALID_FORMAT,
        MISSING_REQUIRED_FIELD,
        DUPLICATE_KEY,
        OUT_OF_RANGE,
        TYPE_MISMATCH,
        FILE_NOT_FOUND,
        PARSE_ERROR,
        SCHEMA_VIOLATION
    };

    ConfigError(ConfigErrorType type, const std::string& key,
                const std::string& message,
                const std::string& context = "",
                const std::string& suggestion = "");

    ConfigErrorType getErrorType() const { return errorType_; }
    const std::string& getKey() const { return key_; }
    std::string getErrorTypeName() const override;

private:
    ConfigErrorType errorType_;
    std::string key_;
};

class MemoryError : public NLMError {
public:
    enum class MemoryErrorType {
        ALLOCATION_FAILED,
        NULL_POINTER_DEREFERENCE,
        BUFFER_OVERFLOW,
        MEMORY_LEAK,
        DOUBLE_FREE,
        OUT_OF_MEMORY,
        INVALID_ALIGNMENT,
        USE_AFTER_FREE
    };

    MemoryError(MemoryErrorType type, const std::string& context = "",
                const std::string& suggestion = "");

    MemoryErrorType getErrorType() const { return errorType_; }
    std::string getErrorTypeName() const override;

private:
    MemoryErrorType errorType_;
};

class NeuralError : public NLMError {
public:
    enum class NeuralErrorType {
        INCOMPATIBLE_CONNECTION,
        INVALID_NEURON_TYPE,
        SYNAPSE_ERROR,
        INTEGRATION_ERROR,
        COHERENCE_VIOLATION,
        UNSTABLE_NETWORK,
        INSUFFICIENT_ACTIVITY
    };

    NeuralError(NeuralErrorType type, const std::string& context = "",
                const std::string& suggestion = "");

    NeuralErrorType getErrorType() const { return errorType_; }
    std::string getErrorTypeName() const override;

private:
    NeuralErrorType errorType_;
};

class ErrorContext {
public:
    ErrorContext(const std::string& operation,
                 const std::string& file = "",
                 int line = 0,
                 const std::string& function = "",
                 const std::unordered_map<std::string, std::string>& contextData = {});

    std::string getOperation() const { return operation_; }
    std::string getFile() const { return file_; }
    int getLine() const { return line_; }
    std::string getFunction() const { return function_; }
    const std::unordered_map<std::string, std::string>& getContextData() const { return contextData_; }
    
    std::string toString() const;

private:
    std::string operation_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> contextData_;
};

class ErrorHandler {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL,
        FATAL
    };

    struct ErrorHandlerConfig {
        LogLevel logLevel;
        bool crashOnCritical;
        bool crashOnFatal;
        std::string logFile;
        bool writeToConsole;
        size_t maxErrorsLogged;
        std::function<void(const NLMError&)> customErrorHandler;
    };

    ErrorHandler();
    ~ErrorHandler() = default;
    
    void setConfig(const ErrorHandlerConfig& config);
    const ErrorHandlerConfig& getConfig() const { return config_; }
    
    void logError(const NLMError& error, const ErrorContext& context = ErrorContext(""));
    void logWarning(const std::string& message, const ErrorContext& context = ErrorContext(""));
    void logInfo(const std::string& message, const ErrorContext& context = ErrorContext(""));
    void logDebug(const std::string& message, const ErrorContext& context = ErrorContext(""));
    
    std::vector<NLMError> getRecentErrors(size_t count = 100) const;
    void clearRecentErrors();
    
    void registerErrorHandler(std::function<bool(const NLMError&)> handler);
    bool handleError(const NLMError& error, const ErrorContext& context = ErrorContext(""));
    
    static ErrorHandler& getInstance();
    static void resetInstance();
    
    // Exception chaining and reporting
    template<typename TException>
    void wrapException(const TException& original, const std::string& operation);
    
    std::string formatError(const NLMError& error, const ErrorContext& context) const;
    std::string formatStackTrace() const;

private:
    ErrorHandlerConfig config_;
    std::vector<NLMError> recentErrors_;
    std::vector<std::function<bool(const NLMError&)>> errorHandlers_;
    mutable std::mutex mutex_;
    
    void logToFile(const std::string& message, LogLevel level);
    void logToConsole(const std::string& message, LogLevel level);
    std::string getCurrentTimestamp() const;
    std::string getLogLevelName(LogLevel level) const;
};

// Global error reporting macros
#define NLM_ERROR_REPORT(operation, error) \
    do { \
        nlm::ErrorHandler::getInstance().logError(error, nlm::ErrorContext(operation, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)); \
    } while(0)

#define NLM_WARNING_REPORT(operation, message) \
    do { \
        nlm::ErrorHandler::getInstance().logWarning(message, nlm::ErrorContext(operation, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)); \
    } while(0)

#define NLM_EXCEPTION_WRAP(original_ex, operation) \
    nlm::ErrorHandler::getInstance().wrapException(original_ex, operation)

// Error reporting utilities
namespace ErrorReporting {
    std::string getStackTrace();
    void setProgramInfo(const std::string& name, const std::string& version);
    std::string getProgramInfo();
    
    template<typename T>
    T withErrorHandling(const std::string& operation, std::function<T()> func,
                       const std::string& context = "");
}

// Memory error detection
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    void* allocate(size_t size, const std::string& sourceFile, int sourceLine);
    void deallocate(void* ptr, const std::string& sourceFile, int sourceLine);
    
    struct MemoryInfo {
        void* address;
        size_t size;
        std::string sourceFile;
        int sourceLine;
        std::chrono::system_clock::time_point allocationTime;
        bool isFreed;
    };
    
    std::vector<MemoryInfo> getActiveAllocations() const;
    size_t getTotalAllocated() const;
    bool hasMemoryLeaks() const;
    
    MemoryError::MemoryErrorType checkForLeaks() const;
    
    static MemoryTracker& getInstance();
    static void resetInstance();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex mutex_;
};

// Safe memory allocation macros
#define NLM_SAFE_NEW(type) new(std::nothrow) type
#define NLM_SAFE_NEW_ARRAY(type, count) new(std::nothrow) type[count]
#define NLM_SAFE_DELETE(ptr) if (ptr) { delete ptr; ptr = nullptr; }
#define NLM_SAFE_DELETE_ARRAY(ptr) if (ptr) { delete[] ptr; ptr = nullptr; }

// Error handling for neural operations
class NeuralOperationError {
public:
    enum class Operation {
        BRAIN_INITIALIZATION,
        NEURON_ACTIVATION,
        SYNAPSE_FORMATION,
        PLASTICITY_UPDATE,
        REWARD_PROCESSING,
        CONCEPT_FORMATION,
        COGNITIVE_PLANNING,
        MOTOR_OUTPUT
    };

    static void checkNeuronConnectivity(int fromRegion, int toRegion,
                                       double connectionProbability);
    static void validateNeuronState(const std::string& state);
    static void checkPlasticityBounds(float learningRate, float weightChange);
    
    static void reportOperationError(Operation op, const std::string& details,
                                    const ErrorContext& context = ErrorContext(""));
};

// Error recovery utilities
class ErrorRecovery {
public:
    enum class RecoveryAction {
        RETRY,
        FALLBACK,
        GRACEFUL_DEGRADATION,
        ALERT,
        RESET,
        TERMINATE
    };

    struct RecoveryStrategy {
        std::function<bool(const NLMError&)> condition;
        RecoveryAction action;
        std::function<void()> recoveryAction;
        int maxRetries;
        std::chrono::milliseconds retryDelay;
    };

    ErrorRecovery();
    ~ErrorRecovery() = default;
    
    void addRecoveryStrategy(const RecoveryStrategy& strategy);
    RecoveryAction handleError(const NLMError& error, const ErrorContext& context);
    
    static ErrorRecovery& getInstance();
    static void resetInstance();

private:
    std::vector<RecoveryStrategy> strategies_;
    mutable std::mutex mutex_;
};

// Logging macros
#define NLM_LOG(level, operation, message) \
    do { \
        nlm::ErrorHandler::getInstance().log##level(message, nlm::ErrorContext(operation, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION)); \
    } while(0)

#define NLM_LOG_ERROR(message, operation) NLM_LOG(ERROR, operation, message)
#define NLM_LOG_WARNING(message, operation) NLM_LOG(WARNING, operation, message)
#define NLM_LOG_INFO(message, operation) NLM_LOG(INFO, operation, message)
#define NLM_LOG_DEBUG(message, operation) NLM_LOG(DEBUG, operation, message)

// Runtime error checking utilities
namespace RuntimeChecks {
    template<typename T>
    T checkNotNull(const T& value, const std::string& errorMessage,
                  const std::string& operation = "");
    
    template<typename T>
    T checkRange(const T& value, const T& min, const T& max,
                const std::string& errorMessage,
                const std::string& operation = "");
    
    template<typename T>
    T checkValidIndex(size_t index, size_t maxSize,
                     const std::string& errorMessage,
                     const std::string& operation = "");
    
    void checkMemoryLimit(size_t maxMemoryBytes,
                         const std::string& operation = "");
    
    bool isInRecoveryMode();
    void setRecoveryMode(bool enabled);
}

// Initialize error handling system
inline void initializeErrorHandling(const ErrorHandler::ErrorHandlerConfig& config = ErrorHandler::ErrorHandlerConfig()) {
    ErrorHandler::getInstance().setConfig(config);
}

} // namespace nlm

// Include implementation files
#include "ErrorHandling.cpp"
#include "MemoryTracker.cpp"
#include "NeuralOperationError.cpp"
#include "ErrorRecovery.cpp"
#include "RuntimeChecks.cpp"
