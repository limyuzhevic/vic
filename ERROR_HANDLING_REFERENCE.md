# NLM - Error Handling Reference

## Overview

NLM implements a comprehensive error handling and validation system to ensure robust operation, graceful degradation, and detailed error reporting. This reference provides detailed information about error types, handling mechanisms, and best practices.

## Error Handling Architecture

### Core Components

1. **Error Handling System**
   - Global error management
   - Exception safety guarantees
   - Graceful degradation

2. **Validation System**
   - Parameter validation
   - State validation
   - Resource validation

3. **Error Reporting**
   - Detailed error messages
   - Error statistics
   - Error categorization

4. **Logging System**
   - Multiple log levels
   - Structured logging
   - Performance logging

### Error Classification

#### Error Categories

| Category | Description | Examples |
|----------|-------------|----------|
| Configuration | Configuration-related errors | Invalid parameters, file not found |
| Validation | Validation rule violations | Out-of-range values, invalid states |
| NeuralComputation | Brain computation errors | Neuron initialization failures, numerical instability |
| Memory | Memory system errors | Allocation failures, corruption detection |
| I/O | Input/output operations | File read/write errors, network issues |
| Resource | Resource management errors | Memory limits, CPU limits, timing issues |
| System | System-level errors | Memory corruption, assertion failures |

#### Error Codes

| Error Code | Category | Description |
|------------|----------|-------------|
| `ConfigValidationFailed` | Configuration | Configuration validation failed |
| `ParameterInvalid` | Validation | Invalid parameter value |
| `StateInvalid` | Validation | Invalid system state |
| `NeuronInvalidState` | NeuralComputation | Neuron in invalid state |
| `MemoryAllocationFailed` | Memory | Memory allocation failed |
| `MemoryCorruption` | Memory | Memory corruption detected |
| `FileNotFound` | I/O | Required file not found |
| `FilePermissionDenied` | I/O | Permission denied for file operation |
| `ResourceLimitExceeded` | Resource | Resource limit exceeded |
| `Timeout` | Resource | Operation timed out |
| `SystemError` | System | System-level error occurred |

## Error Handling Mechanisms

### 1. Exception-Based Handling

#### Standard Exceptions

```cpp
#include "core/ErrorHandling/ErrorHandlingSystem.hpp"

void performOperation() {
    try {
        // Code that may throw exceptions
        nlm::Brain brain(config);
        brain.initialize();
        
        // Simulate error
        if (someErrorCondition) {
            NLM_THROW_CONFIG("Invalid configuration parameter");
        }
        
        // Perform operation
        brain.step(0);
        
    } catch (const nlm::ValidationException& e) {
        // Handle validation errors
        std::cerr << "Validation error: " << e.what() << std::endl;
        reportError(e.what(), nlm::ErrorCode::ConfigValidationFailed);
    } catch (const nlm::NeuralException& e) {
        // Handle neural computation errors
        std::cerr << "Neural error: " << e.what() << std::endl;
        reportError(e.what(), nlm::ErrorCode::NeuronInvalidState);
    } catch (const std::exception& e) {
        // Handle other exceptions
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        reportException(e);
    }
}
```

#### Python Exception Handling

```python
import pynlm

def perform_operation():
    try:
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Simulate error
        if some_error_condition:
            raise ValueError("Invalid configuration")
        
        # Perform operation
        brain.step(0)
        
    except pynlm.ValidationError as e:
        # Handle validation errors
        print(f"Validation error: {e}")
    except pynlm.NeuralError as e:
        # Handle neural errors
        print(f"Neural error: {e}")
    except Exception as e:
        # Handle other errors
        print(f"Unexpected error: {e}")
```

### 2. Error Reporting System

#### Error Reporting Functions

```cpp
// Error reporting functions
void reportError(const std::string& message, 
                 nlm::ErrorCode code,
                 nlm::ErrorCategory category);

void reportException(const std::exception& e);

void reportValidationError(const std::string& field,
                          const std::string& value,
                          const std::string& expected);
```

#### Python Error Reporting

```python
import pynlm

# Error reporting functions
pynlm.report_error("Configuration error", 
                  pynlm.ErrorCode.ConfigValidationFailed,
                  pynlm.ErrorCategory.Configuration)

pynlm.report_exception(ValueError("Test error"))
```

### 3. Graceful Degradation

#### Enabling Graceful Degradation

```cpp
#include "core/ErrorHandling/ErrorHandlingSystem.hpp"

void setupGracefulDegradation() {
    // Enable graceful degradation
    enableGracefulDegradation(true);
    
    // Set error threshold
    setGlobalErrorLevel(nlm::LogLevel::Warning);
    
    // Configure degradation behavior
    setDegradationMode(DegradationMode::ReduceComplexity);
}

void performOperationWithDegradation() {
    setupGracefulDegradation();
    
    try {
        // Complex operation that may fail
        if (operationMayFail()) {
            // Fallback to simpler operation
            performSimpleOperation();
        } else {
            performComplexOperation();
        }
        
    } catch (const std::exception& e) {
        // Use degraded mode
        handleDegradedOperation(e);
    }
}
```

### 4. Validation System

#### Parameter Validation

```cpp
#include "core/ErrorHandling/Validation.hpp"

void validateConfiguration(const nlm::Config& config) {
    // Validate required parameters
    if (!config.has("brain.neuron_count")) {
        NLM_THROW_CONFIG("Missing required parameter: brain.neuron_count");
    }
    
    // Validate parameter ranges
    int neuronCount = config.get<int>("brain.neuron_count");
    if (neuronCount < 10 || neuronCount > 100000) {
        NLM_THROW_CONFIG("Invalid neuron count: " + std::to_string(neuronCount));
    }
    
    // Validate parameter types
    float synapseDensity = config.get<float>("brain.synapse_density");
    if (synapseDensity < 0.0 || synapseDensity > 1.0) {
        NLM_THROW_CONFIG("Invalid synapse density: " + std::to_string(synapseDensity));
    }
}
```

#### Python Validation

```python
import pynlm

class ConfigurationValidator:
    @staticmethod
    def validate_neuron_count(config):
        if not config.has("brain.neuron_count"):
            raise pynlm.ValidationError("Missing required parameter: brain.neuron_count")
        
        neuron_count = config.get("brain.neuron_count")
        if neuron_count < 10 or neuron_count > 100000:
            raise pynlm.ValidationError(f"Invalid neuron count: {neuron_count}")
    
    @staticmethod
    def validate_synapse_density(config):
        if config.has("brain.synapse_density"):
            synapse_density = config.get("brain.synapse_density")
            if synapse_density < 0.0 or synapse_density > 1.0:
                raise pynlm.ValidationError(f"Invalid synapse density: {synapse_density}")

# Usage
validator = ConfigurationValidator()
validator.validate_neuron_count(config)
validator.validate_synapse_density(config)
```

### 5. Logging and Monitoring

#### Logging Levels

| Level | Description | Use Case |
|-------|-------------|----------|
| DEBUG | Detailed diagnostic information | Development, debugging |
| INFO | General operational information | Normal operation |
| WARNING | Potential issues | Warnings about configuration |
| ERROR | Actual errors | Runtime errors, failures |
| CRITICAL | Severe errors | System failures, crashes |

#### Logging Functions

```cpp
#include "core/Logger/Logger.hpp"

void setupLogging() {
    // Initialize logger
    initializeLogger("nlm.log", LogLevel::Info);
    
    // Set up log format
    setLogFormat("[%(timestamp)s] %(level)s: %(message)s");
}

void logOperation(const std::string& operation) {
    LOG_INFO("Performing operation: " + operation);
}

void logError(const std::string& operation, const std::exception& e) {
    LOG_ERROR("Operation " + operation + " failed: " + e.what());
}
```

#### Python Logging

```python
import pynlm
import logging

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm.log'),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger('nlm')

def log_operation(operation):
    logger.info(f"Performing operation: {operation}")

def log_error(operation, error):
    logger.error(f"Operation {operation} failed: {error}")
```

## Error Handling Examples

### Example 1: Complete Error Handling

```cpp
#include "core/ErrorHandling/ErrorHandlingSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>

int main() {
    // Initialize the error handling system
    std::cout << "Initializing NLM error handling system..." << std::endl;
    if (!initializeErrorHandling("", true)) {
        std::cerr << "Failed to initialize error handling system!" << std::endl;
        return 1;
    }
    std::cout << "Error handling system initialized successfully." << std::endl;
    
    try {
        // Example 1: Configuration validation
        std::cout << "\n1. Testing Configuration Validation..." << std::endl;
        nlm::Config config;
        
        // Try to set an empty key (should throw)
        try {
            config.set("", "value");
            std::cerr << "ERROR: Should have thrown exception for empty key!" << std::endl;
        } catch (const nlm::ValidationException& e) {
            std::cout << "Caught expected ValidationException: " << e.what() << std::endl;
        }
        
        // Example 2: Valid configuration
        std::cout << "\n2. Testing Valid Configuration..." << stdendl;
        config.set("simulation.steps", 1000);
        config.set("simulation.dt", 0.001);
        config.set("neuron.count", 100);
        
        std::cout << "Configuration summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Example 3: Neuron creation and validation
        std::cout << "\n3. Testing Neuron Creation..." << std::endl;
        nlm::Neuron neuron(nlm::NeuronId(1));
        std::cout << "Created neuron with ID: " << neuron.getId().index() << std::endl;
        
        // Try to create neuron with invalid ID (should throw)
        try {
            nlm::Neuron invalidNeuron(nlm::INVALID_NEURON_ID);
            std::cerr << "ERROR: Should have thrown exception for invalid neuron ID!" << std::endl;
        } catch (const nlm::ValidationException& e) {
            std::cout << "Caught expected ValidationException: " << e.what() << std::endl;
        }
        
        // Example 4: Error reporting
        std::cout << "\n4. Testing Error Reporting..." << std::endl;
        reportError("This is a test configuration error", nlm::ErrorCode::ConfigValidationFailed, nlm::ErrorCategory::Configuration);
        
        // Example 5: Exception reporting
        std::cout << "\n5. Testing Exception Reporting..." << std::endl;
        try {
            NLM_THROW_CONFIG("This is a test exception");
        } catch (const std::exception& e) {
            reportException(e);
            std::cout << "Exception reported successfully." << std::endl;
        }
        
        // Example 6: Get error statistics
        std::cout << "\n6. Error Statistics..." << std::endl;
        nlm::ErrorStats stats = getErrorStats();
        std::cout << stats.getStatsSummary() << std::endl;
        
        // Example 7: Enable graceful degradation
        std::cout << "\n7. Testing Graceful Degradation..." << std::endl;
        enableGracefulDegradation(true);
        std::cout << "Graceful degradation enabled." << std::endl;
        
        // Example 8: Configuration validation
        std::cout << "\n8. Testing Validation Utilities..." << std::endl;
        
        // Test parameter validation
        bool valid = nlm::ValidationUtils::isInRange(50.0, 0.0, 100.0);
        std::cout << "Range validation (50 in [0, 100]): " << (valid ? "Valid" : "Invalid") << std::endl;
        
        // Test state validation
        nlm::NeuronState state;
        valid = nlm::StateValidator::isValidState(state);
        std::cout << "Neuron state validation: " << (valid ? "Valid" : "Invalid") << std::endl;
        
        // Test resource validation
        valid = nlm::ResourceValidator::validateMemory(1.0); // Check for 1GB memory
        std::cout << "Memory resource validation: " << (valid ? "Valid" : "Invalid") << std::endl;
        
        // Example 9: Integration validation
        std::cout << "\n9. Testing Integration Validation..." << std::endl;
        std::vector<std::string> features = {"logging", "validation", "metrics"};
        valid = nlm::IntegrationValidator::validateComponentCompatibility(
            "test_component",
            {"logging", "validation"},
            features);
        std::cout << "Component compatibility validation: " << (valid ? "Valid" : "Invalid") << std::endl;
        
        std::cout << "\nAll tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception: " << e.what() << std::endl;
        return 1;
    }
    
    // Shutdown the error handling system
    shutdownErrorHandling();
    std::cout << "\nError handling system shutdown complete." << std::endl;
    
    return 0;
}
```

### Example 2: Python Error Handling

```python
import pynlm
import logging

# Setup logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def run_simulation_with_error_handling(config_file="config.json"):
    """Run simulation with comprehensive error handling."""
    
    try:
        # Initialize error handling
        pynlm.initialize_error_handling("")
        
        # Load configuration
        config = pynlm.load_config_from_file(config_file)
        
        # Validate configuration
        validation_errors = config.validate()
        if validation_errors:
            for error in validation_errors:
                logger.error(f"Configuration error: {error}")
            raise pynlm.ConfigurationError(f"Configuration validation failed: {validation_errors}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            try:
                brain.step(step)
                
                # Log progress periodically
                if step % 100 == 0:
                    logger.info(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                    
            except pynlm.NeuralError as e:
                logger.error(f"Neural error at step {step}: {e}")
                # Continue with degradation
                if pynlm.is_graceful_degradation_enabled():
                    logger.warning("Continuing with graceful degradation")
                    brain.reset()
                    continue
                else:
                    raise
                    
            except Exception as e:
                logger.error(f"Unexpected error at step {step}: {e}")
                raise
        
        # Save final state
        brain.save("final_simulation_state.bin")
        logger.info("Simulation completed successfully")
        
        return brain
        
    except pynlm.ConfigurationError as e:
        logger.error(f"Configuration error: {e}")
        raise
        
    except pynlm.NeuralError as e:
        logger.error(f"Neural error: {e}")
        raise
        
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        raise
        
    finally:
        # Cleanup
        pynlm.shutdown_error_handling()
```

### Example 3: Error Recovery

```cpp
#include "core/ErrorHandling/ErrorHandlingSystem.hpp"

class ErrorRecoveryManager {
private:
    std::queue<std::function<void()>> recovery_actions_;
    bool degradation_enabled_ = false;
    
public:
    ErrorRecoveryManager() {
        enableGracefulDegradation(true);
    }
    
    void registerRecoveryAction(const std::function<void()>& action) {
        recovery_actions_.push(action);
    }
    
    bool handleError(const std::exception& error) {
        try {
            throw; // Re-throw to catch in higher-level handler
        } catch (const nlm::ValidationException& e) {
            // Handle validation errors
            std::cerr << "Validation error: " << e.what() << std::endl;
            return executeRecoveryActions();
        } catch (const nlm::NeuralException& e) {
            // Handle neural errors
            std::cerr << "Neural error: " << e.what() << std::endl;
            return executeRecoveryActions();
        } catch (const std::exception& e) {
            // Handle other errors
            std::cerr << "System error: " << e.what() << std::endl;
            return executeRecoveryActions();
        }
    }
    
private:
    bool executeRecoveryActions() {
        bool success = true;
        while (!recovery_actions_.empty()) {
            try {
                recovery_actions_.front()();
                recovery_actions_.pop();
            } catch (const std::exception& e) {
                std::cerr << "Recovery action failed: " << e.what() << std::endl;
                success = false;
            }
        }
        return success;
    }
};
```

## Error Handling Best Practices

### 1. Always Initialize Error Handling

```cpp
// Always initialize error handling at the start of your program
if (!initializeErrorHandling("/path/to/error.log", true)) {
    std::cerr << "Failed to initialize error handling!" << std::endl;
    return 1;
}
```

### 2. Use Specific Exception Types

```cpp
// Use specific exception types for different error categories
try {
    // Code that may throw
} catch (const nlm::ValidationException& e) {
    // Handle validation errors
} catch (const nlm::NeuralException& e) {
    // Handle neural errors
} catch (const nlm::MemoryException& e) {
    // Handle memory errors
} catch (const std::exception& e) {
    // Handle other errors
}
```

### 3. Validate Before Use

```cpp
// Always validate configuration before use
if (!config.has("required.parameter")) {
    NLM_THROW_CONFIG("Missing required parameter");
}

// Validate parameter values
int value = config.get<int>("parameter");
if (value < min_value || value > max_value) {
    NLM_THROW_CONFIG("Parameter value out of range");
}
```

### 4. Implement Graceful Degradation

```cpp
// Enable graceful degradation for robustness
enableGracefulDegradation(true);

// Set appropriate error levels
setGlobalErrorLevel(LogLevel::Warning);
```

### 5. Log Errors Appropriately

```cpp
// Use structured logging for errors
LOG_ERROR("Operation failed: " + operation_name + ", error: " + error_message);

// Include context in error messages
LOG_ERROR("Failed to process neuron " + std::to_string(neuron_id) + 
          " with error: " + error_message + ", context: " + context);
```

### 6. Handle Errors at Appropriate Level

```cpp
// Handle errors at the appropriate level
// Low level: Specific error handling
try {
    // Low-level operation
} catch (const std::exception& e) {
    // Handle low-level error
}

// High level: General error handling
try {
    // High-level operation
    performLowLevelOperation();
} catch (const std::exception& e) {
    // Handle high-level error, log details
    LOG_ERROR("High-level operation failed: " + std::string(e.what()));
}
```

## Error Handling Tools and Utilities

### Configuration Validation Tools

```python
import pynlm

def validate_config(config_path):
    """Validate configuration file."""
    config = pynlm.load_config_from_file(config_path)
    errors = config.validate()
    
    if errors:
        print("Configuration validation failed:")
        for error in errors:
            print(f"  - {error}")
        return False
    else:
        print("Configuration is valid!")
        return True

# Usage
validate_config("config.json")
```

### Error Statistics Tools

```python
import pynlm

def display_error_statistics():
    """Display error statistics."""
    stats = pynlm.get_error_stats()
    
    print("Error Statistics:")
    print(f"  Total Errors: {stats.total_errors}")
    print(f"  Errors by Category: {stats.errors_by_category}")
    print(f"  Errors by Code: {stats.errors_by_code}")
    print(f"  Error Rate: {stats.error_rate:.2%}")
    
    # Display error trends
    print("\nError Trends:")
    print(f"  Last Error Time: {stats.last_error_time}")
    print(f"  Consecutive Errors: {stats.consecutive_errors}")

# Usage
display_error_statistics()
```

### Performance Monitoring Tools

```python
import pynlm
import time
import psutil
import os

def monitor_error_handling_performance():
    """Monitor error handling performance."""
    process = psutil.Process(os.getpid())
    
    # Baseline memory usage
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Start monitoring
    start_time = time.time()
    
    # Perform operations that may trigger errors
    for i in range(100):
        try:
            # Simulate some operations
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.step(i)
            
        except Exception as e:
            # Error handling may be triggered
            pass
    
    # End monitoring
    end_time = time.time()
    final_memory = process.memory_info().rss / 1024 / 1024
    
    # Calculate performance metrics
    total_time = end_time - start_time
    memory_increase = final_memory - initial_memory
    
    print(f"Error handling performance:")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Memory increase: {memory_increase:.2f}MB")
    print(f"  Time per operation: {total_time/100:.4f}s")
    print(f"  Memory per operation: {memory_increase/100:.4f}MB")

# Usage
monitor_error_handling_performance()
```

## Conclusion

Error handling is a critical component of robust NLM operation. A comprehensive error handling system ensures:

1. **Reliability**: Graceful handling of errors and failures
2. **Debuggability**: Detailed error messages and statistics
3. **Performance**: Efficient error detection and recovery
4. **Maintainability**: Clear error categorization and handling

Key best practices:

1. **Initialize error handling** before any operations
2. **Use specific exception types** for different error categories
3. **Validate inputs** before processing
4. **Implement graceful degradation** for robustness
5. **Log errors appropriately** with context
6. **Handle errors at the appropriate level**

The error handling system provides the foundation for robust, reliable NLM operation in production environments.

---

*This error handling reference provides comprehensive information about NLM error handling mechanisms.*
*For configuration details, see CONFIGURATION_GUIDE.md.*
*For performance information, see PERFORMANCE_GUIDE.md.*
*For development practices, see DEVELOPMENT_GUIDE.md.*

**Last updated**: 2024-01-17
**Version**: 1.0.0