#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    Generic,
    Memory,
    Validation,
    Configuration,
    Neural,
    Motor,
    Sensor,
    Plasticity,
    Development,
    System
};

// Custom exception classes
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCategory category = ErrorCategory::Generic)
        : std::runtime_error(message), category_(category) {}
    
    ErrorCategory getCategory() const { return category_; }
    
private:
    ErrorCategory category_;
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message) : NLMException(message, ErrorCategory::Memory) {}
};

class ValidationError : public NLMException {
public:
    ValidationError(const std::string& message) : NLMException(message, ErrorCategory::Validation) {}
};

class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message) : NLMException(message, ErrorCategory::Configuration) {}
};

class NeuralError : public NLMException {
public:
    NeuralError(const std::string& message) : NLMException(message, ErrorCategory::Neural) {}
};

class MotorError : public NLMException {
public:
    MotorError(const std::string& message) : NLMException(message, ErrorCategory::Motor) {}
};

class SensorError : public NLMException {
public:
    SensorError(const std::string& message) : NLMException(message, ErrorCategory::Sensor) {}
};

// Error result structure for non-exception errors
struct ErrorResult {
    bool success;
    std::string message;
    ErrorCategory category;
    
    ErrorResult() : success(true), message(""), category(ErrorCategory::Generic) {}
    ErrorResult(bool s, const std::string& msg, ErrorCategory cat = ErrorCategory::Generic)
        : success(s), message(msg), category(cat) {}
    
    bool isOk() const { return success; }
    bool isError() const { return !success; }
};

// Validation utility functions
class ValidationUtils {
public:
    // Validate parameter ranges
    template<typename T>
    static ErrorResult validateRange(T value, T min, T max, const std::string& paramName) {
        if (value < min || value > max) {
            return ErrorResult(false, 
                "Parameter '" + paramName + "' value " + std::to_string(value) +
                " is out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]", 
                ErrorCategory::Validation);
        }
        return ErrorResult(true, "");
    }
    
    // Validate pointer is not null
    template<typename T>
    static ErrorResult validateNotNull(T* ptr, const std::string& paramName) {
        if (!ptr) {
            return ErrorResult(false, "Parameter '" + paramName + "' cannot be null", 
                             ErrorCategory::Validation);
        }
        return ErrorResult(true, "");
    }
    
    // Validate container is not empty
    template<typename Container>
    static ErrorResult validateNotEmpty(const Container& container, const std::string& paramName) {
        if (container.empty()) {
            return ErrorResult(false, "Parameter '" + paramName + "' cannot be empty", 
                             ErrorCategory::Validation);
        }
        return ErrorResult(true, "");
    }
    
    // Validate neuron parameters
    static ErrorResult validateNeuronParameters(float vThresh, float vRest, float vReset,
                                                 float tauMem, float tauRef) {
        std::vector<ErrorResult> errors;
        
        auto err1 = validateRange(vThresh, -100.0f, -30.0f, "vThresh");
        if (err1.isError()) errors.push_back(err1);
        
        auto err2 = validateRange(vRest, -80.0f, -60.0f, "vRest");
        if (err2.isError()) errors.push_back(err2);
        
        auto err3 = validateRange(vReset, -80.0f, -70.0f, "vReset");
        if (err3.isError()) errors.push_back(err3);
        
        auto err4 = validateRange(tauMem, 1.0f, 50.0f, "tauMem");
        if (err4.isError()) errors.push_back(err4);
        
        auto err5 = validateRange(tauRef, 0.1f, 10.0f, "tauRef");
        if (err5.isError()) errors.push_back(err5);
        
        if (!errors.empty()) {
            std::string combinedMsg = "Neuron parameter validation failed:\n";
            for (const auto& e : errors) {
                combinedMsg += "  " + e.message + "\n";
            }
            return ErrorResult(false, combinedMsg, ErrorCategory::Validation);
        }
        
        return ErrorResult(true, "");
    }
    
    // Validate synaptic connection parameters
    static ErrorResult validateSynapticParameters(float weight, float delay, float learningRate) {
        std::vector<ErrorResult> errors;
        
        auto err1 = validateRange(weight, -10.0f, 10.0f, "weight");
        if (err1.isError()) errors.push_back(err1);
        
        auto err2 = validateRange(delay, 0.0f, 1000.0f, "delay");
        if (err2.isError()) errors.push_back(err2);
        
        auto err3 = validateRange(learningRate, 0.0f, 1.0f, "learningRate");
        if (err3.isError()) errors.push_back(err3);
        
        if (!errors.empty()) {
            std::string combinedMsg = "Synaptic parameter validation failed:\n";
            for (const auto& e : errors) {
                combinedMsg += "  " + e.message + "\n";
            }
            return ErrorResult(false, combinedMsg, ErrorCategory::Validation);
        }
        
        return ErrorResult(true, "");
    }
    
    // Validate memory allocation
    static ErrorResult validateMemoryAllocation(size_t size, const std::string& context) {
        if (size == 0) {
            return ErrorResult(false, "Cannot allocate zero memory in context: " + context, 
                             ErrorCategory::Memory);
        }
        if (size > 1000000000) { // 1GB limit
            return ErrorResult(false, "Memory allocation too large: " + std::to_string(size) +
                             " bytes in context: " + context, ErrorCategory::Memory);
        }
        return ErrorResult(true, "");
    }
    
    // Assert with detailed error reporting
    static void assertCondition(bool condition, const std::string& message, ErrorCategory category = ErrorCategory::Validation) {
        if (!condition) {
            throw NLMException(message, category);
        }
    }
    
    // Assert with logging
    static void assertCondition(bool condition, const std::string& message, 
                                const std::string& logMessage, ErrorCategory category = ErrorCategory::Validation) {
        if (!condition) {
            NLM_LOG_ERROR(logMessage);
            throw NLMException(message, category);
        }
    }
    
    // Safe operation with error result
    template<typename Func>
    static ErrorResult safeOperation(Func func, const std::string& errorContext = "") {
        try {
            func();
            return ErrorResult(true, "");
        } catch (const NLMException& e) {
            return ErrorResult(false, errorContext + ": " + e.what(), e.getCategory());
        } catch (const std::exception& e) {
            return ErrorResult(false, errorContext + ": " + std::string(e.what()), 
                             ErrorCategory::Generic);
        } catch (...) {
            return ErrorResult(false, errorContext + ": Unknown error", 
                             ErrorCategory::Generic);
        }
    }
};

// Logging utility for errors
class ErrorLogger {
public:
    static void logError(const ErrorResult& error) {
        switch (error.category) {
            case ErrorCategory::Memory:
                NLM_LOG_ERROR("Memory Error: " + error.message);
                break;
            case ErrorCategory::Validation:
                NLM_LOG_ERROR("Validation Error: " + error.message);
                break;
            case ErrorCategory::Configuration:
                NLM_LOG_ERROR("Configuration Error: " + error.message);
                break;
            case ErrorCategory::Neural:
                NLM_LOG_ERROR("Neural Error: " + error.message);
                break;
            case ErrorCategory::Motor:
                NLM_LOG_ERROR("Motor Error: " + error.message);
                break;
            case ErrorCategory::Sensor:
                NLM_LOG_ERROR("Sensor Error: " + error.message);
                break;
            case ErrorCategory::Plasticity:
                NLM_LOG_ERROR("Plasticity Error: " + error.message);
                break;
            case ErrorCategory::Development:
                NLM_LOG_ERROR("Development Error: " + error.message);
                break;
            case ErrorCategory::System:
                NLM_LOG_ERROR("System Error: " + error.message);
                break;
            default:
                NLM_LOG_ERROR("Error: " + error.message);
                break;
        }
    }
    
    static void logError(const NLMException& error) {
        switch (error.getCategory()) {
            case ErrorCategory::Memory:
                NLM_LOG_ERROR("Memory Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Validation:
                NLM_LOG_ERROR("Validation Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Configuration:
                NLM_LOG_ERROR("Configuration Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Neural:
                NLM_LOG_ERROR("Neural Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Motor:
                NLM_LOG_ERROR("Motor Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Sensor:
                NLM_LOG_ERROR("Sensor Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Plasticity:
                NLM_LOG_ERROR("Plasticity Error: " + std::string(error.what()));
                break;
            case ErrorCategory::Development:
                NLM_LOG_ERROR("Development Error: " + std::string(error.what()));
                break;
            case ErrorCategory::System:
                NLM_LOG_ERROR("System Error: " + std::string(error.what()));
                break;
            default:
                NLM_LOG_ERROR("Error: " + std::string(error.what()));
                break;
        }
    }
};

} // namespace nlm