"""
Input validation and error recovery for NLM Python bindings.
Provides robust input validation and error recovery mechanisms.
"""

from typing import Any, Dict, List, Optional, Union, Callable, Type, get_type_hints
import traceback
import inspect

from .exceptions import (
    NLMValidationError, NLMTypeError, NLMValueError,
    NLMIndexError, NLMMemoryError, NLMResourceError
)

__all__ = [
    'InputValidator',
    'ErrorRecovery',
    'validate_brain_config',
    'validate_sensory_data',
    'validate_motor_action',
    'validate_world_parameters',
    'validate_simulation_parameters',
    'validate_memory_parameters',
    'validate_neuromodulation_parameters',
    'validate_development_parameters',
    'validate_cognition_parameters',
    'validate_plasticity_parameters',
    'get_validation_errors',
    'try_with_recovery',
    'validate_neuron_id',
    'validate_synapse_id',
    'validate_region_id',
    'validate_population_id',
    'validate_coordinates',
    'validate_timestep',
    'validate_array_size',
]

class ValidationRule:
    """
    Base class for validation rules.
    
    Defines the interface for validation rules and provides common functionality.
    """
    
    def __init__(self, name: str, description: str):
        """
        Initialize validation rule.
        
        Args:
            name: Rule name.
            description: Rule description.
        """
        self.name = name
        self.description = description
    
    def validate(self, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate a value.
        
        Args:
            value: Value to validate.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        raise NotImplementedError(f"Validation rule {self.name} not implemented")
    
    def get_name(self) -> str:
        """Get rule name."""
        return self.name
    
    def get_description(self) -> str:
        """Get rule description."""
        return self.description
class RangeValidationRule(ValidationRule):
    """
    Validation rule for numeric ranges.
    
    Ensures values are within specified range.
    """
    
    def __init__(self, name: str, description: str, min_value: float = None,
                 max_value: float = None, inclusive: bool = True):
        """
        Initialize range validation rule.
        
        Args:
            name: Rule name.
            description: Rule description.
            min_value: Minimum allowed value (None for no minimum).
            max_value: Maximum allowed value (None for no maximum).
            inclusive: Whether range is inclusive (True) or exclusive (False).
        """
        super().__init__(name, description)
        self.min_value = min_value
        self.max_value = max_value
        self.inclusive = inclusive
    
    def validate(self, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate value is within range.
        
        Args:
            value: Value to validate.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        if not isinstance(value, (int, float)):
            errors.append(f"{self.name}: Value must be numeric, got {type(value).__name__}")
            return errors
        
        if self.min_value is not None:
            if self.inclusive and value < self.min_value:
                errors.append(f"{self.name}: Value {value} must be >= {self.min_value}")
            elif not self.inclusive and value <= self.min_value:
                errors.append(f"{self.name}: Value {value} must be > {self.min_value}")
        
        if self.max_value is not None:
            if self.inclusive and value > self.max_value:
                errors.append(f"{self.name}: Value {value} must be <= {self.max_value}")
            elif not self.inclusive and value >= self.max_value:
                errors.append(f"{self.name}: Value {value} must be < {self.max_value}")
        
        return errors
class ArraySizeValidationRule(ValidationRule):
    """
    Validation rule for array sizes.
    
    Ensures array sizes are within specified bounds.
    """
    
    def __init__(self, name: str, description: str, min_size: int = None,
                 max_size: int = None, inclusive: bool = True):
        """
        Initialize array size validation rule.
        
        Args:
            name: Rule name.
            description: Rule description.
            min_size: Minimum allowed size (None for no minimum).
            max_size: Maximum allowed size (None for no maximum).
            inclusive: Whether range is inclusive (True) or exclusive (False).
        """
        super().__init__(name, description)
        self.min_size = min_size
        self.max_size = max_size
        self.inclusive = inclusive
    
    def validate(self, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate array size is within bounds.
        
        Args:
            value: Value to validate (array or size).
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        if isinstance(value, (list, tuple, dict, set)):
            size = len(value)
        elif hasattr(value, '__len__'):
            size = len(value)
        elif isinstance(value, (int, float)):
            # Value itself is treated as size
            size = value
        else:
            errors.append(f"{self.name}: Value must have size, got {type(value).__name__}")
            return errors
        
        if self.min_size is not None:
            if self.inclusive and size < self.min_size:
                errors.append(f"{self.name}: Size {size} must be >= {self.min_size}")
            elif not self.inclusive and size <= self.min_size:
                errors.append(f"{self.name}: Size {size} must be > {self.min_size}")
        
        if self.max_size is not None:
            if self.inclusive and size > self.max_size:
                errors.append(f"{self.name}: Size {size} must be <= {self.max_size}")
            elif not self.inclusive and size >= self.max_size:
                errors.append(f"{self.name}: Size {size} must be < {self.max_size}")
        
        return errors
class EnumValidationRule(ValidationRule):
    """
    Validation rule for enum values.
    
    Ensures values are within allowed set of values.
    """
    
    def __init__(self, name: str, description: str, allowed_values: List[Any]):
        """
        Initialize enum validation rule.
        
        Args:
            name: Rule name.
            description: Rule description.
            allowed_values: List of allowed values.
        """
        super().__init__(name, description)
        self.allowed_values = allowed_values
    
    def validate(self, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate value is in allowed set.
        
        Args:
            value: Value to validate.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        if value not in self.allowed_values:
            errors.append(f"{self.name}: Value {value} not in allowed values: {self.allowed_values}")
        return errors
class TypeValidationRule(ValidationRule):
    """
    Validation rule for type checking.
    
    Ensures values have expected types.
    """
    
    def __init__(self, name: str, description: str, expected_type: Type):
        """
        Initialize type validation rule.
        
        Args:
            name: Rule name.
            description: Rule description.
            expected_type: Expected type.
        """
        super().__init__(name, description)
        self.expected_type = expected_type
    
    def validate(self, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate value has expected type.
        
        Args:
            value: Value to validate.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        if not isinstance(value, self.expected_type):
            errors.append(f"{self.name}: Expected type {self.expected_type.__name__}, got {type(value).__name__}")
        return errors
class CustomValidationRule(ValidationRule):
    """
    Validation rule with custom validation function.
    
    Allows custom validation logic through user-provided function.
    """
    
    def __init__(self, name: str, description: str, validator: Callable[[Any, Dict[str, Any]], List[str]]):
        """
        Initialize custom validation rule.
        
        Args:
            name: Rule name.
            description: Rule description.
            validator: Custom validation function.
        """
        super().__init__(name, description)
        self.validator = validator
    
    def validate(self, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate using custom function.
        
        Args:
            value: Value to validate.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        return self.validator(value, context or {})
class InputValidator:
    """
    Comprehensive input validator for NLM operations.
    
    Provides validation of various types of inputs with detailed error reporting.
    """
    
    def __init__(self):
        """Initialize validator with default rules."""
        self._rules: Dict[str, ValidationRule] = {}
        self._setup_default_rules()
    
    def _setup_default_rules(self) -> None:
        """Setup default validation rules."""
        # Neuron ID validation
        self.add_rule('neuron_id', RangeValidationRule(
            'neuron_id', 'Neuron ID must be valid neuron identifier',
            min_value=0
        ))
        
        # Synapse ID validation
        self.add_rule('synapse_id', RangeValidationRule(
            'synapse_id', 'Synapse ID must be valid synapse identifier',
            min_value=0
        ))
        
        # Region ID validation
        self.add_rule('region_id', RangeValidationRule(
            'region_id', 'Region ID must be valid region identifier',
            min_value=0
        ))
        
        # Population ID validation
        self.add_rule('population_id', RangeValidationRule(
            'population_id', 'Population ID must be valid population identifier',
            min_value=0
        ))
        
        # Coordinates validation
        self.add_rule('coordinates', RangeValidationRule(
            'coordinates', 'Coordinates must be within bounds',
            min_value=0,
            max_value=1000.0
        ))
        
        # Timestep validation
        self.add_rule('timestep', RangeValidationRule(
            'timestep', 'Timestep must be positive and small',
            min_value=0.001,
            max_value=1.0
        ))
        
        # Array size validation
        self.add_rule('array_size', ArraySizeValidationRule(
            'array_size', 'Array size must be reasonable',
            min_value=0,
            max_value=1000000
        ))
        
        # Learning rate validation
        self.add_rule('learning_rate', RangeValidationRule(
            'learning_rate', 'Learning rate must be positive and small',
            min_value=0.0,
            max_value=1.0
        ))
        
        # Probability validation
        self.add_rule('probability', RangeValidationRule(
            'probability', 'Probability must be between 0 and 1',
            min_value=0.0,
            max_value=1.0,
            inclusive=True
        ))
    
    def add_rule(self, name: str, rule: ValidationRule) -> None:
        """
        Add a validation rule.
        
        Args:
            name: Rule name.
            rule: Validation rule.
        """
        self._rules[name] = rule
    
    def validate(self, name: str, value: Any, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate a value using a specific rule.
        
        Args:
            name: Rule name.
            value: Value to validate.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        if name not in self._rules:
            return [f"Unknown validation rule: {name}"]
        
        rule = self._rules[name]
        return rule.validate(value, context or {})
    
    def validate_all(self, values: Dict[str, Any], context: Dict[str, Any] = None) -> Dict[str, List[str]]:
        """
        Validate multiple values at once.
        
        Args:
            values: Dictionary of values to validate.
            context: Context information for validation.
            
        Returns:
            Dictionary mapping rule names to validation errors.
        """
        results = {}
        for name, value in values.items():
            results[name] = self.validate(name, value, context)
        return results
    
    def validate_structured(self, value: Any, struct_type: Type, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate structured data against a type structure.
        
        Args:
            value: Value to validate.
            struct_type: Expected type structure.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        
        # Get type hints for the structure
        type_hints = get_type_hints(struct_type)
        
        # Validate based on structure type
        if isinstance(value, dict):
            for field, expected_type in type_hints.items():
                if field in value:
                    field_errors = self.validate_field(field, value[field], expected_type, context)
                    errors.extend(field_errors)
        elif isinstance(value, (list, tuple)):
            for i, item in enumerate(value):
                item_errors = self.validate_field(f'[{i}]', item, type_hints.get('item', Any), context)
                errors.extend(item_errors)
        
        return errors
    
    def validate_field(self, field_name: str, value: Any, expected_type: Type, context: Dict[str, Any] = None) -> List[str]:
        """
        Validate a single field against expected type.
        
        Args:
            field_name: Name of the field.
            value: Value to validate.
            expected_type: Expected type.
            context: Context information for validation.
            
        Returns:
            List of validation errors (empty if valid).
        """
        errors = []
        
        # Type checking
        if expected_type != Any and not isinstance(value, expected_type):
            # Try to convert if possible
            if expected_type == int and isinstance(value, float):
                if value.is_integer():
                    return []
                else:
                    errors.append(f"Field '{field_name}': Cannot convert float {value} to int")
            elif expected_type == float and isinstance(value, int):
                return []
            elif expected_type == str and isinstance(value, (int, float)):
                try:
                    return []
                except (ValueError, TypeError):
                    errors.append(f"Field '{field_name}': Cannot convert {value} to str")
            else:
                errors.append(f"Field '{field_name}': Expected type {expected_type.__name__}, got {type(value).__name__}")
        
        # Additional validation based on type
        if expected_type == int:
            int_errors = self.validate('int_field', value, context)
            errors.extend(int_errors)
        elif expected_type == float:
            float_errors = self.validate('float_field', value, context)
            errors.extend(float_errors)
        
        return errors
class ErrorRecovery:
    """
    Error recovery mechanism for NLM operations.
    
    Provides recovery strategies for common error conditions and automatic
    retries with exponential backoff.
    """
    
    def __init__(self):
        """Initialize error recovery with default strategies."""
        self._recovery_strategies: Dict[Type[NLMError], Callable] = {}
        self._setup_default_strategies()
    
    def _setup_default_strategies(self) -> None:
        """Setup default recovery strategies."""
        
        def memory_recovery(self, error: NLMMemoryError, context: Dict[str, Any]) -> Any:
            """Recovery strategy for memory errors."""
            # Try to reduce memory usage
            if 'neuron_count' in context:
                original_count = context['neuron_count']
                context['neuron_count'] = max(1, original_count // 2)
                return context['neuron_count']
            return None
        
        def validation_recovery(self, error: NLMValidationError, context: Dict[str, Any]) -> Any:
            """Recovery strategy for validation errors."""
            # Try to fix validation errors
            if error.field and error.value is not None:
                # Return default value for field
                defaults = {
                    'learning_rate': 0.01,
                    'timestep': 0.01,
                    'temperature': 1.0,
                }
                return defaults.get(error.field, error.value)
            return None
        
        def index_recovery(self, error: NLMIndexError, context: Dict[str, Any]) -> Any:
            """Recovery strategy for index errors."""
            if error.index is not None:
                # Return closest valid index
                if error.max_index is not None and error.index >= error.max_index:
                    return max(0, error.max_index - 1)
                elif error.index < 0:
                    return 0
            return None
        
        def file_recovery(self, error: NLMFileError, context: Dict[str, Any]) -> Any:
            """Recovery strategy for file errors."""
            if error.filepath:
                # Try backup file if available
                backup_path = error.filepath.replace('.json', '_backup.json')
                if os.path.exists(backup_path):
                    return backup_path
            return None
        
        self._recovery_strategies[NLMMemoryError] = memory_recovery
        self._recovery_strategies[NLMValidationError] = validation_recovery
        self._recovery_strategies[NLMIndexError] = index_recovery
        self._recovery_strategies[NLMFileError] = file_recovery
    
    def add_recovery_strategy(self, error_type: Type[NLMError], strategy: Callable) -> None:
        """
        Add a recovery strategy for a specific error type.
        
        Args:
            error_type: Error type to handle.
            strategy: Recovery function.
        """
        self._recovery_strategies[error_type] = strategy
    
    def recover(self, error: NLMError, context: Dict[str, Any] = None) -> Any:
        """
        Attempt to recover from an error.
        
        Args:
            error: The error that occurred.
            context: Context information for recovery.
            
        Returns:
            Recovery action or None if no recovery possible.
        """
        error_type = type(error)
        
        # Try exact match first
        if error_type in self._recovery_strategies:
            return self._recovery_strategies[error_type](self, error, context or {})
        
        # Try base class match
        for base_type in error_type.__mro__:
            if base_type in self._recovery_strategies and base_type != NLMError:
                return self._recovery_strategies[base_type](self, error, context or {})
        
        return None
    
    def try_with_recovery(self, operation: Callable, *args, **kwargs) -> Any:
        """
        Execute an operation with automatic error recovery.
        
        Args:
            operation: Operation to execute.
            *args: Arguments to operation.
            **kwargs: Keyword arguments to operation.
            
        Returns:
            Operation result or recovered value.
        """
        max_attempts = kwargs.get('max_attempts', 3)
        attempt = 0
        last_error = None
        
        while attempt < max_attempts:
            try:
                return operation(*args, **kwargs)
            except Exception as e:
                last_error = e
                attempt += 1
                
                # Convert to NLM exception if needed
                if not isinstance(e, NLMBaseException):
                    from .exceptions import create_exception_from_cpp
                    e = create_exception_from_cpp(e)
                
                # Try to recover
                context = kwargs.get('context', {})
                recovery_action = self.recover(e, context)
                
                if recovery_action is not None:
                    # Update context with recovery action
                    if 'context' not in kwargs:
                        kwargs['context'] = {}
                    kwargs['context'].update(context)
                
                # If no recovery possible, break
                if recovery_action is None:
                    break
        
        # All attempts failed
        raise last_error if last_error else Exception("Operation failed after recovery attempts")
# Validation functions for specific parameter groups
def validate_brain_config(config: Dict[str, Any]) -> List[str]:
    """
    Validate brain configuration parameters.
    
    Args:
        config: Brain configuration dictionary.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    # Validate neuron count
    if 'neuron_count' in config:
        neuron_count = config['neuron_count']
        if not isinstance(neuron_count, int) or neuron_count <= 0:
            errors.append("Brain config: neuron_count must be positive integer")
        elif neuron_count > 1000000:
            errors.append("Brain config: neuron_count too large (>1,000,000)")
    
    # Validate region count
    if 'region_count' in config:
        region_count = config['region_count']
        if not isinstance(region_count, int) or region_count <= 0:
            errors.append("Brain config: region_count must be positive integer")
        elif region_count > 100:
            errors.append("Brain config: region_count too large (>100)")
    
    # Validate learning rate
    if 'learning_rate' in config:
        learning_rate = config['learning_rate']
        if not isinstance(learning_rate, (int, float)):
            errors.append("Brain config: learning_rate must be numeric")
        elif learning_rate < 0:
            errors.append("Brain config: learning_rate must be positive")
        elif learning_rate > 1.0:
            errors.append("Brain config: learning_rate must be <= 1.0")
    
    # Validate synaptic density
    if 'synaptic_density' in config:
        synaptic_density = config['synaptic_density']
        if not isinstance(synaptic_density, (int, float)):
            errors.append("Brain config: synaptic_density must be numeric")
        elif synaptic_density < 0:
            errors.append("Brain config: synaptic_density must be positive")
    
    return errors
def validate_sensory_data(data: List[float], expected_dimensions: int) -> List[str]:
    """
    Validate sensory input data.
    
    Args:
        data: Sensory data array.
        expected_dimensions: Expected number of dimensions.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    if not isinstance(data, list):
        errors.append("Sensory data: must be a list")
        return errors
    
    if len(data) != expected_dimensions:
        errors.append(f"Sensory data: expected {expected_dimensions} dimensions, got {len(data)}")
    
    for i, value in enumerate(data):
        if not isinstance(value, (int, float)):
            errors.append(f"Sensory data[{i}]: must be numeric, got {type(value).__name__}")
        elif value < 0 or value > 1:
            errors.append(f"Sensory data[{i}]: must be in range [0, 1], got {value}")
    
    return errors
def validate_motor_action(action_type: Any, parameters: Dict[str, Any]) -> List[str]:
    """
    Validate motor action parameters.
    
    Args:
        action_type: Action type.
        parameters: Action parameters.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    # Validate action type
    valid_actions = ['move', 'turn', 'look', 'interact', 'eat', 'drink', 'rest']
    if action_type not in valid_actions:
        errors.append(f"Motor action: invalid action_type '{action_type}', must be one of {valid_actions}")
    
    # Validate parameters based on action type
    if action_type == 'move':
        for param in ['dx', 'dy']:
            if param in parameters:
                value = parameters[param]
                if not isinstance(value, (int, float)):
                    errors.append(f"Motor action[{action_type}]: parameter '{param}' must be numeric")
    
    elif action_type == 'turn':
        if 'angle' in parameters:
            angle = parameters['angle']
            if not isinstance(angle, (int, float)):
                errors.append("Motor action[turn]: angle must be numeric")
            elif angle < -180 or angle > 180:
                errors.append("Motor action[turn]: angle must be in range [-180, 180]")
    
    return errors
def validate_world_parameters(width: float, height: float, vision_width: float, vision_height: float) -> List[str]:
    """
    Validate world parameters.
    
    Args:
        width: World width.
        height: World height.
        vision_width: Vision width.
        vision_height: Vision height.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    if not isinstance(width, (int, float)) or width <= 0:
        errors.append("World: width must be positive number")
    
    if not isinstance(height, (int, float)) or height <= 0:
        errors.append("World: height must be positive number")
    
    if vision_width <= 0 or vision_width > width:
        errors.append("World: vision_width must be positive and <= world width")
    
    if vision_height <= 0 or vision_height > height:
        errors.append("World: vision_height must be positive and <= world height")
    
    return errors
def validate_simulation_parameters(dt: float, max_steps: int, checkpoint_interval: int) -> List[str]:
    """
    Validate simulation parameters.
    
    Args:
        dt: Time step.
        max_steps: Maximum number of steps.
        checkpoint_interval: Checkpoint interval.
        
    Returns:
        List of validation errors (empty if valid).
    """
    errors = []
    
    if not isinstance(dt, (int, float)) or dt <= 0:
        errors.append("Simulation: dt must be positive number")
    
    if dt > 1.0:
        errors.append("Simulation: dt must be <= 1.0")
    
    if not isinstance(max_steps, int) or max_steps <= 0:
        errors.append("Simulation: max_steps must be positive integer")
    
    if max_steps > 1000000:
        errors.append("Simulation: max_steps too large (>1,000,000)")
    
    if not isinstance(checkpoint_interval, int) or checkpoint_interval <= 0:
        errors.append("Simulation: checkpoint_interval must be positive integer")
    
    if checkpoint_interval > max_steps:
        errors.append("Simulation: checkpoint_interval must be <= max_steps")
    
    return errors
def get_validation_errors(parameters: Dict[str, Any], validator: InputValidator = None) -> Dict[str, List[str]]:
    """
    Get validation errors for a set of parameters.
    
    Args:
        parameters: Parameters to validate.
        validator: Optional validator instance.
        
    Returns:
        Dictionary mapping parameter names to validation errors.
    """
    if validator is None:
        validator = InputValidator()
    
    return validator.validate_all(parameters)

def try_with_recovery(operation: Callable, *args, **kwargs) -> Any:
    """
    Execute an operation with automatic error recovery.
    
    Args:
        operation: Operation to execute.
        *args: Arguments to operation.
        **kwargs: Keyword arguments to operation.
        
    Returns:
        Operation result or recovered value.
    """
    recovery = ErrorRecovery()
    return recovery.try_with_recovery(operation, *args, **kwargs)