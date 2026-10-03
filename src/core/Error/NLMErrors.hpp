#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <memory>

namespace nlm {

// Base error class
class NLMError : public std::exception {
protected:
    std::string message_;
    std::string module_;
    int errorCode_;
    std::vector<std::string> contextStack_;
    
public:
    NLMError(const std::string& message, const std::string& module, int errorCode = 0)
        : message_(message), module_(module), errorCode_(errorCode) {}
    
    virtual ~NLMError() = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& message() const { return message_; }
    const std::string& module() const { return module_; }
    int errorCode() const { return errorCode_; }
    
    void addContext(const std::string& context) {
        contextStack_.push_back(context);
    }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[" << module_ << "] Error " << errorCode_ << ": " << message_;
        if (!contextStack_.empty()) {
            oss << "\nContext:";
            for (const auto& ctx : contextStack_) {
                oss << "  - " << ctx;
            }
        }
        return oss.str();
    }
};

// Validation error for invalid parameters
class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message, const std::string& module = "Validation")
        : NLMError(message, module, 1001) {}
};

// Resource error for memory allocation failures
class ResourceError : public NLMError {
public:
    ResourceError(const std::string& message, const std::string& module = "Resource")
        : NLMError(message, module, 2001) {}
};

// IO error for file system operations
class IOError : public NLMError {
public:
    IOError(const std::string& message, const std::string& module = "IO")
        : NLMError(message, module, 3001) {}
};

// Runtime error for logic errors and exceptions
class RuntimeError : public NLMError {
public:
    RuntimeError(const std::string& message, const std::string& module = "Runtime")
        : NLMError(message, module, 4001) {}
};

// Configuration error for invalid configuration
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& module = "Configuration")
        : NLMError(message, module, 5001) {}
};

// Error handler with error recovery capabilities
class ErrorHandler {
private:
    std::vector<std::shared_ptr<NLMError>> errors_;
    bool shouldThrow_ = true;
    bool logErrors_ = true;
    
public:
    void addError(std::shared_ptr<NLMError> error) {
        errors_.push_back(error);
        if (logErrors_) {
            // Log error via existing logger if available
            std::cerr << error->getFullMessage() << std::endl;
        }
        if (shouldThrow_) {
            throw *error;
        }
    }
    
    void clear() { errors_.clear(); }
    
    bool hasErrors() const { return !errors_.empty(); }
    
    const std::vector<std::shared_ptr<NLMError>>& getErrors() const { return errors_; }
    
    void setThrowOnError(bool throwFlag) { shouldThrow_ = throwFlag; }
    void setLogErrors(bool logFlag) { logErrors_ = logFlag; }
};

// Global error handler instance
inline ErrorHandler& getGlobalErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

// Helper macro for throwing validation errors
#define NLM_THROW_VALIDATION(msg) \
    do { \
        auto err = std::make_shared<ValidationError>(msg, "__FILE__"); \
        getGlobalErrorHandler().addError(err); \
    } while(0)

// Helper macro for throwing resource errors
#define NLM_THROW_RESOURCE(msg) \
    do { \
        auto err = std::make_shared<ResourceError>(msg, "__FILE__"); \
        getGlobalErrorHandler().addError(err); \
    } while(0)

// Helper macro for validation
#define NLM_VALIDATE(cond, msg) \
    do { \
        if (!(cond)) { \
            auto err = std::make_shared<ValidationError>(msg, "__FILE__"); \
            getGlobalErrorHandler().addError(err); \
        } \
    } while(0)

// Helper macro to throw on error
#define NLM_THROW_ON_ERROR() \
    do { \
        if (getGlobalErrorHandler().hasErrors()) { \
            throw *getGlobalErrorHandler().getErrors().back(); \
        } \
    } while(0)

} // namespace nlm
