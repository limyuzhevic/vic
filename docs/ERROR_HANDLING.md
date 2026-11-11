# Enhanced Error Handling and Validation for NLM

## Overview

This module provides comprehensive error handling and validation for the NLM framework,
with detailed error reporting, validation rules, and recovery mechanisms.

## Error Types

### 1. Configuration Errors
Errors related to configuration loading, parsing, and validation.

### 2. Simulation Errors
Errors that occur during neural simulation execution.

### 3. Memory Errors
Errors related to memory allocation, access, and management.

### 4. Plasticity Errors
Errors in plasticity rule calculations and application.

### 5. Development Errors
Errors related to developmental processes.

### 6. Integration Errors
Errors in system integration and coordination.

### 7. Resource Errors
Errors related to computational resources and performance.

## Error Classes

### BaseError
Base class for all NLM errors.

```python
class BaseError(Exception):
    def __init__(self, message, error_type, severity="medium", suggestions=None):
        super().__init__(message)
        self.message = message
        self.error_type = error_type
        self.severity = severity
        self.suggestions = suggestions or []
        
    def to_dict(self):
        return {
            "message": self.message,
            "error_type": self.error_type,
            "severity": self.severity,
            "suggestions": self.suggestions,
            "timestamp": datetime.now().isoformat()
        }
```

### ConfigurationError
Errors in configuration handling.

```python
class ConfigurationError(BaseError):
    def __init__(self, message, key=None, value=None, suggestions=None):
        super().__init__(message, "configuration", "medium", suggestions)
        self.key = key
        self.value = value
        
    def to_dict(self):
        result = super().to_dict()
        result.update({
            "key": self.key,
            "value": self.value
        })
        return result
```

### SimulationError
Errors during neural simulation.

```python
class SimulationError(BaseError):
    def __init__(self, message, step=None, neuron_id=None, suggestions=None):
        super().__init__(message, "simulation", "high", suggestions)
        self.step = step
        self.neuron_id = neuron_id
```

### MemoryError
Errors related to memory systems.

```python
class MemoryError(BaseError):
    def __init__(self, message, memory_type=None, capacity=None, suggestions=None):
        super().__init__(message, "memory", "high", suggestions)
        self.memory_type = memory_type
        self.capacity = capacity
```

## Validation System

### ValidationRule
Base class for validation rules.

```python
class ValidationRule:
    def __init__(self, name, description, error_message):
        self.name = name
        self.description = description
        self.error_message = error_message
        
    def validate(self, value, context=None):
        raise NotImplementedError
        
    def is_applicable(self, key, value, context=None):
        return True
```

### ConfigurationValidator
Validates configuration against a set of rules.

```python
class ConfigurationValidator:
    def __init__(self):
        self.rules = []
        self.error_messages = {}
        
    def add_rule(self, rule):
        self.rules.append(rule)
        
    def validate(self, config):
        errors = []
        warnings = []
        
        for rule in self.rules:
            for key in config.getKeys():
                if rule.is_applicable(key, config.get(key)):
                    result = rule.validate(config.get(key), config)
                    if result is not None:
                        if result.error_type == "error":
                            errors.append(result)
                        else:
                            warnings.append(result)
                            
        return errors, warnings
```

### ValidationRules
Common validation rules for NLM configuration.

```python
class ValidationRules:
    # Required parameters
    @staticmethod
    def required_parameters():
        rule = ValidationRule(
            "required_parameters",
            "Check that all required configuration parameters are present",
            "Missing required configuration parameter: {parameter}"
        )
        
        def validate(value, context):
            required_params = ["random_seed", "simulation_timestep", "neuron_count", "region_count"]
            for param in required_params:
                if not context.has(param):
                    return ValidationResult(
                        "error",
                        rule.error_message.format(parameter=param),
                        "configuration",
                        param
                    )
            return None
            
        rule.validate = validate
        return rule
        
    # Range validation
    @staticmethod
    def range_validation(min_value, max_value, param_name):
        rule = ValidationRule(
            f"range_{param_name}",
            f"Validate that {param_name} is within valid range",
            f"Parameter {param_name} must be between {min_value} and {max_value}, got {{value}}"
        )
        
        def validate(value, context):
            if value < min_value or value > max_value:
                return ValidationResult(
                    "error",
                    rule.error_message.format(value=value),
                    "configuration",
                    param_name
                )
            return None
            
        rule.validate = validate
        return rule
        
    # Consistency validation
    @staticmethod
    def consistency_validation():
        rule = ValidationRule(
            "consistency",
            "Check that configuration parameters are consistent",
            "Configuration inconsistency: {message}"
        )
        
        def validate(value, context):
            errors = []
            
            # Check that log_to_file and log_filename are consistent
            if context.has("log_to_file") and context.has("log_filename"):
                log_to_file = context.get("log_to_file")
                log_filename = context.get("log_filename")
                if log_to_file and not log_filename:
                    errors.append("log_filename is required when log_to_file is true")
                elif not log_to_file and log_filename:
                    errors.append("log_filename should be empty when log_to_file is false")
            
            # Check that visualization parameters are consistent
            if context.has("visualization_enabled") and context.has("visualization_update_rate"):
                enabled = context.get("visualization_enabled")
                update_rate = context.get("visualization_update_rate")
                if enabled and update_rate <= 0:
                    errors.append("visualization_update_rate must be positive when visualization_enabled is true")
            
            if errors:
                for error in errors:
                    yield ValidationResult(
                        "error",
                        rule.error_message.format(message=error),
                        "configuration"
                    )
                return
            
            return None
            
        rule.validate = validate
        return rule
```

## Error Recovery System

### RecoveryStrategy
Base class for error recovery strategies.

```python
class RecoveryStrategy:
    def __init__(self, name, description):
        self.name = name
        self.description = description
        
    def can_recover(self, error):
        raise NotImplementedError
        
    def recover(self, error, context):
        raise NotImplementedError
```

### CommonRecoveryStrategies
Common error recovery strategies for NLM.

```python
class CommonRecoveryStrategies:
    # Default value recovery
    @staticmethod
    def default_value_recovery(default_values):
        strategy = RecoveryStrategy(
            "default_value",
            "Recover from missing configuration by using default values"
        )
        
        def can_recover(error):
            return error.error_type == "configuration" and error.key and not error.value
            
        def recover(error, context):
            if error.key in default_values:
                context.set(error.key, default_values[error.key])
                return True
            return False
            
        strategy.can_recover = can_recover
        strategy.recover = recover
        return strategy
        
    # Fixable error recovery
    @staticmethod
    def fixable_error_recovery():
        strategy = RecoveryStrategy(
            "fixable_error",
            "Attempt to fix recoverable configuration errors"
        )
        
        def can_recover(error):
            return error.error_type == "configuration" and error.suggestions
            
        def recover(error, context):
            if error.suggestions:
                # Apply the first suggestion
                suggestion = error.suggestions[0]
                if suggestion == "set_to_default":
                    # Set parameter to a reasonable default based on type
                    if error.key in context.get_defaults():
                        context.set(error.key, context.get_defaults()[error.key])
                        return True
                elif suggestion == "remove_duplicate":
                    # Remove duplicate parameter (simplified)
                    pass
            return False
            
        strategy.can_recover = can_recover
        strategy.recover = recover
        return strategy
        
    # Reset recovery
    @staticmethod
    def reset_recovery():
        strategy = RecoveryStrategy(
            "reset",
            "Recover from critical errors by resetting to default configuration"
        )
        
        def can_recover(error):
            return error.severity == "critical" or error.error_type == "configuration"
            
        def recover(error, context):
            context.clear()
            context.apply_defaults()
            return True
            
        strategy.can_recover = can_recover
        strategy.recover = recover
        return strategy
```

## Error Handling Utilities

### ErrorReporter
Comprehensive error reporting utilities.

```python
class ErrorReporter:
    def __init__(self, reporter_type="console"):
        self.reporter_type = reporter_type
        self.error_history = []
        
    def report(self, error):
        self.error_history.append(error)
        
        if self.reporter_type == "console":
            self._report_console(error)
        elif self.reporter_type == "json":
            self._report_json(error)
        elif self.reporter_type == "html":
            self._report_html(error)
            
    def _report_console(self, error):
        severity_emoji = {
            "low": "⚠️",
            "medium": "⚡",
            "high": "🚨",
            "critical": "💥"
        }.get(error.severity, "⚡")
        
        print(f"{severity_emoji} {error.message}")
        if error.key:
            print(f"   Key: {error.key}")
        if error.suggestions:
            print("   Suggestions:")
            for suggestion in error.suggestions:
                print(f"     - {suggestion}")
                
    def _report_json(self, error):
        import json
        report = error.to_dict()
        print(json.dumps(report, indent=2))
        
    def _report_html(self, error):
        html = f"""
        <!DOCTYPE html>
        <html>
        <head><title>Error Report</title></head>
        <body>
            <h1>Error Report</h1>
            <p><strong>Severity:</strong> {error.severity}</p>
            <p><strong>Message:</strong> {error.message}</p>
            <p><strong>Type:</strong> {error.error_type}</p>
            <p><strong>Timestamp:</strong> {error.timestamp}</p>
            <h2>Suggestions</h2>
            <ul>
        """
        for suggestion in error.suggestions:
            html += f"<li>{suggestion}</li>"
        html += """
            </ul>
        </body>
        </html>
        """
        print(html)
        
    def get_summary(self):
        summary = {
            "total_errors": len(self.error_history),
            "by_severity": {},
            "by_type": {},
            "recoverable_count": 0
        }
        
        for error in self.error_history:
            # Count by severity
            if error.severity not in summary["by_severity"]:
                summary["by_severity"][error.severity] = 0
            summary["by_severity"][error.severity] += 1
            
            # Count by type
            if error.error_type not in summary["by_type"]:
                summary["by_type"][error.error_type] = 0
            summary["by_type"][error.error_type] += 1
            
            # Count recoverable errors
            if error.suggestions:
                summary["recoverable_count"] += 1
                
        return summary
```

### ErrorContext
Context information for error handling and recovery.

```python
class ErrorContext:
    def __init__(self, config=None, simulation=None, memory=None):
        self.config = config
        self.simulation = simulation
        self.memory = memory
        self.timestamp = datetime.now()
        self.session_id = str(uuid.uuid4())
        
    def add_error(self, error, recovery_strategy=None):
        # Report error
        error_reporter.report(error)
        
        # Attempt recovery
        if recovery_strategy and recovery_strategy.can_recover(error):
            recovery_success = recovery_strategy.recover(error, self)
            if recovery_success:
                return "recovered"
        
        return "not_recovered"
        
    def get_stack_trace(self):
        # Simplified stack trace collection
        import traceback
        return traceback.format_stack()
```

## API Examples

### Basic Error Handling

```python
import nlm
from nlm.error_handling import ErrorReporter, ErrorContext, ConfigurationError

# Create error reporter
error_reporter = ErrorReporter("console")

# Create error context
context = ErrorContext()

# Simulate an error
error = ConfigurationError(
    "Configuration parameter 'neuron_count' must be positive",
    key="neuron_count",
    value=-100,
    suggestions=["Set neuron_count to a positive value", "Use a value >= 10"]
)

# Report and handle error
result = context.add_error(error)
print(f"Recovery result: {result}")

# Get error summary
print("Error summary:", error_reporter.get_summary())
```

### Configuration Validation with Recovery

```python
import nlm

# Create validator
validator = ConfigurationValidator()

# Add validation rules
validator.add_rule(ValidationRules.required_parameters())
validator.add_rule(ValidationRules.range_validation(0, 100000, "neuron_count"))
validator.add_rule(ValidationRules.range_validation(0.0, 1.0, "connection_probability"))
validator.add_rule(ValidationRules.consistency_validation())

# Validate configuration
config = nlm.create_config("research")
errors, warnings = validator.validate(config)

# Report errors
for error in errors:
    print(f"ERROR: {error.message}")
    
for warning in warnings:
    print(f"WARNING: {warning.message}")

# Auto-recovery
if errors:
    print("Attempting auto-recovery...")
    for error in errors:
        for strategy in recovery_strategies:
            if strategy.can_recover(error):
                if strategy.recover(error, config):
                    print(f"Recovered: {error.key}")
                    break
```

### Simulation Error Handling

```python
import nlm

# Create simulation with error handling
brain = nlm.create_enhanced_brain(nlm.create_config("research"))

# Run simulation with error monitoring
error_reporter = ErrorReporter("console")

def safe_simulation_step(brain, step):
    try:
        brain.step(step)
    except SimulationError as e:
        error_reporter.report(e)
        # Attempt recovery
        brain.recover_from_error(e)
        return False
    return True

# Run simulation
for step in range(1000):
    if not safe_simulation_step(brain, step):
        print(f"Error occurred at step {step}, continuing...")
```

## Integration Examples

### Complete Error Handling Workflow

```python
import nlm
from nlm.error_handling import (
    ErrorReporter, ErrorContext, ConfigurationValidator, 
    ValidationRules, CommonRecoveryStrategies
)

# Initialize error handling system
error_reporter = ErrorReporter("console")
validator = ConfigurationValidator()
context = ErrorContext()

# Setup recovery strategies
recovery_strategies = [
    CommonRecoveryStrategies.default_value_recovery({
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.15
    }),
    CommonRecoveryStrategies.fixable_error_recovery(),
    CommonRecoveryStrategies.reset_recovery()
]

# Complete validation and error handling workflow
def validate_and_fix_config(config):
    # Validate configuration
    errors, warnings = validator.validate(config)
    
    # Report warnings
    for warning in warnings:
        print(f"WARNING: {warning.message}")
    
    # Attempt to recover from errors
    if errors:
        print(f"Found {len(errors)} errors, attempting recovery...")
        
        for error in errors:
            for strategy in recovery_strategies:
                if strategy.can_recover(error):
                    success = strategy.recover(error, config)
                    if success:
                        print(f"Successfully recovered from error: {error.key}")
                        break
        
        # Re-validate after recovery
        errors, warnings = validator.validate(config)
        
        if errors:
            print("Still have errors after recovery:")
            for error in errors:
                print(f"  - {error.message}")
    
    return len(errors) == 0

# Example usage
config = nlm.create_config("research")
# Simulate adding invalid configuration
config.set("neuron_count", -100)

# Validate and fix
is_valid = validate_and_fix_config(config)
print(f"Configuration valid: {is_valid}")
```

## Performance Considerations

- Error validation adds O(n) overhead where n is the number of configuration keys
- Recovery strategies should be lightweight to avoid performance impact
- Error reporting should be optional and configurable
- Stack trace collection can be expensive for long-running simulations

## Future Enhancements

- Real-time error monitoring and alerting
- Machine learning-based error prediction
- Automated configuration optimization
- Cross-session error pattern analysis
- Error correlation with simulation performance