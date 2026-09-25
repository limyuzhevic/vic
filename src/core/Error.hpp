#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <sstream>

namespace nlm {

// Error category enumeration
enum class ErrorCategory {
    None,
    Config,
    Memory,
    FileIO,
    Validation,
    Brain,
    System,
    Network,
    DataCorruption,
    Initialization,
    Shutdown,
    ResourceNotFound,
    PermissionDenied,
    Timeout,
    InvalidState,
    InvalidParameter,
    TypeConversion,
    Plasticity,
    MemoryManagement,
    NeuralDynamics,
    Simulation,
    Visualization,
    Experiment,
    Device,
    Performance,
    Custom
};

// Base exception with category support
class NLMError : public std::exception {
protected:
    ErrorCategory category_;
    std::string message_;
    std::vector<std::string> context_;
    std::shared_ptr<NLMError> cause_;
    
public:
    explicit NLMError(const std::string& message, 
                     ErrorCategory category = ErrorCategory::None)
        : category_(category), message_(message) {}
    
    NLMError(const std::string& message, 
             const std::shared_ptr<NLMError>& cause,
             ErrorCategory category = ErrorCategory::None)
        : category_(category), message_(message), cause_(cause) {}
    
    virtual ~NLMError() noexcept = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    ErrorCategory getCategory() const noexcept { return category_; }
    
    const std::string& getMessage() const noexcept { return message_; }
    
    const std::shared_ptr<NLMError>& getCause() const noexcept { return cause_; }
    
    void addContext(const std::string& context) {
        context_.push_back(context);
    }
    
    const std::vector<std::string>& getContext() const noexcept { return context_; }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << message_;
        for (const auto& ctx : context_) {
            oss << " | " << ctx;
        }
        if (cause_) {
            oss << " | caused by: " << cause_->what();
        }
        return oss.str();
    }
};

// Specialized error classes
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message, 
                        const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::Config) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message, 
                        const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::Memory) {}
};

class FileIOError : public NLMError {
public:
    explicit FileIOError(const std::string& message, 
                        const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::FileIO) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message, 
                            const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::Validation) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message, 
                       const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::Brain) {}
};

class SystemError : public NLMError {
public:
    explicit SystemError(const std::string& message, 
                        const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::System) {}
};

class DeviceError : public NLMError {
public:
    explicit DeviceError(const std::string& message, 
                        const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::Device) {}
};

class PerformanceError : public NLMError {
public:
    explicit PerformanceError(const std::string& message, 
                             const std::shared_ptr<NLMError>& cause = nullptr)
        : NLMError(message, cause, ErrorCategory::Performance) {}
};

// Type-safe error throwing macro for immediate chaining
#define THROW_NLM_ERROR(error_class, message) \
    do { \
        auto _cause = std::current_exception(); \
        if (_cause) { \
            std::throw_with_nested(error_class(message)); \
        } else { \
            throw error_class(message); \
        } \
    } while(0)

#define THROW_NLM_ERROR_WITH_CAUSE(error_class, message, cause) \
    do { \
        if (cause) { \
            throw error_class(message, cause); \
        } else { \
            throw error_class(message); \
        } \
    } while(0)

#define THROW_CONFIG_ERROR(msg) THROW_NLM_ERROR(ConfigError, msg)
#define THROW_MEMORY_ERROR(msg) THROW_NLM_ERROR(MemoryError, msg)
#define THROW_FILEIO_ERROR(msg) THROW_NLM_ERROR(FileIOError, msg)
#define THROW_VALIDATION_ERROR(msg) THROW_NLM_ERROR(ValidationError, msg)
#define THROW_BRAIN_ERROR(msg) THROW_NLM_ERROR(BrainError, msg)
#define THROW_SYSTEM_ERROR(msg) THROW_NLM_ERROR(SystemError, msg)

} // namespace nlm
