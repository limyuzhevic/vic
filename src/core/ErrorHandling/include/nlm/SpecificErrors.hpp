// Specific error types for different domains
#pragma once

#include "NLMError.hpp"

namespace nlm {

// Configuration-related errors
class ConfigurationError : public NLMError {
public:
    enum class ConfigErrorType {
        InvalidFormat,
        MissingKey,
        TypeMismatch,
        ValidationFailed,
        FileIOError,
        EnvironmentVariableNotFound,
        OutOfRange,
        CircularReference,
        SchemaMismatch,
        DeprecatedKey,
        ReservedKey
    };

    ConfigurationError(const std::string& message, 
                      ConfigErrorType type = ConfigErrorType::InvalidFormat,
                      const std::string& field = "",
                      const std::string& suggestion = "")
        : NLMError("Configuration Error: " + message, ErrorSeverity::Error, ErrorCategory::Configuration),
          configErrorType_(type),
          field_(field),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    ConfigErrorType getConfigErrorType() const { return configErrorType_; }
    const std::string& getField() const { return field_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("retry_config_load", "true");
        addRecoveryAction("use_defaults", "true");
        addRecoveryAction("check_environment", "true");
        if (!field_.empty()) {
            addRecoveryAction("validate_field", field_);
        }
    }

private:
    ConfigErrorType configErrorType_;
    std::string field_;
    std::string suggestion_;
};

// Validation-related errors
class ValidationError : public NLMError {
public:
    enum class ValidationErrorType {
        TypeMismatch,
        ConstraintViolation,
        FormatError,
        RangeError,
        PatternMismatch,
        SchemaViolation,
        DependencyError,
        ConditionalValidationFailed,
        CrossFieldValidationFailed,
        BusinessLogicViolation,
        SecurityPolicyViolation
    };

    ValidationError(const std::string& message,
                   ValidationErrorType type = ValidationErrorType::TypeMismatch,
                   const std::string& field = "",
                   const std::string& suggestion = "")
        : NLMError("Validation Error: " + message, ErrorSeverity::Warning, ErrorCategory::Validation),
          validationErrorType_(type),
          field_(field),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    ValidationErrorType getValidationErrorType() const { return validationErrorType_; }
    const std::string& getField() const { return field_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("skip_validation", "true");
        addRecoveryAction("use_default_value", "true");
        addRecoveryAction("apply_fixup_rules", "true");
        if (!field_.empty()) {
            addRecoveryAction("validate_field_rules", field_);
        }
    }

private:
    ValidationErrorType validationErrorType_;
    std::string field_;
    std::string suggestion_;
};

// Serialization-related errors
class SerializationError : public NLMError {
public:
    enum class SerializationErrorType {
        Unknown,
        VersionMismatch,
        CorruptedData,
        MissingRequiredData,
        InvalidFormat,
        CompressionError,
        EncryptionError,
        Timeout,
        MemoryUsageExceeded,
        SerializationFailed,
        DeserializationFailed,
        SchemaMismatch,
        IntegrityCheckFailed
    };

    SerializationError(const std::string& message,
                      SerializationErrorType type = SerializationErrorType::Unknown,
                      const std::string& component = "",
                      const std::string& suggestion = "")
        : NLMError("Serialization Error: " + message, ErrorSeverity::Error, ErrorCategory::Serialization),
          serializationErrorType_(type),
          component_(component),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    SerializationErrorType getSerializationErrorType() const { return serializationErrorType_; }
    const std::string& getComponent() const { return component_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("retry_serialization", "true");
        addRecoveryAction("use_alternative_format", "true");
        addRecoveryAction("reduce_data_size", "true");
        addRecoveryAction("increase_memory_allocation", "true");
        if (!component_.empty()) {
            addRecoveryAction("check_component_integrity", component_);
        }
    }

private:
    SerializationErrorType serializationErrorType_;
    std::string component_;
    std::string suggestion_;
};

// Neural network computation errors
class NeuralError : public NLMError {
public:
    enum class NeuralErrorType {
        NeuronInitializationFailed,
        SynapseCreationFailed,
        NetworkTopologyError,
        SpikePropagationFailed,
        PlasticityRuleFailed,
        MemoryAllocationFailed,
        ConvergenceFailed,
        TrainingFailed,
        InferenceFailed,
        WeightUpdateFailed,
        ActivationFunctionError
    };

    NeuralError(const std::string& message,
                NeuralErrorType type = NeuralErrorType::NeuronInitializationFailed,
                const std::string& operation = "",
                const std::string& suggestion = "")
        : NLMError("Neural Error: " + message, ErrorSeverity::Critical, ErrorCategory::Neural),
          neuralErrorType_(type),
          operation_(operation),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    NeuralErrorType getNeuralErrorType() const { return neuralErrorType_; }
    const std::string& getOperation() const { return operation_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("retry_operation", "true");
        addRecoveryAction("use_simplified_model", "true");
        addRecoveryAction("reduce_network_size", "true");
        addRecoveryAction("reset_weights", "true");
        if (!operation_.empty()) {
            addRecoveryAction("check_operation", operation_);
        }
    }

private:
    NeuralErrorType neuralErrorType_;
    std::string operation_;
    std::string suggestion_;
};

// Memory management errors
class MemoryError : public NLMError {
public:
    enum class MemoryErrorType {
        AllocationFailed,
        DeallocationFailed,
        BufferOverflow,
        MemoryLeakDetected,
        OutOfMemory,
        AccessViolation,
        DoubleFree,
        UseAfterFree,
        MemoryCorruption,
        PoolExhaustion,
        MemoryFragmentation
    };

    MemoryError(const std::string& message,
                MemoryErrorType type = MemoryErrorType::AllocationFailed,
                size_t address = 0,
                const std::string& suggestion = "")
        : NLMError("Memory Error: " + message, ErrorSeverity::Critical, ErrorCategory::Memory),
          memoryErrorType_(type),
          address_(address),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    MemoryErrorType getMemoryErrorType() const { return memoryErrorType_; }
    size_t getAddress() const { return address_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("garbage_collect", "true");
        addRecoveryAction("increase_memory_pool", "true");
        addRecoveryAction("use_memory_optimization", "true");
        addRecoveryAction("reduce_active_processes", "true");
        addRecoveryAction("enable_swap_space", "true");
    }

private:
    MemoryErrorType memoryErrorType_;
    size_t address_;
    std::string suggestion_;
};

// System-level errors
class SystemError : public NLMError {
public:
    enum class SystemErrorType {
        FileSystemError,
        NetworkError,
        ProcessError,
        ThreadError,
        SynchronizationError,
        ResourceUnavailable,
        PermissionDenied,
        TimeoutError,
        DeadlockDetected,
        SignalError,
        InterruptError
    };

    SystemError(const std::string& message,
               SystemErrorType type = SystemErrorType::FileSystemError,
               const std::string& subsystem = "",
               const std::string& suggestion = "")
        : NLMError("System Error: " + message, ErrorSeverity::Critical, ErrorCategory::System),
          systemErrorType_(type),
          subsystem_(subsystem),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    SystemErrorType getSystemErrorType() const { return systemErrorType_; }
    const std::string& getSubsystem() const { return subsystem_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("retry_operation", "true");
        addRecoveryAction("use_alternative_resource", "true");
        addRecoveryAction("reduce_concurrency", "true");
        addRecoveryAction("check_system_resources", "true");
        if (!subsystem_.empty()) {
            addRecoveryAction("check_subsystem_health", subsystem_);
        }
    }

private:
    SystemErrorType systemErrorType_;
    std::string subsystem_;
    std::string suggestion_;
};

// Performance-related errors
class PerformanceError : public NLMError {
public:
    enum class PerformanceErrorType {
        MemoryUsageExceeded,
        CPUUsageExceeded,
        TimeLimitExceeded,
        ThroughputBelowMinimum,
        LatencyExceeded,
        ContextSwitchRateHigh,
        CacheMissRateHigh,
        ResourceContention,
        SlowdownDetected,
        DegradationDetected
    };

    PerformanceError(const std::string& message,
                    PerformanceErrorType type = PerformanceErrorType::MemoryUsageExceeded,
                    const std::string& metric = "",
                    const std::string& suggestion = "")
        : NLMError("Performance Error: " + message, ErrorSeverity::Warning, ErrorCategory::Performance),
          performanceErrorType_(type),
          metric_(metric),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    PerformanceErrorType getPerformanceErrorType() const { return performanceErrorType_; }
    const std::string& getMetric() const { return metric_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("reduce_workload", "true");
        addRecoveryAction("optimize_algorithm", "true");
        addRecoveryAction("increase_resources", "true");
        addRecoveryAction("enable_caching", "true");
        addRecoveryAction("reduce_concurrency", "true");
    }

private:
    PerformanceErrorType performanceErrorType_;
    std::string metric_;
    std::string suggestion_;
};

// Network-related errors
class NetworkError : public NLMError {
public:
    enum class NetworkErrorType {
        ConnectionFailed,
        Timeout,
        PacketLoss,
        BandwidthExceeded,
        ProtocolError,
        DNSResolutionFailed,
        AuthenticationFailed,
        RateLimitExceeded,
        ConnectionDropped,
        MalformedPacket
    };

    NetworkError(const std::string& message,
                NetworkErrorType type = NetworkErrorType::ConnectionFailed,
                const std::string& endpoint = "",
                const std::string& suggestion = "")
        : NLMError("Network Error: " + message, ErrorSeverity::Error, ErrorCategory::Network),
          networkErrorType_(type),
          endpoint_(endpoint),
          suggestion_(suggestion) {
        setRecoveryActions();
    }

    NetworkErrorType getNetworkErrorType() const { return networkErrorType_; }
    const std::string& getEndpoint() const { return endpoint_; }
    const std::string& getSuggestion() const { return suggestion_; }
    std::string getRecoveryHint() const;

protected:
    void setRecoveryActions() {
        addRecoveryAction("retry_connection", "true");
        addRecoveryAction("use_alternative_endpoint", "true");
        addRecoveryAction("implement_circuit_breaker", "true");
        addRecoveryAction("enable_connection_pool", "true");
        if (!endpoint_.empty()) {
            addRecoveryAction("check_endpoint_health", endpoint_);
        }
    }

private:
    NetworkErrorType networkErrorType_;
    std::string endpoint_;
    std::string suggestion_;
};

// Type aliases for convenience
using ConfigurationErrorPtr = std::unique_ptr<ConfigurationError>;
using ValidationErrorPtr = std::unique_ptr<ValidationError>;
using SerializationErrorPtr = std::unique_ptr<SerializationError>;
using NeuralErrorPtr = std::unique_ptr<NeuralError>;
using MemoryErrorPtr = std::unique_ptr<MemoryError>;
using SystemErrorPtr = std::unique_ptr<SystemError>;
using PerformanceErrorPtr = std::unique_ptr<PerformanceError>;
using NetworkErrorPtr = std::unique_ptr<NetworkError>;

} // namespace nlm