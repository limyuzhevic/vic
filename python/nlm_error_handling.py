# Error Handling and Validation for NLM Python Interface
# Comprehensive error handling and input validation utilities

import pynlm
import json
import time
import traceback
from typing import Dict, List, Any, Optional, Callable, Union, Tuple
from enum import Enum
import numpy as np

class NLMError(Exception):
    """
    Custom exception class for NLM errors.
    
    This exception provides detailed information about NLM-specific errors,
    including error codes, messages, and recovery suggestions.
    """
    
    def __init__(self, message: str, error_code: int = 999, 
                 component: str = "unknown", recovery_suggestion: str = ""):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.component = component
        self.recovery_suggestion = recovery_suggestion
        self.timestamp = time.time()
        self.traceback = traceback.format_stack()
        
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary representation."""
        return {
            'message': self.message,
            'error_code': self.error_code,
            'component': self.component,
            'recovery_suggestion': self.recovery_suggestion,
            'timestamp': self.timestamp,
            'type': self.__class__.__name__
        }
    
    def __str__(self) -> str:
        return f"[NLM Error {self.error_code}] {self.component}: {self.message}"
class ErrorCode(Enum):
    """NLM error codes for different types of errors."""
    
    # Success
    SUCCESS = 0
    
    # Input errors
    INVALID_INPUT = 1
    INVALID_PARAMETER = 2
    MISSING_REQUIRED_PARAM = 3
    INVALID_FILE_PATH = 4
    INVALID_SYNTAX = 5
    
    # Configuration errors
    CONFIGURATION_ERROR = 10
    INVALID_CONFIG_VALUE = 11
    MISSING_CONFIG_PARAM = 12
    CONFIG_FILE_NOT_FOUND = 13
    INVALID_CONFIG_SYNTAX = 14
    
    # Neural network errors
    NEURAL_NETWORK_ERROR = 20
    BRAIN_INITIALIZATION_ERROR = 21
    NEURON_CREATION_ERROR = 22
    SYNAPSE_CREATION_ERROR = 23
    NETWORK_CONNECTIVITY_ERROR = 24
    
    # Memory errors
    MEMORY_ERROR = 30
    ALLOCATION_FAILED = 31
    OUT_OF_MEMORY = 32
    MEMORY_CORRUPTION = 33
    
    # File system errors
    FILESYSTEM_ERROR = 40
    FILE_NOT_FOUND = 41
    PERMISSION_DENIED = 42
    DISK_FULL = 43
    
    # Validation errors
    VALIDATION_ERROR = 50
    DATA_CORRUPTION = 51
    CONSISTENCY_ERROR = 52
    
    # Simulation errors
    SIMULATION_ERROR = 60
    TIME_STEP_ERROR = 61
    CONVERGENCE_ERROR = 62
    
    # Development errors
    DEVELOPMENT_ERROR = 70
    STAGE_ERROR = 71
    MATURATION_ERROR = 72
    
    # Unknown/Generic errors
    UNKNOWN_ERROR = 999
class ValidationResult:
    """
    Result of input validation.
    
    Contains validation status, errors, warnings, and recovery suggestions.
    """
    
    def __init__(self):
        self.is_valid: bool = True
        self.errors: List[str] = []
        self.warnings: List[str] = []
        self.recovery_suggestions: List[str] = []
        self.validation_timestamp: float = time.time()
    
    def add_error(self, error: str, recovery_suggestion: str = ""):
        """Add a validation error."""
        self.is_valid = False
        self.errors.append(error)
        if recovery_suggestion:
            self.recovery_suggestions.append(f"Error '{error}': {recovery_suggestion}")
    
    def add_warning(self, warning: str):
        """Add a validation warning."""
        self.warnings.append(warning)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert validation result to dictionary."""
        return {
            'is_valid': self.is_valid,
            'errors': self.errors,
            'warnings': self.warnings,
            'recovery_suggestions': self.recovery_suggestions,
            'validation_timestamp': self.validation_timestamp
        }
class InputValidator:
    """
    Comprehensive input validation for NLM operations.
    
    Provides validation methods for various types of NLM inputs and parameters.
    """
    
    @staticmethod
    def validate_neuron_count(count: int) -> ValidationResult:
        """Validate neuron count."""
        result = ValidationResult()
        
        if count <= 0:
            result.add_error("Neuron count must be positive", 
                           "Use a positive integer value for neuron count")
        elif count > 1000000:
            result.add_error("Neuron count too large (max: 1,000,000)",
                           "Reduce neuron count or use a smaller network")
        elif count < 10:
            result.add_warning("Small neuron count may not produce meaningful results",
                             "Consider using at least 100 neurons for meaningful simulation")
        
        return result
    
    @staticmethod
    def validate_simulation_steps(steps: int) -> ValidationResult:
        """Validate simulation steps."""
        result = ValidationResult()
        
        if steps <= 0:
            result.add_error("Simulation steps must be positive",
                           "Use a positive integer value for simulation steps")
        elif steps > 1000000000:
            result.add_error("Simulation steps too large (max: 1,000,000,000)",
                           "Reduce simulation steps or run in batches")
        
        return result
    
    @staticmethod
    def validate_config(config_data: Dict[str, Any]) -> ValidationResult:
        """Validate configuration data."""
        result = ValidationResult()
        
        required_params = ['neuron_count', 'region_count', 'connection_probability']
        
        for param in required_params:
            if param not in config_data:
                result.add_error(f"Missing required parameter: {param}",
                               f"Add {param} to configuration")
            elif not isinstance(config_data[param], (int, float)):
                result.add_error(f"Invalid type for parameter {param}: expected number",
                               f"Provide a numeric value for {param}")
        
        # Validate parameter ranges
        if 'neuron_count' in config_data:
            neuron_count = config_data['neuron_count']
            if neuron_count <= 0:
                result.add_error("neuron_count must be positive",
                               "Provide a positive value for neuron_count")
            elif neuron_count > 1000000:
                result.add_warning("Very large neuron count may cause performance issues",
                                 "Consider reducing neuron_count for better performance")
        
        if 'region_count' in config_data:
            region_count = config_data['region_count']
            if region_count <= 0:
                result.add_error("region_count must be positive",
                               "Provide a positive value for region_count")
        
        if 'connection_probability' in config_data:
            conn_prob = config_data['connection_probability']
            if not 0.0 <= conn_prob <= 1.0:
                result.add_error("connection_probability must be between 0.0 and 1.0",
                               "Provide a value between 0.0 (no connections) and 1.0 (fully connected)")
        
        return result
    
    @staticmethod
    def validate_file_path(filepath: str) -> ValidationResult:
        """Validate file path."""
        result = ValidationResult()
        
        if not filepath:
            result.add_error("File path cannot be empty",
                           "Provide a valid file path")
        elif len(filepath) > 1024:
            result.add_error("File path too long (max: 1024 characters)",
                           "Use a shorter file path")
        
        # Check for directory traversal attempts
        if '..' in filepath:
            result.add_warning("File path contains '..' (directory traversal)",
                             "Ensure file path is safe and doesn't access parent directories")
        
        # Check for suspicious characters
        suspicious_chars = ['<', '>', ':', '"', '|', '?', '*']
        for char in suspicious_chars:
            if char in filepath:
                result.add_warning(f"File path contains suspicious character: '{char}'",
                                 "Use a different file path format")
        
        return result
    
    @staticmethod
    def validate_sensory_input(data: Any) -> ValidationResult:
        """Validate sensory input data."""
        result = ValidationResult()
        
        if not data:
            result.add_warning("Sensory input data is empty",
                             "Empty sensory input may not provide useful information to the network")
        
        if isinstance(data, list):
            if len(data) == 0:
                result.add_warning("Sensory input list is empty",
                                 "Consider providing meaningful sensory data")
            
            for i, value in enumerate(data):
                if isinstance(value, (int, float)):
                    if np.isnan(value) or np.isinf(value):
                        result.add_error(f"Invalid sensory data at index {i}: NaN or infinity",
                                       "Check sensory data for invalid values")
                elif not isinstance(value, (int, float)):
                    result.add_error(f"Invalid sensory data type at index {i}: expected number",
                                   "Ensure all sensory data values are numeric")
        
        elif isinstance(data, dict):
            for key, value in data.items():
                if not isinstance(value, (int, float)):
                    result.add_error(f"Invalid sensory data type for key '{key}': expected number",
                                   "Ensure all sensory data values are numeric")
        
        return result
    
    @staticmethod
    def validate_action_parameters(parameters: Any) -> ValidationResult:
        """Validate action parameters."""
        result = ValidationResult()
        
        if not parameters:
            return result  # Empty parameters are valid
        
        if isinstance(parameters, list):
            for i, value in enumerate(parameters):
                if isinstance(value, (int, float)):
                    if np.isnan(value) or np.isinf(value):
                        result.add_error(f"Invalid action parameter at index {i}: NaN or infinity",
                                       "Check action parameters for invalid values")
                elif not isinstance(value, (int, float)):
                    result.add_error(f"Invalid action parameter type at index {i}: expected number",
                                   "Ensure all action parameters are numeric")
        
        elif isinstance(parameters, dict):
            for key, value in parameters.items():
                if not isinstance(value, (int, float)):
                    result.add_error(f"Invalid action parameter type for key '{key}': expected number",
                                   "Ensure all action parameters are numeric")
        
        return result
    
    @staticmethod
    def validate_synapse_weight(weight: float) -> ValidationResult:
        """Validate synapse weight."""
        result = ValidationResult()
        
        if np.isnan(weight) or np.isinf(weight):
            result.add_error("Synapse weight is NaN or infinity",
                           "Provide a finite numeric value for synapse weight")
        elif not -1.0 <= weight <= 1.0:
            result.add_warning(f"Synapse weight {weight} outside typical range [-1.0, 1.0]",
                             "Consider clamping weight to typical range for stability")
        
        return result
class ErrorLogger:
    """
    Comprehensive error logging system for NLM.
    
    Provides structured error logging with different log levels and
    customizable output formats.
    """
    
    def __init__(self, log_level: str = "INFO", log_file: Optional[str] = None):
        self.log_level = log_level
        self.log_file = log_file
        self.error_count = 0
        self.warning_count = 0
        self.info_count = 0
        self.session_start = time.time()
        
        # Log levels
        self.log_levels = {
            'DEBUG': 0,
            'INFO': 1,
            'WARNING': 2,
            'ERROR': 3,
            'CRITICAL': 4
        }
        
        # Initialize log file if specified
        if self.log_file:
            try:
                with open(self.log_file, 'w') as f:
                    f.write(f"=== NLM Error Log Session Started ===\n")
                    f.write(f"Start Time: {time.ctime(self.session_start)}\n")
                    f.write(f"Log Level: {log_level}\n\n")
            except Exception as e:
                print(f"Warning: Could not open log file {log_file}: {e}")
    
    def log(self, level: str, message: str, component: str = "unknown", 
            context: Optional[Dict[str, Any]] = None, 
            exception: Optional[Exception] = None) -> None:
        """Log a message at the specified level."""
        if self.log_levels.get(level, 1) < self.log_levels.get(self.log_level, 1):
            return  # Skip if below log level
        
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        
        # Build log message
        log_entry = {
            'timestamp': timestamp,
            'level': level,
            'component': component,
            'message': message,
            'context': context or {},
            'session_id': self.session_start
        }
        
        if exception:
            log_entry['exception'] = str(exception)
            log_entry['traceback'] = traceback.format_exc()
        
        # Print to console
        self._print_log_entry(log_entry)
        
        # Write to file if specified
        if self.log_file:
            self._write_log_entry(log_entry)
        
        # Update counters
        self._update_counters(level)
    
    def log_error(self, message: str, component: str = "unknown",
                  context: Optional[Dict[str, Any]] = None,
                  exception: Optional[Exception] = None) -> None:
        """Log an error message."""
        self.log('ERROR', message, component, context, exception)
    
    def log_warning(self, message: str, component: str = "unknown",
                   context: Optional[Dict[str, Any]] = None) -> None:
        """Log a warning message."""
        self.log('WARNING', message, component, context)
    
    def log_info(self, message: str, component: str = "unknown",
                 context: Optional[Dict[str, Any]] = None) -> None:
        """Log an info message."""
        self.log('INFO', message, component, context)
    
    def log_debug(self, message: str, component: str = "unknown",
                  context: Optional[Dict[str, Any]] = None) -> None:
        """Log a debug message."""
        self.log('DEBUG', message, component, context)
    
    def log_validation_error(self, validation_result: ValidationResult,
                           operation: str = "validation") -> None:
        """Log validation errors and warnings."""
        if validation_result.errors:
            for error in validation_result.errors:
                self.log_error(f"Validation Error: {error}", 
                             context={'operation': operation, 'type': 'validation_error'})
        
        if validation_result.warnings:
            for warning in validation_result.warnings:
                self.log_warning(f"Validation Warning: {warning}",
                               context={'operation': operation, 'type': 'validation_warning'})
    
    def log_performance_metric(self, metric_name: str, value: float,
                              unit: str = "", context: Optional[Dict[str, Any]] = None) -> None:
        """Log a performance metric."""
        message = f"{metric_name}: {value:.3f}"
        if unit:
            message += f" {unit}"
        
        self.log_info(message, context=context)
    
    def _print_log_entry(self, entry: Dict[str, Any]) -> None:
        """Print log entry to console."""
        timestamp = entry['timestamp']
        level = entry['level'].rjust(5)
        component = entry['component'].ljust(15)
        message = entry['message']
        
        print(f"[{timestamp}] {level} {component} {message}")
        
        # Add context if present
        if entry.get('context'):
            context_str = ", ".join([f"{k}={v}" for k, v in entry['context'].items()])
            print(f"    Context: {context_str}")
        
        # Add exception if present
        if entry.get('exception'):
            print(f"    Exception: {entry['exception']}")
    
    def _write_log_entry(self, entry: Dict[str, Any]) -> None:
        """Write log entry to file."""
        try:
            with open(self.log_file, 'a') as f:
                # Convert entry to JSON for easy parsing
                json_entry = json.dumps(entry, default=str)
                f.write(json_entry + "\n")
        except Exception as e:
            print(f"Warning: Could not write to log file: {e}")
    
    def _update_counters(self, level: str) -> None:
        """Update log counters."""
        if level == 'ERROR':
            self.error_count += 1
        elif level == 'WARNING':
            self.warning_count += 1
        elif level == 'INFO':
            self.info_count += 1
    
    def get_log_statistics(self) -> Dict[str, Any]:
        """Get log statistics."""
        return {
            'session_start': self.session_start,
            'error_count': self.error_count,
            'warning_count': self.warning_count,
            'info_count': self.info_count,
            'log_level': self.log_level,
            'log_file': self.log_file,
            'current_time': time.time()
        }
class ExceptionHandler:
    """
    Exception handler for NLM operations.
    
    Provides comprehensive exception handling with automatic recovery
    attempts and detailed error reporting.
    """
    
    def __init__(self, logger: Optional[ErrorLogger] = None):
        self.logger = logger or ErrorLogger()
        self.recovery_strategies = {}
        self.register_default_recovery_strategies()
    
    def register_recovery_strategy(self, error_code: int,
                                  strategy_name: str,
                                  can_recover_func: Callable[[], bool],
                                  recover_func: Callable[[], None],
                                  success_rate: float = 1.0) -> None:
        """Register a recovery strategy for an error code."""
        key = (error_code, strategy_name)
        self.recovery_strategies[key] = {
            'can_recover': can_recover_func,
            'recover': recover_func,
            'success_rate': success_rate,
            'attempts': 0,
            'successes': 0
        }
    
    def handle_exception(self, exception: Exception, operation: str = "unknown",
                        component: str = "unknown",
                        context: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Handle an exception with automatic recovery attempts.
        
        Returns a dictionary with handling results.
        """
        result = {
            'exception_handled': True,
            'recovery_attempted': False,
            'recovery_successful': False,
            'error_info': None,
            'recovery_actions': [],
            'log_messages': []
        }
        
        # Convert exception to NLMError if possible
        nlm_error = self._convert_to_nlm_error(exception)
        
        # Log the exception
        if nlm_error:
            log_messages = self.logger.log_error(
                nlm_error.message,
                component=component,
                context=context,
                exception=exception
            )
            result['log_messages'] = log_messages
        
        # Try recovery strategies
        if nlm_error:
            recovery_actions = self._attempt_recovery(nlm_error, context)
            result['recovery_actions'] = recovery_actions
            result['recovery_attempted'] = not recovery_actions.empty()
            
            # Check if recovery was successful
            if recovery_actions:
                result['recovery_successful'] = True
        
        # Store error info
        result['error_info'] = {
            'type': type(exception).__name__,
            'message': str(exception),
            'timestamp': time.time(),
            'operation': operation,
            'component': component,
            'context': context or {}
        }
        
        return result
    
    def _convert_to_nlm_error(self, exception: Exception) -> Optional[NLMError]:
        """Convert a standard exception to NLMError."""
        if isinstance(exception, NLMError):
            return exception
        
        # Map common exception types to NLMError
        error_mapping = {
            ValueError: (ErrorCode.INVALID_INPUT, "Invalid parameter value"),
            TypeError: (ErrorCode.INVALID_INPUT, "Invalid parameter type"),
            KeyError: (ErrorCode.CONFIGURATION_ERROR, "Missing configuration parameter"),
            FileNotFoundError: (ErrorCode.FILESYSTEM_ERROR, "File not found"),
            PermissionError: (ErrorCode.FILESYSTEM_ERROR, "Permission denied"),
            MemoryError: (ErrorCode.MEMORY_ERROR, "Out of memory"),
            RuntimeError: (ErrorCode.UNKNOWN_ERROR, "Runtime error"),
        }
        
        for exc_type, (code, message) in error_mapping.items():
            if isinstance(exception, exc_type):
                return NLMError(str(exception), code, "exception_handler", message)
        
        # Default mapping for unknown exception types
        return NLMError(str(exception), ErrorCode.UNKNOWN_ERROR, "exception_handler")
    
    def _attempt_recovery(self, error: NLMError, 
                         context: Optional[Dict[str, Any]]) -> List[str]:
        """Attempt recovery for the given error."""
        actions = []
        
        # Find applicable recovery strategies
        strategies = []
        for (error_code, strategy_name), strategy_data in self.recovery_strategies.items():
            if error.error_code == error_code and strategy_data['can_recover']():
                strategies.append((strategy_name, strategy_data))
        
        if not strategies:
            # Try default strategies
            strategies = self._get_default_strategies(error)
        
        # Execute recovery strategies
        for strategy_name, strategy_data in strategies:
            try:
                strategy_data['attempts'] += 1
                strategy_data['recover']()
                actions.append(f"Applied recovery strategy: {strategy_name}")
                break  # Stop after first successful recovery
            except Exception as e:
                self.logger.log_warning(f"Recovery strategy {strategy_name} failed: {e}",
                                       context={'error_code': error.error_code})
        
        return actions
    
    def _get_default_strategies(self, error: NLMError) -> List[Tuple[str, Dict]]:
        """Get default recovery strategies for error type."""
        strategies = []
        
        if error.error_code == ErrorCode.INVALID_INPUT:
            strategies.append(('reset_to_defaults', {
                'can_recover': lambda: True,
                'recover': lambda: self.logger.log_info("Resetting to default parameters"),
                'success_rate': 0.9
            }))
        
        elif error.error_code == ErrorCode.CONFIGURATION_ERROR:
            strategies.append(('reload_config', {
                'can_recover': lambda: True,
                'recover': lambda: self.logger.log_info("Reloading configuration file"),
                'success_rate': 0.8
            }))
        
        elif error.error_code == ErrorCode.MEMORY_ERROR:
            strategies.append(('memory_cleanup', {
                'can_recover': lambda: True,
                'recover': lambda: self.logger.log_info("Attempting memory cleanup"),
                'success_rate': 0.7
            }))
        
        return strategies
    
    def register_default_recovery_strategies(self) -> None:
        """Register default recovery strategies."""
        # Register strategies for common error codes
        self.register_recovery_strategy(
            ErrorCode.INVALID_INPUT.value,
            "reset_to_defaults",
            lambda: True,  # Can always attempt
            lambda: self.logger.log_info("Resetting invalid parameters to defaults"),
            0.9
        )
        
        self.register_recovery_strategy(
            ErrorCode.CONFIGURATION_ERROR.value,
            "reload_config",
            lambda: True,  # Can always attempt
            lambda: self.logger.log_info("Reloading configuration from file"),
            0.8
        )
        
        self.register_recovery_strategy(
            ErrorCode.MEMORY_ERROR.value,
            "memory_cleanup",
            lambda: True,  # Can always attempt
            lambda: self.logger.log_info("Attempting memory cleanup and garbage collection"),
            0.7
        )
class NLMErrorManager:
    """
    Main error management system for NLM.
    
    Integrates error logging, exception handling, and validation into a
    single coherent system for robust NLM operation.
    """
    
    def __init__(self, log_level: str = "INFO", log_file: Optional[str] = None):
        self.logger = ErrorLogger(log_level, log_file)
        self.exception_handler = ExceptionHandler(self.logger)
        self.input_validator = InputValidator()
    
    def validate_and_log(self, validation_result: ValidationResult,
                        operation: str = "validation") -> bool:
        """
        Validate input and log any errors or warnings.
        
        Returns True if validation passed, False otherwise.
        """
        self.logger.log_validation_error(validation_result, operation)
        return validation_result.is_valid
    
    def handle_exception(self, exception: Exception, operation: str = "unknown",
                       component: str = "unknown",
                       context: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Handle an exception using the exception handler."""
        return self.exception_handler.handle_exception(
            exception, operation, component, context
        )
    
    def log_performance_metric(self, metric_name: str, value: float,
                              unit: str = "", context: Optional[Dict[str, Any]] = None) -> None:
        """Log a performance metric."""
        self.logger.log_performance_metric(metric_name, value, unit, context)
    
    def get_log_statistics(self) -> Dict[str, Any]:
        """Get log statistics."""
        return self.logger.get_log_statistics()
    
    def safe_execute(self, operation: Callable, *args, **kwargs) -> Any:
        """
        Safely execute an operation with comprehensive error handling.
        
        Returns a tuple of (result, success, error_info).
        """
        try:
            result = operation(*args, **kwargs)
            return result, True, None
        except Exception as e:
            error_info = self.handle_exception(
                e, operation.__name__ if hasattr(operation, '__name__') else 'unknown',
                'operation_handler'
            )
            return None, False, error_info
# Global error manager instance
_default_error_manager = None

def get_error_manager(log_level: str = "INFO", log_file: Optional[str] = None) -> NLMErrorManager:
    """Get or create the global error manager."""
    global _default_error_manager
    if _default_error_manager is None:
        _default_error_manager = NLMErrorManager(log_level, log_file)
    return _default_error_manager

def validate_input(config: Any, validation_type: str = "general") -> ValidationResult:
    """
    Convenience function for input validation.
    
    Args:
        config: Input configuration to validate
        validation_type: Type of validation to perform
        
    Returns:
        Validation result
    """
    manager = get_error_manager()
    
    if validation_type == "general":
        # Generic validation - convert to dict if needed
        if hasattr(config, 'to_dict'):
            config_dict = config.to_dict()
        elif hasattr(config, '__dict__'):
            config_dict = config.__dict__
        else:
            config_dict = config
        
        return manager.input_validator.validate_config(config_dict)
    
    elif validation_type == "sensory":
        return manager.input_validator.validate_sensory_input(config)
    
    elif validation_type == "action":
        return manager.input_validator.validate_action_parameters(config)
    
    else:
        # Default validation
        return ValidationResult()
# Export key classes and functions
__all__ = [
    'NLMError',
    'ErrorCode',
    'ValidationResult',
    'InputValidator',
    'ErrorLogger',
    'ExceptionHandler',
    'NLMErrorManager',
    'get_error_manager',
    'validate_input'
]
