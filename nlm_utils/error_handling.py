# Error Handling and Validation Module

This module provides comprehensive error handling and validation utilities for the enhanced NLM Python API.

## Overview

The error handling module extends the basic NLM API with robust error management, validation utilities, and automatic recovery mechanisms. It provides:

- Comprehensive error class hierarchy for different types of errors
- Configuration validation with detailed error messages and suggestions
- Brain state validation for simulation integrity
- Safe function execution with retry mechanisms and timeout protection
- Automatic error recovery with intelligent recovery actions
- Detailed error logging with context and recovery information

## Key Features

### Error Classes

The module provides a comprehensive error hierarchy:

- **NLMError**: Base exception class for all NLM errors
- **ConfigurationError**: Errors in configuration with parameter details
- **BrainInitializationError**: Errors during brain initialization with suggestions
- **ValidationError**: Validation errors with severity levels and fixes
- **SimulationError**: Simulation-related errors with context
- **MemoryError**: Memory-related errors with recommendations
- **PerformanceError**: Performance-related errors with optimization suggestions

### Validation Utilities

```python
from nlm_utils.error_handling import (
    ConfigurationValidator,
    BrainStateValidator,
    validate_config_comprehensive,
    validate_brain_state_comprehensive
)

# Comprehensive configuration validation
validator = ConfigurationValidator()

# Add custom validation rules
validator.add_rule("brain.neuron_count", 
                  lambda x: x > 0, 
                  "Neuron count must be positive",
                  lambda x: f"Use a smaller count like {max(1, x//2)}")
validator.add_rule("brain.synapse_density",
                  lambda x: 0.0 <= x <= 1.0,
                  "Synapse density must be between 0.0 and 1.0",
                  lambda x: "Use 0.05 for sparse connections or 0.2 for dense connections")
validator.add_rule("plasticity.stdp.learning_rate",
                  lambda x: 0.0 <= x <= 1.0,
                  "Learning rate must be between 0.0 and 1.0",
                  lambda x: f"Use {min(0.01, x)} for stable learning")

# Validate configuration with comprehensive results
validation_result = validate_config_comprehensive(config)
print(f"Validation score: {validation_result['score']}/100")
print(f"Issues found: {validation_result['issues']}")
print(f"Suggestions: {validation_result['suggestions']}")
```

### Safe Function Execution

```python
from nlm_utils.error_handling import safe_function

# Safe wrapper functions with retry
@safe_function("brain_step", retries=3, timeout=1000, backoff=0.1)
def safe_brain_step(brain, step):
    brain.step(step)
    return True

# Safe simulation with error recovery
@safe_function("simulation_step", max_retries=3, on_error="auto_recover")
def robust_simulation_step(session, step):
    try:
        # Simulation logic here
        return True
    except Exception as e:
        # Error recovery handled by decorator
        raise
```

### Error Recovery

```python
from nlm_utils.error_handling import ErrorRecoveryManager

# Error recovery manager
recovery_manager = ErrorRecoveryManager("simulation_recovery")

@safe_function("simulation_step", max_retries=3, on_error="auto_recover")
def robust_simulation_step(session, step):
    try:
        # Simulation logic here
        return True
    except Exception as e:
        # Enhanced error recovery
        recovery_action = recovery_manager.recover_from_error(
            e, step, get_context=session.get_current_context()
        )
        raise
```

## Advanced Error Features

### Error Recovery Actions

The error recovery system supports multiple recovery actions:

- **reset_brain**: Reset brain state and reinitialize
- **reset_world**: Reset world state and reinitialize
- **reset_agent**: Reset agent state and reinitialize
- **restore_checkpoint**: Restore from last checkpoint
- **adjust_parameters**: Adjust simulation parameters
- **reduce_workload**: Reduce simulation complexity
- **skip_step**: Skip problematic step
- **retry_with_delay**: Retry with delay and exponential backoff

### Error Classification

Errors are automatically classified by type:

- **Configuration errors**: Invalid parameter values, missing configuration
- **Initialization errors**: Brain or world initialization failures
- **Simulation errors**: Runtime errors during simulation
- **Resource errors**: Memory or CPU resource issues
- **Performance errors**: Slow execution or bottlenecks
- **Network errors**: Communication failures

### Error Logging and Reporting

```python
from nlm_utils.error_handling import NLMError

class CustomError(NLMError):
    """Custom error with additional context."""
    
    def __init__(self, message, error_code=None, parameter=None, suggested_fix=None):
        super().__init__(message, error_code)
        self.parameter = parameter
        self.suggested_fix = suggested_fix
    
    def __str__(self):
        base_msg = super().__str__()
        if self.parameter:
            base_msg += f" (Parameter: {self.parameter})"
        if self.suggested_fix:
            base_msg += f" (Suggested fix: {self.suggested_fix})"
        return base_msg

# Usage
try:
    raise ConfigurationError(
        "Invalid configuration parameter",
        error_code="CONFIG_INVALID_PARAMETER",
        parameter="brain.neuron_count",
        suggested_fix="Use a value between 100 and 10000"
    )
except CustomError as e:
    print(f"Caught error: {e}")
    print(f"Error code: {e.error_code}")
    print(f"Parameter: {e.parameter}")
    print(f"Suggested fix: {e.suggested_fix}")
```

## Integration Examples

### Enhanced Brain Creation with Error Handling

```python
from nlm_utils import (
    createBrain, 
    validate_config, 
    safe_function,
    BrainStateValidator
)

# Enhanced brain creation with validation
config = {
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": True,
    "performance.monitoring": True,
    "error_handling.recovery": True
}

# Validate configuration before creating brain
if not validate_config(config):
    print("Configuration errors:", config.getErrors())
    # Fix configuration or exit
    exit(1)

# Create brain with error recovery
@safe_function("brain_creation", retries=2)
def create_enhanced_brain(config):
    brain = createBrain(config)
    brain.initialize()
    return brain

try:
    brain = create_enhanced_brain(config)
    print("Brain created successfully")
except BrainInitializationError as e:
    print(f"Brain initialization failed: {e}")
    # Attempt recovery
    brain = create_enhanced_brain(config)
```

### Comprehensive Error Handling in Simulation

```python
from nlm_utils import (
    AdvancedBrainSession, SessionConfigAdvanced,
    AdvancedDebugLogger, ErrorRecoveryManager
)

# Create session with comprehensive error handling
config = SessionConfigAdvanced(
    name="error_handling_demo",
    brain_config={
        "brain.neuron_count": 500,
        "performance.monitoring": True,
        "error_handling.recovery": True
    },
    checkpoint_interval=100,
    auto_save=True
)

session = AdvancedBrainSession(config)
logger = AdvancedDebugLogger()
recovery_manager = ErrorRecoveryManager("simulation_recovery")

@safe_function("simulation_step", max_retries=3, on_error="auto_recover")
def enhanced_simulation_step(session, step):
    try:
        # Run simulation step with comprehensive error handling
        session.world.update(0.1)
        percept = session.world.getSensoryPercept()
        session.agent.processSensoryInput(percept)
        session.brain.step(step)
        action = session.agent.decodeMotorCommand()
        session.world.applyMotorCommand(action, session.world.getSimulationTime())
        
        # Apply neuromodulation with error handling
        reward = session.get_reward()
        session.agent.applyRewardModulation(reward, 0.0)
        
        # Update development with error handling
        session.agent.updateDevelopment(0.1)
        
        return True
        
    except NLMError as e:
        # Log error with context
        logger.log_error(step, f"NLM error: {e}")
        
        # Apply error recovery
        recovery_action = recovery_manager.recover_from_error(
            e, step, get_context=session.get_current_context()
        )
        logger.log_recovery_action(step, recovery_action)
        
        # Attempt recovery
        try:
            return perform_recovery(session, step, recovery_action)
        except Exception as recovery_error:
            logger.log_error(step, f"Recovery failed: {recovery_error}")
            raise
    
    except Exception as e:
        # Handle unexpected errors
        logger.log_error(step, f"Unexpected error: {e}")
        recovery_action = recovery_manager.recover_from_error(
            e, step, get_context=session.get_current_context()
        )
        raise
```

## Best Practices

### 1. Validate Configuration Before Use

Always validate configuration before creating brains or running simulations:

```python
from nlm_utils import validate_config

if not validate_config(config):
    print("Configuration has errors:", config.getErrors())
    exit(1)
```

### 2. Use Safe Functions for Critical Operations

Wrap critical operations with retry mechanisms:

```python
from nlm_utils import safe_function

@safe_function("brain_step", retries=3, timeout=5000)
def critical_brain_step(brain, step):
    brain.step(step)
```

### 3. Implement Comprehensive Error Recovery

Implement multi-layered error recovery:

```python
from nlm_utils import ErrorRecoveryManager

recovery_manager = ErrorRecoveryManager("my_app")

def robust_operation():
    try:
        # Critical operation
        result = perform_operation()
        return result
    except NLMError as e:
        # Log error and attempt recovery
        recovery_action = recovery_manager.recover_from_error(e, get_context=...)
        return perform_recovery(recovery_action)
```

### 4. Log Errors for Debugging

Always log errors with sufficient context:

```python
from nlm_utils import AdvancedDebugLogger

logger = AdvancedDebugLogger()

try:
    # Some operation
    pass
except Exception as e:
    logger.log_error("operation_name", str(e), {
        'step': current_step,
        'context': current_context,
        'timestamp': time.time()
    })
```

## Performance Considerations

### Error Handling Overhead

- Error handling has minimal overhead when no errors occur
- Configure error levels to reduce logging overhead in production
- Use structured logging for better performance

### Recovery Performance

- Recovery actions should be optimized for speed
- Cache recovery results when possible
- Implement progressive recovery (try simpler actions first)

### Memory Usage

- Error objects store context; use sparingly in long-running simulations
- Implement error pooling for common error types
- Use weak references for context to avoid memory leaks

## Migration Guide

### From Basic Error Handling to Enhanced Error Handling

```python
# Basic error handling
try:
    brain.step(step)
except Exception as e:
    print(f"Error: {e}")

# Enhanced error handling
from nlm_utils import safe_function

@safe_function("brain_step", retries=3)
def safe_brain_step(brain, step):
    brain.step(step)
```

### Adding Validation to Existing Code

```python
# Basic code
brain = createBrain(config)
brain.initialize()

# Enhanced code with validation
from nlm_utils import validate_config

if validate_config(config):
    brain = createBrain(config)
    brain.initialize()
else:
    print("Configuration invalid:", config.getErrors())
```

### Implementing Error Recovery

```python
# Basic error handling
try:
    session.brain.step(step)
except Exception as e:
    print(f"Step failed: {e}")
    # Manual recovery
    session.recover()

# Enhanced error handling with recovery
from nlm_utils import safe_function

@safe_function("simulation_step", max_retries=3, on_error="auto_recover")
def enhanced_step(session, step):
    # Operation here
    pass
```

## Conclusion

The error handling module provides comprehensive tools for robust error management in NLM applications. It enables:

- **Early detection** of configuration and state issues
- **Automatic recovery** from common errors
- **Detailed error reporting** for debugging
- **Performance monitoring** with error tracking
- **Safe execution** with retry mechanisms

These enhancements make NLM applications more reliable and easier to debug while maintaining backward compatibility.

For more advanced error handling features, see the other utility modules in the nlm_utils package.
