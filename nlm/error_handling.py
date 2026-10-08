#!/usr/bin/env python3
"""
Enhanced Error Handling for NLM (Neural Learning Machine)

Provides comprehensive error handling with informative messages,
recovery suggestions, and detailed debugging information.
"""

import traceback
import sys
from typing import Dict, Any, Optional, Union, List
from pathlib import Path
import json
import logging

# Set up logging for error handling
logging.basicConfig(
    level=logging.WARNING,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_errors.log'),
        logging.StreamHandler(sys.stderr)
    ]
)

logger = logging.getLogger('nlm.error_handler')

class NLMError(Exception):
    """
    Base exception class for all NLM errors.
    
    Provides detailed error information including recovery suggestions,
    error codes, and context for debugging.
    """
    
    ERROR_CODES = {
        'CONFIG_ERROR': 'Configuration Error',
        'SIMULATION_ERROR': 'Simulation Error',
        'BRAIN_ERROR': 'Brain Initialization Error',
        'MEMORY_ERROR': 'Memory System Error',
        'NEUROMODULATION_ERROR': 'Neuromodulation Error',
        'DEVELOPMENT_ERROR': 'Development Error',
        'PLASTICITY_ERROR': 'Plasticity Error',
        'IO_ERROR': 'Input/Output Error',
        'VALIDATION_ERROR': 'Validation Error',
        'RESOURCE_ERROR': 'Resource Error',
        'UNKNOWN_ERROR': 'Unknown Error'
    }
    
    def __init__(self, message: str, error_code: str = 'UNKNOWN_ERROR', 
                 suggestions: Optional[List[str]] = None, 
                 context: Optional[Dict[str, Any]] = None):
        """
        Initialize NLM error.
        
        Args:
            message: Error message
            error_code: Error code from ERROR_CODES
            suggestions: List of recovery suggestions
            context: Additional context information
        """
        self.message = message
        self.error_code = error_code
        self.suggestions = suggestions or self._get_default_suggestions()
        self.context = context or {}
        self.timestamp = None
        
        # Log the error
        logger.error(f"{error_code}: {message}")
        
        # Create formatted message
        super().__init__(self._format_message())
    
    def _format_message(self) -> str:
        """Format error message with all details"""
        lines = []
        lines.append(f"[NLM {self.error_code}] {self.message}")
        lines.append("")
        
        if self.context:
            lines.append("Context Information:")
            for key, value in self.context.items():
                lines.append(f"  {key}: {value}")
            lines.append("")
        
        if self.suggestions:
            lines.append("Recovery Suggestions:")
            for i, suggestion in enumerate(self.suggestions, 1):
                lines.append(f"  {i}. {suggestion}")
            lines.append("")
        
        return "\n".join(lines)
    
    def _get_default_suggestions(self) -> List[str]:
        """Get default recovery suggestions based on error code"""
        suggestions_map = {
            'CONFIG_ERROR': [
                "Check configuration file syntax",
                "Ensure all required fields are present",
                "Verify configuration values are within valid ranges",
                "Use 'nlm-config validate <file>' to check configuration"
            ],
            'SIMULATION_ERROR': [
                "Reduce simulation step count or neuron count",
                "Check for infinite loops in simulation logic",
                "Verify hardware resources are sufficient",
                "Try running with a smaller configuration"
            ],
            'BRAIN_ERROR': [
                "Check configuration for valid neuron and region counts",
                "Ensure random seed is valid",
                "Verify all brain systems are properly initialized",
                "Try resetting and reinitializing the brain"
            ],
            'MEMORY_ERROR': [
                "Increase memory system capacity in configuration",
                "Check for memory leaks in simulation",
                "Try reducing simulation duration",
                "Clear memory systems and restart simulation"
            ],
            'NEUROMODULATION_ERROR': [
                "Check neuromodulation parameter values",
                "Verify neuromodulator initialization",
                "Try disabling neuromodulation temporarily",
                "Check for conflicting neuromodulation signals"
            ],
            'DEVELOPMENT_ERROR': [
                "Check developmental stage parameters",
                "Verify development system configuration",
                "Try disabling development system",
                "Check for developmental conflicts"
            ],
            'PLASTICITY_ERROR': [
                "Check plasticity parameter values",
                "Verify plasticity rule configurations",
                "Try disabling plasticity temporarily",
                "Check for conflicting plasticity rules"
            ],
            'IO_ERROR': [
                "Check file permissions and disk space",
                "Verify file paths are correct",
                "Ensure input data format is valid",
                "Check network connectivity if applicable"
            ],
            'VALIDATION_ERROR': [
                "Review validation rules",
                "Check input data ranges and formats",
                "Use validation tools to identify problematic fields",
                "Clean and reformat input data"
            ],
            'RESOURCE_ERROR': [
                "Reduce simulation size (neurons, regions, steps)",
                "Check system memory and disk space",
                "Try running on a system with more resources",
                "Use resource monitoring tools"
            ],
        }
        
        return suggestions_map.get(self.error_code, [
            "Check error message for details",
            "Review configuration and simulation parameters",
            "Try reducing simulation complexity",
            "Consult NLM documentation for common issues"
        ])


class ConfigurationError(NLMError):
    """Error in configuration handling"""
    
    def __init__(self, message: str, error_code: str = 'CONFIG_ERROR',
                 suggestions: Optional[List[str]] = None,
                 context: Optional[Dict[str, Any]] = None):
        super().__init__(message, error_code, suggestions, context)


class SimulationError(NLMError):
    """Error in simulation execution"""
    
    def __init__(self, message: str, error_code: str = 'SIMULATION_ERROR',
                 suggestions: Optional[List[str]] = None,
                 context: Optional[Dict[str, Any]] = None):
        super().__init__(message, error_code, suggestions, context)


class BrainError(NLMError):
    """Error in brain initialization or operation"""
    
    def __init__(self, message: str, error_code: str = 'BRAIN_ERROR',
                 suggestions: Optional[List[str]] = None,
                 context: Optional[Dict[str, Any]] = None):
        super().__init__(message, error_code, suggestions, context)


class ErrorHandler:
    """
    Centralized error handling system for NLM.
    
    Provides methods for catching, logging, and recovering from errors,
    with support for different error levels and recovery strategies.
    """
    
    def __init__(self, error_log_file: str = "nlm_errors.log"):
        """
        Initialize error handler.
        
        Args:
            error_log_file: Path to error log file
        """
        self.error_log_file = Path(error_log_file)
        self.error_counts = {}
        self.recovery_attempts = {}
        
        # Ensure error log directory exists
        self.error_log_file.parent.mkdir(exist_ok=True)
    
    def handle_error(self, error: Exception, error_code: str = 'UNKNOWN_ERROR',
                    context: Optional[Dict[str, Any]] = None,
                    recovery_strategy: str = 'log_and_reraise') -> Optional[Exception]:
        """
        Handle an error with appropriate recovery strategy.
        
        Args:
            error: The error to handle
            error_code: Error code for classification
            context: Additional context information
            recovery_strategy: Recovery strategy ('log_and_reraise', 'recover', 'ignore')
            
        Returns:
            Recovered exception if recovery was attempted, None otherwise
        """
        # Log error details
        error_id = f"{error_code}_{len(self.error_counts) + 1}"
        self.error_counts[error_id] = self.error_counts.get(error_id, 0) + 1
        
        # Create context with additional information
        full_context = {
            'error_id': error_id,
            'error_type': type(error).__name__,
            'timestamp': None,  # Would use actual timestamp here
            **context if context else {}
        }
        
        # Create appropriate error based on error code
        if error_code == 'CONFIG_ERROR':
            nlm_error = ConfigurationError(str(error), error_code, context=full_context)
        elif error_code == 'SIMULATION_ERROR':
            nlm_error = SimulationError(str(error), error_code, context=full_context)
        elif error_code == 'BRAIN_ERROR':
            nlm_error = BrainError(str(error), error_code, context=full_context)
        else:
            nlm_error = NLMError(str(error), error_code, context=full_context)
        
        # Log stack trace for debugging
        if isinstance(error, NLMError):
            logger.error(f"NLM Error {error_id}: {error.message}")
        else:
            logger.error(f"Caught exception {error_id}: {str(error)}")
            logger.debug(traceback.format_exc())
        
        # Apply recovery strategy
        if recovery_strategy == 'recover':
            return self._attempt_recovery(nlm_error)
        elif recovery_strategy == 'ignore':
            logger.warning(f"Ignoring error {error_id}: {error}")
            return None
        else:  # log_and_reraise (default)
            logger.error(f"Re-raising error {error_id}: {error}")
            raise nlm_error from error
    
    def _attempt_recovery(self, error: NLMError) -> Optional[Exception]:
        """
        Attempt to recover from an error.
        
        Args:
            error: The error to recover from
            
        Returns:
            Recovered exception if recovery failed, None if successful
        """
        error_id = error.error_code
        
        # Check if we've already attempted recovery recently
        if error_id in self.recovery_attempts:
            attempts = self.recovery_attempts[error_id]
            if attempts >= 3:  # Max 3 recovery attempts
                logger.error(f"Max recovery attempts reached for {error_id}")
                return error
        
        # Attempt recovery based on error type
        if error.error_code == 'CONFIG_ERROR':
            # Try to use default configuration
            logger.info("Attempting recovery with default configuration")
            # This would normally restore default configuration
            # For now, just log the attempt
            
        elif error.error_code == 'SIMULATION_ERROR':
            # Try to reduce simulation complexity
            logger.info("Attempting recovery by reducing simulation complexity")
            # This would normally reduce simulation parameters
            # For now, just log the attempt
        
        elif error.error_code == 'BRAIN_ERROR':
            # Try to reinitialize the brain
            logger.info("Attempting recovery by reinitializing brain")
            # This would normally reinitialize the brain
            # For now, just log the attempt
        
        # Update recovery attempts counter
        self.recovery_attempts[error_id] = self.recovery_attempts.get(error_id, 0) + 1
        
        # In most cases, recovery is not possible, so return the error
        # to indicate that the operation should be retried
        logger.warning(f"Recovery attempt {self.recovery_attempts[error_id]} for {error.error_code}")
        return error
    
    def get_error_statistics(self) -> Dict[str, Any]:
        """
        Get statistics about errors encountered.
        
        Returns:
            Dictionary with error statistics
        """
        return {
            'total_errors': sum(self.error_counts.values()),
            'error_by_id': self.error_counts,
            'recovery_attempts': self.recovery_attempts,
            'unique_error_types': len(set(self.error_counts.keys())),
        }
    
    def reset_statistics(self) -> None:
        """Reset error statistics"""
        self.error_counts.clear()
        self.recovery_attempts.clear()
    
    def export_error_log(self, output_file: Optional[str] = None) -> str:
        """
        Export error log to file.
        
        Args:
            output_file: Path to output file. If None, uses error_log_file
            
        Returns:
            Path to exported log file
        """
        if output_file is None:
            output_file = self.error_log_file
        
        # Copy error log to output file
        if self.error_log_file.exists():
            with open(self.error_log_file, 'r') as f_in, open(output_file, 'w') as f_out:
                f_out.write(f_in.read())
        
        return output_file


class ErrorContext:
    """
    Context manager for error handling in specific code blocks.
    
    Provides automatic error handling with recovery strategies
    for specific code sections.
    """
    
    def __init__(self, handler: ErrorHandler, error_code: str,
                 recovery_strategy: str = 'log_and_reraise',
                 context: Optional[Dict[str, Any]] = None):
        """
        Initialize error context.
        
        Args:
            handler: Error handler instance
            error_code: Error code for this context
            recovery_strategy: Recovery strategy
            context: Additional context information
        """
        self.handler = handler
        self.error_code = error_code
        self.recovery_strategy = recovery_strategy
        self.context = context or {}
    
    def __enter__(self):
        """Enter error context"""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit error context and handle exception"""
        if exc_type is not None:
            return self.handler.handle_error(
                exc_val, self.error_code, self.context, self.recovery_strategy
            ) is not None
        return False


# Global error handler instance
_global_error_handler = ErrorHandler()


def get_error_handler() -> ErrorHandler:
    """
    Get the global error handler instance.
    
    Returns:
        Global error handler instance
    """
    return _global_error_handler


def handle_error(error: Exception, error_code: str = 'UNKNOWN_ERROR',
                 context: Optional[Dict[str, Any]] = None,
                 recovery_strategy: str = 'log_and_reraise') -> Optional[Exception]:
    """
    Global function to handle errors using the global error handler.
    
    Args:
        error: The error to handle
        error_code: Error code for classification
        context: Additional context information
        recovery_strategy: Recovery strategy
        
    Returns:
        Recovered exception if recovery was attempted, None otherwise
    """
    return _global_error_handler.handle_error(
        error, error_code, context, recovery_strategy
    )


def create_error_context(error_code: str, recovery_strategy: str = 'log_and_reraise',
                         context: Optional[Dict[str, Any]] = None) -> ErrorContext:
    """
    Create an error context for a specific code block.
    
    Args:
        error_code: Error code for this context
        recovery_strategy: Recovery strategy
        context: Additional context information
        
    Returns:
        Error context manager
    """
    return ErrorContext(_global_error_handler, error_code, recovery_strategy, context)


def format_error_for_display(error: Exception) -> str:
    """
    Format error for user-friendly display.
    
    Args:
        error: The error to format
        
    Returns:
        Formatted error string
    """
    if isinstance(error, NLMError):
        return str(error)
    else:
        # For non-NLM errors, provide a simpler format
        return f"Error: {str(error)}\n\nFor more information, check the error log file: nlm_errors.log"


# Utility functions for common error scenarios
def validate_configuration(config: Dict[str, Any]) -> bool:
    """
    Validate configuration with enhanced error handling.
    
    Args:
        config: Configuration dictionary to validate
        
    Returns:
        True if configuration is valid, False otherwise
    """
    try:
        if not isinstance(config, dict):
            raise ConfigurationError(
                "Configuration must be a dictionary",
                'CONFIG_ERROR',
                ["Ensure configuration is provided as a dictionary",
                 "Check configuration file format"]
            )
        
        # Check required fields
        required_fields = ['neuron_count', 'region_count', 'connection_probability']
        for field in required_fields:
            if field not in config:
                raise ConfigurationError(
                    f"Missing required configuration field: {field}",
                    'CONFIG_ERROR',
                    [f"Add the required field '{field}' to configuration",
                     "Use 'nlm-config create' to create a proper configuration template"]
                )
        
        # Validate field types and ranges
        if not isinstance(config['neuron_count'], int) or config['neuron_count'] <= 0:
            raise ConfigurationError(
                "neuron_count must be a positive integer",
                'CONFIG_ERROR',
                ["Set neuron_count to a positive integer (e.g., 1000)",
                 "Adjust configuration for simulation complexity"]
            )
        
        if not isinstance(config['region_count'], int) or config['region_count'] <= 0:
            raise ConfigurationError(
                "region_count must be a positive integer",
                'CONFIG_ERROR',
                ["Set region_count to a positive integer (e.g., 2)",
                 "A single region may be simpler for debugging"]
            )
        
        if not (0.0 <= config['connection_probability'] <= 1.0):
            raise ConfigurationError(
                "connection_probability must be between 0.0 and 1.0",
                'CONFIG_ERROR',
                ["Set connection_probability to a value between 0.0 and 1.0",
                 "Try 0.1 for low connectivity or 0.2 for higher connectivity"]
            )
        
        return True
        
    except (ConfigurationError, NLMError):
        raise
    except Exception as e:
        raise ConfigurationError(
            f"Unexpected error validating configuration: {str(e)}",
            'CONFIG_ERROR',
            ["Check configuration format",
             "Ensure all configuration values are properly typed",
             "Use 'nlm-config validate' to check configuration syntax"]
        ) from e


def safe_simulation_step(step_func, *args, **kwargs) -> bool:
    """
    Safely execute a simulation step with error handling.
    
    Args:
        step_func: Function to execute (e.g., brain.step)
        *args: Arguments to pass to step_func
        **kwargs: Keyword arguments to pass to step_func
        
    Returns:
        True if step completed successfully, False if error occurred
    """
    try:
        step_func(*args, **kwargs)
        return True
    except SimulationError as e:
        logger.error(f"Simulation error during step: {e}")
        return False
    except Exception as e:
        logger.error(f"Unexpected error during simulation step: {e}")
        return False


# Export commonly used error classes and functions
__all__ = [
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'BrainError',
    'ErrorHandler',
    'ErrorContext',
    'get_error_handler',
    'handle_error',
    'create_error_context',
    'format_error_for_display',
    'validate_configuration',
    'safe_simulation_step',
]
