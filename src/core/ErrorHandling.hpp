// Error handling framework for NLM
// This file provides comprehensive error handling utilities that can be used throughout the codebase

#pragma once

#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <sstream>
#include <chrono>
#include <thread>
#include <typeinfo>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Exception hierarchy for NLM
class NLMException : public std::runtime_error {
public:
    enum class Type {
        ConfigurationError,
        InitializationError,
        ValidationError,
        MemoryError,
        SimulationError,
        PlasticityError,
        NeuromodulationError,
        MemorySystemError,
        PredictionError,
        CognitionError,
        DevelopmentError,
        EnvironmentError,
        SerializationError,
        UnknownError
    };
    
    NLMException(const std::string& message, Type type = Type::UnknownError)
        : std::runtime_error(message), type_(type), timestamp_(std::chrono::system_clock::now()) {}
    
    Type getType() const { return type_; }
    std::string getTypeString() const;
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    std::string getFormattedTimestamp() const;
    
protected:
    Type type_;
    std::chrono::system_clock::time_point timestamp_;
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, const std::string& key = "")
        : NLMException(message, NLMException::Type::ConfigurationError), key_(key) {}
    
    std::string getKey() const { return key_; }
    
private:
    std::string key_;
};

class InitializationException : public NLMException {
public:
    InitializationException(const std::string& message, const std::string& component = "")
        : NLMException(message, NLMException::Type::InitializationError), component_(component) {}
    
    std::string getComponent() const { return component_; }
    
private:
    std::string component_;
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message, const std::string& field = "")
        : NLMException(message, NLMException::Type::ValidationError), field_(field) {}
    
    std::string getField() const { return field_; }
    
private:
    std::string field_;
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, size_t requested = 0, size_t available = 0)
        : NLMException(message, NLMException::Type::MemoryError), requested_(requested), available_(available) {}
    
    size_t getRequested() const { return requested_; }
    size_t getAvailable() const { return available_; }
    
private:
    size_t requested_;
    size_t available_;
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message, SimulationStep step = 0)
        : NLMException(message, NLMException::Type::SimulationError), step_(step) {}
    
    SimulationStep getStep() const { return step_; }
    
private:
    SimulationStep step_;
};

// Error context information
struct ErrorContext {
    std::string component;
    std::string operation;
    std::map<std::string, std::string> parameters;
    SimulationStep step;
    Timestamp timestamp;
    std::string threadId;
    
    ErrorContext() : step(0), timestamp(0.0) {
        threadId = Logger::getCurrentThreadId();
        timestamp = static_cast<Timestamp>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Component: " << component << "\n";
        oss << "Operation: " << operation << "\n";
        oss << "Step: " << step << "\n";
        oss << "Timestamp: " << timestamp << "\n";
        oss << "Thread: " << threadId << "\n";
        if (!parameters.empty()) {
            oss << "Parameters:\n";
            for (const auto& param : parameters) {
                oss << "  " << param.first << ": " << param.second << "\n";
            }
        }
        return oss.str();
    }
};

// Error handling utilities
namespace ErrorHandling {
    
    // Exception factory methods
    template<typename T, typename... Args>
    T makeException(const std::string& message, Args&&... args) {
        return T(message, std::forward<Args>(args)...);
    }
    
    // Safe execution wrapper
    template<typename T>
    Result<T> safeExecute(const std::string& operation,
                         std::function<T()> func,
                         const T& defaultValue,
                         ErrorContext context = ErrorContext()) {
        try {
            T result = func();
            Logger::getGlobal().info("Successfully completed operation: " + operation, 
                                    "", 0, "", "error_handling");
            return Result<T>(result, true, context);
        } catch (const NLMException& e) {
            std::string errorMsg = "Operation failed: " + operation + " - " + e.what();
            Logger::getGlobal().error(errorMsg, "", 0, "", "error_handling");
            return Result<T>(defaultValue, false, context);
        } catch (const std::exception& e) {
            std::string errorMsg = "Unexpected error in operation " + operation + ": " + e.what();
            Logger::getGlobal().error(errorMsg, "", 0, "", "error_handling");
            return Result<T>(defaultValue, false, context);
        } catch (...) {
            std::string errorMsg = "Unknown error in operation " + operation;
            Logger::getGlobal().error(errorMsg, "", 0, "", "error_handling");
            return Result<T>(defaultValue, false, context);
        }
    }
    
    // Validate with detailed error reporting
    template<typename T>
    bool validate(const std::string& fieldName, const T& value, 
                 std::function<bool(const T&)> validator,
                 const std::string& errorMessage) {
        if (!validator(value)) {
            Logger::getGlobal().error("Validation failed: " + fieldName + " - " + errorMessage,
                                     "", 0, "", "validation");
            return false;
        }
        return true;
    }
    
    // Range validation
    template<typename T>
    bool validateRange(const std::string& fieldName, const T& value,
                      const T& min, const T& max,
                      const std::string& errorMessage = "Value out of range") {
        if (value < min || value > max) {
            Logger::getGlobal().error("Range validation failed: " + fieldName + 
                                    " must be between " + std::to_string(min) + " and " + std::to_string(max) +
                                    " - " + errorMessage,
                                     "", 0, "", "validation");
            return false;
        }
        return true;
    }
    
    // Memory allocation safety check
    bool validateMemoryAllocation(size_t requested, size_t available,
                                 const std::string& operation = "memory_allocation") {
        if (requested > available) {
            MemoryException e("Memory allocation failed: requested " + std::to_string(requested) +
                             " bytes but only " + std::to_string(available) + " bytes available",
                             requested, available);
            Logger::getGlobal().error(e.what(), "", 0, "", "memory");
            return false;
        }
        return true;
    }
    
    // Configuration validation
    bool validateConfig(const std::map<std::string, std::string>& config,
                       const std::map<std::string, std::function<bool(const std::string&)>>& validators);
    
    // Parameter validation with type checking
    template<typename T>
    bool validateParameter(const std::string& name, const std::string& valueStr,
                          T& result,
                          std::function<bool(const T&)> validator = nullptr,
                          const std::string& typeName = "value") {
        try {
            // Simple conversion for common types
            if constexpr (std::is_same_v<T, int>) {
                result = std::stoi(valueStr);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result = std::stoll(valueStr);
            } else if constexpr (std::is_same_v<T, double>) {
                result = std::stod(valueStr);
            } else if constexpr (std::is_same_v<T, bool>) {
                result = (valueStr == "true" || valueStr == "1" || valueStr == "yes");
            } else if constexpr (std::is_same_v<T, std::string>) {
                result = valueStr;
            } else {
                Logger::getGlobal().error("Unsupported parameter type: " + typeName + " for parameter: " + name,
                                         "", 0, "", "parameter_validation");
                return false;
            }
            
            if (validator && !validator(result)) {
                Logger::getGlobal().error("Parameter validation failed for " + name + " of type " + typeName,
                                         "", 0, "", "parameter_validation");
                return false;
            }
            
            return true;
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Parameter conversion failed for " + name + ": " + e.what(),
                                     "", 0, "", "parameter_validation");
            return false;
        }
    }
    
    // Simulation step safety check
    bool validateStep(SimulationStep step, SimulationStep maxStep = 0) {
        if (step < 0) {
            Logger::getGlobal().error("Invalid simulation step: " + std::to_string(step) + " (must be non-negative)",
                                     "", 0, "", "simulation");
            return false;
        }
        if (maxStep > 0 && step > maxStep) {
            Logger::getGlobal().warning("Simulation step " + std::to_string(step) + 
                                       " exceeds maximum " + std::to_string(maxStep),
                                       "", 0, "", "simulation");
        }
        return true;
    }
}

// Exception chaining utility
class ExceptionChainer {
public:
    template<typename T>
    static T chain(const std::string& operation, std::function<T()> func, const std::string& context = "") {
        try {
            return func();
        } catch (const NLMException& e) {
            throw NLMException("Operation " + operation + " failed in context " + context + ": " + e.what(),
                              e.getType());
        } catch (const std::exception& e) {
            throw NLMException("Operation " + operation + " failed in context " + context + ": " + e.what(),
                              NLMException::Type::UnknownError);
        }
    }
};

// Error recovery utilities
namespace Recovery {
    
    // Safe reset with error handling
    bool safeReset(std::function<void()> resetFunc, const std::string& operation = "reset") {
        try {
            resetFunc();
            Logger::getGlobal().info("Successfully reset system after " + operation,
                                    "", 0, "", "recovery");
            return true;
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Failed to reset system after " + operation + ": " + e.what(),
                                     "", 0, "", "recovery");
            return false;
        }
    }
    
    // Safe cleanup with error handling
    bool safeCleanup(std::function<void()> cleanupFunc, const std::string& operation = "cleanup") {
        try {
            cleanupFunc();
            Logger::getGlobal().info("Successfully cleaned up after " + operation,
                                    "", 0, "", "recovery");
            return true;
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Failed to cleanup after " + operation + ": " + e.what(),
                                     "", 0, "", "recovery");
            return false;
        }
    }
}

// Error analysis utilities
namespace Analysis {
    
    // Analyze error patterns from log entries
    struct ErrorAnalysis {
        size_t totalErrors;
        std::map<std::string, size_t> errorsByComponent;
        std::map<std::string, size_t> errorsByOperation;
        std::chrono::system_clock::time_point firstErrorTime;
        std::chrono::system_clock::time_point lastErrorTime;
        
        std::string getSummary() const {
            std::ostringstream oss;
            oss << "Error Analysis:\n";
            oss << "  Total errors: " << totalErrors << "\n";
            oss << "  Errors by component:\n";
            for (const auto& pair : errorsByComponent) {
                oss << "    " << pair.first << ": " << pair.second << "\n";
            }
            oss << "  Errors by operation:\n";
            for (const auto& pair : errorsByOperation) {
                oss << "    " << pair.first << ": " << pair.second << "\n";
            }
            if (firstErrorTime != lastErrorTime) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(lastErrorTime - firstErrorTime).count();
                oss << "  Error duration: " << duration << " seconds\n";
                oss << "  Error rate: " << (duration > 0 ? totalErrors / duration : 0) << " errors/second\n";
            }
            return oss.str();
        }
    };
    
    // Generate error report from log statistics
    ErrorAnalysis analyzeLogStatistics(const LogStatistics& stats);
}

} // namespace nlm