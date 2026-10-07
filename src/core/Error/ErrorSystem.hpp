#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Enhanced error hierarchy for NLM
class NLMError : public std::runtime_error {
public:
    enum class ErrorType {
        // Core system errors
        Unknown,
        MemoryAllocationError,
        FileSystemError,
        ConfigurationError,
        
        // Neural computation errors
        NeuralInitializationError,
        ConnectionError,
        PlasticityError,
        SimulationError,
        
        // Validation errors
        InputValidationError,
        RuntimeValidationError,
        CheckpointError,
        StateError,
        
        // Performance and resource errors
        ResourceExhaustionError,
        PerformanceError,
        TimeoutError,
        
        // Platform-specific errors
        PlatformError,
        
        // Development errors
        DevelopmentError
    };

protected:
    ErrorType type;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> errorChain;
    
public:
    NLMError(ErrorType type, const std::string& message, 
             const std::string& context = "", 
             const std::vector<std::string>& chain = {})
        : std::runtime_error(formatMessage(message, context)),
          type(type),
          context(context),
          timestamp(std::chrono::system_clock::now()),
          errorChain(chain) {}
    
    ErrorType getType() const { return type; }
    const std::string& getContext() const { return context; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    const std::vector<std::string>& getErrorChain() const { return errorChain; }
    
    virtual std::string getErrorCode() const {
        return "NLM_ERROR_" + std::to_string(static_cast<int>(type));
    }
    
    virtual std::string getErrorDetails() const {
        std::ostringstream oss;
        oss << "Error Code: " << getErrorCode() << "\n";
        oss << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp) << "\n";
        if (!context.empty()) {
            oss << "Context: " << context << "\n";
        }
        if (!errorChain.empty()) {
            oss << "Error Chain:\n";
            for (const auto& chainError : errorChain) {
                oss << "  - " << chainError << "\n";
            }
        }
        return oss.str();
    }
    
    virtual bool isRecoverable() const { return false; }
    virtual bool isCritical() const { return false; }
    
    virtual std::string suggestRecovery() const {
        return "Check logs for detailed error information and retry operation.";
    }
    
    std::string what() const override {
        return std::runtime_error::what();
    }
    
protected:
    static std::string formatMessage(const std::string& message, const std::string& context) {
        std::ostringstream oss;
        if (!context.empty()) {
            oss << "[Context: " << context << "] ";
        }
        oss << message;
        return oss.str();
    }
};

// Specific error types for different scenarios

// Configuration-related errors
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::ConfigurationError, message, context) {}
    
    virtual bool isRecoverable() const override { return true; }
    virtual std::string suggestRecovery() const override {
        return "Check configuration file syntax and required keys.";
    }
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::ValidationError, message, context) {}
    
    virtual bool isRecoverable() const override { return true; }
    virtual std::string suggestRecovery() const override {
        return "Fix input values or parameters according to validation rules.";
    }
};

class InputValidationError : public ValidationError {
public:
    InputValidationError(const std::string& message, const std::string& context = "")
        : ValidationError(message, context) {
        this->type = ErrorType::InputValidationError;
    }
};

class RuntimeValidationError : public ValidationError {
public:
    RuntimeValidationError(const std::string& message, const std::string& context = "")
        : ValidationError(message, context) {
        this->type = ErrorType::RuntimeValidationError;
    }
};

// Neural computation errors
class NeuralError : public NLMError {
public:
    NeuralError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::NeuralInitializationError, message, context) {}
};

class PlasticityError : public NeuralError {
public:
    PlasticityError(const std::string& message, const std::string& context = "")
        : NeuralError(message, context) {
        this->type = ErrorType::PlasticityError;
    }
};

class SimulationError : public NeuralError {
public:
    SimulationError(const std::string& message, const std::string& context = "")
        : NeuralError(message, context) {
        this->type = ErrorType::SimulationError;
    }
};

// Resource and performance errors
class ResourceError : public NLMError {
public:
    ResourceError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::ResourceExhaustionError, message, context) {}
    
    virtual bool isRecoverable() const override { return true; }
    virtual bool isCritical() const override { return true; }
    virtual std::string suggestRecovery() const override {
        return "Free up memory or resources and retry the operation.";
    }
};

class MemoryError : public ResourceError {
public:
    MemoryError(const std::string& message, const std::string& context = "")
        : ResourceError(message, context) {
        this->type = ErrorType::MemoryError;
    }
};

class PerformanceError : public ResourceError {
public:
    PerformanceError(const std::string& message, const std::string& context = "")
        : ResourceError(message, context) {
        this->type = ErrorType::PerformanceError;
    }
};

// Platform and system errors
class PlatformError : public NLMError {
public:
    PlatformError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::PlatformError, message, context) {}
    
    virtual std::string suggestRecovery() const override {
        return "Check system compatibility and platform-specific requirements.";
    }
};

// Development and debugging errors
class DevelopmentError : public NLMError {
public:
    DevelopmentError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::DevelopmentError, message, context) {}
    
    virtual bool isRecoverable() const override { return true; }
    virtual std::string suggestRecovery() const override {
        return "Review development code and fix logical errors.";
    }
};

class CheckpointError : public NLMError {
public:
    CheckpointError(const std::string& message, const std::string& context = "")
        : NLMError(ErrorType::CheckpointError, message, context) {}
    
    virtual bool isRecoverable() const override { return true; }
    virtual std::string suggestRecovery() const override {
        return "Check disk space, file permissions, and checkpoint integrity.";
    }
};

// Global error reporting utilities
class ErrorHandler {
public:
    static std::vector<std::string> errorHistory;
    static std::mutex historyMutex;
    static std::function<void(const NLMError&)> errorHandler;
    static std::function<bool(const NLMError&)> errorRecoveryHandler;
    
    static void initialize() {
        errorHistory.clear();
    }
    
    static void report(const NLMError& error) {
        std::lock_guard<std::mutex> lock(historyMutex);
        
        errorHistory.push_back(error.what());
        
        if (errorHandler) {
            errorHandler(error);
        }
    }
    
    static std::string getLastError() {
        std::lock_guard<std::mutex> lock(historyMutex);
        if (errorHistory.empty()) return "";
        return errorHistory.back();
    }
    
    static size_t getErrorCount() {
        std::lock_guard<std::mutex> lock(historyMutex);
        return errorHistory.size();
    }
    
    static void clearHistory() {
        std::lock_guard<std::mutex> lock(historyMutex);
        errorHistory.clear();
    }
    
    static bool attemptRecovery(const NLMError& error) {
        if (errorRecoveryHandler) {
            return errorRecoveryHandler(error);
        }
        return error.isRecoverable();
    }
    
    static std::string getFormattedErrorHistory() {
        std::lock_guard<std::mutex> lock(historyMutex);
        std::ostringstream oss;
        oss << "Error History (" << errorHistory.size() << " errors):\n";
        for (size_t i = 0; i < errorHistory.size(); ++i) {
            oss << "  " << (i + 1) << ". " << errorHistory[i] << "\n";
        }
        return oss.str();
    }
};

// Macro for easy error throwing with context
#define THROW_NLM_ERROR(type, message, context) \
    do { \
        ::nlm::ErrorHandler::report(::nlm::type(message, context)); \
        throw ::nlm::type(message, context); \
    } while(0)

#define THROW_NLM_ERROR_CHAIN(type, message, context, chain) \
    do { \
        ::nlm::ErrorHandler::report(::nlm::type(message, context, chain)); \
        throw ::nlm::type(message, context, chain); \
    } while(0)

// Exception translator for different platforms (to be implemented per platform)
namespace Platform {
    template<typename T>
    T translateException(const std::exception& e) {
        throw e; // Default implementation - rethrow
    }
}

// Error recovery utilities
namespace ErrorRecovery {
    template<typename Operation>
    bool safeExecute(const Operation& op, const std::string& operationName = "operation") {
        try {
            op();
            return true;
        } catch (const NLMError& e) {
            if (ErrorHandler::attemptRecovery(e)) {
                try {
                    op();
                    return true;
                } catch (...) {
                    return false;
                }
            }
            return false;
        } catch (...) {
            return false;
        }
    }
    
    std::string getDiagnosticInfo(const NLMError& error) {
        std::ostringstream oss;
        oss << "Error: " << error.what() << "\n";
        oss << "Code: " << error.getErrorCode() << "\n";
        oss << "Type: " << static_cast<int>(error.getType()) << "\n";
        oss << "Recoverable: " << (error.isRecoverable() ? "Yes" : "No") << "\n";
        oss << "Critical: " << (error.isCritical() ? "Yes" : "No") << "\n";
        oss << "Suggestion: " << error.suggestRecovery() << "\n";
        oss << "Details:\n" << error.getErrorDetails() << "\n";
        return oss.str();
    }
}

} // namespace nlm