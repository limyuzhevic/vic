# Error Handling and Validation Utilities for NLM

"""
Comprehensive error handling and validation utilities for the NLM project.

This module provides robust error handling, validation functions, and debugging
tools to ensure reliable operation of the NLM brain simulation system.
"""

import sys
import traceback
from typing import Any, Dict, List, Optional, Union, Callable, TypeVar
from enum import Enum
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler('nlm_errors.log')
    ]
)

logger = logging.getLogger('nlm_validation')

T = TypeVar('T')

class NLMError(Exception):
    """Base exception for all NLM-related errors."""
    def __init__(self, message: str, error_code: Optional[str] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.timestamp = sys.modules['time'].time() if 'time' in sys.modules else 0
    
    def __str__(self) -> str:
        if self.error_code:
            return f"[{self.error_code}] {self.message}"
        return self.message

class ConfigurationError(NLMError):
    """Error in configuration or parameter validation."""
    pass

class SimulationError(NLMError):
    """Error during simulation execution."""
    pass

class MemoryError(NLMError):
    """Error related to memory operations."""
    pass

class ValidationError(NLMError):
    """Error in data validation."""
    pass

class NLMValidator:
    """Comprehensive validation utilities for NLM operations."""
    
    @staticmethod
    def validate_positive(value: float, name: str) -> float:
        """Validate that a value is positive."""
        if value <= 0:
            raise ValidationError(f"{name} must be positive, got {value}")
        return value
    
    @staticmethod
    def validate_range(value: float, min_val: float, max_val: float, name: str) -> float:
        """Validate that a value is within a specific range."""
        if value < min_val or value > max_val:
            raise ValidationError(f"{name} must be in range [{min_val}, {max_val}], got {value}")
        return value
    
    @staticmethod
    def validate_string(value: str, name: str, min_length: int = 0, max_length: int = float('inf')) -> str:
        """Validate string parameters."""
        if not isinstance(value, str):
            raise ValidationError(f"{name} must be a string, got {type(value)}")
        
        if len(value) < min_length:
            raise ValidationError(f"{name} must be at least {min_length} characters, got {len(value)}")
        
        if len(value) > max_length:
            raise ValidationError(f"{name} must be at most {max_length} characters, got {len(value)}")
        
        return value
    
    @staticmethod
    def validate_dict_structure(data: Dict[str, Any], required_keys: List[str], name: str) -> Dict[str, Any]:
        """Validate that a dictionary has required keys."""
        if not isinstance(data, dict):
            raise ValidationError(f"{name} must be a dictionary, got {type(data)}")
        
        for key in required_keys:
            if key not in data:
                raise ValidationError(f"{name} must contain key '{key}'")
        
        return data
    
    @staticmethod
    def validate_array_shape(data: List[Any], expected_length: int, name: str) -> List[Any]:
        """Validate array length."""
        if not isinstance(data, list):
            raise ValidationError(f"{name} must be a list, got {type(data)}")
        
        if len(data) != expected_length:
            raise ValidationError(f"{name} must have length {expected_length}, got {len(data)}")
        
        return data

class NLMErrorHandler:
    """Comprehensive error handling with context and recovery."""
    
    def __init__(self, logger_name: str = 'nlm_handler'):
        self.logger = logging.getLogger(logger_name)
        self.error_history: List[Dict[str, Any]] = []
        self.recovery_actions: Dict[str, Callable] = {}
    
    def handle_error(self, error: Exception, context: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Handle an error with logging and recovery."""
        error_info = {
            'timestamp': sys.modules['time'].time() if 'time' in sys.modules else 0,
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context or {},
            'traceback': traceback.format_exc() if logger.isEnabledFor(logging.DEBUG) else None
        }
        
        # Log the error
        if isinstance(error, NLMError):
            self.logger.error(f"NLM Error: {error}")
        else:
            self.logger.error(f"Unexpected error: {error}")
            self.logger.debug(f"Full traceback: {error_info['traceback']}")
        
        # Store error info
        self.error_history.append(error_info)
        
        # Attempt recovery if available
        if error_info['error_type'] in self.recovery_actions:
            try:
                recovery_result = self.recovery_actions[error_info['error_type']](error_info)
                error_info['recovered'] = True
                error_info['recovery_result'] = recovery_result
                self.logger.info(f"Error recovered successfully: {error_info['error_type']}")
            except Exception as recovery_error:
                error_info['recovered'] = False
                error_info['recovery_error'] = str(recovery_error)
                self.logger.error(f"Recovery failed: {recovery_error}")
        
        return error_info
    
    def register_recovery_action(self, error_type: str, recovery_function: Callable) -> None:
        """Register a recovery action for a specific error type."""
        self.recovery_actions[error_type] = recovery_function
    
    def get_error_history(self, error_type: Optional[str] = None) -> List[Dict[str, Any]]:
        """Get error history, optionally filtered by error type."""
        if error_type:
            return [e for e in self.error_history if e['error_type'] == error_type]
        return self.error_history.copy()
    
    def clear_error_history(self) -> None:
        """Clear error history."""
        self.error_history.clear()

class NLMMonitor:
    """Monitoring and statistics collection for NLM operations."""
    
    def __init__(self):
        self.operation_times: Dict[str, List[float]] = {}
        self.operation_counts: Dict[str, int] = {}
        self.performance_metrics: Dict[str, Any] = {}
    
    def start_operation(self, operation_name: str) -> float:
        """Start timing an operation."""
        import time
        if operation_name not in self.operation_times:
            self.operation_times[operation_name] = []
            self.operation_counts[operation_name] = 0
        
        start_time = time.time()
        self.operation_counts[operation_name] += 1
        return start_time
    
    def end_operation(self, operation_name: str, start_time: float) -> float:
        """End timing an operation and record the duration."""
        import time
        end_time = time.time()
        duration = end_time - start_time
        
        if operation_name in self.operation_times:
            self.operation_times[operation_name].append(duration)
        
        return duration
    
    def get_operation_stats(self, operation_name: str) -> Dict[str, Any]:
        """Get statistics for a specific operation."""
        if operation_name not in self.operation_times:
            return {'count': 0, 'times': []}
        
        times = self.operation_times[operation_name]
        if not times:
            return {'count': self.operation_counts.get(operation_name, 0), 'times': []}
        
        return {
            'count': self.operation_counts.get(operation_name, 0),
            'times': times,
            'min': min(times),
            'max': max(times),
            'mean': sum(times) / len(times),
            'std': (sum((t - sum(times) / len(times)) ** 2 for t in times) / len(times)) ** 0.5
        }
    
    def get_all_stats(self) -> Dict[str, Any]:
        """Get statistics for all operations."""
        stats = {}
        for operation_name in self.operation_times:
            stats[operation_name] = self.get_operation_stats(operation_name)
        return stats

# Global instances
error_handler = NLMErrorHandler()
monitor = NLMMonitor()

# Context manager for operation timing
class OperationTimer:
    """Context manager for timing operations."""
    
    def __init__(self, operation_name: str, monitor_instance: Optional[NLMMonitor] = None):
        self.operation_name = operation_name
        self.monitor_instance = monitor_instance or monitor
        self.start_time = None
    
    def __enter__(self):
        self.start_time = self.monitor_instance.start_operation(self.operation_name)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.start_time is not None:
            self.monitor_instance.end_operation(self.operation_name, self.start_time)
        
        # Handle errors if any
        if exc_type is not None:
            error_handler.handle_error(exc_val, {'operation': self.operation_name})

# Decorator for error handling and monitoring
def nlm_operation(operation_name: str = None):
    """Decorator to add error handling and monitoring to NLM operations."""
    def decorator(func: Callable) -> Callable:
        def wrapper(*args, **kwargs):
            name = operation_name or func.__name__
            
            with OperationTimer(name):
                try:
                    result = func(*args, **kwargs)
                    logger.info(f"Operation {name} completed successfully")
                    return result
                except Exception as e:
                    error_handler.handle_error(e, {'operation': name, 'function': func.__name__})
                    raise
        
        return wrapper
    
    return decorator

# Configuration validation
class ConfigurationValidator:
    """Validation for NLM configuration parameters."""
    
    @staticmethod
    def validate_config_keys(config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate configuration keys and provide defaults."""
        default_config = {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.1,
            'brain.connection_probability': 0.05,
            'brain.initial_weight_mean': 0.5,
            'brain.initial_weight_std': 0.1,
            'brain.v_thresh': -50.0,
            'brain.v_rest': -70.0,
            'brain.v_reset': -75.0,
            'brain.tau_mem': 20.0,
            'brain.tau_ref': 2.0,
            'plasticity.stdp.enable': True,
            'plasticity.stdp.learning_rate': 0.001,
            'plasticity.stdp.tau_plus': 20.0,
            'plasticity.stdp.tau_minus': 20.0,
            'plasticity.hebbian.enable': True,
            'plasticity.hebbian.learning_rate': 0.001,
            'plasticity.structural.enable': True,
            'plasticity.structural.synaptogenesis_rate': 0.0001,
            'plasticity.structural.pruning_rate': 0.00001,
            'neuromod.dopamine.scale': 1.0,
            'neuromod.curiosity.enable': True,
            'neuromod.novelty.enable': True,
            'development.initial_plasticity': 0.5,
            'development.critical_period_length': 60.0,
            'simulation_timestep': 0.001,
            'random_seed': 42
        }
        
        # Merge with defaults
        merged_config = default_config.copy()
        merged_config.update(config)
        
        # Validate specific parameters
        NLMValidator.validate_positive(merged_config['brain.neuron_count'], 'brain.neuron_count')
        NLMValidator.validate_range(merged_config['brain.synapse_density'], 0.0, 1.0, 'brain.synapse_density')
        NLMValidator.validate_range(merged_config['brain.connection_probability'], 0.0, 1.0, 'brain.connection_probability')
        NLMValidator.validate_positive(merged_config['brain.tau_mem'], 'brain.tau_mem')
        NLMValidator.validate_positive(merged_config['brain.tau_ref'], 'brain.tau_ref')
        
        return merged_config

# Utility functions for common validation tasks
def safe_operation(operation: Callable, default_value: Any = None, error_handler_instance: Optional[NLMErrorHandler] = None) -> Any:
    """Execute an operation safely with error handling."""
    handler = error_handler_instance or error_handler
    try:
        return operation()
    except Exception as e:
        handler.handle_error(e, {'operation': 'safe_operation'})
        return default_value

# Export important components
__all__ = [
    'NLMError',
    'ConfigurationError',
    'SimulationError',
    'MemoryError',
    'ValidationError',
    'NLMValidator',
    'NLMErrorHandler',
    'NLMMonitor',
    'OperationTimer',
    'nlm_operation',
    'ConfigurationValidator',
    'safe_operation',
    'error_handler',
    'monitor'
]

print("Error handling and validation utilities loaded successfully")
print(f"Available components: {__all__}")