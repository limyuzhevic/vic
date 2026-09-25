#pragma once

#include <string>
#include <stdexcept>
#include <chrono>
#include <mutex>
#include <map>

namespace nlm {

/**
 * @brief Performance metrics tracking for error handling operations
 */
struct ErrorPerformanceMetrics {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::duration<double> totalValidationTime;
    std::chrono::duration<double> totalRecoveryTime;
    std::chrono::duration<double> totalErrorHandlingTime;
    size_t validationCount;
    size_t recoveryCount;
    size_t errorCount;
    
    ErrorPerformanceMetrics()
        : totalValidationTime(0.0), totalRecoveryTime(0.0), totalErrorHandlingTime(0.0)
        , validationCount(0), recoveryCount(0), errorCount(0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordValidation(std::chrono::duration<double> duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalValidationTime += duration;
        ++validationCount;
    }
    
    void recordRecovery(std::chrono::duration<double> duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalRecoveryTime += duration;
        ++recoveryCount;
    }
    
    void recordErrorHandling(std::chrono::duration<double> duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalErrorHandlingTime += duration;
        ++errorCount;
    }
    
    double getValidationTimePerOperation() const {
        return validationCount > 0 ? totalValidationTime.count() / validationCount : 0.0;
    }
    
    double getRecoveryTimePerOperation() const {
        return recoveryCount > 0 ? totalRecoveryTime.count() / recoveryCount : 0.0;
    }
    
    double getErrorHandlingTimePerOperation() const {
        return errorCount > 0 ? totalErrorHandlingTime.count() / errorCount : 0.0;
    }
    
    std::string report() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string result = "Error Performance Metrics:\n";
        result += "  Validation: " + std::to_string(validationCount) + " operations, " + 
                 std::to_string(getValidationTimePerOperation() * 1000) + " ms/op\n";
        result += "  Recovery: " + std::to_string(recoveryCount) + " operations, " +
                 std::to_string(getRecoveryTimePerOperation() * 1000) + " ms/op\n";
        result += "  Error Handling: " + std::to_string(errorCount) + " operations, " +
                 std::to_string(getErrorHandlingTimePerOperation() * 1000) + " ms/op\n";
        return result;
    }
    
private:
    mutable std::mutex mutex_;
};

/**
 * @brief Base class for all NLM exceptions
 */
class NLMError : public std::runtime_error {
public:
    enum class Category {
        Configuration,        // Configuration/validation errors
        Memory,              // Memory allocation/failure errors
        Plasticity,          // Plasticity rule errors
        Neuromodulation,     // Neuromodulator errors
        Development,         // Development process errors
        Prediction,          // Prediction system errors
        WorkingMemory,       // Working memory errors
        EpisodicMemory,      // Episodic memory errors
        Action,              // Action production errors
        Environment,         // World/environment errors
        Network,             // Network/connection errors
        Resource,            // Resource allocation errors
        State,               // State validation errors
        Permission,          // Permission/access errors
        Performance,         // Performance-related errors
        Unknown              // Catch-all for unspecified errors
    };
    
protected:
    NLMError(const std::string& message, Category category, 
             const std::string& context = "")
        : std::runtime_error(formatMessage(message, category, context)),
          category_(category), context_(context), timestamp_(std::chrono::high_resolution_clock::now()) {}
    
    virtual ~NLMError() = default;
    
    static std::string formatMessage(const std::string& message, Category category, const std::string& context) {
        std::string result = "[NLM ";
        result += categoryToString(category);
        result += "] ";
        if (!context.empty()) {
            result += "Context: " + context + ". ";
        }
        result += message;
        return result;
    }
    
    static const char* categoryToString(Category category) {
        static const char* categoryStrings[] = {
            "CONFIG", "MEMORY", "PLASTICITY", "NEUROMODULATION", 
            "DEVELOPMENT", "PREDICTION", "WORKING_MEMORY", "EPISODIC_MEMORY",
            "ACTION", "ENVIRONMENT", "NETWORK", "RESOURCE", "STATE", "PERMISSION",
            "PERFORMANCE", "UNKNOWN"
        };
        return categoryStrings[static_cast<int>(category)];
    }
    
public:
    Category getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    std::chrono::high_resolution_clock::time_point getTimestamp() const { return timestamp_; }
    
    virtual std::string getDetailedMessage() const {
        return what();
    }
    
private:
    Category category_;
    std::string context_;
    std::chrono::high_resolution_clock::time_point timestamp_;
};

/**
 * @brief Configuration and validation errors
 */
class NLMConfigError : public NLMError {
public:
    enum class ErrorType {
        InvalidParameter,        // Invalid parameter value
        MissingParameter,        // Required parameter missing
        ParameterRange,          // Parameter outside valid range
        FileNotFound,            // Configuration file not found
        FileFormat,              // Invalid file format
        CommandLineError,        // Command line parsing error
        DuplicateKey,            // Duplicate configuration key
        CircularReference,       // Circular dependency detected
        SchemaValidation,        // Schema validation error
        InvalidValueType         // Value type mismatch
    };
    
    NLMConfigError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Configuration error: " + message, Category::Configuration, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "INVALID_PARAMETER", "MISSING_PARAMETER", "PARAMETER_RANGE", "FILE_NOT_FOUND",
            "FILE_FORMAT", "COMMAND_LINE_ERROR", "DUPLICATE_KEY", "CIRCULAR_REFERENCE",
            "SCHEMA_VALIDATION", "INVALID_VALUE_TYPE"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Memory system errors
 */
class NLMMemoryError : public NLMError {
public:
    enum class ErrorType {
        AllocationFailure,       // Memory allocation failed
        CapacityExceeded,        // Memory capacity exceeded
        ConsistencyViolation,    // Memory consistency error
        Corruption,              // Memory corruption detected
        EncodingError,           // Encoding/decoding error
        Timeout,                 // Operation timed out
        ConcurrentAccess,        // Concurrent access violation
        InvalidReference         // Invalid memory reference
    };
    
    NLMMemoryError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Memory error: " + message, Category::Memory, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Memory Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "ALLOCATION_FAILURE", "CAPACITY_EXCEEDED", "CONSISTENCY_VIOLATION",
            "CORRUPTION", "ENCODING_ERROR", "TIMEOUT", "CONCURRENT_ACCESS",
            "INVALID_REFERENCE"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Plasticity system errors
 */
class NLMPlasticityError : public NLMError {
public:
    enum class ErrorType {
        InvalidSynapticWeight,   // Invalid synaptic weight
        PlasticityRuleError,     // Plasticity rule execution error
        HebbianError,            // Hebbian learning error
        STDPError,               // Spike-Timing Dependent Plasticity error
        StructuralPlasticityError, // Structural plasticity error
        MemoryTraceError,        // Memory trace error
        IntegrationError         // Integration with other systems error
    };
    
    NLMPlasticityError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Plasticity error: " + message, Category::Plasticity, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Plasticity Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "INVALID_SYNAPTIC_WEIGHT", "PLASTICITY_RULE_ERROR", "HEBBIAN_ERROR",
            "STDP_ERROR", "STRUCTURAL_PLASTICITY_ERROR", "MEMORY_TRACE_ERROR",
            "INTEGRATION_ERROR"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Neuromodulation system errors
 */
class NLMNeuromodulationError : public NLMError {
public:
    enum class ErrorType {
        DopamineError,           // Dopamine modulation error
        CuriosityError,          // Curiosity modulation error
        NoveltyError,            // Novelty detection error
        PredictionError,         // Prediction error computation error
        RewardError,             // Reward processing error
        SignalIntegrationError,  // Neuromodulator signal integration error
        PlasticityModulationError // Plasticity modulation error
    };
    
    NLMNeuromodulationError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Neuromodulation error: " + message, Category::Neuromodulation, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Neuromodulation Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "DOPAMINE_ERROR", "CURIOSITY_ERROR", "NOVELTY_ERROR", "PREDICTION_ERROR",
            "REWARD_ERROR", "SIGNAL_INTEGRATION_ERROR", "PLASTICITY_MODULATION_ERROR"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Development system errors
 */
class NLMDevelopmentError : public NLMError {
public:
    enum class ErrorType {
        SynaptogenesisError,    // Synaptogenesis error
        MaturationError,         // Maturation error
        PruningError,            // Pruning error
        DevelopmentalStageError, // Developmental stage error
        GrowthError,             // Growth error
        IntegrationError         // Integration with other systems error
    };
    
    NLMDevelopmentError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Development error: " + message, Category::Development, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Development Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "SYNAPTOGENESIS_ERROR", "MATURATION_ERROR", "PRUNING_ERROR",
            "DEVELOPMENTAL_STAGE_ERROR", "GROWTH_ERROR", "INTEGRATION_ERROR"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Environment/world system errors
 */
class NLMEnvironmentError : public NLMError {
public:
    enum class ErrorType {
        BoundaryViolation,       // Boundary/environment violation
        ObservationError,        // Observation error
        WorldStateError,         // World state error
        SimulationError,         // Simulation error
        EnvironmentalChangeError, // Environmental change error
        InputOutputError         // Input/output error
    };
    
    NLMEnvironmentError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Environment error: " + message, Category::Environment, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Environment Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "BOUNDARY_VIOLATION", "OBSERVATION_ERROR", "WORLD_STATE_ERROR",
            "SIMULATION_ERROR", "ENVIRONMENTAL_CHANGE_ERROR", "INPUT_OUTPUT_ERROR"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Action production system errors
 */
class NLMActionError : public NLMError {
public:
    enum class ErrorType {
        ActionSelectionError,    // Action selection error
        MotorError,             // Motor error
        PlanningError,          // Planning error
        OutputError             // Output generation error
    };
    
    NLMActionError(const std::string& message, ErrorType errorType, const std::string& context = "")
        : NLMError("Action error: " + message, Category::Action, context),
          errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    std::string getDetailedMessage() const override {
        std::string result = NLMError::getDetailedMessage();
        result += " (Action Error Type: " + errorTypeToString(errorType_) + ")";
        return result;
    }
    
private:
    static const char* errorTypeToString(ErrorType type) {
        static const char* typeStrings[] = {
            "ACTION_SELECTION_ERROR", "MOTOR_ERROR", "PLANNING_ERROR", "OUTPUT_ERROR"
        };
        return typeStrings[static_cast<int>(type)];
    }
    
    ErrorType errorType_;
};

/**
 * @brief Main error handling interface
 */
class ErrorHandler {
public:
    enum class RecoveryStrategy {
        None,               // No recovery
        Retry,              // Retry operation
        Fallback,           // Use fallback value/behavior
        Reset,              // Reset to safe state
        PartialRecovery,    // Partial recovery
        EmergencyShutdown,  // Emergency shutdown
        Ignore              // Ignore error
    };
    
    struct ErrorContext {
        std::string operation;
        std::string component;
        std::string details;
        std::chrono::high_resolution_clock::time_point timestamp;
        double performanceImpact; // microseconds
        
        ErrorContext(const std::string& op, const std::string& comp, const std::string& det)
            : operation(op), component(comp), details(det), timestamp(std::chrono::high_resolution_clock::now()), performanceImpact(0.0) {}
    };
    
    ErrorHandler() : metrics_(), enabled_(true), recovery_enabled_(true) {}
    
    // Exception throwing
    template<typename T, typename... Args>
    T throwError(const std::string& message, Category category, const std::string& context = "", Args&&... args) {
        recordErrorHandling(context, message);
        throw NLMError(formatMessageWithArgs(message, args...), category, context);
    }
    
    template<typename T, typename... Args>
    T throwError(const std::string& message, RecoveryStrategy strategy, const std::string& context = "", Args&&... args) {
        recordErrorHandling(context, message);
        if (strategy == RecoveryStrategy::Ignore) {
            return T();
        }
        // For other strategies, log and throw
        throw NLMError(formatMessageWithArgs(message, args...), Category::Unknown, context);
    }
    
    // Validation with recovery
    template<typename T, typename Validator, typename... Args>
    T validateAndRecover(const std::string& operation, const std::string& component,
                        const T& value, Validator validator, 
                        RecoveryStrategy recovery, T fallbackValue,
                        const std::string& context = "", Args&&... args) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (!validator(value)) {
            auto duration = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - startTime);
            recordValidationError(operation, component, duration, context, args...);
            
            if (recovery_enabled_ && recovery != RecoveryStrategy::None) {
                auto recoveryStart = std::chrono::high_resolution_clock::now();
                T result = applyRecovery(operation, component, value, recovery, fallbackValue, context, args...);
                auto recoveryDuration = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - recoveryStart);
                metrics_.recordRecovery(recoveryDuration);
                return result;
            }
            
            throw NLMError(formatMessageWithArgs("Validation failed for " + operation + ": " + message, args...), 
                         Category::Configuration, context);
        }
        
        auto duration = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - startTime);
        metrics_.recordValidation(duration);
        return value;
    }
    
    // Exception conversion utilities
    template<typename T>
    T convertToPythonException(const NLMError& e) {
        // Convert NLM errors to appropriate Python exceptions
        if (const NLMConfigError* configError = dynamic_cast<const NLMConfigError*>(&e)) {
            // Convert to ValueError or RuntimeError based on error type
        } else if (const NLMMemoryError* memoryError = dynamic_cast<const NLMMemoryError*>(&e)) {
            // Convert to MemoryError or RuntimeError
        }
        // Continue for other error types...
        
        // Default: convert to RuntimeError
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return T();
    }
    
    // State validation
    template<typename StateType>
    bool validateState(const std::string& operation, const StateType& state, 
                      const std::string& context = "") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool valid = checkStatePreconditions(operation, state);
        if (!valid) {
            auto duration = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - startTime);
            recordValidationError(operation, "State", duration, context);
        }
        
        metrics_.recordValidation(duration);
        return valid;
    }
    
    // Performance impact tracking
    void recordPerformanceImpact(const std::string& operation, const std::string& component, 
                                double microseconds) {
        std::lock_guard<std::mutex> lock(mutex_);
        performance_impact_[operation + "." + component] += microseconds;
    }
    
    std::string getPerformanceReport() const {
        return metrics_.report();
    }
    
    std::string getDetailedPerformanceReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string result = metrics_.report();
        result += "\nDetailed Performance Impact:\n";
        for (const auto& entry : performance_impact_) {
            result += "  " + entry.first + ": " + std::to_string(entry.second) + " µs\n";
        }
        return result;
    }
    
    void enableRecovery(bool enable) { recovery_enabled_ = enable; }
    void enableErrorHandling(bool enable) { enabled_ = enable; }
    
    template<typename Predicate>
    void setRecoveryStrategy(const std::string& errorPattern, RecoveryStrategy strategy, Predicate predicate) {
        std::lock_guard<std::mutex> lock(mutex_);
        recovery_strategies_[errorPattern] = {strategy, predicate};
    }
    
protected:
    virtual T applyRecovery(const std::string& operation, const std::string& component,
                           const T& value, RecoveryStrategy recovery,
                           T fallbackValue, const std::string& context, Args&&... args) {
        switch (recovery) {
            case RecoveryStrategy::Retry:
                // Implement retry logic
                return fallbackValue;
            case RecoveryStrategy::Fallback:
                return fallbackValue;
            case RecoveryStrategy::Reset:
                // Implement reset logic
                return fallbackValue;
            case RecoveryStrategy::PartialRecovery:
                // Implement partial recovery
                return fallbackValue;
            case RecoveryStrategy::EmergencyShutdown:
                // Implement emergency shutdown
                exit(1);
            case RecoveryStrategy::Ignore:
                return fallbackValue;
            default:
                return fallbackValue;
        }
    }
    
    virtual bool checkStatePreconditions(const std::string& operation, const StateType& state) {
        return true; // Default implementation - can be overridden
    }
    
    virtual void recordValidationError(const std::string& operation, const std::string& component,
                                     const std::chrono::duration<double, std::micro>& duration,
                                     const std::string& context, Args&&... args) {
        // Default implementation - can be overridden
    }
    
    virtual void recordErrorHandling(const std::string& context, const std::string& message) {
        // Default implementation - can be overridden
    }
    
private:
    template<typename... Args>
    std::string formatMessageWithArgs(const std::string& message, Args&&... args) {
        // Format message with arguments (simplified implementation)
        return message;
    }
    
    mutable std::mutex mutex_;
    ErrorPerformanceMetrics metrics_;
    bool enabled_;
    bool recovery_enabled_;
    std::map<std::string, std::pair<RecoveryStrategy, std::function<bool()>>> recovery_strategies_;
    mutable std::map<std::string, double> performance_impact_;
};

// Global error handler instance
inline ErrorHandler& getErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

// Convenience functions for throwing errors
inline void throwConfigError(const std::string& message, NLMConfigError::ErrorType type, 
                             const std::string& context = "") {
    throw NLMConfigError(message, type, context);
}

inline void throwMemoryError(const std::string& message, NLMMemoryError::ErrorType type,
                             const std::string& context = "") {
    throw NLMMemoryError(message, type, context);
}

inline void throwPlasticityError(const std::string& message, NLMPlasticityError::ErrorType type,
                                 const std::string& context = "") {
    throw NLMPlasticityError(message, type, context);
}

inline void throwNeuromodulationError(const std::string& message, NLMNeuromodulationError::ErrorType type,
                                       const std::string& context = "") {
    throw NLMNeuromodulationError(message, type, context);
}

inline void throwDevelopmentError(const std::string& message, NLMDevelopmentError::ErrorType type,
                                   const std::string& context = "") {
    throw NLMDevelopmentError(message, type, context);
}

inline void throwEnvironmentError(const std::string& message, NLMEnvironmentError::ErrorType type,
                                   const std::string& context = "") {
    throw NLMEnvironmentError(message, type, context);
}

inline void throwActionError(const std::string& message, NLMActionError::ErrorType type,
                              const std::string& context = "") {
    throw NLMActionError(message, type, context);
}

// Performance tracking macros
#define NLM_ERROR_PERFORMANCE_SCOPE(operation, component) \
    auto operation##_start_time = std::chrono::high_resolution_clock::now(); \
    const std::string operation##_operation = operation; \
    const std::string operation##_component = component

#define NLM_ERROR_PERFORMANCE_END(operation) \
    auto operation##_end_time = std::chrono::high_resolution_clock::now(); \
    auto operation##_duration = std::chrono::duration<double, std::micro>(operation##_end_time - operation##_start_time); \
    getErrorHandler().recordPerformanceImpact(operation##_operation, operation##_component, operation##_duration.count())

#define NLM_VALIDATE_WITH_RECOVERY(value, validator, recovery, fallback, context) \
    getErrorHandler().validateAndRecover<decltype(value)>(#validator, "Validation", value, validator, recovery, fallback, context)

#define NLM_THROW_ERROR(message, category, context) \
    getErrorHandler().throwError<void>(message, category, context)
