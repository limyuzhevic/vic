"""
Exception classes for NLM (Neural Learning Machine).

This module provides custom exception classes for NLM (Neural Learning Machine),
powering advanced artificial brain capabilities through comprehensive
error handling and exception management.

Key features:
- Custom exception classes for NLM
- Exception inheritance hierarchy
- Exception serialization support
- Exception handling utilities
- Integration with experiment framework

The Exception System is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through comprehensive
error handling and exception management.

Exception classes:
- NLMError: Base exception class
- NLMInitializationError: Initialization errors
- NLMInvalidStateError: Invalid state errors
- NLMSimulationError: Simulation errors
- NLMResourceError: Resource errors
- NLMConfigurationError: Configuration errors
- NLMSerializationError: Serialization errors
- NLMServerError: Server errors

Integration with:
- Component Registry (component initialization errors)
- Plugin System (plugin loading errors)
- Brain (neural computation errors)
- AgentBrain (agent operation errors)
- World (environment errors)
- Memory systems (memory operation errors)
- Prediction systems (prediction errors)
- Cognitive systems (cognitive operation errors)
- Neuromodulation systems (neuromodulation errors)
- Development systems (development errors)
- Performance infrastructure (performance errors)
- Visualization (visualization errors)
- Experiment Framework (experiment errors)

See src/experiments/README.md for detailed documentation.
"""

import time
from typing import Dict, Any, Optional, List
from enum import Enum


class NLMError(Exception):
    """Base exception class for NLM."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR", 
                 details: Optional[Dict[str, Any]] = None, 
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            error_code: Error code
            details: Exception details
            timestamp: Exception timestamp
        """
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = timestamp or time.time()
        
        # Log exception
        logging.error(f"NLM Error [{error_code}]: {message}")
    
    def __str__(self) -> str:
        """String representation."""
        return f"NLM Error [{self.error_code}]: {self.message}"
    
    def get_error_code(self) -> str:
        """Get error code."""
        return self.error_code
    
    def get_details(self) -> Dict[str, Any]:
        """Get exception details."""
        return self.details.copy()
    
    def get_timestamp(self) -> float:
        """Get exception timestamp."""
        return self.timestamp
    
    def add_detail(self, key: str, value: Any):
        """
        Add detail.
        
        Args:
            key: Detail key
            value: Detail value
        """
        self.details[key] = value
    
    def to_dict(self) -> Dict[str, Any]:
        """
        Convert exception to dictionary.
        
        Returns:
            Dictionary representation
        """
        return {
            'message': self.message,
            'error_code': self.error_code,
            'details': self.details,
            'timestamp': self.timestamp
        }


class NLMInitializationError(NLMError):
    """Exception for initialization errors."""
    
    def __init__(self, message: str, component: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            component: Component name
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_INIT_ERROR"
        if details is None:
            details = {}
        
        if component:
            details['component'] = component
        
        super().__init__(message, error_code, details, timestamp)


class NLMInvalidStateError(NLMError):
    """Exception for invalid state errors."""
    
    def __init__(self, message: str, component: Optional[str] = None,
                 expected_state: Optional[str] = None,
                 actual_state: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            component: Component name
            expected_state: Expected state
            actual_state: Actual state
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_INVALID_STATE_ERROR"
        if details is None:
            details = {}
        
        if component:
            details['component'] = component
        
        if expected_state:
            details['expected_state'] = expected_state
        
        if actual_state:
            details['actual_state'] = actual_state
        
        super().__init__(message, error_code, details, timestamp)


class NLMSimulationError(NLMError):
    """Exception for simulation errors."""
    
    def __init__(self, message: str, step: Optional[int] = None,
                 component: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            step: Simulation step
            component: Component name
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_SIMULATION_ERROR"
        if details is None:
            details = {}
        
        if step is not None:
            details['step'] = step
        
        if component:
            details['component'] = component
        
        super().__init__(message, error_code, details, timestamp)


class NLMResourceError(NLMError):
    """Exception for resource errors."""
    
    def __init__(self, message: str, resource_type: Optional[str] = None,
                 resource_name: Optional[str] = None,
                 available: Optional[float] = None,
                 required: Optional[float] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            resource_type: Resource type
            resource_name: Resource name
            available: Available resources
            required: Required resources
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_RESOURCE_ERROR"
        if details is None:
            details = {}
        
        if resource_type:
            details['resource_type'] = resource_type
        
        if resource_name:
            details['resource_name'] = resource_name
        
        if available is not None:
            details['available'] = available
        
        if required is not None:
            details['required'] = required
        
        super().__init__(message, error_code, details, timestamp)


class NLMConfigurationError(NLMError):
    """Exception for configuration errors."""
    
    def __init__(self, message: str, config_key: Optional[str] = None,
                 config_value: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            config_key: Configuration key
            config_value: Configuration value
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_CONFIG_ERROR"
        if details is None:
            details = {}
        
        if config_key:
            details['config_key'] = config_key
        
        if config_value:
            details['config_value'] = config_value
        
        super().__init__(message, error_code, details, timestamp)


class NLMSerializationError(NLMError):
    """Exception for serialization errors."""
    
    def __init__(self, message: str, data_type: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            data_type: Data type
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_SERIALIZATION_ERROR"
        if details is None:
            details = {}
        
        if data_type:
            details['data_type'] = data_type
        
        super().__init__(message, error_code, details, timestamp)


class NLMServerError(NLMError):
    """Exception for server errors."""
    
    def __init__(self, message: str, server_type: Optional[str] = None,
                 server_address: Optional[str] = None,
                 details: Optional[Dict[str, Any]] = None,
                 timestamp: Optional[float] = None):
        """
        Initialize exception.
        
        Args:
            message: Exception message
            server_type: Server type
            server_address: Server address
            details: Exception details
            timestamp: Exception timestamp
        """
        error_code = "NLM_SERVER_ERROR"
        if details is None:
            details = {}
        
        if server_type:
            details['server_type'] = server_type
        
        if server_address:
            details['server_address'] = server_address
        
        super().__init__(message, error_code, details, timestamp)