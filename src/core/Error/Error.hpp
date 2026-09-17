// Enhanced NLM Exception Hierarchy

#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <sstream>

namespace nlm {

// Base NLM exception
class NLMException : public std::exception {
protected:
    std::string message_;
    std::string errorCode_;
    int line_;
    std::string file_;
    std::string function_;
    mutable std::string formattedMessage_;
    
public:
    NLMException(const std::string& message, const std::string& errorCode = "",
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : message_(message), errorCode_(errorCode), line_(line), 
          file_(file), function_(function) {}
    
    virtual ~NLMException() noexcept = default;
    
    const char* what() const noexcept override {
        if (formattedMessage_.empty()) {
            std::ostringstream oss;
            oss << "NLM Error: " << message_;
            if (!errorCode_.empty()) oss << " [Code: " << errorCode_ << "]";
            if (!file_.empty()) oss << " [File: " << file_ << ":" << line_ << "]";
            if (!function_.empty()) oss << " [Function: " << function_ << "]";
            formattedMessage_ = oss.str();
        }
        return formattedMessage_.c_str();
    }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getErrorCode() const { return errorCode_; }
    int getLine() const { return line_; }
    const std::string& getFile() const { return file_; }
    const std::string& getFunction() const { return function_; }
    
    virtual std::string getTypeName() const { return "NLMException"; }
};

// Memory allocation error
class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message, const std::string& file = "", int line = 0,
                const std::string& function = "")
        : NLMException(message, "MEM_ALLOC", file, line, function) {}
    
    std::string getTypeName() const override { return "MemoryError"; }
};

// Configuration error
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message, const std::string& errorCode = "CONFIG_ERROR",
                const std::string& file = "", int line = 0,
                const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "ConfigError"; }
};

// Brain initialization error
class BrainInitializationError : public NLMException {
public:
    BrainInitializationError(const std::string& message, const std::string& errorCode = "BRAIN_INIT",
                             const std::string& file = "", int line = 0,
                             const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "BrainInitializationError"; }
};

// Neural network operation error
class NeuralOperationError : public NLMException {
public:
    NeuralOperationError(const std::string& message, const std::string& errorCode = "NEURAL_OP",
                         const std::string& file = "", int line = 0,
                         const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "NeuralOperationError"; }
};

// AgentBrain operation error
class AgentBrainError : public NLMException {
public:
    AgentBrainError(const std::string& message, const std::string& errorCode = "AGENT_BRAIN",
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "AgentBrainError"; }
};

// Checkpoint system error
class CheckpointError : public NLMException {
public:
    CheckpointError(const std::string& message, const std::string& errorCode = "CHECKPOINT",
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "CheckpointError"; }
};

// Performance monitoring error
class PerformanceError : public NLMException {
public:
    PerformanceError(const std::string& message, const std::string& errorCode = "PERFORMANCE",
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "PerformanceError"; }
};

// Python binding error
class PythonBindingError : public NLMException {
public:
    PythonBindingError(const std::string& message, const std::string& errorCode = "PYTHON_BINDING",
                       const std::string& file = "", int line = 0,
                       const std::string& function = "")
        : NLMException(message, errorCode, file, line, function) {}
    
    std::string getTypeName() const override { return "PythonBindingError"; }
};

// Exception helper functions
inline std::string getStackTrace() {
    // Placeholder for stack trace collection
    return "Stack trace not available in this build";
}

// Exception utilities
class ExceptionUtils {
public:
    static std::string formatException(const std::exception& e) {
        std::ostringstream oss;
        oss << "Exception: " << e.what();
        return oss.str();
    }
    
    static void logException(const std::exception& e, const std::string& context = "") {
        std::string formatted = formatException(e);
        if (!context.empty()) {
            formatted = "Context: " + context + "\n" + formatted;
        }
        // In production, this would use the logger
        // For now, just write to stderr
        std::cerr << formatted << std::endl;
    }
    
    template<typename T>
    static T safeExecute(std::function<T()> func, const std::string& errorContext = "",
                         const std::string& errorCode = "EXEC_ERROR") {
        try {
            return func();
        } catch (const std::exception& e) {
            logException(e, errorContext);
            throw;
        } catch (...) {
            std::string msg = "Unknown error in " + errorContext;
            throw PythonBindingError(msg, errorCode);
        }
    }
};

} // namespace nlm
