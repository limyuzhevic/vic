#pragma once

#include "exceptions/BrainException.hpp"
#include "Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include <stdexcept>

namespace nlm {

/**
 * Validation utilities for the NLM brain system
 */
class Validator {
public:
    // Configuration parameter validation
    static void validateConfigParameter(const std::string& key, 
                                       const std::string& value,
                                       const std::string& description = "");
    
    static void validateNeuronParameters(const NeuronState& state);
    static void validateSynapseParameters(const Synapse& synapse);
    static void validateRegionParameters(const NeuralRegion& region);
    
    static void validateDevelopmentalStage(DevelopmentalStage from,
                                           DevelopmentalStage to);
    
    static void validateMemoryCapacity(size_t capacity, size_t currentUsage);
    static void validateNeuromodulatorLevels(const Neuromodulator& signal);
    
    static void validatePlasticityCompatibility(const PlasticityFlags& flags1,
                                               const PlasticityFlags& flags2);
    
    // Range validation
    template<typename T>
    static void validateRange(T value, T min, T max, 
                             const std::string& paramName = "parameter") {
        if (value < min || value > max) {
            std::map<std::string, std::string> context;
            context["value"] = std::to_string(value);
            context["min"] = std::to_string(min);
            context["max"] = std::to_string(max);
            context["parameter"] = paramName;
            throw BrainRuntimeException(
                "Parameter out of valid range",
                context,
                "Validator"
            );
        }
    }
    
    // Positive value validation
    template<typename T>
    static void validatePositive(T value, const std::string& paramName = "parameter") {
        if (value < 0) {
            std::map<std::string, std::string> context;
            context["value"] = std::to_string(value);
            context["parameter"] = paramName;
            throw BrainRuntimeException(
                "Parameter must be non-negative",
                context,
                "Validator"
            );
        }
    }
    
    // Memory validation
    static void validateMemoryAllocation(size_t size, const std::string& purpose = "");
    static void validateMemoryAccess(const void* ptr, size_t size);
    
    // State validation
    static void validateSystemState(bool condition, const std::string& errorMessage,
                                   const std::map<std::string, std::string>& context = {});
    
    // Error recovery helpers
    static bool canGracefulDegrade(const std::string& operation);
    static void rollbackState(const std::string& operation);
    
    // Error reporting
    static std::string formatError(const BrainException& e);
    static void logError(const BrainException& e);
};

/**
 * Error recovery mechanisms
 */
class ErrorRecovery {
public:
    enum class RecoveryAction {
        Continue,           // Continue with degraded functionality
        Rollback,           // Rollback to previous state
        Fallback,           // Use fallback/default behavior
        Abort,              // Stop operation
        Retry              // Retry operation
    };
    
    // Recovery strategy for different error types
    static RecoveryAction getRecoveryStrategy(BrainException::ErrorCode errorCode);
    
    // Execute recovery action
    static bool executeRecovery(RecoveryAction action, 
                               const std::map<std::string, std::string>& context = {});
    
    // State management for rollback
    static void saveState(const std::string& operationName);
    static bool restoreState(const std::string& operationName);
    static void clearState(const std::string& operationName);
    
    // Graceful degradation
    static bool isNonCriticalError(BrainException::ErrorCode errorCode);
    static void enableGracefulDegradation(const std::string& component);
    static bool isGracefulDegradationEnabled(const std::string& component);
};

/**
 * Error context and diagnostic information
 */
class ErrorContext {
public:
    struct ContextInfo {
        std::string operation;
        std::string component;
        Timestamp timestamp;
        SimulationStep step;
        std::map<std::string, std::string> data;
    };
    
    static void addContext(const std::string& operation, 
                          const std::string& component,
                          const std::map<std::string, std::string>& data = {});
    
    static std::vector<ContextInfo> getRecentContext(size_t maxEntries = 100);
    static void clearContext();
    
    // Error correlation
    static void correlateErrors(const std::string& error1, const std::string& error2);
    static std::vector<std::string> getCorrelatedErrors(const std::string& error);
};

/**
 * Exception macros for common error patterns
 */
#define VALIDATE_RANGE(value, min, max, paramName) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            std::map<std::string, std::string> ctx; \
            ctx["value"] = std::to_string(value); \
            ctx["min"] = std::to_string(min); \
            ctx["max"] = std::to_string(max); \
            ctx["parameter"] = paramName; \
            throw BrainRuntimeException("Parameter out of range: " #paramName, ctx, "Validation"); \
        } \
    } while(0)

#define VALIDATE_POSITIVE(value, paramName) \
    do { \
        if ((value) < 0) { \
            std::map<std::string, std::string> ctx; \
            ctx["value"] = std::to_string(value); \
            ctx["parameter"] = paramName; \
            throw BrainRuntimeException("Parameter must be non-negative: " #paramName, ctx, "Validation"); \
        } \
    } while(0)

#define CHECK_SYSTEM_STATE(condition, message) \
    do { \
        if (!(condition)) { \
            throw BrainRuntimeException(message, "Validation"); \
        } \
    } while(0)

#define HANDLE_ERROR_RECOVERY(errorCode, operation) \
    do { \
        auto action = ErrorRecovery::getRecoveryStrategy(errorCode); \
        if (!ErrorRecovery::executeRecovery(action)) { \
            throw; // Re-throw if recovery failed \
        } \
    } while(0)

/**
 * User-friendly error message utilities
 */
class ErrorMessages {
public:
    static std::string getFriendlyMessage(const BrainException& e);
    static std::string getSuggestedRemedy(BrainException::ErrorCode errorCode);
    static std::string getTechnicalDetails(const BrainException& e);
    
    // Error severity levels for user feedback
    enum class Severity {
        Info,
        Warning,
        Error,
        Critical
    };
    
    static Severity getSeverity(BrainException::ErrorCode errorCode);
};

} // namespace nlm
