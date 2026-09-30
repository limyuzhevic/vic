#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>
#include <stdexcept>

namespace nlm {

// Error types for different failure scenarios
enum class ErrorType {
    None = 0,
    Configuration,
    MemoryAllocation,
    InvalidParameter,
    Simulation,
    FileIO,
    Network,
    Plasticity,
    MemorySystem,
    Neuromodulation,
    Development,
    Prediction,
    Cognition,
    Critical
};

// Base exception class for all NLM errors
class NLMError : public std::runtime_error {
protected:
    ErrorType type;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    
public:
    NLMError(ErrorType type, const std::string& message, 
             const std::string& component = "general")
        : std::runtime_error(formatMessage(message)), 
          type(type), component(component) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorType getType() const { return type; }
    const std::string& getComponent() const { return component; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }
    
    virtual std::string getDetails() const {
        return "Error type: " + std::to_string(static_cast<int>(type)) +
               ", Component: " + component;
    }
    
protected:
    static std::string formatMessage(const std::string& message) {
        return "[NLM " + std::to_string(static_cast<int>(getCurrentTimestamp())) + "] " + message;
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }
};

// Configuration-related errors
class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message, const std::string& component = "config")
        : NLMError(ErrorType::Configuration, message, component) {}
    
    std::string getDetails() const override {
        return NLMError::getDetails() + ", Configuration error";
    }
};

// Memory-related errors
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, const std::string& component = "memory")
        : NLMError(ErrorType::MemoryAllocation, message, component) {}
    
    std::string getDetails() const override {
        return NLMError::getDetails() + ", Memory allocation error";
    }
};

// Simulation-related errors
class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message, const std::string& component = "simulation")
        : NLMError(ErrorType::Simulation, message, component) {}
    
    std::string getDetails() const override {
        return NLMError::getDetails() + ", Simulation error";
    }
};

// Plasticity-related errors
class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message, const std::string& component = "plasticity")
        : NLMError(ErrorType::Plasticity, message, component) {}
    
    std::string getDetails() const override {
        return NLMError::getDetails() + ", Plasticity error";
    }
};

// Memory system errors
class MemorySystemError : public NLMError {
public:
    MemorySystemError(const std::string& message, const std::string& component = "memory_system")
        : NLMError(ErrorType::MemorySystem, message, component) {}
    
    std::string getDetails() const override {
        return NLMError::getDetails() + ", Memory system error";
    }
};

// Advanced Exception Handling Utilities

class ExceptionHandler {
public:
    // Convert NLM exceptions to standard exceptions
    static std::exception convertToStandard(const NLMError& nlmError) {
        return nlmError;
    }
    
    // Get error type as string
    static std::string errorTypeToString(ErrorType type) {
        switch (type) {
            case ErrorType::None: return "None";
            case ErrorType::Configuration: return "Configuration";
            case ErrorType::MemoryAllocation: return "Memory Allocation";
            case ErrorType::InvalidParameter: return "Invalid Parameter";
            case ErrorType::Simulation: return "Simulation";
            case ErrorType::FileIO: return "File IO";
            case ErrorType::Network: return "Network";
            case ErrorType::Plasticity: return "Plasticity";
            case ErrorType::MemorySystem: return "Memory System";
            case ErrorType::Neuromodulation: return "Neuromodulation";
            case ErrorType::Development: return "Development";
            case ErrorType::Prediction: return "Prediction";
            case ErrorType::Cognition: return "Cognition";
            case ErrorType::Critical: return "Critical";
            default: return "Unknown";
        }
    }
    
    // Format exception with full details
    static std::string formatException(const NLMError& error) {
        std::ostringstream oss;
        oss << "[NLM Error " << errorTypeToString(error.getType()) << "] ";
        oss << error.what();
        oss << " (Component: " << error.getComponent() << ")";
        return oss.str();
    }
    
    // Check if error should be caught and recovered
    static bool isRecoverable(const NLMError& error) {
        switch (error.getType()) {
            case ErrorType::InvalidParameter:
            case ErrorType::Configuration:
                return true;
            case ErrorType::MemoryAllocation:
            case ErrorType::Critical:
                return false;
            default:
                return true;
        }
    }
};

// Error recovery utilities
class ErrorRecovery {
public:
    // Default error recovery strategy
    template<typename Callable>
    static bool tryRecover(Callable&& operation, const std::string& operationName) {
        try {
            operation();
            return true;
        } catch (const NLMError& e) {
            // Check if error is recoverable
            if (!ExceptionHandler::isRecoverable(e)) {
                throw; // Re-throw non-recoverable errors
            }
            
            // Log the error
            auto logger = Logger::getGlobal();
            logger->warning("Error in " + operationName + ": " + e.what());
            
            // Attempt recovery
            if (attemptRecovery(e)) {
                logger->info("Recovery successful for " + operationName);
                return true;
            } else {
                logger->error("Recovery failed for " + operationName);
                return false;
            }
        } catch (const std::exception& e) {
            auto logger = Logger::getGlobal();
            logger->error("Standard exception in " + operationName + ": " + e.what());
            return false;
        }
    }
    
private:
    static bool attemptRecovery(const NLMError& error) {
        switch (error.getType()) {
            case ErrorType::InvalidParameter:
                // Reset to valid default values
                return true;
            case ErrorType::Configuration:
                // Reload default configuration
                return true;
            default:
                return false;
        }
    }
};

// Error context information for debugging
struct ErrorContext {
    std::string operation;
    std::string file;
    int line;
    std::string function;
    std::string details;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext(const std::string& op, const std::string& file, int line,
                 const std::string& function, const std::string& details = "")
        : operation(op), file(file), line(line), function(function), details(details) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Exception with context
class ContextualError : public NLMError {
private:
    ErrorContext context;
    
public:
    ContextualError(ErrorType type, const std::string& message,
                    const ErrorContext& ctx, const std::string& component = "general")
        : NLMError(type, message + " (Context: " + formatContext(ctx) + ")", component),
          context(ctx) {}
    
    const ErrorContext& getContext() const { return context; }
    
    std::string getDetails() const override {
        return NLMError::getDetails() + ", Context: " + formatContext(context);
    }
    
private:
    static std::string formatContext(const ErrorContext& ctx) {
        std::ostringstream oss;
        oss << "Operation: " << ctx.operation << ", File: " << ctx.file 
            << ", Line: " << ctx.line << ", Function: " << ctx.function;
        if (!ctx.details.empty()) {
            oss << ", Details: " << ctx.details;
        }
        return oss.str();
    }
};

// Helper macro for error context
#define NLM_THROW_ERROR(error_type, message, component) \
    do { \
        ErrorContext ctx("unknown", __FILE__, __LINE__, __func__, ""); \
        throw error_type(message, component); \
    } while(0)

#define NLM_THROW_CONTEXTUAL_ERROR(error_type, message, component, op) \
    do { \
        ErrorContext ctx(op, __FILE__, __LINE__, __func__, ""); \
        throw error_type(message, ctx, component); \
    } while(0)

} // namespace nlm
