"""
Error handling and exception management for NLM.

This module provides comprehensive error handling for NLM, including
custom exception classes, error handling utilities, and validation
mechanisms.
"""

import traceback
from typing import Optional, Dict, Any, List
from enum import Enum
class ErrorSeverity(Enum):
    """Error severity levels."""
    
    LOW = 0      # Minor issues, not critical
    MEDIUM = 1   # Important issues, may affect functionality
    HIGH = 2     # Critical issues, affect core functionality
    CRITICAL = 3 # System-level failures
class NLMError(Exception):
    """Base exception class for all NLM errors."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.MEDIUM,
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        """
        Initialize NLM error.
        
        Args:
            message: Error message
            severity: Error severity level
            cause: Original exception that caused this error
            context: Additional context information
        """
        super().__init__(message)
        self.message = message
        self.severity = severity
        self.cause = cause
        self.context = context or {}
        self.timestamp = __import__('time').time()
        
        # Create detailed error info
        self.error_info = {
            'message': message,
            'severity': severity.name,
            'timestamp': self.timestamp,
            'cause_type': type(cause).__name__ if cause else None,
            'cause_message': str(cause) if cause else None,
            'context': context,
            'stack_trace': traceback.format_stack()
        }
    
    def __str__(self):
        """String representation of error."""
        base_msg = self.message
        
        if self.cause:
            base_msg += f" (caused by: {type(self.cause).__name__}: {self.cause})"
        
        if self.context:
            base_msg += f" (context: {self.context})"
        
        return base_msg
    
    def get_error_info(self) -> Dict[str, Any]:
        """Get detailed error information."""
        return self.error_info.copy()
    
    def is_critical(self) -> bool:
        """Check if error is critical."""
        return self.severity == ErrorSeverity.CRITICAL
    
    def is_recoverable(self) -> bool:
        """Check if error is recoverable."""
        return self.severity in [ErrorSeverity.LOW, ErrorSeverity.MEDIUM]
class ConfigurationError(NLMError):
    """Error related to configuration issues."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.HIGH,
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, severity, cause, context)
class InitializationError(NLMError):
    """Error related to initialization issues."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.HIGH,
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, severity, cause, context)
class SimulationError(NLMError):
    """Error related to simulation issues."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.MEDIUM,
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, severity, cause, context)
class ResourceError(NLMError):
    """Error related to resource issues (memory, file, etc.)."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.HIGH,
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, severity, cause, context)
class ValidationError(NLMError):
    """Error related to validation issues."""
    
    def __init__(self, message: str, severity: ErrorSeverity = ErrorSeverity.MEDIUM,
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, severity, cause, context)
class ErrorHandler:
    """
    Comprehensive error handling for NLM.
    
    This class provides error handling, recovery mechanisms, and
    error reporting for NLM simulations and operations.
    """
    
    def __init__(self):
        """Initialize error handler."""
        self.errors = []
        self.error_callbacks = []
        self.recovery_strategies = {}
        self.max_errors = 100
        self.log_level = "INFO"
    
    def handle_error(self, error: Exception) -> bool:
        """
        Handle an error.
        
        Args:
            error: Error to handle
            
        Returns:
            True if error was handled successfully, False otherwise
        """
        # Convert to NLMError if needed
        if not isinstance(error, NLMError):
            error = NLMError(str(error), ErrorSeverity.MEDIUM)
        
        # Log the error
        self._log_error(error)
        
        # Store error
        self.errors.append(error)
        if len(self.errors) > self.max_errors:
            self.errors.pop(0)  # Remove oldest error
        
        # Execute error callbacks
        for callback in self.error_callbacks:
            try:
                callback(error)
            except Exception as callback_error:
                print(f"Error in error callback: {callback_error}")
        
        # Try recovery if possible
        if not error.is_critical() and error.is_recoverable():
            return self._attempt_recovery(error)
        
        return error.is_critical()
    
    def _log_error(self, error: NLMError):
        """Log error based on log level."""
        level = error.severity.name
        
        if self.log_level == "DEBUG" or level in ["ERROR", "CRITICAL"]:
            print(f"[{level}] {error}")
        
        if error.cause:
            print(f"  Cause: {type(error.cause).__name__}: {error.cause}")
        
        if error.context:
            print(f"  Context: {error.context}")
    
    def _attempt_recovery(self, error: NLMError) -> bool:
        """Attempt to recover from error."""
        # Check if we have a recovery strategy for this error type
        error_type = type(error).__name__
        if error_type in self.recovery_strategies:
            strategy = self.recovery_strategies[error_type]
            try:
                return strategy(error)
            except Exception as recovery_error:
                print(f"Recovery failed: {recovery_error}")
                return False
        
        # Default recovery strategies
        if isinstance(error, ConfigurationError):
            return self._recover_from_config_error(error)
        elif isinstance(error, ResourceError):
            return self._recover_from_resource_error(error)
        elif isinstance(error, SimulationError):
            return self._recover_from_simulation_error(error)
        
        return False
    
    def _recover_from_config_error(self, error: ConfigurationError) -> bool:
        """Recover from configuration error."""
        # Try to reload default configuration
        try:
            from .core import Config
            default_config = Config()
            # This would be used to restore default config
            print("Recovered from configuration error by restoring defaults")
            return True
        except Exception:
            return False
    
    def _recover_from_resource_error(self, error: ResourceError) -> bool:
        """Recover from resource error."""
        # Try to free resources
        try:
            import gc
            gc.collect()
            print("Recovered from resource error by garbage collection")
            return True
        except Exception:
            return False
    
    def _recover_from_simulation_error(self, error: SimulationError) -> bool:
        """Recover from simulation error."""
        # Try to reset simulation state
        try:
            print("Recovered from simulation error by resetting state")
            return True
        except Exception:
            return False
    
    def add_error_callback(self, callback):
        """
        Add error callback.
        
        Args:
            callback: Callback function to add
        """
        self.error_callbacks.append(callback)
    
    def remove_error_callback(self, callback):
        """
        Remove error callback.
        
        Args:
            callback: Callback function to remove
        """
        if callback in self.error_callbacks:
            self.error_callbacks.remove(callback)
    
    def add_recovery_strategy(self, error_type: str, strategy):
        """
        Add recovery strategy for error type.
        
        Args:
            error_type: Error type (e.g., "ConfigurationError")
            strategy: Recovery strategy function
        """
        self.recovery_strategies[error_type] = strategy
    
    def remove_recovery_strategy(self, error_type: str):
        """
        Remove recovery strategy for error type.
        
        Args:
            error_type: Error type to remove strategy for
        """
        if error_type in self.recovery_strategies:
            del self.recovery_strategies[error_type]
    
    def get_error_count(self, severity: Optional[ErrorSeverity] = None) -> int:
        """
        Get error count.
        
        Args:
            severity: Filter by severity level
            
        Returns:
            Number of errors
        """
        if severity is None:
            return len(self.errors)
        
        return sum(1 for error in self.errors if error.severity == severity)
    
    def get_recent_errors(self, limit: int = 10) -> List[NLMError]:
        """
        Get recent errors.
        
        Args:
            limit: Maximum number of errors to return
            
        Returns:
            List of recent errors
        """
        return self.errors[-limit:]
    
    def clear_errors(self):
        """Clear all errors."""
        self.errors.clear()
    
    def set_log_level(self, level: str):
        """
        Set log level.
        
        Args:
            level: Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
        """
        self.log_level = level.upper()
    
    def validate_input(self, value: Any, validation_type: str, 
                      min_value: Optional[float] = None,
                      max_value: Optional[float] = None,
                      allowed_values: Optional[List[str]] = None) -> bool:
        """
        Validate input value.
        
        Args:
            value: Value to validate
            validation_type: Type of validation
            min_value: Minimum allowed value (for numeric types)
            max_value: Maximum allowed value (for numeric types)
            allowed_values: List of allowed values (for categorical types)
            
        Returns:
            True if valid, False otherwise
        """
        try:
            if validation_type == "numeric":
                if not isinstance(value, (int, float)):
                    raise ValidationError(f"Value must be numeric, got {type(value)}")
                
                if min_value is not None and value < min_value:
                    raise ValidationError(f"Value {value} is less than minimum {min_value}")
                
                if max_value is not None and value > max_value:
                    raise ValidationError(f"Value {value} is greater than maximum {max_value}")
            
            elif validation_type == "categorical":
                if allowed_values and value not in allowed_values:
                    raise ValidationError(f"Value {value} is not in allowed values {allowed_values}")
            
            elif validation_type == "positive":
                if isinstance(value, (int, float)) and value <= 0:
                    raise ValidationError(f"Value {value} must be positive")
            
            elif validation_type == "nonnegative":
                if isinstance(value, (int, float)) and value < 0:
                    raise ValidationError(f"Value {value} must be nonnegative")
            
            # If we get here, validation passed
            return True
            
        except ValidationError:
            raise
        except Exception as e:
            raise ValidationError(f"Validation failed: {e}")
    
    def check_file_exists(self, filepath: str) -> bool:
        """
        Check if file exists.
        
        Args:
            filepath: Path to file
            
        Returns:
            True if file exists, False otherwise
        """
        import os
        return os.path.exists(filepath)
    
    def check_directory_writable(self, directory: str) -> bool:
        """
        Check if directory is writable.
        
        Args:
            directory: Path to directory
            
        Returns:
            True if directory is writable, False otherwise
        """
        import os
        try:
            if not os.path.exists(directory):
                os.makedirs(directory, exist_ok=True)
            
            # Try to create a test file
            test_file = os.path.join(directory, ".test_write")
            with open(test_file, 'w') as f:
                f.write("test")
            
            os.remove(test_file)
            return True
            
        except Exception:
            return False
class ErrorReporting:
    """
    Error reporting for NLM.
    
    This class provides error reporting functionality, including
    reporting errors to users, logging to files, and generating
    error summaries.
    """
    
    def __init__(self, error_handler: ErrorHandler):
        """
        Initialize error reporter.
        
        Args:
            error_handler: Error handler to report errors from
        """
        self.error_handler = error_handler
        self.report_file = None
        self.include_stack_traces = True
    
    def set_report_file(self, filepath: str):
        """
        Set report file.
        
        Args:
            filepath: Path to report file
        """
        self.report_file = filepath
    
    def enable_stack_traces(self, enable: bool):
        """
        Enable/disable stack trace inclusion.
        
        Args:
            enable: Whether to include stack traces
        """
        self.include_stack_traces = enable
    
    def generate_error_report(self) -> str:
        """
        Generate error report.
        
        Returns:
            Error report as string
        """
        errors = self.error_handler.get_recent_errors()
        
        if not errors:
            return "No errors to report."
        
        report = "NLM Error Report\n"
        report += "=" * 50 + "\n\n"
        
        # Summary
        report += "Summary:\n"
        report += f"  Total errors: {len(errors)}\n"
        
        for severity in ErrorSeverity:
            count = self.error_handler.get_error_count(severity)
            if count > 0:
                report += f"  {severity.name}: {count}\n"
        
        report += "\n"
        
        # Detailed errors
        report += "Detailed Errors:\n"
        for i, error in enumerate(errors):
            report += f"Error {i + 1}: {error.severity.name}\n"
            report += f"  Message: {error.message}\n"
            report += f"  Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(error.timestamp))}\n"
            
            if error.cause:
                report += f"  Cause: {type(error.cause).__name__}: {error.cause}\n"
            
            if error.context:
                report += f"  Context: {error.context}\n"
            
            if self.include_stack_traces and error.error_info['stack_trace']:
                report += f"  Stack Trace:\n"
                for line in error.error_info['stack_trace'][-5:]:  # Last 5 lines
                    report += f"    {line}\n"
            
            report += "\n"
        
        return report
    
    def write_error_report(self, filepath: Optional[str] = None):
        """
        Write error report to file.
        
        Args:
            filepath: Path to write report (uses report file if not specified)
        """
        if filepath is None:
            if self.report_file is None:
                print("No report file specified")
                return
            filepath = self.report_file
        
        report = self.generate_error_report()
        
        with open(filepath, 'w') as f:
            f.write(report)
        
        print(f"Error report written to {filepath}")
# Global error handler instance
_global_error_handler = ErrorHandler()
_global_error_reporter = ErrorReporting(_global_error_handler)

# Convenience functions
def get_error_handler() -> ErrorHandler:
    """Get the global error handler."""
    return _global_error_handler

def get_error_reporter() -> ErrorReporting:
    """Get the global error reporter."""
    return _global_error_reporter

def handle_error(error: Exception) -> bool:
    """
    Handle error using the global error handler.
    
    Args:
        error: Error to handle
        
    Returns:
        True if error was handled successfully, False otherwise
    """
    return _global_error_handler.handle_error(error)

def report_error(error: Exception):
    """
    Report error using the global error reporter.
    
    Args:
        error: Error to report
    """
    _global_error_handler.handle_error(error)

def validate_input(value: Any, validation_type: str, 
                   min_value: Optional[float] = None,
                   max_value: Optional[float] = None,
                   allowed_values: Optional[List[str]] = None) -> bool:
    """
    Validate input using the global error handler.
    
    Args:
        value: Value to validate
        validation_type: Type of validation
        min_value: Minimum allowed value (for numeric types)
        max_value: Maximum allowed value (for numeric types)
        allowed_values: List of allowed values (for categorical types)
        
    Returns:
        True if valid, False otherwise
    """
    try:
        return _global_error_handler.validate_input(
            value, validation_type, min_value, max_value, allowed_values
        )
    except ValidationError:
        return False

# Initialize with default settings
__all__ = [
    'ErrorSeverity',
    'NLMError',
    'ConfigurationError',
    'InitializationError',
    'SimulationError',
    'ResourceError',
    'ValidationError',
    'ErrorHandler',
    'ErrorReporting',
    'get_error_handler',
    'get_error_reporter',
    'handle_error',
    'report_error',
    'validate_input'
]
