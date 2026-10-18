# Enhanced Error Handling Documentation

# Overview

This document provides comprehensive information about the enhanced error handling and robustness features implemented in the NLM system.

## Key Improvements

### 1. Comprehensive Exception Hierarchy

The enhanced error handling system includes a complete exception hierarchy:

```python
# Exception classes
import pynlm

# Base NLM exception
try:
    result = some_operation()
except pynlm.NLMError as e:
    print(f"NLM error: {e}")

# Configuration-specific exceptions
try:
    config = pynlm.createEnhancedConfig("missing.json")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")

# Simulation-specific exceptions
try:
    brain.step(0)
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")

# Memory-specific exceptions
try:
    memory_operation()
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
```

### 2. Enhanced Error Messages

Error messages now include comprehensive context information:

```python
# Enhanced error messages with context
try:
    config = pynlm.createEnhancedConfig("config.json")
except pynlm.ConfigurationError as e:
    print(f"Error: {e}")
    # Output: "Error: Configuration Error: Failed to load configuration from: config.json"
```

### 3. Error Recovery and Fallback

The system implements graceful error recovery with automatic fallback:

```python
# Error recovery with fallback
if not config.loadFromFile("production.json"):
    NLM_LOG_WARNING("Failed to load production config")
    # Automatic fallback to defaults
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
```

### 4. Input Validation

Enhanced input validation prevents invalid operations:

```python
# Type validation
try:
    config.set("brain.neuron_count", "invalid")
except pynlm.ConfigurationError as e:
    print(f"Validation error: {e}")
    # Output: "Validation error: Invalid configuration value for key: brain.neuron_count"

# Range validation
try:
    config.set("brain.v_thresh", 100.0)  # Too high
except pynlm.ConfigurationError as e:
    print(f"Range error: {e}")
    # Output: "Range error: Configuration value out of range for key: brain.v_thresh"
```

### 5. Exception Safety

The system ensures exception safety in all operations:

```python
# Exception safety in complex operations
try:
    # Multiple operations that should be atomic
    config.loadFromFile("critical.conf")
    world.reset()
    brain.initialize()
    for step in range(100):
        brain.step(step)
except pynlm.NLMError as e:
    # System continues with partial state if possible
    print(f"Operation failed, but system continues: {e}")
```

## Error Handling Patterns

### Pattern 1: Safe Configuration Loading

```python
import pynlm

def createBrainWithFallback(config_path=None):
    """Create brain with configuration fallback"""
    brain = None
    
    if config_path:
        try:
            config = pynlm.createEnhancedConfig(config_path)
            brain = pynlm.createEnhancedBrain(config)
            print(f"Successfully loaded configuration from {config_path}")
        except pynlm.ConfigurationError as e:
            print(f"Warning: Failed to load configuration: {e}")
            # Fallback to default configuration
            config = pynlm.createEnhancedConfig()\
                .set("brain.neuron_count", 1000)\
                .set("plasticity.enable", True)\
                .set("neuromodulation.curiosity.enable", True)
            brain = pynlm.createEnhancedBrain(config)
            print("Using default configuration")
    else:
        # Use default configuration
        config = pynlm.createEnhancedConfig()\
            .set("brain.neuron_count", 1000)\
            .set("plasticity.enable", True)
        brain = pynlm.createEnhancedBrain(config)
        print("Using default configuration")
    
    # Initialize brain
    try:
        brain.initialize()
    except pynlm.SimulationError as e:
        print(f"Warning: Brain initialization failed: {e}")
        # Brain may be in partial state
    
    return brain

# Usage
brain = createBrainWithFallback("my_config.json")
```

### Pattern 2: Robust Simulation Loop

```python
import pynlm

def runSimulation(agent, world, duration, max_retries=3):
    """Robust simulation loop with error handling"""
    for attempt in range(max_retries):
        try:
            with agent.createEpisode(world) as episode:
                rewards = episode.runEpisode(duration)
                print(f"Simulation completed successfully on attempt {attempt + 1}")
                return rewards
        except pynlm.SimulationError as e:
            print(f"Simulation failed on attempt {attempt + 1}: {e}")
            if attempt == max_retries - 1:
                print("Max retries reached. Returning to caller.")
                raise
            # Wait a bit before retrying
            import time
            time.sleep(0.1 * (attempt + 1))
        except pynlm.NLMError as e:
            print(f"Critical error: {e}")
            raise
    
    return None

# Usage
brain = pynlm.createEnhancedBrain(config)
agent = pynlm.createEnhancedAgent(brain)
world = pynlm.createEnhancedWorld()

rewards = runSimulation(agent, world, 60.0, max_retries=3)
```

### Pattern 3: Configuration Validation

```python
import pynlm
import json

def validateConfiguration(config_dict):
    """Validate configuration dictionary"""
    required_keys = ["brain.neuron_count", "plasticity.enable", "neuromodulation.curiosity.enable"]
    
    for key in required_keys:
        if key not in config_dict:
            raise pynlm.ConfigurationError(f"Missing required configuration key: {key}")
    
    # Validate value ranges
    if not (100 <= config_dict["brain.neuron_count"] <= 10000):
        raise pynlm.ConfigurationError("brain.neuron_count must be between 100 and 10000")
    
    if not isinstance(config_dict["plasticity.enable"], bool):
        raise pynlm.ConfigurationError("plasticity.enable must be a boolean")
    
    if not isinstance(config_dict["neuromodulation.curiosity.enable"], bool):
        raise pynlm.ConfigurationError("neuromodulation.curiosity.enable must be a boolean")
    
    return True

def createConfigurationFromDict(config_dict):
    """Create configuration from validated dictionary"""
    try:
        validateConfiguration(config_dict)
        
        config = pynlm.createEnhancedConfig()
        
        if "brain.neuron_count" in config_dict:
            config.set("brain.neuron_count", config_dict["brain.neuron_count"])
        
        if "brain.connection_probability" in config_dict:
            config.set("brain.connection_probability", config_dict["brain.connection_probability"])
        
        if "plasticity.enable" in config_dict:
            config.set("plasticity.enable", config_dict["plasticity.enable"])
        
        if "plasticity.stdp.learning_rate" in config_dict:
            config.set("plasticity.stdp.learning_rate", config_dict["plasticity.stdp.learning_rate"])
        
        if "neuromodulation.curiosity.enable" in config_dict:
            config.set("neuromodulation.curiosity.enable", config_dict["neuromodulation.curiosity.enable"])
        
        if "neuromodulation.novelty.scale" in config_dict:
            config.set("neuromodulation.novelty.scale", config_dict["neuromodulation.novelty.scale"])
        
        return config
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration validation failed: {e}")
        raise

# Usage with JSON configuration
config_json = {
    "brain.neuron_count": 2000,
    "brain.connection_probability": 0.05,
    "plasticity.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "neuromodulation.curiosity.enable": True,
    "neuromodulation.novelty.scale": 1.0
}

config = createConfigurationFromDict(config_json)
```

## Error Handling Best Practices

### 1. Use Specific Exception Types

```python
# Good: Use specific exception types
try:
    config = pynlm.createEnhancedConfig("missing.json")
except pynlm.ConfigurationError as e:
    handleConfigurationError(e)

# Bad: Catch all exceptions
try:
    config = pynlm.createEnhancedConfig("missing.json")
except Exception as e:
    # Too generic, hides other errors
    print(f"Error: {e}")
```

### 2. Don't Suppress Exceptions

```python
# Bad: Suppressing exceptions hides problems
try:
    result = some_operation()
except pynlm.NLMError:
    result = None  # Silent failure

# Good: Handle or re-raise exceptions
try:
    result = some_operation()
except pynlm.NLMError as e:
    logError(e)
    raise  # Re-raise after logging
```

### 3. Provide Meaningful Error Messages

```python
# Good: Provide context in error messages
try:
    config = pynlm.createEnhancedConfig("production.conf")
except pynlm.ConfigurationError as e:
    print(f"Failed to load production configuration: {e}")
    print("Please check the file path and format")

# Bad: Too generic error message
try:
    config = pynlm.createEnhancedConfig("production.conf")
except Exception as e:
    print(f"Error: {e}")  # Doesn't provide context
```

### 4. Use Context Managers

```python
# Use context managers for resource management
try:
    with agent.createEpisode(world) as episode:
        rewards = episode.runEpisode(10.0)
        print(f"Episode completed with {len(rewards)} rewards")
except pynlm.NLMError as e:
    print(f"Episode failed: {e}")
```

## Error Recovery Strategies

### 1. Graceful Degradation

```python
def createBrainWithGracefulDegradation(config_path=None):
    """Create brain with graceful degradation"""
    try:
        if config_path:
            config = pynlm.createEnhancedConfig(config_path)
        else:
            config = pynlm.createEnhancedConfig()
        
        brain = pynlm.createEnhancedBrain(config)
        brain.initialize()
        
        # Try to run a simple test step
        try:
            brain.step(0)
            print("Brain initialized and tested successfully")
            return brain
        except pynlm.SimulationError:
            print("Warning: Brain test step failed, but continuing")
            return brain  # Return brain even if test fails
            
    except pynlm.ConfigurationError:
        print("Error: Invalid configuration, using defaults")
        # Create minimal configuration
        config = pynlm.createEnhancedConfig()\
            .set("brain.neuron_count", 100)\
            .set("plasticity.enable", False)
        brain = pynlm.createEnhancedBrain(config)
        return brain
    except pynlm.SimulationError:
        print("Error: Brain initialization failed")
        raise
```

### 2. Retry with Backoff

```python
import time
from functools import wraps

def retry_on_error(max_retries=3, initial_delay=0.1):
    """Decorator for retrying operations with exponential backoff"""
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            for attempt in range(max_retries):
                try:
                    return func(*args, **kwargs)
                except pynlm.NLMError as e:
                    if attempt == max_retries - 1:
                        raise
                    delay = initial_delay * (2 ** attempt)
                    print(f"Operation failed, retrying in {delay} seconds: {e}")
                    time.sleep(delay)
        return wrapper
    return decorator

@retry_on_error(max_retries=3, initial_delay=0.1)
def loadConfigurationWithRetry(filepath):
    return pynlm.createEnhancedConfig(filepath)

# Usage
config = loadConfigurationWithRetry("critical.conf")
```

### 3. Circuit Breaker Pattern

```python
class CircuitBreaker:
    def __init__(self, failure_threshold=5, recovery_timeout=60):
        self.failure_threshold = failure_threshold
        self.recovery_timeout = recovery_timeout
        self.failure_count = 0
        self.last_failure_time = 0
        self.state = "CLOSED"  # CLOSED, OPEN, HALF_OPEN
    
    def call(self, func, *args, **kwargs):
        if self.state == "OPEN":
            if time.time() - self.last_failure_time > self.recovery_timeout:
                self.state = "HALF_OPEN"
            else:
                raise pynlm.NLMError("Circuit breaker is OPEN")
        
        try:
            result = func(*args, **kwargs)
            self.onSuccess()
            return result
        except pynlm.NLMError as e:
            self.onFailure()
            raise
    
    def onSuccess(self):
        self.failure_count = 0
        self.state = "CLOSED"
    
    def onFailure(self):
        self.failure_count += 1
        self.last_failure_time = time.time()
        
        if self.failure_count >= self.failure_threshold:
            self.state = "OPEN"

# Usage with circuit breaker
circuit_breaker = CircuitBreaker(failure_threshold=3, recovery_timeout=30)

try:
    config = circuit_breaker.call(pynlm.createEnhancedConfig, "risky.conf")
except pynlm.NLMError as e:
    print(f"Operation failed: {e}")
```

## Testing Error Handling

### 1. Unit Tests for Error Handling

```python
import pytest
import pynlm

def test_configuration_loading_success():
    """Test successful configuration loading"""
    config = pynlm.createEnhancedConfig()
    assert config is not None

def test_configuration_loading_failure():
    """Test configuration loading failure"""
    with pytest.raises(pynlm.ConfigurationError):
        pynlm.createEnhancedConfig("nonexistent.json")

def test_invalid_configuration_value():
    """Test invalid configuration value"""
    config = pynlm.createEnhancedConfig()
    with pytest.raises(pynlm.ConfigurationError):
        config.set("brain.neuron_count", "invalid")

def test_simulation_error_handling():
    """Test simulation error handling"""
    brain = pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())
    # Brain should be in a valid state even after initialization failure
    assert brain is not None
```

### 2. Integration Tests for Error Handling

```python
import pynlm
import tempfile
import os

def test_full_error_handling_workflow():
    """Test complete error handling workflow"""
    # Create temporary configuration file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_data = {
            "brain.neuron_count": 1000,
            "brain.connection_probability": 0.1,
            "plasticity.enable": True
        }
        json.dump(config_data, f)
        temp_file = f.name
    
    try:
        # Test successful loading
        config = pynlm.createEnhancedConfig(temp_file)
        assert config is not None
        
        # Test loading with non-existent file (should fail gracefully)
        with pytest.raises(pynlm.ConfigurationError):
            pynlm.createEnhancedConfig("nonexistent.json")
            
    finally:
        # Cleanup
        os.unlink(temp_file)

def test_error_recovery_workflow():
    """Test error recovery workflow"""
    # Create configuration with fallback
    config = pynlm.createEnhancedConfig()\
        .set("brain.neuron_count", 1000)\
        .set("plasticity.enable", True)
    
    # Create brain
    brain = pynlm.createEnhancedBrain(config)
    
    # Test error handling during operation
    try:
        brain.initialize()
        brain.step(0)
        print("Brain initialized and stepped successfully")
    except pynlm.SimulationError as e:
        print(f"Simulation error (expected in test): {e}")
```

## Performance Considerations

### 1. Error Handling Overhead

```python
import time

def measureErrorHandlingOverhead():
    """Measure performance impact of error handling"""
    # Normal operation
    start = time.time()
    for i in range(1000):
        config = pynlm.createEnhancedConfig()
    normal_time = time.time() - start
    
    # With error handling
    start = time.time()
    for i in range(1000):
        try:
            config = pynlm.createEnhancedConfig("missing.json")
        except pynlm.ConfigurationError:
            config = pynlm.createEnhancedConfig()
    error_time = time.time() - start
    
    print(f"Normal operation: {normal_time:.4f}s")
    print(f"With error handling: {error_time:.4f}s")
    print(f"Overhead: {((error_time - normal_time) / normal_time * 100):.2f}%")
```

### 2. Memory Usage in Error Conditions

```python
import psutil
import os

def measureMemoryUsage():
    """Measure memory usage in error conditions"""
    process = psutil.Process(os.getpid())
    
    # Normal memory usage
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Create many configurations
    configs = []
    for i in range(100):
        try:
            config = pynlm.createEnhancedConfig("missing.json")
        except pynlm.ConfigurationError:
            config = pynlm.createEnhancedConfig()
        configs.append(config)
    
    peak_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    # Cleanup
    del configs
    import gc
    gc.collect()
    
    final_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    print(f"Initial memory: {initial_memory:.2f} MB")
    print(f"Peak memory: {peak_memory:.2f} MB")
    print(f"Final memory: {final_memory:.2f} MB")
    print(f"Memory overhead: {peak_memory - initial_memory:.2f} MB")
```

## Conclusion

Enhanced error handling is crucial for robust and reliable NLM operations. The improvements include:

1. **Comprehensive Exception Hierarchy**: Specific exception types for different error conditions
2. **Graceful Error Recovery**: System continues operating even when components fail
3. **Input Validation**: Prevents invalid operations with meaningful error messages
4. **Exception Safety**: Ensures consistent state even when exceptions occur
5. **Performance Considerations**: Balances error handling overhead with robustness

These improvements make the NLM system more reliable, user-friendly, and suitable for both educational and research applications.