#pragma once

#include "ErrorSystem.hpp"
#include "Config.hpp"

namespace nlm {

// Helper function to create error context from current location
inline ErrorContext makeErrorContext(const std::string& component,
                                      const std::string& operation,
                                      const std::string& file = __FILE__,
                                      int line = __LINE__,
                                      const std::string& function = __func__) {
    return ErrorContext(component, operation, file, line, function);
}

// Helper function to throw exception with reporting
inline void throwWithReport(const NLMException& error) {
    reportError(error);
    throw error;
}

// Helper function to report but not throw
inline void report(const NLMException& error) {
    reportError(error);
}

// Config-specific error factory functions
inline ConfigError configError(ErrorCode code, const std::string& message,
                               const ErrorContext& ctx = makeErrorContext("Config", "loadFromFile")) {
    return ConfigError(code, message, ctx);
}

inline ConfigError configValueError(const std::string& key, const std::string& expectedType,
                                    const std::string& actualType,
                                    const ErrorContext& ctx = makeErrorContext("Config", "get")) {
    std::ostringstream oss;
    oss << "Invalid value for key \"" << key << "\": expected "
        << expectedType << ", got " << actualType;
    return configError(ErrorCode::ConfigValueInvalid, oss.str(), ctx);
}

// Neuron-specific error factory functions
inline NeuronError neuronError(ErrorCode code, const std::string& message,
                               const ErrorContext& ctx = makeErrorContext("Neuron", "step")) {
    return NeuronError(code, message, ctx);
}

// Memory error factory function
inline MemoryError memoryError(ErrorCode code, const std::string& message,
                               const ErrorContext& ctx = makeErrorContext("Memory", "allocate")) {
    return MemoryError(code, message, ctx);
}

// File error factory function
inline FileError fileError(ErrorCode code, const std::string& message,
                           const ErrorContext& ctx = makeErrorContext("File", "read")) {
    return FileError(code, message, ctx);
}

// Network error factory function
inline NetworkError networkError(ErrorCode code, const std::string& message,
                                 const ErrorContext& ctx = makeErrorContext("Network", "propagate")) {
    return NetworkError(code, message, ctx);
}

// Development error factory function
inline DevelopmentError developmentError(ErrorCode code, const std::string& message,
                                         const ErrorContext& ctx = makeErrorContext("Development", "update")) {
    return DevelopmentError(code, message, ctx);
}

// Visualization error factory function
inline VisualizationError visualizationError(ErrorCode code, const std::string& message,
                                             const ErrorContext& ctx = makeErrorContext("Visualization", "render")) {
    return VisualizationError(code, message, ctx);
}

// Integration error factory function
inline IntegrationError integrationError(ErrorCode code, const std::string& message,
                                         const ErrorContext& ctx = makeErrorContext("Integration", "initialize")) {
    return IntegrationError(code, message, ctx);
}

} // namespace nlm
