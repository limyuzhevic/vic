#pragma once

#include "NLMException.hpp"
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

// Exception handling macros
#define NLM_THROW(exception_type, message) \
    do { \
        exception_type e(message); \
        std::vector<std::string> stackTrace; \
        stackTrace.push_back("Exception thrown at " + std::string(__FILE__) + ":" + std::to_string(__LINE__)); \
        e.addStackTrace(stackTrace); \
        throw e; \
    } while(0)

#define NLM_THROW_WITH_CONTEXT(exception_type, message, ...) \
    do { \
        exception_type e(message); \
        std::vector<std::string> stackTrace; \
        stackTrace.push_back("Exception thrown at " + std::string(__FILE__) + ":" + std::to_string(__LINE__)); \
        e.addStackTrace(stackTrace); \
        /* Add context variables here */ \
        throw e; \
    } while(0)

#define NLM_TRY_CATCH(operation, error_handler) \
    try { \
        operation; \
    } catch (const NLMException& e) { \
        error_handler(e); \
    } catch (const std::exception& e) { \
        NLM_THROW(ConfigurationError, std::string("Standard exception in " #operation ": " + std::string(e.what()))); \
    } \

{
    std::vector<std::string> stackTrace; \
    stackTrace.push_back("Exception thrown at " + std::string(__FILE__) + ":" + std::to_string(__LINE__)); \
    e.addStackTrace(stackTrace); \
    throw e; \
} while(0)

#define NLM_TRY_CATCH(operation, error_handler) \
    try { \
        operation; \
    } catch (const NLMException& e) { \
        error_handler(e); \
    } catch (const std::exception& e) { \
        NLM_THROW(ConfigurationError, std::string("Standard exception in " #operation ": " + std::string(e.what()))); \
    } \

#define NLM_CATCH_AND_LOG(operation, logger) \
    try { \
        operation; \
    } catch (const NLMException& e) { \
        logger.error(e.getFullMessage()); \
    } catch (const std::exception& e) { \
        logger.error(std::string("Unexpected error: ") + e.what()); \
    }

// Exception conversion utilities
inline std::string formatException(const NLMException& e) {
    std::ostringstream oss;
    oss << "=== " << e.getCode() << " ===\n";
    oss << "Message: " << e.getMessage() << "\n";
    oss << "Timestamp: " << e.getTimestampString() << "\n";
    oss << "Context: ";
    const auto& context = e.getContext();
    if (context.empty()) {
        oss << "None\n";
    } else {
        for (const auto& pair : context) {
            oss << "[" << pair.first << "=" << pair.second << "] ";
        }
        oss << "\n";
    }
    oss << "Stack Trace:\n" << e.getStackTrace();
    return oss.str();
}

// Exception factory functions
inline ConfigurationError makeConfigurationError(const std::string& message, 
                                                 const std::map<std::string, std::string>& context = {}) {
    return ConfigurationError(message, "CONFIG_ERROR", context);
}

inline MemoryError makeMemoryError(const std::string& message,
                                  const std::map<std::string, std::string>& context = {}) {
    return MemoryError(message, "MEMORY_ERROR", context);
}

inline NeuralError makeNeuralError(const std::string& message,
                                  const std::map<std::string, std::string>& context = {}) {
    return NeuralError(message, "NEURAL_ERROR", context);
}

inline PlasticityError makePlasticityError(const std::string& message,
                                         const std::map<std::string, std::string>& context = {}) {
    return PlasticityError(message, "PLASTICITY_ERROR", context);
}

inline IntegrationError makeIntegrationError(const std::string& message,
                                             const std::map<std::string, std::string>& context = {}) {
    return IntegrationError(message, "INTEGRATION_ERROR", context);
}

inline DevelopmentError makeDevelopmentError(const std::string& message,
                                             const std::map<std::string, std::string>& context = {}) {
    return DevelopmentError(message, "DEVELOPMENT_ERROR", context);
}

inline NeuromodulationError makeNeuromodulationError(const std::string& message,
                                                     const std::map<std::string, std::string>& context = {}) {
    return NeuromodulationError(message, "NEUROMODULATION_ERROR", context);
}

inline MemorySystemError makeMemorySystemError(const std::string& message,
                                               const std::map<std::string, std::string>& context = {}) {
    return MemorySystemError(message, "MEMORY_SYSTEM_ERROR", context);
}

inline PredictionError makePredictionError(const std::string& message,
                                          const std::map<std::string, std::string>& context = {}) {
    return PredictionError(message, "PREDICTION_ERROR", context);
}

inline CognitiveError makeCognitiveError(const std::string& message,
                                        const std::map<std::string, std::string>& context = {}) {
    return CognitiveError(message, "COGNITIVE_ERROR", context);
}

inline SensoryError makeSensoryError(const std::string& message,
                                    const std::map<std::string, std::string>& context = {}) {
    return SensoryError(message, "SENSORY_ERROR", context);
}

inline MotorError makeMotorError(const std::string& message,
                                const std::map<std::string, std::string>& context = {}) {
    return MotorError(message, "MOTOR_ERROR", context);
}

inline WorldError makeWorldError(const std::string& message,
                                const std::map<std::string, std::string>& context = {}) {
    return WorldError(message, "WORLD_ERROR", context);
}

inline AgentError makeAgentError(const std::string& message,
                                const std::map<std::string, std::string>& context = {}) {
    return AgentError(message, "AGENT_ERROR", context);
}

} // namespace nlm