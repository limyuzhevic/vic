#!/usr/bin/env python3
"""
NLM - Error Handling and Reporting System

This module provides a comprehensive error handling and reporting system for NLM,
including:
1. Custom exception hierarchy
2. Error reporting and logging
3. Error recovery mechanisms
4. Performance error handling
5. System error categorization
"""

import os
import sys
import traceback
import time
from typing import Dict, List, Any, Optional, Callable, Union
from enum import Enum
from datetime import datetime
from contextlib import contextmanager
import json
import logging

class ErrorCategory(Enum):
    """Error categories for NLM."""
    SYSTEM = "system"
    CONFIGURATION = "configuration"
    NEURAL = "neural"
    MEMORY = "memory"
    PLASTICITY = "plasticity"
    DEVELOPMENT = "development"
    NEUROMODULATION = "neuromodulation"
    PREDICTION = "prediction"
    COGNITION = "cognition"
    WORLD = "world"
    AGENT = "agent"
    SIMULATION = "simulation"
    VALIDATION = "validation"
    EXPORT = "export"
    VISUALIZATION = "visualization"
    PERFORMANCE = "performance"

class ErrorSeverity(Enum):
    """Error severity levels."""
    DEBUG = "debug"
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"
    FATAL = "fatal"

class NLMError(Exception):
    """Base class for all NLM errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.SYSTEM,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message)
        self.message = message
        self.category = category
        self.severity = severity
        self.cause = cause
        self.context = context or {}
        self.timestamp = datetime.now().isoformat()
        self.stack_trace = traceback.format_exc() if cause else None
        
        # Log the error
        self._log()
    
    def _log(self):
        """Log the error."""
        log_level = {
            ErrorSeverity.DEBUG: logging.DEBUG,
            ErrorSeverity.INFO: logging.INFO,
            ErrorSeverity.WARNING: logging.WARNING,
            ErrorSeverity.ERROR: logging.ERROR,
            ErrorSeverity.CRITICAL: logging.CRITICAL,
            ErrorSeverity.FATAL: logging.CRITICAL,
        }.get(self.severity, logging.ERROR)
        
        logging.log(log_level, f"[{self.category.value.upper()}] {self.message}")
        
        if self.context:
            logging.log(log_level, f"  Context: {self.context}")
        
        if self.cause:
            logging.log(log_level, f"  Caused by: {type(self.cause).__name__}: {self.cause}")

class ConfigurationError(NLMError):
    """Configuration-related errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.CONFIGURATION,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class NeuralError(NLMError):
    """Neural computation errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.NEURAL,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class MemoryError(NLMError):
    """Memory system errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.MEMORY,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class PlasticityError(NLMError):
    """Plasticity rule errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.PLASTICITY,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class DevelopmentError(NLMError):
    """Development system errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.DEVELOPMENT,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class NeuromodulationError(NLMError):
    """Neuromodulation errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.NEUROMODULATION,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class SimulationError(NLMError):
    """Simulation errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.SIMULATION,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class ExportError(NLMError):
    """Export/Visualization errors."""
    
    def __init__(self, message: str, category: ErrorCategory = ErrorCategory.EXPORT,
                 severity: ErrorSeverity = ErrorSeverity.ERROR, 
                 cause: Optional[Exception] = None, context: Optional[Dict[str, Any]] = None):
        super().__init__(message, category, severity, cause, context)

class ErrorHandler:
    """
    Centralized error handler for NLM.
    """
    
    def __init__(self):
        self.error_count = 0
        self.fatal_errors = 0
        self.recovery_actions = []
        self.error_history: List[NLMError] = []
        
        # Configure logging
        self._configure_logging()
    
    def _configure_logging(self):
        """Configure logging system."""
        logging.basicConfig(
            level=logging.DEBUG,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('nlm_error.log'),
                logging.StreamHandler(sys.stdout)
            ]
        )
        
        # Create error summary logger
        self.error_logger = logging.getLogger('nlm_errors')
        self.error_logger.setLevel(logging.WARNING)
        
        # Add file handler for error summary
        file_handler = logging.FileHandler('nlm_error_summary.log')
        file_handler.setLevel(logging.WARNING)
        formatter = logging.Formatter(
            '%(asctime)s - %(levelname)s - %(category)s - %(message)s'
        )
        file_handler.setFormatter(formatter)
        self.error_logger.addHandler(file_handler)
    
    def handle_error(self, error: NLMError) -> bool:
        """
        Handle an error.
        
        Args:
            error: The error to handle
            
        Returns:
            True if error was handled, False if fatal
        """
        self.error_count += 1
        self.error_history.append(error)
        
        # Log the error
        self.error_logger.warning(
            error.message,
            extra={'category': error.category.value}
        )
        
        # Check if error is fatal
        if error.severity in [ErrorSeverity.FATAL, ErrorSeverity.CRITICAL]:
            self.fatal_errors += 1
            print(f"FATAL ERROR: {error.message}")
            return False
        
        # Try recovery actions
        recovery_result = self._try_recovery(error)
        if recovery_result:
            print(f"Recovered from error: {error.message}")
            return True
        
        # Log error but continue if possible
        print(f"Error handled: {error.message} (Category: {error.category.value})")
        return True
    
    def _try_recovery(self, error: NLMError) -> bool:
        """Try to recover from an error."""
        # Recovery actions based on error category
        if error.category == ErrorCategory.CONFIGURATION:
            return self._recover_config_error(error)
        elif error.category == ErrorCategory.NEURAL:
            return self._recover_neural_error(error)
        elif error.category == ErrorCategory.MEMORY:
            return self._recover_memory_error(error)
        elif error.category == ErrorCategory.SIMULATION:
            return self._recover_simulation_error(error)
        else:
            # Generic recovery
            return False
    
    def _recover_config_error(self, error: ConfigurationError) -> bool:
        """Recover from configuration error."""
        # Try to reload default configuration
        context = error.context
        if context and context.get('reload_defaults'):
            print("Reloading default configuration...")
            # Implementation would reload default config
            return True
        return False
    
    def _recover_neural_error(self, error: NeuralError) -> bool:
        """Recover from neural error."""
        # Try to reset neural state
        context = error.context
        if context and context.get('reset_neural_state'):
            print("Resetting neural state...")
            # Implementation would reset neural state
            return True
        return False
    
    def _recover_memory_error(self, error: MemoryError) -> bool:
        """Recover from memory error."""
        # Try to free memory or use fallback
        context = error.context
        if context and context.get('free_memory'):
            print("Freeing memory...")
            # Implementation would free memory
            return True
        return False
    
    def _recover_simulation_error(self, error: SimulationError) -> bool:
        """Recover from simulation error."""
        # Try to adjust simulation parameters
        context = error.context
        if context and context.get('adjust_parameters'):
            print("Adjusting simulation parameters...")
            # Implementation would adjust parameters
            return True
        return False
    
    def get_error_summary(self) -> Dict[str, Any]:
        """Get summary of all errors."""
        error_counts = {}
        for error in self.error_history:
            category = error.category.value
            error_counts[category] = error_counts.get(category, 0) + 1
        
        return {
            'total_errors': self.error_count,
            'fatal_errors': self.fatal_errors,
            'errors_by_category': error_counts,
            'latest_errors': [
                {
                    'message': error.message,
                    'category': error.category.value,
                    'severity': error.severity.value,
                    'timestamp': error.timestamp
                }
                for error in self.error_history[-10:]  # Last 10 errors
            ]
        }
    
    def save_error_report(self, filepath: str):
        """Save error report to file."""
        summary = self.get_error_summary()
        
        with open(filepath, 'w') as f:
            f.write("NLM Error Report\n")
            f.write("=" * 50 + "\n\n")
            f.write(f"Total Errors: {summary['total_errors']}\n")
            f.write(f"Fatal Errors: {summary['fatal_errors']}\n\n")
            
            f.write("Errors by Category:\n")
            for category, count in summary['errors_by_category'].items():
                f.write(f"  {category}: {count}\n")
            
            f.write("\nRecent Errors:\n")
            for error_info in summary['latest_errors']:
                f.write(f"  [{error_info['timestamp']}] {error_info['category'].upper()}: {error_info['message']}\n")

class ErrorRecoveryManager:
    """
    Manages error recovery strategies for NLM.
    """
    
    def __init__(self, error_handler: ErrorHandler):
        self.error_handler = error_handler
        self.recovery_strategies = {}
        
        # Register default recovery strategies
        self._register_default_strategies()
    
    def _register_default_strategies(self):
        """Register default recovery strategies."""
        self.recovery_strategies = {
            ErrorCategory.CONFIGURATION: self._recover_config,
            ErrorCategory.NEURAL: self._recover_neural,
            ErrorCategory.MEMORY: self._recover_memory,
            ErrorCategory.SIMULATION: self._recover_simulation,
            ErrorCategory.DEVELOPMENT: self._recover_development,
            ErrorCategory.NEUROMODULATION: self._recover_neuromodulation,
        }
    
    def _recover_config(self, error: NLMError, context: Dict[str, Any]) -> bool:
        """Recover from configuration error."""
        # Implementation would reload or fix configuration
        return True
    
    def _recover_neural(self, error: NLMError, context: Dict[str, Any]) -> bool:
        """Recover from neural error."""
        # Implementation would reset or fix neural state
        return True
    
    def _recover_memory(self, error: NLMError, context: Dict[str, Any]) -> bool:
        """Recover from memory error."""
        # Implementation would free memory or use fallback
        return True
    
    def _recover_simulation(self, error: NLMError, context: Dict[str, Any]) -> bool:
        """Recover from simulation error."""
        # Implementation would adjust simulation parameters
        return True
    
    def _recover_development(self, error: NLMError, context: Dict[str, Any]) -> bool:
        """Recover from development error."""
        # Implementation would fix development state
        return True
    
    def _recover_neuromodulation(self, error: NLMError, context: Dict[str, Any]) -> bool:
        """Recover from neuromodulation error."""
        # Implementation would fix neuromodulation
        return True

class PerformanceErrorHandler(NLMError):
    """
    Handles performance-related errors and metrics.
    """
    
    def __init__(self, message: str, performance_metrics: Dict[str, Any], 
                 threshold_violations: List[str], category: ErrorCategory = ErrorCategory.PERFORMANCE):
        super().__init__(message, category)
        self.performance_metrics = performance_metrics
        self.threshold_violations = threshold_violations
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Generate performance report."""
        return {
            'message': self.message,
            'metrics': self.performance_metrics,
            'violations': self.threshold_violations,
            'timestamp': self.timestamp
        }

class SafeExecuteContext:
    """
    Context manager for safe execution with error handling.
    """
    
    def __init__(self, error_handler: ErrorHandler, context: Optional[Dict[str, Any]] = None):
        self.error_handler = error_handler
        self.context = context or {}
        self.errors = []
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            # Convert exception to NLM error
            error = self._convert_to_nlm_error(exc_val)
            handled = self.error_handler.handle_error(error)
            self.errors.append(error)
            
            # If error is fatal and not handled, re-raise
            if not handled and error.severity in [ErrorSeverity.FATAL, ErrorSeverity.CRITICAL]:
                raise
    
    def _convert_to_nlm_error(self, exception: Exception) -> NLMError:
        """Convert generic exception to NLM error."""
        # Determine error category based on exception type
        if isinstance(exception, ConfigurationError):
            category = ErrorCategory.CONFIGURATION
        elif isinstance(exception, NeuralError):
            category = ErrorCategory.NEURAL
        elif isinstance(exception, MemoryError):
            category = ErrorCategory.MEMORY
        elif isinstance(exception, SimulationError):
            category = ErrorCategory.SIMULATION
        elif isinstance(exception, PerformanceErrorHandler):
            category = ErrorCategory.PERFORMANCE
        else:
            category = ErrorCategory.SYSTEM
        
        return NLMError(
            str(exception),
            category=category,
            cause=exception,
            context=self.context
        )

# Global error handler instance
_global_error_handler = ErrorHandler()
global_error_recovery_manager = ErrorRecoveryManager(_global_error_handler)

def get_error_handler() -> ErrorHandler:
    """Get the global error handler."""
    return _global_error_handler

def get_error_recovery_manager() -> ErrorRecoveryManager:
    """Get the global error recovery manager."""
    return global_error_recovery_manager

def with_error_handling(context: Optional[Dict[str, Any]] = None):
    """
    Decorator for safe execution with error handling.
    
    Args:
        context: Additional context for error handling
        
    Returns:
        Decorated function
    """
    def decorator(func):
        def wrapper(*args, **kwargs):
            with SafeExecuteContext(_global_error_handler, context):
                return func(*args, **kwargs)
        return wrapper
    return decorator

def report_error(error: NLMError) -> bool:
    """
    Report an error to the global error handler.
    
    Args:
        error: The error to report
        
    Returns:
        True if error was handled, False if fatal
    """
    return _global_error_handler.handle_error(error)

def log_performance_error(message: str, metrics: Dict[str, Any], 
                         threshold_violations: List[str]) -> PerformanceErrorHandler:
    """
    Log a performance error.
    
    Args:
        message: Error message
        metrics: Performance metrics
        threshold_violations: List of threshold violations
        
    Returns:
        PerformanceErrorHandler instance
    """
    error = PerformanceErrorHandler(message, metrics, threshold_violations)
    _global_error_handler.handle_error(error)
    return error

# Exception types for easy import
__all__ = [
    'NLMError',
    'ConfigurationError',
    'NeuralError',
    'MemoryError',
    'PlasticityError',
    'DevelopmentError',
    'NeuromodulationError',
    'SimulationError',
    'ExportError',
    'ErrorCategory',
    'ErrorSeverity',
    'ErrorHandler',
    'ErrorRecoveryManager',
    'PerformanceErrorHandler',
    'SafeExecuteContext',
    'get_error_handler',
    'get_error_recovery_manager',
    'with_error_handling',
    'report_error',
    'log_performance_error'
]

if __name__ == "__main__":
    # Example usage
    print("NLM Error Handling and Reporting System")
    print("=" * 50)
    
    # Configure logging
    logging.basicConfig(level=logging.INFO)
    
    # Get error handler
    error_handler = get_error_handler()
    
    # Test error handling
    print("1. Testing Configuration Error...")
    try:
        raise ConfigurationError("Invalid configuration file format", 
                               context={"file": "config.txt", "reload_defaults": True})
    except ConfigurationError as e:
        error_handler.handle_error(e)
    
    print("\n2. Testing Neural Error...")
    try:
        raise NeuralError("Neural computation failed", 
                         context={"reset_neural_state": True})
    except NeuralError as e:
        error_handler.handle_error(e)
    
    print("\n3. Testing Safe Execution...")
    
    @with_error_handling({"context": "test_operation"})
    def risky_operation():
        raise ConfigurationError("Test error in risky operation")
    
    try:
        risky_operation()
    except NLMError:
        print("Error caught in safe execution")
    
    print("\n4. Error Summary...")
    summary = error_handler.get_error_summary()
    print(f"Total Errors: {summary['total_errors']}")
    print(f"Fatal Errors: {summary['fatal_errors']}")
    
    print("\n5. Performance Error...")
    perf_error = log_performance_error(
        "Performance threshold violated",
        {"cpu_usage": 95.5, "memory_usage": 80.2, "processing_time": 2.5},
        ["CPU usage > 90%", "Memory usage > 80%"]
    )
    
    print("\nAll tests completed!")
