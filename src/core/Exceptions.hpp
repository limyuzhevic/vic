#pragma once

#include <string>
#include <stdexcept>
#include <memory>

namespace nlm {

/**
 * @class NLMException
 * @brief Base exception class for all NLM errors
 * 
 * This is the root exception class for the NLM (Neural Learning Machine) system.
 * All other NLM-specific exceptions derive from this class.
 * 
 * @author NLM Development Team
 * @date Current
 * @version 0.1.0
 * 
 * Example usage:
 * @code
 * try {
 *     // NLM operations that might throw
 *     brain.step(step);
 * } catch (const nlm::NLMException& e) {
 *     std::cerr << "NLM Error: " << e.what() << std::endl;
 *     std::cerr << "Error code: " << static_cast<int>(e.getErrorCode()) << std::endl;
 * }
 * @endcode
 */
class NLMException : public std::runtime_error {
public:
    /**
     * @enum ErrorCode
     * @brief Enumeration of NLM error codes
     * 
     * Each error code corresponds to a specific type of error that can occur
     * during NLM operations. This helps with error classification and recovery.
     */
    enum class ErrorCode {
        SUCCESS = 0,                    ///< Operation completed successfully
        MEMORY_ALLOCATION_FAILED,        ///< Failed to allocate memory
        INVALID_CONFIGURATION,          ///< Invalid configuration parameter
        FILE_NOT_FOUND,                  ///< File not found during I/O operation
        NETWORK_ERROR,                   ///< Network-related error
        SIMULATION_ERROR,                ///< General simulation error
        VALIDATION_ERROR,                ///< Data validation failed
        RESOURCE_NOT_AVAILABLE,          ///< Required resource not available
        OUT_OF_BOUNDS_ACCESS,            ///< Attempted access outside valid range
        THREAD_SAFETY_VIOLATION,        ///< Thread safety violation detected
        CHECKPOINT_ERROR,                ///< Checkpoint/load error
        PLASTICITY_ERROR,                ///< Plasticity operation failed
        DEVELOPMENT_ERROR,               ///< Development system error
        NEUROMODULATION_ERROR,          ///< Neuromodulation system error
        MEMORY_POOL_ERROR,               ///< Memory pool operation failed
        SYNCHRONIZATION_ERROR,           ///< Synchronization operation failed
        INVALID_INPUT,                   ///< Invalid input parameter
        CONFIGURATION_FILE_ERROR,        ///< Configuration file parsing error
        COMPATIBILITY_ERROR,             ///< Version/compatibility error
        PERMISSION_ERROR,                ///< Permission denied error
        TIMEOUT_ERROR,                   ///< Operation timeout
        UNKNOWN_ERROR                    ///< Unknown or unexpected error
    };
    
    /**
     * @brief Construct an NLMException with error code and message
     * 
     * @param code The error code for this exception
     * @param message Human-readable error message
     */
    NLMException(ErrorCode code, const std::string& message)
        : std::runtime_error(formatMessage(code, message)), errorCode_(code) {}
    
    /**
     * @brief Get the error code associated with this exception
     * 
     * @return ErrorCode The error code
     */
    ErrorCode getErrorCode() const { return errorCode_; }
    
    /**
     * @brief Get string representation of the error code
     * 
     * @return const char* String representation of error code
     */
    const char* getErrorCodeString() const;
    
    /**
     * @brief Get the error code as integer (for compatibility)
     * 
     * @return int Integer representation of error code
     */
    int getErrorCodeInt() const { return static_cast<int>(errorCode_); }
    
protected:
    /**
     * @brief Format error message with error code prefix
     * 
     * @param code The error code
     * @param message The error message
     * @return std::string Formatted error message
     */
    static std::string formatMessage(ErrorCode code, const std::string& message) {
        std::string codeStr = getErrorCodeStringHelper(code);
        return "[NLM " + codeStr + "] " + message;
    }
    
    /**
     * @brief Helper to get string representation of error code
     * 
     * @param code The error code
     * @return const char* String representation
     */
    static const char* getErrorCodeStringHelper(ErrorCode code);
    
private:
    ErrorCode errorCode_;
};

/**
 * @class MemoryPoolException
 * @brief Exception thrown by memory pool operations
 * 
 * This exception is thrown when memory pool operations fail, such as
 * allocation failures, pool exhaustion, or invalid pool states.
 * 
 * @see MemoryPool
 * 
 * Example usage:
 * @code
 * try {
 *     MemoryPool<int> pool(1000);
 *     int* ptr = pool.allocate();
 *     // Use ptr...
 *     pool.deallocate(ptr);
 * } catch (const nlm::MemoryPoolException& e) {
 *     std::cerr << "Memory pool error: " << e.what() << std::endl;
 *     std::cerr << "Pool size: " << e.getPoolSize() << std::endl;
 * }
 * @endcode
 */
class MemoryPoolException : public NLMException {
public:
    /**
     * @brief Construct a MemoryPoolException
     * 
     * @param message Error message describing the failure
     * @param poolSize Optional: size of the pool that failed
     */
    MemoryPoolException(const std::string& message, size_t poolSize = 0)
        : NLMException(ErrorCode::MEMORY_POOL_ERROR, message), poolSize_(poolSize) {}
    
    /**
     * @brief Get the size of the pool that caused the error
     * 
     * @return size_t Size of the pool
     */
    size_t getPoolSize() const { return poolSize_; }
    
private:
    size_t poolSize_;
};

/**
 * @class ConfigurationException
 * @brief Exception thrown by configuration operations
 * 
 * This exception is thrown when configuration operations fail, such as
 * loading invalid configuration files, accessing non-existent keys, or
 * setting invalid configuration values.
 * 
 * @see Config
 * 
 * Example usage:
 * @code
 * try {
 *     Config config;
 *     config.loadFromFile("invalid_config.json");
 * } catch (const nlm::ConfigurationException& e) {
 *     std::cerr << "Configuration error: " << e.what() << std::endl;
 *     std::cerr << "Invalid key: " << e.getInvalidKey() << std::endl;
 * }
 * @endcode
 */
class ConfigurationException : public NLMException {
public:
    /**
     * @brief Construct a ConfigurationException
     * 
     * @param key The configuration key that caused the error
     * @param reason Description of why the configuration is invalid
     */
    ConfigurationException(const std::string& key, const std::string& reason)
        : NLMException(ErrorCode::INVALID_CONFIGURATION, 
                      "Invalid configuration for key \"" + key + "\": " + reason),
          invalidKey_(key) {}
    
    /**
     * @brief Get the invalid configuration key
     * 
     * @return const std::string& The invalid key
     */
    const std::string& getInvalidKey() const { return invalidKey_; }
    
private:
    std::string invalidKey_;
};

/**
 * @class SimulationException
 * @brief Exception thrown by simulation operations
 * 
 * This exception is thrown when simulation operations fail, such as
 * invalid simulation states, convergence failures, or numerical errors.
 * 
 * Example usage:
 * @code
 * try {
 *     Brain brain(config);
 *     brain.initialize();
 *     for (int i = 0; i < 1000; ++i) {
 *         brain.step(i);
 *     }
 * } catch (const nlm::SimulationException& e) {
 *     std::cerr << "Simulation error: " << e.what() << std::endl;
 *     std::cerr << "Error step: " << e.getErrorStep() << std::endl;
 * }
 * @endcode
 */
class SimulationException : public NLMException {
public:
    /**
     * @brief Construct a SimulationException
     * 
     * @param message Error message describing the simulation failure
     * @param errorStep Optional: simulation step where error occurred
     */
    SimulationException(const std::string& message, SimulationStep errorStep = 0)
        : NLMException(ErrorCode::SIMULATION_ERROR, message), errorStep_(errorStep) {}
    
    /**
     * @brief Get the simulation step where the error occurred
     * 
     * @return SimulationStep The error step
     */
    SimulationStep getErrorStep() const { return errorStep_; }
    
private:
    SimulationStep errorStep_;
};

/**
 * @class ValidationException
 * @brief Exception thrown by validation operations
 * 
 * This exception is thrown when data validation fails, such as
 * invalid input parameters, out-of-range values, or corrupted data.
 * 
 * Example usage:
 * @code
 * try {
 *     validateNeuronState(neuronState);
 * } catch (const nlm::ValidationException& e) {
 *     std::cerr << "Validation error: " << e.what() << std::endl;
 *     std::cerr << "Validation error code: " << e.getValidationCode() << std::endl;
 * }
 * @endcode
 */
class ValidationException : public NLMException {
public:
    /**
     * @enum ValidationCode
     * @brief Codes for different types of validation errors
     */
    enum class ValidationCode {
        OUT_OF_RANGE,           ///< Value outside valid range
        TYPE_MISMATCH,          ///< Incorrect data type
        MISSING_REQUIRED,       ///< Missing required field
        INVALID_FORMAT,         ///< Invalid data format
        DUPLICATE_ENTRY,        ///< Duplicate entry found
        CORRUPTED_DATA,         ///< Data appears corrupted
        INCONSISTENT_STATE,     ///< Inconsistent state
        PRECISION_LOSS,         ///< Precision loss in conversion
        UNEXPECTED_VALUE        ///< Value not expected
    };
    
    /**
     * @brief Construct a ValidationException
     * 
     * @param code Validation error code
     * @param message Description of validation failure
     */
    ValidationException(ValidationCode code, const std::string& message)
        : NLMException(ErrorCode::VALIDATION_ERROR, message), validationCode_(code) {}
    
    /**
     * @brief Get the validation error code
     * 
     * @return ValidationCode The validation code
     */
    ValidationCode getValidationCode() const { return validationCode_; }
    
private:
    ValidationCode validationCode_;
};

/**
 * @class FileOperationException
 * @brief Exception thrown by file operations
 * 
 * This exception is thrown when file operations fail, such as
 * read/write errors, permission denied, or file format errors.
 * 
 * Example usage:
 * @code
 * try {
 *     Config config;
 *     config.loadFromFile("brain_config.json");
 * } catch (const nlm::FileOperationException& e) {
 *     std::cerr << "File operation error: " << e.what() << std::endl;
 *     std::cerr << "Failed file: " << e.getFilename() << std::endl;
 * }
 * @endcode
 */
class FileOperationException : public NLMException {
public:
    /**
     * @enum OperationType
     * @brief Types of file operations
     */
    enum class OperationType {
        READ,    ///< File read operation
        WRITE,   ///< File write operation
        CREATE,  ///< File creation operation
        DELETE,  ///< File deletion operation
        OPEN,    ///< File open operation
        CLOSE,   ///< File close operation
        RENAME   ///< File rename operation
    };
    
    /**
     * @brief Construct a FileOperationException
     * 
     * @param operation Type of file operation that failed
     * @param filename Name of the file involved
     * @param reason Description of why the operation failed
     */
    FileOperationException(OperationType operation, const std::string& filename, const std::string& reason)
        : NLMException(ErrorCode::FILE_NOT_FOUND, 
                      "File operation " + getOperationString(operation) + 
                      " failed for file \"" + filename + "\": " + reason),
          filename_(filename), operation_(operation) {}
    
    /**
     * @brief Get the filename involved in the error
     * 
     * @return const std::string& The filename
     */
    const std::string& getFilename() const { return filename_; }
    
    /**
     * @brief Get the type of file operation that failed
     * 
     * @return OperationType The operation type
     */
    OperationType getOperationType() const { return operation_; }
    
    /**
     * @brief Get string representation of operation type
     * 
     * @return const char* String representation
     */
    const char* getOperationString(OperationType operation);
    
private:
    std::string filename_;
    OperationType operation_;
};

/**
 * @class ResourceException
 * @brief Exception thrown by resource management operations
 * 
 * This exception is thrown when resource management fails, such as
 * resource not available, resource already in use, or resource cleanup failures.
 */
class ResourceException : public NLMException {
public:
    /**
     * @enum ResourceError
     * @brief Types of resource errors
     */
    enum class ResourceError {
        NOT_AVAILABLE,     ///< Resource not available
        ALREADY_IN_USE,    ///< Resource already in use
        CLEANUP_FAILED,    ///< Resource cleanup failed
        TIMEOUT,           ///< Resource operation timeout
        INSUFFICIENT_PERMISSION,  ///< Insufficient permissions
        CORrupted          ///< Resource corrupted
    };
    
    /**
     * @brief Construct a ResourceException
     * 
     * @param resourceName Name of the resource
     * @param error Type of resource error
     * @param reason Description of the error
     */
    ResourceException(const std::string& resourceName, ResourceError error, const std::string& reason)
        : NLMException(ErrorCode::RESOURCE_NOT_AVAILABLE,
                      "Resource \"" + resourceName + \"\": " + getResourceErrorString(error) + ": " + reason),
          resourceName_(resourceName), error_(error) {}
    
    /**
     * @brief Get the resource name
     * 
     * @return const std::string& The resource name
     */
    const std::string& getResourceName() const { return resourceName_; }
    
    /**
     * @brief Get the resource error type
     * 
     * @return ResourceError The error type
     */
    ResourceError getResourceError() const { return error_; }
    
    /**
     * @brief Get string representation of resource error
     * 
     * @param error The error type
     * @return const char* String representation
     */
    const char* getResourceErrorString(ResourceError error);
    
private:
    std::string resourceName_;
    ResourceError error_;
};

/**
 * @class CheckpointException
 * @brief Exception thrown by checkpoint operations
 * 
 * This exception is thrown when checkpoint operations fail, such as
 * checkpoint file errors, validation failures, or serialization issues.
 */
class CheckpointException : public NLMException {
public:
    /**
     * @enum CheckpointError
     * @brief Types of checkpoint errors
     */
    enum class CheckpointError {
        FILE_CREATE_FAILED,   ///< Failed to create checkpoint file
        FILE_OPEN_FAILED,     ///< Failed to open checkpoint file
        VALIDATION_FAILED,    ///< Checkpoint validation failed
        SERIALIZATION_FAILED, ///< Checkpoint serialization failed
        DESERIALIZATION_FAILED, ///< Checkpoint deserialization failed
        COMPRESSION_FAILED,   ///< Checkpoint compression failed
        INTEGRITY_CHECK_FAILED, ///< Checkpoint integrity check failed
        BACKUP_FAILED         ///< Checkpoint backup failed
    };
    
    /**
     * @brief Construct a CheckpointException
     * 
     * @param error Type of checkpoint error
     * @param filename Name of the checkpoint file
     * @param reason Description of the error
     */
    CheckpointException(CheckpointError error, const std::string& filename, const std::string& reason)
        : NLMException(ErrorCode::CHECKPOINT_ERROR,
                      "Checkpoint error " + getCheckpointErrorString(error) +
                      " for file \"" + filename + "\": " + reason),
          filename_(filename), error_(error) {}
    
    /**
     * @brief Get the checkpoint filename
     * 
     * @return const std::string& The filename
     */
    const std::string& getFilename() const { return filename_; }
    
    /**
     * @brief Get the checkpoint error type
     * 
     * @return CheckpointError The error type
     */
    CheckpointError getCheckpointError() const { return error_; }
    
    /**
     * @brief Get string representation of checkpoint error
     * 
     * @param error The error type
     * @return const char* String representation
     */
    const char* getCheckpointErrorString(CheckpointError error);
    
private:
    std::string filename_;
    CheckpointError error_;
};

/**
 * @class DevelopmentException
 * @brief Exception thrown by development system operations
 * 
 * This exception is thrown when development system operations fail, such as
 * developmental stage errors, maturation failures, or synaptic development issues.
 */
class DevelopmentException : public NLMException {
public:
    /**
     * @enum DevelopmentError
     * @brief Types of development errors
     */
    enum class DevelopmentError {
        INVALID_STAGE,           ///< Invalid developmental stage
        MATURATION_FAILED,       ///< Neural maturation failed
        SYNAPTOGENESIS_FAILED,   ///< Synaptogenesis failed
        PRUNING_FAILED,          ///< Neural pruning failed
        CRITICAL_PERIOD_ERROR,   ///< Critical period error
        AGING_ERROR,             ///< Aging process error
        ENVIRONENTAL_INFLUENCE_FAILED  ///< Environmental influence failed
    };
    
    /**
     * @brief Construct a DevelopmentException
     * 
     * @param error Type of development error
     * @param reason Description of the error
     */
    DevelopmentException(DevelopmentError error, const std::string& reason)
        : NLMException(ErrorCode::DEVELOPMENT_ERROR,
                      "Development error " + getDevelopmentErrorString(error) + ": " + reason),
          error_(error) {}
    
    /**
     * @brief Get the development error type
     * 
     * @return DevelopmentError The error type
     */
    DevelopmentError getDevelopmentError() const { return error_; }
    
    /**
     * @brief Get string representation of development error
     * 
     * @param error The error type
     * @return const char* String representation
     */
    const char* getDevelopmentErrorString(DevelopmentError error);
    
private:
    DevelopmentError error_;
};

/**
 * @class NeuromodulationException
 * @brief Exception thrown by neuromodulation system operations
 * 
 * This exception is thrown when neuromodulation operations fail, such as
 * dopamine calculation errors, curiosity computation issues, or novelty detection failures.
 */
class NeuromodulationException : public NLMException {
public:
    /**
     * @enum NeuromodulationError
     * @brief Types of neuromodulation errors
     */
    enum class NeuromodulationError {
        DOPAMINE_CALCULATION_FAILED,   ///< Dopamine level calculation failed
        CURIOUSITY_COMPUTATION_FAILED, ///< Curiosity level computation failed
        NOVELTY_DETECTION_FAILED,     ///< Novelty detection failed
        PREDICTION_ERROR_CALCULATION_FAILED,  ///< Prediction error calculation failed
        REWARD_MODULATION_FAILED,     ///< Reward modulation failed
        ACTION_SELECTION_FAILED       ///< Action selection failed
    };
    
    /**
     * @brief Construct a NeuromodulationException
     * 
     * @param error Type of neuromodulation error
     * @param reason Description of the error
     */
    NeuromodulationException(NeuromodulationError error, const std::string& reason)
        : NLMException(ErrorCode::NEUROMODULATION_ERROR,
                      "Neuromodulation error " + getNeuromodulationErrorString(error) + ": " + reason),
          error_(error) {}
    
    /**
     * @brief Get the neuromodulation error type
     * 
     * @return NeuromodulationError The error type
     */
    NeuromodulationError getNeuromodulationError() const { return error_; }
    
    /**
     * @brief Get string representation of neuromodulation error
     * 
     * @param error The error type
     * @return const char* String representation
     */
    const char* getNeuromodulationErrorString(NeuromodulationError error);
    
private:
    NeuromodulationError error_;
};

// Implementation details for NLMException

inline const char* NLMException::getErrorCodeString() const {
    return getErrorCodeStringHelper(errorCode_);
}

inline const char* NLMException::getErrorCodeStringHelper(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS: return "SUCCESS";
        case ErrorCode::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
        case ErrorCode::INVALID_CONFIGURATION: return "INVALID_CONFIGURATION";
        case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCode::NETWORK_ERROR: return "NETWORK_ERROR";
        case ErrorCode::SIMULATION_ERROR: return "SIMULATION_ERROR";
        case ErrorCode::VALIDATION_ERROR: return "VALIDATION_ERROR";
        case ErrorCode::RESOURCE_NOT_AVAILABLE: return "RESOURCE_NOT_AVAILABLE";
        case ErrorCode::OUT_OF_BOUNDS_ACCESS: return "OUT_OF_BOUNDS_ACCESS";
        case ErrorCode::THREAD_SAFETY_VIOLATION: return "THREAD_SAFETY_VIOLATION";
        case ErrorCode::CHECKPOINT_ERROR: return "CHECKPOINT_ERROR";
        case ErrorCode::PLASTICITY_ERROR: return "PLASTICITY_ERROR";
        case ErrorCode::DEVELOPMENT_ERROR: return "DEVELOPMENT_ERROR";
        case ErrorCode::NEUROMODULATION_ERROR: return "NEUROMODULATION_ERROR";
        case ErrorCode::MEMORY_POOL_ERROR: return "MEMORY_POOL_ERROR";
        case ErrorCode::SYNCHRONIZATION_ERROR: return "SYNCHRONIZATION_ERROR";
        case ErrorCode::INVALID_INPUT: return "INVALID_INPUT";
        case ErrorCode::CONFIGURATION_FILE_ERROR: return "CONFIGURATION_FILE_ERROR";
        case ErrorCode::COMPATIBILITY_ERROR: return "COMPATIBILITY_ERROR";
        case ErrorCode::PERMISSION_ERROR: return "PERMISSION_ERROR";
        case ErrorCode::TIMEOUT_ERROR: return "TIMEOUT_ERROR";
        case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        default: return "UNKNOWN";
    }
}

inline const char* FileOperationException::getOperationString(FileOperationException::OperationType operation) {
    switch (operation) {
        case OperationType::READ: return "READ";
        case OperationType::WRITE: return "WRITE";
        case OperationType::CREATE: return "CREATE";
        case OperationType::DELETE: return "DELETE";
        case OperationType::OPEN: return "OPEN";
        case OperationType::CLOSE: return "CLOSE";
        case OperationType::RENAME: return "RENAME";
        default: return "UNKNOWN";
    }
}

inline const char* ResourceException::getResourceErrorString(ResourceException::ResourceError error) {
    switch (error) {
        case ResourceError::NOT_AVAILABLE: return "NOT_AVAILABLE";
        case ResourceError::ALREADY_IN_USE: return "ALREADY_IN_USE";
        case ResourceError::CLEANUP_FAILED: return "CLEANUP_FAILED";
        case ResourceError::TIMEOUT: return "TIMEOUT";
        case ResourceError::INSUFFICIENT_PERMISSION: return "INSUFFICIENT_PERMISSION";
        case ResourceError::CORrupted: return "CORRUPTED";
        default: return "UNKNOWN";
    }
}

inline const char* CheckpointException::getCheckpointErrorString(CheckpointException::CheckpointError error) {
    switch (error) {
        case CheckpointError::FILE_CREATE_FAILED: return "FILE_CREATE_FAILED";
        case CheckpointError::FILE_OPEN_FAILED: return "FILE_OPEN_FAILED";
        case CheckpointError::VALIDATION_FAILED: return "VALIDATION_FAILED";
        case CheckpointError::SERIALIZATION_FAILED: return "SERIALIZATION_FAILED";
        case CheckpointError::DESERIALIZATION_FAILED: return "DESERIALIZATION_FAILED";
        case CheckpointError::COMPRESSION_FAILED: return "COMPRESSION_FAILED";
        case CheckpointError::INTEGRITY_CHECK_FAILED: return "INTEGRITY_CHECK_FAILED";
        case CheckpointError::BACKUP_FAILED: return "BACKUP_FAILED";
        default: return "UNKNOWN";
    }
}

inline const char* DevelopmentException::getDevelopmentErrorString(DevelopmentException::DevelopmentError error) {
    switch (error) {
        case DevelopmentError::INVALID_STAGE: return "INVALID_STAGE";
        case DevelopmentError::MATURATION_FAILED: return "MATURATION_FAILED";
        case DevelopmentError::SYNAPTOGENESIS_FAILED: return "SYNAPTOGENESIS_FAILED";
        case DevelopmentError::PRUNING_FAILED: return "PRUNING_FAILED";
        case DevelopmentError::CRITICAL_PERIOD_ERROR: return "CRITICAL_PERIOD_ERROR";
        case DevelopmentError::AGING_ERROR: return "AGING_ERROR";
        case DevelopmentError::ENVIRONENTAL_INFLUENCE_FAILED: return "ENVIRONENTAL_INFLUENCE_FAILED";
        default: return "UNKNOWN";
    }
}

inline const char* NeuromodulationException::getNeuromodulationErrorString(NeuromodulationException::NeuromodulationError error) {
    switch (error) {
        case NeuromodulationError::DOPAMINE_CALCULATION_FAILED: return "DOPAMINE_CALCULATION_FAILED";
        case NeuromodulationError::CURIOUSITY_COMPUTATION_FAILED: return "CURIOUSITY_COMPUTATION_FAILED";
        case NeuromodulationError::NOVELTY_DETECTION_FAILED: return "NOVELTY_DETECTION_FAILED";
        case NeuromodulationError::PREDICTION_ERROR_CALCULATION_FAILED: return "PREDICTION_ERROR_CALCULATION_FAILED";
        case NeuromodulationError::REWARD_MODULATION_FAILED: return "REWARD_MODULATION_FAILED";
        case NeuromodulationError::ACTION_SELECTION_FAILED: return "ACTION_SELECTION_FAILED";
        default: return "UNKNOWN";
    }
}

} // namespace nlm

/**
 * @file Exceptions.hpp
 * 
 * This file contains the NLM exception hierarchy for consistent error handling
 * across the Neural Learning Machine codebase.
 * 
 * The exception hierarchy includes:
 * - NLMException: Base exception class
 * - MemoryPoolException: Memory pool operation errors
 * - ConfigurationException: Configuration-related errors
 * - SimulationException: Simulation operation errors
 * - ValidationException: Data validation errors
 * - FileOperationException: File I/O errors
 * - ResourceException: Resource management errors
 * - CheckpointException: Checkpoint operation errors
 * - DevelopmentException: Development system errors
 * - NeuromodulationException: Neuromodulation system errors
 * 
 * Each exception type includes detailed error codes and contextual information
 * to aid in debugging and error recovery.
 * 
 * Features:
 * - Consistent error classification with ErrorCode enum
 * - Contextual error information (file names, resource names, etc.)
 * - Detailed error messages with error code prefixes
 * - Type-specific error codes for fine-grained error handling
 * - Backward compatibility with existing std::exception usage
 * 
 * This exception hierarchy significantly improves:
 * - Error diagnosis and recovery
 * - Debugging experience
 * - Integration with Python bindings
 * - Code maintainability
 * 
 * Usage example:
 * @code
 * #include "Exceptions.hpp"
 * 
 * try {
 *     // NLM operations
 *     brain.step(step);
 *     config.loadFromFile("config.json");
 *     MemoryPool<int> pool;
 *     pool.allocate();
 * } catch (const nlm::NLMException& e) {
 *     std::cerr << "Error: " << e.what() << std::endl;
 *     std::cerr << "Error code: " << static_cast<int>(e.getErrorCode()) << std::endl;
 *     
 *     // Type-specific handling
 *     if (dynamic_cast<const nlm::ConfigurationException*>(&e)) {
 *         // Handle configuration error
 *     } else if (dynamic_cast<const nlm::MemoryPoolException*>(&e)) {
 *         // Handle memory pool error
 *     } else if (dynamic_cast<const nlm::SimulationException*>(&e)) {
 *         // Handle simulation error
 *     }
 * }
 * @endcode
 */