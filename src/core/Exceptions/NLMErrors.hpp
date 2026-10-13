#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <optional>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <chrono>
#include <type_traits>

// C++20 features
#if __cplusplus >= 202002L
#include <source_location>
#endif

namespace nlm {

/**
 * Base class for all NLM exceptions
 * Provides common functionality for error codes, messages, source location,
 * exception chaining, and serialization
 */
class NLMException : public std::exception {
public:
    enum class ErrorCode {
        // Generic errors
        Unknown = 0,
        NotImplemented = 1,
        InvalidArgument = 2,
        OutOfRange = 3,
        NotFound = 4,
        AlreadyExists = 5,
        AccessDenied = 6,
        PermissionDenied = 7,
        Timeout = 8,
        Interrupted = 9,
        
        // Configuration errors
        ConfigFileNotFound = 100,
        ConfigParseError = 101,
        InvalidParameter = 102,
        MissingRequiredParameter = 103,
        
        // Initialization errors
        InitializationFailed = 200,
        ResourceNotAvailable = 201,
        InvalidState = 202,
        
        // Simulation errors
        SimulationStepFailed = 300,
        InvalidStep = 301,
        ConvergenceFailed = 302,
        TimeStepTooSmall = 303,
        NumericalInstability = 304,
        
        // Memory errors
        MemoryAllocationFailed = 400,
        MemoryCorruption = 401,
        OutOfMemory = 402,
        InvalidDeallocation = 403,
        MemoryLeak = 404,
        
        // I/O errors
        FileNotFound = 500,
        FilePermissionDenied = 501,
        FileReadError = 502,
        FileWriteError = 503,
        DirectoryNotFound = 504,
        PathNotFound = 505,
        DeviceError = 506,
        
        // Neural computation errors
        NeuralNetworkError = 600,
        InvalidNetworkTopology = 601,
        InvalidWeights = 602,
        ConvergenceError = 603,
        ActivationError = 604,
        BackpropagationError = 605,
        
        // System errors
        SystemError = 700,
        SystemCallFailed = 701,
        ResourceExhausted = 702,
        InvalidOperation = 703,
        NotSupported = 704,
        OutOfBounds = 705
    };

protected:
    std::string message_;
    ErrorCode code_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::shared_ptr<NLMException> cause_;
    std::unordered_map<std::string, std::string> metadata_;

    // Helper to convert enum to string
    static std::string errorCodeToString(ErrorCode code);

public:
    /**
     * Construct an exception with error code and message
     * @param code Error code
     * @param message Error message
     * @param file Source file (optional, auto-detected if not provided)
     * @param line Source line (optional, auto-detected if not provided)
     * @param function Source function (optional, auto-detected if not provided)
     */
    NLMException(ErrorCode code, const std::string& message,
                 const std::string& file = "",
                 int line = 0,
                 const std::string& function = "");

    virtual ~NLMException() = default;

    // Copy and move operations
    NLMException(const NLMException& other);
    NLMException& operator=(const NLMException& other);
    NLMException(NLMException&& other) noexcept;
    NLMException& operator=(NLMException&& other) noexcept;

    /**
     * Get the error message
     * @return Error message
     */
    const char* what() const noexcept override;

    /**
     * Get the error code
     * @return Error code
     */
    ErrorCode getErrorCode() const noexcept { return code_; }

    /**
     * Get the error message
     * @return Error message
     */
    const std::string& getMessage() const noexcept { return message_; }

    /**
     * Get the source file where the exception was thrown
     * @return Source file
     */
    const std::string& getFile() const noexcept { return file_; }

    /**
     * Get the source line where the exception was thrown
     * @return Source line
     */
    int getLine() const noexcept { return line_; }

    /**
     * Get the source function where the exception was thrown
     * @return Source function
     */
    const std::string& getFunction() const noexcept { return function_; }

    /**
     * Get the timestamp when the exception was created
     * @return Timestamp
     */
    const std::chrono::system_clock::time_point& getTimestamp() const noexcept { return timestamp_; }

    /**
     * Get the cause of this exception (if any)
     * @return Pointer to the cause exception
     */
    std::shared_ptr<NLMException> getCause() const { return cause_; }

    /**
     * Set the cause of this exception
     * @param cause Pointer to the cause exception
     */
    void setCause(std::shared_ptr<NLMException> cause) { cause_ = cause; }

    /**
     * Add metadata to the exception
     * @param key Metadata key
     * @param value Metadata value
     */
    void addMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
    }

    /**
     * Get metadata value by key
     * @param key Metadata key
     * @return Metadata value (empty string if not found)
     */
    std::string getMetadata(const std::string& key) const {
        auto it = metadata_.find(key);
        return it != metadata_.end() ? it->second : "";
    }

    /**
     * Get all metadata
     * @return Map of all metadata
     */
    const std::unordered_map<std::string, std::string>& getMetadata() const { return metadata_; }

    /**
     * Serialize exception to string
     * @return Serialized exception string
     */
    virtual std::string serialize() const;

    /**
     * Check if the exception has a cause
     * @return true if exception has a cause
     */
    bool hasCause() const { return cause_ != nullptr; }

    /**
     * Format exception with cause information
     * @return Formatted exception string
     */
    std::string formatWithCause() const;

    /**
     * Create a copy of this exception
     * @return Pointer to copy
     */
    virtual std::unique_ptr<NLMException> clone() const = 0;

    /**
     * Check if exception is of specific type
     * @tparam T Exception type to check
     * @return true if exception is of type T
     */
    template<typename T>
    bool is() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }

    /**
     * Get the cause of this exception if it's of specific type
     * @tparam T Exception type to check
     * @return Pointer to cause if it's of type T, nullptr otherwise
     */
    template<typename T>
    std::shared_ptr<T> getCauseAs() const {
        if (!cause_) return nullptr;
        auto derived = std::dynamic_pointer_cast<T>(cause_);
        return derived;
    }
};

/**
 * Configuration exception - configuration loading/parameter errors
 */
class ConfigurationException : public NLMException {
public:
    enum class ConfigurationErrorCode {
        FileNotFound = 100,
        ParseError = 101,
        InvalidParameter = 102,
        MissingRequiredParameter = 103,
        InvalidValue = 104,
        ValidationFailed = 105
    };

    ConfigurationException(ConfigurationErrorCode code, const std::string& message,
                           const std::string& file = "",
                           int line = 0,
                           const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    ConfigurationErrorCode getConfigurationErrorCode() const noexcept {
        return static_cast<ConfigurationErrorCode>(getErrorCode());
    }
};

/**
 * Initialization exception - initialization failures
 */
class InitializationException : public NLMException {
public:
    enum class InitializationErrorCode {
        Failed = 200,
        ResourceNotAvailable = 201,
        InvalidState = 202,
        DependencyFailed = 203,
        Timeout = 204
    };

    InitializationException(InitializationErrorCode code, const std::string& message,
                            const std::string& file = "",
                            int line = 0,
                            const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    InitializationErrorCode getInitializationErrorCode() const noexcept {
        return static_cast<InitializationErrorCode>(getErrorCode());
    }
};

/**
 * Simulation exception - runtime simulation errors
 */
class SimulationException : public NLMException {
public:
    enum class SimulationErrorCode {
        StepFailed = 300,
        InvalidStep = 301,
        ConvergenceFailed = 302,
        TimeStepTooSmall = 303,
        NumericalInstability = 304
    };

    SimulationException(SimulationErrorCode code, const std::string& message,
                        const std::string& file = "",
                        int line = 0,
                        const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    SimulationErrorCode getSimulationErrorCode() const noexcept {
        return static_cast<SimulationErrorCode>(getErrorCode());
    }
};

/**
 * Memory exception - memory allocation/management errors
 */
class MemoryException : public NLMException {
public:
    enum class MemoryErrorCode {
        AllocationFailed = 400,
        Corruption = 401,
        OutOfMemory = 402,
        InvalidDeallocation = 403,
        MemoryLeak = 404
    };

    MemoryException(MemoryErrorCode code, const std::string& message,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    MemoryErrorCode getMemoryErrorCode() const noexcept {
        return static_cast<MemoryErrorCode>(getErrorCode());
    }
};

/**
 * I/O exception - file I/O errors
 */
class IOException : public NLMException {
public:
    enum class IOErrorCode {
        FileNotFound = 500,
        FilePermissionDenied = 501,
        FileReadError = 502,
        FileWriteError = 503,
        DirectoryNotFound = 504,
        PathNotFound = 505,
        DeviceError = 506
    };

    IOException(IOErrorCode code, const std::string& message,
               const std::string& file = "",
               int line = 0,
               const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    IOErrorCode getIOErrorCode() const noexcept {
        return static_cast<IOErrorCode>(getErrorCode());
    }
};

/**
 * Neural exception - neural computation errors
 */
class NeuralException : public NLMException {
public:
    enum class NeuralErrorCode {
        NetworkError = 600,
        InvalidTopology = 601,
        InvalidWeights = 602,
        ConvergenceError = 603,
        ActivationError = 604,
        BackpropagationError = 605
    };

    NeuralException(NeuralErrorCode code, const std::string& message,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    NeuralErrorCode getNeuralErrorCode() const noexcept {
        return static_cast<NeuralErrorCode>(getErrorCode());
    }
};

/**
 * System exception - system-level errors
 */
class SystemException : public NLMException {
public:
    enum class SystemErrorCode {
        SystemError = 700,
        SystemCallFailed = 701,
        ResourceExhausted = 702,
        InvalidOperation = 703,
        NotSupported = 704,
        OutOfBounds = 705
    };

    SystemException(SystemErrorCode code, const std::string& message,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;
    
    SystemErrorCode getSystemErrorCode() const noexcept {
        return static_cast<SystemErrorCode>(getErrorCode());
    }
};

/**
 * Helper function to create exceptions with source location
 * @tparam ExceptionType Exception type to create
 * @tparam ErrorCodeType Error code type
 * @param code Error code
 * @param message Error message
 * @return Pointer to created exception
 */
#ifdef __cplusplus
#if __cplusplus >= 202002L
template<typename ExceptionType, typename ErrorCodeType>
std::unique_ptr<ExceptionType> createException(ErrorCodeType code, const std::string& message,
                                            std::source_location location = std::source_location::current()) {
    return std::make_unique<ExceptionType>(code, message,
                                          location.file_name(),
                                          location.line(),
                                          location.function_name());
}
#else
// Fallback for C++17
template<typename ExceptionType, typename ErrorCodeType>
std::unique_ptr<ExceptionType> createException(ErrorCodeType code, const std::string& message,
                                            const std::string& file = "",
                                            int line = 0,
                                            const std::string& function = "") {
    return std::make_unique<ExceptionType>(code, message, file, line, function);
}
#endif
#endif

/**
 * Helper function to throw an exception with chaining
 * @tparam ExceptionType Exception type to throw
 * @tparam ErrorCodeType Error code type
 * @param code Error code
 * @param message Error message
 * @param cause Pointer to cause exception (optional)
 */
#ifdef __cplusplus
#if __cplusplus >= 202002L
template<typename ExceptionType, typename ErrorCodeType>
void throwException(ErrorCodeType code, const std::string& message,
                   std::shared_ptr<NLMException> cause = nullptr,
                   std::source_location location = std::source_location::current()) {
    auto exc = createException<ExceptionType, ErrorCodeType>(code, message, location);
    if (cause) {
        exc->setCause(cause);
    }
    throw std::move(exc);
}
#else
// Fallback for C++17
template<typename ExceptionType, typename ErrorCodeType>
void throwException(ErrorCodeType code, const std::string& message,
                   std::shared_ptr<NLMException> cause = nullptr,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "") {
    auto exc = createException<ExceptionType, ErrorCodeType>(code, message, file, line, function);
    if (cause) {
        exc->setCause(cause);
    }
    throw std::move(exc);
}
#endif
#endif
}