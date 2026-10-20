#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Base exception for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Config Error: " + message) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& filepath, const std::string& reason)
        : ConfigException("Failed to load config from " + filepath + ": " + reason) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& key, const std::string& expected, const std::string& actual)
        : ConfigException("Invalid value for key '" + key + "': expected " + expected + ", got " + actual) {}
};

class ConfigSchemaException : public ConfigException {
public:
    explicit ConfigSchemaException(const std::string& message) : ConfigException("Schema error: " + message) {}
};

// Brain-related exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException("Brain Error: " + message) {}
};

class BrainInitializationException : public BrainException {
public:
    explicit BrainInitializationException(const std::string& reason)
        : BrainException("Brain initialization failed: " + reason) {}
};

class SimulationStepException : public BrainException {
public:
    explicit SimulationStepException(SimulationStep step, const std::string& reason)
        : BrainException("Simulation step " + std::to_string(step) + " failed: " + reason) {}
};
};

// Memory and performance-related exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException("Memory Error: " + message) {}
};

class PerformanceException : public NLMException {
public:
    explicit PerformanceException(const std::string& message) : NLMException("Performance Error: " + message) {}
};

// Plugin-related exceptions
class PluginException : public NLMException {
public:
    explicit PluginException(const std::string& message) : NLMException("Plugin Error: " + message) {}
};

// Result type for functions that can fail
enum class ErrorCode {
    Success = 0,
    FileNotFound,
    InvalidFormat,
    OutOfMemory,
    InvalidParameter,
    InitializationFailed,
    SimulationError,
    ConfigError,
    PluginError,
    UnknownError
};

class Result {
public:
    Result() : success_(true), error_code_(ErrorCode::Success), message_("Success") {}
    Result(ErrorCode code, const std::string& msg) : success_(false), error_code_(code), message_(msg) {}
    
    bool success() const { return success_; }
    ErrorCode errorCode() const { return error_code_; }
    const std::string& message() const { return message_; }
    
    // Exception conversion for error cases
    void throwIfError() const {
        if (!success_) {
            throw NLMException(message_);
        }
    }
    
    template<typename T>
    T valueOr(const T& defaultValue) const {
        return success_ ? value_ : defaultValue;
    }
    
    template<typename T>
    void setValue(const T& val) {
        value_ = val;
    }
    
private:
    bool success_;
    ErrorCode error_code_;
    std::string message_;
    std::variant<int, double, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, bool> value_;
};

// Smart error reporting for debugging
class ErrorReporter {
public:
    static void log(const std::string& context, const std::string& operation, const std::string& error) {
        std::cerr << "[NLM Error] " << context << "." << operation << ": " << error << std::endl;
    }
    
    static std::string captureAndLog(const std::string& context, const std::string& operation, std::function<void()> operation_func) {
        try {
            operation_func();
            return "";
        } catch (const std::exception& e) {
            std::string error_msg = e.what();
            log(context, operation, error_msg);
            return error_msg;
        }
    }
};

} // namespace nlm