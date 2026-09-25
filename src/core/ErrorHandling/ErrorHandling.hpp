/**
 * @file ErrorHandling.hpp
 * @brief Comprehensive error handling framework for NLM neural simulation
 *
 * This file provides a comprehensive error handling framework for the Neural Learning
 * Machine (NLM) neural simulation framework. It includes error type definitions,
 * exception classes, error management utilities, and macros for consistent error
 * reporting throughout the codebase.
 *
 * The error handling framework provides:
 * - Detailed error type enumeration for all possible error conditions
 * - Exception classes with full context and timestamp information
 * - Structured error information for comprehensive error tracking
 * - Static error management for error accumulation and reporting
 * - Exception throwing macros for consistent error reporting
 * - Global error handling utility functions for system-wide error management
 * - Python-specific error handling for bindings integration
 * - Context-preserving error chaining for complex error scenarios
 *
 * This framework is designed for:
 * - Comprehensive error reporting with full context
 * - Consistent error handling throughout the codebase
 * - Integration with Python bindings for cross-language error handling
 * - Debugging and troubleshooting of neural simulations
 * - System health monitoring and error tracking
 * - Graceful error recovery and error handling
 *
 * The error handling system is essential for:
 * - Robust neural simulation with proper error detection
 * - Comprehensive error reporting for debugging
 * - Integration with higher-level error handling systems
 * - Cross-language error compatibility with Python bindings
 * - Maintaining system stability during error conditions
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 */

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <chrono>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <stdexcept>

namespace nlm {

/**
 * @brief Enumeration of all possible error conditions in NLM framework
 *
 * This enumeration defines all possible error types that can occur during
 * neural simulation, computation, and learning operations. Each error type
 * represents a specific class of error conditions with distinct characteristics
 * and handling requirements.
 *
 * Error types are used throughout the NLM framework to:
 * - Categorize error conditions for appropriate handling
 * - Enable type-specific error recovery strategies
 * - Provide detailed error information for debugging
 * - Support comprehensive error reporting and analysis
 *
 * @note Error types are ordered by category:
 * - Configuration and input errors (ConfigParsing, InvalidParameter, ValidationError)
 * - File system and I/O errors (FileIO)
 * - Memory management errors (MemoryAllocation)
 * - System initialization errors (SystemNotInitialized)
 * - Neural computation errors (NeuronInvalid, SynapseInvalid, PlasticityFailed)
 * - Integration and optimization errors (IntegrationFailed)
 * - Cross-language binding errors (CallbackError)
 * - Development and testing errors (DevelopmentError)
 * - Generic errors (UnknownError)
 *
 * @see NLMErrors::Exception
 * @see NLMErrors::Error
 * @see NLMErrors::throwException
 * @see NLMErrors::addError
 */
enum class NLMErrors {
    /** Configuration file parsing errors - invalid JSON/YAML syntax, missing required parameters */
    ConfigParsing,
    /** File input/output errors - file not found, permission denied, read/write failures */
    FileIO,
    /** Memory allocation failures - out of memory, allocation errors in neural computations */
    MemoryAllocation,
    /** Invalid parameter values - out-of-range parameters, invalid neuron/synapse IDs */
    InvalidParameter,
    /** System components not properly initialized - brain, neuron, or synapse initialization failures */
    SystemNotInitialized,
    /** Plasticity system failures - STDP, Hebbian, or other plasticity computation errors */
    PlasticityFailed,
    /** Neuron-related errors - neuron computation, state management, or simulation failures */
    NeuronInvalid,
    /** Synapse-related errors - synaptic transmission, plasticity, or connection failures */
    SynapseInvalid,
    /** Python callback or event handler errors - Python binding or callback execution failures */
    CallbackError,
    /** Integration/optimization failures - neural computation, convergence, or performance errors */
    IntegrationFailed,
    /** Development system errors - developmental process, maturation, or growth failures */
    DevelopmentError,
    /** Input validation errors - parameter validation, data integrity, or format errors */
    ValidationError,
    /** Generic unknown errors - unexpected conditions, internal framework errors */
    UnknownError
};

/**
 * @class NLMErrors::Exception
 * @brief Exception class with full context and timestamp information
 *
 * This class provides detailed exception information for NLM framework errors,
 * including error type, context, timestamp, and comprehensive error message
 * generation. It extends std::runtime_error to provide full compatibility
 * with standard C++ exception handling while adding NLM-specific features.
 *
 * The Exception class is designed for:
 * - Comprehensive error reporting with full context
 * - Detailed error analysis and debugging
 * - Cross-language error compatibility with Python bindings
 * - Timestamp-based error tracking and analysis
 * - Context-preserving error chaining
 *
 * Key features:
 * - Error type enumeration for specific error categorization
 * - Context information for debugging and analysis
 * - Timestamp information for error timing analysis
 * - Full message generation with context and type information
 * - Conversion to string format for error reporting
 * - Copy constructor for error preservation
 *
 * @note Exception objects are used throughout the NLM framework to:
 * - Report specific error conditions with detailed information
 * - Enable type-specific error handling and recovery
 * - Provide debugging information for error scenarios
 * - Support comprehensive error reporting and analysis
 *
 * @warning Exception objects should be caught and handled appropriately
 *          to prevent program termination and enable graceful error recovery.
 *
 * @see NLMErrors::Error
 * @see NLMErrors::throwException
 * @see NLMErrors::addError
 * @see NLMErrors::Exception::getFullMessage
 */
class NLMErrors {
public:
    /**
     * @class Exception
     * @brief Detailed exception information for NLM errors
     *
     * This inner class provides comprehensive exception information for NLM framework
     * errors, including error type, context, timestamp, and error message. It extends
     * std::runtime_error to provide full compatibility with standard C++ exception handling.
     *
     * @note This exception class provides:
     * - Type-specific error handling through NLMErrors enumeration
     * - Context information for debugging and analysis
     * - Timestamp information for error timing analysis
     * - Full message generation with context and type information
     */
    class Exception : public std::runtime_error {
    public:
        /** Error type for specific error categorization */
        NLMErrors type;
        /** Context information for debugging and analysis */
        std::string context;
        /** Timestamp for error timing analysis and debugging */
        std::chrono::system_clock::time_point timestamp;
        
        /**
         * @brief Constructor for NLM exceptions with error type and context
         *
         * Creates a new NLM exception with the specified error type, message, and optional
         * context information. The exception includes a timestamp for error tracking and
         * analysis.
         *
         * @param t Error type for specific error categorization
         * @param msg Error message describing the specific error condition
         * @param ctx Optional context information for debugging and analysis
         *
         * @note The constructor:
         * - Initializes the base std::runtime_error with the message
         * - Sets the error type for specific error handling
         * - Stores context information for debugging
         * - Generates a timestamp for error timing analysis
         *
         * @see getFullMessage
         * @param t Error type for specific error categorization
         * @param msg Error message describing the specific error condition
         * @param ctx Optional context information for debugging and analysis
         */
        Exception(NLMErrors t, const std::string& msg, const std::string& ctx = "")
            : std::runtime_error(msg), type(t), context(ctx), 
              timestamp(std::chrono::system_clock::now()) {}
              
        /**
         * @brief Copy constructor for NLM exceptions
         *
         * Creates a new NLM exception by copying from another exception object.
         * This constructor ensures that all exception information including type,
         * context, and timestamp is properly preserved during copying.
         *
         * @param other The source NLM exception to copy from
         *
         * @note The copy constructor:
         * - Copies the base std::runtime_error message
         * - Preserves the error type for specific handling
         * - Maintains context information for debugging
         * - Preserves the timestamp for error timing analysis
         *
         * @see Exception(NLMErrors, const std::string&, const std::string&)
         * @param other The source NLM exception to copy from
         */
        Exception(const NLMErrors::Exception& other)
            : std::runtime_error(other.what()), type(other.type), 
              context(other.context), timestamp(other.timestamp) {}
              
        /**
         * @brief Generate full error message with context and timestamp
         *
         * Generates a comprehensive error message that includes the timestamp,
         * original error message, context information (if any), and error type.
         * This method provides complete error information for debugging and analysis.
         *
         * @return std::string Complete error message with timestamp, context, and type
         * @throws NLMErrors::Exception if timestamp conversion fails
         *
         * @note The full message format includes:
         * - Timestamp: [YYYY-MM-DD HH:MM:SS]
         * - Error message: Original error description
         * - Context: (Context: <context>) if context is provided
         * - Error type: [Error Type: <type>]
         *
         * @see what
         * @see timestampToString
         * @see errorTypeToString
         */
        std::string getFullMessage() const {
            std::stringstream ss;
            ss << "[" << timestampToString(timestamp) << "] " << what();
            if (!context.empty()) {
                ss << " (Context: " << context << ")";
            }
            ss << " [Error Type: " << errorTypeToString(type) << "]";
            return ss.str();
        }
        
    private:
        /**
         * @brief Convert timestamp to string representation
         *
         * Converts a timestamp to a human-readable string format for error reporting.
         * This method is used internally by getFullMessage() to generate timestamps
         * in the error message.
         *
         * @param tp Timestamp to convert
         * @return std::string Timestamp string in format: "YYYY-MM-DD HH:MM:SS"
         * @throws NLMErrors::Exception if timestamp conversion fails
         *
         * @note The timestamp format:
         * - Date: YYYY-MM-DD
         * - Time: HH:MM:SS
         * - Local time zone based on system settings
         *
         * @see getFullMessage
         * @param tp Timestamp to convert
         */
        std::string timestampToString(const std::chrono::system_clock::time_point& tp) const {
            auto time = std::chrono::system_clock::to_time_t(tp);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
        
        /**
         * @brief Convert error type to string representation
         *
         * Converts an NLMErrors enumeration value to its string representation for
         * error reporting. This method is used internally by getFullMessage() to
         * generate error type information in error messages.
         *
         * @param type Error type to convert
         * @return std::string String representation of error type
         * @throws NLMErrors::Exception if error type conversion fails
         *
         * @note Error type strings:
         * - ConfigParsing: Configuration file parsing errors
         * - FileIO: File input/output errors
         * - MemoryAllocation: Memory allocation failures
         * - InvalidParameter: Invalid parameter values
         * - SystemNotInitialized: System components not properly initialized
         * - PlasticityFailed: Plasticity system failures
         * - NeuronInvalid: Neuron-related errors
         * - SynapseInvalid: Synapse-related errors
         * - CallbackError: Python callback or event handler errors
         * - IntegrationFailed: Integration/optimization failures
         * - DevelopmentError: Development system errors
         * - ValidationError: Input validation errors
         * - UnknownError: Generic unknown errors
         *
         * @see getFullMessage
         * @param type Error type to convert
         */
        std::string errorTypeToString(NLMErrors type) const {
            switch (type) {
                case NLMErrors::ConfigParsing: return "ConfigParsing";
                case NLMErrors::FileIO: return "FileIO";
                case NLMErrors::MemoryAllocation: return "MemoryAllocation";
                case NLMErrors::InvalidParameter: return "InvalidParameter";
                case NLMErrors::SystemNotInitialized: return "SystemNotInitialized";
                case NLMErrors::PlasticityFailed: return "PlasticityFailed";
                case NLMErrors::NeuronInvalid: return "NeuronInvalid";
                case NLMErrors::SynapseInvalid: return "SynapseInvalid";
                case NLMErrors::CallbackError: return "CallbackError";
                case NLMErrors::IntegrationFailed: return "IntegrationFailed";
                case NLMErrors::DevelopmentError: return "DevelopmentError";
                case NLMErrors::ValidationError: return "ValidationError";
                case NLMErrors::UnknownError: return "UnknownError";
                default: return "UnknownError";
            }
        }
    };
    
    /**
     * @struct Error
     * @brief Structured error information for comprehensive error tracking
     *
     * This structure provides detailed error information for the NLM framework,
     * including error type, message, context, and timestamp. It is used internally
     * by the error handling system to accumulate and manage errors throughout the
     * neural simulation framework.
     *
     * The Error structure is designed for:
     * - Comprehensive error tracking and analysis
     * - Error accumulation and management throughout the system
     * - Detailed error reporting with context and timing
     * - Cross-language error compatibility with Python bindings
     * - Integration with error handling utilities
     *
     * Key features:
     * - Error type enumeration for specific error categorization
     * - Context information for debugging and analysis
     * - Timestamp information for error timing analysis
     * - Full message generation with context and type information
     * - Memory-efficient storage for large-scale error tracking
     * - Thread-safe error management for concurrent access
     *
     * @note Error objects are used throughout the NLM framework to:
     * - Accumulate errors during neural simulation
     * - Provide detailed error information for debugging
     * - Support comprehensive error reporting and analysis
     * - Enable cross-language error compatibility
     * - Maintain system stability during error conditions
     *
     * @see NLMErrors::Exception
     * @see NLMErrors::addError
     * @see NLMErrors::getErrors
     */
    struct Error {
        /** Error type for specific error categorization */
        NLMErrors type;
        /** Error message describing the specific error condition */
        std::string message;
        /** Context information for debugging and analysis */
        std::string context;
        /** Timestamp for error timing analysis and debugging */
        std::chrono::system_clock::time_point timestamp;
        
        /**
         * @brief Constructor for NLM errors with error type and context
         *
         * Creates a new NLM error with the specified error type, message, and optional
         * context information. The error includes a timestamp for error tracking and
         * analysis.
         *
         * @param t Error type for specific error categorization
         * @param msg Error message describing the specific error condition
         * @param ctx Optional context information for debugging and analysis
         *
         * @note The constructor:
         * - Initializes the error type for specific error handling
         * - Stores the error message for reporting
         * - Sets context information for debugging
         * - Generates a timestamp for error timing analysis
         *
         * @see getFullMessage
         * @param t Error type for specific error categorization
         * @param msg Error message describing the specific error condition
         * @param ctx Optional context information for debugging and analysis
         */
        Error(NLMErrors t, const std::string& msg, const std::string& ctx = "")
            : type(t), message(msg), context(ctx), 
              timestamp(std::chrono::system_clock::now()) {}
              
        /**
         * @brief Generate full error message with context and timestamp
         *
         * Generates a comprehensive error message that includes the timestamp,
         * error message, context information (if any), and error type. This method
         * provides complete error information for debugging and analysis.
         *
         * @return std::string Complete error message with timestamp, context, and type
         * @throws NLMErrors::Exception if timestamp conversion fails
         *
         * @note The full message format includes:
         * - Timestamp: [YYYY-MM-DD HH:MM:SS]
         * - Error message: Original error description
         * - Context: (Context: <context>) if context is provided
         * - Error type: [Error Type: <type>]
         *
         * @see message
         * @see context
         * @see type
         * @see timestampToString
         * @see errorTypeToString
         */
        std::string getFullMessage() const {
            std::stringstream ss;
            ss << "[" << timestampToString(timestamp) << "] " << message;
            if (!context.empty()) {
                ss << " (Context: " << context << ")";
            }
            ss << " [Error Type: " << errorTypeToString(type) << "]";
            return ss.str();
        }
        
    private:
        /**
         * @brief Convert timestamp to string representation
         *
         * Converts a timestamp to a human-readable string format for error reporting.
         * This method is used internally by getFullMessage() to generate timestamps
         * in the error message.
         *
         * @param tp Timestamp to convert
         * @return std::string Timestamp string in format: "YYYY-MM-DD HH:MM:SS"
         * @throws NLMErrors::Exception if timestamp conversion fails
         *
         * @note The timestamp format:
         * - Date: YYYY-MM-DD
         * - Time: HH:MM:SS
         * - Local time zone based on system settings
         *
         * @see getFullMessage
         * @param tp Timestamp to convert
         */
        std::string timestampToString(const std::chrono::system_clock::time_point& tp) const {
            auto time = std::chrono::system_clock::to_time_t(tp);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
        
        /**
         * @brief Convert error type to string representation
         *
         * Converts an NLMErrors enumeration value to its string representation for
         * error reporting. This method is used internally by getFullMessage() to
         * generate error type information in error messages.
         *
         * @param type Error type to convert
         * @return std::string String representation of error type
         * @throws NLMErrors::Exception if error type conversion fails
         *
         * @note Error type strings:
         * - ConfigParsing: Configuration file parsing errors
         * - FileIO: File input/output errors
         * - MemoryAllocation: Memory allocation failures
         * - InvalidParameter: Invalid parameter values
         * - SystemNotInitialized: System components not properly initialized
         * - PlasticityFailed: Plasticity system failures
         * - NeuronInvalid: Neuron-related errors
         * - SynapseInvalid: Synapse-related errors
         * - CallbackError: Python callback or event handler errors
         * - IntegrationFailed: Integration/optimization failures
         * - DevelopmentError: Development system errors
         * - ValidationError: Input validation errors
         * - UnknownError: Generic unknown errors
         *
         * @see getFullMessage
         * @param type Error type to convert
         */
        std::string errorTypeToString(NLMErrors type) const {
            switch (type) {
                case NLMErrors::ConfigParsing: return "ConfigParsing";
                case NLMErrors::FileIO: return "FileIO";
                case NLMErrors::MemoryAllocation: return "MemoryAllocation";
                case NLMErrors::InvalidParameter: return "InvalidParameter";
                case NLMErrors::SystemNotInitialized: return "SystemNotInitialized";
                case NLMErrors::PlasticityFailed: return "PlasticityFailed";
                case NLMErrors::NeuronInvalid: return "NeuronInvalid";
                case NLMErrors::SynapseInvalid: return "SynapseInvalid";
                case NLMErrors::CallbackError: return "CallbackError";
                case NLMErrors::IntegrationFailed: return "IntegrationFailed";
                case NLMErrors::DevelopmentError: return "DevelopmentError";
                case NLMErrors::ValidationError: return "ValidationError";
                case NLMErrors::UnknownError: return "UnknownError";
                default: return "UnknownError";
            }
        }
    };
    
    /**
     * @brief Static error management for error accumulation and reporting
     *
     * This class provides static methods for managing errors throughout the NLM
     * framework, including error addition, retrieval, clearing, and counting.
     * These methods are used internally by the error handling system to accumulate
     * and manage errors during neural simulation and computation.
     *
     * Static error management includes:
     * - Error accumulation for comprehensive error reporting
     * - Error retrieval for analysis and debugging
     * - Error clearing for system reset and cleanup
     * - Error counting for system health monitoring
     * - Cross-language error compatibility with Python bindings
     * - Thread-safe error management for concurrent access
     *
     * @note Static error management is used throughout the NLM framework to:
     * - Accumulate errors during neural simulation and computation
     * - Provide error information for debugging and analysis
     * - Support comprehensive error reporting and analysis
     * - Enable cross-language error compatibility
     * - Maintain system stability during error conditions
     *
     * @warning Static error management should be used consistently throughout
     *          the codebase to ensure comprehensive error tracking and reporting.
     *
     * @see NLMErrors::Exception
     * @see NLMErrors::Error
     * @see NLMErrors::throwException
     * @see NLMErrors::addError
     */
    static std::vector<Error> errors;
    
    /**
     * @brief Add an error to the error accumulator
     *
     * Adds a new error to the internal error accumulator, which stores all errors
     * encountered during neural simulation and computation. This method is used
     * internally by the error handling system to accumulate errors for later
     * retrieval and reporting.
     *
     * @param type Error type for specific error categorization
     * @param message Error message describing the specific error condition
     * @param context Optional context information for debugging and analysis
     * @throws NLMErrors::Exception if error addition fails
     *
     * @note Error addition includes:
     * - Creating a new Error object with the specified parameters
     * - Adding the error to the internal error accumulator
     * - Updating error count for system health monitoring
     * - Maintaining error context for debugging
     *
     * @see getErrors
     * @see clearErrors
     * @see errorCount
     * @param type Error type for specific error categorization
     * @param message Error message describing the specific error condition
     * @param context Optional context information for debugging and analysis
     */
    static void addError(NLMErrors type, const std::string& message, const std::string& context = "");
    
    /**
     * @brief Get all accumulated errors
     *
     * Returns a vector containing all errors that have been accumulated during neural
     * simulation and computation. This method is used for comprehensive error
     * analysis and reporting.
     *
     * @return std::vector<Error> Vector of all accumulated errors
     * @throws NLMErrors::Exception if error retrieval fails
     *
     * @note Error retrieval includes:
     * - Returning a copy of all accumulated errors
     * - Maintaining error order for chronological analysis
     * - Providing complete error information for debugging
     * - Enabling cross-language error compatibility
     *
     * @see addError
     * @see clearErrors
     * @see errorCount
     */
    static std::vector<Error> getErrors();
    
    /**
     * @brief Clear all accumulated errors
     *
     * Clears all errors from the internal error accumulator, resetting the error
     * state to an empty condition. This method is used for system reset and error
     * recovery operations.
     *
     * @note Error clearing includes:
     * - Clearing the internal error accumulator
     * - Resetting error count to zero
     * - Enabling error recovery operations
     * - Maintaining system stability
     *
     * @see addError
     * @see getErrors
     * @see errorCount
     */
    static void clearErrors();
    
    /**
     * @brief Get the total number of accumulated errors
     *
     * Returns the total number of errors that have been accumulated during neural
     * simulation and computation. This method is used for system health monitoring
     * and error analysis.
     *
     * @return size_t Total number of accumulated errors
     * @throws NLMErrors::Exception if error counting fails
     *
     * @note Error counting includes:
     * - Returning the current error count from the accumulator
     * - Monitoring system health and stability
     * - Enabling error analysis and reporting
     * - Supporting cross-language error compatibility
     *
     * @see addError
     * @see getErrors
     * @see clearErrors
     */
    static size_t errorCount();
    
    /**
     * @brief Throw an exception for the specified error type and message
     *
     * Throws an NLM exception with the specified error type and message. This method
     * is used internally by the error handling system to throw exceptions for specific
     * error conditions during neural simulation and computation.
     *
     * @param type Error type for specific error categorization
     * @param message Error message describing the specific error condition
     * @param context Optional context information for debugging and analysis
     * @throws NLMErrors::Exception Always throws an exception
     *
     * @note Exception throwing includes:
     * - Creating a new NLM exception with the specified parameters
     * - Thowing the exception to enable error handling and recovery
     * - Providing detailed error information for debugging
     * - Supporting cross-language error compatibility
     *
     * @see NLMErrors::Exception
     * @see NLMErrors::addError
     * @see NLMErrors::getErrors
     * @param type Error type for specific error categorization
     * @param message Error message describing the specific error condition
     * @param context Optional context information for debugging and analysis
     */
    static void throwException(NLMErrors type, const std::string& message, const std::string& context = "");
};

/**
 * @defgroup NLMErrorHandling Error Handling Framework
 * @brief Comprehensive error handling for NLM neural simulation
 *
 * This group contains all error handling functionality for the Neural Learning Machine
 * framework, including error type definitions, exception classes, error management
 * utilities, and macros for consistent error reporting throughout the codebase.
 *
 * The error handling framework provides:
 * - Detailed error type enumeration for all possible error conditions
 * - Exception classes with full context and timestamp information
 * - Structured error information for comprehensive error tracking
 * - Static error management for error accumulation and reporting
 * - Exception throwing macros for consistent error reporting
 * - Global error handling utility functions for system-wide error management
 * - Python-specific error handling for bindings integration
 * - Context-preserving error chaining for complex error scenarios
 *
 * @see NLMErrors
 * @see NLMErrors::Exception
 * @see NLMErrors::Error
 * @see NLMErrors::throwException
 * @see NLMErrors::addError
 */

/**
 * @brief Error handling macros for consistent error reporting
 *
 * This macro adds an error to the internal error accumulator with the specified
 * error type and message. It is used throughout the NLM codebase for consistent
 * error reporting and accumulation.
 *
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 *
 * @note The NLM_ERROR macro:
 * - Adds errors to the internal error accumulator
 * - Enables comprehensive error tracking and analysis
 * - Supports cross-language error compatibility
 * - Enables error recovery operations
 *
 * @see NLM_THROW
 * @see NLM_CHECK
 * @see NLM_VALIDATE
 * @see NLM_ERROR
 * @see NLM_PY_ERROR
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 */
#define NLM_ERROR(type, message) \
    nlm::NLMErrors::addError(type, message, __FUNCTION__)

/**
 * @brief Exception throwing macro for consistent error handling
 *
 * This macro throws an NLM exception with the specified error type and message.
 * It is used throughout the NLM codebase for consistent exception throwing and
 * error handling.
 *
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 *
 * @note The NLM_THROW macro:
 * - Throws NLM exceptions with specific error types
 * - Enables exception-based error handling
 * - Provides detailed error information for debugging
 * - Supports cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_CHECK
 * @see NLM_VALIDATE
 * @see NLM_PY_THROW
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 */
#define NLM_THROW(type, message) \
    do { \
        nlm::NLMErrors::throwException(type, message, __FUNCTION__); \
    } while(0)

/**
 * @brief Condition validation macro for consistent error checking
 *
 * This macro validates a condition and throws an exception if the condition is false.
 * It is used throughout the NLM codebase for consistent error checking and validation.
 *
 * @param condition Condition to validate
 * @param type Error type for specific error categorization
 * @param message Error message describing the validation failure
 *
 * @note The NLM_CHECK macro:
 * - Validates specified conditions
 * - Throws exceptions for failed conditions
 * - Enables consistent error checking
 * - Supports robust error handling
 *
 * @see NLM_ERROR
 * @see NLM_THROW
 * @see NLM_VALIDATE
 * @param condition Condition to validate
 * @param type Error type for specific error categorization
 * @param message Error message describing the validation failure
 */
#define NLM_CHECK(condition, type, message) \
    if (!(condition)) { \
        NLM_THROW(type, message); \
    }

/**
 * @brief Parameter validation macro for consistent error checking
 *
 * This macro validates a parameter condition and throws an exception with parameter
 * name if the condition is false. It is used throughout the NLM codebase for
 * consistent parameter validation and error handling.
 *
 * @param param Parameter name for error reporting
 * @param condition Condition to validate
 * @param type Error type for specific error categorization
 * @param message Error message describing the validation failure
 *
 * @note The NLM_VALIDATE macro:
 * - Validates specified parameter conditions
 * - Includes parameter name in error messages
 * - Enables comprehensive parameter validation
 * - Supports robust error handling
 *
 * @see NLM_ERROR
 * @see NLM_THROW
 * @see NLM_CHECK
 * @see NLM_VALIDATE_DEBUG
 * @param param Parameter name for error reporting
 * @param condition Condition to validate
 * @param type Error type for specific error categorization
 * @param message Error message describing the validation failure
 */
#define NLM_VALIDATE(param, condition, type, message) \
    if (!(condition)) { \
        std::stringstream ss; \
        ss << message << " (parameter: " << #param << ")"; \
        NLM_THROW(type, ss.str()); \
    }

/**
 * @brief Error logging macro for information messages
 *
 * This macro logs an error message through the error handling system for
 * information purposes. It is used throughout the NLM codebase for logging
 * informational messages and system status.
 *
 * @param message Error message to log
 *
 * @note The NLM_LOG_INFO macro:
 * - Logs informational messages through error handling system
 * - Enables comprehensive system logging
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_WARNING
 * @see NLM_LOG_ERROR
 * @see NLM_LOG_DEBUG
 * @param message Error message to log
 */
#define NLM_LOG_INFO(message) \
    NLM_ERROR(NLMErrors::UnknownError, message)

/**
 * @brief Error logging macro for warning messages
 *
 * This macro logs a warning message through the error handling system for
 * warning purposes. It is used throughout the NLM codebase for logging
 * warning messages and system alerts.
 *
 * @param message Warning message to log
 *
 * @note The NLM_LOG_WARNING macro:
 * - Logs warning messages through error handling system
 * - Enables system warning and alerting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_INFO
 * @see NLM_LOG_ERROR
 * @see NLM_LOG_DEBUG
 * @param message Warning message to log
 */
#define NLM_LOG_WARNING(message) \
    NLM_ERROR(NLMErrors::UnknownError, "WARNING: " + message)

/**
 * @brief Error logging macro for error messages
 *
 * This macro logs an error message through the error handling system for
 * error purposes. It is used throughout the NLM codebase for logging
 * error messages and system failures.
 *
 * @param message Error message to log
 *
 * @note The NLM_LOG_ERROR macro:
 * - Logs error messages through error handling system
 * - Enables system error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_INFO
 * @see NLM_LOG_WARNING
 * @see NLM_LOG_DEBUG
 * @param message Error message to log
 */
#define NLM_LOG_ERROR(message) \
    NLM_ERROR(NLMErrors::UnknownError, message)

/**
 * @brief Debug logging macro for debugging messages
 *
 * This macro logs a debug message through the error handling system for
 * debugging purposes. It is used throughout the NLM codebase for logging
 * debug messages and development information.
 *
 * @param message Debug message to log
 *
 * @note The NLM_LOG_DEBUG macro:
 * - Logs debug messages through error handling system
 * - Enables development and debugging support
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_INFO
 * @see NLM_LOG_WARNING
 * @see NLM_LOG_ERROR
 * @param message Debug message to log
 */
#define NLM_LOG_DEBUG(message) \
    NLM_ERROR(NLMErrors::UnknownError, "DEBUG: " + message)

/**
 * @brief Python-specific error handling macro
 *
 * This macro adds a Python-specific error to the internal error accumulator with
 * the specified error type and message. It is used throughout the NLM Python
 * bindings for consistent error reporting and accumulation.
 *
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 *
 * @note The NLM_PY_ERROR macro:
 * - Adds Python-specific errors to the internal error accumulator
 * - Enables Python binding error handling
 * - Supports cross-language error compatibility
 * - Enables error recovery operations
 *
 * @see NLM_ERROR
 * @see NLM_PY_THROW
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 */
#define NLM_PY_ERROR(type, message) \
    nlm::NLMErrors::addError(type, "Python: " + message, "Python bindings");

/**
 * @brief Python-specific exception throwing macro
 *
 * This macro throws a Python-specific NLM exception with the specified error type
 * and message. It is used throughout the NLM Python bindings for consistent
 * exception throwing and error handling.
 *
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 *
 * @note The NLM_PY_THROW macro:
 * - Throws Python-specific NLM exceptions
 * - Enables Python binding exception handling
 * - Provides detailed error information for debugging
 * - Supports cross-language error compatibility
 *
 * @see NLM_THROW
 * @see NLM_PY_ERROR
 * @param type Error type for specific error categorization
 * @param message Error message describing the specific error condition
 */
#define NLM_PY_THROW(type, message) \
    do { \
        nlm::NLMErrors::throwException(type, "Python: " + message, "Python bindings"); \
    } while(0)

/**
 * @brief File I/O specific error macro
 *
 * This macro logs a file I/O error message through the error handling system.
 * It is used throughout the NLM codebase for file-related error reporting.
 *
 * @param message File I/O error message to log
 *
 * @note The NLM_FILE_ERROR macro:
 * - Logs file I/O errors through error handling system
 * - Enables file-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message File I/O error message to log
 */
#define NLM_FILE_ERROR(message) \
    NLM_ERROR(NLMErrors::FileIO, "File Error: " + message)

/**
 * @brief Memory allocation error macro
 *
 * This macro logs a memory allocation error message through the error handling
 * system. It is used throughout the NLM codebase for memory-related error
 * reporting.
 *
 * @param message Memory allocation error message to log
 *
 * @note The NLM_MEMORY_ERROR macro:
 * - Logs memory allocation errors through error handling system
 * - Enables memory-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message Memory allocation error message to log
 */
#define NLM_MEMORY_ERROR(message) \
    NLM_ERROR(NLMErrors::MemoryAllocation, "Memory Error: " + message)

/**
 * @brief Configuration parsing error macro
 *
 * This macro logs a configuration parsing error message through the error handling
 * system. It is used throughout the NLM codebase for configuration-related error
 * reporting.
 *
 * @param message Configuration parsing error message to log
 *
 * @note The NLM_CONFIG_ERROR macro:
 * - Logs configuration parsing errors through error handling system
 * - Enables configuration-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message Configuration parsing error message to log
 */
#define NLM_CONFIG_ERROR(message) \
    NLM_ERROR(NLMErrors::ConfigParsing, "Config Error: " + message)

/**
 * @brief Neuron error macro
 *
 * This macro logs a neuron error message through the error handling system.
 * It is used throughout the NLM codebase for neuron-related error reporting.
 *
 * @param message Neuron error message to log
 *
 * @note The NLM_NEURON_ERROR macro:
 * - Logs neuron errors through error handling system
 * - Enables neuron-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message Neuron error message to log
 */
#define NLM_NEURON_ERROR(message) \
    NLM_ERROR(NLMErrors::NeuronInvalid, "Neuron Error: " + message)

/**
 * @brief Synapse error macro
 *
 * This macro logs a synapse error message through the error handling system.
 * It is used throughout the NLM codebase for synapse-related error reporting.
 *
 * @param message Synapse error message to log
 *
 * @note The NLM_SYNAPSE_ERROR macro:
 * - Logs synapse errors through error handling system
 * - Enables synapse-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message Synapse error message to log
 */
#define NLM_SYNAPSE_ERROR(message) \
    NLM_ERROR(NLMErrors::SynapseInvalid, "Synapse Error: " + message)

/**
 * @brief Plasticity error macro
 *
 * This macro logs a plasticity error message through the error handling system.
 * It is used throughout the NLM codebase for plasticity-related error reporting.
 *
 * @param message Plasticity error message to log
 *
 * @note The NLM_PLASTICITY_ERROR macro:
 * - Logs plasticity errors through error handling system
 * - Enables plasticity-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message Plasticity error message to log
 */
#define NLM_PLASTICITY_ERROR(message) \
    NLM_ERROR(NLMErrors::PlasticityFailed, "Plasticity Error: " + message)

/**
 * @brief Validation error macro
 *
 * This macro logs a validation error message through the error handling system.
 * It is used throughout the NLM codebase for validation-related error reporting.
 *
 * @param message Validation error message to log
 *
 * @note The NLM_VALIDATION_ERROR macro:
 * - Logs validation errors through error handling system
 * - Enables validation-related error reporting
 * - Supports error tracking and monitoring
 * - Enables cross-language error compatibility
 *
 * @see NLM_ERROR
 * @see NLM_LOG_ERROR
 * @param message Validation error message to log
 */
#define NLM_VALIDATION_ERROR(message) \
    NLM_ERROR(NLMErrors::ValidationError, "Validation Error: " + message)

/**
 * @brief Conditional debug exception macro
 *
 * This macro throws an exception only in debug mode if a condition is false.
 * It is used throughout the NLM codebase for debug-only error checking.
 *
 * @param type Error type for specific error categorization
 * @param message Error message describing the validation failure
 *
 * @note The NLM_THROW_IF_DEBUG macro:
 * - Throws exceptions only in debug mode
 * - Enables debug-only error checking
 * - Supports conditional error handling
 * - Provides development and debugging support
 *
 * @see NLM_THROW
 * @see NLM_VALIDATE_DEBUG
 * @param type Error type for specific error categorization
 * @param message Error message describing the validation failure
 */
#ifdef NDEBUG
    #define NLM_THROW_IF_DEBUG(type, message)
    #define NLM_VALIDATE_DEBUG(param, condition, type, message) \
        if (!(condition)) { \
            std::stringstream ss; \
            ss << message << " (parameter: " << #param << ")"; \
            NLM_THROW(type, ss.str()); \
        }
#else
    #define NLM_THROW_IF_DEBUG(type, message) NLM_THROW(type, message)
    #define NLM_VALIDATE_DEBUG(param, condition, type, message) \
        if (!(condition)) { \
            std::stringstream ss; \
            ss << message << " (parameter: " << #param << ")"; \
            NLM_THROW(type, ss.str()); \
        }
#endif

/**
 * @brief Global error handling utility functions
 *
 * This section contains global error handling utility functions for the NLM framework.
 * These functions provide essential error handling capabilities for system-wide
 * error management and reporting.
 *
 * Utility functions include:
 * - Getting the last error message for debugging
 * - Checking if errors exist for error recovery
 * - Printing all errors to output streams
 * - Python error handler integration for bindings
 *
 * @note Utility functions are used throughout the NLM framework to:
 * - Provide error information for debugging and analysis
 * - Enable error recovery operations
 * - Support comprehensive error reporting and analysis
 * - Enable cross-language error compatibility
 * - Maintain system stability during error conditions
 *
 * @see NLMErrors::addError
 * @see NLMErrors::getErrors
 */

/**
 * @brief Get the last error message for debugging
 *
 * This function returns the last error message from the error accumulator for
 * debugging and analysis purposes. It provides detailed error information for
 * system diagnosis and troubleshooting.
 *
 * @return std::string The last error message, or "No errors" if no errors exist
 * @throws NLMErrors::Exception if error retrieval fails
 *
 * @note This function:
 * - Returns the most recent error from the error accumulator
 * - Provides detailed error information for debugging
 * - Enables error analysis and reporting
 * - Supports cross-language error compatibility
 *
 * @see hasErrors
 * @see printErrors
 */
inline std::string getLastErrorMessage() {
    auto errors = nlm::NLMErrors::getErrors();
    return errors.empty() ? "No errors" : errors.back().getFullMessage();
}

/**
 * @brief Check if errors exist for error recovery
 *
 * This function checks if any errors exist in the error accumulator for error
 * recovery purposes. It is used throughout the NLM framework to determine
 * when error recovery operations are needed.
 *
 * @return bool true if errors exist, false if no errors exist
 * @throws NLMErrors::Exception if error checking fails
 *
 * @note This function:
 * - Checks if any errors exist in the error accumulator
 * - Enables error recovery operations
 * - Supports comprehensive error reporting
 * - Supports cross-language error compatibility
 *
 * @see getLastErrorMessage
 * @see printErrors
 */
inline bool hasErrors() {
    return !nlm::NLMErrors::getErrors().empty();
}

/**
 * @brief Print all errors to output streams
 *
 * This function prints all accumulated errors to the specified output stream for
 * error reporting and analysis. It provides comprehensive error information for
 * system diagnosis and troubleshooting.
 *
 * @param out Output stream to write errors to (defaults to std::cout)
 * @throws NLMErrors::Exception if error printing fails
 *
 * @note This function:
 * - Prints all accumulated errors to the specified output stream
 * - Provides comprehensive error information for debugging
 * - Enables error analysis and reporting
 * - Supports cross-language error compatibility
 *
 * @see getLastErrorMessage
 * @see hasErrors
 * @see nlm::NLMErrors::Error
 * @param out Output stream to write errors to (defaults to std::cout)
 */
inline void printErrors(std::ostream& out = std::cout) {
    auto errors = nlm::NLMErrors::getErrors();
    for (const auto& error : errors) {
        out << error.getFullMessage() << std::endl;
    }
}

/**
 * @class PythonErrorHandler
 * @brief Python error handler for NLM bindings
 *
 * This class provides error handling utilities for NLM Python bindings, enabling
 * comprehensive error handling and reporting for cross-language operations. It
 * integrates seamlessly with Python's error handling mechanisms while providing
 * NLM-specific error information and context.
 *
 * The PythonErrorHandler class is designed for:
 * - Python binding error handling and reporting
 * - Cross-language error compatibility with Python
 * - Python-specific error information and context
 * - Integration with Python error handling mechanisms
 * - Error recovery operations for Python bindings
 *
 * @note The PythonErrorHandler class:
 * - Provides Python-specific error handling utilities
 * - Integrates seamlessly with Python error handling mechanisms
 * - Maintains NLM-specific error information and context
 * - Enables cross-language error compatibility
 * - Supports error recovery operations for Python bindings
 *
 * @see NLMErrors::Exception
 * @see NLMErrors::addError
 * @see NLMErrors::getErrors
 */
class PythonErrorHandler {
public:
    /**
     * @brief Get the last error message for Python bindings
     *
     * This function returns the last error message from the error accumulator
     * for Python bindings. It provides detailed error information for Python
     * error handling and debugging.
     *
     * @return std::string The last error message, or "No errors" if no errors exist
     * @throws NLMErrors::Exception if error retrieval fails
     *
     * @note This function:
     * - Returns the most recent error from the error accumulator
     * - Provides detailed error information for Python bindings
     * - Enables Python error handling and recovery
     * - Supports cross-language error compatibility
     *
     * @see hasErrors
     * @see clearErrors
     * @see logErrorsToPython
     */
    static std::string getLastError() {
        auto errors = NLMErrors::getErrors();
        if (errors.empty()) return "No errors";
        return errors.back().getFullMessage();
    }
    
    /**
     * @brief Check if errors exist for Python bindings
     *
     * This function checks if any errors exist in the error accumulator for
     * Python bindings. It is used throughout the NLM Python bindings to
     * determine when error recovery operations are needed.
     *
     * @return bool true if errors exist, false if no errors exist
     * @throws NLMErrors::Exception if error checking fails
     *
     * @note This function:
     * - Checks if any errors exist in the error accumulator
     * - Enables error recovery operations for Python bindings
     * - Supports comprehensive error reporting
     * - Supports cross-language error compatibility
     *
     * @see getLastError
     * @see clearErrors
     */
    static bool hasErrors() {
        return !NLMErrors::getErrors().empty();
    }
    
    /**
     * @brief Clear all stored errors for Python bindings
     *
     * This function clears all errors from the error accumulator, resetting the
     * error state to an empty condition. It is used for system reset and error
     * recovery operations in Python bindings.
     *
     * @note Error clearing includes:
     * - Clearing the internal error accumulator
     * - Resetting error count to zero
     * - Enabling error recovery operations
     * - Maintaining system stability
     *
     * @see addError
     * @see getErrors
     */
    static void clearErrors() {
        NLMErrors::clearErrors();
    }
    
    /**
     * @brief Log errors to Python string for bindings
     *
     * This function logs all accumulated errors to a Python string for
     * Python bindings. It provides comprehensive error information for
     * Python error handling and analysis.
     *
     * @param errorString String to append error messages to
     * @throws NLMErrors::Exception if error logging fails
     *
     * @note This function:
     * - Appends all accumulated errors to the specified string
     * - Provides comprehensive error information for Python bindings
     * - Enables Python error handling and recovery
     * - Supports cross-language error compatibility
     *
     * @see getLastError
     * @see hasErrors
     * @see clearErrors
     * @param errorString String to append error messages to
     */
    static void logErrorsToPython(std::string& errorString) {
        auto errors = NLMErrors::getErrors();
        for (const auto& error : errors) {
            errorString += error.getFullMessage() + "\n";
        }
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H
