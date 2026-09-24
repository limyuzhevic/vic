// Copyright 2026 NLM Authors
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <functional>
#include <thread>
#include <cmath>

namespace nlm {

// Forward declarations
class GlobalErrorHandler;

// Error Categories for classification
enum class ErrorCategory {
    // Core system errors
    None = 0,
    System,
    Memory,
    Network,
    Plasticity,
    Visualization,
    Configuration,
    Persistence,
    Performance,
    Resource,
    Integration,
    Validation,
    Development,
    User,
    CustomStart = 100
};

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,      // For debugging information
    Info = 1,        // Informational messages
    Warning = 2,     // Warnings that don't prevent operation
    Error = 3,       // Errors that prevent operation but allow recovery
    Critical = 4,    // Critical errors that prevent continued operation
    Fatal = 5        // Unrecoverable fatal errors
};

// Error codes for specific error conditions
enum class ErrorCode {
    // Generic codes
    Success = 0,
    UnknownError,
    NotImplemented,
    InvalidArgument,
    OutOfRange,
    AccessDenied,
    NotFound,
    Timeout,
    ResourceExhausted,
    ResourceUnavailable,
    
    // Memory-related codes
    MemoryAllocationFailed,
    MemoryDeallocationFailed,
    MemoryLeakDetected,
    MemoryCorruption,
    
    // Network-related codes
    ConnectionFailed,
    ConnectionLost,
    PacketLoss,
    ProtocolError,
    NeuronStateInvalid,
    SynapseFormationFailed,
    SpikeDeliveryFailed,
    
    // Plasticity-related codes
    PlasticityRuleInvalid,
    STDPUpdateFailed,
    StructuralPlasticityFailed,
    WeightBoundsViolation,
    
    // Visualization-related codes
    VisualizationNotInitialized,
    RenderFailed,
    DisplayModeUnsupported,
    
    // Configuration-related codes
    ConfigFileNotFound,
    ConfigParseError,
    ConfigValidationFailed,
    RequiredFieldMissing,
    
    // Persistence-related codes
    SaveFailed,
    LoadFailed,
    CheckpointFailed,
    DatabaseError,
    
    // Performance-related codes
    PerformanceDegraded,
    TimeoutExpired,
    BackpressureApplied,
    CPUExhausted,
    MemoryPressure,
    
    // Integration-related codes
    ExternalModuleError,
    NetworkCommunicationError,
    FileIOError,
    APIIntegrationError,
    
    // Validation-related codes
    ValidationFailed,
    InputSyntaxError,
    DataCorruption,
    IncompatibleFormat,
    
    // Development-related codes
    InitializationFailed,
    SimulationStepFailed,
    StateInconsistency,
    RecoveryFailed,
    
    // Custom error codes
    CustomStart = 1000
};

// Error context information for detailed debugging
struct ErrorContext {
    std::string file;
    std::string function;
    int line = 0;
    std::string threadId;
    std::string threadName;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
    
    ErrorContext() : line(0), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorContext(const std::string& file, const std::string& function, int line)
        : file(file), function(function), line(line), timestamp(std::chrono::system_clock::now()) {}
    
    void addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "File: " << file << "\n";
        ss << "Function: " << function << "\n";
        ss << "Line: " << line << "\n";
        ss << "Thread: " << threadName << " (" << threadId << ")\n";
        time_t time = std::chrono::system_clock::to_time_t(timestamp);
        ss << "Timestamp: " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f") << "\n";
        if (!metadata.empty()) {
            ss << "Metadata:\n";
            for (const auto& pair : metadata) {
                ss << "  " << pair.first << ": " << pair.second << "\n";
            }
        }
        return ss.str();
    }
};

// Base exception class with full error information
class NLMException : public std::exception {
protected:
    ErrorCategory category_;
    ErrorCode code_;
    ErrorSeverity severity_;
    std::string message_;
    std::string formattedMessage_;
    ErrorContext context_;
    
public:
    NLMException(ErrorCategory category, ErrorCode code, ErrorSeverity severity, 
                 const std::string& message)
        : category_(category), code_(code), severity_(severity), message_(message) {
        formattedMessage_ = formatMessage();
    }
    
    NLMException(ErrorCategory category, ErrorCode code, ErrorSeverity severity,
                 const std::string& message, const ErrorContext& context)
        : category_(category), code_(code), severity_(severity), message_(message), context_(context) {
        formattedMessage_ = formatMessage();
    }
    
    // Virtual destructor for proper cleanup
    virtual ~NLMException() noexcept = default;
    
    // Override what() method to provide formatted error message
    virtual const char* what() const noexcept override {
        return formattedMessage_.c_str();
    }
    
    // Getters for error properties
    ErrorCategory getCategory() const { return category_; }
    ErrorCode getCode() const { return code_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getMessage() const { return message_; }
    const ErrorContext& getContext() const { return context_; }
    
    // Check if this is a recoverable error
    bool isRecoverable() const {
        return severity_ <= ErrorSeverity::Error;
    }
    
    // Check if this is a fatal error
    bool isFatal() const {
        return severity_ >= ErrorSeverity::Critical;
    }
    
protected:
    virtual std::string formatMessage() const {
        std::stringstream ss;
        ss << "[" << errorCodeToString(code_) << "] " << message_;
        if (!context_.file.empty()) {
            ss << " (" << context_.file << ":" << context_.line << ")";
        }
        return ss.str();
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::unordered_map<ErrorCategory, std::string> categoryMap = {
            {ErrorCategory::None, "None"},
            {ErrorCategory::System, "System"},
            {ErrorCategory::Memory, "Memory"},
            {ErrorCategory::Network, "Network"},
            {ErrorCategory::Plasticity, "Plasticity"},
            {ErrorCategory::Visualization, "Visualization"},
            {ErrorCategory::Configuration, "Configuration"},
            {ErrorCategory::Persistence, "Persistence"},
            {ErrorCategory::Performance, "Performance"},
            {ErrorCategory::Resource, "Resource"},
            {ErrorCategory::Integration, "Integration"},
            {ErrorCategory::Validation, "Validation"},
            {ErrorCategory::Development, "Development"},
            {ErrorCategory::User, "User"}
        };
        auto it = categoryMap.find(category);
        return it != categoryMap.end() ? it->second : "Unknown";
    }
    
    static std::string errorCodeToString(ErrorCode code) {
        static const std::unordered_map<ErrorCode, std::string> codeMap = {
            {ErrorCode::Success, "SUCCESS"},
            {ErrorCode::UnknownError, "UNKNOWN_ERROR"},
            {ErrorCode::NotImplemented, "NOT_IMPLEMENTED"},
            {ErrorCode::InvalidArgument, "INVALID_ARGUMENT"},
            {ErrorCode::OutOfRange, "OUT_OF_RANGE"},
            {ErrorCode::AccessDenied, "ACCESS_DENIED"},
            {ErrorCode::NotFound, "NOT_FOUND"},
            {ErrorCode::Timeout, "TIMEOUT"},
            {ErrorCode::ResourceExhausted, "RESOURCE_EXHAUSTED"},
            {ErrorCode::ResourceUnavailable, "RESOURCE_UNAVAILABLE"},
            {ErrorCode::MemoryAllocationFailed, "MEMORY_ALLOCATION_FAILED"},
            {ErrorCode::MemoryDeallocationFailed, "MEMORY_DEALLOCATION_FAILED"},
            {ErrorCode::MemoryLeakDetected, "MEMORY_LEAK_DETECTED"},
            {ErrorCode::MemoryCorruption, "MEMORY_CORRUPTION"},
            {ErrorCode::ConnectionFailed, "CONNECTION_FAILED"},
            {ErrorCode::ConnectionLost, "CONNECTION_LOST"},
            {ErrorCode::PacketLoss, "PACKET_LOSS"},
            {ErrorCode::ProtocolError, "PROTOCOL_ERROR"},
            {ErrorCode::NeuronStateInvalid, "NEURON_STATE_INVALID"},
            {ErrorCode::SynapseFormationFailed, "SYNAPSE_FORMATION_FAILED"},
            {ErrorCode::SpikeDeliveryFailed, "SPIKE_DELIVERY_FAILED"},
            {ErrorCode::PlasticityRuleInvalid, "PLASTICITY_RULE_INVALID"},
            {ErrorCode::STDPUpdateFailed, "STDP_UPDATE_FAILED"},
            {ErrorCode::StructuralPlasticityFailed, "STRUCTURAL_PLASTICITY_FAILED"},
            {ErrorCode::WeightBoundsViolation, "WEIGHT_BOUNDS_VIOLATION"},
            {ErrorCode::VisualizationNotInitialized, "VISUALIZATION_NOT_INITIALIZED"},
            {ErrorCode::RenderFailed, "RENDER_FAILED"},
            {ErrorCode::DisplayModeUnsupported, "DISPLAY_MODE_UNSUPPORTED"},
            {ErrorCode::ConfigFileNotFound, "CONFIG_FILE_NOT_FOUND"},
            {ErrorCode::ConfigParseError, "CONFIG_PARSE_ERROR"},
            {ErrorCode::ConfigValidationFailed, "CONFIG_VALIDATION_FAILED"},
            {ErrorCode::RequiredFieldMissing, "REQUIRED_FIELD_MISSING"},
            {ErrorCode::SaveFailed, "SAVE_FAILED"},
            {ErrorCode::LoadFailed, "LOAD_FAILED"},
            {ErrorCode::CheckpointFailed, "CHECKPOINT_FAILED"},
            {ErrorCode::DatabaseError, "DATABASE_ERROR"},
            {ErrorCode::PerformanceDegraded, "PERFORMANCE_DEGRADED"},
            {ErrorCode::TimeoutExpired, "TIMEOUT_EXPIRED"},
            {ErrorCode::BackpressureApplied, "BACKPRESSURE_APPLIED"},
            {ErrorCode::CPUExhausted, "CPU_EXHAUSTED"},
            {ErrorCode::MemoryPressure, "MEMORY_PRESSURE"},
            {ErrorCode::ExternalModuleError, "EXTERNAL_MODULE_ERROR"},
            {ErrorCode::NetworkCommunicationError, "NETWORK_COMMUNICATION_ERROR"},
            {ErrorCode::FileIOError, "FILE_IO_ERROR"},
            {ErrorCode::APIIntegrationError, "API_INTEGRATION_ERROR"},
            {ErrorCode::ValidationFailed, "VALIDATION_FAILED"},
            {ErrorCode::InputSyntaxError, "INPUT_SYNTAX_ERROR"},
            {ErrorCode::DataCorruption, "DATA_CORRUPTION"},
            {ErrorCode::IncompatibleFormat, "INCOMPATIBLE_FORMAT"},
            {ErrorCode::InitializationFailed, "INITIALIZATION_FAILED"},
            {ErrorCode::SimulationStepFailed, "SIMULATION_STEP_FAILED"},
            {ErrorCode::StateInconsistency, "STATE_INCONSISTENCY"},
            {ErrorCode::RecoveryFailed, "RECOVERY_FAILED"}
        };
        auto it = codeMap.find(code);
        return it != codeMap.end() ? it->second : "UNKNOWN_ERROR_CODE";
    }
    
    static std::string severityToString(ErrorSeverity severity) {
        static const std::unordered_map<ErrorSeverity, std::string> severityMap = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARNING"},
            {ErrorSeverity::Error, "ERROR"},
            {ErrorSeverity::Critical, "CRITICAL"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        auto it = severityMap.find(severity);
        return it != severityMap.end() ? it->second : "UNKNOWN";
    }
};

// Specific error classes for different domains

// Memory management errors
class MemoryAllocationError : public NLMException {
public:
    MemoryAllocationError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Memory, code, ErrorSeverity::Critical, message, context) {}
};

class MemoryDeallocationError : public NLMException {
public:
    MemoryDeallocationError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Memory, code, ErrorSeverity::Critical, message, context) {}
};

// Network simulation errors
class NetworkError : public NLMException {
public:
    NetworkError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Network, code, ErrorSeverity::Error, message, context) {}
};

class NeuronError : public NLMException {
public:
    NeuronError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Network, code, ErrorSeverity::Error, message, context) {}
};

class SynapseError : public NLMException {
public:
    SynapseError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Network, code, ErrorSeverity::Error, message, context) {}
};

// Plasticity errors
class PlasticityError : public NLMException {
public:
    PlasticityError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Plasticity, code, ErrorSeverity::Warning, message, context) {}
};

// Configuration errors
class ConfigError : public NLMException {
public:
    ConfigError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Configuration, code, ErrorSeverity::Error, message, context) {}
};

// Persistence errors
class PersistenceError : public NLMException {
public:
    PersistenceError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Persistence, code, ErrorSeverity::Error, message, context) {}
};

class SaveError : public PersistenceError {
public:
    SaveError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : PersistenceError(code, message, context) {}
};

class LoadError : public PersistenceError {
public:
    LoadError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : PersistenceError(code, message, context) {}
};

// Performance and resource errors
class PerformanceError : public NLMException {
public:
    PerformanceError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Performance, code, ErrorSeverity::Warning, message, context) {}
};

class ResourceError : public NLMException {
public:
    ResourceError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Resource, code, ErrorSeverity::Critical, message, context) {}
};

// Visualization errors
class VisualizationError : public NLMException {
public:
    VisualizationError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Visualization, code, ErrorSeverity::Warning, message, context) {}
};

// Integration errors
class IntegrationError : public NLMException {
public:
    IntegrationError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Integration, code, ErrorSeverity::Error, message, context) {}
};

// Development and testing errors
class DevelopmentError : public NLMException {
public:
    DevelopmentError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Development, code, ErrorSeverity::Debug, message, context) {}
};

class ValidationError : public NLMException {
public:
    ValidationError(ErrorCode code, const std::string& message, const ErrorContext& context = ErrorContext())
        : NLMException(ErrorCategory::Validation, code, ErrorSeverity::Error, message, context) {}
};

#define NLM_CURRENT_ERROR_CONTEXT nlm::ErrorContext(__FILE__, __func__, __LINE__)

// Macro for throwing error with current context
#define NLM_THROW_CURRENT_ERROR(code, message, category) \
    throw nlm::NLMException(category, code, nlm::ErrorSeverity::Error, message, NLM_CURRENT_ERROR_CONTEXT)

// Macro for error logging
#define NLM_ERROR_LOG(message, category, code) \
    nlm::ErrorHandling::handleException(nlm::NLMException(category, code, nlm::ErrorSeverity::Info, message, NLM_CURRENT_ERROR_CONTEXT))

} // namespace nlm

} // namespace nlm
