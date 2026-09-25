// Enhanced Error Categories for NLM (Neural Learning Machine)
// Comprehensive error classification system

#pragma once

namespace nlm {

// Enhanced error categories as specified in requirements
enum class ErrorCategory {
    // Configuration errors
    CONFIG_INVALID_VALUE = 1000,      // Invalid values in configuration
    CONFIG_MISSING_FILE = 1001,        // Missing configuration files
    CONFIG_JSON_PARSING = 1002,        // JSON parsing errors
    CONFIG_VALIDATION_FAILED = 1003,   // Configuration validation failures
    
    // Neural system errors
    NEURON_NOT_FOUND = 2000,           // Neuron not found
    INVALID_SYNAPSES = 2001,           // Invalid synapses
    SIMULATION_ERROR = 2002,           // Simulation errors
    BRAIN_INITIALIZATION_FAILED = 2003,// Brain initialization failures
    
    // IO errors
    IO_READ_ERROR = 3000,              // File read errors
    IO_WRITE_ERROR = 3001,             // File write errors
    IO_NETWORK_ERROR = 3002,           // Network errors
    FILE_NOT_FOUND = 3003,             // File not found
    
    // Resource errors
    RESOURCE_OUT_OF_MEMORY = 4000,     // Out of memory
    RESOURCE_INVALID_PARAMS = 4001,    // Invalid parameters
    RESOURCE_TIMEOUT = 4002,           // Timeouts
    RESOURCE_QUOTA_EXCEEDED = 4003,    // Resource quotas exceeded
    
    // System errors
    SYSTEM_INIT_FAILURE = 5000,        // Initialization failures
    SYSTEM_NOT_IMPLEMENTED = 5001,     // Not implemented features
    SYSTEM_INTERNAL_ERROR = 5002,      // Internal system errors
    SYSTEM_UNAVAILABLE = 5003,         // System unavailable
    
    // Additional categories
    VALIDATION_FAILED = 6000,          // Parameter validation errors
    MEMORY_ALLOCATION_FAILED = 6001,    // Memory allocation errors
    THREADING_ERROR = 6002,             // Threading and concurrency errors
    RANDOM_SEED_ERROR = 6003,           // Random number generation errors
    UNKNOWN = 9999                     // Unknown error type
};

// Error severity levels
enum class ErrorSeverity {
    INFO = 0,           // Information only
    WARNING = 1,        // Warning conditions
    ERROR = 2,          // Error conditions
    CRITICAL = 3,       // Critical errors
    FATAL = 4           // Fatal errors, system may not recover
};

// Error handling strategies
enum class ErrorStrategy {
    THROW,             // Re-throw the error for caller to handle
    LOG_AND_THROW,     // Log the error then re-throw
    RECOVER,           // Attempt automatic recovery
    RETRY,             // Retry the operation
    FALLBACK,          // Use fallback values/strategies
    IGNORE,            // Ignore the error (use defaults)
    SILENT             // Only log internally, don't propagate
};

} // namespace nlm
