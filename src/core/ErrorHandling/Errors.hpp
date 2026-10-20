#pragma once

#include <string>
#include <map>
#include <chrono>
#include <memory>
#include <sstream>
#include <type_traits>

namespace nlm {

// Error codes for different types of errors
enum class ErrorCode {
    // Configuration errors
    CONFIG_FILE_NOT_FOUND = 1000,
    CONFIG_INVALID_FORMAT,
    CONFIG_VALIDATION_FAILED,
    CONFIG_KEY_NOT_FOUND,
    CONFIG_TYPE_MISMATCH,
    
    // Neural network errors
    NEURON_INITIALIZATION_FAILED = 2000,
    SYNAPSE_CREATION_FAILED,
    POPULATION_OVERFLOW,
    REGION_NOT_FOUND,
    INVALID_NEURON_TYPE,
    
    // Simulation errors
    SIMULATION_NOT_STARTED = 3000,
    SIMULATION_STEP_FAILED,
    INVALID_TIME_STEP,
    CLOCK_SYNC_FAILED,
    TICK_FAILED,
    
    // Memory errors
    MEMORY_ALLOCATION_FAILED = 4000,
    MEMORY_CORRUPTION,
    NULL_POINTER_DEREFERENCE,
    INVALID_MEMORY_ACCESS,
    MEMORY_POOL_EXHAUSTED,
    
    // Validation errors
    INPUT_VALIDATION_FAILED = 5000,
    RANGE_VALIDATION_FAILED,
    TYPE_VALIDATION_FAILED,
    CONSTRAINT_VALIDATION_FAILED,
    SCHEMA_VALIDATION_FAILED,
    DATA_INTEGRITY_FAILED,
    
    // Sensory system errors
    SENSOR_NOT_INITIALIZED = 6000,
    SENSOR_DATA_CORRUPTED,
    SENSOR_OVERFLOW,
    SENSOR_TIMEOUT,
    AUDIO_PROCESSING_FAILED,
    VISION_PROCESSING_FAILED,
    
    // Motor system errors
    MOTOR_COMMAND_FAILED = 7000,
    ACTION_QUEUE_FULL,
    INVALID_ACTION_TYPE,
    MOTOR_SYSTEM_BUSY,
    ACTUATOR_FAILURE,
    
    // Plasticity errors
    PLASTICITY_RULE_FAILED = 8000,
    INVALID_PLACING_METHOD,
    STDP_CALCULATION_FAILED,
    HEBBIAN_UPDATE_FAILED,
    
    // Neuromodulation errors
    MODULATOR_NOT_INITIALIZED = 9000,
    NOVELTY_DETECTION_FAILED,
    REWARD_CALCULATION_FAILED,
    PREDICTION_ERROR_CALCULATION_FAILED,
    CURIOUSY_TOO_LOW,
    
    // Prediction errors
    PREDICTION_FAILED = 10000,
    MODEL_NOT_TRAINED,
    INVALID_PREDICTION_TYPE,
    PREDICTION_TIMEOUT,
    
    // Cognition errors
    CONCEPT_FORMATION_FAILED = 11000,
    PLANNING_FAILED,
    INVALID_CONCEPT_TYPE,
    
    // Memory system errors
    WORKING_MEMORY_FULL = 12000,
    EPISODIC_MEMORY_WRITE_FAILED,
    SEMANTIC_MEMORY_ACCESS_FAILED,
    PROCEEDURAL_MEMORY_RECALL_FAILED,
    
    // Performance errors
    PERFORMANCE_LIMIT_EXCEEDED = 13000,
    OPTIMIZATION_FAILED,
    SIMD_NOT_SUPPORTED,
    
    // General errors
    UNKNOWN_ERROR = 20000,
    NOT_IMPLEMENTED,
    OUT_OF_MEMORY,
    INVALID_ARGUMENT,
    TIMEOUT,
    NETWORK_ERROR,
    IO_ERROR,
    PERMISSION_DENIED,
    SYSTEM_ERROR
};

// Exception hierarchy
class NLMException : public std::exception {
protected:
    ErrorCode code_;
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::map<std::string, std::string> context_;
    std::shared_ptr<NLMException> cause_;

public:
    NLMException(ErrorCode code, const std::string& message)
        : code_(code), message_(message), file_(""), line_(0), function_("") {
        timestamp_ = std::chrono::system_clock::now();
    }

    NLMException(ErrorCode code, const std::string& message,
                 const std::string& file, int line, const std::string& function)
        : code_(code), message_(message), file_(file), line_(line), function_(function) {
        timestamp_ = std::chrono::system_clock::now();
    }

    virtual ~NLMException() noexcept = default;

    // Getters
    ErrorCode getCode() const noexcept { return code_; }
    const std::string& getMessage() const noexcept { return message_; }
    const std::string& getFile() const noexcept { return file_; }
    int getLine() const noexcept { return line_; }
    const std::string& getFunction() const noexcept { return function_; }
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return timestamp_; }

    // Context management
    void addContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }

    const std::map<std::string, std::string>& getContext() const noexcept { return context_; }

    // Cause management
    void setCause(std::shared_ptr<NLMException> cause) { cause_ = cause; }
    std::shared_ptr<NLMException> getCause() const noexcept { return cause_; }

    // Exception interface
    virtual const char* what() const noexcept override {
        static std::string whatMsg;
        std::ostringstream oss;
        oss << "[" << static_cast<int>(code_) << "] " << message_;
        if (!file_.empty()) {
            oss << " (" << file_ << ":" << line_ << ")";
        }
        if (!function_.empty()) {
            oss << " in " << function_;
        }
        whatMsg = oss.str();
        return whatMsg.c_str();
    }

    // Serialization
    virtual std::string serialize() const {
        std::ostringstream oss;
        oss << static_cast<int>(code_) << "," << message_;
        return oss.str();
    }

    // Deserialization factory
    static std::shared_ptr<NLMException> deserialize(const std::string& data) {
        // Parse data and create appropriate exception type
        // This is a simplified implementation
        size_t comma = data.find(',');
        if (comma == std::string::npos) {
            return std::make_shared<NLMException>(ErrorCode::UNKNOWN_ERROR, "Invalid error format");
        }
        int code = std::stoi(data.substr(0, comma));
        std::string message = data.substr(comma + 1);
        return std::make_shared<NLMException>(static_cast<ErrorCode>(code), message);
    }

    // Virtual destructor for polymorphism
    virtual void log() const {
        std::cerr << "Exception: " << what() << std::endl;
    }
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message,
                          const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::CONFIG_VALIDATION_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Configuration:" + base;
    }

    static std::shared_ptr<ConfigurationException> deserialize(const std::string& data) {
        std::string message = data.substr(14); // Remove "Configuration:" prefix
        return std::make_shared<ConfigurationException>(message);
    }
};

class NeuralNetworkException : public NLMException {
public:
    NeuralNetworkException(const std::string& message,
                         const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::INVALID_NEURON_TYPE, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "NeuralNetwork:" + base;
    }

    static std::shared_ptr<NeuralNetworkException> deserialize(const std::string& data) {
        std::string message = data.substr(14); // Remove "NeuralNetwork:" prefix
        return std::make_shared<NeuralNetworkException>(message);
    }
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message,
                       const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::SIMULATION_STEP_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Simulation:" + base;
    }

    static std::shared_ptr<SimulationException> deserialize(const std::string& data) {
        std::string message = data.substr(12); // Remove "Simulation:" prefix
        return std::make_shared<SimulationException>(message);
    }
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message,
                       const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::INPUT_VALIDATION_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Validation:" + base;
    }

    static std::shared_ptr<ValidationException> deserialize(const std::string& data) {
        std::string message = data.substr(11); // Remove "Validation:" prefix
        return std::make_shared<ValidationException>(message);
    }
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message,
                   const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::MEMORY_ALLOCATION_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Memory:" + base;
    }

    static std::shared_ptr<MemoryException> deserialize(const std::string& data) {
        std::string message = data.substr(7); // Remove "Memory:" prefix
        return std::make_shared<MemoryException>(message);
    }
};

class SensoryException : public NLMException {
public:
    SensoryException(const std::string& message,
                    const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::SENSOR_NOT_INITIALIZED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Sensory:" + base;
    }

    static std::shared_ptr<SensoryException> deserialize(const std::string& data) {
        std::string message = data.substr(8); // Remove "Sensory:" prefix
        return std::make_shared<SensoryException>(message);
    }
};

class MotorException : public NLMException {
public:
    MotorException(const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::MOTOR_COMMAND_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Motor:" + base;
    }

    static std::shared_ptr<MotorException> deserialize(const std::string& data) {
        std::string message = data.substr(7); // Remove "Motor:" prefix
        return std::make_shared<MotorException>(message);
    }
};

class PlasticityException : public NLMException {
public:
    PlasticityException(const std::string& message,
                      const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::PLASTICITY_RULE_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Plasticity:" + base;
    }

    static std::shared_ptr<PlasticityException> deserialize(const std::string& data) {
        std::string message = data.substr(12); // Remove "Plasticity:" prefix
        return std::make_shared<PlasticityException>(message);
    }
};

class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message,
                           const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::MODULATOR_NOT_INITIALIZED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Neuromodulation:" + base;
    }

    static std::shared_ptr<NeuromodulationException> deserialize(const std::string& data) {
        std::string message = data.substr(16); // Remove "Neuromodulation:" prefix
        return std::make_shared<NeuromodulationException>(message);
    }
};

class PredictionException : public NLMException {
public:
    PredictionException(const std::string& message,
                       const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::PREDICTION_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Prediction:" + base;
    }

    static std::shared_ptr<PredictionException> deserialize(const std::string& data) {
        std::string message = data.substr(11); // Remove "Prediction:" prefix
        return std::make_shared<PredictionException>(message);
    }
};

class CognitionException : public NLMException {
public:
    CognitionException(const std::string& message,
                     const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(ErrorCode::CONCEPT_FORMATION_FAILED, message, file, line, function) {}

    std::string serialize() const override {
        std::string base = NLMException::serialize();
        return "Cognition:" + base;
    }

    static std::shared_ptr<CognitionException> deserialize(const std::string& data) {
        std::string message = data.substr(10); // Remove "Cognition:" prefix
        return std::make_shared<CognitionException>(message);
    }
};

// Macros for throwing exceptions with file/line info
#define NLM_THROW(code, msg) throw nlm::NLMException(code, msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_WITH_CAUSE(code, msg, cause) { auto exc = std::make_shared<nlm::NLMException>(code, msg, __FILE__, __LINE__, __func__); exc->setCause(cause); throw exc; }

// Validation functions
class Validation {
public:
    // Input validation
    template<typename T>
    static T validateRange(const T& value, const T& min, const T& max, const std::string& context = "") {
        if (value < min || value > max) {
            std::ostringstream oss;
            oss << "Value " << value << " is out of range [" << min << ", " << max << "]";
            if (!context.empty()) oss << " in context: " << context;
            NLM_THROW(ErrorCode::RANGE_VALIDATION_FAILED, oss.str());
        }
        return value;
    }

    template<typename T>
    static T validatePositive(const T& value, const std::string& context = "") {
        if (value <= 0) {
            std::ostringstream oss;
            oss << "Value must be positive, got " << value;
            if (!context.empty()) oss << " in context: " << context;
            NLM_THROW(ErrorCode::RANGE_VALIDATION_FAILED, oss.str());
        }
        return value;
    }

    template<typename T>
    static T validateNonNegative(const T& value, const std::string& context = "") {
        if (value < 0) {
            std::ostringstream oss;
            oss << "Value must be non-negative, got " << value;
            if (!context.empty()) oss << " in context: " << context;
            NLM_THROW(ErrorCode::RANGE_VALIDATION_FAILED, oss.str());
        }
        return value;
    }

    template<typename T>
    static T validateType(const T& value, const std::string& expectedType, const std::string& context = "") {
        // This is a placeholder for type validation
        // In practice, you'd use std::is_same or similar type traits
        if (context.find("string") != std::string::npos && !std::is_same<T, std::string>::value) {
            std::ostringstream oss;
            oss << "Expected string type, got " << typeid(T).name();
            if (!context.empty()) oss << " in context: " << context;
            NLM_THROW(ErrorCode::TYPE_VALIDATION_FAILED, oss.str());
        }
        return value;
    }

    template<typename T>
    static T validateNotNull(const T& value, const std::string& context = "") {
        // For pointers
        if constexpr (std::is_pointer<T>::value) {
            if (!value) {
                std::ostringstream oss;
                oss << "Value cannot be null";
                if (!context.empty()) oss << " in context: " << context;
                NLM_THROW(ErrorCode::NULL_POINTER_DEREFERENCE, oss.str());
            }
        }
        return value;
    }

    // Schema validation
    static bool validateSchema(const std::string& schema, const std::string& data, std::vector<std::string>& errors) {
        // Placeholder for schema validation
        // In practice, use a JSON schema validator or similar
        errors.clear();
        return true;
    }

    // Data integrity validation
    template<typename Container>
    static bool validateDataIntegrity(const Container& data, size_t maxSize, const std::string& context = "") {
        if (data.size() > maxSize) {
            std::ostringstream oss;
            oss << "Data size " << data.size() << " exceeds maximum " << maxSize;
            if (!context.empty()) oss << " in context: " << context;
            NLM_THROW(ErrorCode::DATA_INTEGRITY_FAILED, oss.str());
        }
        return true;
    }
};

// Error recovery mechanisms
class ErrorRecovery {
public:
    enum class RecoveryStrategy {
        FAIL_FAST,
        GRACEFUL_DEGRADATION,
        RETRY,
        FALLBACK,
        CIRCUIT_BREAKER,
        AUTO_RECOVER
    };

    struct RecoveryConfig {
        RecoveryStrategy strategy;
        int maxRetries;
        int timeoutMs;
        int retryDelayMs;
        double circuitBreakerThreshold;
        int circuitBreakerTimeoutMs;
        std::function<bool()> fallbackCondition;
        std::function<void()> recoveryAction;
    };

    // Circuit breaker implementation
    class CircuitBreaker {
    private:
        RecoveryConfig config_;
        int failureCount_ = 0;
        int successCount_ = 0;
        bool state_ = true; // true = closed, false = open
        std::chrono::system_clock::time_point lastStateChange_;

    public:
        CircuitBreaker(const RecoveryConfig& config) : config_(config) {
            lastStateChange_ = std::chrono::system_clock::now();
        }

        bool canExecute() const {
            if (!state_) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStateChange_);
                if (duration.count() >= config_.circuitBreakerTimeoutMs) {
                    state_ = true; // Close the circuit
                }
            }
            return state_;
        }

        void recordSuccess() {
            if (!state_) {
                successCount_++;
                if (successCount_ >= config_.circuitBreakerThreshold) {
                    state_ = true;
                    lastStateChange_ = std::chrono::system_clock::now();
                }
            }
        }

        void recordFailure() {
            if (state_) {
                failureCount_++;
                if (failureCount_ >= config_.circuitBreakerThreshold) {
                    state_ = false;
                    lastStateChange_ = std::chrono::system_clock::now();
                }
            }
        }

        bool isOpen() const { return !state_; }
    };

    // Retry mechanism
    template<typename Func, typename... Args>
    static auto withRetry(Func&& func, const RecoveryConfig& config, Args&&... args) {
        int attempt = 0;
        while (attempt < config.maxRetries) {
            try {
                return func(std::forward<Args>(args)...);
            } catch (const NLMException&) {
                attempt++;
                if (attempt >= config.maxRetries) throw;
                std::this_thread::sleep_for(std::chrono::milliseconds(config.retryDelayMs));
            }
        }
    }

    // Graceful degradation
    template<typename Func, typename... Args>
    static auto withGracefulDegradation(Func&& func, const RecoveryConfig& config, Args&&... args) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const NLMException& e) {
            if (config.fallbackCondition()) {
                NLM_LOG_WARNING("Error occurred, using fallback: " << e.what());
                if (config.recoveryAction) config.recoveryAction();
                // Return a default/empty value or throw a fallback exception
                throw;
            }
            throw;
        }
    }
};

// Error reporting and diagnostics
class ErrorReporter {
public:
    struct DiagnosticInfo {
        std::string errorType;
        std::string errorMessage;
        std::string stackTrace;
        std::string systemState;
        std::string performanceImpact;
        std::map<std::string, std::string> context;
    };

    static DiagnosticInfo collectDiagnostics(const std::exception& e) {
        DiagnosticInfo info;
        info.errorType = typeid(e).name();
        info.errorMessage = e.what();
        // In practice, collect actual stack trace, system state, etc.
        info.stackTrace = "Stack trace would be collected here";
        info.systemState = "System state would be collected here";
        info.performanceImpact = "Performance impact would be analyzed here";
        return info;
    }

    static void reportError(const NLMException& e, bool logStackTrace = true) {
        DiagnosticInfo diag = collectDiagnostics(e);
        NLM_LOG_ERROR("Error reported: " << e.what());
        if (logStackTrace) {
            NLM_LOG_ERROR("Stack trace: " << diag.stackTrace);
        }
        if (!diag.performanceImpact.empty()) {
            NLM_LOG_WARNING("Performance impact: " << diag.performanceImpact);
        }
    }
};

} // namespace nlm

// Global error handler
class GlobalErrorHandler {
public:
    enum class Mode {
        THROW,
        LOG_ONLY,
        TERMINATE,
        CUSTOM
    };

    using ErrorHandlerFunc = std::function<void(const nlm::NLMException&)>;

private:
    Mode mode_ = Mode::THROW;
    std::vector<ErrorHandlerFunc> customHandlers_;
    std::mutex mutex_;

public:
    void handle(const nlm::NLMException& e) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        switch (mode_) {
            case Mode::THROW:
                throw e;
            case Mode::LOG_ONLY:
                e.log();
                break;
            case Mode::TERMINATE:
                std::cerr << "Fatal error: " << e.what() << std::endl;
                std::terminate();
            case Mode::CUSTOM:
                for (const auto& handler : customHandlers_) {
                    handler(e);
                }
                break;
        }
    }

    void setMode(Mode mode) { mode_ = mode; }
    void addCustomHandler(ErrorHandlerFunc handler) { customHandlers_.push_back(handler); }
    Mode getMode() const { return mode_; }
};

// Global error handler instance
inline GlobalErrorHandler& getGlobalErrorHandler() {
    static GlobalErrorHandler handler;
    return handler;
}

// Convenience macros for error handling
#define NLM_TRY_CATCH(rethrow) try { \
} catch (const nlm::NLMException& e) { \
    NLM_LOG_ERROR("Exception caught: " << e.what()); \
    if (rethrow) throw; \
}

#define NLM_SAFE_CALL(code, msg, func) try { \
    func; \
} catch (const nlm::NLMException& e) { \
    NLM_THROW(code, msg << ": " << e.what()); \
}

// Error codes for convenience
namespace ErrorCodes {
    const nlm::ErrorCode CONFIG_FILE_NOT_FOUND = nlm::ErrorCode::CONFIG_FILE_NOT_FOUND;
    const nlm::ErrorCode NEURON_INITIALIZATION_FAILED = nlm::ErrorCode::NEURON_INITIALIZATION_FAILED;
    const nlm::ErrorCode SIMULATION_STEP_FAILED = nlm::ErrorCode::SIMULATION_STEP_FAILED;
    const nlm::ErrorCode MEMORY_ALLOCATION_FAILED = nlm::ErrorCode::MEMORY_ALLOCATION_FAILED;
    const nlm::ErrorCode INPUT_VALIDATION_FAILED = nlm::ErrorCode::INPUT_VALIDATION_FAILED;
    const nlm::ErrorCode SENSOR_NOT_INITIALIZED = nlm::ErrorCode::SENSOR_NOT_INITIALIZED;
    const nlm::ErrorCode MOTOR_COMMAND_FAILED = nlm::ErrorCode::MOTOR_COMMAND_FAILED;
    const nlm::ErrorCode PLASTICITY_RULE_FAILED = nlm::ErrorCode::PLASTICITY_RULE_FAILED;
    const nlm::ErrorCode MODULATOR_NOT_INITIALIZED = nlm::ErrorCode::MODULATOR_NOT_INITIALIZED;
    const nlm::ErrorCode PREDICTION_FAILED = nlm::ErrorCode::PREDICTION_FAILED;
    const nlm::ErrorCode CONCEPT_FORMATION_FAILED = nlm::ErrorCode::CONCEPT_FORMATION_FAILED;
    const nlm::ErrorCode WORKING_MEMORY_FULL = nlm::ErrorCode::WORKING_MEMORY_FULL;
}

} // namespace nlm