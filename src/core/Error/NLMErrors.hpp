#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <memory>
#include <any>

namespace nlm {

// Error codes enumeration
enum class ErrorCode : uint32_t {
    // Generic errors
    OK = 0,
    UNKNOWN_ERROR = 1,
    NOT_IMPLEMENTED = 2,
    OUT_OF_MEMORY = 3,
    INVALID_ARGUMENT = 4,
    INVALID_STATE = 5,
    TIMEOUT = 6,
    
    // Validation errors
    VALIDATION_ERROR = 100,
    PARAMETER_VALIDATION_FAILED = 101,
    STATE_VALIDATION_FAILED = 102,
    RESOURCE_VALIDATION_FAILED = 103,
    INTEGRATION_VALIDATION_FAILED = 104,
    
    // Memory errors
    MEMORY_ALLOCATION_FAILED = 200,
    MEMORY_EXHAUSTION = 201,
    MEMORY_CORRUPTION = 202,
    MEMORY_ACCESS_VIOLATION = 203,
    
    // Configuration errors
    CONFIGURATION_ERROR = 300,
    CONFIG_FILE_NOT_FOUND = 301,
    CONFIG_INVALID_FORMAT = 302,
    CONFIG_VALUE_INVALID = 303,
    CONFIG_KEY_MISSING = 304,
    
    // Simulation errors
    SIMULATION_ERROR = 400,
    NEURAL_ERROR = 401,
    NETWORK_UNCONNECTED = 402,
    INVALID_SIMULATION_STEP = 403,
    SIMULATION_TIMEOUT = 404,
    
    // Performance errors
    PERFORMANCE_ERROR = 500,
    COMPUTATION_OVERLOADED = 501,
    BANDWIDTH_EXCEEDED = 502,
    CPU_LIMIT_EXCEEDED = 503,
    LATENCY_EXCEEDED = 504,
    
    // Plugin errors
    PLUGIN_ERROR = 600,
    PLUGIN_NOT_FOUND = 601,
    PLUGIN_VERSION_MISMATCH = 602,
    PLUGIN_LOAD_FAILED = 603,
    PLUGIN_UNSAFE = 604,
    
    // Integration errors
    INTEGRATION_ERROR = 700,
    DEPENDENCY_MISSING = 701,
    API_VERSION_MISMATCH = 702,
    INTERFACE_ERROR = 703,
    PROTOCOL_ERROR = 704,
};

// Error categories for organization
enum class ErrorCategory : uint8_t {
    None = 0,
    Generic = 1,
    Validation = 2,
    Memory = 3,
    Configuration = 4,
    Simulation = 5,
    Performance = 6,
    Plugin = 7,
    Integration = 8,
    System = 9,
};

// Error severity levels
enum class ErrorSeverity : uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error context information
struct ErrorContext {
    std::string file;
    int line = 0;
    std::string function;
    std::string thread_id;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional contextual data
    std::map<std::string, std::any> data;
    
    ErrorContext() : line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(const std::string& file, int line, const std::string& function)
        : file(file), line(line), function(function) {
        timestamp = std::chrono::system_clock::now();
    }
    
    void addData(const std::string& key, const std::any& value) {
        data[key] = value;
    }
    
    template<typename T>
    void addData(const std::string& key, const T& value) {
        data[key] = value;
    }
    
    template<typename T>
    T getData(const std::string& key, const T& defaultValue = T{}) const {
        auto it = data.find(key);
        if (it != data.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {}
        }
        return defaultValue;
    }
};

// Base NLMException class
class NLMException : public std::exception {
protected:
    ErrorCode code;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string file;
    int line;
    std::string function;
    mutable std::string formattedMessage;
    
public:
    NLMException(ErrorCode code, ErrorCategory category, ErrorSeverity severity,
                 const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "");
    
    virtual ~NLMException() noexcept = default;
    
    // Error information
    ErrorCode getCode() const noexcept { return code; }
    ErrorCategory getCategory() const noexcept { return category; }
    ErrorSeverity getSeverity() const noexcept { return severity; }
    const std::string& getMessage() const noexcept { return message; }
    const std::string& getFile() const noexcept { return file; }
    int getLine() const noexcept { return line; }
    const std::string& getFunction() const noexcept { return function; }
    
    // String representation
    const char* what() const noexcept override;
    
    // Formatted message with context
    std::string formatMessage() const;
    
    // Check if this exception is of a specific type
    virtual bool isOfType(ErrorCategory category) const noexcept { return this->category == category; }
    
    virtual bool isOfType(ErrorCode code) const noexcept { return this->code == code; }
    
    // Get error context
    ErrorContext getContext() const {
        return ErrorContext(file, line, function);
    }
    
    // Clone method for polymorphic handling
    virtual std::unique_ptr<NLMException> clone() const = 0;
    
protected:
    virtual void buildMessage() {}
};

// Specific exception types

// ValidationError for validation failures
class ValidationError : public NLMException {
public:
    enum class ErrorType {
        ParameterValidation,
        StateValidation,
        ResourceValidation,
        IntegrationValidation,
        FormatValidation
    };
    
    ValidationError(const std::string& message, const std::string& param = "",
                    const std::string& file = "", int line = 0, const std::string& function = "");
    
    ValidationError(ErrorCode code, const std::string& message, 
                    const std::string& param = "", const std::string& file = "",
                    int line = 0, const std::string& function = "");
    
    ValidationError(const ValidationError& other) : NLMException(other) {
        this->parameterName = other.parameterName;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<ValidationError>(*this);
    }
    
    const std::string& getParameterName() const { return parameterName; }
    
    void setParameterName(const std::string& param) { parameterName = param; }
    
protected:
    void buildMessage() override;
    
private:
    std::string parameterName;
};

// MemoryError for memory-related issues
class MemoryError : public NLMException {
public:
    enum class ErrorType {
        AllocationFailed,
        Exhaustion,
        Corruption,
        AccessViolation
    };
    
    MemoryError(const std::string& message, size_t sizeRequested = 0,
                const std::string& file = "", int line = 0, const std::string& function = "");
    
    MemoryError(ErrorCode code, const std::string& message, size_t sizeRequested = 0,
                const std::string& file = "", int line = 0, const std::string& function = "");
    
    MemoryError(const MemoryError& other) : NLMException(other) {
        this->sizeRequested = other.sizeRequested;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<MemoryError>(*this);
    }
    
    size_t getSizeRequested() const { return sizeRequested; }
    
protected:
    void buildMessage() override;
    
private:
    size_t sizeRequested;
};

// ConfigurationError for configuration issues
class ConfigurationError : public NLMException {
public:
    enum class ErrorType {
        FileNotFound,
        InvalidFormat,
        InvalidValue,
        MissingKey,
        TemplateMismatch
    };
    
    ConfigurationError(const std::string& message, const std::string& key = "",
                       const std::string& file = "", int line = 0, const std::string& function = "");
    
    ConfigurationError(ErrorCode code, const std::string& message, const std::string& key = "",
                       const std::string& file = "", int line = 0, const std::string& function = "");
    
    ConfigurationError(const ConfigurationError& other) : NLMException(other) {
        this->configKey = other.configKey;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<ConfigurationError>(*this);
    }
    
    const std::string& getConfigKey() const { return configKey; }
    
protected:
    void buildMessage() override;
    
private:
    std::string configKey;
};

// SimulationError for simulation-related issues
class SimulationError : public NLMException {
public:
    enum class ErrorType {
        NeuralNetworkError,
        NetworkUnconnected,
        InvalidStep,
        Timeout,
        EvolutionStalled
    };
    
    SimulationError(const std::string& message, const std::string& neuralId = "",
                    const std::string& file = "", int line = 0, const std::string& function = "");
    
    SimulationError(ErrorCode code, const std::string& message, const std::string& neuralId = "",
                    const std::string& file = "", int line = 0, const std::string& function = "");
    
    SimulationError(const SimulationError& other) : NLMException(other) {
        this->neuralId = other.neuralId;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<SimulationError>(*this);
    }
    
    const std::string& getNeuralId() const { return neuralId; }
    
protected:
    void buildMessage() override;
    
private:
    std::string neuralId;
};

// PerformanceError for performance-related issues
class PerformanceError : public NLMException {
public:
    enum class ErrorType {
        ComputationOverload,
        BandwidthExceeded,
        CpuLimitExceeded,
        LatencyExceeded,
        MemoryUsageExceeded
    };
    
    PerformanceError(const std::string& message, double threshold = 0.0,
                     const std::string& metric = "",
                     const std::string& file = "", int line = 0, const std::string& function = "");
    
    PerformanceError(ErrorCode code, const std::string& message, double threshold = 0.0,
                     const std::string& metric = "", const std::string& file = "",
                     int line = 0, const std::string& function = "");
    
    PerformanceError(const PerformanceError& other) : NLMException(other) {
        this->threshold = other.threshold;
        this->metric = other.metric;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<PerformanceError>(*this);
    }
    
    double getThreshold() const { return threshold; }
    const std::string& getMetric() const { return metric; }
    
protected:
    void buildMessage() override;
    
private:
    double threshold;
    std::string metric;
};

// PluginError for plugin-related issues
class PluginError : public NLMException {
public:
    enum class ErrorType {
        NotFound,
        VersionMismatch,
        LoadFailed,
        Unsafe,
        InitializationFailed
    };
    
    PluginError(const std::string& message, const std::string& pluginName = "",
                const std::string& version = "", const std::string& file = "",
                int line = 0, const std::string& function = "");
    
    PluginError(ErrorCode code, const std::string& message, const std::string& pluginName = "",
                const std::string& version = "", const std::string& file = "",
                int line = 0, const std::string& function = "");
    
    PluginError(const PluginError& other) : NLMException(other) {
        this->pluginName = other.pluginName;
        this->pluginVersion = other.pluginVersion;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<PluginError>(*this);
    }
    
    const std::string& getPluginName() const { return pluginName; }
    const std::string& getPluginVersion() const { return pluginVersion; }
    
protected:
    void buildMessage() override;
    
private:
    std::string pluginName;
    std::string pluginVersion;
};

// IntegrationError for integration-related issues
class IntegrationError : public NLMException {
public:
    enum class ErrorType {
        DependencyMissing,
        ApiVersionMismatch,
        InterfaceError,
        ProtocolError,
        CompatibilityError
    };
    
    IntegrationError(const std::string& message, const std::string& dependency = "",
                     const std::string& interface = "", const std::string& file = "",
                     int line = 0, const std::string& function = "");
    
    IntegrationError(ErrorCode code, const std::string& message, const std::string& dependency = "",
                     const std::string& interface = "", const std::string& file = "",
                     int line = 0, const std::string& function = "");
    
    IntegrationError(const IntegrationError& other) : NLMException(other) {
        this->dependencyName = other.dependencyName;
        this->interfaceName = other.interfaceName;
    }
    
    std::unique_ptr<NLMException> clone() const override {
        return std::make_unique<IntegrationError>(*this);
    }
    
    const std::string& getDependencyName() const { return dependencyName; }
    const std::string& getInterfaceName() const { return interfaceName; }
    
protected:
    void buildMessage() override;
    
private:
    std::string dependencyName;
    std::string interfaceName;
};

// Exception helper functions
namespace Exception {
    // Convert error codes to strings
    const char* errorCodeToString(ErrorCode code);
    const char* errorCategoryToString(ErrorCategory category);
    const char* errorSeverityToString(ErrorSeverity severity);
    
    // Check if an exception is of a specific type
    bool isType(const NLMException& e, ErrorCategory category);
    bool isType(const NLMException& e, ErrorCode code);
    
    // Extract specific exception types
    template<typename T>
    std::unique_ptr<T> extractAs(const std::unique_ptr<NLMException>& e) {
        if (auto* derived = dynamic_cast<T*>(e.get())) {
            return std::make_unique<T>(*derived);
        }
        return nullptr;
    }
    
    // Chain exceptions
    void chain(const NLMException& cause, const std::string& message);
    
    // Report exception to logger
    void report(const NLMException& e);
}

} // namespace nlm
