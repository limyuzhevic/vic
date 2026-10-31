"""
Exception classes for NLM Python bindings.
"""


class NLMError(Exception):
    """Base exception for all NLM errors."""
    
    def __init__(self, message: str, error_code: str = "NLM_ERROR"):
        """
        Initialize NLM error.
        
        Args:
            message: Error message
            error_code: Error code
        """
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.timestamp = time.time()
    
    def __str__(self) -> str:
        return f"[{self.error_code}] {self.message}"


class NLMConfigurationError(NLMError):
    """Error in configuration."""
    
    def __init__(self, message: str, config_key: str = None):
        """
        Initialize configuration error.
        
        Args:
            message: Error message
            config_key: Configuration key that caused the error
        """
        super().__init__(message, "NLM_CONFIG_ERROR")
        self.config_key = config_key


class NLMInitializationError(NLMError):
    """Error during initialization."""
    
    def __init__(self, message: str, component: str = None):
        """
        Initialize initialization error.
        
        Args:
            message: Error message
            component: Component that failed to initialize
        """
        super().__init__(message, "NLM_INIT_ERROR")
        self.component = component


class NLMExecutionError(NLMError):
    """Error during execution."""
    
    def __init__(self, message: str, step: int = None, component: str = None):
        """
        Initialize execution error.
        
        Args:
            message: Error message
            step: Step where error occurred
            component: Component where error occurred
        """
        super().__init__(message, "NLM_EXEC_ERROR")
        self.step = step
        self.component = component


class NLMBoundaryError(NLMError):
    """Error related to boundary conditions."""
    
    def __init__(self, message: str, boundary_type: str = None, position: Tuple[float, float] = None):
        """
        Initialize boundary error.
        
        Args:
            message: Error message
            boundary_type: Type of boundary
            position: Position that caused the error
        """
        super().__init__(message, "NLM_BOUNDARY_ERROR")
        self.boundary_type = boundary_type
        self.position = position


class NLMMemoryError(NLMError):
    """Error related to memory operations."""
    
    def __init__(self, message: str, memory_type: str = None, operation: str = None):
        """
        Initialize memory error.
        
        Args:
            message: Error message
            memory_type: Type of memory
            operation: Operation that failed
        """
        super().__init__(message, "NLM_MEMORY_ERROR")
        self.memory_type = memory_type
        self.operation = operation


class NLMValidationError(NLMError):
    """Error related to validation."""
    
    def __init__(self, message: str, validation_rule: str = None, value: Any = None):
        """
        Initialize validation error.
        
        Args:
            message: Error message
            validation_rule: Validation rule that failed
            value: Invalid value
        """
        super().__init__(message, "NLM_VALIDATION_ERROR")
        self.validation_rule = validation_rule
        self.value = value


class NLMImportError(NLMError):
    """Error related to importing data."""
    
    def __init__(self, message: str, source: str = None, file_path: str = None):
        """
        Initialize import error.
        
        Args:
            message: Error message
            source: Data source
            file_path: File path
        """
        super().__init__(message, "NLM_IMPORT_ERROR")
        self.source = source
        self.file_path = file_path


class NLMExportError(NLMError):
    """Error related to exporting data."""
    
    def __init__(self, message: str, destination: str = None, file_path: str = None):
        """
        Initialize export error.
        
        Args:
            message: Error message
            destination: Data destination
            file_path: File path
        """
        super().__init__(message, "NLM_EXPORT_ERROR")
        self.destination = destination
        self.file_path = file_path


# Import time for timestamping
import time
from typing import Tuple, Any


# Re-export exceptions from development module for convenience
from .development import DevelopmentalStage
