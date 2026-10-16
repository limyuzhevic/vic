#pragma once

// Error handling and validation utilities for NLM
// Provides comprehensive error detection, validation, and recovery

#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cassert>

namespace nlm {

// Error levels for NLM
enum class ErrorLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// NLM exception hierarchy
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorLevel level = ErrorLevel::ERROR)
        : std::runtime_error(message), level_(level) {}
    
    ErrorLevel level() const { return level_; }
    
protected:
    ErrorLevel level_;
};

class NLMValidationError : public NLMError {
public:
    NLMValidationError(const std::string& message)
        : NLMError(message, ErrorLevel::ERROR) {}
};

class NLMConfigurationError : public NLMError {
public:
    NLMConfigurationError(const std::string& message)
        : NLMError(message, ErrorLevel::ERROR) {}
};

class NLMPermissionError : public NLMError {
public:
    NLMPermissionError(const std::string& message)
        : NLMError(message, ErrorLevel::ERROR) {}
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    virtual void handleError(const std::string& message, ErrorLevel level = ErrorLevel::ERROR) = 0;
    virtual bool shouldAbortOnError(ErrorLevel level) const = 0;
};

// Console error handler
class ConsoleErrorHandler : public ErrorHandler {
public:
    void handleError(const std::string& message, ErrorLevel level = ErrorLevel::ERROR) override {
        std::string levelStr;
        switch (level) {
            case ErrorLevel::DEBUG:   levelStr = "DEBUG"; break;
            case ErrorLevel::INFO:    levelStr = "INFO"; break;
            case ErrorLevel::WARNING: levelStr = "WARNING"; break;
            case ErrorLevel::ERROR:   levelStr = "ERROR"; break;
            case ErrorLevel::CRITICAL: levelStr = "CRITICAL"; break;
        }
        
        std::cerr << "[" << levelStr << "] " << message << std::endl;
    }
    
    bool shouldAbortOnError(ErrorLevel level) const override {
        return level == ErrorLevel::CRITICAL || level == ErrorLevel::ERROR;
    }
};

// Error manager for NLM application
class ErrorManager {
public:
    ErrorManager() : errorCount_(0), warningCount_(0) {
        consoleHandler_ = std::make_unique<ConsoleErrorHandler>();
        registerHandler(consoleHandler_.get());
    }
    
    void registerHandler(ErrorHandler* handler) {
        handlers_.push_back(handler);
    }
    
    void logError(const std::string& message, ErrorLevel level = ErrorLevel::ERROR) {
        errorCount_++;
        for (auto* handler : handlers_) {
            handler->handleError(message, level);
            if (handler->shouldAbortOnError(level)) {
                throw NLMError("Critical error occurred: " + message, level);
            }
        }
    }
    
    void logWarning(const std::string& message) {
        warningCount_++;
        logError(message, ErrorLevel::WARNING);
    }
    
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }
    
    void reset() {
        errorCount_ = 0;
        warningCount_ = 0;
    }
    
    // Validation utilities
    template<typename T>
    T validate(const T& value, const std::string& context, T min = T(), T max = T()) {
        if constexpr (std::is_arithmetic_v<T>) {
            if (value < min) {
                throw NLMValidationError(context + ": value " + std::to_string(value) + " is less than minimum " + std::to_string(min));
            }
            if (value > max) {
                throw NLMValidationError(context + ": value " + std::to_string(value) + " is greater than maximum " + std::to_string(max));
            }
        }
        return value;
    }
    
    std::string validateFilePath(const std::string& path) {
        if (path.empty()) {
            throw NLMValidationError("File path cannot be empty");
        }
        
        // Check for valid file path characters
        if (path.find('\0') != std::string::npos) {
            throw NLMValidationError("File path contains null character");
        }
        
        return path;
    }
    
    template<typename T>
    std::vector<T> validateArray(const std::vector<T>& values, size_t minSize = 0) {
        if (values.size() < minSize) {
            throw NLMValidationError("Array size " + std::to_string(values.size()) + " is less than minimum " + std::to_string(minSize));
        }
        return values;
    }
    
    // Configuration validation
    class ConfigValidator {
    public:
        static void validate(const Config& config) {
            // Validate essential configuration parameters
            if (auto neuronCount = config.get<size_t>("neuron_count")) {
                if (*neuronCount == 0) {
                    throw NLMConfigurationError("neuron_count cannot be zero");
                }
                if (*neuronCount > 100000) {
                    throw NLMConfigurationError("neuron_count cannot exceed 100,000");
                }
            }
            
            if (auto timestep = config.get<double>("simulation_timestep")) {
                if (*timestep <= 0.0) {
                    throw NLMConfigurationError("simulation_timestep must be positive");
                }
                if (*timestep > 1.0) {
                    throw NLMConfigurationError("simulation_timestep cannot exceed 1.0 second");
                }
            }
            
            if (auto connectionProb = config.get<float>("connection_probability")) {
                if (*connectionProb < 0.0f || *connectionProb > 1.0f) {
                    throw NLMConfigurationError("connection_probability must be between 0.0 and 1.0");
                }
            }
        }
    };
    
    // Resource manager for validation
    class ResourceValidator {
    public:
        static bool checkMemoryAvailable(size_t requiredBytes) {
            // Simple implementation - in real scenario would check system memory
            return requiredBytes < 512 * 1024 * 1024; // 512MB limit for demonstration
        }
        
        static bool checkFileExists(const std::string& path) {
            // Implementation would check if file exists
            return !path.empty();
        }
        
        static bool checkWritePermission(const std::string& path) {
            // Implementation would check write permissions
            return true;
        }
    };
    
private:
    size_t errorCount_;
    size_t warningCount_;
    std::vector<ErrorHandler*> handlers_;
    std::unique_ptr<ErrorHandler> consoleHandler_;
};

// Global error manager instance
inline ErrorManager& getErrorManager() {
    static ErrorManager manager;
    return manager;
}

// Convenience functions for error handling
inline void NLMValidate(bool condition, const std::string& message) {
    if (!condition) {
        throw NLMValidationError(message);
    }
}

inline void NLMCheckRange(double value, double min, double max, const std::string& context) {
    if (value < min || value > max) {
        throw NLMValidationError("Value " + std::to_string(value) + " for " + context + 
                               " is out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
    }
}

inline void NLMCheckPositive(size_t value, const std::string& context) {
    if (value == 0) {
        throw NLMValidationError("Value " + std::to_string(value) + " for " + context + " must be positive");
    }
}

// RAII wrapper for error context
class ErrorContext {
public:
    ErrorContext(const std::string& operation) : operation_(operation) {
        NLM_LOG_INFO("Starting operation: " + operation_);
    }
    
    ~ErrorContext() {
        NLM_LOG_INFO("Completed operation: " + operation_);
    }
    
    void validateFileRead(const std::string& path) {
        getErrorManager().validateFilePath(path);
        // Additional file validation would go here
    }
    
    void validateConfiguration(const Config& config) {
        ErrorManager::ConfigValidator::validate(config);
    }
    
    template<typename T>
    T safeOperation(const std::string& operation, std::function<T()> func) {
        try {
            T result = func();
            NLM_LOG_INFO("Successfully completed: " + operation);
            return result;
        } catch (const NLMError& e) {
            NLM_LOG_ERROR("Operation failed: " + operation + " - " + e.what());
            throw;
        }
    }
    
private:
    std::string operation_;
};

} // namespace nlm