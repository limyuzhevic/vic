#include "NLMErrors.hpp"
#include "Logger.hpp"
#include <sstream>

namespace nlm {

NLMException::NLMException(ErrorCode code, ErrorCategory category, ErrorSeverity severity,
                           const std::string& message, const std::string& file, 
                           int line, const std::string& function)
    : code(code), category(category), severity(severity), 
      message(message), file(file), line(line), function(function) {
    buildMessage();
}

const char* NLMException::what() const noexcept {
    return formatMessage().c_str();
}

std::string NLMException::formatMessage() const {
    if (!formattedMessage.empty()) {
        return formattedMessage;
    }
    
    std::ostringstream oss;
    oss << "[" << Exception::errorCategoryToString(category) << "] "
        << "[" << Exception::errorCodeToString(code) << "] "
        << "[" << Exception::errorSeverityToString(severity) << "] "
        << message;
    
    if (!file.empty()) {
        oss << " (" << file << ":" << line << " in " << function << ")";
    }
    
    formattedMessage = oss.str();
    return formattedMessage;
}

// ValidationError implementations
ValidationError::ValidationError(const std::string& message, const std::string& param,
                                 const std::string& file, int line, const std::string& function)
    : NLMException(ErrorCode::VALIDATION_ERROR, ErrorCategory::Validation, ErrorSeverity::Error,
                   message, file, line, function) {
    parameterName = param;
}

ValidationError::ValidationError(ErrorCode code, const std::string& message,
                                 const std::string& param, const std::string& file,
                                 int line, const std::string& function)
    : NLMException(code, ErrorCategory::Validation, ErrorSeverity::Error,
                   message, file, line, function) {
    parameterName = param;
}

void ValidationError::buildMessage() {
    std::ostringstream oss;
    oss << "Validation failed";
    if (!parameterName.empty()) {
        oss << " for parameter: " << parameterName;
    }
    oss << ". " << message;
    message = oss.str();
}

// MemoryError implementations
MemoryError::MemoryError(const std::string& message, size_t sizeRequested,
                         const std::string& file, int line, const std::string& function)
    : NLMException(ErrorCode::OUT_OF_MEMORY, ErrorCategory::Memory, ErrorSeverity::Error,
                   message, file, line, function) {
    this->sizeRequested = sizeRequested;
}

MemoryError::MemoryError(ErrorCode code, const std::string& message, size_t sizeRequested,
                         const std::string& file, int line, const std::string& function)
    : NLMException(code, ErrorCategory::Memory, ErrorSeverity::Error,
                   message, file, line, function) {
    this->sizeRequested = sizeRequested;
}

void MemoryError::buildMessage() {
    std::ostringstream oss;
    oss << "Memory error";
    if (sizeRequested > 0) {
        oss << " (requested " << sizeRequested << " bytes)";
    }
    oss << ". " << message;
    message = oss.str();
}

// ConfigurationError implementations
ConfigurationError::ConfigurationError(const std::string& message, const std::string& key,
                                       const std::string& file, int line, const std::string& function)
    : NLMException(ErrorCode::CONFIGURATION_ERROR, ErrorCategory::Configuration, ErrorSeverity::Error,
                   message, file, line, function) {
    configKey = key;
}

ConfigurationError::ConfigurationError(ErrorCode code, const std::string& message,
                                       const std::string& key, const std::string& file,
                                       int line, const std::string& function)
    : NLMException(code, ErrorCategory::Configuration, ErrorSeverity::Error,
                   message, file, line, function) {
    configKey = key;
}

void ConfigurationError::buildMessage() {
    std::ostringstream oss;
    oss << "Configuration error";
    if (!configKey.empty()) {
        oss << " for key: " << configKey;
    }
    oss << ". " << message;
    message = oss.str();
}

// SimulationError implementations
SimulationError::SimulationError(const std::string& message, const std::string& neuralId,
                                 const std::string& file, int line, const std::string& function)
    : NLMException(ErrorCode::SIMULATION_ERROR, ErrorCategory::Simulation, ErrorSeverity::Error,
                   message, file, line, function) {
    this->neuralId = neuralId;
}

SimulationError::SimulationError(ErrorCode code, const std::string& message, const std::string& neuralId,
                                 const std::string& file, int line, const std::string& function)
    : NLMException(code, ErrorCategory::Simulation, ErrorSeverity::Error,
                   message, file, line, function) {
    this->neuralId = neuralId;
}

void SimulationError::buildMessage() {
    std::ostringstream oss;
    oss << "Simulation error";
    if (!neuralId.empty()) {
        oss << " in neural entity: " << neuralId;
    }
    oss << ". " << message;
    message = oss.str();
}

// PerformanceError implementations
PerformanceError::PerformanceError(const std::string& message, double threshold,
                                   const std::string& metric, const std::string& file,
                                   int line, const std::string& function)
    : NLMException(ErrorCode::PERFORMANCE_ERROR, ErrorCategory::Performance, ErrorSeverity::Warning,
                   message, file, line, function) {
    this->threshold = threshold;
    this->metric = metric;
}

PerformanceError::PerformanceError(ErrorCode code, const std::string& message, double threshold,
                                   const std::string& metric, const std::string& file,
                                   int line, const std::string& function)
    : NLMException(code, ErrorCategory::Performance, ErrorSeverity::Warning,
                   message, file, line, function) {
    this->threshold = threshold;
    this->metric = metric;
}

void PerformanceError::buildMessage() {
    std::ostringstream oss;
    oss << "Performance threshold exceeded";
    if (!metric.empty()) {
        oss << " for metric: " << metric;
    }
    if (threshold > 0) {
        oss << " (threshold: " << threshold << ")";
    }
    oss << ". " << message;
    message = oss.str();
}

// PluginError implementations
PluginError::PluginError(const std::string& message, const std::string& pluginName,
                         const std::string& version, const std::string& file,
                         int line, const std::string& function)
    : NLMException(ErrorCode::PLUGIN_ERROR, ErrorCategory::Plugin, ErrorSeverity::Error,
                   message, file, line, function) {
    this->pluginName = pluginName;
    this->pluginVersion = version;
}

PluginError::PluginError(ErrorCode code, const std::string& message, const std::string& pluginName,
                         const std::string& version, const std::string& file,
                         int line, const std::string& function)
    : NLMException(code, ErrorCategory::Plugin, ErrorSeverity::Error,
                   message, file, line, function) {
    this->pluginName = pluginName;
    this->pluginVersion = version;
}

void PluginError::buildMessage() {
    std::ostringstream oss;
    oss << "Plugin error";
    if (!pluginName.empty()) {
        oss << " for plugin: " << pluginName;
    }
    if (!pluginVersion.empty()) {
        oss << " (version: " << pluginVersion << ")";
    }
    oss << ". " << message;
    message = oss.str();
}

// IntegrationError implementations
IntegrationError::IntegrationError(const std::string& message, const std::string& dependency,
                                   const std::string& interface, const std::string& file,
                                   int line, const std::string& function)
    : NLMException(ErrorCode::INTEGRATION_ERROR, ErrorCategory::Integration, ErrorSeverity::Error,
                   message, file, line, function) {
    this->dependencyName = dependency;
    this->interfaceName = interface;
}

IntegrationError::IntegrationError(ErrorCode code, const std::string& message,
                                   const std::string& dependency, const std::string& interface,
                                   const std::string& file, int line, const std::string& function)
    : NLMException(code, ErrorCategory::Integration, ErrorSeverity::Error,
                   message, file, line, function) {
    this->dependencyName = dependency;
    this->interfaceName = interface;
}

void IntegrationError::buildMessage() {
    std::ostringstream oss;
    oss << "Integration error";
    if (!dependencyName.empty()) {
        oss << " for dependency: " << dependencyName;
    }
    if (!interfaceName.empty()) {
        oss << " with interface: " << interfaceName;
    }
    oss << ". " << message;
    message = oss.str();
}

// Exception helper function implementations
namespace Exception {

const char* errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::OK: return "OK";
        case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        case ErrorCode::NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case ErrorCode::OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case ErrorCode::INVALID_ARGUMENT: return "INVALID_ARGUMENT";
        case ErrorCode::INVALID_STATE: return "INVALID_STATE";
        case ErrorCode::TIMEOUT: return "TIMEOUT";
        case ErrorCode::VALIDATION_ERROR: return "VALIDATION_ERROR";
        case ErrorCode::PARAMETER_VALIDATION_FAILED: return "PARAMETER_VALIDATION_FAILED";
        case ErrorCode::STATE_VALIDATION_FAILED: return "STATE_VALIDATION_FAILED";
        case ErrorCode::RESOURCE_VALIDATION_FAILED: return "RESOURCE_VALIDATION_FAILED";
        case ErrorCode::INTEGRATION_VALIDATION_FAILED: return "INTEGRATION_VALIDATION_FAILED";
        case ErrorCode::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
        case ErrorCode::MEMORY_EXHAUSTION: return "MEMORY_EXHAUSTION";
        case ErrorCode::MEMORY_CORRUPTION: return "MEMORY_CORRUPTION";
        case ErrorCode::MEMORY_ACCESS_VIOLATION: return "MEMORY_ACCESS_VIOLATION";
        case ErrorCode::CONFIGURATION_ERROR: return "CONFIGURATION_ERROR";
        case ErrorCode::CONFIG_FILE_NOT_FOUND: return "CONFIG_FILE_NOT_FOUND";
        case ErrorCode::CONFIG_INVALID_FORMAT: return "CONFIG_INVALID_FORMAT";
        case ErrorCode::CONFIG_VALUE_INVALID: return "CONFIG_VALUE_INVALID";
        case ErrorCode::CONFIG_KEY_MISSING: return "CONFIG_KEY_MISSING";
        case ErrorCode::SIMULATION_ERROR: return "SIMULATION_ERROR";
        case ErrorCode::NEURAL_ERROR: return "NEURAL_ERROR";
        case ErrorCode::NETWORK_UNCONNECTED: return "NETWORK_UNCONNECTED";
        case ErrorCode::INVALID_SIMULATION_STEP: return "INVALID_SIMULATION_STEP";
        case ErrorCode::SIMULATION_TIMEOUT: return "SIMULATION_TIMEOUT";
        case ErrorCode::PERFORMANCE_ERROR: return "PERFORMANCE_ERROR";
        case ErrorCode::COMPUTATION_OVERLOADED: return "COMPUTATION_OVERLOADED";
        case ErrorCode::BANDWIDTH_EXCEEDED: return "BANDWIDTH_EXCEEDED";
        case ErrorCode::CPU_LIMIT_EXCEEDED: return "CPU_LIMIT_EXCEEDED";
        case ErrorCode::LATENCY_EXCEEDED: return "LATENCY_EXCEEDED";
        case ErrorCode::PLUGIN_ERROR: return "PLUGIN_ERROR";
        case ErrorCode::PLUGIN_NOT_FOUND: return "PLUGIN_NOT_FOUND";
        case ErrorCode::PLUGIN_VERSION_MISMATCH: return "PLUGIN_VERSION_MISMATCH";
        case ErrorCode::PLUGIN_LOAD_FAILED: return "PLUGIN_LOAD_FAILED";
        case ErrorCode::PLUGIN_UNSAFE: return "PLUGIN_UNSAFE";
        case ErrorCode::INTEGRATION_ERROR: return "INTEGRATION_ERROR";
        case ErrorCode::DEPENDENCY_MISSING: return "DEPENDENCY_MISSING";
        case ErrorCode::API_VERSION_MISMATCH: return "API_VERSION_MISMATCH";
        case ErrorCode::INTERFACE_ERROR: return "INTERFACE_ERROR";
        case ErrorCode::PROTOCOL_ERROR: return "PROTOCOL_ERROR";
        default: return "UNKNOWN";
    }
}

const char* errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::None: return "NONE";
        case ErrorCategory::Generic: return "GENERIC";
        case ErrorCategory::Validation: return "VALIDATION";
        case ErrorCategory::Memory: return "MEMORY";
        case ErrorCategory::Configuration: return "CONFIGURATION";
        case ErrorCategory::Simulation: return "SIMULATION";
        case ErrorCategory::Performance: return "PERFORMANCE";
        case ErrorCategory::Plugin: return "PLUGIN";
        case ErrorCategory::Integration: return "INTEGRATION";
        case ErrorCategory::System: return "SYSTEM";
        default: return "UNKNOWN";
    }
}

const char* errorSeverityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARN";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
        case ErrorSeverity::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

bool isType(const NLMException& e, ErrorCategory category) {
    return e.isOfType(category);
}

bool isType(const NLMException& e, ErrorCode code) {
    return e.isOfType(code);
}

void chain(const NLMException& cause, const std::string& message) {
    std::string chainedMessage = std::string("Chained from: ") + cause.what();
    // This would ideally create a new exception that chains to the original
    // For simplicity, we just log it
    Logger::getGlobal().error(chainedMessage);
}

void report(const NLMException& e) {
    // Map severity to log level
    LogLevel logLevel;
    switch (e.getSeverity()) {
        case ErrorSeverity::Debug: logLevel = LogLevel::Debug; break;
        case ErrorSeverity::Info: logLevel = LogLevel::Info; break;
        case ErrorSeverity::Warning: logLevel = LogLevel::Warning; break;
        case ErrorSeverity::Error: logLevel = LogLevel::Error; break;
        case ErrorSeverity::Critical: logLevel = LogLevel::Critical; break;
        case ErrorSeverity::Fatal: logLevel = LogLevel::Critical; break;
        default: logLevel = LogLevel::Error; break;
    }
    
    Logger::getGlobal().log(logLevel, e.formatMessage(), e.getFile(), e.getLine(), e.what());
}

} // namespace Exception

} // namespace nlm
