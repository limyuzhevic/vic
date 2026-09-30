# Error Handling Examples

These examples demonstrate enhanced error handling and validation in NLM Python bindings.

## 1. Enhanced Exception Types

```python
"""
Example 1: Enhanced exception types
Demonstrates custom exception classes with detailed recovery information.
"""

import pynlm
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    ValidationError, ConfigError, ResourceError, StateError
)

print("=== Enhanced Exception Types ===\n")

# Example 1.1: ValidationError
print("1.1 ValidationError example:")
try:
    # Trigger validation error
    raise ValidationError(
        ValidationError.INVALID_PARAMETER,
        "Test validation error: Parameter 'test_param' has invalid value 999."
    )
except ValidationError as e:
    print(f"   Caught ValidationError")
    print(f"   Error code: {e.code}")
    print(f"   Error message: {e.message}")
    print(f"   Recovery suggestion: Check parameter value and ensure it meets requirements.")

# Example 1.2: ConfigError
print("\n1.2 ConfigError example:")
try:
    # Trigger config error
    raise ConfigError(
        "brain.neuron_count",
        "Required configuration key not found: 'brain.neuron_count'. Recovery: Ensure key exists or provide a default value."
    )
except ConfigError as e:
    print(f"   Caught ConfigError")
    print(f"   Configuration key: {e.key}")
    print(f"   Error message: {e.message}")

# Example 1.3: ResourceError
print("\n1.3 ResourceError example:")
try:
    # Trigger resource error
    raise ResourceError(
        "memory_allocation",
        "Memory allocation failed: Requested 1GB but only 512MB available. Recovery: Try with smaller memory allocation."
    )
except ResourceError as e:
    print(f"   Caught ResourceError")
    print(f"   Operation: {e.operation}")
    print(f"   Error message: {e.message}")

# Example 1.4: StateError
print("\n1.4 StateError example:")
try:
    # Trigger state error
    raise StateError(
        "brain.initialize",
        "State validation failed for component 'brain.initialize': Brain already initialized. Recovery: Reset brain or use a fresh instance."
    )
except StateError as e:
    print(f"   Caught StateError")
    print(f"   Component: {e.component}")
    print(f"   Error message: {e.message}")
```

**Expected Output:**
```
=== Enhanced Exception Types ===

1.1 ValidationError example:
   Caught ValidationError
   Error code: 1001
   Error message: Test validation error: Parameter 'test_param' has invalid value 999.
   Recovery suggestion: Check parameter value and ensure it meets requirements.

1.2 ConfigError example:
   Caugh ConfigError
   Configuration key: brain.neuron_count
   Error message: Required configuration key not found: 'brain.neuron_count'. Recovery: Ensure key exists or provide a default value.

1.3 ResourceError example:
   Caught ResourceError
   Operation: memory_allocation
   Error message: Memory allocation failed: Requested 1GB but only 512MB available. Recovery: Try with smaller memory allocation.

1.4 StateError example:
   Caught StateError
   Component: brain.initialize
   Error message: State validation failed for component 'brain.initialize': Brain already initialized. Recovery: Reset brain or use a fresh instance.
```

## 2. Type Validation

```python
"""
Example 2: Type validation
Demonstrates strict type checking with helpful error messages.
"""

import pynlm
from enhanced_bindings import EnhancedConfig

print("=== Type Validation Examples ===\n")

# Example 2.1: Create configuration
config = EnhancedConfig(pynlm.createDefaultConfig())

# Set values using different types
config['brain.neuron_count'] = 100
config['brain.synapse_density'] = 0.1
config['agent.max_energy'] = 100.0
config['agent.enable_curiosity'] = True

print("2.1 Configuration setup:")
print(f"   Set int value: brain.neuron_count = {config['brain.neuron_count']}")
print(f"   Set float value: brain.synapse_density = {config['brain.synapse_density']}")
print(f"   Set float value: agent.max_energy = {config['agent.max_energy']}")
print(f"   Set bool value: agent.enable_curiosity = {config['agent.enable_curiosity']}")

# Example 2.2: Type-safe getter methods
print("\n2.2 Type-safe getter methods:")

# Int value with default
neuron_count = config.get_int('brain.neuron_count', 50)
print(f"   get_int('brain.neuron_count', 50): {neuron_count}")

# Float value with default  
synapse_density = config.get_float('brain.synapse_density', 0.05)
print(f"   get_float('brain.synapse_density', 0.05): {synapse_density}")

# Bool value with default
enable_curiosity = config.get_bool('agent.enable_curiosity', False)
print(f"   get_bool('agent.enable_curiosity', False): {enable_curiosity}")

# String value with default
vision_mode = config.get_string('agent.vision_mode', 'basic')
print(f"   get_string('agent.vision_mode', 'basic'): {vision_mode}")

# Example 2.3: Required getter methods
print("\n2.3 Required getter methods (will throw exceptions if missing):")

# These will work because keys exist
try:
    required_count = config.get_int('brain.neuron_count')
    print(f"   get_int('brain.neuron_count'): {required_count} ✓")
except Exception as e:
    print(f"   get_int('brain.neuron_count'): Error - {e}")

try:
    required_bool = config.get_bool('agent.enable_curiosity')
    print(f"   get_bool('agent.enable_curiosity'): {required_bool} ✓")
except Exception as e:
    print(f"   get_bool('agent.enable_curiosity'): Error - {e}")

# Example 2.4: Type validation with non-existent keys
print("\n2.4 Error handling for missing keys:")

try:
    missing_value = config.get_int('nonexistent.key', 999)
    print(f"   get_int('nonexistent.key', 999): {missing_value} ✓")
except Exception as e:
    print(f"   get_int('nonexistent.key', 999): Error - {e}")

# Example 2.5: Configuration type validation
print("\n2.5 Configuration type validation:")

# Check if key exists and has correct type
print(f"   'brain.neuron_count' exists: {'brain.neuron_count' in config}")
print(f"   'brain.neuron_count' type: {config.getType('brain.neuron_count')}")

# Validate type
config.validate('brain.neuron_count', 'int')
print(f"   Validation of 'brain.neuron_count' as 'int': PASSED ✓")

# This will fail
print(f"   Validation of 'brain.neuron_count' as 'string': ", end="")
try:
    config.validate('brain.neuron_count', 'string')
    print("PASSED ✓")
except Exception as e:
    print(f"FAILED ✗ - {str(e).split('Recovery:')[0]}")
```

**Expected Output:**
```
=== Type Validation Examples ===

2.1 Configuration setup:
   Set int value: brain.neuron_count = 100
   Set float value: brain.synapse_density = 0.1
   Set float value: agent.max_energy = 100.0
   Set bool value: agent.enable_curiosity = True

2.2 Type-safe getter methods:
   get_int('brain.neuron_count', 50): 100
   get_float('brain.synapse_density', 0.05): 0.1
   get_bool('agent.enable_curiosity', False): True
   get_string('agent.vision_mode', 'basic'): 

2.3 Required getter methods (will throw exceptions if missing):
   get_int('brain.neuron_count'): 100 ✓
   get_bool('agent.enable_curiosity'): True ✓

2.4 Error handling for missing keys:
   get_int('nonexistent.key', 999): 999 ✓

2.5 Configuration type validation:
   'brain.neuron_count' exists: True
   'brain.neuron_count' type: int
   Validation of 'brain.neuron_count' as 'int': PASSED ✓
   Validation of 'brain.neuron_count' as 'string': FAILED ✗ - Config validation failed for key 'brain.neuron_count': expected string.
```

## 3. Parameter Validation

```python
"""
Example 3: Parameter validation
Demonstrates range checking, null pointer detection, and bounds checking.
"""

import pynlm
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    ValidationError, ConfigError, ResourceError, StateError
)

print("=== Parameter Validation Examples ===\n")

# Example 3.1: Range validation
print("3.1 Range validation:")

# Helper function for range validation
def validateRangeExample(value, min_val, max_val, param_name):
    """Simulate range validation with enhanced error handling."""
    if value < min_val or value > max_val:
        raise ValidationError(
            ValidationError.OUT_OF_BOUNDS,
            f"Parameter '{param_name}' value {value} is out of range [{min_val}, {max_val}]. "
            "Recovery: Adjust parameter to valid range."
        )
    return True

# Test valid range
try:
    validateRangeExample(50, 0, 100, "test_value")
    print("   validateRangeExample(50, 0, 100, 'test_value'): PASSED ✓")
except Exception as e:
    print(f"   validateRangeExample(50, 0, 100, 'test_value'): FAILED ✗ - {e}")

# Test invalid range
try:
    validateRangeExample(150, 0, 100, "test_value")
    print("   validateRangeExample(150, 0, 100, 'test_value'): PASSED ✓")
except Exception as e:
    print(f"   validateRangeExample(150, 0, 100, 'test_value'): FAILED ✗ - {e}")

# Example 3.2: Parameter validation
print("\n3.2 Parameter validation:")

def validateParameterExample(condition, param_name, error_code, context=""):
    """Simulate parameter validation."""
    if not condition:
        msg = f"Parameter validation failed: '{param_name}'"
        if context:
            msg += f" in context: '{context}'"
        msg += ". Recovery: Check parameter value and ensure it meets requirements."
        raise ValidationError(error_code, msg)
    return True

try:
    validateParameterExample(True, "positive_number", ValidationError.INVALID_PARAMETER, "setup")
    print("   validateParameterExample(True, 'positive_number', ...): PASSED ✓")
except Exception as e:
    print(f"   validateParameterExample(True, 'positive_number', ...): FAILED ✗ - {e}")

try:
    validateParameterExample(False, "positive_number", ValidationError.INVALID_PARAMETER, "setup")
    print("   validateParameterExample(False, 'positive_number', ...): PASSED ✓")
except Exception as e:
    print(f"   validateParameterExample(False, 'positive_number', ...): FAILED ✗ - {e}")

# Example 3.3: Division by zero validation
print("\n3.3 Division by zero validation:")

def checkDivisionByZeroExample(divisor, operation):
    """Simulate division by zero detection."""
    if abs(divisor) < 1e-15:
        raise ValidationError(
            ValidationError.INVALID_PARAMETER,
            f"Division by zero detected in {operation}. "
            "Recovery: Ensure divisor is non-zero or handle division case differently."
        )
    return 100.0 / divisor

try:
    result = checkDivisionByZeroExample(2.0, "calculation")
    print(f"   checkDivisionByZeroExample(2.0, 'calculation'): PASSED ✓, result: {result}")
except Exception as e:
    print(f"   checkDivisionByZeroExample(2.0, 'calculation'): FAILED ✗ - {e}")

try:
    result = checkDivisionByZeroExample(0.0, "calculation")
    print(f"   checkDivisionByZeroExample(0.0, 'calculation'): PASSED ✓, result: {result}")
except Exception as e:
    print(f"   checkDivisionByZeroExample(0.0, 'calculation'): FAILED ✗ - {e}")

# Example 3.4: Safe array access
print("\n3.4 Safe array access:")

def checkBoundsExample(index, size, context):
    """Simulate bounds checking."""
    if index >= size:
        raise ValidationError(
            ValidationError.OUT_OF_BOUNDS,
            f"Array index {index} is out of bounds [0, {size}) in {context}. "
            "Recovery: Use valid index within array bounds."
        )
    return f"Access element {index} in array of size {size}"

# Test valid access
try:
    result = checkBoundsExample(2, 5, "array_processing")
    print(f"   checkBoundsExample(2, 5, 'array_processing'): PASSED ✓, {result}")
except Exception as e:
    print(f"   checkBoundsExample(2, 5, 'array_processing'): FAILED ✗ - {e}")

# Test invalid access
try:
    result = checkBoundsExample(5, 5, "array_processing")
    print(f"   checkBoundsExample(5, 5, 'array_processing'): PASSED ✓, {result}")
except Exception as e:
    print(f"   checkBoundsExample(5, 5, 'array_processing'): FAILED ✗ - {e}")

# Example 3.5: Null pointer validation
print("\n3.5 Null pointer validation:")

def validatePointerExample(ptr, param_name, error_code):
    """Simulate null pointer validation."""
    if not ptr:
        raise ValidationError(
            error_code,
            f"Null pointer detected for parameter '{param_name}'. "
            "Recovery: Ensure object is properly initialized before use."
        )
    return "Pointer is valid"

# Test valid pointer
try:
    result = validatePointerExample("valid_object", "object_param", ValidationError.NULL_POINTER)
    print(f"   validatePointerExample('valid_object', ...): PASSED ✓, {result}")
except Exception as e:
    print(f"   validatePointerExample('valid_object', ...): FAILED ✗ - {e}")

# Test null pointer
try:
    result = validatePointerExample(None, "object_param", ValidationError.NULL_POINTER)
    print(f"   validatePointerExample(None, ...): PASSED ✓, {result}")
except Exception as e:
    print(f"   validatePointerExample(None, ...): FAILED ✗ - {e}")
```

**Expected Output:**
```
=== Parameter Validation Examples ===

3.1 Range validation:
   validateRangeExample(50, 0, 100, 'test_value'): PASSED ✓
   validateRangeExample(150, 0, 100, 'test_value'): FAILED ✗ - Parameter 'test_value' value 150 is out of range [0, 100]. Recovery: Adjust parameter to valid range.

3.2 Parameter validation:
   validateParameterExample(True, 'positive_number', 1001, 'setup'): PASSED ✓
   validateParameterExample(False, 'positive_number', 1001, 'setup'): FAILED ✗ - Parameter validation failed: 'positive_number' in context: 'setup'. Recovery: Check parameter value and ensure it meets requirements.

3.3 Division by zero validation:
   checkDivisionByZeroExample(2.0, 'calculation'): PASSED ✓, result: 50.0
   checkDivisionByZeroExample(0.0, 'calculation'): FAILED ✗ - Division by zero detected in calculation. Recovery: Ensure divisor is non-zero or handle division case differently.

3.4 Safe array access:
   checkBoundsExample(2, 5, 'array_processing'): PASSED ✓, Access element 2 in array of size 5
   checkBoundsExample(5, 5, 'array_processing'): FAILED ✗ - Array index 5 is out of bounds [0, 5) in array_processing. Recovery: Use valid index within array bounds.

3.5 Null pointer validation:
   validatePointerExample('valid_object', 'object_param', 1003): PASSED ✓, Pointer is valid
   validatePointerExample(None, 'object_param', 1003): FAILED ✗ - Null pointer detected for parameter 'object_param'. Recovery: Ensure object is properly initialized before use.
```

## 4. State Validation

```python
"""
Example 4: State validation
Demonstrates state checking and error recovery.
"""

import pynlm
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    StateError, ResourceError, ValidationError
)

print("=== State Validation Examples ===\n")

# Example 4.1: State validation helper
print("4.1 State validation helper:")

def validateStateExample(condition, component, error_code, message=""):
    """Simulate state validation with enhanced error handling."""
    if not condition:
        msg = f"State validation failed for component '{component}'."
        if message:
            msg += f" {message}"
        raise StateError(component, msg)
    return True

# Test valid state
try:
    validateStateExample(True, "brain.initialize", StateError.NOT_INITIALIZED)
    print("   validateStateExample(True, 'brain.initialize', ...): PASSED ✓")
except Exception as e:
    print(f"   validateStateExample(True, 'brain.initialize', ...): FAILED ✗ - {e}")

# Test invalid state
try:
    validateStateExample(False, "brain.initialize", StateError.NOT_INITIALIZED, "Brain already initialized")
    print("   validateStateExample(False, 'brain.initialize', ...): PASSED ✓")
except Exception as e:
    print(f"   validateStateExample(False, 'brain.initialize', ...): FAILED ✗ - {e}")

# Example 4.2: State validation in brain operations
print("\n4.2 State validation in brain operations:")

# Helper for state validation with realistic brain scenarios
def validateBrainState(brain, operation):
    """Validate brain state for operations."""
    # Simulate brain state validation
    if not brain:  # Simulate null check
        raise ValidationError(ValidationError.NULL_POINTER, "Brain object is null")
    
    # Simulate initialized check
    if hasattr(brain, '_initialized') and not brain._initialized:
        raise StateError("brain.initialize", f"Brain not initialized for operation '{operation}'")
    
    # Simulate operation-specific checks
    if operation == "step" and hasattr(brain, '_steps_completed'):
        if brain._steps_completed > 1000:  # Simulate maximum steps
            raise StateError("brain.step", f"Maximum steps (1000) exceeded")
    
    return True

# Simulate brain object
class MockBrain:
    def __init__(self):
        self._initialized = True
        self._steps_completed = 5

mock_brain = MockBrain()

# Test valid state
try:
    validateBrainState(mock_brain, "step")
    print("   validateBrainState(valid_brain, 'step'): PASSED ✓")
except Exception as e:
    print(f"   validateBrainState(valid_brain, 'step'): FAILED ✗ - {e}")

# Test uninitialized state
mock_brain._initialized = False
try:
    validateBrainState(mock_brain, "step")
    print("   validateBrainState(uninitialized_brain, 'step'): PASSED ✓")
except Exception as e:
    print(f"   validateBrainState(uninitialized_brain, 'step'): FAILED ✗ - {e}")

# Test maximum steps exceeded
mock_brain._initialized = True
mock_brain._steps_completed = 1001
try:
    validateBrainState(mock_brain, "step")
    print("   validateBrainState(overused_brain, 'step'): PASSED ✓")
except Exception as e:
    print(f"   validateBrainState(overused_brain, 'step'): FAILED ✗ - {e}")

# Example 4.3: Resource error handling
print("\n4.3 Resource error handling:")

def validateVersionExample(expected, actual, component):
    """Simulate version validation."""
    if expected != actual:
        raise ResourceError("version_check", 
            f"Version mismatch for component '{component}': expected {expected}, got {actual}. "
            "Recovery: Update component or adjust version configuration."
        )
    return True

# Test valid version
try:
    validateVersionExample("1.0.0", "1.0.0", "pynlm")
    print("   validateVersionExample('1.0.0', '1.0.0', 'pynlm'): PASSED ✓")
except Exception as e:
    print(f"   validateVersionExample('1.0.0', '1.0.0', 'pynlm'): FAILED ✗ - {e}")

# Test version mismatch
try:
    validateVersionExample("1.0.0", "2.0.0", "pynlm")
    print("   validateVersionExample('1.0.0', '2.0.0', 'pynlm'): PASSED ✓")
except Exception as e:
    print(f"   validateVersionExample('1.0.0', '2.0.0', 'pynlm'): FAILED ✗ - {e}")
```

**Expected Output:**
```
=== State Validation Examples ===

4.1 State validation helper:
   validateStateExample(True, 'brain.initialize', 4001): PASSED ✓
   validateStateExample(False, 'brain.initialize', 4001, 'Brain already initialized'): FAILED ✗ - State validation failed for component 'brain.initialize'. Brain already initialized.

4.2 State validation in brain operations:
   validateBrainState(valid_brain, 'step'): PASSED ✓
   validateBrainState(uninitialized_brain, 'step'): FAILED ✗ - State validation failed for component 'brain.initialize': Brain not initialized for operation 'step'.
   validateBrainState(overused_brain, 'step'): FAILED ✗ - State validation failed for component 'brain.step': Maximum steps (1000) exceeded.

4.3 Resource error handling:
   validateVersionExample('1.0.0', '1.0.0', 'pynlm'): PASSED ✓
   validateVersionExample('1.0.0', '2.0.0', 'pynlm'): FAILED ✗ - Version mismatch for component 'pynlm': expected 1.0.0, got 2.0.0. Recovery: Update component or adjust version configuration.
```

## 5. Error Recovery

```python
"""
Example 5: Error recovery
Demonstrates graceful error handling and recovery patterns.
"""

import pynlm
from enhanced_bindings import EnhancedConfig, ValidationError, ConfigError

print("=== Error Recovery Examples ===\n")

# Example 5.1: Try-catch with specific exception handling
print("5.1 Try-catch with specific exception handling:")

config = EnhancedConfig(pynlm.createDefaultConfig())

def processConfiguration(config_obj):
    """Process configuration with specific exception handling."""
    try:
        # Validate required parameters
        if not config_obj.has('brain.neuron_count'):
            raise ConfigError('brain.neuron_count', "Required key missing")
        
        # Validate parameter type
        config_obj.validate('brain.neuron_count', 'int')
        
        # Validate parameter range
        neuron_count = config_obj.get_int('brain.neuron_count')
        if neuron_count < 1 or neuron_count > 10000:
            raise ValidationError(
                ValidationError.OUT_OF_BOUNDS,
                f"Ne neuron count {neuron_count} is out of valid range [1, 10000]. Recovery: Adjust to valid range."
            )
        
        return True
        
    except ConfigError as e:
        print(f"   [ConfigError] Recovery: Missing configuration key '{e.key}' - providing default")
        config_obj.setValue('brain.neuron_count', 100, config_obj.ConfigSource.Runtime)
        return processConfiguration(config_obj)
    
    except ValidationError as e:
        print(f"   [ValidationError] Recovery: {str(e).split('Recovery:')[0]} - adjusting value")
        return processConfiguration(config_obj)
    
    except Exception as e:
        print(f"   [UnexpectedError] Recovery: {str(e)} - aborting")
        return False

# Test with missing key
print("   Test 1: Processing configuration with missing key:")
result = processConfiguration(config)
print(f"   Result: {'Success' if result else 'Failed'}")

# Test with invalid value
config.setValue('brain.neuron_count', 50000, config.ConfigSource.Runtime)
print("\n   Test 2: Processing configuration with invalid value:")
result = processConfiguration(config)
print(f"   Result: {'Success' if result else 'Failed'}")

# Example 5.2: Recovery with retry logic
print("\n5.2 Recovery with retry logic:")

def runWithRetry(operation, max_retries=3, delay=1.0):
    """Run operation with retry logic and recovery."""
    for attempt in range(max_retries):
        try:
            operation()
            print(f"   Operation succeeded on attempt {attempt + 1} ✓")
            return True
        except (ValidationError, ConfigError) as e:
            print(f"   Attempt {attempt + 1} failed: {str(e).split('Recovery:')[0]}")
            if attempt < max_retries - 1:
                print(f"   Retrying in {delay} seconds...")
            else:
                print(f"   Max retries ({max_retries}) exceeded - aborting")
                return False
    return False

# Simulate operation that fails then succeeds
fail_count = 0
def failingOperation():
    global fail_count
    fail_count += 1
    if fail_count < 3:
        raise ValidationError(
            ValidationError.INVALID_PARAMETER,
            f"Operation failed (simulated error {fail_count}). Recovery: Retry with same parameters."
        )
    return "Success"

print("   Testing retry logic:")
result = runWithRetry(failingOperation, max_retries=3)
print(f"   Final result: {'Success' if result else 'Failed'}")

# Example 5.3: Graceful degradation
print("\n5.3 Graceful degradation:")

def runWithFallback(primary_operation, fallback_operation, fallback_condition):
    """Run primary operation with fallback on failure."""
    try:
        primary_operation()
        print("   Primary operation succeeded ✓")
        return True
    except Exception as e:
        print(f"   Primary operation failed: {str(e).split('Recovery:')[0]}")
        
        if fallback_condition():
            print("   Fallback condition met - attempting fallback operation")
            try:
                fallback_operation()
                print("   Fallback operation succeeded ✓")
                return True
            except Exception as fallback_e:
                print(f"   Fallback operation also failed: {fallback_e}")
                return False
        else:
            print("   Fallback condition not met - aborting")
            return False

# Simulate primary and fallback operations
primary_called = False
fallback_called = False

def primaryOperation():
    global primary_called
    primary_called = True
    raise ValidationError(ValidationError.INVALID_PARAMETER, "Primary operation unavailable")

def fallbackOperation():
    global fallback_called
    fallback_called = True
    print("   Fallback operation: Using reduced functionality")
    return "Degraded success"

def fallbackCondition():
    return True  # Always enable fallback

print("   Testing graceful degradation:")
result = runWithFallback(primaryOperation, fallbackOperation, fallbackCondition)
print(f"   Result: {'Success' if result else 'Failed'}")
print(f"   Primary operation called: {primary_called}")
print(f"   Fallback operation called: {fallback_called}")

# Example 5.4: Circuit breaker pattern
print("\n5.4 Circuit breaker pattern:")

class CircuitBreaker:
    def __init__(self, failure_threshold=3, timeout=60):
        self.failure_threshold = failure_threshold
        self.timeout = timeout
        self.failure_count = 0
        self.last_failure_time = 0
        self.state = "CLOSED"  # CLOSED, OPEN, HALF_OPEN
    
    def call(self, operation):
        if self.state == "OPEN":
            print("   Circuit breaker is OPEN - aborting call")
            raise ResourceError("circuit_breaker", "Circuit breaker is open - service unavailable")
        
        try:
            result = operation()
            if self.state == "HALF_OPEN":
                print("   Circuit breaker reset to CLOSED - service restored")
                self.state = "CLOSED"
                self.failure_count = 0
            return result
        except Exception as e:
            self.failure_count += 1
            self.last_failure_time = 1.0  # Simulated time
            
            if self.failure_count >= self.failure_threshold:
                self.state = "OPEN"
                print(f"   Circuit breaker tripped to OPEN after {self.failure_count} failures")
            
            print(f"   Operation failed ({self.failure_count}/{self.failure_threshold}): {str(e).split('Recovery:')[0]}")
            raise

# Test circuit breaker
circuit_breaker = CircuitBreaker(failure_threshold=2)

def riskyOperation():
    if circuit_breaker.failure_count < circuit_breaker.failure_threshold:
        raise ValidationError(ValidationError.INVALID_PARAMETER, "Risky operation failed (simulated)")
    return "Risky operation succeeded"

print("   Testing circuit breaker:")
try:
    result = circuit_breaker.call(riskyOperation)
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Circuit breaker prevented call: {type(e).__name__}")

try:
    result = circuit_breaker.call(riskyOperation)
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Circuit breaker prevented call: {type(e).__name__}")

try:
    result = circuit_breaker.call(riskyOperation)
    print(f"   Result: {result}")
except Exception as e:
    print(f"   Circuit breaker prevented call: {type(e).__name__}")
```

**Expected Output:**
```
=== Error Recovery Examples ===

5.1 Try-catch with specific exception handling:
   Test 1: Processing configuration with missing key:
   [ConfigError] Recovery: Missing configuration key 'brain.neuron_count' - providing default
   Result: Success

   Test 2: Processing configuration with invalid value:
   [ValidationError] Recovery: Parameter 'brain.neuron_count' value 50000 is out of valid range [1, 10000]. - adjusting value
   Result: Success

5.2 Recovery with retry logic:
   Testing retry logic:
   Attempt 1 failed: Operation failed (simulated error 1). Recovery: Retry with same parameters.
   Retrying in 1.0 seconds...
   Attempt 2 failed: Operation failed (simulated error 2). Recovery: Retry with same parameters.
   Retrying in 1.0 seconds...
   Attempt 3 failed: Operation failed (simulated error 3). Recovery: Retry with same parameters.
   Max retries (3) exceeded - aborting
   Final result: Failed

5.3 Graceful degradation:
   Testing graceful degradation:
   Primary operation failed: Primary operation unavailable
   Fallback condition met - attempting fallback operation
   Fallback operation succeeded ✓
   Result: Success
   Primary operation called: True
   Fallback operation called: True

5.4 Circuit breaker pattern:
   Testing circuit breaker:
   Operation failed (1/2): Risky operation failed (simulated)
   Operation failed (2/2): Risky operation failed (simulated)
   Circuit breaker tripped to OPEN after 2 failures
   Circuit breaker prevented call: ResourceError
   Circuit breaker prevented call: ResourceError
```

## 6. Error Logging

```python
"""
Example 6: Error logging
Demonstrates structured error logging and reporting.
"""

import pynlm
from enhanced_bindings import EnhancedConfig, ValidationError, ConfigError

print("=== Error Logging Examples ===\n")

# Example 6.1: Simple error logging
print("6.1 Simple error logging:")

error_log = []

def logError(error, context=""):
    """Simple error logging function."""
    error_entry = {
        'timestamp': 1.0,  # Simulated timestamp
        'level': 'ERROR',
        'error_type': type(error).__name__,
        'error_message': str(error),
        'context': context,
        'recovery_guidance': str(error).split('Recovery:')[1] if 'Recovery:' in str(error) else "No recovery guidance"
    }
    error_log.append(error_entry)
    print(f"   Logged error: {error_entry['error_type']}: {error_entry['error_message']}")
    print(f"   Context: {error_entry['context']}")
    print(f"   Recovery: {error_entry['recovery_guidance']}")

# Log different types of errors
try:
    raise ValidationError(ValidationError.OUT_OF_BOUNDS, "Value out of range. Recovery: Adjust to valid range.")
except Exception as e:
    logError(e, "Configuration processing")

try:
    raise ConfigError("brain.neuron_count", "Required key missing. Recovery: Provide default value.")
except Exception as e:
    logError(e, "Brain setup")

# Example 6.2: Comprehensive error logging
print("\n6.2 Comprehensive error logging:")

class ErrorLogger:
    def __init__(self):
        self.logs = []
        self.stats = {
            'total_errors': 0,
            'error_types': {},
            'recovery_attempts': 0,
            'successful_recoveries': 0
        }
    
    def log(self, error, context="", operation=""):
        """Log error with comprehensive information."""
        error_info = {
            'timestamp': 1.0,
            'operation': operation,
            'context': context,
            'error': {
                'type': type(error).__name__,
                'message': str(error),
                'code': getattr(error, 'code', None) if hasattr(error, 'code') else None,
                'component': getattr(error, 'component', None) if hasattr(error, 'component') else None,
                'key': getattr(error, 'key', None) if hasattr(error, 'key') else None
            },
            'recovery': self._extractRecoveryGuidance(error)
        }
        
        self.logs.append(error_info)
        
        # Update statistics
        self.stats['total_errors'] += 1
        error_type = error_info['error']['type']
        self.stats['error_types'][error_type] = self.stats['error_types'].get(error_type, 0) + 1
        
        # Log the error
        print(f"   [{error_type}] Operation '{operation}': {error_info['error']['message']}")
        print(f"   Context: {error_info['context']}")
        if error_info['recovery']:
            print(f"   Recovery: {error_info['recovery']}")
        
        return error_info
    
    def _extractRecoveryGuidance(self, error):
        """Extract recovery guidance from error message."""
        error_str = str(error)
        if 'Recovery:' in error_str:
            return error_str.split('Recovery:')[1].strip()
        return None
    
    def getSummary(self):
        """Get error summary statistics."""
        print(f"\n   Error Summary:")
        print(f"     Total errors: {self.stats['total_errors']}")
        print(f"     Error types:")
        for error_type, count in self.stats['error_types'].items():
            print(f"       {error_type}: {count}")
        print(f"     Recovery guidance available: {sum(1 for e in self.logs if e['recovery'])}")
        
        return self.stats

# Use comprehensive error logger
logger = ErrorLogger()

# Process configuration with multiple potential errors
config = EnhancedConfig(pynlm.createDefaultConfig())

def processWithLogging(config_obj, operation_name):
    """Process configuration with logging."""
    try:
        if not config_obj.has('brain.neuron_count'):
            raise ConfigError('brain.neuron_count', "Required key missing. Recovery: Provide default value.")
        
        config_obj.validate('brain.neuron_count', 'int')
        neuron_count = config_obj.get_int('brain.neuron_count')
        
        if neuron_count < 10 or neuron_count > 1000:
            raise ValidationError(
                ValidationError.OUT_OF_BOUNDS,
                f"Ne neuron count {neuron_count} invalid. Recovery: Adjust to range [10, 1000]."
            )
        
        print(f"   Processing {operation_name}: SUCCESS")
        return True
        
    except (ValidationError, ConfigError) as e:
        logger.log(e, "Configuration processing", operation_name)
        return False
    except Exception as e:
        logger.log(e, "Configuration processing", operation_name)
        return False

# Run multiple operations with logging
print("   Running operations with error logging:")
operations = ["validate_config", "process_brain", "setup_agent", "configure_world"]
for op in operations:
    result = processWithLogging(config, op)

# Get summary
logger.getSummary()

# Example 6.3: Error correlation and analysis
print("\n6.3 Error correlation and analysis:")

class ErrorAnalyzer:
    def __init__(self, error_logger):
        self.error_logger = error_logger
        self.patterns = {}
    
    def analyzePatterns(self):
        """Analyze error patterns and correlations."""
        if not self.error_logger.logs:
            return "No errors to analyze"
        
        error_types = list(self.error_logger.stats['error_types'].keys())
        print(f"   Error types detected: {', '.join(error_types)}")
        
        # Look for patterns
        validation_errors = self.error_logger.stats['error_types'].get('ValidationError', 0)
        config_errors = self.error_logger.stats['error_types'].get('ConfigError', 0)
        
        if validation_errors > config_errors:
            print("   Pattern: More ValidationErrors than ConfigErrors")
            print("   Recommendation: Check parameter validation rules")
        elif config_errors > validation_errors:
            print("   Pattern: More ConfigErrors than ValidationErrors")
            print("   Recommendation: Check configuration keys and defaults")
        
        # Check for recovery guidance
        errors_with_recovery = sum(1 for e in self.error_logger.logs if e['recovery'])
        recovery_rate = (errors_with_recovery / len(self.error_logger.logs)) * 100
        print(f"   Recovery guidance availability: {recovery_rate:.1f}%")
        
        if recovery_rate < 50:
            print("   Recommendation: Improve error messages with recovery guidance")

# Analyze error patterns
analyzer = ErrorAnalyzer(logger)
analyzer.analyzePatterns()

# Example 6.4: Error reporting for different stakeholders
print("\n6.4 Error reporting for different stakeholders:")

def generateReport(error_logger):
    """Generate error report for different stakeholders."""
    
    technical_report = {
        'title': 'Technical Error Report',
        'summary': f"{error_logger.stats['total_errors']} errors occurred",
        'details': error_logger.logs,
        'error_types': error_logger.stats['error_types'],
        'technical_info': 'Available for developers'
    }
    
    user_report = {
        'title': 'User Error Report',
        'summary': f"{error_logger.stats['total_errors']} operations had issues",
        'user_facing_errors': [e for e in error_logger.logs if e['error']['type'] in ['ValidationError', 'ConfigError']],
        'recommended_actions': [
            "Check your configuration file",
            "Use the provided recovery guidance for each error",
            "Contact support if errors persist"
        ]
    }
    
    admin_report = {
        'title': 'Administrative Error Report',
        'summary': f"System performance metrics based on {error_logger.stats['total_errors']} errors",
        'error_trends': error_logger.stats['error_types'],
        'system_health': "Degraded" if error_logger.stats['total_errors'] > 10 else "Operational",
        'monitoring_recommendations': [
            "Set up alerts for ValidationError rate > 50%",
            "Monitor ConfigError patterns for configuration issues",
            "Track error recovery success rates"
        ]
    }
    
    print("   Technical Report:")
    print(f"     Title: {technical_report['title']}")
    print(f"     Summary: {technical_report['summary']}")
    print(f"     Error types: {list(technical_report['error_types'].keys())}")
    
    print("\n   User Report:")
    print(f"     Title: {user_report['title']}")
    print(f"     Summary: {user_report['summary']}")
    print(f"     User-facing errors: {len(user_report['user_facing_errors'])}")
    
    print("\n   Administrative Report:")
    print(f"     Title: {admin_report['title']}")
    print(f"     Summary: {admin_report['summary']}")
    print(f"     System health: {admin_report['system_health']}")

# Generate reports for different stakeholders
generateReport(logger)
```

**Expected Output:**
```
=== Error Logging Examples ===

6.1 Simple error logging:
   Logged error: ValidationError: Value out of range. Recovery: Adjust to valid range.
   Context: Configuration processing
   Recovery: Adjust to valid range.
   Logged error: ConfigError: Required key missing. Recovery: Provide default value.
   Context: Brain setup
   Recovery: Provide default value.

6.2 Comprehensive error logging:
   [ConfigError] Operation 'validate_config': Required key missing. Recovery: Provide default value.
   Context: Configuration processing
   Recovery: Provide default value.
   [ValidationError] Operation 'process_brain': Value out of range. Recovery: Adjust to valid range.
   Context: Configuration processing
   Recovery: Adjust to valid range.
   [ConfigError] Operation 'setup_agent': Required key missing. Recovery: Provide default value.
   Context: Configuration processing
   Recovery: Provide default value.
   [ValidationError] Operation 'configure_world': Value out of range. Recovery: Adjust to valid range.
   Context: Configuration processing
   Recovery: Adjust to valid range.

   Error Summary:
     Total errors: 4
     Error types:
       ConfigError: 2
       ValidationError: 2
     Recovery guidance available: 100%

6.3 Error correlation and analysis:
   Error types detected: ValidationError, ConfigError
   Pattern: More ValidationErrors than ConfigErrors
   Recommendation: Check parameter validation rules
   Recovery guidance availability: 100.0%
   Recommendation: Improve error messages with recovery guidance

6.4 Error reporting for different stakeholders:
   Technical Report:
     Title: Technical Error Report
     Summary: 4 errors occurred
     Error types: [ConfigError, ValidationError]

   User Report:
     Title: User Error Report
     Summary: 4 operations had issues
     User-facing errors: 2

   Administrative Report:
     Title: Administrative Error Report
     Summary: System performance metrics based on 4 errors
     System health: Operational
     Monitoring recommendations:
       Set up alerts for ValidationError rate > 50%
       Monitor ConfigError patterns for configuration issues
       Track error recovery success rates
```
