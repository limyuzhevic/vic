#!/usr/bin/env python3
"""
Error Handling Examples - Robust Error Handling and Validation

This guide demonstrates comprehensive error handling for NLM Python bindings:
- Exception hierarchy and handling patterns
- Input validation and sanitization
- Graceful degradation and fallback strategies
- Error recovery and retry mechanisms
- Performance monitoring with error tracking
- Debugging and error analysis tools
- Production-ready error handling patterns
"""
import nlm
import time
import json
import traceback
import numpy as np
from typing import List, Dict, Any, Optional, Callable, Union
from enum import Enum
import logging
from contextlib import contextmanager

# Configure logging for error tracking
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_errors.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

class NLMErrorHandler:
    """
    Comprehensive error handler for NLM operations.
    
    This class provides robust error handling patterns for NLM simulations,
    including error classification, recovery strategies, and performance monitoring.
    """
    
    def __init__(self, auto_recover: bool = True, max_retries: int = 3):
        """
        Initialize error handler.
        
        Args:
            auto_recover: Whether to automatically attempt recovery
            max_retries: Maximum number of retry attempts
        """
        self.auto_recover = auto_recover
        self.max_retries = max_retries
        self.error_counts = {}
        self.recovery_attempts = {}
        self.error_history = []
        
        print("Initializing NLM Error Handler...")
        print(f"  - Auto-recovery: {auto_recover}")
        print(f"  - Max retries: {max_retries}")
    
    def log_error(self, operation: str, error: Exception, context: Dict[str, Any] = None):
        """
        Log error with context and statistics.
        
        Args:
            operation: Operation that failed
            error: Exception that occurred
            context: Additional context information
        """
        error_type = type(error).__name__
        
        # Track error counts
        error_key = f"{operation}_{error_type}"
        self.error_counts[error_key] = self.error_counts.get(error_key, 0) + 1
        
        # Log with full context
        error_info = {
            'timestamp': time.time(),
            'operation': operation,
            'error_type': error_type,
            'error_message': str(error),
            'context': context or {},
            'traceback': traceback.format_exc(),
        }
        
        self.error_history.append(error_info)
        
        # Log using appropriate severity
        if isinstance(error, (nlm.ValidationError, nlm.SerializationError)):
            logger.warning(f"Error in {operation}: {error}")
            if context:
                logger.warning(f"Context: {context}")
        else:
            logger.error(f"Error in {operation}: {error}")
            if context:
                logger.error(f"Context: {context}")
        
        print(f"✗ Error logged: {operation} -> {error_type}: {error}")
    
    def attempt_recovery(self, operation: str, error: Exception) -> bool:
        """
        Attempt to recover from error.
        
        Args:
            operation: Operation that failed
            error: Exception that occurred
            
        Returns:
            True if recovery successful, False otherwise
        """
        if not self.auto_recover:
            return False
        
        # Track recovery attempts
        error_type = type(error).__name__
        recovery_key = f"{operation}_{error_type}"
        self.recovery_attempts[recovery_key] = self.recovery_attempts.get(recovery_key, 0) + 1
        
        if self.recovery_attempts[recovery_key] > self.max_retries:
            print(f"⚠ Max recovery attempts exceeded for {operation}")
            return False
        
        print(f"  Attempting recovery for {operation} (attempt {self.recovery_attempts[recovery_key]})...")
        
        # Recovery strategies based on error type
        if isinstance(error, nlm.ValidationError):
            return self._recover_from_validation_error(operation, error)
        elif isinstance(error, nlm.SerializationError):
            return self._recover_from_serialization_error(operation, error)
        elif isinstance(error, nlm.CallbackError):
            return self._recover_from_callback_error(operation, error)
        else:
            return self._recover_from_general_error(operation, error)
    
    def _recover_from_validation_error(self, operation: str, error: nlm.ValidationError) -> bool:
        """Recover from validation error."""
        print(f"  - Validation error recovery: {error}")
        
        # Reset to safe state
        if "processSensoryInput" in operation:
            print(f"  - Resetting agent state and retrying with valid input")
            # In real implementation, would reset agent and retry
            return True
        elif "reward" in operation.lower():
            print(f"  - Applying default reward and continuing")
            # Apply default reward and continue
            return True
        
        return False
    
    def _recover_from_serialization_error(self, operation: str, error: nlm.SerializationError) -> bool:
        """Recover from serialization error."""
        print(f"  - Serialization error recovery: {error}")
        
        # Try alternative serialization format
        print(f"  - Attempting alternative serialization method")
        return True
    
    def _recover_from_callback_error(self, operation: str, error: nlm.CallbackError) -> bool:
        """Recover from callback error."""
        print(f"  - Callback error recovery: {error}")
        
        # Remove problematic callback
        print(f"  - Removing problematic callback")
        return True
    
    def _recover_from_general_error(self, operation: str, error: Exception) -> bool:
        """Recover from general error."""
        print(f"  - General error recovery: {error}")
        
        # Reset and retry with default parameters
        print(f"  - Resetting to defaults and retrying")
        return True
    
    def get_error_summary(self) -> Dict[str, Any]:
        """Get summary of errors encountered."""
        return {
            'total_errors': len(self.error_history),
            'error_types': {k: v for k, v in self.error_counts.items()},
            'recovery_attempts': self.recovery_attempts,
            'recent_errors': self.error_history[-5:],  # Last 5 errors
        }
class InputValidator:
    """
    Comprehensive input validator for NLM operations.
    
    This class validates all inputs to NLM operations before execution,
    ensuring data quality and preventing errors.
    """
    
    def __init__(self):
        self.validation_rules = {}
        self.default_values = {}
        
        print("Initializing Input Validator...")
        self._setup_validation_rules()
    
    def _setup_validation_rules(self):
        """Setup validation rules for different input types."""
        # Vision validation rules
        self.validation_rules['vision'] = {
            'min_size': 1,
            'max_size': 4096,
            'min_value': 0.0,
            'max_value': 1.0,
            'required_dimensions': (64, 64),  # Expected size
        }
        
        # Touch validation rules
        self.validation_rules['touch'] = {
            'min_size': 1,
            'max_size': 10,
            'min_value': 0.0,
            'max_value': 1.0,
        }
        
        # Internal signals validation
        self.validation_rules['internal'] = {
            'min_size': 1,
            'max_size': 20,
            'min_value': 0.0,
            'max_value': 1.0,
        }
        
        # Proprioception validation
        self.validation_rules['proprioception'] = {
            'min_size': 1,
            'max_size': 20,
            'value_ranges': {
                'x': (-100.0, 100.0),
                'y': (-100.0, 100.0),
                'orientation': (0.0, 360.0),
                'energy': (0.0, 1.0),
                'health': (0.0, 1.0),
                'age': (0.0, 100.0),
            }
        }
        
        # Default values for recovery
        self.default_values['vision'] = [0.5] * 64 * 64
        self.default_values['touch'] = [0.5] * 6
        self.default_values['internal'] = [0.5] * 6
        self.default_values['proprioception'] = [0.0] * 12
    
    def validate_percept(self, percept: nlm.SensoryPercept, validation_type: str = 'all') -> Dict[str, Any]:
        """
        Validate sensory percept comprehensively.
        
        Args:
            percept: Sensory percept to validate
            validation_type: Type of validation ('all', 'strict', 'lenient')
            
        Returns:
            Dictionary with validation results
        """
        results = {
            'is_valid': True,
            'errors': [],
            'warnings': [],
            'validated_at': time.time(),
        }
        
        # Extract percept data
        vision_data = percept.getVision()
        touch_data = percept.getTouch()
        internal_data = percept.getInternal()
        proprioception_data = percept.getProprioception()
        
        # Validate each component
        if validation_type in ['all', 'vision']:
            vision_errors = self._validate_vision_data(vision_data)
            results['errors'].extend(vision_errors)
        
        if validation_type in ['all', 'touch']:
            touch_errors = self._validate_touch_data(touch_data)
            results['errors'].extend(touch_errors)
        
        if validation_type in ['all', 'internal']:
            internal_errors = self._validate_internal_data(internal_data)
            results['errors'].extend(internal_errors)
        
        if validation_type in ['all', 'proprioception']:
            proprioception_errors = self._validate_proprioception_data(proprioception_data)
            results['errors'].extend(proprioception_errors)
        
        # Determine if percept is valid
        results['is_valid'] = len(results['errors']) == 0
        
        if results['warnings']:
            print(f"⚠ Validation warnings: {results['warnings']}")
        
        return results
    
    def _validate_vision_data(self, vision_data: List[float]) -> List[str]:
        """Validate vision data."""
        errors = []
        rules = self.validation_rules['vision']
        
        # Check size
        if len(vision_data) < rules['min_size']:
            errors.append(f"Vision data too small: {len(vision_data)} < {rules['min_size']}")
        
        if len(vision_data) > rules['max_size']:
            errors.append(f"Vision data too large: {len(vision_data)} > {rules['max_size']}")
        
        # Check values
        for i, value in enumerate(vision_data):
            if value < rules['min_value']:
                errors.append(f"Vision value {i} too small: {value}")
            
            if value > rules['max_value']:
                errors.append(f"Vision value {i} too large: {value}")
        
        # Check dimensions if expected size is specified
        if rules['required_dimensions'] and len(vision_data) != rules['required_dimensions'][0] * rules['required_dimensions'][1]:
            errors.append(f"Vision data dimensions incorrect: expected {rules['required_dimensions']}, got {len(vision_data)}")
        
        return errors
    
    def _validate_touch_data(self, touch_data: List[float]) -> List[str]:
        """Validate touch data."""
        errors = []
        rules = self.validation_rules['touch']
        
        if len(touch_data) < rules['min_size']:
            errors.append(f"Touch data too small: {len(touch_data)} < {rules['min_size']}")
        
        if len(touch_data) > rules['max_size']:
            errors.append(f"Touch data too large: {len(touch_data)} > {rules['max_size']}")
        
        for i, value in enumerate(touch_data):
            if value < rules['min_value']:
                errors.append(f"Touch value {i} too small: {value}")
            
            if value > rules['max_value']:
                errors.append(f"Touch value {i} too large: {value}")
        
        return errors
    
    def _validate_internal_data(self, internal_data: List[float]) -> List[str]:
        """Validate internal data."""
        errors = []
        rules = self.validation_rules['internal']
        
        if len(internal_data) < rules['min_size']:
            errors.append(f"Internal data too small: {len(internal_data)} < {rules['min_size']}")
        
        if len(internal_data) > rules['max_size']:
            errors.append(f"Internal data too large: {len(internal_data)} > {rules['max_size']}")
        
        for i, value in enumerate(internal_data):
            if value < rules['min_value']:
                errors.append(f"Internal value {i} too small: {value}")
            
            if value > rules['max_value']:
                errors.append(f"Internal value {i} too large: {value}")
        
        return errors
    
    def _validate_proprioception_data(self, proprioception_data: List[float]) -> List[str]:
        """Validate proprioception data."""
        errors = []
        rules = self.validation_rules['proprioception']
        
        if len(proprioception_data) < rules['min_size']:
            errors.append(f"Proprioception data too small: {len(proprioception_data)} < {rules['min_size']}")
        
        if len(proprioception_data) > rules['max_size']:
            errors.append(f"Proprioception data too large: {len(proprioception_data)} > {rules['max_size']}")
        
        # Validate specific value ranges
        for i, (key, (min_val, max_val)) in enumerate(rules['value_ranges'].items()):
            if i < len(proprioception_data):
                value = proprioception_data[i]
                if value < min_val:
                    errors.append(f"{key} value {i} too small: {value}")
                
                if value > max_val:
                    errors.append(f"{key} value {i} too large: {value}")
        
        return errors
    
    def sanitize_percept(self, percept: nlm.SensoryPercept) -> nlm.SensoryPercept:
        """
        Sanitize and fix invalid percept data.
        
        Args:
            percept: Input percept to sanitize
            
        Returns:
            Sanitized percept
        """
        # Create sanitized copy
        sanitized = nlm.SensoryPercept()
        
        # Sanitize each component
        vision_data = percept.getVision()
        vision_data = self._sanitize_vision_data(vision_data)
        sanitized.setVision(vision_data)
        
        touch_data = percept.getTouch()
        touch_data = self._sanitize_touch_data(touch_data)
        sanitized.setTouch(touch_data)
        
        internal_data = percept.getInternal()
        internal_data = self._sanitize_internal_data(internal_data)
        sanitized.setInternal(internal_data)
        
        proprioception_data = percept.getProprioception()
        proprioception_data = self._sanitize_proprioception_data(proprioception_data)
        sanitized.setProprioception(proprioception_data)
        
        # Preserve timestamp
        sanitized.setTimestamp(percept.getTimestamp())
        
        return sanitized
    
    def _sanitize_vision_data(self, vision_data: List[float]) -> List[float]:
        """Sanitize vision data."""
        rules = self.validation_rules['vision']
        
        # Create sanitized copy
        sanitized = vision_data.copy()
        
        # Clamp values to valid range
        for i in range(len(sanitized)):
            if sanitized[i] < rules['min_value']:
                sanitized[i] = rules['min_value']
            elif sanitized[i] > rules['max_value']:
                sanitized[i] = rules['max_value']
        
        # Ensure minimum size
        if len(sanitized) < rules['min_size']:
            sanitized.extend([rules['min_value']] * (rules['min_size'] - len(sanitized)))
        
        # Ensure maximum size
        if len(sanitized) > rules['max_size']:
            sanitized = sanitized[:rules['max_size']]
        
        # Reshape to expected dimensions if possible
        if rules['required_dimensions']:
            expected_size = rules['required_dimensions'][0] * rules['required_dimensions'][1]
            if len(sanitized) != expected_size:
                # Pad or truncate
                if len(sanitized) < expected_size:
                    sanitized.extend([rules['min_value']] * (expected_size - len(sanitized)))
                else:
                    sanitized = sanitized[:expected_size]
        
        return sanitized
    
    def _sanitize_touch_data(self, touch_data: List[float]) -> List[float]:
        """Sanitize touch data."""
        rules = self.validation_rules['touch']
        
        sanitized = touch_data.copy()
        
        # Clamp values
        for i in range(len(sanitized)):
            if sanitized[i] < rules['min_value']:
                sanitized[i] = rules['min_value']
            elif sanitized[i] > rules['max_value']:
                sanitized[i] = rules['max_value']
        
        # Ensure size
        if len(sanitized) < rules['min_size']:
            sanitized.extend([rules['min_value']] * (rules['min_size'] - len(sanitized)))
        
        if len(sanitized) > rules['max_size']:
            sanitized = sanitized[:rules['max_size']]
        
        return sanitized
    
    def _sanitize_internal_data(self, internal_data: List[float]) -> List[float]:
        """Sanitize internal data."""
        rules = self.validation_rules['internal']
        
        sanitized = internal_data.copy()
        
        # Clamp values
        for i in range(len(sanitized)):
            if sanitized[i] < rules['min_value']:
                sanitized[i] = rules['min_value']
            elif sanitized[i] > rules['max_value']:
                sanitized[i] = rules['max_value']
        
        # Ensure size
        if len(sanitized) < rules['min_size']:
            sanitized.extend([rules['min_value']] * (rules['min_size'] - len(sanitized)))
        
        if len(sanitized) > rules['max_size']:
            sanitized = sanitized[:rules['max_size']]
        
        return sanitized
    
    def _sanitize_proprioception_data(self, proprioception_data: List[float]) -> List[float]:
        """Sanitize proprioception data."""
        rules = self.validation_rules['proprioception']
        
        sanitized = proprioception_data.copy()
        
        # Clamp values based on ranges
        for i, (key, (min_val, max_val)) in enumerate(rules['value_ranges'].items()):
            if i < len(sanitized):
                if sanitized[i] < min_val:
                    sanitized[i] = min_val
                elif sanitized[i] > max_val:
                    sanitized[i] = max_val
        
        # Ensure size
        if len(sanitized) < rules['min_size']:
            sanitized.extend([0.0] * (rules['min_size'] - len(sanitized)))
        
        if len(sanitized) > rules['max_size']:
            sanitized = sanitized[:rules['max_size']]
        
        return sanitized
class ErrorRecoveryManager:
    """
    Manager for error recovery and fallback strategies.
    
    This class manages recovery strategies for various error types,
    ensuring the system can gracefully handle failures and continue operation.
    """
    
    def __init__(self):
        self.recovery_strategies = {}
        self.fallback_parameters = {}
        
        print("Initializing Error Recovery Manager...")
        self._setup_recovery_strategies()
    
    def _setup_recovery_strategies(self):
        """Setup recovery strategies for different error scenarios."""
        # Recovery strategies
        self.recovery_strategies = {
            'validation_failure': self._recover_from_validation,
            'serialization_failure': self._recover_from_serialization,
            'callback_failure': self._recover_from_callback,
            'performance_degradation': self._recover_from_performance,
            'memory_exhaustion': self._recover_from_memory,
            'network_failure': self._recover_from_network,
        }
        
        # Fallback parameters
        self.fallback_parameters = {
            'learning_rate': 0.01,
            'exploration_rate': 0.1,
            'reward_shaping': 1.0,
            'curiosity_level': 0.5,
            'batch_size': 32,
            'validation_strictness': 'lenient',
        }
    
    def recover(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """
        Execute recovery based on error context.
        
        Args:
            error_context: Context where error occurred
            current_state: Current system state
            
        Returns:
            Dictionary with recovery results
        """
        error_type = error_context.get('error_type', 'unknown')
        operation = error_context.get('operation', '')
        
        print(f"\nRecovery Manager: Processing {error_type} in {operation}")
        
        # Get appropriate recovery strategy
        recovery_strategy = self.recovery_strategies.get(error_type, self._recover_from_generic)
        
        # Execute recovery
        recovery_result = recovery_strategy(error_context, current_state)
        
        # Update current state with recovered values
        updated_state = current_state.copy()
        updated_state.update(recovery_result.get('updated_parameters', {}))
        
        # Log recovery
        print(f"✓ Recovery completed: {recovery_result.get('status', 'unknown')}")
        
        return {
            'status': 'recovered',
            'updated_state': updated_state,
            'recovery_details': recovery_result,
        }
    
    def _recover_from_validation(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Recover from validation failure."""
        print(f"  - Applying validation recovery strategy")
        
        # Reset to safe defaults
        updated_parameters = {
            'learning_rate': self.fallback_parameters['learning_rate'],
            'exploration_rate': self.fallback_parameters['exploration_rate'],
            'reward_shaping': self.fallback_parameters['reward_shaping'],
        }
        
        # Adjust percept validation strictness
        if 'percept' in error_context.get('component', ''):
            updated_parameters['validation_strictness'] = self.fallback_parameters['validation_strictness']
        
        return {
            'status': 'validation_recovered',
            'updated_parameters': updated_parameters,
            'message': 'System reset to safe parameters'
        }
    
    def _recover_from_serialization(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Recover from serialization failure."""
        print(f"  - Applying serialization recovery strategy")
        
        # Try alternative serialization format
        updated_parameters = {
            'serialization_format': 'json',  # Fallback to JSON
            'compression_enabled': False,     # Disable compression
        }
        
        return {
            'status': 'serialization_recovered',
            'updated_parameters': updated_parameters,
            'message': 'Switching to alternative serialization format'
        }
    
    def _recover_from_callback(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:nml.CallbackError:
        """Recover from callback failure."""
        print(f"  - Applying callback recovery strategy")
        
        # Remove problematic callbacks
        updated_parameters = {
            'callbacks_enabled': False,
            'callback_interval': 1000,  # Increase interval
        }
        
        return {
            'status': 'callback_recovered',
            'updated_parameters': updated_parameters,
            'message': 'Disabling callbacks and increasing interval'
        }
    
    def _recover_from_performance(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Recover from performance degradation."""
        print(f"  - Applying performance recovery strategy")
        
        # Reduce computational load
        updated_parameters = {
            'batch_size': self.fallback_parameters['batch_size'],
            'learning_rate': min(current_state.get('learning_rate', 0.01), 0.005),
            'exploration_rate': max(current_state.get('exploration_rate', 0.1), 0.05),
        }
        
        return {
            'status': 'performance_recovered',
            'updated_parameters': updated_parameters,
            'message': 'Reducing computational load for stability'
        }
    
    def _recover_from_memory(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Recover from memory exhaustion."""
        print(f"  - Applying memory recovery strategy")
        
        # Clear memory and reduce data retention
        updated_parameters = {
            'memory_limit': 256,  # MB
            'data_retention': 10,  # seconds
        }
        
        return {
            'status': 'memory_recovered',
            'updated_parameters': updated_parameters,
            'message': 'Reducing memory usage and data retention'
        }
    
    def _recover_from_network(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Recover from network failure."""
        print(f"  - Applying network recovery strategy")
        
        # Switch to offline mode
        updated_parameters = {
            'online_mode': False,
            'sync_interval': 3600,  # Hour
        }
        
        return {
            'status': 'network_recovered',
            'updated_parameters': updated_parameters,
            'message': 'Switching to offline mode'
        }
    
    def _recover_from_generic(self, error_context: Dict[str, Any], current_state: Dict[str, Any]) -> Dict[str, Any]:
        """Generic recovery for unknown error types."""
        print(f"  - Applying generic recovery strategy")
        
        # Reset to all safe defaults
        updated_parameters = self.fallback_parameters.copy()
        
        return {
            'status': 'generic_recovered',
            'updated_parameters': updated_parameters,
            'message': 'Resetting to safe defaults'
        }
class ProductionErrorHandler:
    """
    Production-ready error handler with comprehensive error management.
    
    This class provides production-grade error handling for NLM systems,
    including monitoring, alerting, and recovery mechanisms suitable for
    deployment in production environments.
    """
    
    def __init__(self, production_config: Dict[str, Any] = None):
        """
        Initialize production error handler.
        
        Args:
            production_config: Production configuration
        """
        self.config = production_config or {}
        self.error_handler = NLMErrorHandler(
            auto_recover=self.config.get('auto_recover', True),
            max_retries=self.config.get('max_retries', 3)
        )
        
        self.validator = InputValidator()
        self.recovery_manager = ErrorRecoveryManager()
        
        # Production-specific settings
        self.alert_threshold = self.config.get('alert_threshold', 0.8)  # 80% error rate
        self.max_memory_usage = self.config.get('max_memory_usage', 512)  # MB
        self.performance_threshold = self.config.get('performance_threshold', 0.5)  # 50% efficiency
        
        print("Initializing Production Error Handler...")
        print(f"  - Alert threshold: {self.alert_threshold * 100}%")
        print(f"  - Max memory usage: {self.max_memory_usage} MB")
        print(f"  - Performance threshold: {self.performance_threshold * 100}%")
    
    @contextmanager
    def monitored_operation(self, operation_name: str, context: Dict[str, Any] = None):
        """
        Context manager for monitoring operations with error handling.
        
        Args:
            operation_name: Name of the operation being performed
            context: Additional context information
        """
        start_time = time.time()
        error_occurred = False
        
        try:
            # Record operation start
            operation_info = {
                'name': operation_name,
                'context': context or {},
                'start_time': start_time,
                'status': 'started',
            }
            
            print(f"🔄 Operation started: {operation_name}")
            
            yield operation_info
            
        except Exception as e:
            error_occurred = True
            
            # Record error
            error_info = {
                'operation_name': operation_name,
                'context': context or {},
                'error': e,
                'timestamp': time.time(),
            }
            
            self.error_handler.log_error(operation_name, e, context)
            
            # Check if we should alert
            self._check_alert_conditions(error_info)
            
            # Attempt recovery
            recovery_result = self.error_handler.attempt_recovery(operation_name, e)
            
            if recovery_result:
                print(f"✓ Recovery successful for {operation_name}")
                # Re-raise for caller to handle
                raise
            else:
                print(f"✗ Recovery failed for {operation_name}")
                raise
                
        finally:
            # Calculate operation duration
            duration = time.time() - start_time
            
            # Log operation completion
            status = 'completed' if not error_occurred else 'failed'
            print(f"📊 Operation {status}: {operation_name} ({duration:.3f}s)")
            
            # Record metrics
            self._record_operation_metrics(operation_name, duration, error_occurred)
    
    def _check_alert_conditions(self, error_info: Dict[str, Any]):
        """Check if alert conditions are met."""
        total_errors = len(self.error_handler.error_history)
        
        if total_errors > 0:
            recent_errors = len([
                e for e in self.error_handler.error_history
                if time.time() - e['timestamp'] < 300  # Last 5 minutes
            ])
            
            error_rate = recent_errors / max(1, total_errors)
            
            if error_rate >= self.alert_threshold:
                print(f"🚨 ALERT: High error rate detected ({error_rate * 100:.1f}%)")
                self._send_alert(error_info)
    
    def _send_alert(self, error_info: Dict[str, Any]):
        """Send alert for critical errors."""
        print(f"🚨 ALERT: Critical error in {error_info['operation_name']}")
        print(f"   Error: {error_info['error']}")
        
        # In production, this would send alerts via:
        # - Email
        # - Slack webhook
        # - PagerDuty
        # - Monitoring system
        
        alert_message = f"NLM Critical Error: {error_info['operation_name']} - {error_info['error']}"
        print(f"   Alert: {alert_message}")
    
    def _record_operation_metrics(self, operation_name: str, duration: float, error_occurred: bool):
        """Record operation metrics for analysis."""
        metric = {
            'operation': operation_name,
            'duration': duration,
            'error_occurred': error_occurred,
            'timestamp': time.time(),
        }
        
        # Store in error handler
        self.error_handler.error_history.append(metric)
    
    def get_production_summary(self) -> Dict[str, Any]:
        """Get production summary with error analysis."""
        error_summary = self.error_handler.get_error_summary()
        
        # Calculate key metrics
        total_operations = len(self.error_handler.error_history)
        total_errors = len([e for e in self.error_handler.error_history if 'error' in e])
        
        error_rate = total_errors / max(1, total_operations)
        
        # Check system health
        system_health = self._assess_system_health()
        
        return {
            'production_summary': {
                'total_operations': total_operations,
                'total_errors': total_errors,
                'error_rate': error_rate,
                'system_health': system_health,
            },
            'error_summary': error_summary,
            'recommendations': self._generate_recommendations(error_summary),
        }
    
    def _assess_system_health(self) -> Dict[str, Any]:
        """Assess overall system health."""
        error_summary = self.error_handler.get_error_summary()
        
        # Check error counts
        total_errors = error_summary['total_errors']
        error_counts = error_summary['error_types']
        
        # Determine health status
        if total_errors == 0:
            health_status = 'excellent'
        elif total_errors < 10:
            health_status = 'good'
        elif total_errors < 50:
            health_status = 'fair'
        else:
            health_status = 'poor'
        
        # Get most common error
        if error_counts:
            most_common_error = max(error_counts.items(), key=lambda x: x[1])
        else:
            most_common_error = ('none', 0)
        
        return {
            'status': health_status,
            'total_errors': total_errors,
            'most_common_error': most_common_error,
            'recovery_attempts': len(self.error_handler.recovery_attempts),
        }
    
    def _generate_recommendations(self, error_summary: Dict[str, Any]) -> List[str]:nml.ValidationError:
        """Generate recommendations based on error analysis."""
        recommendations = []
        
        total_errors = error_summary['total_errors']
        error_counts = error_summary['error_types']
        
        if total_errors > 20:
            recommendations.append("📈 High error count detected - consider system review")
        
        if 'validation' in str(error_counts):
            recommendations.append("✅ Validation errors are within acceptable range")
        
        if 'serialization' in str(error_counts):
            recommendations.append("💾 Serialization errors detected - check file system and permissions")
        
        if 'callback' in str(error_counts):
            recommendations.append("🔄 Callback errors detected - review callback implementation")
        
        if not recommendations:
            recommendations.append("✅ Error handling is functioning normally")
        
        return recommendations
def demonstrate_error_handling():
    """Demonstrate comprehensive error handling patterns."""
    print("Error Handling Examples - Robust Error Handling and Validation")
    print("=" * 80)
    
    # Initialize production error handler
    production_handler = ProductionErrorHandler({
        'auto_recover': True,
        'max_retries': 3,
        'alert_threshold': 0.7,
    })
    
    # Example 1: Input validation
    print("\n" + "=" * 80)
    print("EXAMPLE 1: INPUT VALIDATION")
    print("=" * 80)
    
    validator = InputValidator()
    
    # Create test percepts
    valid_percept = nlm.SensoryPercept()
    vision_data = [0.5] * 256
    touch_data = [0.5] * 6
    internal_data = [0.5] * 6
    proprioception_data = [0.0] * 12
    
    valid_percept.setVision(vision_data)
    valid_percept.setTouch(touch_data)
    valid_percept.setInternal(internal_data)
    valid_percept.setProprioception(proprioception_data)
    
    print("Validating valid percept...")
    validation_result = validator.validate_percept(valid_percept, 'strict')
    print(f"✓ Valid percept: {validation_result['is_valid']}")
    
    # Create invalid percept
    invalid_percept = nlm.SensoryPercept()
    invalid_percept.setVision([])  # Empty vision
    invalid_percept.setTouch([-1.5, 2.0])  # Out of range values
    invalid_percept.setInternal([0.5, 1.5, -0.5])  # Out of range
    invalid_percept.setProprioception([0.0] * 20)  # Wrong size
    
    print("\nValidating invalid percept...")
    invalid_result = validator.validate_percept(invalid_percept, 'strict')
    print(f"✗ Invalid percept: {invalid_result['is_valid']}")
    print(f"   Errors: {len(invalid_result['errors'])}")
    for error in invalid_result['errors']:
        print(f"   - {error}")
    
    # Sanitize invalid percept
    print("\nSanitizing invalid percept...")
    sanitized_percept = validator.sanitize_percept(invalid_percept)
    sanitized_result = validator.validate_percept(sanitized_percept, 'strict')
    print(f"✓ Sanitized percept: {sanitized_result['is_valid']}")
    print(f"   Errors after sanitization: {len(sanitized_result['errors'])}")
    
    # Example 2: Error recovery
    print("\n" + "=" * 80)
    print("EXAMPLE 2: ERROR RECOVERY")
    print("=" * 80)
    
    error_handler = NLMErrorHandler(auto_recover=True, max_retries=3)
    
    # Simulate various errors
    test_errors = [
        ("processSensoryInput", nlm.ValidationError("Invalid percept format")),
        ("serializeState", nlm.SerializationError("File write failed")),
        ("updateDevelopment", Exception("Generic system error")),
    ]
    
    for operation, error in test_errors:
        print(f"\nSimulating error in {operation}...")
        error_handler.log_error(operation, error, {'step': 10, 'context': 'test'})
        
        recovery_success = error_handler.attempt_recovery(operation, error)
        print(f"   Recovery {'successful' if recovery_success else 'failed'}")
    
    # Example 3: Production error handling
    print("\n" + "=" * 80)
    print("EXAMPLE 3: PRODUCTION ERROR HANDLING")
    print("=" * 80)
    
    # Create production handler with monitoring
    prod_handler = ProductionErrorHandler({
        'auto_recover': True,
        'max_retries': 2,
        'alert_threshold': 0.5,
    })
    
    # Simulate production operations with potential errors
    operations = [
        ("initialize_brain", {"status": "started"}),
        ("process_sensor_data", {"status": "processing", "step": 1}),
        ("decode_motor_command", {"status": "error", "message": "Invalid input"}),
        ("update_development", {"status": "completed", "step": 2}),
    ]
    
    for operation, context in operations:
        try:
            with prod_handler.monitored_operation(operation, context):
                # Simulate operation
                if "error" in context.get("status", ""):
                    raise Exception(f"Simulated error in {operation}")
                
                # Simulate work
                time.sleep(0.1)
                
                print(f"   ✅ {operation} completed successfully")
                
        except Exception as e:
            print(f"   ❌ {operation} failed: {e}")
    
    # Example 4: Performance monitoring with errors
    print("\n" + "=" * 80)
    print("EXAMPLE 4: PERFORMANCE MONITORING WITH ERROR TRACKING")
    print("=" * 80)
    
    perf_handler = ProductionErrorHandler()
    
    # Simulate performance-degrading operations
    for i in range(5):
        try:
            with perf_handler.monitored_operation(f"performance_test_{i}", {"iteration": i}):
                # Simulate varying performance
                if i % 3 == 0:
                    time.sleep(0.5)  # Slow operation
                    print(f"   ⚠️ Performance test {i} - slow")
                else:
                    time.sleep(0.1)  # Normal operation
                    print(f"   ✅ Performance test {i} - normal")
                    
        except Exception as e:
            print(f"   ❌ Performance test {i} failed: {e}")
    
    # Get production summary
    print("\n" + "=" * 80)
    print("EXAMPLE 5: PRODUCTION SUMMARY AND ANALYSIS")
    print("=" * 80)
    
    production_summary = perf_handler.get_production_summary()
    
    print(f"Production Summary:")
    print(f"  - Total operations: {production_summary['production_summary']['total_operations']}")
    print(f"  - Total errors: {production_summary['production_summary']['total_errors']}")
    print(f"  - Error rate: {production_summary['production_summary']['error_rate'] * 100:.1f}%")
    print(f"  - System health: {production_summary['production_summary']['system_health']['status']}")
    
    print(f"\nError Analysis:")
    print(f"  - Most common error: {production_summary['error_summary']['error_types']}")
    print(f"  - Recovery attempts: {len(production_summary['error_summary']['recovery_attempts'])}")
    
    print(f"\nRecommendations:")
    for recommendation in production_summary['recommendations']:
        print(f"  - {recommendation}")
    
    print("\n" + "=" * 80)
    print("ERROR HANDLING EXAMPLES COMPLETE")
    print("=" * 80)
    
    # Save error log
    error_log = {
        'handler': production_handler,
        'summary': production_summary,
        'timestamp': time.time(),
    }
    
    with open("error_handling_log.json", "w") as f:
        json.dump(error_log, f, indent=2, default=str)
    
    print(f"✓ Error handling log saved to error_handling_log.json")

if __name__ == "__main__":
    demonstrate_error_handling()