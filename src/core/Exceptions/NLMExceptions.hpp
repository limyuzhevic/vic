// Core exception hierarchy for NLM
// Provides centralized error handling with specific exception types for different error categories

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <memory>
#include <chrono>
#include <sstream>

namespace nlm {

// Error codes for different error categories
enum class ErrorCode {
    // Configuration errors (1000-1999)
    CONFIG_FILE_NOT_FOUND = 1000,
    CONFIG_PARSE_ERROR = 1001,
    CONFIG_VALIDATION_FAILED = 1002,
    CONFIG_PARAMETER_INVALID = 1003,
    CONFIG_CROSS_PARAMETER_VIOLATION = 1004,
    
    // Simulation errors (2000-2999)
    SIMULATION_STEP_FAILED = 2000,
    SIMULATION_STATE_INVALID = 2001,
    SIMULATION_MEMORY_ERROR = 2002,
    SIMULATION_TIMEOUT = 2003,
    
    // Neural component errors (3000-3999)
    NEURON_VALIDATION_FAILED = 3000,
    NEURON_INITIALIZATION_FAILED = 3001,
    NEURON_STATE_INVALID = 3002,
    
    SYNAPSE_VALIDATION_FAILED = 3100,
    SYNAPSE_INITIALIZATION_FAILED = 3101,
    SYNAPSE_WEIGHT_BOUNDS_VIOLATION = 3102,
    
    // Plasticity errors (4000-4999)
    PLASTICITY_RULE_FAILED = 4000,
    STDP_COMPUTATION_FAILED = 4001,
    HEBBIAN_COMPUTATION_FAILED = 4002,
    
    // Neuromodulation errors (5000-5999)
    NEUROMODULATOR_INVALID_STATE = 5000,
    REWARD_SIGNAL_INVALID = 5001,
    NOVELTY_DETECTION_FAILED = 5002,
    
    // Development errors (6000-6999)
    DEVELOPMENT_SYSTEM_FAILED = 6000,
    MATURATION_FAILED = 6001,
    SYNAPTogenesis_FAILED = 6002,
    PRUNING_FAILED = 6003,
    
    // Memory errors (7000-7999)
    MEMORY_SYSTEM_FAILED = 7000,
    CONSISTENCY_CHECK_FAILED = 7001,
    CAPACITY_EXCEEDED = 7002,
    
    // General errors (8000-8999)
    UNKNOWN_ERROR = 8000,
    OUT_OF_BOUNDS = 8001,
    INVALID_PARAMETER = 8002,
    NOT_IMPLEMENTED = 8003,
    RESOURCE_UNAVAILABLE = 8004
};

// Base exception class with context and stack trace management
class NLMException : public std::runtime_error {
protected:
    ErrorCode errorCode;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
    
public:
    explicit NLMException(ErrorCode code, const std::string& message)
        : std::runtime_error(message), errorCode(code), timestamp(std::chrono::system_clock::now()) {
        stackTrace.push_back(getCurrentStackFrame());
    }
    
    NLMException(ErrorCode code, const std::string& message, const std::string& context)
        : std::runtime_error(message), errorCode(code), context(context), timestamp(std::chrono::system_clock::now()) {
        stackTrace.push_back(getCurrentStackFrame());
    }
    
    virtual ~NLMException() = default;
    
    // Getters
    ErrorCode getErrorCode() const { return errorCode; }
    const std::string& getContext() const { return context; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    const std::vector<std::string>& getStackTrace() const { return stackTrace; }
    
    // Setters
    void setContext(const std::string& ctx) { context = ctx; }
    void addMetadata(const std::string& key, const std::string& value) { metadata[key] = value; }
    void addStackFrame(const std::string& frame) { stackTrace.push_back(frame); }
    
    // Helper to get formatted exception information
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[Error " << static_cast<int>(errorCode) << "] " << what();
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        if (!metadata.empty()) {
            oss << " (Metadata: ";
            bool first = true;
            for (const auto& pair : metadata) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << ")";
        }
        return oss.str();
    }
    
protected:
    std::string getCurrentStackFrame() {
        return "[stack frame]";
    }
};

// Configuration-specific exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Configuration error") {}
    
    ConfigException(ErrorCode code, const std::string& message, const std::string& param)
        : NLMException(code, message, "Configuration error: " + param) {
        addMetadata("parameter", param);
    }
};

class ConfigFileException : public ConfigException {
public:
    explicit ConfigFileException(const std::string& message, const std::string& filepath)
        : ConfigException(ErrorCode::CONFIG_FILE_NOT_FOUND, message, filepath) {
        addMetadata("file", filepath);
    }
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message, const std::string& format)
        : ConfigException(ErrorCode::CONFIG_PARSE_ERROR, message, "Format: " + format) {
        addMetadata("format", format);
    }
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message, const std::map<std::string, std::string>& validationErrors)
        : ConfigException(ErrorCode::CONFIG_VALIDATION_FAILED, message, "Validation failed") {
        for (const auto& pair : validationErrors) {
            addMetadata("error_" + pair.first, pair.second);
        }
    }
};

// Simulation-specific exceptions
class SimulationException : public NLMException {
public:
    explicit SimulationException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Simulation error") {}
};

class SimulationStepException : public SimulationException {
public:
    SimulationStepException(const std::string& message, SimulationStep step)
        : SimulationException(ErrorCode::SIMULATION_STEP_FAILED, message) {
        addMetadata("step", std::to_string(static_cast<long long>(step)));
    }
};

class SimulationStateException : public SimulationException {
public:
    SimulationStateException(const std::string& message, const std::string& stateInfo)
        : SimulationException(ErrorCode::SIMULATION_STATE_INVALID, message) {
        addMetadata("state", stateInfo);
    }
};

// Neural component exceptions
class NeuronException : public NLMException {
public:
    explicit NeuronException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Neuron error") {}
    
    NeuronException(ErrorCode code, const std::string& message, NeuronId id)
        : NLMException(code, message, "Neuron error: ID " + std::to_string(static_cast<long long>(id))) {
        addMetadata("neuron_id", std::to_string(static_cast<long long>(id)));
    }
};

class SynapseException : public NLMException {
public:
    explicit SynapseException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Synapse error") {}
    
    SynapseException(ErrorCode code, const std::string& message, SynapseId id)
        : NLMException(code, message, "Synapse error: ID " + std::to_string(static_cast<long long>(id))) {
        addMetadata("synapse_id", std::to_string(static_cast<long long>(id)));
    }
};

// Plasticity exceptions
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Plasticity error") {}
};

// Neuromodulation exceptions
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Neuromodulation error") {}
};

// Development exceptions
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Development error") {}
};

// Memory system exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(ErrorCode code, const std::string& message)
        : NLMException(code, message, "Memory error") {}
};

// Error reporting utilities
class ErrorReporter {
public:
    struct ErrorReport {
        ErrorCode code;
        std::string message;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
        std::vector<std::string> stackTrace;
    };
    
    static ErrorReport createErrorReport(const std::exception& ex) {
        ErrorReport report;
        
        if (const NLMException* nlmEx = dynamic_cast<const NLMException*>(&ex)) {
            report.code = nlmEx->getErrorCode();
            report.message = nlmEx->what();
            report.context = nlmEx->getContext();
            report.timestamp = nlmEx->getTimestamp();
            report.metadata = nlmEx->getMetadata();
            report.stackTrace = nlmEx->getStackTrace();
        } else {
            report.code = ErrorCode::UNKNOWN_ERROR;
            report.message = ex.what();
            report.context = "Generic exception";
            report.timestamp = std::chrono::system_clock::now();
        }
        
        return report;
    }
    
    static std::string formatErrorReport(const ErrorReport& report) {
        std::ostringstream oss;
        oss << "=== Error Report ===" << std::endl;
        oss << "Code: " << static_cast<int>(report.code) << std::endl;
        oss << "Message: " << report.message << std::endl;
        oss << "Context: " << report.context << std::endl;
        oss << "Timestamp: " << formatTimestamp(report.timestamp) << std::endl;
        
        if (!report.metadata.empty()) {
            oss << "Metadata:" << std::endl;
            for (const auto& pair : report.metadata) {
                oss << "  " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        if (!report.stackTrace.empty()) {
            oss << "Stack Trace:" << std::endl;
            for (const auto& frame : report.stackTrace) {
                oss << "  " << frame << std::endl;
            }
        }
        
        return oss.str();
    }
    
private:
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S.%f");
        return oss.str();
    }
};

// Error recovery mechanisms
class ErrorRecovery {
public:
    enum class RecoveryAction {
        IGNORE,
        RETRY,
        ROLLBACK,
        FALLBACK,
        SHUTDOWN
    };
    
    struct RecoveryStrategy {
        ErrorCode errorCode;
        RecoveryAction action;
        int maxRetries;
        std::chrono::milliseconds retryDelay;
        std::function<bool(const std::exception&)> shouldRetry;
    };
    
    static bool attemptRecovery(const std::exception& error, 
                               const std::vector<RecoveryStrategy>& strategies) {
        for (const auto& strategy : strategies) {
            if (matchesStrategy(error, strategy)) {
                switch (strategy.action) {
                    case RecoveryAction::IGNORE:
                        return true; // Recovery successful by ignoring
                    case RecoveryAction::RETRY:
                        return retryOperation(strategy, error);
                    case RecoveryAction::ROLLBACK:\n                        return rollbackOperation(error);
                    case RecoveryAction::FALLBACK:
                        return fallbackOperation(error);
                    case RecoveryAction::SHUTDOWN:
                        return shutdownOperation(error);
                }
            }
        }
        return false; // No matching recovery strategy found
    }
    
private:
    static bool matchesStrategy(const std::exception& error, const RecoveryStrategy& strategy) {
        if (const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error)) {
            return nlmEx->getErrorCode() == strategy.errorCode;
        }
        return false;
    }
    
    static bool retryOperation(const RecoveryStrategy& strategy, const std::exception& error) {
        // Implementation for retry logic
        return false;
    }
    
    static bool rollbackOperation(const std::exception& error) {
        // Implementation for rollback logic
        return false;
    }
    
    static bool fallbackOperation(const std::exception& error) {
        // Implementation for fallback logic
        return false;
    }
    
    static bool shutdownOperation(const std::exception& error) {
        // Implementation for shutdown logic
        return false;
    }
};

} // namespace nlm