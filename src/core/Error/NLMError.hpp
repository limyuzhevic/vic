#pragma once

#include <string>
#include <stdexcept>
#include <system_error>

namespace nlm {

// Base error class for NLM system
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error(message), category_(ErrorCategory::Generic) {}
    
    NLMError(ErrorCategory category, const std::string& message)
        : std::runtime_error(message), category_(category) {}
    
    ErrorCategory getCategory() const noexcept { return category_; }
    
    static const char* categoryToString(ErrorCategory cat) {
        switch (cat) {
            case ErrorCategory::FileSystem: return "FileSystem";
            case ErrorCategory::Validation: return "Validation";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Threading: return "Threading";
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::Random: return "Random";
            case ErrorCategory::Simulation: return "Simulation";
            case ErrorCategory::Generic: return "Generic";
            default: return "Unknown";
        }
    }

protected:
    ErrorCategory category_;
};

// File system related errors
class FileSystemError : public NLMError {
public:
    FileSystemError(const std::string& path, const std::string& operation, const std::string& reason)
        : NLMError(ErrorCategory::FileSystem, 
                  "File system error [path: " + path + ", operation: " + operation + ", reason: " + reason + "]") {}
};

// Configuration related errors
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& key, const std::string& operation, const std::string& reason)
        : NLMError(ErrorCategory::Configuration,
                  "Configuration error [key: " + key + ", operation: " + operation + ", reason: " + reason + "]") {}
    
    ConfigError(const std::string& message)
        : NLMError(ErrorCategory::Configuration, message) {}
};

// Random number generation errors
class RandomError : public NLMError {
public:
    RandomError(const std::string& operation, const std::string& reason)
        : NLMError(ErrorCategory::Random,
                  "Random generation error [operation: " + operation + ", reason: " + reason + "]") {}
};

// Logger related errors
class LoggerError : public NLMError {
public:
    LoggerError(const std::string& operation, const std::string& reason)
        : NLMError(ErrorCategory::Generic,
                  "Logger error [operation: " + operation + ", reason: " + reason + "]") {}
};

// Simulation clock errors
class SimulationClockError : public NLMError {
public:
    SimulationClockError(const std::string& operation, const std::string& reason)
        : NLMError(ErrorCategory::Simulation,
                  "Simulation clock error [operation: " + operation + ", reason: " + reason + "]") {}
};

// Memory allocation errors
class MemoryError : public NLMError {
public:
    MemoryError(size_t size, const std::string& reason)
        : NLMError(ErrorCategory::Memory,
                  "Memory allocation error [size: " + std::to_string(size) + ", reason: " + reason + "]") {}
};

// Threading errors
class ThreadingError : public NLMError {
public:
    ThreadingError(const std::string& operation, const std::string& reason)
        : NLMError(ErrorCategory::Threading,
                  "Threading error [operation: " + operation + ", reason: " + reason + "]") {}
};

} // namespace nlm
