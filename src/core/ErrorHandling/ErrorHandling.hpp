#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <any>
#include <memory>

namespace nlm {

// Base exception for all NLM errors
class NLMException : public std::runtime_error {
protected:
    std::string filename_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message), filename_(""), line_(0), function_(""),
          timestamp_(std::chrono::system_clock::now()) {}
    
    NLMException(const std::string& message, const std::string& filename, 
                 int line, const std::string& function)
        : std::runtime_error(message), filename_(filename), line_(line), 
          function_(function), timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~NLMException() = default;
    
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
    std::string getFullMessage() const {
        std::string result = std::runtime_error::what();
        if (!filename_.empty()) {
            result += " (at " + filename_;
            if (line_ > 0) result += ":" + std::to_string(line_);
            if (!function_.empty()) result += " in " + function_;
            result += ")";
        }
        return result;
    }
    
    const std::string& getFilename() const { return filename_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
};

// System errors (initialization, configuration, memory)
class SystemError : public NLMException {
public:
    explicit SystemError(const std::string& message) : NLMException(message) {}
    SystemError(const std::string& message, const std::string& filename, 
                int line, const std::string& function) 
        : NLMException(message, filename, line, function) {}
};

class InitializationError : public SystemError {
public:
    explicit InitializationError(const std::string& message) : SystemError(message) {}
    InitializationError(const std::string& message, const std::string& filename, 
                        int line, const std::string& function)
        : SystemError(message, filename, line, function) {}
};

class ConfigurationError : public SystemError {
public:
    explicit ConfigurationError(const std::string& message) : SystemError(message) {}
    ConfigurationError(const std::string& message, const std::string& filename, 
                      int line, const std::string& function)
        : SystemError(message, filename, line, function) {}
};

class MemoryError : public SystemError {
public:
    explicit MemoryError(const std::string& message) : SystemError(message) {}
    MemoryError(const std::string& message, const std::string& filename, 
                int line, const std::string& function)
        : SystemError(message, filename, line, function) {}
};

// Neural errors (neuron dynamics, synaptic operations)
class NeuralError : public NLMException {
public:
    explicit NeuralError(const std::string& message) : NLMException(message) {}
    NeuralError(const std::string& message, const std::string& filename, 
                int line, const std::string& function)
        : NLMException(message, filename, line, function) {}
};

class NeuronError : public NeuralError {
public:
    explicit NeuronError(const std::string& message) : NeuralError(message) {}
    NeuronError(const std::string& message, const std::string& filename, 
                int line, const std::string& function)
        : NeuralError(message, filename, line, function) {}
};

class SynapseError : public NeuralError {
public:
    explicit SynapseError(const std::string& message) : NeuralError(message) {}
    SynapseError(const std::string& message, const std::string& filename, 
                 int line, const std::string& function)
        : NeuralError(message, filename, line, function) {}
};

// Memory errors (working memory, episodic memory)
class WorkingMemoryError : public MemoryError {
public:
    explicit WorkingMemoryError(const std::string& message) : MemoryError(message) {}
    WorkingMemoryError(const std::string& message, const std::string& filename, 
                      int line, const std::string& function)
        : MemoryError(message, filename, line, function) {}
};

class EpisodicMemoryError : public MemoryError {
public:
    explicit EpisodicMemoryError(const std::string& message) : MemoryError(message) {}
    EpisodicMemoryError(const std::string& message, const std::string& filename, 
                       int line, const std::string& function)
        : MemoryError(message, filename, line, function) {}
};

// Integration errors (brain loop, agent integration)
class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message) : NLMException(message) {}
    IntegrationError(const std::string& message, const std::string& filename, 
                     int line, const std::string& function)
        : NLMException(message, filename, line, function) {}
};

class BrainLoopError : public IntegrationError {
public:
    explicit BrainLoopError(const std::string& message) : IntegrationError(message) {}
    BrainLoopError(const std::string& message, const std::string& filename, 
                   int line, const std::string& function)
        : IntegrationError(message, filename, line, function) {}
};

// Persistence errors (save/load, checkpoint management)
class PersistenceError : public NLMException {
public:
    explicit PersistenceError(const std::string& message) : NLMException(message) {}
    PersistenceError(const std::string& message, const std::string& filename, 
                     int line, const std::string& function)
        : NLMException(message, filename, line, function) {}
};

class CheckpointError : public PersistenceError {
public:
    explicit CheckpointError(const std::string& message) : PersistenceError(message) {}
    CheckpointError(const std::string& message, const std::string& filename, 
                    int line, const std::string& function)
        : PersistenceError(message, filename, line, function) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& message) : NLMException(message) {}
    ValidationError(const std::string& message, const std::string& filename, 
                    int line, const std::string& function)
        : NLMException(message, filename, line, function) {}
};

// Error recovery types
enum class RecoveryAction {
    None,          // No recovery possible
    Retry,         // Can retry the operation
    Fallback,      // Use alternative/better mode
    Skip,          // Skip this operation
    GracefulDegrade, // Degrade to simpler working mode
};

// Error metrics for performance monitoring
struct ErrorMetrics {
    size_t systemErrors = 0;
    size_t neuralErrors = 0;
    size_t memoryErrors = 0;
    size_t integrationErrors = 0;
    size_t persistenceErrors = 0;
    size_t validationErrors = 0;
    float recoverySuccessRate = 0.0f;
    double totalTimeHandlingErrors = 0.0;
    
    void increment(const std::string& errorType, double timeSpent = 0.0) {
        if (errorType.find("System") != std::string::npos) systemErrors++;
        else if (errorType.find("Neural") != std::string::npos) neuralErrors++;
        else if (errorType.find("Memory") != std::string::npos) memoryErrors++;
        else if (errorType.find("Integration") != std::string::npos) integrationErrors++;
        else if (errorType.find("Persistence") != std::string::npos) persistenceErrors++;
        else if (errorType.find("Validation") != std::string::npos) validationErrors++;
        
        totalTimeHandlingErrors += timeSpent;
    }
    
    size_t getTotalErrors() const {
        return systemErrors + neuralErrors + memoryErrors + 
               integrationErrors + persistenceErrors + validationErrors;
    }
    
    float getErrorRate() const {
        // This would need a total count for a complete error rate
        return getTotalErrors() > 0 ? 1.0f : 0.0f;
    }
};

// Global error metrics
ErrorMetrics& getGlobalErrorMetrics();

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual RecoveryAction handleError(const NLMException& error) = 0;
    virtual void logError(const NLMException& error) = 0;
    virtual void setErrorLevel(LogLevel level) = 0;
};

// Default error handler (logs and optionally recovers)
class DefaultErrorHandler : public IErrorHandler {
public:
    DefaultErrorHandler();
    
    RecoveryAction handleError(const NLMException& error) override;
    void logError(const NLMException& error) override;
    void setErrorLevel(LogLevel level) override;
    
    void setRecoveryEnabled(bool enabled) { recoveryEnabled_ = enabled; }
    bool isRecoveryEnabled() const { return recoveryEnabled_; }
    
    void setFallbackMode(bool enabled) { fallbackMode_ = enabled; }
    bool isFallbackMode() const { return fallbackMode_; }
    
    void setGracefulDegradation(bool enabled) { gracefulDegradation_ = enabled; }
    bool isGracefulDegradation() const { return gracefulDegradation_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool recoveryEnabled_;
    bool fallbackMode_;
    bool gracefulDegradation_;
    
    // Error recovery logic
    RecoveryAction recoverFromSystemError(const SystemError& error);
    RecoveryAction recoverFromNeuralError(const NeuralError& error);
    RecoveryAction recoverFromMemoryError(const MemoryError& error);
    RecoveryAction recoverFromIntegrationError(const IntegrationError& error);
    RecoveryAction recoverFromPersistenceError(const PersistenceError& error);
    RecoveryAction recoverFromValidationError(const ValidationError& error);
};

// Global error handler management
void setGlobalErrorHandler(std::shared_ptr<IErrorHandler> handler);
std::shared_ptr<IErrorHandler> getGlobalErrorHandler();

// Convenience macros for error reporting
#define NLM_THROW_SYSTEM_ERROR(msg) throw SystemError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_INITIALIZATION_ERROR(msg) throw InitializationError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_CONFIGURATION_ERROR(msg) throw ConfigurationError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_MEMORY_ERROR(msg) throw MemoryError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_WORKING_MEMORY_ERROR(msg) throw WorkingMemoryError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_NEURAL_ERROR(msg) throw NeuralError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_NEURON_ERROR(msg) throw NeuronError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_SYNAPSE_ERROR(msg) throw SynapseError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_EpisodicMemoryError(msg) throw EpisodicMemoryError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_INTEGRATION_ERROR(msg) throw IntegrationError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_BRAIN_LOOP_ERROR(msg) throw BrainLoopError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_PERSISTENCE_ERROR(msg) throw PersistenceError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_CHECKPOINT_ERROR(msg) throw CheckpointError(msg, __FILE__, __LINE__, __func__)
#define NLM_THROW_VALIDATION_ERROR(msg) throw ValidationError(msg, __FILE__, __LINE__, __func__)

// Validation function template
class Validator {
public:
    template<typename T>
    static bool validate(const T& value, const std::string& name, 
                        const std::function<bool(const T&)>& condition,
                        const std::string& errorMsg = "") {
        if (!condition(value)) {
            if (!errorMsg.empty()) {
                NLM_THROW_VALIDATION_ERROR(name + ": " + errorMsg);
            } else {
                NLM_THROW_VALIDATION_ERROR(name + ": validation failed");
            }
        }
        return true;
    }
    
    template<typename T>
    static bool validateRange(const T& value, const T& min, const T& max, 
                             const std::string& name = "value",
                             const std::string& errorMsg = "") {
        if (!(value >= min && value <= max)) {
            std::string msg = name + ": " + std::to_string(value) + 
                            " not in range [" + std::to_string(min) + ", " + 
                            std::to_string(max) + "]";
            if (!errorMsg.empty()) msg += ": " + errorMsg;
            NLM_THROW_VALIDATION_ERROR(msg);
        }
        return true;
    }
    
    template<typename T>
    static bool validateNotNull(const T* ptr, const std::string& name = "pointer") {
        if (!ptr) {
            NLM_THROW_VALIDATION_ERROR(name + ": cannot be null");
        }
        return true;
    }
    
    template<typename T>
    static bool validateCapacity(size_t current, size_t capacity,
                                 const std::string& name = "container") {
        if (current > capacity) {
            NLM_THROW_VALIDATION_ERROR(name + ": size (" + std::to_string(current) +
                                      ") exceeds capacity (" + std::to_string(capacity) + ")");
        }
        return true;
    }
};

} // namespace nlm
