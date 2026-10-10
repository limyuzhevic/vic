#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <format>
#include <vector>
#include <chrono>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                  const std::string& file = "", 
                  int line = 0, 
                  const std::string& function = "")
        : std::runtime_error(formatMessage(message, file, line, function)),
          file_(file), line_(line), function_(function), timestamp_(std::chrono::system_clock::now()) {}
    
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }
    
protected:
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    
private:
    std::string formatMessage(const std::string& message, 
                             const std::string& file, 
                             int line, 
                             const std::string& function) {
        std::string result = message;
        if (!file.empty() && line > 0) {
            result += " (" + file;
            if (!function.empty()) {
                result += "::" + function;
            }
            result += ":" + std::to_string(line) + ")";
        }
        return result;
    }
};

// Configuration-related exceptions
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message, 
                const std::string& file = "", 
                int line = 0, 
                const std::string& function = "")
        : NLMException("Config error: " + message, file, line, function) {}
};

class ConfigParseError : public ConfigError {
public:
    ConfigParseError(const std::string& message, 
                     const std::string& file = "", 
                     int line = 0, 
                     const std::string& function = "")
        : ConfigError("Parse error: " + message, file, line, function) {}
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, 
                         const std::string& file = "", 
                         int line = 0, 
                         const std::string& function = "")
        : ConfigError("Validation error: " + message, file, line, function) {}
};

// Brain-related exceptions
class BrainError : public NLMException {
public:
    BrainError(const std::string& message, 
               const std::string& file = "", 
               int line = 0, 
               const std::string& function = "")
        : NLMException("Brain error: " + message, file, line, function) {}
};

class NeuronError : public BrainError {
public:
    NeuronError(const std::string& message, 
                const std::string& file = "", 
                int line = 0, 
                const std::string& function = "")
        : BrainError("Neuron error: " + message, file, line, function) {}
};

class SynapseError : public BrainError {
public:
    SynapseError(const std::string& message, 
                 const std::string& file = "", 
                 int line = 0, 
                 const std::string& function = "")
        : BrainError("Synapse error: " + message, file, line, function) {}
};

// Memory system exceptions
class MemoryError : public BrainError {
public:
    MemoryError(const std::string& message, 
                const std::string& file = "", 
                int line = 0, 
                const std::string& function = "")
        : BrainError("Memory error: " + message, file, line, function) {}
};

// Simulation errors
class SimulationError : public BrainError {
public:
    SimulationError(const std::string& message, 
                    const std::string& file = "", 
                    int line = 0, 
                    const std::string& function = "")
        : BrainError("Simulation error: " + message, file, line, function) {}
};

// Error reporting and handling utilities
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void addErrorHandler(std::function<void(const NLMException&)>) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.push_back(std::move(handler));
    }
    
    void logError(const NLMException& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Log to all registered handlers
        for (auto& handler : handlers_) {
            try {
                handler(error);
            } catch (const std::exception& e) {
                // If handler throws, log to stderr as fallback
                std::cerr << "Error handler threw exception: " << e.what() << std::endl;
            }
        }
        
        // Default fallback logging
        std::cerr << "NLM Error: " << error.what() << std::endl;
    }
    
    void setLogLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        logLevel_ = level;
    }
    
    LogLevel getLogLevel() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return logLevel_;
    }
    
private:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    std::vector<std::function<void(const NLMException&)>> handlers_;
    std::mutex mutex_;
    LogLevel logLevel_ = LogLevel::Error;
};

// Exception-safe wrapper for operations that can throw
class ExceptionSafe {
public:
    template<typename T, typename Func>
    static T executeSafely(Func&& func, const std::string& operationName) {
        try {
            return func();
        } catch (const ConfigError& e) {
            // Re-throw configuration errors as-is
            throw;
        } catch (const BrainError& e) {
            // Re-throw brain errors as-is
            throw;
        } catch (const std::exception& e) {
            // Wrap other exceptions in a BrainError
            throw BrainError(std::format("Operation '{}' failed: {}", operationName, e.what()));
        }
    }
};

// Macros for consistent error handling
#define NLM_THROW_ERROR(errorType, message) \
    throw errorType(message, __FILE__, __LINE__, __func__)

#define NLM_THROW_CONFIG_ERROR(message) NLM_THROW_ERROR(nlm::ConfigError, message)
#define NLM_THROW_BRAIN_ERROR(message) NLM_THROW_ERROR(nlm::BrainError, message)
#define NLM_THROW_NEURON_ERROR(message) NLM_THROW_ERROR(nlm::NeuronError, message)
#define NLM_THROW_SYNAPSE_ERROR(message) NLM_THROW_ERROR(nlm::SynapseError, message)
#define NLM_THROW_MEMORY_ERROR(message) NLM_THROW_ERROR(nlm::MemoryError, message)
#define NLM_THROW_SIMULATION_ERROR(message) NLM_THROW_ERROR(nlm::SimulationError, message)

// Function to get the current error handler
inline ErrorHandler& getErrorHandler() {
    return ErrorHandler::getInstance();
}

// Function to handle an exception and log it
inline void handleException(const std::exception& e, const std::string& operationName) {
    try {
        throw; // Re-throw to get the full exception chain
    } catch (const NLMException& nlmError) {
        getErrorHandler().logError(nlmError);
    } catch (const std::exception& stdError) {
        NLMException wrapper("Unexpected error: " + std::string(stdError.what()), __FILE__, __LINE__, __func__);
        getErrorHandler().logError(wrapper);
    }
}

} // namespace nlm
