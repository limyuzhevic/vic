// **NLM Error Handling System**
//
// Comprehensive error handling and recovery system for the NLM framework.
// This header provides a complete error handling infrastructure that can be
// optionally included in any NLM component.

#pragma once

// Core error handling infrastructure
#include "ErrorTypes.hpp"
#include "RecoveryUtils.hpp"

// Include specific exception types for convenience
#include "ErrorTypes.hpp" // Already included, but ensures completeness

// Integration macros for existing components

// **Config Error Handling Integration**
#define NLM_CONFIG_ERROR_HANDLER(operation) \
    nlm::ErrorHandling::ConfigErrorHandler::operation

#define NLM_CONFIG_RETRY(config, filepath, identifier, func) \
    nlm::ErrorHandling::ConfigErrorHandler::safeLoadConfig(config, filepath, identifier, func)

// **Brain Error Handling Integration**
#define NLM_BRAIN_ERROR_HANDLER(operation) \
    nlm::ErrorHandling::BrainErrorHandler::operation

#define NLM_BRAIN_RETRY(brain, config, identifier, func) \
    nlm::ErrorHandling::BrainErrorHandler::safeInitializeBrain(brain, config, identifier, func)

// **Neuron Error Handling Integration**
#define NLM_NEURON_ERROR_HANDLER(operation) \
    nlm::ErrorHandling::NeuronErrorHandler::operation

#define NLM_NEURON_RETRY(neuron, currentTime, dt, func) \
    nlm::ErrorHandling::NeuronErrorHandler::safeNeuronStep(neuron, currentTime, dt, func)

// **Synapse Error Handling Integration**
#define NLM_SYNAPSE_ERROR_HANDLER(operation) \
    nlm::ErrorHandling::SynapseErrorHandler::operation

// **Simulation Error Handling Integration**
#define NLM_SIMULATION_ERROR_HANDLER(operation) \
    nlm::ErrorHandling::SimulationErrorHandler::operation

// **Global Error Management**
namespace nlm {
    // Initialize global error handling system
    inline void initializeErrorHandling(ErrorReportLevel level = ErrorReportLevel::Error,
                                        ExceptionStrategy strategy = ExceptionStrategy::Throw) {
        ErrorHandler::initialize(level, strategy);
    }
    
    // Get global error handler
    inline ErrorHandler& getErrorHandler() {
        return ErrorHandler::getInstance();
    }
    
    // Get global error recovery system
    inline ErrorHandling::GlobalErrorRecovery& getGlobalErrorRecovery() {
        return ErrorHandling::getGlobalErrorRecovery();
    }
    
    // Convenience error reporting functions
    inline void logConfigError(const std::string& message, const std::string& context = "") {
        ErrorHandler::getInstance().report(message, ErrorReportLevel::Error, context);
    }
    
    inline void logBrainError(const std::string& message, const std::string& context = "") {
        ErrorHandler::getInstance().report(message, ErrorReportLevel::Error, context);
    }
    
    inline void logNeuronError(const std::string& message, const std::string& context = "") {
        ErrorHandler::getInstance().report(message, ErrorReportLevel::Error, context);
    }
    
    inline void logSynapseError(const std::string& message, const std::string& context = "") {
        ErrorHandler::getInstance().report(message, ErrorReportLevel::Error, context);
    }
    
    inline void logSimulationError(const std::string& message, const std::string& context = "") {
        ErrorHandler::getInstance().report(message, ErrorReportLevel::Error, context);
    }
    
    // Error recovery helper functions
    inline bool isErrorRetryable(ErrorCode errorCode) {
        // Common retryable errors
        switch (errorCode) {
            case ErrorCode::MemoryAllocationFailure:
            case ErrorCode::SimulationRuntimeError:
            case ErrorCode::NeuronStateError:
            case ErrorCode::SynapseConnectionError:
                return true;
            default:
                return false;
        }
    }
    
    // Exception safety wrappers
    template<typename Func>
    void withErrorHandling(const std::string& operation, Func&& func,
                          RecoveryMode mode = RecoveryMode::Abort) {
        try {
            func();
        } catch (const NLMException& e) {
            ErrorHandler::getInstance().reportException(e);
            
            if (mode == RecoveryMode::Abort) {
                throw;
            }
            // Graceful degradation or retry handled by caller
        }
    }
}

// **Backward Compatibility Layer**
namespace nlm {
    // Legacy exception types for backward compatibility
    typedef nlm::ConfigFileNotFoundException ConfigFileNotFoundError;
    typedef nlm::MalformedConfigException MalformedConfigError;
    typedef nlm::ConfigValidationException ConfigValidationError;
    typedef nlm::ConfigParseException ConfigParseError;
    typedef nlm::BrainInitException BrainInitError;
    typedef nlm::SimulationRuntimeException SimulationRuntimeError;
    typedef nlm::MemoryAllocationException MemoryAllocationError;
    typedef nlm::NeuronThresholdException NeuronThresholdError;
    typedef nlm::RefractoryPeriodException RefractoryPeriodError;
    typedef nlm::SynapticInputException SynapticInputError;
    typedef nlm::SynapseConnectionException SynapseConnectionError;
    typedef nlm::WeightCalculationException WeightCalculationError;
    typedef nlm::TimeStepException TimeStepError;
    typedef nlm::StabilityException StabilityError;
    typedef nlm::ResourceAllocationException ResourceAllocationError;
}

// **Usage Examples**

/*
 * Example 1: Basic Error Handling
 * 
 * #include "ErrorHandling.hpp"
 * 
 * void someFunction() {
 *     try {
 *         // Critical operation
 *         riskyOperation();
 *     } catch (const nlm::NLMException& e) {
 *         nlm::logError(e.getMessage(), e.getContext());
 *         // Handle error or rethrow
 *     }
 * }
 *
 *
 * Example 2: Error Handling with Recovery
 * 
 * #include "ErrorHandling.hpp"
 * 
 * void safeOperation() {
 *     nlm::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();
 *     
 *     auto result = recovery.executeSafely("ComplexOperation", []() {
 *         return performComplexOperation();
 *     });
 *     
 *     if (result.isDegradedValue()) {
 *         nlm::logWarning("Operation completed in degraded mode: " + 
 *                        result.getDegradationReason());
 *     }
 * }
 *
 *
 * Example 3: Config Loading with Error Handling
 * 
 * #include "ErrorHandling.hpp"
 * 
 * int main() {
 *     nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
 *     
 *     nlm::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();n
 *     
 *     auto config = recovery.getConfigCheckpoint().load("last");
 *     if (!config) {
 *         config = recovery.getRetryManager().executeWithRetry("LoadConfig", []() {
 *             return nlm::ErrorHandling::ConfigErrorHandler::loadFromFile("config.cfg");
 *         });
 *     }
 *     
 *     if (config) {
 *         // Use config
 *     } else {
 *         // Handle missing config
 *     }
 * }
 */
