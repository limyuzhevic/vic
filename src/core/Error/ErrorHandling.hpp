#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <functional>
#include <optional>

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    None = 0,
    Memory = 1,
    Neural = 2,
    Plasticity = 3,
    Development = 4,
    Neuromodulation = 5,
    MemorySystem = 6,
    Prediction = 7,
    Cognition = 8,
    Sensory = 9,
    Motor = 10,
    Environment = 11,
    Configuration = 12,
    Logging = 13,
    Simulation = 14,
    DevelopmentSystem = 15
};

// Error codes for specific error conditions
enum class ErrorCode {
    // Success
    Success = 0,
    
    // Memory errors
    MemoryAllocationFailed = 1001,
    MemoryAccessViolation = 1002,
    MemoryCorruption = 1003,
    
    // Neural computation errors
    NeuronCreationFailed = 2001,
    SynapseCreationFailed = 2002,
    NeuralRegionCreationFailed = 2003,
    SpikePropagationFailed = 2004,
    InvalidNeuronState = 2005,
    
    // Plasticity errors
    PlasticityRuleFailed = 3001,
    STDPError = 3002,
    HebbianLearningFailed = 3003,
    StructuralPlasticityFailed = 3004,
    InvalidPlasticityFlags = 3005,
    
    // Development errors
    SynaptogenesisFailed = 4001,
    PruningFailed = 4002,
    MaturationFailed = 4003,
    DevelopmentalStageInvalid = 4004,
    
    // Neuromodulation errors
    NeuromodulatorCreationFailed = 5001,
    DopamineCalculationFailed = 5002,
    RewardProcessingFailed = 5003,
    PredictionErrorComputationFailed = 5004,
    
    // Memory system errors
    WorkingMemoryAccessFailed = 6001,
    EpisodicMemoryStorageFailed = 6002,
    AssociativeMemoryCreationFailed = 6003,
    MemoryReplayFailed = 6004,
    
    // Prediction system errors
    PredictionComputationFailed = 7001,
    PredictionErrorCalculationFailed = 7002,
    PredictionLearningFailed = 7003,
    
    // Cognition errors
    ConceptFormationFailed = 8001,
    PlanningFailed = 8002,
    AttentionSelectionFailed = 8003,
    
    // Sensory/Motor errors
    SensoryProcessingFailed = 9001,
    MotorCommandFailed = 9002,
    SensorimotorIntegrationFailed = 9003,
    
    // Environment errors
    EnvironmentSetupFailed = 10001,
    ObservationFailed = 10002,
    ActionExecutionFailed = 10003,
    
    // Configuration errors
    ConfigurationLoadFailed = 11001,
    ConfigurationValidationFailed = 11002,
    ConfigurationSaveFailed = 11003,
    InvalidConfigurationValue = 11004,
    
    // Logging errors
    LogFileOpenFailed = 12001,
    LogFileWriteFailed = 12002,
    LogMemoryAllocationFailed = 12003,
    
    // Simulation errors
    SimulationStepFailed = 13001,
    SimulationResetFailed = 13002,
    SimulationInitializationFailed = 13003,
    InvalidTimestep = 13004,
    
    // General errors
    InvalidParameter = 14001,
    OperationCancelled = 14002,
    NotImplemented = 14003,
    Timeout = 14004,
    ResourceExhausted = 14005,
    
    // System errors
    SystemError = 15001,
    ThreadError = 15002,
    SemaphoreError = 15003,
    MutexError = 15004
};

// Error exception class
class Error : public std::runtime_error {
public:
    Error(const std::string& message, ErrorCode code = ErrorCode::SystemError,
          ErrorCategory category = ErrorCategory::None,
          const std::string& details = "")
        : std::runtime_error(formatMessage(message, code, category, details)),
          code_(code), category_(category), details_(details) {}
    
    ErrorCode getCode() const { return code_; }
    ErrorCategory getCategory() const { return category_; }
    const std::string& getDetails() const { return details_; }
    
    // Create specific error types for convenience
    class MemoryAllocationError : public Error {
    public:
        MemoryAllocationError(const std::string& details = "")
            : Error("Memory allocation failed", ErrorCode::MemoryAllocationFailed,
                   ErrorCategory::Memory, details) {}
    };
    
    class NeuronError : public Error {
    public:
        NeuronError(const std::string& message, const std::string& details = "")
            : Error("Neuron error: " + message, ErrorCode::NeuronCreationFailed,
                   ErrorCategory::Neural, details) {}
    };
    
    class PlasticityError : public Error {
    public:
        PlasticityError(const std::string& message, const std::string& details = "")
            : Error("Plasticity error: " + message, ErrorCode::PlasticityRuleFailed,
                   ErrorCategory::Plasticity, details) {}
    };
    
    class ConfigurationError : public Error {
    public:
        ConfigurationError(const std::string& message, const std::string& details = "")
            : Error("Configuration error: " + message, ErrorCode::ConfigurationValidationFailed,
                   ErrorCategory::Configuration, details) {}
    };
    
    class SimulationError : public Error {
    public:
        SimulationError(const std::string& message, const std::string& details = "")
            : Error("Simulation error: " + message, ErrorCode::SimulationStepFailed,
                   ErrorCategory::Simulation, details) {}
    };
    
private:
    static std::string formatMessage(const std::string& message, ErrorCode code,
                                    ErrorCategory category, const std::string& details) {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(category) << "] "
            << "[" << static_cast<int>(code) << "] "
            << message;
        if (!details.empty()) {
            oss << " (details: " << details << ")";
        }
        return oss.str();
    }
    
    ErrorCode code_;
    ErrorCategory category_;
    std::string details_;
};

// Error handler interface for custom error handling
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const Error& error) = 0;
    virtual bool shouldThrow(const Error& error) const = 0;
};

// Default error handler that throws exceptions
class DefaultErrorHandler : public IErrorHandler {
public:
    void handleError(const Error& error) override {
        if (shouldThrow(error)) {
            throw error;
        }
    }
    
    bool shouldThrow(const Error& error) const override {
        // Throw for most errors except certain types that can be recovered from
        ErrorCode code = error.getCode();
        switch (code) {
            case ErrorCode::MemoryAccessViolation:
            case ErrorCode::MemoryCorruption:
                return false; // Critical errors - may not be recoverable
            case ErrorCode::InvalidParameter:
            case ErrorCode::OperationCancelled:
                return true;
            default:
                return true;
        }
    }
};

// Global error handler management
class ErrorHandlerManager {
public:
    static ErrorHandlerManager& getInstance() {
        static ErrorHandlerManager instance;
        return instance;
    }
    
    void setErrorHandler(std::unique_ptr<IErrorHandler> handler) {
        errorHandler_ = std::move(handler);
    }
    
    void handleError(const Error& error) {
        if (errorHandler_) {
            errorHandler_->handleError(error);
        } else {
            // Default behavior: throw the exception
            throw error;
        }
    }
    
    void setThrowOnError(bool throwOnError) {
        throwOnError_ = throwOnError;
    }
    
    bool getThrowOnError() const { return throwOnError_; }
    
private:
    ErrorHandlerManager() : throwOnError_(true) {
        errorHandler_ = std::make_unique<DefaultErrorHandler>();
    }
    
    std::unique_ptr<IErrorHandler> errorHandler_;
    bool throwOnError_;
};

// Convenience functions for error handling
inline void throwError(const std::string& message, ErrorCode code = ErrorCode::SystemError,
                       ErrorCategory category = ErrorCategory::None,
                       const std::string& details = "") {
    ErrorHandlerManager::getInstance().handleError(
        Error(message, code, category, details)
    );
}

inline void throwIf(bool condition, const std::string& message, ErrorCode code = ErrorCode::InvalidParameter,
                    ErrorCategory category = ErrorCategory::None,
                    const std::string& details = "") {
    if (condition) {
        throwError(message, code, category, details);
    }
}

inline void throwIfNull(void* ptr, const std::string& message = "Pointer is null") {
    if (ptr == nullptr) {
        throwError(message, ErrorCode::MemoryAccessViolation, ErrorCategory::Memory);
    }
}

// Error recovery utilities
class ErrorRecovery {
public:
    template<typename T>
    static T safeExecute(const std::function<T()>& func, const T& defaultValue,
                         const std::string& errorContext = "Operation") {
        try {
            return func();
        } catch (const Error& e) {
            // Log the error
            // For now, just return default value - in production you'd want proper logging
            return defaultValue;
        } catch (const std::exception& e) {
            // Log unexpected exception
            return defaultValue;
        }
    }
    
    template<typename T>
    static std::optional<T> safeExecuteOptional(const std::function<T()>& func,
                                               const std::string& errorContext = "Operation") {
        try {
            return func();
        } catch (...) {
            return std::nullopt;
        }
    }
    
    // Reset system state after error
    static void resetSystemState() {
        // Implement system state reset logic here
        // This could reset neural connections, clear memory, etc.
    }
};

// Error reporting utilities
class ErrorReporter {
public:
    static void reportError(const Error& error) {
        // Log error to error log file
        // Send error to monitoring system
        // Update error statistics
        // For now, just output to stderr
        std::cerr << "ERROR: " << error.what() << std::endl;
    }
    
    static void reportWarning(const std::string& message) {
        std::cerr << "WARNING: " << message << std::endl;
    }
    
    static void reportInfo(const std::string& message) {
        std::cout << "INFO: " << message << std::endl;
    }
};

} // namespace nlm
