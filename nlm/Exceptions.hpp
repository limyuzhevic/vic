/*
 * nlm/Exceptions.hpp - Core exception classes for the NLM system
 * Comprehensive exception hierarchy with detailed error reporting
 */

#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <map>
#include <vector>
#include <sstream>

namespace nlm {

// Base exception class for all NLM exceptions
class NLMException : public std::exception {
protected:
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::map<std::string, std::string> context_;
    mutable std::string formattedMessage_;
    
public:
    explicit NLMException(const std::string& message) : message_(message) {}
    
    NLMException(const std::string& message, 
                 const std::string& file, 
                 int line,
                 const std::string& function) 
        : message_(message), file_(file), line_(line), function_(function) {}
    
    virtual ~NLMException() noexcept = default;
    
    const char* what() const noexcept override {
        if (formattedMessage_.empty()) {
            formatMessage();
        }
        return formattedMessage_.c_str();
    }
    
    virtual void formatMessage() const {
        std::ostringstream oss;
        oss << "[NLM " << type() << "] " << message_;
        if (!file_.empty()) {
            oss << "\n  File: " << file_ << ":" << line_;
        }
        if (!function_.empty()) {
            oss << "\n  Function: " << function_;
        }
        if (!context_.empty()) {
            oss << "\n  Context:";
            for (const auto& pair : context_) {
                oss << "\n    " << pair.first << " = " << pair.second;
            }
        }
        formattedMessage_ = oss.str();
    }
    
    virtual const char* type() const noexcept { return "NLMException"; }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    void addContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }
    
    const std::map<std::string, std::string>& getContext() const { return context_; }
};

// Config-specific exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException(message) {}
    
    ConfigException(const std::string& message,
                    const std::string& file,
                    int line,
                    const std::string& function)
        : NLMException(message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "ConfigException"; }
};

// Brain-specific exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException(message) {}
    
    BrainException(const std::string& message,
                   const std::string& file,
                   int line,
                   const std::string& function)
        : NLMException(message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "BrainException"; }
};

// Neural computation-specific exceptions
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message) : NLMException(message) {}
    
    NeuralException(const std::string& message,
                    const std::string& file,
                    int line,
                    const std::string& function)
        : NLMException(message, file, line, function) {}
    
    virtual const char* type() const noexcept override { return "NeuralException"; }
};

// Specific config exception types
class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath)
        : ConfigException("Configuration file not found: " + filepath) {
        addContext("filepath", filepath);
    }
    
    ConfigFileNotFoundException(const std::string& filepath,
                                const std::string& file,
                                int line,
                                const std::string& function)
        : ConfigException("Configuration file not found: " + filepath, file, line, function) {
        addContext("filepath", filepath);
    }
    
    virtual const char* type() const noexcept override { return "ConfigFileNotFoundException"; }
};

class ConfigSyntaxErrorException : public ConfigException {
public:
    explicit ConfigSyntaxErrorException(const std::string& details)
        : ConfigException("Configuration syntax error: " + details) {
        addContext("details", details);
    }
    
    ConfigSyntaxErrorException(const std::string& details,
                               const std::string& file,
                               int line,
                               const std::string& function)
        : ConfigException("Configuration syntax error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "ConfigSyntaxErrorException"; }
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& key, const std::string& value)
        : ConfigException("Configuration validation error for key: " + key) {
        addContext("key", key);
        addContext("value", value);
    }
    
    ConfigValidationException(const std::string& key,
                              const std::string& value,
                              const std::string& file,
                              int line,
                              const std::string& function)
        : ConfigException("Configuration validation error for key: " + key, file, line, function) {
        addContext("key", key);
        addContext("value", value);
    }
    
    virtual const char* type() const noexcept override { return "ConfigValidationException"; }
};

class InvalidConfigurationException : public ConfigException {
public:
    explicit InvalidConfigurationException(const std::string& reason)
        : ConfigException("Invalid configuration: " + reason) {
        addContext("reason", reason);
    }
    
    InvalidConfigurationException(const std::string& reason,
                                  const std::string& file,
                                  int line,
                                  const std::string& function)
        : ConfigException("Invalid configuration: " + reason, file, line, function) {
        addContext("reason", reason);
    }
    
    virtual const char* type() const noexcept override { return "InvalidConfigurationException"; }
};

// Specific brain exception types
class BrainSaveErrorException : public BrainException {
public:
    explicit BrainSaveErrorException(const std::string& filepath)
        : BrainException("Brain save error: " + filepath) {
        addContext("filepath", filepath);
    }
    
    BrainSaveErrorException(const std::string& filepath,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : BrainException("Brain save error: " + filepath, file, line, function) {
        addContext("filepath", filepath);
    }
    
    virtual const char* type() const noexcept override { return "BrainSaveErrorException"; }
};

class BrainLoadErrorException : public BrainException {
public:
    explicit BrainLoadErrorException(const std::string& filepath)
        : BrainException("Brain load error: " + filepath) {
        addContext("filepath", filepath);
    }
    
    BrainLoadErrorException(const std::string& filepath,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : BrainException("Brain load error: " + filepath, file, line, function) {
        addContext("filepath", filepath);
    }
    
    virtual const char* type() const noexcept override { return "BrainLoadErrorException"; }
};

class BrainInitializationErrorException : public BrainException {
public:
    explicit BrainInitializationErrorException(const std::string& reason)
        : BrainException("Brain initialization error: " + reason) {
        addContext("reason", reason);
    }
    
    BrainInitializationErrorException(const std::string& reason,
                                       const std::string& file,
                                       int line,
                                       const std::string& function)
        : BrainException("Brain initialization error: " + reason, file, line, function) {
        addContext("reason", reason);
    }
    
    virtual const char* type() const noexcept override { return "BrainInitializationErrorException"; }
};

// Specific neural exception types
class NeuralParameterException : public NeuralException {
public:
    explicit NeuralParameterException(const std::string& parameter, float value)
        : NeuralException("Neural parameter error: " + parameter + " = " + std::to_string(value)) {
        addContext("parameter", parameter);
        addContext("value", std::to_string(value));
    }
    
    NeuralParameterException(const std::string& parameter,
                             float value,
                             const std::string& file,
                             int line,
                             const std::string& function)
        : NeuralException("Neural parameter error: " + parameter + " = " + std::to_string(value), file, line, function) {
        addContext("parameter", parameter);
        addContext("value", std::to_string(value));
    }
    
    virtual const char* type() const noexcept override { return "NeuralParameterException"; }
};

class PlasticityErrorException : public NeuralException {
public:
    explicit PlasticityErrorException(const std::string& details)
        : NeuralException("Plasticity error: " + details) {
        addContext("details", details);
    }
    
    PlasticityErrorException(const std::string& details,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : NeuralException("Plasticity error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "PlasticityErrorException"; }
};

class MemoryAllocationException : public NeuralException {
public:
    explicit MemoryAllocationException(size_t size, const std::string& type)
        : NeuralException("Memory allocation error: " + std::to_string(size) + " bytes of type " + type) {
        addContext("size", std::to_string(size));
        addContext("type", type);
    }
    
    MemoryAllocationException(size_t size,
                              const std::string& type,
                              const std::string& file,
                              int line,
                              const std::string& function)
        : NeuralException("Memory allocation error: " + std::to_string(size) + " bytes of type " + type, file, line, function) {
        addContext("size", std::to_string(size));
        addContext("type", type);
    }
    
    virtual const char* type() const noexcept override { return "MemoryAllocationException"; }
};

// Helper function for exception creation with automatic location info
inline std::string getCurrentFile() {
    return "unknown";
}

inline int getCurrentLine() {
    return 0;
}

inline std::string getCurrentFunction() {
    return "unknown";
}

} // namespace nlm
