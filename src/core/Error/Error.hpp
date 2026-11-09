// NLM Error System
// Comprehensive exception hierarchy for all error conditions

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <any>
#include <typeindex>
#include <functional>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Base exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                  ErrorSeverity severity = ErrorSeverity::Error,
                  const std::string& code = "",
                  const std::string& file = "",
                  int line = 0,
                  const std::string& function = "")
        : std::runtime_error(message),
          severity_(severity),
          code_(code),
          file_(file),
          line_(line),
          function_(function),
          timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMException() = default;
    
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getCode() const { return code_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    auto getTimestamp() const { return timestamp_; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[" << severityToString(severity_) << "] " << getCode() << ": " << what();
        if (!file_.empty()) {
            oss << " at " << file_;
            if (line_ > 0) oss << ":" << line_;
        }
        if (!function_.empty()) oss << " in " << function_;
        return oss.str();
    }
    
    virtual std::string toString() const {
        return getFullMessage();
    }
    
protected:
    static std::string severityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARN";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRIT";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
private:
    ErrorSeverity severity_;
    std::string code_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
};

// Configuration errors
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Error, "CONFIG", file, line, function) {}
};

class ConfigParseError : public ConfigError {
public:
    ConfigParseError(const std::string& message, const std::string& key = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : ConfigError(message, file, line, function) {
        if (!key.empty()) code_ = "CONFIG_PARSE_" + key;
    }
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : ConfigError(message, file, line, function) {
        if (!key.empty()) code_ = "CONFIG_VALIDATION_" + key;
    }
};

class ConfigSchemaError : public ConfigError {
public:
    ConfigSchemaError(const std::string& message, const std::string& schema = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : ConfigError(message, file, line, function) {
        if (!schema.empty()) code_ = "CONFIG_SCHEMA_" + schema;
    }
};

// Simulation errors
class SimulationError : public NLMException {
public:
    SimulationError(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Error, "SIMULATION", file, line, function) {}
};

class NeuronError : public SimulationError {
public:
    NeuronError(const std::string& message, NeuronId neuronId = INVALID_NEURON_ID, const std::string& file = "", int line = 0, const std::string& function = "")
        : SimulationError(message, file, line, function) {
        code_ = "NEURON_" + (neuronId.value != INVALID_NEURON_ID.value ? std::to_string(neuronId.value) : "UNKNOWN");
    }
};

class SynapseError : public SimulationError {
public:
    SynapseError(const std::string& message, SynapseId synapseId = INVALID_SYNAPSE_ID, const std::string& file = "", int line = 0, const std::string& function = "")
        : SimulationError(message, file, line, function) {
        code_ = "SYNAPSE_" + (synapseId.value != INVALID_SYNAPSE_ID.value ? std::to_string(synapseId.value) : "UNKNOWN");
    }
};

class RegionError : public SimulationError {
public:
    RegionError(const std::string& message, RegionId regionId = INVALID_REGION_ID, const std::string& file = "", int line = 0, const std::string& function = "")
        : SimulationError(message, file, line, function) {
        code_ = "REGION_" + (regionId.value != INVALID_REGION_ID.value ? std::to_string(regionId.value) : "UNKNOWN");
    }
};

// Memory and resource errors
class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Critical, "MEMORY", file, line, function) {}
};

class ResourceError : public NLMException {
public:
    ResourceError(const std::string& message, const std::string& resource = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Error, "RESOURCE" + (!resource.empty() ? "_" + resource : ""), file, line, function) {}
};

class FileError : public ResourceError {
public:
    FileError(const std::string& message, const std::string& filepath = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : ResourceError(message, "FILE_" + filepath, file, line, function) {}
};

// Cognitive system errors
class CognitionError : public NLMException {
public:
    CognitionError(const std::string& message, const std::string& component = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Error, "COGNITION" + (!component.empty() ? "_" + component : ""), file, line, function) {}
};

class PlanningError : public CognitionError {
public:
    PlanningError(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")
        : CognitionError(message, "PLANNER", file, line, function) {}
};

class ConceptError : public CognitionError {
public:
    ConceptError(const std::string& message, const std::string& concept = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : CognitionError(message, "CONCEPT" + (!concept.empty() ? "_" + concept : ""), file, line, function) {}
};

// Neuromodulation errors
class NeuromodulationError : public NLMException {
public:
    NeuromodulationError(const std::string& message, const std::string& system = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Warning, "NEUROMODULATION" + (!system.empty() ? "_" + system : ""), file, line, function) {}
};

// Agent system errors
class AgentError : public NLMException {
public:
    AgentError(const std::string& message, const std::string& component = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Error, "AGENT" + (!component.empty() ? "_" + component : ""), file, line, function) {}
};

// Development errors
class DevelopmentError : public NLMException {
public:
    DevelopmentError(const std::string& message, const std::string& stage = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Warning, "DEVELOPMENT" + (!stage.empty() ? "_" + stage : ""), file, line, function) {}
};

// Performance errors
class PerformanceError : public NLMException {
public:
    PerformanceError(const std::string& message, const std::string& component = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Warning, "PERFORMANCE" + (!component.empty() ? "_" + component : ""), file, line, function) {}
};

// Python binding errors
class PythonBindingError : public NLMException {
public:
    PythonBindingError(const std::string& message, const std::string& binding = "", const std::string& file = "", int line = 0, const std::string& function = "")
        : NLMException(message, ErrorSeverity::Error, "PYTHON_BINDING" + (!binding.empty() ? "_" + binding : ""), file, line, function) {}
};

// Error reporting and handling utilities
class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    void registerErrorHandler(std::function<void(const NLMException&)>) {
        errorHandler_ = std::move(errorHandler_);
    }
    
    void handleError(const NLMException& error) {
        if (errorHandler_) {
            errorHandler_(error);
        } else {
            // Default: log to stderr and potentially throw
            std::cerr << "NLM Error: " << error.getFullMessage() << std::endl;
        }
    }
    
    void setGlobalErrorHandling(bool enableRethrow) {
        enableRethrow_ = enableRethrow;
    }
    
    bool isGlobalErrorHandlingEnabled() const {
        return enableRethrow_;
    }
    
private:
    ErrorReporter() : enableRethrow_(false) {}
    std::function<void(const NLMException&)> errorHandler_;
    bool enableRethrow_;
};

// Utility function to convert exceptions to strings
inline std::string exceptionToString(const std::exception& e) {
    std::ostringstream oss;
    oss << "Exception: " << e.what();
    return oss.str();
}

// Utility function to convert NLM exceptions to strings
inline std::string nlmExceptionToString(const NLMException& e) {
    std::ostringstream oss;
    oss << "NLM Exception: " << e.getFullMessage();
    return oss.str();
}

// Global exception macros for easier error reporting
#define NLM_THROW(...) throw nlm::NLMException(__VA_ARGS__)
#define NLM_THROW_WITH_CODE(...) throw nlm::NLMException(__VA_ARGS__)
#define NLM_TRY_CATCH(...) try { __VA_ARGS__ } catch(const std::exception& e) { /* handle exception */ }

} // namespace nlm