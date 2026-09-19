# NLM Error Handling Framework

## Overview

This document provides comprehensive information about the NLM (Neural Network Library) error handling framework. It covers error types, codes, recovery mechanisms, and best practices.

## Error Hierarchy

### Base Error Classes

#### NLMError

**Purpose**: Base class for all NLM-specific errors.

**Inheritance**: `std::runtime_error`

**Key Features**:
- Detailed error information including code, severity, and context
- Support for exception chaining with causes
- Automatic source location tracking (file, line, function)
- Error context management (function name, parameters, state)
- Recovery suggestions for common error types
- Serialization support for logging

**Constructor**:
```cpp
NLMError(const std::string& message, 
         ErrorCode code = ErrorCode::ERROR_UNKNOWN,
         ErrorSeverity severity = ErrorSeverity::Error,
         const SourceLocation& location = SourceLocation(__FILE__, __LINE__, __FUNCTION__))
```

**Key Methods**:
- `getCode()`: Returns error code
- `getSeverity()`: Returns error severity
- `getLocation()`: Returns source location
- `getContext()`: Returns error context
- `getCause()`: Returns chained exception cause
- `getUserMessage()`: Returns user-friendly message
- `getDeveloperMessage()`: Returns detailed debugging message
- `getRecoverySuggestion()`: Returns recovery guidance
- `serialize()`: Serializes error for logging

#### Specific Error Types

**ConfigError**: For configuration-related errors
- Code: `ERROR_INVALID_CONFIG` (11)
- Recovery: Check configuration file format and syntax

**BrainError**: For brain/network initialization errors
- Code: `ERROR_BRAIN_CREATION_FAILED` (42)
- Recovery: Verify brain parameters and network architecture

**NeuronError**: For neuron activation issues
- Code: `ERROR_NEUPRON_ACTIVATION_FAILED` (44)
- Recovery: Check neuron parameters and initialization

**SynapseError**: For synaptic connection problems
- Code: `ERROR_SYNAPSE_CREATION_FAILED` (43)
- Recovery: Verify synaptic connection parameters

**MemoryError**: For memory-related errors
- Code: `ERROR_OUT_OF_MEMORY` (20)
- Recovery: Reduce memory usage or increase available memory

**NetworkError**: For neural network architecture errors
- Code: `ERROR_INVALID_NETWORK` (40)
- Recovery: Verify network architecture parameters

## Error Codes

### Success
- `ERROR_OK` (0): Operation completed successfully

### Null Pointer Errors
- `ERROR_NULL_POINTER` (1): General null pointer
- `ERROR_NULL_BRAIN` (2): Null brain pointer
- `ERROR_NULL_NEURON` (3): Null neuron pointer
- `ERROR_NULL_SYNAPSE` (4): Null synapse pointer
- `ERROR_NULL_MEMORY` (5): Null memory pointer

### Validation Errors
- `ERROR_INVALID_PARAMETER` (10): Invalid function parameter
- `ERROR_INVALID_CONFIG` (11): Invalid configuration
- `ERROR_INVALID_STATE` (12): Invalid system state
- `ERROR_INVALID_OPERATION` (13): Invalid operation
- `ERROR_OUT_OF_BOUNDS` (14): Array/vector index out of bounds
- `ERROR_NEGATIVE_VALUE` (15): Negative value encountered

### Resource Errors
- `ERROR_OUT_OF_MEMORY` (20): Insufficient memory
- `ERROR_OUT_OF_CPU` (21): Insufficient CPU resources
- `ERROR_OUT_OF_GPU` (22): Insufficient GPU resources
- `ERROR_FILE_NOT_FOUND` (23): File not found
- `ERROR_FILE_PERMISSION` (24): Permission denied

### Runtime Errors
- `ERROR_DIVISION_BY_ZERO` (30): Division by zero
- `ERROR_OVERFLOW` (31): Integer/numeric overflow
- `ERROR_UNDERFLOW` (32): Integer/numeric underflow
- `ERROR_CONVERGENCE_FAILED` (33): Learning algorithm didn't converge
- `ERROR_TIMED_OUT` (34): Operation timed out
- `ERROR_INTERRUPTED` (35): Operation interrupted

### Neural Network Errors
- `ERROR_INVALID_NETWORK` (40): Invalid network architecture
- `ERROR_WEIGHT_MISMATCH` (41): Weight matrix dimension mismatch
- `ERROR_BRAIN_CREATION_FAILED` (42): Brain creation failed
- `ERROR_SYNAPSE_CREATION_FAILED` (43): Synapse creation failed
- `ERROR_NEUPRON_ACTIVATION_FAILED` (44): Neuron activation failed

### System Errors
- `ERROR_SYSTEM_CALL_FAILED` (50): System call failed
- `ERROR_OS_NOT_SUPPORTED` (51): Operating system not supported
- `ERROR_DRIVER_NOT_FOUND` (52): Required driver not found
- `ERROR_HARDWARE_NOT_SUPPORTED` (53): Hardware not supported

### General Errors
- `ERROR_UNKNOWN` (99): Unknown error
- `ERROR_NOT_IMPLEMENTED` (100): Feature not implemented
- `ERROR_DEPRECATED` (101): Deprecated function/method
- `ERROR_UNSUPPORTED` (102): Unsupported operation

## Error Severity Levels

### Severity Levels
- `Info` (0): Informational message
- `Warning` (1): Warning that doesn't prevent operation
- `Error` (2): Error that prevents operation but system can continue
- `Critical` (3): Critical error that may cause system instability
- `Fatal` (4): Fatal error requiring system shutdown

## Error Context

Error context provides detailed debugging information including:

### Source Location
- File name
- Line number
- Function name

### Runtime Context
- Function name
- Input parameters
- System state at time of error

### Timing Information
- Timestamp when error occurred

## Recovery Suggestions

The framework provides automatic recovery suggestions for common error types:

### Null Pointer
- Check if required object was properly initialized before use

### Invalid Parameter
- Validate input parameters before calling function

### Out of Bounds
- Ensure array/vector indices are within valid range

### Out of Memory
- Reduce memory usage or increase available memory

### Division by Zero
- Check denominator before performing division

### Convergence Failed
- Check learning rate or network architecture
- Try increasing iterations

### Invalid Config
- Verify configuration file syntax and values

### Timeout
- Increase timeout value or optimize performance

## Error Handling Macros

### Basic Error Throwing
```cpp
NLM_THROW(ConfigError, "Invalid configuration format");
```

### Error Throwing with Code
```cpp
NLM_THROW_WITH_CODE(NeuronError, "Neuron threshold too high", ErrorCode::ERROR_INVALID_PARAMETER);
```

### Error Throwing with Context
```cpp
ErrorContext ctx = ErrorUtils::createContext("trainNeuralNetwork");
ctx.addParameter("learning_rate", std::to_string(lr));
NLM_THROW_WITH_CONTEXT(MemoryError, "Memory allocation failed", ctx);
```

### Error Checking
```cpp
NLM_CHECK(pointer != nullptr, NullPointerError, "Neuron pointer is null");
```

### Error Checking with Code
```cpp
NLM_CHECK_WITH_CODE(index >= 0 && index < size, OutOfBoundsError, 
                   "Array index out of bounds", ErrorCode::ERROR_OUT_OF_BOUNDS);
```

## Exception Chaining

The framework supports exception chaining to preserve the original cause:

```cpp
try {
    // Some operation that might throw
    someOperation();
} catch (const std::exception& e) {
    throwWithCause(NLMError("Wrapper message"), e);
}
```

## Error Handling Integration

### Backward Compatibility

The framework includes backward compatibility features:

1. **Legacy Error Code Conversion**: Convert old integer error codes to NLMError objects
2. **Exception Wrapping**: Wrap existing std::exceptions as NLMError with proper context
3. **Gradual Migration**: Existing code can be incrementally updated

### Error Recovery

The framework supports graceful error recovery:

1. **State Rollback**: Restore system state on recoverable errors
2. **Fallback Values**: Use sensible defaults for critical parameters
3. **Degradation**: Continue operation with reduced functionality
4. **Error Tolerance**: Configurable error tolerance levels

## Usage Examples

### Example 1: Basic Error Handling
```cpp
void initializeBrain(const BrainConfig& config) {
    try {
        // Validate configuration
        if (config.learningRate <= 0.0f) {
            NLM_THROW(ConfigError, "Learning rate must be positive");
        }
        
        if (config.hiddenLayers.empty()) {
            NLM_THROW(NetworkError, "Network must have at least one hidden layer");
        }
        
        // Create brain
        brain = std::make_unique<Brain>(config);
        
    } catch (const NLMError& e) {
        // Log error
        Logger::getInstance().error(ErrorUtils::formatForUser(e));
        
        // Re-throw with additional context
        NLMError enhancedError("Brain initialization failed: " + std::string(e.what()),
                            e.getLocation());
        enhancedError.setCause(e);
        throw enhancedError;
    }
}
```

### Example 2: Error Recovery
```cpp
float trainNetwork(const NeuralData& data, float maxLearningRate = 0.1f) {
    float bestError = std::numeric_limits<float>::max();
    
    for (float lr = maxLearningRate; lr > 0.0001f; lr *= 0.5f) {
        try {
            float error = trainWithLearningRate(data, lr);
            
            if (error < bestError) {
                bestError = error;
                trainedNetwork = currentNetwork;
                Logger::getInstance().info("Successfully trained with learning rate: " + 
                                         std::to_string(lr));
            }
            
            return bestError;
            
        } catch (const NLMError& e) {
            if (ErrorHandler::isRecoverable(e)) {
                Logger::getInstance().warning("Training failed with learning rate " + 
                                           std::to_string(lr) + ": " + e.getUserMessage());
                continue; // Try with smaller learning rate
            } else {
                throw; // Re-throw non-recoverable errors
            }
        }
    }
    
    NLM_THROW(BrainError, "Training failed after trying all learning rates");
}
```

### Example 3: Comprehensive Error Handling
```cpp
class ErrorHandlingExample {
public:
    static void processData(const std::vector<float>& data) {
        ErrorContext ctx = ErrorUtils::createContext("processData");
        ctx.addParameter("data_size", std::to_string(data.size()));
        
        try {
            // Validate input
            NLM_CHECK_WITH_CODE(!data.empty(), ConfigError, 
                               "Input data cannot be empty", 
                               ErrorCode::ERROR_INVALID_PARAMETER);
            
            // Check data range
            for (size_t i = 0; i < data.size(); ++i) {
                NLM_CHECK_WITH_CODE(data[i] >= -1.0f && data[i] <= 1.0f,
                                   OutOfBoundsError,
                                   "Data value out of valid range [-1, 1]",
                                   ErrorCode::ERROR_OUT_OF_BOUNDS);
            }
            
            // Process data
            processInternal(data);
            
        } catch (const NLMError& e) {
            // Add more context
            e.addState("data_size", std::to_string(data.size()));
            
            // Handle based on error type
            if (ErrorHandler::isRecoverable(e)) {
                // Attempt recovery
                if (!attemptRecovery(e)) {
                    throw; // Recovery failed, re-throw
                }
            }
            
            // Log and re-throw
            Logger::getInstance().error(ErrorUtils::formatForUser(e));
            throw;
        }
    }
    
private:
    static bool attemptRecovery(const NLMError& error) {
        switch (error.getCode()) {
            case ErrorCode::ERROR_OUT_OF_BOUNDS:
                // Clamp values to valid range
                return clampDataValues();
            case ErrorCode::ERROR_INVALID_PARAMETER:
                // Use default values
                applyDefaults();
                return true;
            default:
                return false;
        }
    }
};
```

## Performance Considerations

### Error Rate Monitoring

The framework includes utilities for monitoring error rates:

```cpp
class ErrorRateMonitor {
public:
    void recordError(const NLMError& error) {
        totalErrors_++;
        if (ErrorHandler::isRecoverable(error)) {
            recoverableErrors_++;
        }
        
        // Log periodically
        if (totalErrors_ % 100 == 0) {
            logErrorRate();
        }
    }
    
    float getErrorRate() const {
        return static_cast<float>(totalErrors_) / static_cast<float>(totalOperations_);
    }
    
    float getRecoverableRate() const {
        return static_cast<float>(recoverableErrors_) / static_cast<float>(totalErrors_);
    }
    
private:
    void logErrorRate() {
        Logger::getInstance().info("Error rate: " + std::to_string(getErrorRate()) +
                                 ", Recoverable: " + std::to_string(getRecoverableRate()));
    }
    
    size_t totalErrors_ = 0;
    size_t recoverableErrors_ = 0;
    size_t totalOperations_ = 0;
};
```

### Performance Impact

- Error handling is designed to be lightweight when no errors occur
- Only minimal overhead for successful operations
- Detailed context and recovery information only captured when errors occur
- Configurable logging levels to control output volume

## Migration Guide

### From Error Codes to Exceptions

1. **Replace boolean returns** with exception throwing
   ```cpp
   // Old
   bool loadFile(const std::string& path) {
       if (!fileExists(path)) return false;
       // ... load file
       return true;
   }
   
   // New
   void loadFile(const std::string& path) {
       NLM_CHECK_WITH_CODE(fileExists(path), FileError, 
                          "File not found: " + path,
                          ErrorCode::ERROR_FILE_NOT_FOUND);
       // ... load file
   }
   ```

2. **Replace error codes** with exception throwing
   ```cpp
   // Old
   int loadNeuralNetwork(const std::string& filename) {
       if (!validateFile(filename)) return -1;
       // ... load network
       return 0;
   }
   
   // New
   void loadNeuralNetwork(const std::string& filename) {
       NLM_CHECK_WITH_CODE(validateFile(filename), NetworkError,
                          "Invalid network file: " + filename,
                          ErrorCode::ERROR_INVALID_NETWORK);
       // ... load network
   }
   ```

3. **Add error context** to improve debugging
   ```cpp
   try {
       processData(data);
   } catch (const NLMError& e) {
       ErrorContext ctx = e.getContext();
       ctx.addParameter("data_size", std::to_string(data.size()));
       ctx.addState("processing_stage", "feature_extraction");
       
       // Log with enhanced context
       Logger::getInstance().error(ErrorUtils::formatForLogging(e));
       
       throw; // Re-throw with context
   }
   ```

### Testing Error Handling

When testing error scenarios:

1. **Test error cases** with try-catch blocks
2. **Verify error messages** contain expected information
3. **Check error codes** match expected values
4. **Validate recovery suggestions** are appropriate
5. **Ensure exception chaining** preserves original causes

### Integration Testing

1. **Test error boundaries** (min/max values)
2. **Test resource limits** (memory, CPU, GPU)
3. **Test concurrent error scenarios**
4. **Test error recovery mechanisms**
5. **Test graceful degradation**

## Conclusion

The NLM error handling framework provides:

- Comprehensive error hierarchy for all error types
- Detailed error information with context and recovery suggestions
- Support for exception chaining and backward compatibility
- Graceful error recovery and state management
- Performance monitoring and optimization
- Complete documentation and examples

This framework enables robust error handling, detailed debugging, and graceful recovery in production environments.
