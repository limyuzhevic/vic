#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>
#include <chrono>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::exception {
protected:
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    
public:
    explicit NLMException(const std::string& message) 
        : message_(message), line_(0) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    NLMException(const std::string& message, const std::string& file, int line, const std::string& function)
        : message_(message), file_(file), line_(line), function_(function) {
        timestamp_ = std::chrono::system_clock::now();
    }
    
    virtual ~NLMException() = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    std::string getFullErrorInfo() const {
        std::ostringstream oss;
        oss << "[NLM " << getCurrentTime() << "] " << message_;
        if (!file_.empty()) {
            oss << "\n  File: " << file_;
        }
        if (line_ > 0) {
            oss << "\n  Line: " << line_;
        }
        if (!function_.empty()) {
            oss << "\n  Function: " << function_;
        }
        return oss.str();
    }
    
private:
    std::string getCurrentTime() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Configuration-specific exception
class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
    
    ConfigurationException(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Configuration Error: " + message, file, line, function) {}
};

// File I/O specific exception
class FileIOError : public NLMException {
public:
    enum class ErrorType {
        FileNotFound,
        PermissionDenied,
        DiskFull,
        InvalidFormat,
        ReadError,
        WriteError,
        CorruptFile
    };
    
    FileIOError(const std::string& filepath, ErrorType errorType, const std::string& message)
        : NLMException("File I/O Error: " + filepath + " - " + getErrorTypeString(errorType) + ": " + message),
          filepath_(filepath), errorType_(errorType) {}
    
    FileIOError(const std::string& filepath, ErrorType errorType, const std::string& message,
                const std::string& file, int line, const std::string& function)
        : NLMException("File I/O Error: " + filepath + " - " + getErrorTypeString(errorType) + ": " + message, file, line, function),
          filepath_(filepath), errorType_(errorType) {}
    
    const std::string& getFilepath() const { return filepath_; }
    ErrorType getErrorType() const { return errorType_; }
    
    static std::string getErrorTypeString(ErrorType type) {
        switch (type) {
            case ErrorType::FileNotFound: return "File not found";
            case ErrorType::PermissionDenied: return "Permission denied";
            case ErrorType::DiskFull: return "Disk full";
            case ErrorType::InvalidFormat: return "Invalid format";
            case ErrorType::ReadError: return "Read error";
            case ErrorType::WriteError: return "Write error";
            case ErrorType::CorruptFile: return "Corrupt file";
            default: return "Unknown error";
        }
    }
    
private:
    std::string filepath_;
    ErrorType errorType_;
};

// Neural simulation specific exception
class NeuralSimulationError : public NLMException {
public:
    enum class ErrorType {
        InvalidConfiguration,
        MemoryAllocationError,
        NumericalInstability,
        PlasticityError,
        BoundaryViolation,
        IntegrationError,
        ConvergenceError
    };
    
    NeuralSimulationError(const std::string& message, ErrorType errorType)
        : NLMException("Neural Simulation Error: " + message), errorType_(errorType) {}
    
    NeuralSimulationError(const std::string& message, ErrorType errorType,
                          const std::string& file, int line, const std::string& function)
        : NLMException("Neural Simulation Error: " + message, file, line, function), errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    static std::string getErrorTypeString(ErrorType type) {
        switch (type) {
            case ErrorType::InvalidConfiguration: return "Invalid configuration";
            case ErrorType::MemoryAllocationError: return "Memory allocation error";
            case ErrorType::NumericalInstability: return "Numerical instability";
            case ErrorType::PlasticityError: return "Plasticity error";
            case ErrorType::BoundaryViolation: return "Boundary violation";
            case ErrorType::IntegrationError: return "Integration error";
            case ErrorType::ConvergenceError: return "Convergence error";
            default: return "Unknown error";
        }
    }
    
private:
    ErrorType errorType_;
};

// Memory-specific exception
class MemoryError : public NLMException {
public:
    enum class ErrorType {
        AllocationFailed,
        DeallocationFailed,
        MemoryCorruption,
        OutOfMemory,
        CacheMiss
    };
    
    MemoryError(const std::string& message, ErrorType errorType)
        : NLMException("Memory Error: " + message), errorType_(errorType) {}
    
    MemoryError(const std::string& message, ErrorType errorType,
                const std::string& file, int line, const std::string& function)
        : NLMException("Memory Error: " + message, file, line, function), errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    static std::string getErrorTypeString(ErrorType type) {
        switch (type) {
            case ErrorType::AllocationFailed: return "Allocation failed";
            case ErrorType::DeallocationFailed: return "Deallocation failed";
            case ErrorType::MemoryCorruption: return "Memory corruption";
            case ErrorType::OutOfMemory: return "Out of memory";
            case ErrorType::CacheMiss: return "Cache miss";
            default: return "Unknown error";
        }
    }
    
private:
    ErrorType errorType_;
};

// Validation specific exception
class ValidationError : public NLMException {
public:
    struct ValidationResult {
        std::string key;
        std::string expectedType;
        std::string actualType;
        std::string errorMessage;
        bool isWarning;
    };
    
    ValidationError(const std::string& message) 
        : NLMException("Validation Error: " + message) {}
    
    ValidationError(const std::string& message, const std::string& file, int line, const std::string& function)
        : NLMException("Validation Error: " + message, file, line, function) {}
    
    ValidationError(const std::vector<ValidationResult>& results)
        : NLMException(formatValidationResults(results)) {}
    
    static std::string formatValidationResults(const std::vector<ValidationResult>& results) {
        std::ostringstream oss;
        oss << "Validation failed:\n";
        for (const auto& result : results) {
            oss << "  Key: " << result.key << "\n";
            if (result.isWarning) {
                oss << "    WARNING: ";
            } else {
                oss << "    ERROR: ";
            }
            oss << "Expected type " << result.expectedType;
            if (!result.actualType.empty()) {
                oss << ", got " << result.actualType;
            }
            oss << " - " << result.errorMessage << "\n";
        }
        return oss.str();
    }
};

// Plasticity-specific exception
class PlasticityError : public NLMException {
public:
    enum class ErrorType {
        WeightUpdateError,
        StructuralPlasticityError,
        STDPError,
        HebbianError,
        SynaptogenesisError,
        PruningError
    };
    
    PlasticityError(const std::string& message, ErrorType errorType)
        : NLMException("Plasticity Error: " + message), errorType_(errorType) {}
    
    PlasticityError(const std::string& message, ErrorType errorType,
                   const std::string& file, int line, const std::string& function)
        : NLMException("Plasticity Error: " + message, file, line, function), errorType_(errorType) {}
    
    ErrorType getErrorType() const { return errorType_; }
    
    static std::string getErrorTypeString(ErrorType type) {
        switch (type) {
            case ErrorType::WeightUpdateError: return "Weight update error";
            case ErrorType::StructuralPlasticityError: return "Structural plasticity error";
            case ErrorType::STDPError: return "STDP error";
            case ErrorType::HebbianError: return "Hebbian error";
            case ErrorType::SynaptogenesisError: return "Synaptogenesis error";
            case ErrorType::PruningError: return "Pruning error";
            default: return "Unknown error";
        }
    }
    
private:
    ErrorType errorType_;
};

// Exception wrapper for functions that catch and rethrow
class ExceptionWrapper {
public:
    template<typename Func>
    static auto wrap(Func&& func) {
        return [func = std::forward<Func>(func)]() -> auto {
            try {
                return func();
            } catch (const NLMException& e) {
                // Re-throw NLMException as-is
                throw;
            } catch (const std::exception& e) {
                // Convert other exceptions to NLMException
                throw NLMException(std::string("Unexpected error: ") + e.what());
            } catch (...) {
                // Handle unknown exceptions
                throw NLMException("Unknown exception occurred");
            }
        };
    }
    
    template<typename Func, typename... Args>
    static auto wrap(Func&& func, Args&&... args) {
        return [func = std::forward<Func>(func), ...args = std::forward<Args>(args)]() -> auto {
            try {
                return func(args...);
            } catch (const NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw NLMException(std::string("Unexpected error: ") + e.what());
            } catch (...) {
                throw NLMException("Unknown exception occurred");
            }
        };
    }
};

// Global exception handler
class GlobalExceptionHandler {
public:
    using ExceptionHandler = std::function<void(const NLMException&)>;;
    
    static void setHandler(const ExceptionHandler& handler) {
        handler_ = handler;
    }
    
    static ExceptionHandler getHandler() {
        return handler_;
    }
    
    static void handle(const NLMException& error) {
        if (handler_) {
            handler_(error);
        } else {
            // Default handler: print to stderr
            std::cerr << error.getFullErrorInfo() << std::endl;
        }
    }
    
    static void handle(const std::exception& error) {
        handle(NLMException(std::string("Standard exception: ") + error.what()));
    }
    
private:
    static ExceptionHandler handler_;
};

// Macro for throwing exceptions with context
#define NLM_THROW(error_type, message) \
    throw error_type(message, __FILE__, __LINE__, __func__)

#define NLM_THROW_WITH_CONTEXT(error_type, message, context) \
    throw error_type(message + " (" + context + ")", __FILE__, __LINE__, __func__

// Macro for handling exceptions
#define NLM_HANDLE_EXCEPTION(handler) \
    catch (const NLMException& e) { handler(e); }

// Result type for functions that can fail
template<typename T>
class Result {
public:
    enum class Status { Success, Failure };
    
private:
    Status status_;
    T value_;
    std::shared_ptr<NLMException> error_;
    
public:
    Result() : status_(Status::Success) {}
    Result(const T& value) : status_(Status::Success), value_(value) {}
    Result(const std::shared_ptr<NLMException>& error) : status_(Status::Failure), error_(error) {}
    
    bool isSuccess() const { return status_ == Status::Success; }
    bool isFailure() const { return status_ == Status::Failure; }
    
    const T& value() const { return value_; }
    const NLMException& error() const { return *error_; }
    
    // Allow user code to extract the exception
    const std::shared_ptr<NLMException>& getError() const { return error_; }
};

// Utility functions for error handling
namespace ErrorUtils {
    // Generate a unique error ID for debugging
    static std::string generateErrorId() {
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        ss << "ERR_" << std::hex << std::this_thread::get_id() << "_" << time;
        return ss.str();
    }
    
    // Sanitize error messages for logging
    static std::string sanitizeMessage(const std::string& message) {
        // Remove newlines and control characters
        std::string sanitized;
        for (char c : message) {
            if (c >= 32 && c != 127) {
                sanitized += c;
            } else if (c == '\n') {
                sanitized += "\\n";
            } else if (c == '\r') {
                sanitized += "\\r";
            } else if (c == '\t') {
                sanitized += "\\t";
            }
        }
        return sanitized;
    }
    
    // Validate error message length
    static bool isValidErrorMessage(const std::string& message) {
        return message.length() <= 1024;  // Reasonable limit
    }
    
    // Get error class name for a given exception
    static std::string getExceptionClassName(const std::exception& e) {
        const std::type_info& ti = typeid(e);
        return ti.name();
    }
}

} // namespace nlm
