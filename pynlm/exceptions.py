"""NLM exceptions and error handling."""

from typing import List, Dict, Any, Optional, Callable


class NLMException(Exception):
    """Base exception for NLM errors."""
    
    def __init__(self, message: str, error_code: int = -1):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
    
    def __str__(self) -> str:
        return f"NLM Error [{self.error_code}]: {self.message}"


class ConfigurationError(NLMException):
    """Configuration-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=1000)


class SimulationError(NLMException):
    """Simulation-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=2000)


class IntegrationError(NLMException):
    """Integration-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=3000)


class MemoryError(NLMException):
    """Memory-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=4000)


class PlasticityError(NLMException):
    """Plasticity-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=5000)


class NeuromodulationError(NLMException):
    """Neuromodulation-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=6000)


class DevelopmentError(NLMException):
    """Development-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=7000)


class PredictionError(NLMException):
    """Prediction-related errors."""
    
    def __init__(self, message: str):
        super().__init__(message, error_code=8000)


# Exception factory functions
def create_nlm_error(error_type: str, message: str, **kwargs) -> NLMException:
    """
    Create appropriate NLM exception based on error type.
    
    Args:
        error_type: Type of error (configuration, simulation, integration, etc.)
        message: Error message
        **kwargs: Additional error parameters
        
    Returns:
        Appropriate NLM exception
    """
    error_type = error_type.lower()
    
    error_mapping = {
        'configuration': ConfigurationError,
        'simulation': SimulationError,
        'integration': IntegrationError,
        'memory': MemoryError,
        'plasticity': PlasticityError,
        'neuromodulation': NeuromodulationError,
        'development': DevelopmentError,
        'prediction': PredictionError,
        'default': NLMException
    }
    
    exception_class = error_mapping.get(error_type, error_mapping['default'])
    return exception_class(message)


def handle_exception(exc: Exception) -> str:
    """
    Handle exception and return formatted error message.
    
    Args:
        exc: Exception to handle
        
    Returns:
        Formatted error message
    """
    if isinstance(exc, NLMException):
        return str(exc)
    else:
        return f"Unexpected error: {exc}"


def check_error_condition(condition: bool, error_message: str, 
                         error_type: str = "default", 
                         raise_exception: bool = True) -> bool:
    """
    Check error condition and optionally raise exception.
    
    Args:
        condition: Condition to check
        error_message: Error message if condition is False
        error_type: Type of error
        raise_exception: Whether to raise exception
        
    Returns:
        True if condition is True, False otherwise
    """
    if not condition:
        if raise_exception:
            raise create_nlm_error(error_type, error_message)
        else:
            return False
    return True


def validate_configuration(config_dict: Dict[str, Any], 
                          schema: Optional[Dict[str, Any]] = None) -> List[str]:
    """
    Validate configuration dictionary.
    
    Args:
        config_dict: Configuration to validate
        schema: Schema for validation
        
    Returns:
        List of validation errors (empty if valid)
    """
    """
    Validate configuration dictionary.
    
    Args:
        config_dict: Configuration to validate
        schema: Schema for validation
        
    Returns:
        List of validation errors (empty if valid)
    """
    errors = []
    
    # Basic validation
    if not isinstance(config_dict, dict):
        errors.append("Configuration must be a dictionary")
        return errors
    
    # Validate against schema if provided
    if schema:
        for key, expected_type in schema.items():
            if key not in config_dict:
                errors.append(f"Missing required key: {key}")
            else:
                value = config_dict[key]
                if not isinstance(value, expected_type):
                    errors.append(f"Invalid type for {key}: expected {expected_type}, got {type(value)}")
    
    return errors


# Error context management
class ErrorContext:
    """Context for error handling and reporting."""
    
    def __init__(self, component: str = "", operation: str = ""):
        self.component = component
        self.operation = operation
        self.errors = []
    
    def add_error(self, error: NLMException):
        """Add error to context."""
        self.errors.append(error)
    
    def has_errors(self) -> bool:
        """Check if context has errors."""
        return len(self.errors) > 0
    
    def get_errors(self) -> List[NLMException]:
        """Get all errors."""
        return self.errors.copy()
    
    def clear(self):
        """Clear all errors."""
        self.errors.clear()
    
    def report(self) -> str:
        """Generate error report."""
        if not self.errors:
            return "No errors"
        
        report = f"Error report for {self.component} - {self.operation}:\n"
        
        for i, error in enumerate(self.errors, 1):
            report += f"  {i}. {error}\n"
        
        return report.strip()


# Global error handler
_global_error_handler = None


def set_global_error_handler(handler: callable):
    """
    Set global error handler.
    
    Args:
        handler: Error handler function
    """
    global _global_error_handler
    _global_error_handler = handler


def get_global_error_handler() -> callable:
    """Get global error handler."""
    return _global_error_handler


def handle_error(exc: Exception, context: Optional[ErrorContext] = None) -> Any:
    """
    Handle error with global handler if available.
    
    Args:
        exc: Exception to handle
        context: Error context
        
    Returns:
        Handler result or None
    """
    if _global_error_handler:
        return _global_error_handler(exc, context)
    
    if context:
        context.add_error(exc if isinstance(exc, NLMException) else NLMException(str(exc)))
    
    return None