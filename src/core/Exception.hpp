/**
 * @file Exception.hpp
 * @brief Comprehensive exception hierarchy for NLM framework
 * 
 * This header defines a comprehensive exception system for the NLM framework with
 * specific exception types for different error conditions. The exception system
 * provides detailed error information, error codes, and source location tracking
 * for robust error handling throughout the neural network simulation.
 * 
 * @details The exception system features:
 * - Hierarchical exception classes for organized error handling
 * - Error codes categorizing different types of errors
 * - Source location tracking (file, line, function)
 * - Convenience macros for consistent error throwing
 * - Stream-style exception throwing for complex error messages
 * - Performance optimizations for error reporting
 * 
 * @note The exception system is used throughout the NLM framework to handle:
 * - Configuration errors (invalid parameters, missing files)
 * - Memory allocation errors
 * - Runtime errors (invalid state, timeouts)
 * - Validation errors (input validation)
 * - File I/O errors
 * - Checkpoint errors
 * - Plasticity-related errors
 * - Neuromodulation errors
 * - Neural dynamics errors
 * - Motor system errors
 * - World/environment errors
 * - Sensory system errors
 * - Prediction system errors
 * - Cognition system errors
 * - Agent-related errors
 * - Experiment errors
 * - Visualization errors
 * - System-level errors
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Exception handling should be used sparingly in performance-critical
 * code paths. Consider using assertion macros (NLM_THROW_ASSERT) for debugging
 * and recovery conditions, and proper exception handling for error cases.
 * 
 * @see NLMException for base exception class
 * @see NLMRuntimeError for runtime errors
 * @see NLMMemoryError for memory errors
 * @see NLMConfigError for configuration errors
 * @see NLMValidationError for validation errors
 * @see NLMAssertionError for assertion errors
 * @see NLMException::ErrorCode for error code enumeration
 * 
 * @ingroup CoreComponents
 */
#pragma once

#include <string>
#include <stdexcept>
#include <iostream>

namespace nlm {

/**
 * @enum ErrorCode
 * @brief Enumeration of all possible error codes in the NLM framework
 * 
 * ErrorCode provides a comprehensive set of error codes categorizing different
 * types of errors that can occur in the NLM framework. Each error code has a
 * specific meaning and is used by corresponding exception classes.
 * 
 * @details Error codes are organized by error category:
 * - SUCCESS (0): Operation completed successfully
 * - UNKNOWN (1): General error, unspecified cause
 * - INVALID_ARGUMENT (2): Invalid input parameters
 * - OUT_OF_RANGE (3): Value outside valid range
 * - NOT_IMPLEMENTED (4): Feature not yet implemented
 * - INVALID_STATE (5): Invalid system state
 * - TIMEOUT (6): Operation timed out
 * - MEMORY_ALLOCATION (7): Memory allocation failure
 * - FILE_NOT_FOUND (8): Required file not found
 * - PERMISSION_DENIED (9): Insufficient permissions
 * - NETWORK_ERROR (10): Network-related error
 * - CONFIGURATION (11): Configuration error
 * - VALIDATION (12): Input validation error
 * - CHECKPOINT (13): Checkpoint/save/restore error
 * - CONVERSION (14): Data conversion error
 * - PLASTICITY (15): Plasticity rule error
 * - NEUROMODULATION (16): Neuromodulator error
 * - NEURAL_DYNAMICS (17): Neural dynamics error
 * - MOTOR (18): Motor system error
 * - WORLD (19): Environment/world error
 * - SENSORY (20): Sensory system error
 * - PREDICTION (21): Prediction system error
 * - COGNITION (22): Cognition system error
 * - AGENT (23): Agent-related error
 * - EXPERIMENT (24): Experiment error
 * - VISUALIZATION (25): Visualization error
 * - SYSTEM (26): General system error
 * 
 * Error codes are used by the exception hierarchy to provide specific error
 * handling and recovery strategies. Each error code corresponds to a specific
 * exception class in the NLMException hierarchy.
 * 
 * @see NLMException for base exception class
 * @see NLMRuntimeError for runtime exceptions
 * @see NLMConfigError for configuration exceptions
 * @see NLMMemoryError for memory exceptions
 */
enum class ErrorCode {
    SUCCESS = 0,                  ///< Operation completed successfully
    UNKNOWN = 1,                  ///< General error, unspecified cause
    INVALID_ARGUMENT = 2,         ///< Invalid input parameters
    OUT_OF_RANGE = 3,             ///< Value outside valid range
    NOT_IMPLEMENTED = 4,          ///< Feature not yet implemented
    INVALID_STATE = 5,            ///< Invalid system state
    TIMEOUT = 6,                  ///< Operation timed out
    MEMORY_ALLOCATION = 7,        ///< Memory allocation failure
    FILE_NOT_FOUND = 8,           ///< Required file not found
    PERMISSION_DENIED = 9,        ///< Insufficient permissions
    NETWORK_ERROR = 10,           ///< Network-related error
    CONFIGURATION = 11,           ///< Configuration error
    VALIDATION = 12,              ///< Input validation error
    CHECKPOINT = 13,              ///< Checkpoint/save/restore error
    CONVERSION = 14,              ///< Data conversion error
    PLASTICITY = 15,              ///< Plasticity rule error
    NEUROMODULATION = 16,         ///< Neuromodulator error
    NEURAL_DYNAMICS = 17,         ///< Neural dynamics error
    MOTOR = 18,                   ///< Motor system error
    WORLD = 19,                   ///< Environment/world error
    SENSORY = 20,                 ///< Sensory system error
    PREDICTION = 21,              ///< Prediction system error
    COGNITION = 22,               ///< Cognition system error
    AGENT = 23,                   ///< Agent-related error
    EXPERIMENT = 24,              ///< Experiment error
    VISUALIZATION = 25,           ///< Visualization error
    SYSTEM = 26                   ///< General system error
};

/**
 * @class NLMException
 * @brief Base exception class for all NLM errors
 * 
 * NLMException provides the foundation for all exceptions in the NLM framework.
 * It extends std::runtime_error to provide NLM-specific functionality including
 * error codes, source location tracking, and formatted error messages.
 * 
 * @details NLMException features:
 * - Comprehensive error code support (26 different error types)
 * - Source location tracking (file, line, function)
 * - Formatted error message with location information
 * - Virtual `what()` method with source location in message
 * - Conversion from other exception types
 * - Error code and source information access
 * 
 * The exception class provides detailed error information for debugging and
 * troubleshooting. Error codes allow for specific error handling strategies
 * across different parts of the framework.
 * 
 * @note NLMException is designed for:
 * - Detailed error reporting and debugging
 * - Specific error type identification
 * - Source location tracking for debugging
 * - Consistent error handling across the framework
 * 
 * @see ErrorCode for all possible error codes
 * @see NLMRuntimeError for runtime errors
 * @see NLMConfigError for configuration errors
 * @see NLMMemoryError for memory errors
 * @see NLMAssertionError for assertion errors
 * @see NLMRuntimeError for common runtime exceptions
 * 
 * @warning Exception objects are typically thrown by value and may be
 * copied or moved. Ensure proper exception handling in production code.
 * 
 * @ingroup CoreComponents
 */
class NLMException : public std::runtime_error {
public:
    /**
     * @brief Construct an NLM exception with error message and code
     * 
     * Creates a comprehensive NLM exception with detailed error information.
     * The exception includes source location information automatically
     * captured from the __FILE__, __LINE__, and __func__ macros.
     * 
     * @param message Human-readable error description
     * @param code Error code (default: ErrorCode::UNKNOWN)
     * @param file Source file name (default: "", captured automatically)
     * @param line Line number in source file (default: 0, captured automatically)
     * @param function Function name where exception was thrown (default: "", captured automatically)
     * 
     * @throws None (constructor should not throw)
     * 
     * @note This constructor captures and stores source location information
     * from the macro parameters. When called directly, the file, line, and
     * function parameters default to empty strings and will be set to the
     * actual values when called from macro expansions.
     * 
     * @see formatMessage() for message formatting implementation
     * @see ErrorCode for available error codes
     * @see NLMRuntimeError for runtime exception constructor
     * 
     * @par Example
     * @code
     * // Standard exception throwing
     * try {
     *     nlm::Config config;
     *     if (!config.loadFromFile("config.json")) {
     *         throw nlm::NLMException("Failed to load configuration file");
     *     }
     * } catch (const nlm::NLMException& e) {
     *     std::cout << "Error: " << e.what() << std::endl;
     *     std::cout << "Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
     * }
     * 
     * // Macro-style exception throwing
     * NLM_THROW_VALIDATION("Invalid configuration parameter");
     * @endcode
     */
    NLMException(const std::string& message, 
                  ErrorCode code = ErrorCode::UNKNOWN,
                  const std::string& file = "",
                  int line = 0,
                  const std::string& function = "");
    
    /**
     * @brief Get the error code associated with this exception
     * 
     * @return ErrorCode Error code for this exception
     * 
     * @see ErrorCode for all possible error codes
     * @see what() for error message
     */
    ErrorCode getErrorCode() const;
    
    /**
     * @brief Get the source file name where exception was thrown
     * 
     * @return const std::string& Source file name
     * 
     * @note This provides debugging information for exception handling.
     * In most cases, the file parameter is captured from the __FILE__ macro
     * automatically.
     */
    const std::string& getFile() const;
    
    /**
     * @brief Get the line number where exception was thrown
     * 
     * @return int Line number in source file
     * 
     * @note This provides debugging information for exception handling.
     * In most cases, the line parameter is captured from the __LINE__ macro
     * automatically.
     */
    int getLine() const;
    
    /**
     * @brief Get the function name where exception was thrown
     * 
     * @return const std::string& Function name
     * 
     * @note This provides debugging information for exception handling.
     * In most cases, the function parameter is captured from the __func__
     * macro automatically.
     */
    const std::string& getFunction() const;
    
    /**
     * @brief Get the formatted error message
     * 
     * @return const char* Formatted error message including source location
     * 
     * @note The message includes source location information if available,
     * making it easier to debug where exceptions occur in the codebase.
     */
    const char* what() const noexcept override;
    
protected:
    /**
     * @brief Format the error message with source location information
     * 
     * This protected method formats the error message by combining the original
     * message with source location information (file, line, function) when
     * available. It provides the implementation for the what() method.
     * 
     * @param message Original error message
     * @param file Source file name
     * @param line Line number in source file
     * @param function Function name where exception was thrown
     * @return std::string Formatted error message
     * 
     * @note The formatting produces output like:
     * "Error message [file:line in function]"
     * 
     * @see what() for formatted message usage
     */
    virtual std::string formatMessage(const std::string& message,
                                      const std::string& file,
                                      int line,
                                      const std::string& function);
    
private:
    ErrorCode errorCode_;             ///< Error code for this exception
    std::string file_;                ///< Source file name where exception was thrown
    int line_;                        ///< Line number where exception was thrown
    std::string function_;            ///< Function name where exception was thrown
    std::string formattedMessage_;    ///< Formatted error message for what() method
};

/**
 * @class NLMRuntimeError
 * @brief Exception for runtime errors (unexpected state or behavior)
 * 
 * NLMRuntimeError represents errors that occur during normal runtime execution
 * but are not necessarily fatal. These are typically errors that indicate
 * invalid state, unexpected behavior, or recoverable conditions.
 * 
 * @details NLMRuntimeError is used for:
 * - Invalid system state errors
 * - Unexpected behavior errors
 * - Recoverable runtime conditions
 * - Logic errors during execution
 * 
 * @note NLMRuntimeError inherits from NLMException with ErrorCode::UNKNOWN
 * as the default error code. It is the most general runtime exception type.
 * 
 * @see NLMException for base class
 * @see NLMAssertionError for assertion-related errors
 * @see NLMConfigError for configuration errors
 * 
 * @par Example
 * @code
 * try {
 *     nlm::Brain brain;
 *     // ... simulation code ...
 *     if (brain.isInInvalidState()) {
 *         throw nlm::NLMRuntimeError("Brain is in invalid state for simulation");
 *     }
 * } catch (const nlm::NLMRuntimeError& e) {
 *     std::cout << "Runtime error: " << e.what() << std::endl;
 * }
 * @endcode
 */
class NLMRuntimeError : public NLMException {
public:
    NLMRuntimeError(const std::string& message,
                    ErrorCode code = ErrorCode::UNKNOWN,
                    const std::string& file = "",
                    int line = 0,
                    const std::string& function = "")
        : NLMException(message, code, file, line, function) {}
};

/**
 * @class NLMMemoryError
 * @brief Exception for memory-related errors
 * 
 * NLMMemoryError represents errors that occur during memory allocation or
 * deallocation operations. These are typically fatal errors that indicate
 * insufficient memory or memory corruption.
 * 
 * @details NLMMemoryError is used for:
 * - Memory allocation failures
 * - Memory corruption detection
 * - Memory pool errors
 * - Out-of-memory conditions
 * 
 * @note NLMMemoryError inherits from NLMException with ErrorCode::MEMORY_ALLOCATION
 * as the default error code. This helps distinguish memory errors from other
 * types of runtime errors.
 * 
 * @see NLMException for base class
 * @see ErrorCode::MEMORY_ALLOCATION for error code
 * @see NLMCheckpointError for checkpoint-related memory errors
 * 
 * @par Example
 * @code
 * try {
 *     // Attempt large memory allocation
 *     std::vector<double> largeVector(1000000000);
 * } catch (const nlm::NLMMemoryError& e) {
 *     std::cout << "Memory error: " << e.what() << std::endl;
 *     std::cout << "Suggestion: Increase available memory or reduce memory usage" << std::endl;
 * }
 * @endcode
 */
class NLMMemoryError : public NLMException {
public:
    NLMMemoryError(const std::string& message,
                   ErrorCode code = ErrorCode::MEMORY_ALLOCATION,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "")
        : NLMException(message, code, file, line, function) {}
};

/**
 * @class NLMConfigError
 * @brief Exception for configuration errors
 * 
 * NLMConfigError represents errors that occur during configuration operations,
 * such as invalid configuration values, missing configuration files, or
 * configuration parsing failures.
 * 
 * @details NLMConfigError is used for:
 * - Invalid configuration parameter values
 * - Missing or malformed configuration files
 * - Configuration parsing failures
 * - Invalid configuration combinations
 * - Configuration validation failures
 * 
 * @note NLMConfigError inherits from NLMException with ErrorCode::CONFIGURATION
 * as the default error code. This helps applications handle configuration
 * errors specifically.
 * 
 * @see NLMException for base class
 * @see ErrorCode::CONFIGURATION for error code
 * @see NLMException::ErrorCode::FILE_NOT_FOUND for file-related configuration errors
 * @see NLMException::ErrorCode::VALIDATION for validation errors
 * 
 * @par Example
 * @code
 * try {
 *     nlm::Config config;
 *     if (!config.loadFromFile("missing_config.json")) {
 *         throw nlm::NLMConfigError("Cannot load configuration file");
 *     }
 * } catch (const nlm::NLMConfigError& e) {
 *     std::cout << "Configuration error: " << e.what() << std::endl;
 *     std::cout << "Please check configuration file syntax and permissions" << std::endl;
 * }
 * @endcode
 */
class NLMConfigError : public NLMException {
public:
    NLMConfigError(const std::string& message,
                   ErrorCode code = ErrorCode::CONFIGURATION,
                   const std::string& file = "",
                   int line = 0,
                   const std::string& function = "")
        : NLMException(message, code, file, line, function) {}
};

/**
 * @class NLMValidationError
 * @brief Exception for validation errors (input validation)
 * 
 * NLMValidationError represents errors that occur during input validation,
 * such as invalid parameter values, data type mismatches, or range violations.
 * These are typically errors that can be caught early in the execution flow.
 * 
 * @details NLMValidationError is used for:
 * - Invalid parameter values
 * - Data type mismatches
 * - Range violations
 * - Constraint violations
 * - Input validation failures
 * 
 * @note NLMValidationError inherits from NLMException with ErrorCode::VALIDATION
 * as the default error code. This helps applications handle validation errors
 * specifically and provides early error detection.
 * 
 * @see NLMException for base class
 * @see ErrorCode::VALIDATION for error code
 * @see NLMConfigError for configuration validation errors
 * @see NLMException::ErrorCode::INVALID_ARGUMENT for argument validation errors
 * 
 * @par Example
 * @code
 * try {
 *     int value = getSomeValue();
 *     if (value < 0 || value > 100) {
 *         throw nlm::NLMValidationError("Value must be between 0 and 100");
 *     }
 * } catch (const nlm::NLMValidationError& e) {
 *     std::cout << "Validation error: " << e.what() << std::endl;
 *     std::cout << "Please provide a valid value" << std::endl;
 * }
 * @endcode
 */
class NLMValidationError : public NLMException {
public:
    NLMValidationError(const std::string& message,
                       ErrorCode code = ErrorCode::VALIDATION,
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "")
        : NLMException(message, code, file, line, function) {}
};

/**
 * @class NLMFileError
 * @brief Exception for file I/O errors
 * 
 * NLMFileError represents errors that occur during file operations, such as
 * file not found, permission denied, or file read/write errors.
 * 
 * @details NLMFileError is used for:
 * - File not found errors
 * - Permission denied errors
 * - File read/write errors
 * - File format errors
 * - Disk space errors
 * 
 * @note NLMFileError inherits from NLMException with ErrorCode::FILE_NOT_FOUND
 * as the default error code. This helps applications handle file-related
 * errors specifically.
 * 
 * @see NLMException for base class
 * @see ErrorCode::FILE_NOT_FOUND for error code
 * @see NLMConfigError for configuration file errors
 * @see NLMCheckpointError for checkpoint file errors
 * 
 * @par Example
 * @code
 * try {
 *     nlm::Config config;
 *     if (!config.saveToFile("/root/protected_config.json")) {
 *         throw nlm::NLMFileError("Permission denied writing to configuration file");
 *     }
 * } catch (const nlm::NLMFileError& e) {
 *     std::cout << "File error: " << e.what() << std::endl;
 *     std::cout << "Check file permissions and disk space" << std::endl;
 * }
 * @endcode
 */
class NLMFileError : public NLMException {
public:
    NLMFileError(const std::string& message,
                 ErrorCode code = ErrorCode::FILE_NOT_FOUND,
                 const std::string& file = "",
                 int line = 0,
                 const std::string& function = "")
        : NLMException(message, code, file, line, function) {}
};

/**
 * @class NLMCheckpointError
 * @brief Exception for checkpoint-related errors
 * 
 * NLMCheckpointError represents errors that occur during checkpoint save or
 * restore operations, such as serialization errors, file format errors, or
 * checkpoint corruption.
 * 
 * @details NLMCheckpointError is used for:
 * - Checkpoint file errors
 * - Serialization/deserialization errors
 * - Checkpoint corruption errors
 * - Memory errors during checkpoint operations
 * - File I/O errors during checkpoint operations
 * 
 * @note NLMCheckpointError inherits from NLMException with ErrorCode::CHECKPOINT
 * as the default error code. This helps applications handle checkpoint errors
 * specifically and recover from checkpoint failures.
 * 
 * @see NLMException for base class
 * @see ErrorCode::CHECKPOINT for error code
 * @see NLMFileError for file-related errors
 * @see NLMMemoryError for memory-related errors
 * 
 * @par Example
 * @code
 * try {
 *     nlm::CheckpointSystem checkpoint;
 *     checkpoint.save("experiment_1.checkpoint");
 * } catch (const nlm::NLMCheckpointError& e) {
 *     std::cout << "Checkpoint error: " << e.what() << std::endl;
 *     std::cout << "Check disk space and file permissions" << std::endl;
 * }
 * @endcode
 */
class NLMCheckpointError : public NLMException {
public:
    NLMCheckpointError(const std::string& message,
                       ErrorCode code = ErrorCode::CHECKPOINT,
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "")
        : NLMException(message, code, file, line, function) {}
};

/**
 * @class NLMAssertionError
 * @brief Exception for assertion errors (formerly assert() calls)
 * 
 * NLMAssertionError represents errors that occur when program invariants
 * are violated. These are typically programming errors that should not
 * occur in correct code.
 * 
 * @details NLMAssertionError is used for:
 * - Programming errors (null pointer dereferences)
 * - Logic errors
 * - Invalid assumptions
 * - Debugging assertions
 * 
 * @note NLMAssertionError inherits from NLMException with ErrorCode::INVALID_STATE
 * as the default error code. This helps distinguish assertion errors from other
 * types of runtime errors.
 * 
 * @see NLMException for base class
 * @see ErrorCode::INVALID_STATE for error code
 * @see NLMRuntimeError for runtime errors
 * @see NLMValidationError for validation errors
 * 
 * @par Example
 * @code
 * try {
 *     nlm::Neuron neuron(nullptr); // Invalid pointer
 * } catch (const nlm::NLMAssertionError& e) {
 *     std::cout << "Assertion error: " << e.what() << std::endl;
 *     std::cout << "This indicates a programming error" << std::endl;
 * }
 * @endcode
 */
class NLMAssertionError : public NLMException {
public:
    NLMAssertionError(const std::string& message,
                      const std::string& file = "",
                      int line = 0,
                      const std::string& function = "")
        : NLMException(message, ErrorCode::INVALID_STATE, file, line, function) {}
};

/**
 * @class NLMTestException
 * @brief Exception for test-related errors
 * 
 * NLMTestException represents errors that occur during testing, such as
 * test failures, invalid test conditions, or test setup errors.
 * 
 * @details NLMTestException is used for:
 * - Test failures
 * - Invalid test conditions
 * - Test setup errors
 * - Test teardown errors
 * 
 * @note NLMTestException inherits from NLMException with ErrorCode::UNKNOWN
 * as the default error code. This helps distinguish test errors from other
 * types of runtime errors.
 * 
 * @see NLMException for base class
 * @see ErrorCode::UNKNOWN for error code
 * 
 * @par Example
 * @code
 * try {
 *     // Test code
 *     nlm::runTest("neuron_test");
 * } catch (const nlm::NLMTestException& e) {
 *     std::cout << "Test error: " << e.what() << std::endl;
 *     std::cout << "Test failed" << std::endl;
 * }
 * @endcode
 */
class NLMTestException : public NLMException {
public:
    NLMTestException(const std::string& message,
                     const std::string& file = "",
                     int line = 0,
                     const std::string& function = "")
        : NLMException(message, ErrorCode::UNKNOWN, file, line, function) {}
};

// Convenience macro for throwing validation errors
/**
 * @def NLM_THROW_VALIDATION(message)
 * @brief Convenience macro for throwing validation errors
 * 
 * This macro expands to a call to NLMValidationError constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw validation errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::VALIDATION for error code
 * @see NLMValidationError for exception class
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 * @see NLM_THROW_TEST Macro for test error macro
 * 
 * @par Example
 * @code
 * try {
 *     int value = getConfigurationValue("learning_rate");
 *     if (value <= 0.0) {
 *         NLM_THROW_VALIDATION("Learning rate must be positive");
 *     }
 * } catch (const nlm::NLMValidationError& e) {
 *     std::cout << "Validation error caught: " << e.what() << std::endl;
 * }
 * @endcode
 */
#define NLM_THROW_VALIDATION(message) throw nlm::NLMValidationError(message, nlm::NLMException::ErrorCode::VALIDATION, __FILE__, __LINE__, __func__)

// Convenience macro for throwing runtime errors  
/**
 * @def NLM_THROW_RUNTIME(message)
 * @brief Convenience macro for throwing runtime errors
 * 
 * This macro expands to a call to NLMRuntimeError constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw runtime errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::UNKNOWN for error code
 * @see NLMRuntimeError for exception class
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 * @see NLM_THROW_TEST Macro for test error macro
 */
#define NLM_THROW_RUNTIME(message) throw nlm::NLMRuntimeError(message, nlm::NLMException::ErrorCode::UNKNOWN, __FILE__, __LINE__, __func__)

// Convenience macro for throwing memory errors
/**
 * @def NLM_THROW_MEMORY(message)
 * @brief Convenience macro for throwing memory errors
 * 
 * This macro expands to a call to NLMMemoryError constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw memory errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::MEMORY_ALLOCATION for error code
 * @see NLMMemoryError for exception class
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 * @see NLM_THROW_TEST Macro for test error macro
 */
#define NLM_THROW_MEMORY(message) throw nlm::NLMMemoryError(message, nlm::NLMException::ErrorCode::MEMORY_ALLOCATION, __FILE__, __LINE__, __func__)

// Convenience macro for throwing file errors
/**
 * @def NLM_THROW_FILE(message)
 * @brief Convenience macro for throwing file errors
 * 
 * This macro expands to a call to NLMFileError constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw file errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::FILE_NOT_FOUND for error code
 * @see NLMFileError for exception class
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 * @see NLM_THROW_TEST Macro for test error macro
 */
#define NLM_THROW_FILE(message) throw nlm::NLMFileError(message, nlm::NLMException::ErrorCode::FILE_NOT_FOUND, __FILE__, __LINE__, __func__)

// Convenience macro for throwing checkpoint errors
/**
 * @def NLM_THROW_CHECKPOINT(message)
 * @brief Convenience macro for throwing checkpoint errors
 * 
 * This macro expands to a call to NLMCheckpointError constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw checkpoint errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::CHECKPOINT for error code
 * @see NLMCheckpointError for exception class
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 * @see NLM_THROW_TEST Macro for test error macro
 */
#define NLM_THROW_CHECKPOINT(message) throw nlm::NLMCheckpointError(message, nlm::NLMException::ErrorCode::CHECKPOINT, __FILE__, __LINE__, __func__)

// Convenience macro for throwing assertion errors
/**
 * @def NLM_THROW_ASSERT(message)
 * @brief Convenience macro for throwing assertion errors
 * 
 * This macro expands to a call to NLMAssertionError constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw assertion errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::INVALID_STATE for error code
 * @see NLMAssertionError for exception class
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_TEST Macro for test error macro
 */
#define NLM_THROW_ASSERT(message) throw nlm::NLMAssertionError(message, __FILE__, __LINE__, __func__)

// Convenience macro for throwing test errors
/**
 * @def NLM_THROW_TEST(message)
 * @brief Convenience macro for throwing test errors
 * 
 * This macro expands to a call to NLMTestException constructor with the
 * specified message and appropriate error code. It provides a convenient
 * way to throw test errors with consistent formatting and error code.
 * 
 * @param message Error message to throw
 * 
 * @see NLMException::ErrorCode::UNKNOWN for error code
 * @see NLMTestException for exception class
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 */
#define NLM_THROW_TEST(message) throw nlm::NLMTestException(message, __FILE__, __LINE__, __func__)

/**
 * @def NLM_THROW_STREAM(exception_type)
 * @brief Stream-style exception throwing macro
 * 
 * This macro expands to a stream-style exception throwing mechanism that
 * constructs the exception with the message in the constructor. This provides
 * an alternative to traditional exception throwing for complex error messages.
 * 
 * @param exception_type Exception class to throw
 * 
 * @note This macro provides a convenient way to throw exceptions with
 * formatted messages. The exception is constructed with the message in its
 * constructor, allowing for more complex error message construction.
 * 
 * @see NLM_THROW_VALIDATION Macro for validation error macro
 * @see NLM_THROW_RUNTIME Macro for runtime error macro
 * @see NLM_THROW_MEMORY Macro for memory error macro
 * @see NLM_THROW_FILE Macro for file error macro
 * @see NLM_THROW_CHECKPOINT Macro for checkpoint error macro
 * @see NLM_THROW_ASSERT Macro for assertion error macro
 * @see NLM_THROW_TEST Macro for test error macro
 */
#define NLM_THROW_STREAM(exception_type) \
    do { \
        class _NLMThrowStream : public exception_type { \
        public: \
            _NLMThrowStream(const std::string& msg, const std::string& file, int line, const std::string& func) : exception_type(msg, file, line, func) {} \
        }; \
        _NLMThrowStream _nlm_exc_obj; \
    } while (0)

} // namespace nlm
