#!/usr/bin/env python3
"""
Enhanced NLM Error Handling System

Comprehensive error handling for NLM (Neural Learning Machine) CLI and related tools.
Implements centralized error management, custom exception hierarchy, validation,
error recovery, and diagnostic reporting.
"""

import sys
import logging
import time
import traceback
from typing import Dict, List, Any, Optional, Union, Callable, Type
from enum import Enum, auto
from datetime import datetime
from dataclasses import dataclass, field
from contextlib import contextmanager
import json
import yaml
from pathlib import Path
import threading
import asyncio

# Version
__version__ = "2.0.0"

# Error codes enum for systematic error identification
class ErrorCode(Enum):
    # Configuration errors
    CONFIG_FILE_NOT_FOUND = auto()
    CONFIG_INVALID_FORMAT = auto()
    CONFIG_VALIDATION_FAILED = auto()
    CONFIG_KEY_NOT_FOUND = auto()
    CONFIG_TYPE_MISMATCH = auto()
    CONFIG_LOAD_FAILED = auto()
    CONFIG_SAVE_FAILED = auto()
    
    # Neural network errors
    BRAIN_INITIALIZATION_FAILED = auto()
    BRAIN_NOT_INITIALIZED = auto()
    NEURON_NOT_FOUND = auto()
    SYNAPSE_CREATION_FAILED = auto()
    POPULATION_OVERFLOW = auto()
    REGION_NOT_FOUND = auto()
    INVALID_NEURON_TYPE = auto()
    BRAIN_STEP_FAILED = auto()
    
    # Simulation errors
    SIMULATION_NOT_STARTED = auto()
    SIMULATION_STEP_FAILED = auto()
    INVALID_TIME_STEP = auto()
    CLOCK_SYNC_FAILED = auto()
    TICK_FAILED = auto()
    SIMULATION_TIMEOUT = auto()
    
    # Memory errors
    MEMORY_ALLOCATION_FAILED = auto()
    MEMORY_CORRUPTION = auto()
    NULL_POINTER_DEREFERENCE = auto()
    INVALID_MEMORY_ACCESS = auto()
    MEMORY_POOL_EXHAUSTED = auto()
    
    # Validation errors
    INPUT_VALIDATION_FAILED = auto()
    RANGE_VALIDATION_FAILED = auto()
    TYPE_VALIDATION_FAILED = auto()
    CONSTRAINT_VALIDATION_FAILED = auto()
    SCHEMA_VALIDATION_FAILED = auto()
    DATA_INTEGRITY_FAILED = auto()
    
    # Sensory system errors
    SENSOR_NOT_INITIALIZED = auto()
    SENSOR_DATA_CORRUPTED = auto()
    SENSOR_OVERFLOW = auto()
    SENSOR_TIMEOUT = auto()
    AUDIO_PROCESSING_FAILED = auto()
    VISION_PROCESSING_FAILED = auto()
    
    # Motor system errors
    MOTOR_COMMAND_FAILED = auto()
    ACTION_QUEUE_FULL = auto()
    INVALID_ACTION_TYPE = auto()
    MOTOR_SYSTEM_BUSY = auto()
    ACTUATOR_FAILURE = auto()
    
    # Plasticity errors
    PLASTICITY_RULE_FAILED = auto()
    INVALID_PLACING_METHOD = auto()
    STDP_CALCULATION_FAILED = auto()
    HEBBIAN_UPDATE_FAILED = auto()
    
    # Neuromodulation errors
    MODULATOR_NOT_INITIALIZED = auto()
    NOVELTY_DETECTION_FAILED = auto()
    REWARD_CALCULATION_FAILED = auto()
    PREDICTION_ERROR_CALCULATION_FAILED = auto()
    CURIOUSY_TOO_LOW = auto()
    
    # Prediction errors
    PREDICTION_FAILED = auto()
    MODEL_NOT_TRAINED = auto()
    INVALID_PREDICTION_TYPE = auto()
    PREDICTION_TIMEOUT = auto()
    
    # Cognition errors
    CONCEPT_FORMATION_FAILED = auto()
    PLANNING_FAILED = auto()
    INVALID_CONCEPT_TYPE = auto()
    
    # Memory system errors
    WORKING_MEMORY_FULL = auto()
    EPISODIC_MEMORY_WRITE_FAILED = auto()
    SEMANTIC_MEMORY_ACCESS_FAILED = auto()
    PROCEEDURAL_MEMORY_RECALL_FAILED = auto()
    
    # Performance errors
    PERFORMANCE_LIMIT_EXCEEDED = auto()
    OPTIMIZATION_FAILED = auto()
    SIMD_NOT_SUPPORTED = auto()
    
    # General errors
    UNKNOWN_ERROR = auto()
    NOT_IMPLEMENTED = auto()
    OUT_OF_MEMORY = auto()
    INVALID_ARGUMENT = auto()
    TIMEOUT = auto()
    NETWORK_ERROR = auto()
    IO_ERROR = auto()
    PERMISSION_DENIED = auto()
    SYSTEM_ERROR = auto()

# Exception hierarchy
class NLMException(Exception):
    """Base exception for all NLM errors"""
    
    def __init__(self, 
                 error_code: ErrorCode,
                 message: str,
                 context: Optional[Dict[str, Any]] = None,
                 cause: Optional[Exception] = None,
                 file: Optional[str] = None,
                 line: Optional[int] = None,
                 function: Optional[str] = None):
        self.error_code = error_code
        self.message = message
        self.context = context or {}
        self.cause = cause
        self.file = file or ""
        self.line = line or 0
        self.function = function or ""
        self.timestamp = datetime.now()
        self.stack_trace = traceback.format_stack()
        
        # Add file/line info from __init__ if not provided
        if not file and "__init__" in self.__class__.__name__:
            import inspect
            frame = inspect.currentframe()
            if frame and frame.f_back:
                self.file = frame.f_back.f_code.co_filename
                self.line = frame.f_back.f_lineno
                self.function = frame.f_back.f_code.co_name
        
        super().__init__(self._format_message())
    
    def _format_message(self) -> str:
        """Format the error message with error code and context"""
        msg = f"[{self.error_code.name}] {self.message}"
        if self.file:
            msg += f" (at {self.file}:{self.line})"
        if self.context:
            context_str = ", ".join(f"{k}={v}" for k, v in self.context.items())
            msg += f" Context: {context_str}"
        return msg
    
    def __str__(self) -> str:
        return self._format_message()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert exception to dictionary for serialization"""
        return {
            "error_code": self.error_code.name,
            "message": self.message,
            "context": self.context,
            "file": self.file,
            "line": self.line,
            "function": self.function,
            "timestamp": self.timestamp.isoformat(),
            "stack_trace": self.stack_trace,
            "cause": str(self.cause) if self.cause else None
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'NLMException':
        """Create exception from dictionary"""
        error_code = ErrorCode[data.get("error_code", "UNKNOWN_ERROR")]
        message = data.get("message", "Unknown error")
        context = data.get("context", {})
        
        # Try to reconstruct cause if available
        cause = None
        if data.get("cause"):
            cause = Exception(data["cause"])
        
        return cls(
            error_code=error_code,
            message=message,
            context=context,
            cause=cause,
            file=data.get("file"),
            line=data.get("line"),
            function=data.get("function")
        )
    
    def log(self, logger: logging.Logger, level=logging.ERROR) -> None:
        """Log the exception with detailed information"""
        logger.log(level, str(self))
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug(f"Stack trace: {''.join(self.stack_trace)}")
            if self.cause:
                logger.debug(f"Caused by: {self.cause}")
            if self.context:
                logger.debug(f"Context: {self.context}")

# Specific exception classes
class ConfigurationException(NLMException):
    """Exception for configuration-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.CONFIG_VALIDATION_FAILED, message, **kwargs)

class BrainException(NLMException):
    """Exception for brain-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.BRAIN_INITIALIZATION_FAILED, message, **kwargs)

class SimulationException(NLMException):
    """Exception for simulation-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.SIMULATION_STEP_FAILED, message, **kwargs)

class MemoryException(NLMException):
    """Exception for memory-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.MEMORY_ALLOCATION_FAILED, message, **kwargs)

class ValidationException(NLMException):
    """Exception for validation-related errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.INPUT_VALIDATION_FAILED, message, **kwargs)

class SensoryException(NLMException):
    """Exception for sensory system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.SENSOR_NOT_INITIALIZED, message, **kwargs)

class MotorException(NLMException):
    """Exception for motor system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.MOTOR_COMMAND_FAILED, message, **kwargs)

class PlasticityException(NLMException):
    """Exception for plasticity system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.PLASTICITY_RULE_FAILED, message, **kwargs)

class NeuromodulationException(NLMException):
    """Exception for neuromodulation system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.MODULATOR_NOT_INITIALIZED, message, **kwargs)

class PredictionException(NLMException):
    """Exception for prediction system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.PREDICTION_FAILED, message, **kwargs)

class CognitionException(NLMException):
    """Exception for cognition system errors"""
    
    def __init__(self, message: str, **kwargs):
        super().__init__(ErrorCode.CONCEPT_FORMATION_FAILED, message, **kwargs)

# Error recovery and resilience
class ErrorRecovery:
    """Error recovery mechanisms"""
    
    class RecoveryStrategy(Enum):
        FAIL_FAST = "fail_fast"
        GRACEFUL_DEGRADATION = "graceful_degradation"
        RETRY = "retry"
        FALLBACK = "fallback"
        CIRCUIT_BREAKER = "circuit_breaker"
        AUTO_RECOVER = "auto_recover"
    
    @dataclass
    class RecoveryConfig:
        strategy: RecoveryStrategy = RecoveryStrategy.FAIL_FAST
        max_retries: int = 3
        timeout_ms: int = 5000
        retry_delay_ms: int = 1000
        circuit_breaker_threshold: int = 5
        circuit_breaker_timeout_ms: int = 60000
        fallback_condition: Optional[Callable[[], bool]] = None
        recovery_action: Optional[Callable[[], None]] = None
        
        # Performance tracking
        total_operations: int = field(default=0, init=False)
        failed_operations: int = field(default=0, init=False)
        last_failure_time: Optional[float] = field(default=None, init=False)
        
        def record_operation(self, success: bool):
            self.total_operations += 1
            if not success:
                self.failed_operations += 1
                self.last_failure_time = time.time()
        
        def is_circuit_open(self) -> bool:
            """Check if circuit breaker is open"""
            if self.last_failure_time is None:
                return False
            
            if self.failed_operations >= self.circuit_breaker_threshold:
                if (time.time() - self.last_failure_time) * 1000 >= self.circuit_breaker_timeout_ms:
                    # Reset circuit breaker after timeout
                    self.failed_operations = 0
                    self.last_failure_time = None
                    return False
                return True
            return False
        
        def can_proceed(self) -> bool:
            """Check if operation can proceed"""
            if self.strategy == self.RecoveryStrategy.FAIL_FAST:
                return True
            
            if self.strategy == self.RecoveryStrategy.CIRCUIT_BREAKER:
                return not self.is_circuit_open()
            
            return True
    
    @staticmethod
    def with_retry(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with retry mechanism"""
        if not config.can_proceed():
            raise ConfigurationException("Circuit breaker is open, cannot retry operation")
        
        for attempt in range(config.max_retries + 1):
            try:
                result = func(*args, **kwargs)
                config.record_operation(True)
                return result
            except Exception as e:
                config.record_operation(False)
                if attempt == config.max_retries:
                    raise
                
                # Exponential backoff
                delay = config.retry_delay_ms * (2 ** attempt)
                time.sleep(delay / 1000.0)
    
    @staticmethod
    def with_graceful_degradation(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with graceful degradation"""
        try:
            result = func(*args, **kwargs)
            config.record_operation(True)
            return result
        except Exception as e:
            config.record_operation(False)
            
            if config.fallback_condition and config.fallback_condition():
                # Use fallback
                if config.recovery_action:
                    config.recovery_action()
                raise ValidationException(f"Operation failed, using fallback: {e}")
            
            raise
    
    @staticmethod
    def with_circuit_breaker(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with circuit breaker"""
        if not config.can_proceed():
            raise ConfigurationException("Circuit breaker is open")
        
        try:
            result = func(*args, **kwargs)
            config.record_operation(True)
            return result
        except Exception as e:
            config.record_operation(False)
            raise
    
    @staticmethod
    def auto_recover(config: RecoveryConfig, func: Callable, *args, **kwargs):
        """Execute function with automatic recovery"""
        if not config.can_proceed():
            if config.recovery_action:
                config.recovery_action()
        
        return ErrorRecovery.with_retry(config, func, *args, **kwargs)

# Validation system
class Validation:
    """Validation utilities for NLM"""
    
    @staticmethod
    def validate_range(value: Union[int, float], min_val: Union[int, float], max_val: Union[int, float], 
                      name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is within specified range"""
        if value < min_val or value > max_val:
            error_msg = f"Value {name} ({value}) is out of range [{min_val}, {max_val}]"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_positive(value: Union[int, float], name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is positive"""
        if value <= 0:
            error_msg = f"Value {name} ({value}) must be positive"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_non_negative(value: Union[int, float], name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is non-negative"""
        if value < 0:
            error_msg = f"Value {name} ({value}) must be non-negative"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_type(value: Any, expected_type: Type, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is of expected type"""
        if not isinstance(value, expected_type):
            error_msg = f"Value {name} ({value}) is not of type {expected_type.__name__}"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_not_null(value: Any, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value is not None"""
        if value is None:
            error_msg = f"Value {name} cannot be None"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_string_not_empty(value: str, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that string is not empty"""
        if not value or not value.strip():
            error_msg = f"Value {name} cannot be empty"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_list_not_empty(value: List, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that list is not empty"""
        if not value:
            error_msg = f"Value {name} cannot be empty list"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_max_size(value: Union[List, Dict, str], max_size: int, name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that value size does not exceed maximum"""
        actual_size = len(value)
        if actual_size > max_size:
            error_msg = f"Value {name} size ({actual_size}) exceeds maximum ({max_size})"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True
    
    @staticmethod
    def validate_dict_keys(value: Dict, required_keys: List[str], name: str = "value", raise_exception: bool = True) -> bool:
        """Validate that dictionary contains required keys"""
        missing_keys = [key for key in required_keys if key not in value]
        if missing_keys:
            error_msg = f"Value {name} is missing required keys: {', '.join(missing_keys)}"
            if raise_exception:
                raise ValidationException(error_msg)
            return False
        return True

# Error reporting and diagnostics
class ErrorReporter:
    """Error reporting and diagnostics"""
    
    @dataclass
    class DiagnosticInfo:
        error_type: str
        error_message: str
        stack_trace: str
        system_state: str
        performance_impact: str
        context: Dict[str, str] = field(default_factory=dict)
        recovery_suggestions: List[str] = field(default_factory=list)
    
    @staticmethod
    def collect_diagnostics(exception: Exception) -> DiagnosticInfo:
        """Collect diagnostics information for an exception"""
        info = ErrorReporter.DiagnosticInfo(
            error_type=type(exception).__name__,
            error_message=str(exception),
            stack_trace=traceback.format_exc(),
            system_state=ErrorReporter._get_system_state(),
            performance_impact=ErrorReporter._analyze_performance_impact(exception)
        )
        return info
    
    @staticmethod
    def _get_system_state() -> str:
        """Get current system state information"""
        import platform
        import psutil
        
        state = []
        state.append(f"Platform: {platform.system()} {platform.release()}")
        state.append(f"Python: {platform.python_version()}")
        
        try:
            process = psutil.Process()
            memory_info = process.memory_info()
            state.append(f"Memory: RSS={memory_info.rss / 1024 / 1024:.1f}MB, VMS={memory_info.vms / 1024 / 1024:.1f}MB")
            state.append(f"CPU: {process.cpu_percent()}%")
        except ImportError:
            state.append("psutil: not available")
        except Exception as e:
            state.append(f"Memory check failed: {e}")
        
        return "; ".join(state)
    
    @staticmethod
    def _analyze_performance_impact(exception: Exception) -> str:
        """Analyze performance impact of the error"""
        # This is a simplified implementation
        if isinstance(exception, (ValidationException, MemoryException)):
            return "Low impact - error is recoverable"
        elif isinstance(exception, (BrainException, SimulationException)):
            return "High impact - may require restart"
        elif isinstance(exception, ConfigurationException):
            return "Medium impact - affects system configuration"
        else:
            return "Unknown impact"
    
    @staticmethod
    def generate_report(diagnostic: DiagnosticInfo, logger: logging.Logger) -> str:
        """Generate a human-readable error report"""
        report = f"Error Report\n"
        report += f"Timestamp: {datetime.now().isoformat()}\n"
        report += f"Error Type: {diagnostic.error_type}\n"
        report += f"Error Message: {diagnostic.error_message}\n"
        report += f"System State: {diagnostic.system_state}\n"
        report += f"Performance Impact: {diagnostic.performance_impact}\n"
        
        if diagnostic.context:
            report += "Context:\n"
            for key, value in diagnostic.context.items():
                report += f"  {key}: {value}\n"
        
        if diagnostic.recovery_suggestions:
            report += "Recovery Suggestions:\n"
            for suggestion in diagnostic.recovery_suggestions:
                report += f"  - {suggestion}\n"
        
        # Log the report
        logger.error(report)
        
        return report

# Error handling infrastructure
class ErrorHandling:
    """Centralized error handling infrastructure"""
    
    class ErrorHandlerConfig:
        def __init__(self):
            self.default_log_level = logging.WARNING
            self.error_reporting_enabled = True
            self.error_serialization_enabled = True
            self.error_recovery_enabled = True
            self.circuit_breaker_enabled = True
            self.graceful_degradation_enabled = True
            self.retry_enabled = True
            self.max_error_history = 100
            self.error_persistence_file = "error_history.json"
    
    def __init__(self, config: Optional[ErrorHandlerConfig] = None):
        self.config = config or self.ErrorHandlerConfig()
        self.error_history: List[Dict[str, Any]] = []
        self.error_handlers: List[Callable[[NLMException], None]] = []
        self.logger = logging.getLogger(__name__)
        
        # Setup logging
        logging.basicConfig(
            level=self.config.default_log_level,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
    
    def add_error_handler(self, handler: Callable[[NLMException], None]):
        """Add custom error handler"""
        self.error_handlers.append(handler)
    
    def handle_error(self, exception: NLMException) -> None:
        """Handle error with configured strategy"""
        # Store in error history
        self._store_error(exception)
        
        # Log the error
        exception.log(self.logger)
        
        # Call custom handlers
        for handler in self.error_handlers:
            try:
                handler(exception)
            except Exception as handler_error:
                self.logger.error(f"Error handler failed: {handler_error}")
        
        # Generate diagnostics if reporting is enabled
        if self.config.error_reporting_enabled:
            diagnostic = ErrorReporter.collect_diagnostics(exception)
            ErrorReporter.generate_report(diagnostic, self.logger)
    
    def _store_error(self, exception: NLMException):
        """Store error in history"""
        error_data = exception.to_dict()
        self.error_history.append(error_data)
        
        # Trim history if too large
        if len(self.error_history) > self.config.max_error_history:
            self.error_history = self.error_history[-self.config.max_error_history:]
    
    def load_error_history(self) -> List[Dict[str, Any]]:
        """Load error history from file"""
        if not Path(self.config.error_persistence_file).exists():
            return self.error_history
        
        try:
            with open(self.config.error_persistence_file, 'r') as f:
                file_history = json.load(f)
            
            self.error_history = file_history
            return self.error_history
        except Exception as e:
            self.logger.error(f"Failed to load error history: {e}")
            return self.error_history
    
    def save_error_history(self):
        """Save error history to file"""
        if not self.config.error_persistence_file:
            return
        
        try:
            with open(self.config.error_persistence_file, 'w') as f:
                json.dump(self.error_history, f, indent=2, default=str)
        except Exception as e:
            self.logger.error(f"Failed to save error history: {e}")
    
    def get_error_summary(self) -> Dict[str, Any]:
        """Get summary of errors"""
        if not self.error_history:
            return {"total_errors": 0, "error_by_code": {}, "recent_errors": []}
        
        error_by_code: Dict[str, int] = {}
        for error in self.error_history:
            code = error.get("error_code", "UNKNOWN")
            error_by_code[code] = error_by_code.get(code, 0) + 1
        
        recent_errors = self.error_history[-10:] if len(self.error_history) > 10 else self.error_history
        
        return {
            "total_errors": len(self.error_history),
            "error_by_code": error_by_code,
            "recent_errors": recent_errors
        }
    
    @contextmanager
    def error_context(self, operation_name: str, **context):
        """Context manager for error handling"""
        try:
            yield
        except NLMException as e:
            e.add_context("operation", operation_name)
            e.add_context(**context)
            raise
        except Exception as e:
            # Convert non-NLM exceptions
            nlm_error = NLMException(
                ErrorCode.SYSTEM_ERROR,
                f"Unexpected error in {operation_name}: {str(e)}",
                context={"operation": operation_name, **context},
                cause=e
            )
            raise nlm_error

# Global error handler
_global_error_handler: Optional[ErrorHandling] = None

def get_global_error_handler() -> ErrorHandling:
    """Get the global error handler instance"""
    global _global_error_handler
    if _global_error_handler is None:
        _global_error_handler = ErrorHandling()
    return _global_error_handler

def set_global_error_handler(handler: ErrorHandling):
    """Set the global error handler instance"""
    global _global_error_handler
    _global_error_handler = handler

def initialize_error_handling(config: Optional[ErrorHandling.ErrorHandlerConfig] = None):
    """Initialize the error handling system"""
    handler = ErrorHandling(config)
    set_global_error_handler(handler)
    return handler

def handle_exception(exception: Exception):
    """Handle exception with global error handler"""
    global_error_handler = get_global_error_handler()
    
    if isinstance(exception, NLMException):
        global_error_handler.handle_error(exception)
    else:
        # Convert non-NLM exceptions
        nlm_error = NLMException(
            ErrorCode.SYSTEM_ERROR,
            str(exception),
            cause=exception
        )
        global_error_handler.handle_error(nlm_error)

# Convenience macros for error handling
import functools

def safe_call(func: Callable, *args, **kwargs) -> Any:
    """Safely call a function with error handling"""
    try:
        return func(*args, **kwargs)
    except NLMException:
        raise
    except Exception as e:
        raise NLMException(ErrorCode.SYSTEM_ERROR, f"Error calling {func.__name__}: {str(e)}") from e

def validate_call(func: Callable) -> Callable:
    """Decorator to validate function calls"""
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        # Validate arguments (example implementation)
        for arg in args:
            Validation.validate_not_null(arg)
        
        return func(*args, **kwargs)
    return wrapper

def with_error_handler(config: ErrorRecovery.RecoveryConfig):
    """Decorator to add error handling to a function"""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            try:
                return ErrorRecovery.with_retry(config, func, *args, **kwargs)
            except NLMException:
                raise
            except Exception as e:
                raise NLMException(ErrorCode.SYSTEM_ERROR, f"Error in {func.__name__}: {str(e)}") from e
        return wrapper
    return decorator

# Initialize default error handling
def _init_default_error_handling():
    """Initialize default error handling configuration"""
    config = ErrorHandling.ErrorHandlerConfig()
    config.default_log_level = logging.WARNING
    config.error_reporting_enabled = True
    config.error_recovery_enabled = True
    config.circuit_breaker_enabled = True
    config.graceful_degradation_enabled = True
    config.retry_enabled = True
    
    initialize_error_handling(config)

# Auto-initialize
_init_default_error_handling()

# Global exception handler
import sys

class GlobalExceptionHandler:
    """Global exception handler for the entire application"""
    
    def __init__(self):
        self.original_excepthook = sys.excepthook
        self.error_handler = get_global_error_handler()
    
    def __enter__(self):
        sys.excepthook = self._handle_exception
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            self._handle_exception(exc_type, exc_val, exc_tb)
    
    def _handle_exception(self, exc_type, exc_value, exc_traceback):
        """Handle unhandled exceptions"""
        if issubclass(exc_type, NLMException):
            nlm_exception = exc_value
            self.error_handler.handle_error(nlm_exception)
        else:
            # Convert to NLM exception
            nlm_exception = NLMException(
                ErrorCode.SYSTEM_ERROR,
                f"Unhandled exception: {str(exc_value)}",
                cause=exc_value,
                file=exc_traceback.tb_frame.f_code.co_filename,
                line=exc_traceback.tb_lineno,
                function=exc_traceback.tb_frame.f_code.co_name
            )
            self.error_handler.handle_error(nlm_exception)
        
        # Call original handler
        self.original_excepthook(exc_type, exc_value, exc_traceback)

# Usage example
if __name__ == "__main__":
    # Example usage
    try:
        # Test error handling
        Validation.validate_range(15, 0, 10)
    except ValidationException as e:
        print(f"Caught validation error: {e}")
    
    try:
        # Test error recovery
        config = ErrorRecovery.RecoveryConfig(
            strategy=ErrorRecovery.RecoveryStrategy.RETRY,
            max_retries=2
        )
        
        def failing_function():
            raise ValidationException("This function fails")
        
        # This will retry and then fail
        ErrorRecovery.with_retry(config, failing_function)
    except Exception as e:
        print(f"Caught expected error: {e}")
    
    print("Error handling system initialized successfully!")